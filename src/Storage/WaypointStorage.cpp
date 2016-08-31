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
#include "WaypointStorage.h"
#include "DatabaseConnection.h"
#include "Log.h"

WaypointStorage::WaypointStorage()
{
    //
}

WaypointStorage::~WaypointStorage()
{
    //
}

void WaypointStorage::LoadFromDB()
{
    uint32_t pathId, count;

    sLog->Info(">> Loading waypoint data...");
    DBResult res = sMainDatabase.PQuery("SELECT path_id, point_id, flags, position_x, position_y, wait_delay FROM waypoints ORDER BY path_id ASC, point_id ASC");

    m_paths.clear();
    count = 0;
    while (res.FetchRow())
    {
        pathId = res.GetUInt32(0);
        if (m_paths.find(pathId) == m_paths.end())
            m_paths[pathId].clear();

        m_paths[pathId].push_back(WaypointRecord(
            pathId,
            res.GetUInt32(1),
            res.GetUInt32(2),
            res.GetFloat(3),
            res.GetFloat(4),
            res.GetUInt32(5)
        ));

        count++;
    }

    sLog->Info("Loaded %u points in %u paths", count, m_paths.size());
}

WaypointList* WaypointStorage::GetWaypointPath(uint32_t id)
{
    if (m_paths.find(id) == m_paths.end())
        return nullptr;

    return &m_paths[id];
}
