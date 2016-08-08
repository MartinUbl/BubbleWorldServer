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

#ifndef BW_OPCODES_H
#define BW_OPCODES_H

// opcodes enumeration
enum Opcodes
{
    MSG_NONE                                    = 0,
    CP_LOGIN_REQUEST                            = 1,
    SP_LOGIN_RESPONSE                           = 2,
    CP_CHARACTER_LIST_REQUEST                   = 3,
    SP_CHARACTER_LIST                           = 4,
    CP_REQUEST_RESOURCE                         = 5,
    SP_RESOURCE_SEND_START                      = 6,
    SP_RESOURCE_SEND_FINISHED                   = 7,
    SP_RESOURCE_DATA                            = 8,
    CP_RESOURCE_VERIFY_CHECKSUM                 = 9,
    SP_RESOURCE_VERIFY_CHECKSUM                 = 10,
    CP_ENTER_WORLD                              = 11,
    SP_ENTER_WORLD_RESULT                       = 12,
    CP_WORLD_ENTER_COMPLETE                     = 13,
    SP_CREATE_OBJECT                            = 14,
    SP_UPDATE_OBJECT                            = 15,
    SP_DESTROY_OBJECT                           = 16,
    CP_GET_MAP_METADATA                         = 17,
    SP_MAP_METADATA                             = 18,
    CP_GET_MAP_CHUNK                            = 19,
    SP_MAP_CHUNK                                = 20,
    CP_MAP_METADATA_VERIFY_CHECKSUM             = 21,
    CP_MAP_CHUNK_VERIFY_CHECKSUM                = 22,
    SP_MAP_METADATA_VERIFY_CHECKSUM             = 23,
    SP_MAP_CHUNK_VERIFY_CHECKSUM                = 24,
    CP_GET_IMAGE_METADATA                       = 25,
    SP_IMAGE_METADATA                           = 26,
    CP_VERIFY_IMAGE_METADATA_CHECKSUM           = 27,
    SP_VERIFY_IMAGE_METADATA_CHECKSUM           = 28,
    CP_NAME_QUERY                               = 29,
    SP_NAME_QUERY_RESPONSE                      = 30,
    CP_MOVE_START_DIRECTION                     = 31,
    CP_MOVE_STOP_DIRECTION                      = 32,
    CP_MOVE_HEARTBEAT                           = 33,
    SP_MOVE_START_DIRECTION                     = 34,
    SP_MOVE_STOP_DIRECTION                      = 35,
    SP_MOVE_HEARTBEAT                           = 36,
    CP_CHAT_MESSAGE                             = 37,
    SP_CHAT_MESSAGE                             = 38,
    CP_INTERACTION_REQUEST                      = 39,
    MAX_OPCODES
};

#endif
