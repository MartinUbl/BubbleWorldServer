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
#include "ResourceStorage.h"

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
    // if some of updatefields was changed, send update to sorroundings
    if (m_updateFieldsNeedsUpdate)
    {
        SmartPacket pkt(SP_UPDATE_OBJECT);
        BuildUpdatePacketBlock(pkt);
        SendPacketToSorroundings(pkt);

        m_updateFieldsNeedsUpdate = false;
    }
}

void WorldObject::BuildUpdatePacketBlock(SmartPacket &pkt)
{
    // write GUID to allow client to identify one specific object
    pkt.WriteUInt64(GetGUID());

    uint16_t pos = pkt.GetWritePos();
    // placeholder for count
    pkt.WriteUInt8(0);

    uint8_t fieldCount = 0;

    // go through all updatefields, determine if they need to be updated, and send those who do
    for (uint32_t pos = 0; pos < m_maxUpdateFieldIndex; pos++)
    {
        if ((m_updateFieldsChangeBits[pos / 32] & (1 << (pos % 32))) != 0)
        {
            // write field that needs update
            pkt.WriteUInt32(pos);
            pkt.WriteUInt32(m_updateFields[pos]);

            // clear that bit
            m_updateFieldsChangeBits[pos / 32] &= ~(1 << (pos % 32));

            // increase count to be written to packet
            fieldCount++;
        }
    }

    // finally write field count
    pkt.WriteUInt8At(fieldCount, pos);
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
    m_updateFieldsNeedsUpdate = true;
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

uint32_t WorldObject::GetImageId()
{
    return GetUInt32Value(OBJECT_FIELD_IMAGEID);
}

void WorldObject::SetImageId(uint32_t imageId)
{
    SetUInt32Value(OBJECT_FIELD_IMAGEID, imageId);
}

float WorldObject::GetMinimumBoxDistance(WorldObject* other)
{
    ImageResourceMetadata* meta = sResourceStorage->GetImageMetadata(GetUInt32Value(OBJECT_FIELD_IMAGEID));
    ImageResourceMetadata* objmeta = sResourceStorage->GetImageMetadata(other->GetUInt32Value(OBJECT_FIELD_IMAGEID));

    if (!meta || !objmeta)
        return GetPosition().GetDistance(other->GetPosition());

    float Ax1, Ay1, Ax2, Ay2;
    float Bx1, By1, Bx2, By2;

    Ax1 = GetPositionX() - meta->unitBaseX + meta->unitCollisionX1;
    Ay1 = GetPositionY() - meta->unitBaseY + meta->unitCollisionY1;
    Ax2 = GetPositionX() - meta->unitBaseX + meta->unitCollisionX2;
    Ay2 = GetPositionY() - meta->unitBaseY + meta->unitCollisionY2;

    Bx1 = other->GetPositionX() - objmeta->unitBaseX + objmeta->unitCollisionX1;
    By1 = other->GetPositionY() - objmeta->unitBaseY + objmeta->unitCollisionY1;
    Bx2 = other->GetPositionX() - objmeta->unitBaseX + objmeta->unitCollisionX2;
    By2 = other->GetPositionY() - objmeta->unitBaseY + objmeta->unitCollisionY2;

    // is Y projection overlapping?
    if ((Ay1 > By1 && Ay1 < By2) || (Ay2 > By1 && Ay2 < By2) || (Ay1 < By1 && Ay2 > By2))
    {
        // A on the left of B
        if (Ax2 < Bx1)
            return Bx1 - Ax2;
        // A on the right of B
        else if (Ax1 > Bx2)
            return Ax1 - Bx2;
        // A overlapping with B
        else
            return 0.0f;
    }

    // is X projection overlapping?
    if ((Ax1 > Bx1 && Ax1 < Bx2) || (Ax2 > Bx1 && Ax2 < Bx2) || (Ax1 < Bx1 && Ax2 > Bx2))
    {
        // A above B
        if (Ay2 < By1)
            return By1 - Ay2;
        // A below B
        else if (Ay1 > By2)
            return Ay1 - By2;
        // A overlapping with B (should be caught by previous condition set)
        else
            return 0.0f;
    }

    // no orthogonal projection overlap, determine quadrant and calculate euclidean distance between two points

    // on the left; mathematically 2. and 3. quadrant
    if (Ax1 < Bx1)
    {
        // A above B, 2. quadrant
        if (Ay2 < By1)
            return Position(Ax2, Ay2).GetDistance(Position(Bx1, By1));
        // A below B, 3. quadrant
        else
            return Position(Ax2, Ay1).GetDistance(Position(Bx1, By2));
    }
    // on the right; mathematically 1. and 4. quadrant
    else
    {
        // A above B, 1. quadrant
        if (Ay2 < By1)
            return Position(Ax1, Ay2).GetDistance(Position(Bx2, By1));
        // A below B, 4. quadrant
        else
            return Position(Ax1, Ay1).GetDistance(Position(Bx2, By2));
    }
}
