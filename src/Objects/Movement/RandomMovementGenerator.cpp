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
#include "Unit.h"
#include "Creature.h"
#include "MotionMaster.h"
#include "PointMovementGenerator.h"
#include "MovementGeneratorBase.h"
#include "RandomMovementGenerator.h"
#include "Random.h"
#include "Log.h"

RandomMovementGenerator::RandomMovementGenerator(Unit* owner) : MovementGeneratorBase(owner, MOVEMENT_RANDOM), m_pointMovementGenerator(owner)
{
    // initialize child point movement generator as composite child
    m_pointMovementGenerator.SetCompositeChildFlag(true, this);
    m_lastPointTime = 0;
    m_moveNextPoint = false;
    m_minDelay = 0;
    m_maxDelay = 0;
    m_distanceOrigin = 2.0f;
    m_currentDelay = 0;

    // default origin is owner initial position
    m_origin.x = owner->GetPositionX();
    m_origin.y = owner->GetPositionY();
}

RandomMovementGenerator::~RandomMovementGenerator()
{
    //
}

void RandomMovementGenerator::Initialize()
{
    m_pointMovementGenerator.Initialize();
}

void RandomMovementGenerator::Finalize()
{
    m_pointMovementGenerator.Finalize();
}

void RandomMovementGenerator::Update()
{
    m_pointMovementGenerator.Update();

    MovementGeneratorBase::Update();

    // if the flag for further movement is set, move on
    if (m_moveNextPoint)
    {
        // if the delay is not set, or has already passed, move
        if (!m_currentDelay || getMSTimeDiff(m_lastPointTime, getMSTime()) >= m_currentDelay)
        {
            m_moveNextPoint = false;

            MoveToNextRandomPoint();
        }
    }
}

void RandomMovementGenerator::ReceiveChildSignal(uint32_t signalId, uint32_t param)
{
    // for now we react to both of these events in the same way - just go to next point
    if (signalId == MOVEMENT_SIGNAL_POINT_REACHED || signalId == MOVEMENT_SIGNAL_CANNOT_REACH_POINT)
    {
        m_lastPointTime = getMSTime();
        if (m_maxDelay == m_minDelay)
            m_currentDelay = m_maxDelay;
        else
            m_currentDelay = urand(m_minDelay, m_maxDelay);
        m_moveNextPoint = true;
    }
}

void RandomMovementGenerator::SetOrigin(float x, float y)
{
    m_origin.x = x;
    m_origin.y = y;
}

void RandomMovementGenerator::StartRandomMovement(float distanceOrigin, uint32_t minDelay, uint32_t maxDelay)
{
    // store parameters
    m_distanceOrigin = distanceOrigin;
    m_minDelay = minDelay;
    m_maxDelay = maxDelay;

    // and launch generator
    MoveToNextRandomPoint();
}

void RandomMovementGenerator::MoveToNextRandomPoint()
{
    // get random properties (random distance and angle
    float dist = frand(0.0f, m_distanceOrigin);
    float angle = frand(0.0f, 2 * F_PI);

    // get final destination
    Position dest(m_origin.x + dist * cosf(angle),
                  m_origin.y + dist * sinf(angle));

    // go !
    m_pointMovementGenerator.SetTarget(dest.x, dest.y);
}
