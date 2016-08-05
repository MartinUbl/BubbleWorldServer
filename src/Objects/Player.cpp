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
#include "Player.h"
#include "SmartPacket.h"
#include "Session.h"
#include "DatabaseConnection.h"
#include "Log.h"

Player::Player() : Unit(OTYPE_PLAYER)
{
    //
}

Player::~Player()
{
    //
}

void Player::Create(uint32_t guidLow, Session* session)
{
    Unit::Create(MAKE_GUID64(HIGHGUID_PLAYER, 0, guidLow));

    m_session = session;

    // set default image
    SetUInt32Value(OBJECT_FIELD_IMAGEID, 4);
}

void Player::Update()
{
    Unit::Update();
}

void Player::CreateUpdateFields()
{
    m_maxUpdateFieldIndex = PLAYER_FIELDS_END;

    m_updateFields = new uint32_t[m_maxUpdateFieldIndex];
    m_updateFieldsChangeBits = new uint32_t[1 + (m_maxUpdateFieldIndex / 32)];

    memset(m_updateFields, 0, sizeof(uint32_t) * m_maxUpdateFieldIndex);
    memset(m_updateFieldsChangeBits, 0, sizeof(uint32_t) * (1 + (m_maxUpdateFieldIndex / 32)));

    Unit::CreateUpdateFields();
}

Session* Player::GetSession()
{
    return m_session;
}

void Player::SendPacketToMe(SmartPacket &pkt)
{
    m_session->SendPacket(pkt);
}

bool Player::LoadFromDB()
{
    DBResult res = sMainDatabase.PQuery("SELECT name, level, position_map, position_x, position_y FROM characters WHERE guid = %u", GetGUIDLow());
    if (!res.FetchRow())
    {
        sLog->Error("Could not load player with GUID %u from database - no row selected", GetGUIDLow());
        return false;
    }

    SetName(res.GetString(0).c_str());
    SetLevel(res.GetUInt32(1), true);
    SetInitialPositionAfterLoad(res.GetUInt32(2), res.GetFloat(3), res.GetFloat(4));

    return true;
}

void Player::SaveToDB()
{
    // TODO: transactions

    sMainDatabase.PExecute("UPDATE characters SET level = %u, position_map = %u, position_x = %f, position_y = %f WHERE guid = %u",
        GetLevel(), m_positionMap, m_position.x, m_position.y, GetGUIDLow());
}
