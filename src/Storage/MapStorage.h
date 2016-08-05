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

#ifndef BW_MAPSTORAGE_H
#define BW_MAPSTORAGE_H

#include "Singleton.h"

#include "MapEnums.h"

// force alignment to 4 bytes
#if defined(__GNUC__)
#pragma pack(4)
#else
#pragma pack(push,4)
#endif

/*
 * Map header structure
 */
struct MapHeader
{
    uint32_t mapVersionMagic;               // magic identifier
    uint32_t mapId;                         // ID of map
    char name[MAP_NAME_LENGTH];             // short name identifier, unused characters filled with zeroes
    uint32_t sizeX;                         // number of fields in X direction
    uint32_t sizeY;                         // number of fields in Y direction
    uint32_t entryX;                        // map entry point X coordinate
    uint32_t entryY;                        // map entry point Y coordinate
    uint16_t defaultFieldType;              // default field type
    uint32_t defaultFieldTexture;           // default field texture
    uint32_t defaultFieldFlags;             // default field flags
};

/*
 * Map field structure
 */
struct MapField
{
    uint16_t type;                          // field type
    uint32_t texture;                       // field texture
    uint32_t flags;                         // field flags
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

typedef std::map<uint32_t, MapField> MapFieldRow;
typedef std::map<uint32_t, MapFieldRow> MapFieldMap;

/*
 * Structure for one map chunk
 */
struct MapChunkRecord
{
    // map ID
    uint32_t mapId;
    // starting X coordinate
    uint32_t startX;
    // starting Y coordinate
    uint32_t startY;
    // count of fields in X direction
    uint32_t sizeX;
    // count of fields in Y direction
    uint32_t sizeY;
    // chunk checksum
    std::string checksum;

    // runtime generated/loaded data

    // stored fields
    MapFieldMap fields;

    // retrieves field X offset within chunk using absolute X coordinate
    uint32_t GetFieldOffsetX(uint32_t absX)
    {
        return absX - startX;
    }
    // retrieves field Y offset within chunk using absolute Y coordinate
    uint32_t GetFieldOffsetY(uint32_t absY)
    {
        return absY - startY;
    }
};

typedef std::map<uint32_t, MapChunkRecord> MapChunkRow;
typedef std::map<uint32_t, MapChunkRow> MapChunkMap;

/*
 * Structure containing information about map database record
 */
struct MapRecord
{
    // map ID
    uint32_t id;
    // map filename
    std::string filename;
    // header checksum
    std::string headerChecksum;

    // runtime generated/loaded data

    // map header loaded from file
    MapHeader header;
    // chunk map
    MapChunkMap chunks;
};

typedef std::map<uint32_t, MapRecord> MapMap;

/*
 * Singleton class maintaining map database records and map file loading
 */
class MapStorage
{
    friend class Singleton<MapStorage>;
    public:
        ~MapStorage();

        // load map records from database
        void LoadFromDB();
        // verify checksums of maps and their chunks
        void VerifyChecksums();

        // retrieves map record
        MapRecord* GetMapRecord(uint32_t id);

        // retrieves chunk X index using starting coordinate
        static uint32_t GetChunkIndexX(uint32_t startX);
        // retrieves chunk Y index using starting coordinate
        static uint32_t GetChunkIndexY(uint32_t startY);
        // retrieves chunk starting X coordinate using index
        static uint32_t GetChunkStartX(uint32_t iX);
        // retrieves chunk starting Y coordinate using index
        static uint32_t GetChunkStartY(uint32_t iY);

    protected:
        // protected singleton constructor
        MapStorage();

        // updates checksum of map header in database
        void UpdateChecksumOfMap(uint32_t id, const char* checksum);
        // updates checksum of map chunk contents in database
        void UpdateChecksumOfMapChunk(uint32_t map_id, uint32_t start_x, uint32_t start_y, const char* checksum);

        // loads map from file
        void LoadMap(uint32_t id);

    private:
        // all loaded maps
        MapMap m_maps;
};

#define sMapStorage Singleton<MapStorage>::getInstance()

#endif
