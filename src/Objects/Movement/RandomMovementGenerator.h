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

#ifndef BW_RANDOMMOVEMENTGENERATOR_H
#define BW_RANDOMMOVEMENTGENERATOR_H

#include "MovementGeneratorBase.h"

class Unit;
class PointMovementGenerator;

/*
 * Class representing motion generator for random movement (technically multiple point movements generated randomly)
 */
class RandomMovementGenerator : public MovementGeneratorBase
{
    public:
        RandomMovementGenerator(Unit* owner);
        virtual ~RandomMovementGenerator();

        virtual void Initialize();
        virtual void Finalize();

        virtual void Update();
        virtual void ReceiveChildSignal(uint32_t signalId, uint32_t param);

        // sets random movement origin ("center" of random radius)
        void SetOrigin(float x, float y);
        // starts random movement with supplied parameters
        void StartRandomMovement(float distanceOrigin, uint32_t minDelay, uint32_t maxDelay);

    protected:
        // starts movement towards new random point
        void MoveToNextRandomPoint();

    private:
        // origin for calculating random points
        Position m_origin;
        // maximum distance from spawn position
        float m_distanceOrigin;
        // minimum delay before next random point
        uint32_t m_minDelay;
        // maximum delay before next random point
        uint32_t m_maxDelay;
        // current delay set
        uint32_t m_currentDelay;
        // child motion generator for generating movement between waypoints
        PointMovementGenerator m_pointMovementGenerator;
        // flag set by signaling event to start movement to next point in next Update tick
        bool m_moveNextPoint;
        // time of reaching last point
        uint32_t m_lastPointTime;
};

#endif
