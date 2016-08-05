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
#include "MapStorage.h"
#include "DatabaseConnection.h"
#include "CRC32.h"
#include "Log.h"

#include <sstream>
#include <iomanip>

MapStorage::MapStorage()
{
    //
}

MapStorage::~MapStorage()
{
    //
}

void MapStorage::LoadFromDB()
{
    uint32_t id, sx, sy, cx, cy;
    uint32_t count;

    // load map records
    sLog->Info(">> Loading map records...");
    DBResult res = sMainDatabase.Query("SELECT id, filename, header_checksum FROM map;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        m_maps[id].id = id;
        m_maps[id].filename = res.GetString(1).c_str();
        m_maps[id].headerChecksum = res.GetString(2).c_str();
        count++;
    }
    sLog->Info("Loaded %u map records", count);
    sLog->Info("");

    // load map chunk records
    sLog->Info(">> Loading map chunk cache...");
    DBResult res2 = sMainDatabase.Query("SELECT map_id, start_x, start_y, size_x, size_y, checksum FROM map_chunk;");
    count = 0;
    while (res2.FetchRow())
    {
        id = res2.GetUInt32(0);
        sx = res2.GetUInt32(1);
        sy = res2.GetUInt32(2);

        cx = GetChunkIndexX(sx);
        cy = GetChunkIndexY(sy);

        m_maps[id].chunks[cx][cy].mapId = id;
        m_maps[id].chunks[cx][cy].startX = sx;
        m_maps[id].chunks[cx][cy].startY = sy;
        m_maps[id].chunks[cx][cy].sizeX = res2.GetUInt32(3);
        m_maps[id].chunks[cx][cy].sizeY = res2.GetUInt32(4);

        m_maps[id].chunks[cx][cy].checksum = res2.GetString(5).c_str();
        count++;
    }
    sLog->Info("Loaded %u cached map chunks", count);
    sLog->Info("");

    // load map contents
    sLog->Info(">> Loading maps...");
    count = 0;
    for (MapMap::iterator itr = m_maps.begin(); itr != m_maps.end(); ++itr)
    {
        LoadMap(itr->first);
        count++;
    }
    sLog->Info("Loaded %u maps", count);
    sLog->Info("");
}

void MapStorage::LoadMap(uint32_t id)
{
    // map must exist
    MapRecord* rec = GetMapRecord(id);
    if (!rec)
    {
        sLog->Error("Invalid map ID %u", id);
        return;
    }

    std::string path = std::string(DATA_DIR) + rec->filename;

    // open map file for reading
    FILE* f = fopen(path.c_str(), "rb");
    if (!f)
    {
        sLog->Error("Cannot open map file %s for reading!", path.c_str());
        return;
    }

    // clear header and load it from file
    memset(&rec->header, 0, sizeof(MapHeader));
    fread(&rec->header, sizeof(MapHeader), 1, f);

    // verify map file magic
    if (rec->header.mapVersionMagic != MAP_VERSION_MAGIC)
    {
        sLog->Error("Unsupported map format file %s (ID: %u)", path.c_str(), id);
        fclose(f);
        return;
    }

    // load chunks
    uint32_t cx, cy, ix, iy;
    MapChunkRecord* mch;
    for (uint32_t x = 0; x < rec->header.sizeX; x++)
    {
        for (uint32_t y = 0; y < rec->header.sizeY; y++)
        {
            // get chunk indexes
            cx = GetChunkIndexX(x);
            cy = GetChunkIndexY(y);

            // this will create the record before actual usage
            if (rec->chunks.find(cx) == rec->chunks.end())
                rec->chunks[cx].clear();
            if (rec->chunks[cx].find(cy) == rec->chunks[cx].end())
            {
                rec->chunks[cx][cy].mapId = id;
                rec->chunks[cx][cy].startX = cx * MAP_CHUNK_SIZE_X;
                rec->chunks[cx][cy].startY = cy * MAP_CHUNK_SIZE_Y;
                rec->chunks[cx][cy].sizeX = MAP_CHUNK_SIZE_X;
                if (rec->chunks[cx][cy].sizeX + rec->chunks[cx][cy].startX > rec->header.sizeX)
                    rec->chunks[cx][cy].sizeX = rec->chunks[cx][cy].sizeX + rec->chunks[cx][cy].startX - rec->header.sizeX;
                rec->chunks[cx][cy].sizeY = MAP_CHUNK_SIZE_Y;
                if (rec->chunks[cx][cy].sizeY + rec->chunks[cx][cy].startY > rec->header.sizeY)
                    rec->chunks[cx][cy].sizeY = rec->chunks[cx][cy].sizeY + rec->chunks[cx][cy].startY - rec->header.sizeY;
                rec->chunks[cx][cy].checksum = "";
            }

            mch = &rec->chunks[cx][cy];
            ix = mch->GetFieldOffsetX(x);
            iy = mch->GetFieldOffsetY(y);

            // read field data
            memset(&mch->fields[ix][iy], 0, sizeof(MapField));
            fread(&mch->fields[ix][iy], sizeof(MapField), 1, f);
        }
    }
}

