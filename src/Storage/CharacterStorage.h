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

#ifndef BW_CHARACTER_STORAGE_H
#define BW_CHARACTER_STORAGE_H

#include "Singleton.h"

/*
 * Structure containing base information about player
 */
struct CharacterBaseRecord
{
    // init constructor
    CharacterBaseRecord(uint32_t _guid, uint32_t _accountId, const char* _name, uint16_t _level) :
        guid(_guid), accountId(_accountId), name(_name), level(_level)
    {
    }

    // character GUID
    uint32_t guid;
    // account ID
    uint32_t accountId;
    // character name
    std::string name;
    // character level
    uint16_t level;
};

/*
 * Singleton class maintaining characters storage and operations
 */
class CharacterStorage
{
    friend class Singleton<CharacterStorage>;
    public:
        ~CharacterStorage();

        // retrieves character list for supplied account
        void GetCharacterListForAccount(uint32_t accountId, std::list<CharacterBaseRecord> &targetList);

    protected:
        // protected singleton storage
        CharacterStorage();

    private:
        //
};

#define sCharacterStorage Singleton<CharacterStorage>::getInstance()

#endif
