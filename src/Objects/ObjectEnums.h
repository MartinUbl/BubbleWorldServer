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

#ifndef BW_OBJECT_ENUMS_H
#define BW_OBJECT_ENUMS_H

// high 6 bits (0-31) of guid
enum ObjectGUIDHigh
{
    HIGHGUID_PLAYER         = 0,
    HIGHGUID_CREATURE       = 1,
    HIGHGUID_GAMEOBJECT     = 2,
};

#define MAKE_GUID64(hi,en,lo) (((uint64_t)hi << 58LL) | (((uint64_t)en & 0x3FFFFFF) << 32LL) | ((uint64_t)lo & 0xFFFFFFFFLL))
#define EXTRACT_GUIDLOW(guid64) ((uint32_t)(guid64 & 0xFFFFFFFFLL))
#define EXTRACT_GUIDHIGH(guid64) ((uint32_t)(guid64 >> 58LL))
#define EXTRACT_ENTRY(guid64) ((uint32_t)((guid64 >> 32LL) & 0x3FFFFFFF))

// recognized object type
enum ObjectType
{
    OTYPE_PLAYER            = 0,
    OTYPE_CREATURE          = 1,
    OTYPE_GAMEOBJECT        = 2,
    MAX_OBJECT_TYPE
};

// retrieves object type using highguid
static ObjectType _getTypeFromHighGUID(uint32_t highguid)
{
    switch (highguid)
    {
        case HIGHGUID_PLAYER:
            return OTYPE_PLAYER;
        case HIGHGUID_CREATURE:
            return OTYPE_CREATURE;
        case HIGHGUID_GAMEOBJECT:
            return OTYPE_GAMEOBJECT;
        default:
            return MAX_OBJECT_TYPE;
    }
}

#endif
