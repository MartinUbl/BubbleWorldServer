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
#include "MovementGeneratorBase.h"
#include "MotionMaster.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "Log.h"

MotionMaster::MotionMaster(Unit* owner) : m_owner(owner)
{
    m_movementGenerator = nullptr;
}

void MotionMaster::Initialize()
{
    // start with "idle movement"
    ChangeTo<IdleMovementGenerator>();
}

void MotionMaster::Update()
{
    if (m_movementGenerator)
        m_movementGenerator->Update();

    if (m_movementGeneratorChangeLambda != nullptr)
    {
        m_movementGeneratorChangeLambda();
        m_movementGeneratorChangeLambda = nullptr;
    }
}

MotionType MotionMaster::GetCurrentMotionType()
{
    if (m_movementGenerator)
        return m_movementGenerator->GetType();

    return MAX_MOTION_TYPE;
}

void MotionMaster::MoveIdle()
{
    m_movementGeneratorChangeLambda = [this]() {
        ChangeTo<IdleMovementGenerator>();
    };
}

void MotionMaster::MovePoint(float x, float y)
{
    m_movementGeneratorChangeLambda = [this, x, y]() {
        PointMovementGenerator* gen = ChangeTo<PointMovementGenerator>();

        gen->SetTarget(x, y);
    };
}

void MotionMaster::MoveWaypointPath(uint32_t pathId)
{
    m_movementGeneratorChangeLambda = [this, pathId]() {
        WaypointMovementGenerator* gen = ChangeTo<WaypointMovementGenerator>();

        gen->StartWaypointPathId(pathId);
    };
}

template <class T>
T* MotionMaster::ChangeTo()
{
    // finalize and delete old movement generator
    if (m_movementGenerator)
    {
        m_movementGenerator->Finalize();
        delete m_movementGenerator;
    }

    // create and initialize new movement generator
    m_movementGenerator = new T(m_owner);
    m_movementGenerator->Initialize();

    return ((T*)m_movementGenerator);
}
