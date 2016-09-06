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
#include "ItemStorage.h"
#include "ObjectAccessor.h"

Player::Player() : Unit(OTYPE_PLAYER)
{
    memset(m_inventory, 0, sizeof(InventoryItem*)*CHARACTER_INVENTORY_SLOTS);
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

void Player::FillInventoryPacket(SmartPacket &pkt)
{
    InventoryItem* item;

    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS; i++)
    {
        item = m_inventory[i];
        // no item - write 0 into guid field, the client won't read any further fields related to this slot
        if (!item)
            pkt.WriteUInt32(0);
        else
        {
            pkt.WriteUInt32(item->item.itemGuid);
            pkt.WriteUInt32(item->item.itemId);
            pkt.WriteUInt32(item->item.stackCount);
        }
    }
}

void Player::SendInventorySlotUpdate(uint32_t slot)
{
    if (slot >= CHARACTER_INVENTORY_SLOTS)
        return;

    InventoryItem* item = m_inventory[slot];

    SmartPacket pkt(SP_UPDATE_INVENTORY_SLOT);
    pkt.WriteUInt32(slot);
    // no item - write 0 into guid field, the client won't read any further fields related to this slot, and will delete slot contents if any
    if (!item)
        pkt.WriteUInt32(0);
    else
    {
        pkt.WriteUInt32(item->item.itemGuid);
        pkt.WriteUInt32(item->item.itemId);
        pkt.WriteUInt32(item->item.stackCount);
    }

    SendPacketToMe(pkt);
}

void Player::SendInventoryItemOperation(uint32_t itemId, ItemInventoryOperation operation, uint32_t count)
{
    SmartPacket pkt(SP_ITEM_OPERATION_INFO);
    pkt.WriteUInt8(operation);
    pkt.WriteUInt32(itemId);
    pkt.WriteUInt32(count);
    SendPacketToMe(pkt);
}

void Player::CreateItemInInventory(uint32_t id, uint32_t count, ItemInventoryOperation operation)
{
    InventoryItem* item;

    ItemTemplateRecord* irec = sItemStorage->GetItemTemplate(id);
    if (!irec || !count)
        return;

    // check available space in inventory
    uint32_t availableSpace = 0;
    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS && availableSpace < count; i++)
    {
        item = m_inventory[i];
        if (!item)
            availableSpace += irec->stackSize;
        else if (item->item.itemId == id && item->item.stackCount < irec->stackSize)
            availableSpace += irec->stackSize - item->item.stackCount;
    }

    // if no space, sorry
    if (availableSpace < count)
    {
        sLog->Error("Could not create %u of item %u - not enough space in inventory", count, id);
        return;
    }

    // now we can be sure there's enough room for items in inventory and we could skip the checks

    uint32_t tmpCount = count;
    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS && tmpCount != 0; i++)
    {
        item = m_inventory[i];
        // if there's no item in slot, create it and fill with item requested
        if (!item)
        {
            item = new InventoryItem();
            item->itemStatus = ITEM_NEW;
            item->slot = i;
            item->item.itemGuid = sObjectAccessor->AllocateItemGUID();
            item->item.itemId = id;
            item->item.stackCount = num_min(irec->stackSize, tmpCount);
            item->item.ownerGuid = GetGUIDLow();

            tmpCount -= item->item.stackCount;
            m_inventory[i] = item;
            SendInventorySlotUpdate(i);
        }
        else if (item->item.itemId == id && item->item.stackCount < irec->stackSize)
        {
            // if the slot is capable of containing more or equal items of this type...
            if (irec->stackSize - item->item.stackCount >= tmpCount)
            {
                // add appropriate count and end it
                item->item.stackCount += tmpCount;
                tmpCount = 0;
            }
            else
            {
                // subtract the amount remaining to full stack and max up the stack size
                tmpCount -= irec->stackSize - item->item.stackCount;
                item->item.stackCount = irec->stackSize;
            }

            item->itemStatus = ITEM_CHANGED;

            SendInventorySlotUpdate(i);
        }
    }

    if (operation != ITEM_OP_NONE)
        SendInventoryItemOperation(id, operation, count);
}

void Player::RemoveItemFromInventory(uint32_t id, uint32_t count, bool destroy, ItemInventoryOperation operation)
{
    uint32_t tmpCount = GetItemCount(id);
    InventoryItem* item;

    // do not allow deleting count lower than requested
    if (tmpCount < count)
    {
        sLog->Error("Attempt to destroy %u of item %u, despite having just %u", count, id, tmpCount);
        return;
    }

    // go through inventory and destroy requested count
    tmpCount = count;
    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS && tmpCount > 0; i++)
    {
        item = m_inventory[i];
        if (!item || item->item.itemId != id)
            continue;

        if (item->item.stackCount > tmpCount)
        {
            item->item.stackCount -= tmpCount;
            tmpCount = 0;
            SendInventorySlotUpdate(item->slot);
            break;
        }
        
        tmpCount -= item->item.stackCount;
        if (destroy)
            DestroyInventorySlot(item->slot);
        else
        {
            m_inventory[item->slot] = nullptr;
            SendInventorySlotUpdate(item->slot);
        }
    }

    if (operation != ITEM_OP_NONE)
        SendInventoryItemOperation(id, operation, count);
}

