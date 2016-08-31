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

#ifndef BW_MOTIONMASTER_H
#define BW_MOTIONMASTER_H

#include "MovementGeneratorBase.h"

// known motion types
enum MotionType
{
    MOVEMENT_IDLE = 0,
    MOVEMENT_WAYPOINT = 1,
    MOVEMENT_POINT = 2,
    MAX_MOTION_TYPE
};

class Unit;

/*
 * Class maintaining movement - generator change, adapting to new conditions, etc.
 */
class MotionMaster
{
    public:
        MotionMaster(Unit* owner);

        // initialize motion master - set idle type, etc.
        void Initialize();
        // update movement
        void Update();

        // retrieves current generator type
        MotionType GetCurrentMotionType();

        // sets movement generator to "idle"
        void MoveIdle();
        // starts point movement (with pathfinding to avoid obstacles)
        void MovePoint(float x, float y);
        // start waypoint movement
        void MoveWaypointPath(uint32_t pathId);

    protected:
        // internal template class for creating and initializing a new instance of motion generator
        template <class T>
        T* ChangeTo();

        std::function<void()> m_movementGeneratorChangeLambda;

    private:
        // pointer to motion master owner
        Unit* m_owner;
        // current motion generator
        MovementGeneratorBase* m_movementGenerator;
};

#endif
