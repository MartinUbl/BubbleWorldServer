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

#ifndef BW_PATHFINDING_H
#define BW_PATHFINDING_H

#define PATHFIND_ITERATIONS_LIMIT 100

// enum of pathfinding flags
enum PathfindFlags
{
    PATH_INCOMPLETE = 0x01,         // only incomplete path was found; perform another attempt when we reach end
};

/*
 * Structure containing working data for A* pathfinding algorithm
 */
struct PathfindFieldRecord
{
    PathfindFieldRecord(uint32_t _x, uint32_t _y, PathfindFieldRecord* _prev, float _srcCost, float _dstCost) : x(_x), y(_y), prev(_prev), srcCost(_srcCost), dstCost(_dstCost) {};

    // field X coordinate
    uint32_t x;
    // field Y coordinate
    uint32_t y;
    // previous record (parent, to allow path reconstruction)
    PathfindFieldRecord* prev;
    // source cost (from start to here)
    float srcCost;
    // destination cost (from here to destination)
    float dstCost;
};

/*
 * Structure serving as pathfinding priority queue comparator (priority function)
 */
struct PathfindPriorityComparator
{
    bool operator()(PathfindFieldRecord const* a, PathfindFieldRecord const* b)
    {
        return a->srcCost + a->dstCost > b->srcCost + b->dstCost;
    }
};

struct Position;
class Map;

/*
 * Class maintaining pathfinding routines on specified map
 */
class Pathfinder
{
    public:
        Pathfinder(Map* map);
        virtual ~Pathfinder();

        // set the pathfinder source - from where we will look for path
        void SetSource(float x, float y);
        // set the pathfinder destination - where we will to get
        void SetTarget(float x, float y);
        // sets movement type mask (walk, swim, .. ) to be able to find path within all possible fields
        void SetMovementMask(uint32_t moveMask);

        // find path using A* algorithm with previously set parameters; returns true when found, false when not found
        bool FindPath(MotionPointVector& dstVector, uint32_t* outFlags);

    protected:

        // calculates heuristic between two fields (using octile distance)
        float CalculateHeuristic(uint32_t srcX, uint32_t srcY, uint32_t dstX, uint32_t dstY);
        // retrieves movement direction mask for traversing between two fields
        template<typename T>
        uint32_t GetDestMoveMask(T srcX, T srcY, T dstX, T dstY);

        // pushes pathfinding record to priority queue
        void PushPathfindRecord(PathfindFieldRecord* pfrecord);
        // clears all pathfinding structures used
        void ClearPathfindStructures();

        // source position
        Position m_sourcePos;
        // destination position
        Position m_destPos;
        // allowed movement type mask
        uint32_t m_moveMask;

        // instance of map we are finding path in
        Map* m_map;

        // priority queue used for determining next field to be expanded
        std::priority_queue<PathfindFieldRecord*, std::vector<PathfindFieldRecord*>, PathfindPriorityComparator> m_pathfindQueue;
        // list of all searched fields
        std::list<PathfindFieldRecord*> m_searchedList;
        // set consisting of all visited coordinate pairs
        std::set<uint64_t> m_visitedSet;

        // we are always storing the closest record to destination in case no complete path was found
        PathfindFieldRecord* m_closest;

    private:
        //
};

#endif
