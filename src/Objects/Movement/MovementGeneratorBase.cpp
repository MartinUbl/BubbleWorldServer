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
#include "MotionMaster.h"
#include "MovementGeneratorBase.h"
#include "Unit.h"

MovementGeneratorBase::MovementGeneratorBase(Unit* owner, MotionType type) : m_owner(owner), m_type(type), m_isCompositeChild(false), m_parent(nullptr)
{
    m_movementPointId = 0;
    m_lastPointTime = 0;
    m_nextPointDiffTime = 0;
    m_pathfinderFlags = 0;
    m_stopped = true;
}

MovementGeneratorBase::~MovementGeneratorBase()
{
    //
}

MotionType MovementGeneratorBase::GetType()
{
    return m_type;
}

void MovementGeneratorBase::SetCompositeChildFlag(bool flag, MovementGeneratorBase* parent)
{
    m_isCompositeChild = flag;
    m_parent = parent;
}

bool MovementGeneratorBase::IsCompositeChild()
{
    return m_isCompositeChild;
}

void MovementGeneratorBase::Initialize()
{
    //
}

void MovementGeneratorBase::Update()
{
    // if we are moving
    if (m_lastPointTime != 0 && m_nextPointDiffTime != 0)
    {
        // and if we should be at the destination point yet
        if (getMSTimeDiff(m_lastPointTime, getMSTime()) >= m_nextPointDiffTime)
        {
            m_nextPointDiffTime = 0;

            PointReached(m_movementPointId);

            // the PointReached method MUST maintain movement timers reset or unset (usually through SetNextMovement or StopMovement methods)
        }
    }
}

void MovementGeneratorBase::Finalize()
{
    //
}

void MovementGeneratorBase::PointReached(uint32_t id)
{
    // if no behaviour defined by child class, we stop the movement by default

    StopMovement();
}

void MovementGeneratorBase::TerminateMovement()
{
    // we fall back only when we are not child of other movement generator
    if (!IsCompositeChild())
    {
        // TODO: determine actual movement generator, that's default for NPC (may be random, may be waypoint, may be idle)

        m_owner->GetMotionMaster().MoveIdle();
    }
}

void MovementGeneratorBase::ReceiveChildSignal(uint32_t signalId, uint32_t param)
{
    // no implicit behaviour
}

void MovementGeneratorBase::SetNextMovement(uint32_t id, float sourceX, float sourceY, float destX, float destY, uint8_t moveMask)
{
    // calculate distance and speed
    float distance = sqrtf(powf(sourceX - destX, 2.0f) + powf(sourceY - destY, 2.0f));
    float perMillisecond = m_owner->GetFloatValue(UNIT_FIELD_MOVEMENT_SPEED) * 0.001f;

    // get real delay
    uint32_t msDelay = (uint32_t)(distance / perMillisecond);

    m_lastPointTime = getMSTime();
    m_nextPointDiffTime = msDelay;
    m_stopped = false;

    // set movement elements, so the client will see proper motion
    for (uint8_t i = 0; i < 4; i++)
    {
        if ((moveMask & (1 << i)) != 0)
            m_owner->StartMoving((MoveDirectionElement)(1 << i));
        else
            m_owner->StopMoving((MoveDirectionElement)(1 << i));
    }

    m_movementPointId = id;
}

void MovementGeneratorBase::StopMovement()
{
    m_lastPointTime = 0;
    m_nextPointDiffTime = 0;
    m_stopped = true;

    // stop all movement elements
    for (uint8_t i = 0; i < 4; i++)
        m_owner->StopMoving((MoveDirectionElement)(1 << i));
}
