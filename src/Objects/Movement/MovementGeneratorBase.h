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

#ifndef BW_MOVEMENTGENERATORBASE_H
#define BW_MOVEMENTGENERATORBASE_H

class Unit;
enum MotionType;
struct MotionPoint;

typedef std::vector<MotionPoint> MotionPointVector;

/*
 * Class representing movement generator - used as base for other generators
 */
class MovementGeneratorBase
{
    public:
        virtual ~MovementGeneratorBase();

        // retrieves motion generator type
        MotionType GetType();

        // called when the motion generator is created and applied on Unit
        virtual void Initialize();
        // called on every motion update tick
        virtual void Update();
        // called when the motion ended its duration/finished movement/etc.
        virtual void Finalize();
        // called when any point in movement was reached (i.e. when the direction has to change, ..)
        virtual void PointReached(uint32_t id);

    protected:
        // protected constructor; instantiate child classes only
        MovementGeneratorBase(Unit* owner, MotionType type);

        // sets next movement "waypoint" (uses also movement mask to be set to unit)
        void SetNextMovement(uint32_t id, float sourceX, float sourceY, float destX, float destY, uint8_t moveMask);
        // stops movement entirely
        void StopMovement();

        // owner of this movement generator
        Unit* m_owner;
        // type of movement generator
        MotionType m_type;
        // current movement target ID (identifies "instance of movement", used when calling PointReached)
        uint32_t m_movementPointId;
        // time of last movement point
        uint32_t m_lastPointTime;
        // we will reach the next point in this time (m_lastPointTime + m_nextPointDiffTime = time of arrival to next point)
        uint32_t m_nextPointDiffTime;

        // full path stored by derived movement generator
        MotionPointVector m_fullPath;
        // flags from pathfinder stored
        uint32_t m_pathfinderFlags;

    private:
        //
};

#endif
