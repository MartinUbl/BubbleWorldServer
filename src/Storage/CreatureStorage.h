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

#ifndef BW_CREATURESTORAGE_H
#define BW_CREATURESTORAGE_H

#include "Singleton.h"

/*
 * Structure containing creature template database record contents
 */
struct CreatureTemplateRecord
{
    // creature ID
    uint32_t id;
    // creature name
    std::string name;
    // creature level
    uint32_t level;
    // image ID creature uses
    uint32_t imageId;
    // faction standing
    uint32_t faction;
    // base health
    uint32_t health;
    // name of script the creature uses
    std::string scriptName;
};

typedef std::unordered_map<uint32_t, CreatureTemplateRecord> CreatureTemplateMap;

/*
 * Structure containing creature database record (spawn)
 */
struct CreatureSpawnRecord
{
    // creature low GUID
    uint32_t guid;
    // creature ID (from creature_template)
    uint32_t id;
    // map ID
    uint32_t positionMap;
    // X position
    float positionX;
    // Y position
    float positionY;
};

typedef std::list<CreatureSpawnRecord> CreatureSpawnList;

/*
 * Singleton class maintaining database operations on creature template and creature records
 */
class CreatureStorage
{
    friend class Singleton<CreatureStorage>;
    public:
        ~CreatureStorage();

        // loads all necessary information from database
        void LoadFromDB();
        // retrieves creature template record
        CreatureTemplateRecord* GetCreatureTemplate(uint32_t id);
        // retrieves creature spawns for specified map
        void GetCreatureSpawnsForMap(uint32_t map, CreatureSpawnList& targetList);

    protected:
        // protected singleton constructor
        CreatureStorage();

    private:
        CreatureTemplateMap m_creatureTemplateMap;
};

#define sCreatureStorage Singleton<CreatureStorage>::getInstance()

#endif
