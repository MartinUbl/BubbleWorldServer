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

#ifndef BW_POINTMOVEMENTGENERATOR_H
#define BW_POINTMOVEMENTGENERATOR_H

#include "MovementGeneratorBase.h"

class Unit;

/*
 * Class representing motion generator for static destination targetted movement
 */
class PointMovementGenerator : public MovementGeneratorBase
{
    public:
        PointMovementGenerator(Unit* owner);
        virtual ~PointMovementGenerator();

        virtual void Finalize();

        // sets point ID which should then be sent to parent and/or AI
        void SetPointId(uint32_t id);
        // retrieves target point ID
        uint32_t GetPointId();
        // sets target coordinates
        void SetTarget(float x, float y);

        virtual void PointReached(uint32_t id);

    protected:
        // original target we wanted to reach
        Position m_originalTarget;
        // point of ID we are heading towards (waypoint, not path segment)
        uint32_t m_pointId;

    private:
        //
};

#endif
