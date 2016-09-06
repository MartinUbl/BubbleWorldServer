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

#ifndef BW_PLAYER_H
#define BW_PLAYER_H

#include "Unit.h"

// enumerator of item statuses
enum ItemInventoryStatus
{
    ITEM_NEW = 1,               // item was created in memory, but is not saved to database
    ITEM_UNCHANGED = 2,         // item status in memory and in database is consistent
    ITEM_CHANGED = 3,           // item needs update in database
    ITEM_DELETED = 4,           // item was deleted and needs to be deleted in DB also
};

/*
 * Structure containing data about one item instance (generic, not just in inventory)
 */
struct ItemInstance
{
    // item GUID
    uint32_t itemGuid;
    // item template ID
    uint32_t itemId;
    // GUID of owner (if any)
    uint64_t ownerGuid;
    // item count in one stack (current)
    uint32_t stackCount;
};

/*
 * Structure containing data about item in inventory
 */
struct InventoryItem
{
    // inventory slot
    uint32_t slot;
    // item status (needs update, etc.)
    ItemInventoryStatus itemStatus;
    // item instance record
    ItemInstance item;
};

// maximum number of slots the character could have in his inventory
#define CHARACTER_INVENTORY_SLOTS 100
// slot which is set after deletion, no physical meaning, just musn't conflict with valid slots
#define CHARACTER_INVENTORY_SLOT_DELETED (CHARACTER_INVENTORY_SLOTS+1)

class Session;
class SmartPacket;

/*
 * Class representing player object in game
 */
class Player : public Unit
{
    public:
        Player();
        virtual ~Player();

        virtual void Create(uint32_t guidLow, Session* session);
        virtual void Update();

        // loads player from DB
        bool LoadFromDB();
        // saves player to DB
        void SaveToDB();

        // retrieves player session
        Session* GetSession();
        // sends packet to this player
        void SendPacketToMe(SmartPacket &pkt);

        // fills prepared inventory packet with full inventory data
        void FillInventoryPacket(SmartPacket &pkt);
        // sends inventory slot update packet
        void SendInventorySlotUpdate(uint32_t slot);
        // sends message about item creation
        void SendInventoryItemOperation(uint32_t itemId, ItemInventoryOperation operation, uint32_t count = 1);
        // creates specific item in inventory
        void CreateItemInInventory(uint32_t id, uint32_t count = 1, ItemInventoryOperation operation = ITEM_INV_CREATE_OBTAIN);
        // removes item from inventory (if there is any)
        void RemoveItemFromInventory(uint32_t id, uint32_t count, bool destroy = true, ItemInventoryOperation operation = ITEM_INV_REMOVE_DESTROY);
        // retrieves specific item count within inventory slots
        uint32_t GetItemCount(uint32_t id);
        // swaps two inventory slots (even works with empty slots)
        void SwapInventorySlots(uint32_t srcSlot, uint32_t dstSlot);
        // destroys item in specified slot
        void DestroyInventorySlot(uint32_t slot);

    protected:
        virtual void CreateUpdateFields();

        // loads inventory from database
        void LoadInventory();
        // saves inventory to database
        void SaveInventory();

    private:
        // player session
        Session* m_session;
        // player inventory
        InventoryItem* m_inventory[CHARACTER_INVENTORY_SLOTS];
        // items to be removed at next inventory save
        std::list<InventoryItem*> m_pendingDeleteItems;
};

#endif