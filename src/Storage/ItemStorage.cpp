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
#include "ItemStorage.h"
#include "DatabaseConnection.h"
#include "Log.h"

ItemStorage::ItemStorage()
{
    //
}

ItemStorage::~ItemStorage()
{
    //
}

void ItemStorage::LoadFromDB()
{
    uint32_t itemId, count;

    sLog->Info(">> Loading item template data...");
    DBResult res = sMainDatabase.PQuery("SELECT id, image_id, name, description, stack_size, rarity FROM item_template");

    m_itemTemplates.clear();
    count = 0;
    while (res.FetchRow())
    {
        itemId = res.GetUInt32(0);

        m_itemTemplates[itemId] = ItemTemplateRecord(
            itemId,
            res.GetUInt32(1),
            res.GetString(2).c_str(),
            res.GetString(3).c_str(),
            res.GetUInt32(4),
            res.GetUInt32(5)
        );

        count++;
    }

    sLog->Info("Loaded %u item templates", count);
}

ItemTemplateRecord* ItemStorage::GetItemTemplate(uint32_t id)
{
    if (m_itemTemplates.find(id) == m_itemTemplates.end())
        return nullptr;

    return &m_itemTemplates[id];
}
