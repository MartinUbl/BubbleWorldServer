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
#include "MotionMaster.h"
#include "MovementGeneratorBase.h"
#include "PointMovementGenerator.h"
#include "Pathfinding.h"

PointMovementGenerator::PointMovementGenerator(Unit* owner) : MovementGeneratorBase(owner, MOVEMENT_POINT)
{
    m_originalTarget.x = 0.0f;
    m_originalTarget.y = 0.0f;
    m_pointId = 0;
}

PointMovementGenerator::~PointMovementGenerator()
{
    //
}

void PointMovementGenerator::Finalize()
{
    StopMovement();
}

void PointMovementGenerator::SetPointId(uint32_t id)
{
    m_pointId = id;
}

uint32_t PointMovementGenerator::GetPointId()
{
    return m_pointId;
}

void PointMovementGenerator::SetTarget(float x, float y)
{
    m_originalTarget.x = x;
    m_originalTarget.y = y;

    m_fullPath.clear();

    // create pathfinder instance, set source and target position
    Pathfinder pf(m_owner->GetMap());

    pf.SetSource(m_owner->GetPositionX(), m_owner->GetPositionY());
    pf.SetTarget(x, y);

    // if the path was found, move by it, otherwise stop movement and fall back to idle movement
    if (pf.FindPath(m_fullPath, &m_pathfinderFlags))
        SetNextMovement(0, m_owner->GetPositionX(), m_owner->GetPositionY(), m_fullPath[0].position.x, m_fullPath[0].position.y, m_fullPath[0].moveMask);
    else
    {
        StopMovement();
        TerminateMovement();
        if (IsCompositeChild() && m_parent)
            m_parent->ReceiveChildSignal(MOVEMENT_SIGNAL_CANNOT_REACH_POINT, m_pointId);
    }
}

void PointMovementGenerator::PointReached(uint32_t id)
{
    uint32_t nextId = id + 1;

    // if the next point does not exist
    if (nextId >= m_fullPath.size())
    {
        // when no complete path found, try finding the path again; if no better path found, the movement generator will stop
        if (m_pathfinderFlags & PATH_INCOMPLETE)
            SetTarget(m_originalTarget.x, m_originalTarget.y);
        else
        {
            StopMovement();
            TerminateMovement();
        }

        return;
    }

    // signal the parent motion generator about reaching point
    if (IsCompositeChild() && m_parent)
        m_parent->ReceiveChildSignal(MOVEMENT_SIGNAL_POINT_REACHED, m_pointId);

    // signal the owner about point reaching
    m_owner->MovementGeneratorPointReached(m_pointId);

    // move to next point
    SetNextMovement(nextId, m_owner->GetPositionX(), m_owner->GetPositionY(), m_fullPath[nextId].position.x, m_fullPath[nextId].position.y, m_fullPath[nextId].moveMask);
}