uint32_t Player::GetItemCount(uint32_t id)
{
    InventoryItem* item;
    uint32_t count = 0;

    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS; i++)
    {
        item = m_inventory[i];
        if (!item || item->item.itemId != id)
            continue;

        count += item->item.stackCount;
    }

    return count;
}

void Player::SwapInventorySlots(uint32_t srcSlot, uint32_t dstSlot)
{
    if (srcSlot >= CHARACTER_INVENTORY_SLOTS || dstSlot >= CHARACTER_INVENTORY_SLOTS)
        return;

    InventoryItem* src = m_inventory[srcSlot];
    InventoryItem* dst = m_inventory[dstSlot];

    m_inventory[srcSlot] = dst;
    m_inventory[dstSlot] = src;

    if (src)
        src->slot = dstSlot;
    if (dst)
        dst->slot = srcSlot;

    SendInventorySlotUpdate(srcSlot);
    SendInventorySlotUpdate(dstSlot);
}

void Player::DestroyInventorySlot(uint32_t slot)
{
    if (slot >= CHARACTER_INVENTORY_SLOTS || !m_inventory[slot])
        return;

    InventoryItem* item = m_inventory[slot];

    m_inventory[slot] = nullptr;
    SendInventorySlotUpdate(slot);

    item->itemStatus = ITEM_DELETED;
    item->slot = CHARACTER_INVENTORY_SLOT_DELETED;
    m_pendingDeleteItems.push_back(item);
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

    LoadInventory();

    return true;
}

void Player::SaveToDB()
{
    // TODO: transactions

    sMainDatabase.PExecute("UPDATE characters SET level = %u, position_map = %u, position_x = %f, position_y = %f WHERE guid = %u",
        GetLevel(), m_positionMap, m_position.x, m_position.y, GetGUIDLow());

    SaveInventory();
}

void Player::LoadInventory()
{
    uint32_t slot;
    InventoryItem* item;

    DBResult res = sMainDatabase.PQuery("SELECT ci.item_guid, ci.slot, ii.id, ii.owner_guid, ii.stack_count FROM character_inventory AS ci RIGHT JOIN item AS ii ON ci.item_guid = ii.guid WHERE ci.guid = %u", GetGUIDLow());
    while (res.FetchRow())
    {
        slot = res.GetUInt32(1);
        item = new InventoryItem();

        item->itemStatus = ITEM_UNCHANGED;
        item->slot = slot;

        item->item.itemGuid = res.GetUInt32(0);
        item->item.itemId = res.GetUInt32(2);
        item->item.ownerGuid = res.GetUInt32(3);
        item->item.stackCount = res.GetUInt32(4);

        m_inventory[slot] = item;
    }
}

void Player::SaveInventory()
{
    InventoryItem* item;

    for (uint32_t i = 0; i < CHARACTER_INVENTORY_SLOTS; i++)
    {
        item = m_inventory[i];
        if (!item || item->itemStatus == ITEM_UNCHANGED)
            continue;

        // new item requires creating records
        if (item->itemStatus == ITEM_NEW)
        {
            sMainDatabase.PExecute("INSERT INTO item (guid, id, owner_guid, stack_count) VALUES (%u, %u, %u, %u)", item->item.itemGuid, item->item.itemId, item->item.ownerGuid, item->item.stackCount);
            sMainDatabase.PExecute("INSERT INTO character_inventory (item_guid, guid, slot) VALUES (%u, %u, %u)", item->item.itemGuid, GetGUIDLow(), item->slot);
        }
        // changed item just needs update
        else if (item->itemStatus == ITEM_CHANGED)
        {
            sMainDatabase.PExecute("UPDATE item SET id = %u, owner_guid = %u, stack_count = %u WHERE guid = %u", item->item.itemId, item->item.ownerGuid, item->item.stackCount, item->item.itemGuid);
            sMainDatabase.PExecute("UPDATE character_inventory SET guid = %u, slot = %u WHERE item_guid = %u", GetGUIDLow(), item->slot, item->item.itemGuid);
        }
        // no other status should get here
        else
        {
            sLog->Error("Item %u is in invalid state (%u) whilst still in inventory, not saving", item->item.itemGuid, item->itemStatus);
            item->itemStatus = ITEM_UNCHANGED;
            continue;
        }

        item->itemStatus = ITEM_UNCHANGED;
    }

    // this will also delete any items pending to be deleted
    for (InventoryItem* item : m_pendingDeleteItems)
    {
        sMainDatabase.PExecute("DELETE FROM character_inventory WHERE guid = %u AND item_guid = %u", item->item.ownerGuid, item->item.itemGuid);
        sMainDatabase.PExecute("DELETE FROM item WHERE guid = %u", item->item.itemGuid);

        delete item;
    }
}
