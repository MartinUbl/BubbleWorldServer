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
#include "FollowMovementGenerator.h"
#include "ObjectAccessor.h"

FollowMovementGenerator::FollowMovementGenerator(Unit* owner) : MovementGeneratorBase(owner, MOVEMENT_FOLLOW), m_pointMovementGenerator(owner)
{
    // initialize child point movement generator as composite child
    m_pointMovementGenerator.SetCompositeChildFlag(true, this);
    m_lastPointTime = 0;
    m_followDistance = 1.0f;
    m_maxDistance = 2.0f;
    m_followGUID = 0;
    m_lastCheckTime = 0;
}

FollowMovementGenerator::~FollowMovementGenerator()
{
    //
}

void FollowMovementGenerator::Initialize()
{
    m_pointMovementGenerator.Initialize();
}

void FollowMovementGenerator::Finalize()
{
    m_pointMovementGenerator.Finalize();
}

void FollowMovementGenerator::Update()
{
    m_pointMovementGenerator.Update();

    MovementGeneratorBase::Update();

    if (m_pointMovementGenerator.IsStopped() && getMSTimeDiff(m_lastCheckTime, getMSTime()) >= FOLLOW_DISTANCE_CHECK_DELAY)
    {
        m_lastCheckTime = getMSTime();

        if (!CheckFollowerDistance())
            MoveToClosePoint();
    }
}

void FollowMovementGenerator::ReceiveChildSignal(uint32_t signalId, uint32_t param)
{
    // for now we react to both of these events in the same way
    if (signalId == MOVEMENT_SIGNAL_POINT_REACHED || signalId == MOVEMENT_SIGNAL_CANNOT_REACH_POINT)
    {
        // nothing to do for now?
    }
}

bool FollowMovementGenerator::CheckFollowerDistance()
{
    WorldObject* target = sObjectAccessor->FindWorldObject(m_followGUID);
    // if the target is unreachable, terminate movement and return true (to avoid finding close point)
    if (!target || target->GetMapId() != m_owner->GetMapId())
    {
        TerminateMovement();
        return true;
    }

    // if the distance is greater than it should be, return false to move to close point
    if (target->GetMinimumBoxDistance(m_owner) > m_maxDistance)
        return false;

    return true;
}

void FollowMovementGenerator::StartFollowing(Unit* target, float followDistance, float maxDistance)
{
    // store parameters
    if (!target)
    {
        TerminateMovement();
        return;
    }

    m_followGUID = target->GetGUID();
    m_followDistance = followDistance;
    m_maxDistance = maxDistance;
    m_lastCheckTime = 0;

    // and launch generator if needed
    if (!CheckFollowerDistance())
        MoveToClosePoint();
}

void FollowMovementGenerator::MoveToClosePoint()
{
    WorldObject* target = sObjectAccessor->FindWorldObject(m_followGUID);
    // if the target is unreachable, terminate movement and return true (to avoid finding close point)
    if (!target || target->GetMapId() != m_owner->GetMapId())
    {
        TerminateMovement();
        return;
    }

    // create follow vector, make it unit, turn it around and multiply by follow distance
    Vector2 distVector(target->GetPositionX() - m_owner->GetPositionX(), target->GetPositionY() - m_owner->GetPositionY());
    distVector.MakeUnit();
    distVector *= -m_followDistance;

    Position dest(target->GetPositionX() + distVector.x,
                  target->GetPositionY() + distVector.y);

    m_pointMovementGenerator.SetTarget(dest.x, dest.y);
}
