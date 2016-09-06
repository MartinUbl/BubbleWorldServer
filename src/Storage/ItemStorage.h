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

#ifndef BW_ITEM_STORAGE_H
#define BW_ITEM_STORAGE_H

#include "Singleton.h"

/*
 * Structure containing item template data from database
 */
struct ItemTemplateRecord
{
    ItemTemplateRecord() : id(0) {};
    // init constructor
    ItemTemplateRecord(uint32_t _id, uint32_t _imageId, const char* _name, const char* _description, uint32_t _stackSize, uint32_t _rarity) :
        id(_id), imageId(_imageId), name(_name), description(_description), stackSize(_stackSize), rarity(_rarity)
    {
    }

    // item ID
    uint32_t id;
    // item image ID
    uint32_t imageId;
    // item name
    std::string name;
    // item description
    std::string description;
    // size of one stack
    uint32_t stackSize;
    // item rarity
    uint32_t rarity;
};

typedef std::map<uint32_t, ItemTemplateRecord> ItemTemplateMap;

/*
 * Singleton class maintaining item template storage
 */
class ItemStorage
{
    friend class Singleton<ItemStorage>;
    public:
        ~ItemStorage();

        // loads item template data from database
        void LoadFromDB();
        // retrieves item template
        ItemTemplateRecord* GetItemTemplate(uint32_t id);

    protected:
        // protected singleton constructor
        ItemStorage();

    private:
        // loaded items from database
        ItemTemplateMap m_itemTemplates;
};

#define sItemStorage Singleton<ItemStorage>::getInstance()

#endif
