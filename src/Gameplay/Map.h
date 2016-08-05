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

#ifndef BW_MAP_H
#define BW_MAP_H

#include "MapEnums.h"

struct MapRecord;
class WorldObject;
class SmartPacket;
class Player;

typedef std::list<WorldObject*> WorldObjectList;
typedef std::vector<WorldObjectList> WorldObjectMapRow;
typedef std::vector<WorldObjectMapRow> WorldObjectMap;

/*
 * Class maintaining objects on one map
 */
class Map
{
    public:
        // constructor retaining map ID and pointer to storage record
        Map(uint32_t id, MapRecord* mapTemplate = nullptr);
        virtual ~Map();

        // initialize internal structures
        void InitContents();

        // adds object to map
        void AddToMap(WorldObject* obj);
        // removes object from map
        void RemoveFromMap(WorldObject* obj);
        // relocates object within map
        void Relocate(WorldObject* obj, float oldX, float oldY, float newX, float newY);

        // update objects on map
        virtual void Update();

        // retrieves map ID
        uint32_t GetMapID();

        // retrieves cell X index using X coordinate within cell
        uint32_t GetCellIndexX(float fieldX);
        // retrieves cell Y index using Y coordinate within cell
        uint32_t GetCellIndexY(float fieldY);
        // retrieves cell starting X coordinate using its X index
        uint32_t GetCellStartX(uint32_t indexX);
        // retrieves cell starting Y coordinate using its Y index
        uint32_t GetCellStartY(uint32_t indexY);

        // retrieves cell sorroundings, considering map size and boundaries
        void GetCellSorroundingLimits(uint32_t cellX, uint32_t cellY, uint32_t &beginX, uint32_t &beginY, uint32_t &endX, uint32_t &endY);

        // sends packet to single cell
        void SendPacketToCell(uint32_t cellX, uint32_t cellY, SmartPacket &pkt);
        // sends packet to cell sorroundings
        void SendPacketToSorroundings(uint32_t cellX, uint32_t cellY, SmartPacket &pkt);
        // sends packet to position sorroundings
        void SendPacketToSorroundings(float x, float y, SmartPacket &pkt);

        // sends create packets for every object in sorrounding to player
        void SendCreateSorroundings(Player* plr);
        // when object enters cell, create him for objects there, and create objects for him
        void SendCellCreatePacketsFor(uint32_t cellX, uint32_t cellY, WorldObject* wobj);
        // when object leaves cell, delete him for objects there, and delete objects for him
        void SendCellDeletePacketsFor(uint32_t cellX, uint32_t cellY, WorldObject* wobj);

    protected:
        //

    private:
        // map ID
        uint32_t m_mapId;
        // stored pointer to map record
        MapRecord* m_storedMapRecord;
        // objects in map; key1 = cellX, key2 = cellY
        WorldObjectMap m_objects;
};

#endif
