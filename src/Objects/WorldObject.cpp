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
#include "WorldObject.h"
#include "MapManager.h"
#include "Unit.h"
#include "Player.h"
#include "Creature.h"
#include "SmartPacket.h"
#include "ObjectAccessor.h"

WorldObject::WorldObject(ObjectType type) : m_position(0.0f, 0.0f), m_positionMap(0), m_objectType(type)
{
    m_updateFieldsNeedsUpdate = true;
    m_name = "???";
}

WorldObject::~WorldObject()
{
    //
}

void WorldObject::Create(uint64_t guid)
{
    CreateUpdateFields();

    SetUInt64Value(OBJECT_FIELD_GUID, guid);

    sObjectAccessor->AddObject(guid, this);

    // TODO: removing object from ObjectAccessor
}

void WorldObject::Update()
{
    //
}

void WorldObject::BuildCreatePacketBlock(SmartPacket &pkt)
{
    // write GUID (although it's in updatefields, we need the client
    // to be able to create the object before setting fields)
    pkt.WriteUInt64(GetGUID());

    // field count
    pkt.WriteUInt32(m_maxUpdateFieldIndex);

    // field contents
    for (uint32_t pos = 0; pos < m_maxUpdateFieldIndex; pos++)
        pkt.WriteUInt32(m_updateFields[pos]);
}

uint64_t WorldObject::GetGUID()
{
    return GetUInt64Value(OBJECT_FIELD_GUID);
}

uint32_t WorldObject::GetEntry()
{
    return EXTRACT_ENTRY(GetGUID());
}

uint32_t WorldObject::GetGUIDLow()
{
    return EXTRACT_GUIDLOW(GetGUID());
}

ObjectType WorldObject::GetType()
{
    return m_objectType;
}

Unit* WorldObject::ToUnit()
{
    return dynamic_cast<Unit*>(this);
}

Player* WorldObject::ToPlayer()
{
    return dynamic_cast<Player*>(this);
}

Creature* WorldObject::ToCreature()
{
    return dynamic_cast<Creature*>(this);
}

void WorldObject::SetUpdateFieldUpdateNeeded(uint32_t field)
{
    m_updateFieldsChangeBits[field / 32] |= 1 << (field % 32);
}

void WorldObject::SendPacketToSorroundings(SmartPacket &pkt)
{
    Map* m = sMapManager->GetMap(GetMapId());
    if (m)
        m->SendPacketToSorroundings(GetPositionX(), GetPositionY(), pkt);
}

void WorldObject::SetUInt32Value(uint32_t field, uint32_t value)
{
    if (m_updateFields[field] == value)
        return;

    m_updateFields[field] = value;
    SetUpdateFieldUpdateNeeded(field);
}

void WorldObject::SetUInt64Value(uint32_t field, uint64_t value)
{
    SetUInt32Value(field, value & 0xFFFFFFFF);
    SetUInt32Value(field + 1, (value >> 32LL) & 0xFFFFFFFF);
}

void WorldObject::SetUByteValue(uint32_t field, uint8_t offset, uint8_t value)
{
    // allowed offsets are 0-3, as there are 4 bytes in uint32_t
    if (offset > 3)
        return;

    if (m_updateFields[field] >> (offset * 8) == value)
        return;

    m_updateFields[field] = m_updateFields[field] & (~(0xFF << (offset * 8))) | (value << (offset * 8));
    SetUpdateFieldUpdateNeeded(field);
}

void WorldObject::SetInt32Value(uint32_t field, int32_t value)
{
    SetUInt32Value(field, (uint32_t)value);
}

void WorldObject::SetInt64Value(uint32_t field, int64_t value)
{
    SetUInt64Value(field, (uint64_t)value);
}

void WorldObject::SetByteValue(uint32_t field, uint8_t offset, int8_t value)
{
    SetUByteValue(field, offset, (uint8_t)value);
}

void WorldObject::SetFloatValue(uint32_t field, float value)
{
    SetUInt32Value(field, *((uint32_t*)(&value)));
}

uint32_t WorldObject::GetUInt32Value(uint32_t field)
{
    return m_updateFields[field];
}

uint64_t WorldObject::GetUInt64Value(uint32_t field)
{
    return ((uint64_t)m_updateFields[field]) | (((uint64_t)m_updateFields[field + 1]) << 32LL);
}

uint8_t WorldObject::GetUByteValue(uint32_t field, uint8_t offset)
{
    return m_updateFields[field] >> (offset * 8);
}

int32_t WorldObject::GetInt32Value(uint32_t field)
{
    return (int32_t)GetUInt32Value(field);
}

int64_t WorldObject::GetInt64Value(uint32_t field)
{
    return (int64_t)GetUInt64Value(field);
}

int8_t WorldObject::GetByteValue(uint32_t field, uint8_t offset)
{
    return (int8_t)GetUByteValue(field, offset);
}

float WorldObject::GetFloatValue(uint32_t field)
{
    uint32_t val = GetUInt32Value(field);
    return *((float*)(&val));
}

void WorldObject::SetPositionX(float x)
{
    m_position.x = x;
}

void WorldObject::SetPositionY(float y)
{
    m_position.y = y;
}

void WorldObject::SetPosition(float x, float y)
{
    m_position.x = x;
    m_position.y = y;
}

Position const& WorldObject::GetPosition()
{
    return m_position;
}

float WorldObject::GetPositionX()
{
    return m_position.x;
}

float WorldObject::GetPositionY()
{
    return m_position.y;
}

uint32_t WorldObject::GetMapId()
{
    return m_positionMap;
}

Map* WorldObject::GetMap()
{
    return sMapManager->GetMap(m_positionMap);
}

void WorldObject::RelocateWithinMap(float x, float y)
{
    Map* map = sMapManager->GetMap(m_positionMap);
    map->Relocate(this, m_position.x, m_position.y, x, y);

    SetPosition(x, y);
}

void WorldObject::TeleportTo(uint32_t mapId, float x, float y)
{
    if (m_positionMap == mapId)
    {
        RelocateWithinMap(x, y);
        return;
    }

    // TODO: remove from old map, add to new map
    Map* map = sMapManager->GetMap(m_positionMap);
    map->RemoveFromMap(this);

    map = sMapManager->GetMap(mapId);
    map->AddToMap(this);
    RelocateWithinMap(x, y);
}

void WorldObject::SetInitialPositionAfterLoad(uint32_t mapId, float x, float y)
{
    m_positionMap = mapId;
    SetPosition(x, y);
}

void WorldObject::CreateUpdateFields()
{
    // updatefields are created in child classes
    // WorldObject itself does not have any extra fields
}

void WorldObject::SetName(const char* name)
{
    if (name != nullptr)
        m_name = name;
}

const char* WorldObject::GetName()
{
    return m_name.c_str();
}
