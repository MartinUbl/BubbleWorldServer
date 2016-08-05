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
#include "MapManager.h"
#include "MapStorage.h"
#include "Log.h"

MapManager::MapManager()
{
    //
}

MapManager::~MapManager()
{
    //
}

Map* MapManager::GetMap(uint32_t mapId)
{
    // if exists, return the instance
    if (m_createdMaps.find(mapId) != m_createdMaps.end())
        return m_createdMaps[mapId];

    // retrieve map record
    MapRecord* mrec = sMapStorage->GetMapRecord(mapId);
    if (!mrec)
    {
        sLog->Error("Attempted to create map ID %u, that doesn't exist", mapId);
        return nullptr;
    }

    // create new map
    Map* map = new Map(mapId, mrec);
    // init contents
    map->InitContents();
    // store into manager's map
    m_createdMaps[mapId] = map;

    return map;
}

void MapManager::UpdateMaps()
{
    for (std::unordered_map<uint32_t, Map*>::iterator itr = m_createdMaps.begin(); itr != m_createdMaps.end(); ++itr)
        itr->second->Update();
}
