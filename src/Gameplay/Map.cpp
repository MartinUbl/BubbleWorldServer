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
#include "Map.h"
#include "SmartPacket.h"
#include "Player.h"
#include "Creature.h"
#include "Gameobject.h"
#include "MapManager.h"
#include "MapStorage.h"
#include "CreatureStorage.h"
#include "GameobjectStorage.h"

Map::Map(uint32_t id, MapRecord* mapTemplate) : m_mapId(id), m_storedMapRecord(mapTemplate)
{
    //
}

Map::~Map()
{
    //
}

void Map::InitContents()
{
    // no map record = no contents
    if (!m_storedMapRecord)
        return;

    uint32_t csizeX = GetCellIndexX((float)m_storedMapRecord->header.sizeX) + 1;
    uint32_t csizeY = GetCellIndexY((float)m_storedMapRecord->header.sizeY) + 1;

    // resize object vectors
    m_objects.resize(csizeX);
    for (uint32_t i = 0; i < csizeX; i++)
        m_objects[i].resize(csizeY);

    // retrieve creature spawns
    CreatureSpawnList crList;
    sCreatureStorage->GetCreatureSpawnsForMap(m_mapId, crList);

    // put creatures on map
    Creature* cr;
    for (CreatureSpawnRecord& rec : crList)
    {
        cr = new Creature();
        cr->Create(rec.guid, rec.id);
        cr->SetInitialPositionAfterLoad(rec.positionMap, rec.positionX, rec.positionY);
        AddToMap(cr);
    }

    // retrieve gameobject spawns
    GameobjectSpawnList goList;
    sGameobjectStorage->GetGameobjectSpawnsForMap(m_mapId, goList);

    // put gameobjects on map
    Gameobject* go;
    for (GameobjectSpawnRecord& rec : goList)
    {
        go = new Gameobject();
        go->Create(rec.guid, rec.id);
        go->SetInitialPositionAfterLoad(rec.positionMap, rec.positionX, rec.positionY);
        AddToMap(go);
    }
}

void Map::AddToMap(WorldObject* obj)
{
    uint32_t cx, cy;

    // retrieve cell position
    cx = GetCellIndexX(obj->GetPositionX());
    cy = GetCellIndexY(obj->GetPositionY());

    // add object to cell
    m_objects[cx][cy].push_back(obj);

    // create object in cell
    SmartPacket pkt(SP_CREATE_OBJECT);
    pkt.WriteUInt8(1); // will create 1 object
    obj->BuildCreatePacketBlock(pkt);
    SendPacketToSorroundings(cx, cy, pkt);

    // if it's player, create sorroundings for him
    if (obj->GetType() == OTYPE_PLAYER)
        SendCreateSorroundings(obj->ToPlayer());
}

void Map::RemoveFromMap(WorldObject* obj)
{
    uint32_t cx, cy;

    // retrieve cell position
    cx = GetCellIndexX(obj->GetPositionX());
    cy = GetCellIndexY(obj->GetPositionY());

    // find him in cell and erase him
    for (WorldObjectList::iterator itr = m_objects[cx][cy].begin(); itr != m_objects[cx][cy].end(); ++itr)
    {
        if (*itr == obj)
        {
            m_objects[cx][cy].erase(itr);
            break;
        }
    }

    // destroy object for objects in his cell
    SmartPacket pkt(SP_DESTROY_OBJECT);
    pkt.WriteUInt8(1); // will destroy 1 object
    pkt.WriteUInt64(obj->GetGUID());
    SendPacketToSorroundings(cx, cy, pkt);
}

