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
#include "Opcodes.h"
#include "SmartPacket.h"
#include "Log.h"
#include "Map.h"
#include "MapStorage.h"
#include "Player.h"

Unit::Unit(ObjectType type) : WorldObject(type)
{
    m_moveMask = 0;
    m_lastMovementUpdate = 0;
}

Unit::~Unit()
{
    //
}

void Unit::Create(uint64_t guid)
{
    WorldObject::Create(guid);
}

void Unit::Update()
{
    WorldObject::Update();

    // update movement if the unit is moving
    if (IsMoving())
    {
        const uint32_t msNow = getMSTime();

        // update position using movement vector
        uint32_t msDiff = getMSTimeDiff(m_lastMovementUpdate, msNow);
        if (msDiff >= 1)
        {
            float coef = (float)msDiff;
            float oldX = m_position.x;
            float oldY = m_position.y;

            m_position.x += coef * m_moveVector.x;
            if (m_position.x < 0.0f)
                m_position.x = 0.0f;

            MapField* mf = GetMap()->GetField(m_position.x, m_position.y);
            if (!mf || !CanMoveOn((MapFieldType)mf->type, mf->flags))
                m_position.x = oldX;

            m_position.y += coef * m_moveVector.y;
            if (m_position.y < 0.0f)
                m_position.y = 0.0f;

            mf = GetMap()->GetField(m_position.x, m_position.y);
            if (!mf || !CanMoveOn((MapFieldType)mf->type, mf->flags))
                m_position.y = oldY;

            m_lastMovementUpdate = msNow;
        }
    }
}

void Unit::BuildCreatePacketBlock(SmartPacket &pkt)
{
    WorldObject::BuildCreatePacketBlock(pkt);

    pkt.WriteFloat(m_position.x);
    pkt.WriteFloat(m_position.y);
    pkt.WriteUInt8(m_moveMask);
}

void Unit::CreateUpdateFields()
{
    // Unit is also not a standalone class

    SetFloatValue(UNIT_FIELD_MOVEMENT_SPEED, 4.0f);

    WorldObject::CreateUpdateFields();
}

uint16_t Unit::GetLevel()
{
    return GetUInt32Value(UNIT_FIELD_LEVEL);
}

void Unit::SetLevel(uint16_t lvl, bool onLoad)
{
    SetUInt32Value(UNIT_FIELD_LEVEL, lvl);

    if (onLoad)
        return;

    // TODO: things related to levelup / leveldown ?
}

bool Unit::CanMoveOn(MapFieldType type, uint32_t flags)
{
    // for now just ground type
    return (type == MFT_GROUND);
}

void Unit::StartMoving(MoveDirectionElement dir)
{
    if ((m_moveMask & dir) != 0)
        return;

    SmartPacket pkt(SP_MOVE_START_DIRECTION);
    pkt.WriteUInt64(GetGUID());
    pkt.WriteUInt8(dir);
    SendPacketToSorroundings(pkt);

    // if started movement, set timers for movement update
    if (m_moveMask == 0)
        m_lastMovementUpdate = getMSTime();

    m_moveMask |= dir;
    UpdateMovementVector();
}

void Unit::StopMoving(MoveDirectionElement dir)
{
    if ((m_moveMask & dir) == 0)
        return;

    SmartPacket pkt(SP_MOVE_STOP_DIRECTION);
    pkt.WriteUInt64(GetGUID());
    pkt.WriteUInt8(dir);
    pkt.WriteFloat(GetPositionX());
    pkt.WriteFloat(GetPositionY());
    SendPacketToSorroundings(pkt);

    m_moveMask &= ~dir;
    UpdateMovementVector();
}

bool Unit::IsMoving()
{
    return m_moveMask != 0;
}

uint8_t Unit::GetMoveMask()
{
    return m_moveMask;
}

void Unit::UpdateMovementVector()
{
    // every fifth state (0000, 0101, 1010, 1111) cancels the movement
    if (m_moveMask % 5 == 0)
    {
        m_moveVector.x = 0;
        m_moveVector.y = 0;
    }
    else
    {
        // update vecotr using polar coordinates
        m_moveVector.SetFromPolar(movementAngles[m_moveMask], GetFloatValue(UNIT_FIELD_MOVEMENT_SPEED));
        m_moveVector.x *= MOVEMENT_UPDATE_UNIT_FRACTION;
        m_moveVector.y *= MOVEMENT_UPDATE_UNIT_FRACTION;
    }
}

void Unit::Talk(TalkType type, const char* str)
{
    SmartPacket pkt(SP_CHAT_MESSAGE);
    pkt.WriteUInt8(type);
    pkt.WriteUInt64(GetGUID());
    pkt.WriteString(str);
    SendPacketToSorroundings(pkt);
}

void Unit::TalkTo(TalkType type, const char* str, Player* target)
{
    SmartPacket pkt(SP_CHAT_MESSAGE);
    pkt.WriteUInt8(type);
    pkt.WriteUInt64(GetGUID());
    pkt.WriteString(str);
    target->SendPacketToMe(pkt);
}

uint32_t Unit::GetHealth()
{
    return GetUInt32Value(UNIT_FIELD_HEALTH);
}

uint32_t Unit::GetMaxHealth()
{
    return GetUInt32Value(UNIT_FIELD_MAXHEALTH);
}

void Unit::SetHealth(uint32_t health)
{
    SetUInt32Value(UNIT_FIELD_HEALTH, health);
}

void Unit::SetMaxHealth(uint32_t maxHealth)
{
    // unit has to have at least 1 health point
    if (maxHealth == 0)
        maxHealth = 1;

    SetUInt32Value(UNIT_FIELD_MAXHEALTH, maxHealth);
}

uint32_t Unit::GetFaction()
{
    return GetUInt32Value(UNIT_FIELD_FACTION);
}

void Unit::SetFaction(uint32_t faction)
{
    SetUInt32Value(UNIT_FIELD_FACTION, faction);
}
