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
#include "Pathfinding.h"
#include "MotionMaster.h"
#include "Map.h"
#include "Log.h"

Pathfinder::Pathfinder(Map* map) : m_map(map)
{
    m_moveMask = (1 << MOVEMENT_TYPE_WALK);
}

Pathfinder::~Pathfinder()
{
    //
}

void Pathfinder::SetSource(float x, float y)
{
    m_sourcePos.x = x;
    m_sourcePos.y = y;
}

void Pathfinder::SetTarget(float x, float y)
{
    m_destPos.x = x;
    m_destPos.y = y;
}

void Pathfinder::SetMovementMask(uint32_t moveMask)
{
    m_moveMask = moveMask;
}

float Pathfinder::CalculateHeuristic(uint32_t srcX, uint32_t srcY, uint32_t dstX, uint32_t dstY)
{
    // octile distance (diagonal distance with up/down/left/right cost of 1, and diagonal cost of square root of 2)

    float dX = fabs((float)((int)dstX - (int)srcX));
    float dY = fabs((float)((int)dstY - (int)srcY));

    return (float)(num_min(dX, dY)*sqrt(2) + fabs((float)(dX - dY)));
}

uint32_t Pathfinder::GetDestMoveMask(uint32_t srcX, uint32_t srcY, uint32_t dstX, uint32_t dstY)
{
    uint32_t moveMask = 0;

    // if two fields differ in vertical direction
    if (srcY > dstY)
        moveMask |= MOVE_UP;
    else if (srcY < dstY)
        moveMask |= MOVE_DOWN;

    // if they differ in horizontal direction
    if (srcX > dstX)
        moveMask |= MOVE_LEFT;
    else if (srcX < dstX)
        moveMask |= MOVE_RIGHT;

    return moveMask;
}

// we use this instead of std::pair stuff; this should be a bit faster
#define MP32(a,b) ((((uint64_t)a) << 32L) | ((uint64_t)b))

void Pathfinder::PushPathfindRecord(PathfindFieldRecord* pfrecord)
{
    m_searchedList.push_back(pfrecord);
    m_pathfindQueue.push(pfrecord);
    m_visitedSet.insert(MP32(pfrecord->x, pfrecord->y));

    // if we got closer, store closest record
    if (!m_closest || m_closest->dstCost > pfrecord->dstCost)
        m_closest = pfrecord;
}

void Pathfinder::ClearPathfindStructures()
{
    // clear remaining items in queue (if any)
    while (!m_pathfindQueue.empty())
        m_pathfindQueue.pop();

    // delete used records
    for (PathfindFieldRecord* pff : m_searchedList)
        delete pff;

    m_searchedList.clear();
    m_visitedSet.clear();
}