void Map::Relocate(WorldObject* obj, float oldX, float oldY, float newX, float newY)
{
    // retrieve old coordinates
    uint32_t cellX_old, cellY_old, cellX_new, cellY_new;
    cellX_old = GetCellIndexX(oldX);
    cellY_old = GetCellIndexY(oldY);
    cellX_new = GetCellIndexX(newX);
    cellY_new = GetCellIndexY(newY);

    // no need for cell relocation
    if (cellX_old == cellX_new && cellY_old == cellY_new)
        return;

    uint32_t itX, itY;

    // old sorrounding limits
    uint32_t beginX, endX, beginY, endY;
    GetCellSorroundingLimits(cellX_old, cellY_old, beginX, beginY, endX, endY);

    // new sorrounding limits
    uint32_t beginX_n, endX_n, beginY_n, endY_n;
    GetCellSorroundingLimits(cellX_new, cellY_new, beginX_n, beginY_n, endX_n, endY_n);

    for (itX = beginX; itX <= endX; itX++)
    {
        for (itY = beginY; itY <= endY; itY++)
        {
            // these coordinates are in the new cell sorroundings, no need to update
            if (itX >= beginX_n && itX <= endX_n && itY >= beginY_n && itY <= endY_n)
                continue;

            // leaving sorrounding, destroy player in that cell and destroy objects for them
            SendCellDeletePacketsFor(itX, itY, obj);
        }
    }

    for (itX = beginX_n; itX <= endX_n; itX++)
    {
        for (itY = beginY_n; itY <= endY_n; itY++)
        {
            // these coordinates are in the old cell sorroundings, no need to update
            if (itX >= beginX && itX <= endX && itY >= beginY && itY <= endY)
                continue;

            // entering new sorrounding, create player in that cell and send him objects here
            SendCellCreatePacketsFor(itX, itY, obj);
        }
    }
}

void Map::SendCellCreatePacketsFor(uint32_t cellX, uint32_t cellY, WorldObject* wobj)
{
    // create player in that cell
    SmartPacket cpkt(SP_CREATE_OBJECT);
    cpkt.WriteUInt8(1); // will create 1 object
    wobj->BuildCreatePacketBlock(cpkt);
    SendPacketToCell(cellX, cellY, cpkt);

    // for non-players, we are done
    if (wobj->GetType() != OTYPE_PLAYER)
        return;

    Player* plr = wobj->ToPlayer();

    // send objects in that cell for player
    SmartPacket* pkt = nullptr;
    uint32_t counter = UPDATEPACKET_COUNT_LIMIT;

    for (WorldObject *obj : m_objects[cellX][cellY])
    {
        if (counter >= UPDATEPACKET_COUNT_LIMIT)
        {
            if (pkt)
            {
                pkt->WriteUInt8At(counter, 0);
                plr->SendPacketToMe(*pkt);
            }
            else
            {
                pkt = new SmartPacket(SP_CREATE_OBJECT);
                pkt->WriteUInt8(0); // placeholder
            }

            pkt->ResetData();
            counter = 0;
        }

        obj->BuildCreatePacketBlock(*pkt);
        counter++;
    }

    if (pkt)
    {
        if (counter > 0)
        {
            pkt->WriteUInt8At(counter, 0);
            plr->SendPacketToMe(*pkt);
        }

        delete pkt;
    }
}

void Map::SendCellDeletePacketsFor(uint32_t cellX, uint32_t cellY, WorldObject* wobj)
{
    // destroy player in that cell
    SmartPacket cpkt(SP_DESTROY_OBJECT);
    cpkt.WriteUInt8(1); // will create 1 object
    cpkt.WriteUInt64(wobj->GetGUID());
    SendPacketToCell(cellX, cellY, cpkt);

    // for non-players, we are done
    if (wobj->GetType() != OTYPE_PLAYER)
        return;

    Player* plr = wobj->ToPlayer();

    // destroy objects in that cell for player
    SmartPacket* pkt = nullptr;
    uint32_t counter = UPDATEPACKET_COUNT_LIMIT;

    for (WorldObject *obj : m_objects[cellX][cellY])
    {
        if (counter >= UPDATEPACKET_COUNT_LIMIT)
        {
            if (pkt)
            {
                pkt->WriteUInt8At(counter, 0);
                plr->SendPacketToMe(*pkt);
            }
            else
            {
                pkt = new SmartPacket(SP_DESTROY_OBJECT);
                pkt->WriteUInt8(0); // placeholder
            }

            pkt->ResetData();
            counter = 0;
        }

        pkt->WriteUInt64(obj->GetGUID());
        counter++;
    }

    if (pkt)
    {
        if (counter > 0)
        {
            pkt->WriteUInt8At(counter, 0);
            plr->SendPacketToMe(*pkt);
        }

        delete pkt;
    }
}

uint32_t Map::GetMapID()
{
    return m_mapId;
}

uint32_t Map::GetCellIndexX(float fieldX)
{
    return (uint32_t)fieldX / MAP_CELL_SIZE_X;
}

uint32_t Map::GetCellIndexY(float fieldY)
{
    return (uint32_t)fieldY / MAP_CELL_SIZE_Y;
}

uint32_t Map::GetCellStartX(uint32_t indexX)
{
    return indexX * MAP_CELL_SIZE_X;
}

uint32_t Map::GetCellStartY(uint32_t indexY)
{
    return indexY * MAP_CELL_SIZE_Y;
}

