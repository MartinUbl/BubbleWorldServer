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

#ifndef BW_FOLLOWMOVEMENTGENERATOR_H
#define BW_FOLLOWMOVEMENTGENERATOR_H

#include "MovementGeneratorBase.h"

class Unit;
class PointMovementGenerator;

// time in milliseconds between follower distance checks
#define FOLLOW_DISTANCE_CHECK_DELAY 500

/*
 * Class representing motion generator for follow movement (technically multiple point movements triggered to follow target)
 */
class FollowMovementGenerator : public MovementGeneratorBase
{
    public:
        FollowMovementGenerator(Unit* owner);
        virtual ~FollowMovementGenerator();

        virtual void Initialize();
        virtual void Finalize();

        virtual void Update();
        virtual void ReceiveChildSignal(uint32_t signalId, uint32_t param);

        // starts random movement with supplied parameters
        void StartFollowing(Unit* target, float followDistance, float maxDistance);

    protected:
        // checks the follower distance; return true if close enough, false if too far away
        bool CheckFollowerDistance();
        // starts movement towards new random point
        void MoveToClosePoint();

    private:
        // GUID of unit we follow
        uint64_t m_followGUID;
        // minimum follow distance - we will go there when we are too far away
        float m_followDistance;
        // maximum distance - a step further and we will go closer again
        float m_maxDistance;
        // time of last distance check
        uint32_t m_lastCheckTime;
        // child motion generator for generating movement between waypoints
        PointMovementGenerator m_pointMovementGenerator;
};

#endif