bool Pathfinder::FindPath(MotionPointVector& dstVector, uint32_t* outFlags)
{
    // clear flags
    *outFlags = 0;

    uint32_t i = 0;

    m_closest = nullptr;

    uint32_t curX = (uint32_t)m_sourcePos.x;
    uint32_t curY = (uint32_t)m_sourcePos.y;

    uint32_t dstX = (uint32_t)m_destPos.x;
    uint32_t dstY = (uint32_t)m_destPos.y;

    PathfindFieldRecord* pff;
    PathfindFieldRecord* pfftr;

    // push the source field, so the algorithm may be generic from this point
    pff = new PathfindFieldRecord(curX, curY, nullptr, 0.0f, CalculateHeuristic(curX, curY, dstX, dstY));
    PushPathfindRecord(pff);

    PathfindField* pfm[4];
    PathfindField* diagfld;

    // while there's something to be processed and the pathfinding loop didn't exceed the limit
    while (!m_pathfindQueue.empty() && i < PATHFIND_ITERATIONS_LIMIT)
    {
        i++;

        // pop the element with lowest heuristic (guaranteed by priority queue priority function)
        pff = m_pathfindQueue.top();
        m_pathfindQueue.pop();

        curX = pff->x;
        curY = pff->y;

        // if we reached the destination, leave loop
        if (curX == dstX && curY == dstY)
            break;

        pfm[0] = m_map->GetPathfindField(curX - 1, curY); // left
        pfm[1] = m_map->GetPathfindField(curX + 1, curY); // right
        pfm[2] = m_map->GetPathfindField(curX, curY - 1); // up
        pfm[3] = m_map->GetPathfindField(curX, curY + 1); // down

        // the algorithm takes left/right/up/down directions first and checks, if they are accessible and "walkable" with movement type mask set
        // if yes, pushes them into priority queue for later processing; then the diagonal fields are taken, but only if their "elementary directions"
        // are accessible (i.e. left-up field is taken only if left AND up fields are accessible).

        // left
        if (pfm[0] && (pfm[0]->moveType & m_moveMask) != 0)
        {
            if (m_visitedSet.find(MP32(curX - 1, curY)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX - 1, curY, pff, pff->srcCost + 1.0f, CalculateHeuristic(curX - 1, curY, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }
        else
            pfm[0] = nullptr;

        // right
        if (pfm[1] && (pfm[1]->moveType & m_moveMask) != 0)
        {
            if (m_visitedSet.find(MP32(curX + 1, curY)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX + 1, curY, pff, pff->srcCost + 1.0f, CalculateHeuristic(curX + 1, curY, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }
        else
            pfm[1] = nullptr;

        // up
        if (pfm[2] && (pfm[2]->moveType & m_moveMask) != 0)
        {
            if (m_visitedSet.find(MP32(curX, curY - 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX, curY - 1, pff, pff->srcCost + 1.0f, CalculateHeuristic(curX, curY - 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }
        else
            pfm[2] = nullptr;

        // down
        if (pfm[3] && (pfm[3]->moveType & m_moveMask) != 0)
        {
            if (m_visitedSet.find(MP32(curX, curY + 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX, curY + 1, pff, pff->srcCost + 1.0f, CalculateHeuristic(curX, curY + 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }
        else
            pfm[3] = nullptr;

        // diagonal fields

        // left up
        if (pfm[0] && pfm[2])
        {
            diagfld = m_map->GetPathfindField(curX - 1, curY - 1);
            if (diagfld && (diagfld->moveType & m_moveMask) != 0 && m_visitedSet.find(MP32(curX - 1, curY - 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX - 1, curY - 1, pff, pff->srcCost + sqrt(2.0f), CalculateHeuristic(curX - 1, curY - 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }

        // left down
        if (pfm[0] && pfm[3])
        {
            diagfld = m_map->GetPathfindField(curX - 1, curY + 1);
            if (diagfld && (diagfld->moveType & m_moveMask) != 0 && m_visitedSet.find(MP32(curX - 1, curY + 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX - 1, curY + 1, pff, pff->srcCost + sqrt(2.0f), CalculateHeuristic(curX - 1, curY + 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }

        // right up
        if (pfm[1] && pfm[2])
        {
            diagfld = m_map->GetPathfindField(curX + 1, curY - 1);
            if (diagfld && (diagfld->moveType & m_moveMask) != 0 && m_visitedSet.find(MP32(curX + 1, curY - 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX + 1, curY - 1, pff, pff->srcCost + sqrt(2.0f), CalculateHeuristic(curX + 1, curY - 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }

        // right down
        if (pfm[1] && pfm[3])
        {
            diagfld = m_map->GetPathfindField(curX + 1, curY + 1);
            if (diagfld && (diagfld->moveType & m_moveMask) != 0 && m_visitedSet.find(MP32(curX + 1, curY + 1)) == m_visitedSet.end())
            {
                pfftr = new PathfindFieldRecord(curX + 1, curY + 1, pff, pff->srcCost + sqrt(2.0f), CalculateHeuristic(curX + 1, curY + 1, dstX, dstY));
                PushPathfindRecord(pfftr);
            }
        }
    }

    // path not found, set "incomplete" flag and use closest record
    if (curX != dstX || curY != dstY)
    {
        *outFlags |= PATH_INCOMPLETE;
        pff = m_closest;
    }

    // vector for reconstructed path
    std::vector<PathfindFieldRecord*> pfrecs;

    // reconstruct path using parent ("prev") pointers
    while (pff != nullptr)
    {
        pfrecs.push_back(pff);
        pff = pff->prev;
    }

    // if we wouldn't move from place, no path has been found at all
    if (pfrecs.size() <= 1)
    {
        ClearPathfindStructures();
        return false;
    }

    // inverse the reconstructed path (so it goes from start to destination, not backwards)
    std::reverse(pfrecs.begin(), pfrecs.end());

    // and build motion point path from reconstructed path
    for (uint32_t i = 1; i < pfrecs.size(); i++)
    {
        uint8_t moveMask = GetDestMoveMask(pfrecs[i - 1]->x, pfrecs[i - 1]->y, pfrecs[i]->x, pfrecs[i]->y);
        dstVector.push_back(MotionPoint((float)pfrecs[i]->x + 0.5f, (float)pfrecs[i]->y + 0.5f, moveMask));
    }

    ClearPathfindStructures();
    return true;
}

#undef MP32
