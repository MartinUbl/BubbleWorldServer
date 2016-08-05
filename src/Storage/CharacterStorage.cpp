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
#include "CharacterStorage.h"
#include "DatabaseConnection.h"

CharacterStorage::CharacterStorage()
{
    //
}

CharacterStorage::~CharacterStorage()
{
    //
}

void CharacterStorage::GetCharacterListForAccount(uint32_t accountId, std::list<CharacterBaseRecord> &targetList)
{
    // select characters from DB
    DBResult res = sMainDatabase.PQuery("SELECT guid, account_id, name, level FROM characters WHERE account_id = '%u'", accountId);

    targetList.clear();

    // fill the list
    while (res.FetchRow())
    {
        targetList.push_back(CharacterBaseRecord(
            res.GetUInt32(0),           // guid
            res.GetUInt32(1),           // accountId
            res.GetString(2).c_str(),   // name
            res.GetUInt16(3)            // level
        ));
    }
}
