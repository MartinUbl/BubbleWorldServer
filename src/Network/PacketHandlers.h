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

#ifndef BW_PACKETHANDLERS_H
#define BW_PACKETHANDLERS_H

#include "Session.h"
#include "SmartPacket.h"

// packet handler function arguments
#define PACKET_HANDLER_ARGS Session* sess, SmartPacket &packet
// packet handler definition
#define PACKET_HANDLER(x) void x(PACKET_HANDLER_ARGS)

// prepared state restriction masks
enum StateRestrictionMask
{
    STATE_RESTRICTION_NEVER         = 0,
    STATE_RESTRICTION_ANY           = (uint32_t)(-1),
    STATE_RESTRICTION_AUTH          = 1 << CONNECTION_STATE_AUTH,
    STATE_RESTRICTION_LOBBY         = 1 << CONNECTION_STATE_LOBBY,
    STATE_RESTRICTION_GAME          = 1 << CONNECTION_STATE_INGAME,
    STATE_RESTRICTION_VERIFIED      = 1 << CONNECTION_STATE_LOBBY | 1 << CONNECTION_STATE_INGAME,
};

/*
 * Structure of packet handler record
 */
struct PacketHandlerStructure
{
    // handler function
    void (*handler)(PACKET_HANDLER_ARGS);

    // state restriction
    StateRestrictionMask stateRestriction;
};

// we wrap all packet handlers into namespace
namespace PacketHandlers
{
    PACKET_HANDLER(Handle_NULL);
    PACKET_HANDLER(Handle_ClientSide);

    PACKET_HANDLER(HandleLoginRequest);
    PACKET_HANDLER(HandleCharacterListRequest);
    PACKET_HANDLER(HandleResourceRequest);
    PACKET_HANDLER(HandleResourceChecksumVerify);
    PACKET_HANDLER(HandleEnterWorld);
    PACKET_HANDLER(HandleEnterWorldComplete);
    PACKET_HANDLER(HandleGetMapMetadata);
    PACKET_HANDLER(HandleGetMapChunk);
    PACKET_HANDLER(HandleMapMetaChecksumVerify);
    PACKET_HANDLER(HandleMapChunkChecksumVerify);
    PACKET_HANDLER(HandleGetImageMetadata);
    PACKET_HANDLER(HandleImageMetaChecksumVerify);
    PACKET_HANDLER(HandleNameQuery);
    PACKET_HANDLER(HandleMoveStartDir);
    PACKET_HANDLER(HandleMoveStopDir);
    PACKET_HANDLER(HandleMoveHeartbeat);
    PACKET_HANDLER(HandleChatMessage);
    PACKET_HANDLER(HandleInteractionRequest);
    PACKET_HANDLER(HandleDialogueDecision);
};

// table of packet handlers; the opcode is also an index here
static PacketHandlerStructure PacketHandlerTable[] = {
    { &PacketHandlers::Handle_NULL,             STATE_RESTRICTION_NEVER },      // OPCODE_NONE
    { &PacketHandlers::HandleLoginRequest,      STATE_RESTRICTION_AUTH },       // CP_LOGIN_REQUEST
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_LOGIN_RESPONSE
    { &PacketHandlers::HandleCharacterListRequest, STATE_RESTRICTION_LOBBY },   // CP_CHARACTER_LIST_REQUEST
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_CHARACTER_LIST
    { &PacketHandlers::HandleResourceRequest,   STATE_RESTRICTION_ANY },        // CP_REQUEST_RESOURCE
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_RESOURCE_SEND_START
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_RESOURCE_SEND_FINISHED
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_RESOURCE_DATA
    { &PacketHandlers::HandleResourceChecksumVerify, STATE_RESTRICTION_ANY },   // CP_RESOURCE_VERIFY_CHECKSUM
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_RESOURCE_VERIFY_CHECKSUM
    { &PacketHandlers::HandleEnterWorld,        STATE_RESTRICTION_LOBBY },      // CP_ENTER_WORLD
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_ENTER_WORLD_RESULT
    { &PacketHandlers::HandleEnterWorldComplete, STATE_RESTRICTION_LOBBY },     // CP_WORLD_ENTER_COMPLETE
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_CREATE_OBJECT
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_UPDATE_OBJECT
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_DESTROY_OBJECT
    { &PacketHandlers::HandleGetMapMetadata,    STATE_RESTRICTION_VERIFIED },   // CP_GET_MAP_METADATA
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MAP_METADATA
    { &PacketHandlers::HandleGetMapChunk,       STATE_RESTRICTION_VERIFIED },   // CP_GET_MAP_CHUNK
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MAP_CHUNK
    { &PacketHandlers::HandleMapMetaChecksumVerify,  STATE_RESTRICTION_VERIFIED},   // CP_MAP_METADATA_VERIFY_CHECKSUM
    { &PacketHandlers::HandleMapChunkChecksumVerify, STATE_RESTRICTION_VERIFIED},   // CP_MAP_CHUNK_VERIFY_CHECKSUM
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MAP_METADATA_VERIFY_CHECKSUM
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER},       // SP_MAP_CHUNK_VERIFY_CHECKSUM
    { &PacketHandlers::HandleGetImageMetadata,  STATE_RESTRICTION_VERIFIED},    // CP_GET_IMAGE_METADATA
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER},       // SP_IMAGE_METADATA
    { &PacketHandlers::HandleImageMetaChecksumVerify, STATE_RESTRICTION_VERIFIED},  // CP_VERIFY_IMAGE_METADATA_CHECKSUM
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER},       // SP_VERIFY_IMAGE_METADATA_CHECKSUM
    { &PacketHandlers::HandleNameQuery,         STATE_RESTRICTION_VERIFIED},    // CP_NAME_QUERY
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER},       // SP_NAME_QUERY_RESPONSE
    { &PacketHandlers::HandleMoveStartDir,      STATE_RESTRICTION_GAME  },      // CP_MOVE_START_DIRECTION
    { &PacketHandlers::HandleMoveStopDir,       STATE_RESTRICTION_GAME  },      // CP_MOVE_STOP_DIRECTION
    { &PacketHandlers::HandleMoveHeartbeat,     STATE_RESTRICTION_GAME  },      // CP_MOVE_HEARTBEAT
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MOVE_START_DIRECTION
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MOVE_STOP_DIRECTION
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_MOVE_HEARTBEAT
    { &PacketHandlers::HandleChatMessage,       STATE_RESTRICTION_GAME  },      // CP_CHAT_MESSAGE
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_CHAT_MESSAGE
    { &PacketHandlers::HandleInteractionRequest,STATE_RESTRICTION_GAME  },      // CP_INTERACTION_REQUEST
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_DIALOGUE_DATA
    { &PacketHandlers::HandleDialogueDecision,  STATE_RESTRICTION_GAME  },      // CP_DIALOGUE_DECISION
    { &PacketHandlers::Handle_ClientSide,       STATE_RESTRICTION_NEVER },      // SP_DIALOGUE_CLOSE
};

#endif
