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
#include "PacketHandlers.h"
#include "Session.h"
#include "SmartPacket.h"
#include "AuthenticationService.h"
#include "ResourceStreamService.h"
#include "ResourceStorage.h"
#include "CharacterStorage.h"
#include "Player.h"
#include "Creature.h"
#include "MapManager.h"
#include "MapStorage.h"
#include "Log.h"
#include "ObjectAccessor.h"

void PacketHandlers::Handle_NULL(Session* sess, SmartPacket& packet)
{
    // NULL handler - this means we throw away whole packet
}

void PacketHandlers::Handle_ClientSide(Session* sess, SmartPacket& packet)
{
    // This should never happen - we should never receive server-to-client packet
}

void PacketHandlers::HandleLoginRequest(Session* sess, SmartPacket& packet)
{
    std::string username, password;
    uint32_t version;

    // read contents
    username = packet.ReadString();
    password = packet.ReadString();
    version = packet.ReadUInt32();

    uint32_t accId;

    // attempt to login
    AuthStatus statusCode = sAuthenticationService->AuthenticateUser(username, password, &accId);
    // if OK, move state
    if (statusCode == AUTH_STATUS_OK)
    {
        sess->SetAccountId(accId);
        sess->SetConnectionState(CONNECTION_STATE_LOBBY);
    }

    // send response
    SmartPacket pkt(SP_LOGIN_RESPONSE);
    pkt.WriteUInt8(statusCode);
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleCharacterListRequest(Session* sess, SmartPacket& packet)
{
    std::list<CharacterBaseRecord> charList;
    // retrieve character list
    sCharacterStorage->GetCharacterListForAccount(sess->GetAccountId(), charList);

    SmartPacket pkt(SP_CHARACTER_LIST);

    pkt.WriteUInt8((uint8_t)charList.size());   // count
    for (CharacterBaseRecord &rec : charList)
    {
        pkt.WriteUInt32(rec.guid);              // guid
        pkt.WriteString(rec.name.c_str());      // name
        pkt.WriteUInt16(rec.level);             // level
    }

    sess->SendPacket(pkt);
}

void PacketHandlers::HandleResourceRequest(Session* sess, SmartPacket& packet)
{
    ResourceType type = (ResourceType)packet.ReadUInt8();
    uint32_t id = packet.ReadUInt32();

    // create resource stream
    sResourceStreamService->StartStream(type, id, sess->GetSessionId());
}

void PacketHandlers::HandleResourceChecksumVerify(Session* sess, SmartPacket& packet)
{
    ResourceRecord* rec;
    ResourceType type;
    uint32_t id;
    int i;

    uint16_t count = packet.ReadUInt16();

    std::vector<ResourceType> m_failTypes;
    std::vector<uint32_t> m_failIDs;

    // verify checksums of all requested resources
    for (i = 0; i < count; i++)
    {
        type = (ResourceType)packet.ReadUInt8();
        id = packet.ReadUInt32();
        rec = sResourceStorage->GetResource(type, id);

        // if does not match, put it in vectors to be send as failed
        if (rec->checksum != packet.ReadString())
        {
            m_failTypes.push_back(type);
            m_failIDs.push_back(id);
        }
    }

    // send failed IDs
    SmartPacket pkt(SP_RESOURCE_VERIFY_CHECKSUM);
    pkt.WriteUInt16((uint16_t)m_failTypes.size());
    for (i = 0; i < m_failTypes.size(); i++)
    {
        pkt.WriteUInt8(m_failTypes[i]);
        pkt.WriteUInt32(m_failIDs[i]);
    }
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleEnterWorld(Session* sess, SmartPacket& packet)
{
    uint32_t guidLow = packet.ReadUInt32();

    // TODO: verify if selected character belongs to session account
    //       then fail with ENTER_WORLD_FAILED_NOT_OWNED_CHARACTER

    Player* plr = new Player();
    plr->Create(guidLow, sess);
    // load character
    if (!plr->LoadFromDB())
    {
        // when failed, do not allow entering world
        SmartPacket pkt(SP_ENTER_WORLD_RESULT);
        pkt.WriteUInt8(ENTER_WORLD_FAILED_NONEXISTENT_CHARACTER);
        sess->SendPacket(pkt);
        return;
    }

    sess->SetPlayer(plr);

    // go forth
    SmartPacket pkt(SP_ENTER_WORLD_RESULT);
    pkt.WriteUInt8(ENTER_WORLD_OK);
    pkt.WriteUInt32(plr->GetMapId());
    pkt.WriteFloat(plr->GetPositionX());
    pkt.WriteFloat(plr->GetPositionY());
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleEnterWorldComplete(Session* sess, SmartPacket& packet)
{
    // entering world complete, now it's safe to consider player "ingame"
    sess->SetConnectionState(CONNECTION_STATE_INGAME);

    // get player's map
    Map* map = sMapManager->GetMap(sess->GetPlayer()->GetMapId());
    if (!map)
    {
        sLog->Error("Could not retrieve map ID %u, that should be already loaded!", sess->GetPlayer()->GetMapId());
        return;
    }

    // add player object to map
    map->AddToMap(sess->GetPlayer());
}

void PacketHandlers::HandleGetMapMetadata(Session* sess, SmartPacket& packet)
{
    uint32_t mapId = packet.ReadUInt32();

    // get map record
    MapRecord* mrec = sMapStorage->GetMapRecord(mapId);
    if (!mrec)
    {
        SmartPacket pkt(SP_MAP_METADATA);
        pkt.WriteUInt8(GENERIC_STATUS_NOTFOUND);
        sess->SendPacket(pkt);
        return;
    }

    // send metadata
    SmartPacket pkt(SP_MAP_METADATA);
    pkt.WriteUInt8(GENERIC_STATUS_OK);

    pkt.WriteUInt32(mrec->header.mapId);
    pkt.WriteUInt32(mrec->header.sizeX);
    pkt.WriteUInt32(mrec->header.sizeY);
    pkt.WriteString(mrec->header.name);
    pkt.WriteUInt32(mrec->header.entryX);
    pkt.WriteUInt32(mrec->header.entryY);
    pkt.WriteUInt16(mrec->header.defaultFieldType);
    pkt.WriteUInt32(mrec->header.defaultFieldTexture);
    pkt.WriteUInt32(mrec->header.defaultFieldFlags);

    pkt.WriteString(mrec->filename.c_str());

    sess->SendPacket(pkt);
}

void PacketHandlers::HandleGetMapChunk(Session* sess, SmartPacket& packet)
{
    uint32_t mapId = packet.ReadUInt32();
    uint32_t startX = packet.ReadUInt32();
    uint32_t startY = packet.ReadUInt32();

    // transform start coordinates to chunk index
    uint32_t chunkIndexX = MapStorage::GetChunkIndexX(startX);
    uint32_t chunkIndexY = MapStorage::GetChunkIndexY(startY);

    // get map record
    MapRecord* mrec = sMapStorage->GetMapRecord(mapId);
    Map* map = sMapManager->GetMap(mapId);
    if (!mrec || !map)
    {
        SmartPacket pkt(SP_MAP_CHUNK);
        pkt.WriteUInt8(GENERIC_STATUS_NOTFOUND);
        sess->SendPacket(pkt);
        return;
    }

    // limit chunk coordinates
    if (chunkIndexX >= mrec->chunks.size() || chunkIndexY >= mrec->chunks[chunkIndexX].size())
    {
        SmartPacket pkt(SP_MAP_CHUNK);
        pkt.WriteUInt8(GENERIC_STATUS_ERROR);
        sess->SendPacket(pkt);
        return;
    }

    SmartPacket pkt(SP_MAP_CHUNK);
    pkt.WriteUInt8(GENERIC_STATUS_OK);

    MapChunkRecord &chunk = mrec->chunks[chunkIndexX][chunkIndexY];

    // chunk metadata
    pkt.WriteUInt32(chunk.mapId);
    pkt.WriteUInt32(chunk.startX);
    pkt.WriteUInt32(chunk.startY);
    pkt.WriteUInt32(chunk.sizeX);
    pkt.WriteUInt32(chunk.sizeY);

    // send chunk contents
    for (uint32_t i = 0; i < chunk.sizeX; i++)
    {
        for (uint32_t j = 0; j < chunk.sizeY; j++)
        {
            pkt.WriteUInt16(chunk.fields[i][j].type);
            pkt.WriteUInt32(chunk.fields[i][j].texture);
            pkt.WriteUInt32(chunk.fields[i][j].flags);
        }
    }

    sess->SendPacket(pkt);
}

void PacketHandlers::HandleMapMetaChecksumVerify(Session* sess, SmartPacket& packet)
{
    uint32_t mapId = packet.ReadUInt32();

    // retrieve map record and verify checksum
    MapRecord* mrec = sMapStorage->GetMapRecord(mapId);
    if (!mrec || mrec->headerChecksum == packet.ReadString())
    {
        SmartPacket pkt(SP_MAP_METADATA_VERIFY_CHECKSUM);
        pkt.WriteUInt8(GENERIC_STATUS_OK);
        pkt.WriteUInt32(mapId);
        sess->SendPacket(pkt);
        return;
    }

    SmartPacket pkt(SP_MAP_METADATA_VERIFY_CHECKSUM);
    pkt.WriteUInt8(GENERIC_STATUS_ERROR);
    pkt.WriteUInt32(mapId);
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleMapChunkChecksumVerify(Session* sess, SmartPacket& packet)
{
    uint32_t mapId = packet.ReadUInt32();
    uint32_t startX = packet.ReadUInt32();
    uint32_t startY = packet.ReadUInt32();

    uint32_t chunkIndexX = MapStorage::GetChunkIndexX(startX);
    uint32_t chunkIndexY = MapStorage::GetChunkIndexY(startY);

    std::string checksum = packet.ReadString();

    // retrieve map record, secure bounds and verify checksum
    MapRecord* mrec = sMapStorage->GetMapRecord(mapId);
    if (!mrec || chunkIndexX >= mrec->chunks.size() || chunkIndexY >= mrec->chunks[chunkIndexX].size() ||
        mrec->chunks[chunkIndexX][chunkIndexY].checksum == checksum)
    {
        SmartPacket pkt(SP_MAP_CHUNK_VERIFY_CHECKSUM);
        pkt.WriteUInt8(GENERIC_STATUS_OK);
        pkt.WriteUInt32(mapId);
        pkt.WriteUInt32(startX);
        pkt.WriteUInt32(startY);
        sess->SendPacket(pkt);
        return;
    }

    SmartPacket pkt(SP_MAP_CHUNK_VERIFY_CHECKSUM);
    pkt.WriteUInt8(GENERIC_STATUS_ERROR);             // 1 failed
    pkt.WriteUInt32(mapId);
    pkt.WriteUInt32(startX);
    pkt.WriteUInt32(startY);
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleGetImageMetadata(Session* sess, SmartPacket& packet)
{
    uint32_t id = packet.ReadUInt32();

    // retrieve image resource metadata record
    ImageResourceMetadata* meta = sResourceStorage->GetImageMetadata(id);
    if (!meta)
    {
        SmartPacket pkt(SP_IMAGE_METADATA);
        pkt.WriteUInt8(GENERIC_STATUS_ERROR);
        sess->SendPacket(pkt);
        return;
    }

    SmartPacket pkt(SP_IMAGE_METADATA);
    pkt.WriteUInt8(GENERIC_STATUS_OK);

    // send metadata
    pkt.WriteUInt32(meta->id);
    pkt.WriteUInt32(meta->sizeX);
    pkt.WriteUInt32(meta->sizeY);
    pkt.WriteUInt32(meta->baseCenterX);
    pkt.WriteUInt32(meta->baseCenterY);
    pkt.WriteUInt32(meta->collisionX1);
    pkt.WriteUInt32(meta->collisionY1);
    pkt.WriteUInt32(meta->collisionX2);
    pkt.WriteUInt32(meta->collisionY2);

    // send animations
    pkt.WriteUInt32((uint32_t)meta->animations.size());
    for (ImageAnimationMap::iterator itr = meta->animations.begin(); itr != meta->animations.end(); ++itr)
    {
        ImageAnimationMetadata* animmeta = &itr->second;

        pkt.WriteUInt32(animmeta->animId);
        pkt.WriteUInt32(animmeta->frameBegin);
        pkt.WriteUInt32(animmeta->frameEnd);
        pkt.WriteUInt32(animmeta->frameDelay);
    }

    sess->SendPacket(pkt);
}

void PacketHandlers::HandleImageMetaChecksumVerify(Session* sess, SmartPacket& packet)
{
    uint32_t id = packet.ReadUInt32();
    std::string checksum = packet.ReadString();

    // retrieve image resource metadata record and verify checksum
    ImageResourceMetadata* meta = sResourceStorage->GetImageMetadata(id);
    if (!meta || meta->checksum == checksum)
    {
        SmartPacket pkt(SP_VERIFY_IMAGE_METADATA_CHECKSUM);
        pkt.WriteUInt8(GENERIC_STATUS_OK);
        pkt.WriteUInt32(id);
        sess->SendPacket(pkt);
        return;
    }

    SmartPacket pkt(SP_VERIFY_IMAGE_METADATA_CHECKSUM);
    pkt.WriteUInt8(GENERIC_STATUS_ERROR);
    pkt.WriteUInt32(id);
    sess->SendPacket(pkt);
}

void PacketHandlers::HandleNameQuery(Session* sess, SmartPacket& packet)
{
    uint64_t guid = packet.ReadUInt64();

    SmartPacket pkt(SP_NAME_QUERY_RESPONSE);
    pkt.WriteUInt64(guid);

    // object has to exist and be in world
    WorldObject* obj = sObjectAccessor->FindWorldObject(guid);
    if (!obj)
        pkt.WriteUInt8(0); // empty string if not found
    else
        pkt.WriteString(obj->GetName());

    sess->SendPacket(pkt);
}

void PacketHandlers::HandleMoveStartDir(Session* sess, SmartPacket& packet)
{
    uint8_t direction = packet.ReadUInt8() & 0xF;

    sess->GetPlayer()->StartMoving((MoveDirectionElement)direction);
}

void PacketHandlers::HandleMoveStopDir(Session* sess, SmartPacket& packet)
{
    uint8_t direction = packet.ReadUInt8() & 0xF;
    float x = packet.ReadFloat();
    float y = packet.ReadFloat();

    // TODO: verify if this is possible, anticheat, etc.

    sess->GetPlayer()->RelocateWithinMap(x, y);

    sess->GetPlayer()->StopMoving((MoveDirectionElement)direction);
}

void PacketHandlers::HandleMoveHeartbeat(Session* sess, SmartPacket& packet)
{
    float x = packet.ReadFloat();
    float y = packet.ReadFloat();

    // TODO: verify if this is possible, anticheat, etc.

    Player* plr = sess->GetPlayer();

    plr->RelocateWithinMap(x, y);

    // TODO: timer, limit maximum count of heartbeats per second

    SmartPacket pkt(SP_MOVE_HEARTBEAT);
    pkt.WriteUInt64(plr->GetGUID());
    pkt.WriteUInt8(plr->GetMoveMask());
    pkt.WriteFloat(x);
    pkt.WriteFloat(y);
    plr->SendPacketToSorroundings(pkt);
}

void PacketHandlers::HandleChatMessage(Session* sess, SmartPacket& packet)
{
    uint8_t type = packet.ReadUInt8();
    std::string msg = packet.ReadString();

    // TODO: secure possible talk types for player

    sess->GetPlayer()->Talk((TalkType)type, msg.c_str());
}

void PacketHandlers::HandleInteractionRequest(Session* sess, SmartPacket& packet)
{
    uint64_t guid = packet.ReadUInt64();

    WorldObject* obj = sObjectAccessor->FindWorldObject(guid);
    if (!obj)
        return;

    // TODO: distance check

    if (obj->GetType() == OTYPE_CREATURE)
        obj->ToCreature()->Interact(sess->GetPlayer());
}

void PacketHandlers::HandleDialogueDecision(Session* sess, SmartPacket& packet)
{
    uint64_t guid = packet.ReadUInt64();
    uint32_t decision = packet.ReadUInt32();

    WorldObject* obj = sObjectAccessor->FindWorldObject(guid);
    // if the object is not found, or is not creature, send dialogue close packet
    if (!obj || obj->GetType() != OTYPE_CREATURE)
    {
        SmartPacket pkt(SP_DIALOGUE_CLOSE);
        sess->SendPacket(pkt);
        return;
    }

    obj->ToCreature()->DialogueDecision(sess->GetPlayer(), decision);
}
