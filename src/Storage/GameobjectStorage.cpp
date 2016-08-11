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
#include "GameobjectStorage.h"
#include "DatabaseConnection.h"
#include "Log.h"

GameobjectStorage::GameobjectStorage()
{
    //
}

GameobjectStorage::~GameobjectStorage()
{
    //
}

void GameobjectStorage::LoadFromDB()
{
    uint32_t count, id;

    sLog->Info(">> Loading gameobject template records...");
    DBResult res = sMainDatabase.Query("SELECT id, name, image_id FROM gameobject_template;");
    count = 0;
    while (res.FetchRow())
    {
        id = res.GetUInt32(0);
        m_gameobjectTemplateMap[id].id = id;
        m_gameobjectTemplateMap[id].name = res.GetString(1).c_str();
        m_gameobjectTemplateMap[id].imageId = res.GetUInt32(2);
        count++;
    }
    sLog->Info("Loaded %u gameobject template records", count);
}

GameobjectTemplateRecord* GameobjectStorage::GetGameobjectTemplate(uint32_t id)
{
    if (m_gameobjectTemplateMap.find(id) == m_gameobjectTemplateMap.end())
        return nullptr;

    return &m_gameobjectTemplateMap[id];
}

void GameobjectStorage::GetGameobjectSpawnsForMap(uint32_t map, GameobjectSpawnList& targetList)
{
    DBResult res = sMainDatabase.PQuery("SELECT guid, id, position_map, position_x, position_y FROM gameobject WHERE position_map = %u;", map);

    targetList.clear();
    while (res.FetchRow())
    {
        GameobjectSpawnRecord gsp;
        gsp.guid = res.GetUInt32(0);
        gsp.id = res.GetUInt32(1);
        gsp.positionMap = res.GetUInt32(2);
        gsp.positionX = res.GetFloat(3);
        gsp.positionY = res.GetFloat(4);

        targetList.push_back(gsp);
    }
}
