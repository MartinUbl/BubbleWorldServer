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

#ifndef BW_SHARED_ENUMS
#define BW_SHARED_ENUMS

// enumerator of generic statuses
enum GenericStatus
{
    GENERIC_STATUS_OK = 0,                          // everything went fine
    GENERIC_STATUS_ERROR = 1,                       // error during processing - bad arguments (their combination), not met state precondition, ...
    GENERIC_STATUS_NOTFOUND = 2,                    // the requested resource/element/instance was not found
};

// authentication status
enum AuthStatus
{
    AUTH_STATUS_OK = 0,
    AUTH_STATUS_UNKNOWN_USER = 1,
    AUTH_STATUS_INVALID_PASSWORD = 2,
    AUTH_STATUS_BANNED = 3,
    AUTH_STATUS_INCOMPATIBLE_VERSION = 4,
};

// status of world entering
enum EnterWorldStatus
{
    ENTER_WORLD_OK = 0,
    ENTER_WORLD_FAILED_NOT_OWNED_CHARACTER = 1,
    ENTER_WORLD_FAILED_NONEXISTENT_CHARACTER = 2,
};

// type of resources
enum ResourceType
{
    RSTYPE_GENERIC = 0,
    RSTYPE_IMAGE = 1,
    MAX_RSTYPE
};

// move direction flag elements (powers of 2 to allow binary operations)
enum MoveDirectionElement
{
    MOVE_UP = 1,
    MOVE_RIGHT = 2,
    MOVE_DOWN = 4,
    MOVE_LEFT = 8,
};

// type of talk (chat)
enum TalkType
{
    TALK_SAY = 0,
    TALK_YELL = 1,
    TALK_SERVER_MESSAGE = 2,
};

#endif