MapField* Map::GetField(float x, float y)
{
    uint32_t iX = GetCellIndexX(x);
    uint32_t iY = GetCellIndexY(y);

    if (m_storedMapRecord->chunks.size() <= iX)
        return nullptr;
    if (m_storedMapRecord->chunks[iX].size() <= iY)
        return nullptr;

    MapChunkRecord* target = &m_storedMapRecord->chunks[iX][iY];

    iX = target->GetFieldOffsetX((uint32_t)x);
    iY = target->GetFieldOffsetY((uint32_t)y);

    return &target->fields[iX][iY];
}

void Map::GetCellSorroundingLimits(uint32_t cellX, uint32_t cellY, uint32_t &beginX, uint32_t &beginY, uint32_t &endX, uint32_t &endY)
{
    beginX = cellX > MAP_SORROUNDING_CELLS_X ? cellX - MAP_SORROUNDING_CELLS_X : 0;
    beginY = cellY > MAP_SORROUNDING_CELLS_Y ? cellY - MAP_SORROUNDING_CELLS_Y : 0;

    endX = cellX < (uint32_t)m_objects.size() - 1 - MAP_SORROUNDING_CELLS_X ? cellX + MAP_SORROUNDING_CELLS_X : (uint32_t)m_objects.size() - 1;
    endY = cellY < (uint32_t)m_objects[0].size() - 1 - MAP_SORROUNDING_CELLS_Y ? cellY + MAP_SORROUNDING_CELLS_Y : (uint32_t)m_objects[0].size() - 1;
}

void Map::SendPacketToCell(uint32_t cellX, uint32_t cellY, SmartPacket &pkt)
{
    for (WorldObject *obj : m_objects[cellX][cellY])
    {
        // send only to players, nobody else would ever retrieve that packet
        if (obj->GetType() == OTYPE_PLAYER)
            obj->ToPlayer()->SendPacketToMe(pkt);
    }
}

void Map::SendPacketToSorroundings(uint32_t cellX, uint32_t cellY, SmartPacket &pkt)
{
    uint32_t beginX, endX, beginY, endY, itY;

    GetCellSorroundingLimits(cellX, cellY, beginX, beginY, endX, endY);

    for (; beginX <= endX; beginX++)
        for (itY = beginY; itY <= endY; itY++)
            SendPacketToCell(beginX, itY, pkt);
}

void Map::SendPacketToSorroundings(float x, float y, SmartPacket &pkt)
{
    SendPacketToSorroundings(GetCellIndexX(x), GetCellIndexY(y), pkt);
}

void Map::SendCreateSorroundings(Player* plr)
{
    uint32_t cellX = GetCellIndexX(plr->GetPositionX());
    uint32_t cellY = GetCellIndexY(plr->GetPositionY());
    uint32_t beginX, endX, beginY, endY, itY;

    // retrieve sorroundings
    GetCellSorroundingLimits(cellX, cellY, beginX, beginY, endX, endY);

    SmartPacket* pkt = nullptr;
    uint32_t counter = UPDATEPACKET_COUNT_LIMIT;

    // create contents of each cell in sorroundings
    for (; beginX <= endX; beginX++)
    {
        for (itY = beginY; itY <= endY; itY++)
        {
            for (WorldObject *obj : m_objects[beginX][itY])
            {
                // split to multiple packets if needed (if exceeds limit)
                if (counter >= UPDATEPACKET_COUNT_LIMIT)
                {
                    if (pkt)
                    {
                        pkt->WriteUInt8At(counter, 0);
                        plr->SendPacketToMe(*pkt);
                        pkt->ResetData();
                    }
                    else
                    {
                        pkt = new SmartPacket(SP_CREATE_OBJECT);
                        pkt->WriteUInt8(0); // placeholder
                    }

                    counter = 0;
                }

                obj->BuildCreatePacketBlock(*pkt);
                counter++;
            }
        }
    }

    if (pkt)
    {
        if (counter > 0)
        {
            pkt->WriteUInt8At(counter, 0);
            plr->SendPacketToMe(*pkt);
        }

        delete pkt;
    }
}

void Map::Update()
{
    uint32_t cx, cy;
    // TODO: manage and update only active cells

    for (cx = 0; cx < m_objects.size(); cx++)
    {
        for (cy = 0; cy < m_objects[cx].size(); cy++)
        {
            for (WorldObject* obj : m_objects[cx][cy])
                obj->Update();
        }
    }
}
