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
#include "Session.h"
#include "SmartPacket.h"
#include "PacketHandlers.h"
#include "Log.h"
#include "Player.h"
#include "Map.h"
#include "MapManager.h"
#include "ObjectAccessor.h"

Session::Session(SOCK socket, sockaddr_in sockAddr, uint32_t sessionId) : m_sessionId(sessionId), m_socket(socket), m_sockAddr(sockAddr)
{
    m_connectionState = CONNECTION_STATE_AUTH;
    m_isExpired = false;
}

Session::~Session()
{
    //
}

sockaddr_in const& Session::GetSockInfo()
{
    return m_sockAddr;
}

SOCK Session::GetSocket()
{
    return m_socket;
}

void Session::HandlePacket(SmartPacket &packet)
{
    // do not handle opcodes higher than maximum
    if (packet.GetOpcode() >= MAX_OPCODES)
    {
        sLog->Network("Client (IP: %s) sent invalid packet (unknown opcode %u), not handling", GetRemoteAddr(), packet.GetOpcode());
        return;
    }

    sLog->Network("NETWORK: Received packet %u", packet.GetOpcode());

    // packet handlers might throw exception about trying to reach out of packet data range
    try
    {
        // verify the state of client connection
        if ((PacketHandlerTable[packet.GetOpcode()].stateRestriction & (1 << (int)m_connectionState)) == 0)
        {
            sLog->Network("Client (IP: %s) sent invalid packet (opcode %u) for state %u, not handling", GetRemoteAddr(), packet.GetOpcode(), m_connectionState);
            return;
        }

        // look handler up in handler table and call it
        PacketHandlerTable[packet.GetOpcode()].handler(this, packet);
    }
    catch (PacketReadException &ex)
    {
        sLog->Error("Read error during executing handler for opcode %u - attempt to read %u bytes at offset %u (real size %u bytes) (client IP: %s)", packet.GetOpcode(), ex.GetAttemptSize(), ex.GetPosition(), packet.GetSize(), GetRemoteAddr());
    }
}

void Session::SendPacket(SmartPacket &packet)
{
    ProcessedPacket* ppkt = new ProcessedPacket;

    uint16_t op, sz;
    ppkt->len = SmartPacket::HeaderSize + packet.GetSize();
    ppkt->data = new uint8_t[SmartPacket::HeaderSize + packet.GetSize()];

    op = htons(packet.GetOpcode());
    sz = htons(packet.GetSize());

    // write opcode
    memcpy(ppkt->data, &op, 2);
    // write contents size
    memcpy(ppkt->data + 2, &sz, 2);
    // write contents
    memcpy(ppkt->data + 4, packet.GetData(), packet.GetSize());

    std::unique_lock<std::mutex> lck(m_packetSendQueueMutex);

    // queue packet
    m_packetSendQueue.push(ppkt);
}

void Session::SendQueuedPackets()
{
    std::unique_lock<std::mutex> lck(m_packetSendQueueMutex);

    ProcessedPacket* ppkt;
    // while there's something to be sent
    while (!m_packetSendQueue.empty())
    {
        ppkt = m_packetSendQueue.front();

        // send packet if socket not busy
        if (sNetwork->SendPacket(ppkt->len, ppkt->data, m_socket))
            m_packetSendQueue.pop(); // remove from queue
        else
        {
            // otherwise delay packet sending
            sLog->Network("Packet send queue busy, delaying packet send");
            break;
        }
    }
}

void Session::AddPacketToHandleQueue(SmartPacket *packet)
{
    m_packetHandleQueue.push(packet);
}

void Session::HandleQueuedPackets()
{
    // while there's something to be handled, handle it
    while (!m_packetHandleQueue.empty())
    {
        HandlePacket(*m_packetHandleQueue.front());
        m_packetHandleQueue.pop();
    }
}

ConnectionState Session::GetConnectionState()
{
    return m_connectionState;
}

void Session::SetConnectionState(ConnectionState state)
{
    m_connectionState = state;
}

void Session::SetRemoteAddr(char* addr)
{
    m_remoteAddr = addr;
}

const char* Session::GetRemoteAddr()
{
    return m_remoteAddr.c_str();
}

uint32_t Session::GetSessionId()
{
    return m_sessionId;
}

void Session::SetAccountId(uint32_t accountId)
{
    m_accountId = accountId;
}

uint32_t Session::GetAccountId()
{
    return m_accountId;
}

void Session::SetPlayer(Player* plr)
{
    m_player = plr;
}

Player* Session::GetPlayer()
{
    return m_player;
}

void Session::Logout()
{
    Player* plr = GetPlayer();
    if (plr)
    {
        // remove player from map if in map
        if (plr->GetMapId())
        {
            Map* m = sMapManager->GetMap(plr->GetMapId());
            if (m)
                m->RemoveFromMap(plr);
        }

        // save player record to database
        plr->SaveToDB();

        // remove object from world
        sObjectAccessor->RemoveObject(plr->GetGUID());

        // destroy
        delete plr;
        SetPlayer(nullptr);
    }
}

void Session::Kick()
{
    // TODO: some kick packet?

    // remove player from game
    Logout();

    // this will cause socket close on next networking thread update
    m_isExpired = true;
}

bool Session::IsMarkedAsExpired()
{
    return m_isExpired;
}

time_t Session::GetSessionTimeoutValue()
{
    return m_sessionTimeout;
}

void Session::SetSessionTimeoutValue(time_t tm)
{
    m_sessionTimeout = time(nullptr) + tm;
}
