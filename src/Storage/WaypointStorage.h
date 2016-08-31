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

#ifndef BW_WAYPOINT_STORAGE_H
#define BW_WAYPOINT_STORAGE_H

#include "Singleton.h"

// flags for point within waypoint path
enum WaypointFlags
{
    WPFLAG_RELATIVE_SPAWN           = 0x0001,   // coordinates are relative to spawn position; could not be set with WPFLAG_RELATIVE_CURRENT
    WPFLAG_RELATIVE_CURRENT         = 0x0002,   // coordinates are relative to current position (may be from last point); could not be set with WPFLAG_RELATIVE_SPAWN
    WPFLAG_REPEAT                   = 0x0004,   // this is the ending point in waypoint path and the path should be repeated from first point
    WPFLAG_REPEAT_REVERSE           = 0x0008,   // this is the ending point (or beginning) in waypoint path and the path should be replayed backwards
};

/*
 * Structure containing waypoint data from database
 */
struct WaypointRecord
{
    // init constructor
    WaypointRecord(uint32_t _pathId, uint32_t _pointId, uint32_t _flags, float _positionX, float _positionY, uint32_t _waitDelay) :
        pathId(_pathId), pointId(_pointId), flags(_flags), positionX(_positionX), positionY(_positionY), waitDelay(_waitDelay)
    {
    }

    // path ID
    uint32_t pathId;
    // point ID within path
    uint32_t pointId;
    // point flags
    uint32_t flags;
    // X position of point
    float positionX;
    // Y position of point
    float positionY;
    // time in milliseconds to wait upon reaching this point
    uint32_t waitDelay;
};

typedef std::list<WaypointRecord> WaypointList;
typedef std::map<uint32_t, WaypointList> WaypointPathMap;

/*
 * Singleton class maintaining waypoint storage
 */
class WaypointStorage
{
    friend class Singleton<WaypointStorage>;
    public:
        ~WaypointStorage();

        // loads waypoint data from database
        void LoadFromDB();
        // retrieves waypoint path if exists
        WaypointList* GetWaypointPath(uint32_t id);

    protected:
        // protected singleton storage
        WaypointStorage();

    private:
        // loaded paths from database
        WaypointPathMap m_paths;
};

#define sWaypointStorage Singleton<WaypointStorage>::getInstance()

#endif
