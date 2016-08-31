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
#include "WaypointMovementGenerator.h"
#include "WaypointStorage.h"
#include "Log.h"

WaypointMovementGenerator::WaypointMovementGenerator(Unit* owner) : MovementGeneratorBase(owner, MOVEMENT_WAYPOINT), m_pointMovementGenerator(owner)
{
    // initialize child point movement generator as composite child
    m_pointMovementGenerator.SetCompositeChildFlag(true, this);
    m_forward = true;
    m_lastPointTime = 0;
    m_moveNextPoint = false;
}

WaypointMovementGenerator::~WaypointMovementGenerator()
{
    //
}

void WaypointMovementGenerator::Initialize()
{
    m_pointMovementGenerator.Initialize();
}

void WaypointMovementGenerator::Finalize()
{
    m_pointMovementGenerator.Finalize();
}

void WaypointMovementGenerator::Update()
{
    m_pointMovementGenerator.Update();

    MovementGeneratorBase::Update();

    // if the flag for further movement is set, move on
    if (m_moveNextPoint)
    {
        // if the delay is not set, or has already passed, move
        if (m_waypoints[m_pointId].waitDelay == 0 || getMSTimeDiff(m_lastPointTime, getMSTime()) >= m_waypoints[m_pointId].waitDelay)
        {
            m_moveNextPoint = false;

            // select next point if available; if not, end movement and go back to default
            uint32_t nextPoint;
            if (!SelectNextPoint(nextPoint))
            {
                StopMovement();
                TerminateMovement();
                return;
            }

            // move!
            MoveToPoint(nextPoint);
        }
    }
}

void WaypointMovementGenerator::ReceiveChildSignal(uint32_t signalId, uint32_t param)
{
    // for now we react to both of these events in the same way - just go to next point
    if ((signalId == MOVEMENT_SIGNAL_POINT_REACHED || signalId == MOVEMENT_SIGNAL_CANNOT_REACH_POINT) && param == m_pointId + 1)
    {
        m_lastPointTime = getMSTime();
        m_moveNextPoint = true;
    }
}

void WaypointMovementGenerator::StartWaypointPathId(uint32_t id)
{
    m_pathId = id;

    // load waypoints
    WaypointList* wplist = sWaypointStorage->GetWaypointPath(id);
    // if not available or empty, we won't move at all
    if (!wplist || wplist->empty())
    {
        TerminateMovement();
        return;
    }

    // prepare waypoints structure
    m_waypoints.clear();
    m_waypoints.resize(wplist->size());

    uint32_t i = 0;

    // copy waypoint data to avoid data race in case of i.e. live reload
    WaypointRecord* rec;
    for (WaypointList::iterator itr = wplist->begin(); itr != wplist->end(); ++itr)
    {
        Position basePosition(0, 0);
        rec = &(*itr);
        // if the waypoint has this flag, we will consider spawn position as base position
        if ((rec->flags & WPFLAG_RELATIVE_SPAWN) != 0 && m_owner->GetType() == OTYPE_CREATURE)
            basePosition = m_owner->ToCreature()->GetSpawnPosition();

        m_waypoints[i] = WaypointPreparedRecord(basePosition.x + rec->positionX, basePosition.y + rec->positionY, rec->waitDelay, (rec->flags & WPFLAG_RELATIVE_CURRENT) != 0);

        i++;
    }

    // determine repeat type using first and last point
    WaypointRecord &frst = *(wplist->begin());
    WaypointRecord &last = *(wplist->rbegin());

    m_repeatType = WPPATH_REPEAT_NONE;

    // if the last point has "repeat" flag, we repeat the path from 0 (0..n, 0..n, ..)
    if (last.flags & WPFLAG_REPEAT)
        m_repeatType = WPPATH_REPEAT_FROM_START;
    // if the last point has "repeat in reverse" flag, we have two more outcomes
    else if (last.flags & WPFLAG_REPEAT_REVERSE)
    {
        // if also the first waypoint has this flag, we repeat the whole path from beginning to and and back to beginning over and over (0..n, n..0, 0..n, ..)
        if (frst.flags & WPFLAG_REPEAT_REVERSE)
            m_repeatType = WPPATH_REPEAT_CYCLIC;
        else // if not, we just go forth and back and end the motion generation
            m_repeatType = WPPATH_REPEAT_BACKWARD;
    }

    // start moving towards the first point
    MoveToPoint(0);
}

uint32_t WaypointMovementGenerator::GetWaypointPathId()
{
    return m_pathId;
}

void WaypointMovementGenerator::MoveToPoint(uint32_t pointId)
{
    WaypointPreparedRecord const& wp = m_waypoints[pointId];

    // build position
    Position dest(wp.positionX, wp.positionY);
    // if the "relative to current position" flag was set, calculate position
    if (wp.relativeCurrent)
    {
        // if moving backwards, "reverse" the path using negative coordinates
        if (!m_forward)
        {
            dest.x *= -1.0f;
            dest.y *= -1.0f;
        }

        // add current position
        dest.x += m_owner->GetPositionX();
        dest.y += m_owner->GetPositionY();
    }

    // and move!

    m_pointId = pointId;

    // note the "+ 1" - this is due to remaining consistent with database and signaling events to AI and so.
    m_pointMovementGenerator.SetPointId(pointId + 1);
    m_pointMovementGenerator.SetTarget(dest.x, dest.y);
}

bool WaypointMovementGenerator::SelectNextPoint(uint32_t &target)
{
    // forward direction
    if (m_forward)
    {
        // if we would end...
        if (m_pointId == m_waypoints.size() - 1)
        {
            // if we should repeat the path backwards, reverse
            if (m_repeatType == WPPATH_REPEAT_BACKWARD || m_repeatType == WPPATH_REPEAT_CYCLIC)
            {
                m_forward = false;
                target = m_pointId;
            }
            // if we should repeat from start, set next point to 0
            else if (m_repeatType == WPPATH_REPEAT_FROM_START)
            {
                target = 0;
            }
            else // otherwise end
                return false;
        }
        else // if no special condition, just move to next point
            target = m_pointId + 1;
    }
    else // backward direction
    {
        // if we are at the beginning...
        if (m_pointId == 0)
        {
            // if we should repeat the whole path again, reverse
            if (m_repeatType == WPPATH_REPEAT_CYCLIC)
            {
                m_forward = true;
                target = m_pointId;
            }
            else // otherwise end
                return false;
        }
        else // if no special condition, just move to next (previous) point
            target = m_pointId - 1;
    }

    // good, we found next waypoint to go to
    return true;
}