void MapStorage::VerifyChecksums()
{
    uint32_t crc;

    // check for every map changes
    for (MapMap::iterator itr = m_maps.begin(); itr != m_maps.end(); ++itr)
    {
        // calculate checksum of map header
        crc = CRC32_Bytes((uint8_t*)&itr->second.header, sizeof(MapHeader));

        std::string checksum = GetCRC32String(crc);
        // verify validity, and in case of need, update it
        if (checksum != itr->second.headerChecksum)
        {
            sLog->Info("Updating checksum of %s to %s", itr->second.filename.c_str(), checksum.c_str());
            UpdateChecksumOfMap(itr->second.id, checksum.c_str());
        }

        // calculate checksum of all chunks
        for (uint32_t cx = 0; cx < itr->second.chunks.size(); cx++)
        {
            for (uint32_t cy = 0; cy < itr->second.chunks[cx].size(); cy++)
            {
                crc = 0;

                MapChunkRecord* mch = &itr->second.chunks[cx][cy];

                // calculate checksum using fields within chunk
                for (uint32_t ix = 0; ix < mch->sizeX; ix++)
                    for (uint32_t iy = 0; iy < mch->sizeY; iy++)
                        crc = CRC32_Bytes_Continuous((uint8_t*)&mch->fields[ix][iy], sizeof(MapField), crc);

                crc = CRC32_Bytes_ContinuousFinalize(crc);

                checksum = GetCRC32String(crc);

                // update if needed
                if (checksum != mch->checksum)
                {
                    sLog->Info("Updating checksum of map (ID: %u) chunk [%u : %u] to %s", itr->first, cx, cy, checksum.c_str());
                    UpdateChecksumOfMapChunk(itr->second.id, cx, cy, checksum.c_str());
                }
            }
        }
    }
}

MapRecord* MapStorage::GetMapRecord(uint32_t id)
{
    if (m_maps.find(id) == m_maps.end())
        return nullptr;

    return &m_maps[id];
}

void MapStorage::UpdateChecksumOfMap(uint32_t id, const char* checksum)
{
    m_maps[id].headerChecksum = checksum;

    sMainDatabase.PExecute("UPDATE map SET header_checksum = '%s' WHERE id = %u", checksum, id);
}

void MapStorage::UpdateChecksumOfMapChunk(uint32_t map_id, uint32_t ix, uint32_t iy, const char* checksum)
{
    // not yet cached
    if (m_maps[map_id].chunks[ix][iy].checksum == "")
    {
        sMainDatabase.PExecute("INSERT INTO map_chunk (map_id, start_x, start_y, size_x, size_y, checksum) \
            VALUES (%u, %u, %u, %u, %u, '%s')", map_id, GetChunkStartX(ix), GetChunkStartY(iy), m_maps[map_id].chunks[ix][iy].sizeX, m_maps[map_id].chunks[ix][iy].sizeY, checksum);
    }
    else
    {
        sMainDatabase.PExecute("UPDATE map_chunk SET checksum = '%s' WHERE map_id = %u AND start_x = %u AND start_y = %u", checksum, map_id, GetChunkStartX(ix), GetChunkStartY(iy));
    }

    m_maps[map_id].chunks[ix][iy].checksum = checksum;
}

uint32_t MapStorage::GetChunkIndexX(uint32_t startX)
{
    return startX / MAP_CHUNK_SIZE_X;
}

uint32_t MapStorage::GetChunkIndexY(uint32_t startY)
{
    return startY / MAP_CHUNK_SIZE_Y;
}

uint32_t MapStorage::GetChunkStartX(uint32_t iX)
{
    return iX * MAP_CHUNK_SIZE_X;
}

uint32_t MapStorage::GetChunkStartY(uint32_t iY)
{
    return iY * MAP_CHUNK_SIZE_Y;
}
