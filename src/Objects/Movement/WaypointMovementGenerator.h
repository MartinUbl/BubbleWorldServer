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

#ifndef BW_WAYPOINTMOVEMENTGENERATOR_H
#define BW_WAYPOINTMOVEMENTGENERATOR_H

#include "MovementGeneratorBase.h"

class Unit;
class PointMovementGenerator;

// type of path repetition
enum WaypointPathRepeatType
{
    WPPATH_REPEAT_NONE = 0,             // the path won't be repeated - once it ends, the creature goes back to default movement
    WPPATH_REPEAT_FROM_START = 1,       // the path will be repeated from start (0..n, 0..n, ..) (last point has WPFLAG_REPEAT)
    WPPATH_REPEAT_BACKWARD = 2,         // the path will be repeated backwards (0..n, n..0) (last point has WPFLAG_REPEAT_REVERSE)
    WPPATH_REPEAT_CYCLIC = 3            // the path will be repeated forward and backwards repeatedly (0..n, n..0, 0..n, n..0, ..) (first and last point has WPFLAG_REPEAT_REVERSE)
};

/*
 * Structure containing prepared waypoint data for current movement
 */
struct WaypointPreparedRecord
{
    WaypointPreparedRecord() {};
    WaypointPreparedRecord(float _positionX, float _positionY, uint32_t _waitDelay, bool _relativeCurrent) : positionX(_positionX), positionY(_positionY), waitDelay(_waitDelay), relativeCurrent(_relativeCurrent) {};

    // X coordinate of waypoint
    float positionX;
    // Y coordinate of waypoint
    float positionY;
    // waiting delay upon reaching this waypoint
    uint32_t waitDelay;
    // should we add current position to base when starting this waypoint movement?
    bool relativeCurrent;
};

/*
 * Class representing motion generator for waypoint movement (technically multiple point movements)
 */
class WaypointMovementGenerator : public MovementGeneratorBase
{
    public:
        WaypointMovementGenerator(Unit* owner);
        virtual ~WaypointMovementGenerator();

        virtual void Initialize();
        virtual void Finalize();

        virtual void Update();
        virtual void ReceiveChildSignal(uint32_t signalId, uint32_t param);

        // sets waypoint path ID
        void StartWaypointPathId(uint32_t id);
        // retrieves waypoint path ID
        uint32_t GetWaypointPathId();

    protected:
        // move to waypoint path point
        void MoveToPoint(uint32_t pointId);
        // selects next waypoint ID
        bool SelectNextPoint(uint32_t &target);

    private:
        // currently processed path ID
        uint32_t m_pathId;
        // currently processed point ID (within path, index to m_waypoints)
        uint32_t m_pointId;
        // child motion generator for generating movement between waypoints
        PointMovementGenerator m_pointMovementGenerator;
        // stored waypoints
        std::vector<WaypointPreparedRecord> m_waypoints;
        // repeat type of path
        WaypointPathRepeatType m_repeatType;
        // are we going through the path in forward direction?
        bool m_forward;
        // flag set by signaling event to start movement to next point in next Update tick
        bool m_moveNextPoint;
        // time of reaching last point
        uint32_t m_lastPointTime;
};

#endif
