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

#ifndef BW_OBJECT_ACCESSOR_H
#define BW_OBJECT_ACCESSOR_H

#include "Singleton.h"
#include "GuidMap.h"

class WorldObject;

// type of guidmap
enum GUIDMapType
{
    GUIDMAP_CREATURE = 0,
    GUIDMAP_GAMEOBJECT = 1,
    GUIDMAP_PLAYER = 2,
    GUIDMAP_ITEM = 3,
    MAX_GUIDMAP = 4
};

/*
 * Class used for maintaining all existing objects in world
 */
class ObjectAccessor
{
    friend class Singleton<ObjectAccessor>;
    public:
        ~ObjectAccessor();

        // initializes GUID maps for further use
        void InitGUIDMaps();

        // adds object to evidence
        void AddObject(uint64_t guid, WorldObject* obj);
        // removes object from evidence
        void RemoveObject(uint64_t guid);
        // finds existing object
        WorldObject* FindWorldObject(uint64_t guid);

        // allocates and builds new creature GUID
        uint64_t AllocateCreatureGUID(uint32_t entry);
        // allocates and builds new gameobject GUID
        uint64_t AllocateGameobjectGUID(uint32_t entry);
        // allocates and builds new player GUID
        uint64_t AllocatePlayerGUID();
        // allocates and builds new item GUID
        uint32_t AllocateItemGUID();

    protected:
        // protected singleton constructor
        ObjectAccessor();

    private:
        // map of all objects
        std::unordered_map<uint64_t, WorldObject*> m_objectMap;
        // guidspace
        GuidMap m_guidMap[MAX_GUIDMAP];
};

#define sObjectAccessor Singleton<ObjectAccessor>::getInstance()

#endif
