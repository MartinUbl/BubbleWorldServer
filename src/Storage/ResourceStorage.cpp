/**
 * Copyright (C) 2016 Martin Ubl <http://kennny.cz>
 *
 * This file is part of BubbleWorld MMORPG engine
 *
 * BubbleWorld is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BubbleWorld is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BubbleWorld. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "General.h"
#include "ResourceStorage.h"
#include "DatabaseConnection.h"
#include "CRC32.h"
#include "Log.h"

#include <sstream>
#include <iomanip>

const char* resourceTableNames[MAX_RSTYPE] = {
    nullptr,
    "resource_images"
};

ResourceStorage::ResourceStorage()
{
    //
}

ResourceStorage::~ResourceStorage()
{
    //
}

void ResourceStorage::LoadFromDB()
{
    uint32_t id, animId;
    uint32_t count;

    // load image resource records
    sLog->Info(">> Loading image resources...");
    DBResult res = sMainDatabase.Query("SELECT id, filename, checksum FROM resource_images;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        m_resources[RSTYPE_IMAGE][id].id = id;
        m_resources[RSTYPE_IMAGE][id].type = RSTYPE_IMAGE;
        m_resources[RSTYPE_IMAGE][id].filename = res.GetString(1).c_str();
        m_resources[RSTYPE_IMAGE][id].checksum = res.GetString(2).c_str();
        count++;
    }
    sLog->Info("Loaded %u image resources", count);
    sLog->Info("");

    // load image metadata records
    sLog->Info(">> Loading image resource metadata...");
    res = sMainDatabase.Query("SELECT id, size_x, size_y, base_center_x, base_center_y, checksum FROM resource_images_metadata;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        m_imageMetadata[id].id = id;
        m_imageMetadata[id].sizeX = res.GetUInt32(1);
        m_imageMetadata[id].sizeY = res.GetUInt32(2);
        m_imageMetadata[id].baseCenterX = res.GetUInt32(3);
        m_imageMetadata[id].baseCenterY = res.GetUInt32(4);
        m_imageMetadata[id].checksum = res.GetString(5).c_str();
        count++;
    }
    sLog->Info("Loaded %u image resources metadata", count);
    sLog->Info("");

    // load animation records
    sLog->Info(">> Loading image resource animations...");
    res = sMainDatabase.Query("SELECT id, animation_id, frame_begin, frame_end, frame_delay FROM resource_images_anims;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        animId = res.GetUInt32(1);
        m_imageMetadata[id].animations[animId].imageId = id;
        m_imageMetadata[id].animations[animId].animId = animId;
        m_imageMetadata[id].animations[animId].frameBegin = res.GetUInt32(2);
        m_imageMetadata[id].animations[animId].frameEnd = res.GetUInt32(3);
        m_imageMetadata[id].animations[animId].frameDelay = res.GetUInt32(4);
        count++;
    }
    sLog->Info("Loaded %u image resource animations", count);

    // verify, that every image has metadata record
    for (std::pair<uint32_t, ResourceRecord> rc : m_resources[RSTYPE_IMAGE])
    {
        if (m_imageMetadata.find(rc.first) == m_imageMetadata.end())
            sLog->Error("ERROR: Image ID %u does not have metadata set!", rc.first);
    }

    // TODO: more resources
}

void ResourceStorage::VerifyChecksums()
{
    ResourceRecord* rec;
    ImageResourceMetadata* meta;
    ImageAnimationMetadata* animmeta;
    FILE* f;
    uint32_t crc;
    std::string checksum;

    // calculate and verify checksums of all resource files
    for (ResourceMap::iterator itr = m_resources[RSTYPE_IMAGE].begin(); itr != m_resources[RSTYPE_IMAGE].end(); ++itr)
    {
        rec = &itr->second;

        f = fopen((std::string(DATA_DIR) + rec->filename).c_str(), "rb");
        if (f)
        {
            checksum = GetCRC32String(CRC32_File(f));

            // update if needed
            if (checksum != rec->checksum)
            {
                sLog->Info("Updating checksum of %s to %s", rec->filename.c_str(), checksum.c_str());
                UpdateChecksumOf(RSTYPE_IMAGE, rec->id, checksum.c_str());
            }

            fclose(f);
        }
        else
        {
            sLog->Error("Could not open resource %s", rec->filename.c_str());
        }
    }

    // calculate and verify all metadata checksums
    for (ImageMetadataMap::iterator itr = m_imageMetadata.begin(); itr != m_imageMetadata.end(); ++itr)
    {
        meta = &itr->second;

        // perform checksum on every field standalone, not whole structure
        crc = 0;
        crc = CRC32_Bytes_Continuous((uint8_t*)&meta->id, sizeof(uint32_t), crc);
        crc = CRC32_Bytes_Continuous((uint8_t*)&meta->sizeX, sizeof(uint32_t), crc);
        crc = CRC32_Bytes_Continuous((uint8_t*)&meta->sizeY, sizeof(uint32_t), crc);
        crc = CRC32_Bytes_Continuous((uint8_t*)&meta->baseCenterX, sizeof(uint32_t), crc);
        crc = CRC32_Bytes_Continuous((uint8_t*)&meta->baseCenterY, sizeof(uint32_t), crc);

        // also perform checksum calculation on animation data
        for (ImageAnimationMap::iterator aitr = meta->animations.begin(); aitr != meta->animations.end(); ++aitr)
        {
            animmeta = &aitr->second;

            crc = CRC32_Bytes_Continuous((uint8_t*)&animmeta->animId, sizeof(uint32_t), crc);
            crc = CRC32_Bytes_Continuous((uint8_t*)&animmeta->frameBegin, sizeof(uint32_t), crc);
            crc = CRC32_Bytes_Continuous((uint8_t*)&animmeta->frameEnd, sizeof(uint32_t), crc);
            crc = CRC32_Bytes_Continuous((uint8_t*)&animmeta->frameDelay, sizeof(uint32_t), crc);
        }

        // finalize CRC32 calculation
        crc = CRC32_Bytes_ContinuousFinalize(crc);

        checksum = GetCRC32String(crc);
        // update checksum if needed
        if (checksum != meta->checksum)
        {
            sLog->Info("Updating checksum of %u image metadata to %s", meta->id, checksum.c_str());
            UpdateChecksumOfImageMetadata(meta->id, checksum.c_str());
        }
    }
}

void ResourceStorage::UpdateChecksumOf(ResourceType type, uint32_t id, const char* checksum)
{
    m_resources[type][id].checksum = checksum;

    if (resourceTableNames[type] != nullptr)
        sMainDatabase.PExecute("UPDATE %s SET checksum = '%s' WHERE id = %u", resourceTableNames[type], checksum, id);
}

void ResourceStorage::UpdateChecksumOfImageMetadata(uint32_t id, const char* checksum)
{
    m_imageMetadata[id].checksum = checksum;

    sMainDatabase.PExecute("UPDATE resource_images_metadata SET checksum = '%s' WHERE id = %u", checksum, id);
}

ResourceRecord* ResourceStorage::GetResource(ResourceType type, uint32_t id)
{
    if (m_resources[type].find(id) == m_resources[type].end())
        return nullptr;

    return &m_resources[type][id];
}

ImageResourceMetadata* ResourceStorage::GetImageMetadata(uint32_t id)
{
    if (m_imageMetadata.find(id) == m_imageMetadata.end())
        return nullptr;

    return &m_imageMetadata[id];
}
