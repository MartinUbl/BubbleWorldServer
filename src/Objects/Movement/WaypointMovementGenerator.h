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

/*
 * Class representing motion generator for waypoint movement (technically multiple point movements)
 */
class WaypointMovementGenerator : public MovementGeneratorBase
{
    public:
        WaypointMovementGenerator(Unit* owner);
        virtual ~WaypointMovementGenerator();

        // Not yet implemented

    protected:
        //

    private:
        //
};

#endif
