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

#ifndef BW_RESOURCESTORAGE_H
#define BW_RESOURCESTORAGE_H

#include "Singleton.h"
#include "MapEnums.h"

/*
 * Structure containing resource base information
 */
struct ResourceRecord
{
    // resource ID
    uint32_t id;
    // resource type
    ResourceType type;
    // resource filename
    std::string filename;
    // checksum of file
    std::string checksum;
};

typedef std::map<uint32_t, ResourceRecord> ResourceMap;

/*
 * Structure containing metadata of animation
 */
struct ImageAnimationMetadata
{
    // image ID
    uint32_t imageId;
    // animation ID
    uint32_t animId;
    // first frame
    uint32_t frameBegin;
    // last frame
    uint32_t frameEnd;
    // milliseconds between frames
    uint32_t frameDelay;
};

typedef std::map<uint32_t, ImageAnimationMetadata> ImageAnimationMap;

/*
 * Structure containing image resource metadata
 */
struct ImageResourceMetadata
{
    // resource ID
    uint32_t id;
    // image (frame) width
    uint32_t sizeX;
    // image (frame) height
    uint32_t sizeY;
    // image centering point X coordinate
    uint32_t baseCenterX;
    // image centering point Y coordinate
    uint32_t baseCenterY;
    // upper-left corner of collision box (X coordinate)
    uint32_t collisionX1;
    // upper-left corner of collision box (Y coordinate)
    uint32_t collisionY1;
    // bottom-right corner of collision box (X coordinate)
    uint32_t collisionX2;
    // bottom-right corner of collision box (Y coordinate)
    uint32_t collisionY2;
    // metadata checksum
    std::string checksum;

    // loaded animations
    ImageAnimationMap animations;
    // collision box converted from pixels to game units (upper-left X coordinate)
    float unitCollisionX1;
    // collision box converted from pixels to game units (upper-left Y coordinate)
    float unitCollisionY1;
    // collision box converted from pixels to game units (bottom-right X coordinate)
    float unitCollisionX2;
    // collision box converted from pixels to game units (bottom-right Y coordinate)
    float unitCollisionY2;
    // converted base center X from pixels to game units
    float unitBaseX;
    // converted base center Y from pixels to game units
    float unitBaseY;

    // calculates collision box in game units, not in pixels
    void CalculateUnitCollisionBox()
    {
        unitCollisionX1 = (float)collisionX1 / MAP_FIELD_PX_SIZE_X;
        unitCollisionY1 = (float)collisionY1 / MAP_FIELD_PX_SIZE_Y;
        unitCollisionX2 = (float)collisionX2 / MAP_FIELD_PX_SIZE_X;
        unitCollisionY2 = (float)collisionY2 / MAP_FIELD_PX_SIZE_Y;
        unitBaseX = (float)baseCenterX / MAP_FIELD_PX_SIZE_X;
        unitBaseY = (float)baseCenterY / MAP_FIELD_PX_SIZE_Y;
    }
};

typedef std::map<uint32_t, ImageResourceMetadata> ImageMetadataMap;

/*
 * Singleton class maintaining all resource records, metadata and animations
 */
class ResourceStorage
{
    friend class Singleton<ResourceStorage>;
    public:
        ~ResourceStorage();

        // loads resource data from database
        void LoadFromDB();
        // verifies checksums of all resources and their metadata
        void VerifyChecksums();

        // retrieves generic resource record
        ResourceRecord* GetResource(ResourceType type, uint32_t id);
        // retrieves image metadata structure
        ImageResourceMetadata* GetImageMetadata(uint32_t id);

    protected:
        // protected singleton constructor
        ResourceStorage();

        // update checksum of resource in database
        void UpdateChecksumOf(ResourceType type, uint32_t id, const char* checksum);
        // update checksum of resource metadata in database
        void UpdateChecksumOfImageMetadata(uint32_t id, const char* checksum);

    private:
        // all loaded and available resources map
        ResourceMap m_resources[MAX_RSTYPE];
        // map of loaded image metadata
        ImageMetadataMap m_imageMetadata;
};

#define sResourceStorage Singleton<ResourceStorage>::getInstance()

#endif
