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
#include "ObjectAccessor.h"
#include "Log.h"
#include "WorldObject.h"
#include "DatabaseConnection.h"

// guid map granularities - how many "bits" should be allocated at once

#define GUIDMAP_GRANULARITY_CREATURE 1024
#define GUIDMAP_GRANULARITY_GAMEOBJECT 1024
#define GUIDMAP_GRANULARITY_PLAYER 1024
#define GUIDMAP_GRANULARITY_ITEM 8092

ObjectAccessor::ObjectAccessor()
{
    //
}

ObjectAccessor::~ObjectAccessor()
{
    //
}

void ObjectAccessor::InitGUIDMaps()
{
    sLog->Info(">> Initializing guidspace");

    // at first, initialize guidmaps with granularities, to allow slices to be allocated, etc.
    m_guidMap[GUIDMAP_CREATURE].Init(GUIDMAP_GRANULARITY_CREATURE);
    m_guidMap[GUIDMAP_GAMEOBJECT].Init(GUIDMAP_GRANULARITY_GAMEOBJECT);
    m_guidMap[GUIDMAP_PLAYER].Init(GUIDMAP_GRANULARITY_PLAYER);
    m_guidMap[GUIDMAP_ITEM].Init(GUIDMAP_GRANULARITY_ITEM);

    // then go through existing records in database and set occupied GUIDs so they won't be assigned again

    DBResult res;

    res = sMainDatabase.PQuery("SELECT guid FROM creature");
    while (res.FetchRow())
        m_guidMap[GUIDMAP_CREATURE].SetBit(res.GetUInt32(0));

    res = sMainDatabase.PQuery("SELECT guid FROM gameobject");
    while (res.FetchRow())
        m_guidMap[GUIDMAP_GAMEOBJECT].SetBit(res.GetUInt32(0));

    res = sMainDatabase.PQuery("SELECT guid FROM characters");
    while (res.FetchRow())
        m_guidMap[GUIDMAP_PLAYER].SetBit(res.GetUInt32(0));

    res = sMainDatabase.PQuery("SELECT guid FROM item");
    while (res.FetchRow())
        m_guidMap[GUIDMAP_ITEM].SetBit(res.GetUInt32(0));

    sLog->Info("Guidspace successfully initialized");
}

void ObjectAccessor::AddObject(uint64_t guid, WorldObject* obj)
{
    // do not allow to store same GUID twice
    if (m_objectMap.find(guid) != m_objectMap.end())
    {
        sLog->Error("Attempted to add object with the same guid (%llu) more than once!", guid);
        return;
    }

    m_objectMap[guid] = obj;
}

void ObjectAccessor::RemoveObject(uint64_t guid)
{
    if (m_objectMap.find(guid) != m_objectMap.end())
        m_objectMap.erase(guid);
}

WorldObject* ObjectAccessor::FindWorldObject(uint64_t guid)
{
    if (m_objectMap.find(guid) == m_objectMap.end())
        return nullptr;

    return m_objectMap[guid];
}

uint64_t ObjectAccessor::AllocateCreatureGUID(uint32_t entry)
{
    int64_t guid = m_guidMap[GUIDMAP_CREATURE].UseEmpty();
    if (guid == -1)
    {
        sLog->Error("Could not allocate next creature GUID!");
        // TODO: gently terminate server
        return 0;
    }

    return MAKE_GUID64(HIGHGUID_CREATURE, entry, guid);
}

uint64_t ObjectAccessor::AllocateGameobjectGUID(uint32_t entry)
{
    int64_t guid = m_guidMap[GUIDMAP_GAMEOBJECT].UseEmpty();
    if (guid == -1)
    {
        sLog->Error("Could not allocate next gameobject GUID!");
        // TODO: gently terminate server
        return 0;
    }

    return MAKE_GUID64(HIGHGUID_GAMEOBJECT, entry, guid);
}

uint64_t ObjectAccessor::AllocatePlayerGUID()
{
    int64_t guid = m_guidMap[GUIDMAP_PLAYER].UseEmpty();
    if (guid == -1)
    {
        sLog->Error("Could not allocate next player GUID!");
        // TODO: gently terminate server
        return 0;
    }

    return MAKE_GUID64(HIGHGUID_PLAYER, 0, guid);
}

uint32_t ObjectAccessor::AllocateItemGUID()
{
    int64_t guid = m_guidMap[GUIDMAP_ITEM].UseEmpty();
    // limit item GUIDs to uint32 maximum
    if (guid == -1 || guid >= UINT32_MAX)
    {
        sLog->Error("Could not allocate next item GUID!");
        // TODO: gently terminate server
        return 0;
    }

    return (uint32_t)guid;
}
