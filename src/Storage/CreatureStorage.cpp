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
#include "CreatureStorage.h"
#include "DatabaseConnection.h"
#include "Log.h"

CreatureStorage::CreatureStorage()
{
    //
}

CreatureStorage::~CreatureStorage()
{
    //
}

void CreatureStorage::LoadFromDB()
{
    uint32_t count, id;

    sLog->Info(">> Loading creature template records...");
    DBResult res = sMainDatabase.Query("SELECT id, name, level, image_id, faction, health, script_name FROM creature_template;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        m_creatureTemplateMap[id].id = id;
        m_creatureTemplateMap[id].name = res.GetString(1).c_str();
        m_creatureTemplateMap[id].level = res.GetUInt32(2);
        m_creatureTemplateMap[id].imageId = res.GetUInt32(3);
        m_creatureTemplateMap[id].faction = res.GetUInt32(4);
        m_creatureTemplateMap[id].health = res.GetUInt32(5);
        m_creatureTemplateMap[id].scriptName = res.GetString(6);
        count++;
    }
    sLog->Info("Loaded %u creature template records", count);
}

CreatureTemplateRecord* CreatureStorage::GetCreatureTemplate(uint32_t id)
{
    if (m_creatureTemplateMap.find(id) == m_creatureTemplateMap.end())
        return nullptr;

    return &m_creatureTemplateMap[id];
}

void CreatureStorage::GetCreatureSpawnsForMap(uint32_t map, CreatureSpawnList& targetList)
{
    DBResult res = sMainDatabase.PQuery("SELECT guid, id, position_map, position_x, position_y FROM creature WHERE position_map = %u;", map);

    targetList.clear();
    while (res.FetchRow())
    {
        CreatureSpawnRecord csp;
        csp.guid = res.GetUInt32(0);
        csp.id = res.GetUInt32(1);
        csp.positionMap = res.GetUInt32(2);
        csp.positionX = res.GetFloat(3);
        csp.positionY = res.GetFloat(4);

        targetList.push_back(csp);
    }
}
