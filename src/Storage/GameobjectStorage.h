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

#ifndef BW_GAMEOBJECTSTORAGE_H
#define BW_GAMEOBJECTSTORAGE_H

#include "Singleton.h"

/*
 * Structure containing gameobject template database record contents
 */
struct GameobjectTemplateRecord
{
    // gameobject ID
    uint32_t id;
    // gameobject name
    std::string name;
    // image ID gameobject uses
    uint32_t imageId;
};

typedef std::unordered_map<uint32_t, GameobjectTemplateRecord> GameobjectTemplateMap;

/*
 * Structure containing gameobject database record (spawn)
 */
struct GameobjectSpawnRecord
{
    // gameobject low GUID
    uint32_t guid;
    // gameobject ID (from gameobject_template)
    uint32_t id;
    // map ID
    uint32_t positionMap;
    // X position
    float positionX;
    // Y position
    float positionY;
};

typedef std::list<GameobjectSpawnRecord> GameobjectSpawnList;

/*
 * Singleton class maintaining database operations on gameobject template and gameobject records
 */
class GameobjectStorage
{
    friend class Singleton<GameobjectStorage>;
    public:
        ~GameobjectStorage();

        // loads all necessary information from database
        void LoadFromDB();
        // retrieves gameobject template record
        GameobjectTemplateRecord* GetGameobjectTemplate(uint32_t id);
        // retrieves gameobject spawns for specified map
        void GetGameobjectSpawnsForMap(uint32_t map, GameobjectSpawnList& targetList);

    protected:
        // protected singleton constructor
        GameobjectStorage();

    private:
        GameobjectTemplateMap m_gameobjectTemplateMap;
};

#define sGameobjectStorage Singleton<GameobjectStorage>::getInstance()

#endif
