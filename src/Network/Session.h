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

#ifndef BW_SESSION_H
#define BW_SESSION_H

#include "NetworkManager.h"

/*
 * Structure for holding processed packet contents prepared to be sent
 */
struct ProcessedPacket
{
    int len;
    uint8_t* data;
};

// enumerator of connection states
enum ConnectionState
{
    CONNECTION_STATE_NONE = 0,
    CONNECTION_STATE_AUTH = 1,
    CONNECTION_STATE_LOBBY = 2,
    CONNECTION_STATE_INGAME = 3,
    MAX_CONN_STATE
};

class SmartPacket;
class Player;

/*
 * Class maintaining one client connection
 */
class Session
{
    public:
        Session(SOCK socket, sockaddr_in sockAddr, uint32_t sessionId);
        virtual ~Session();

        // retrieves socket info (sockaddr)
        sockaddr_in const& GetSockInfo();
        // retrieves socket identifier
        SOCK GetSocket();

        // adds packet to be handled in next tick
        void AddPacketToHandleQueue(SmartPacket *packet);
        // handles packet
        void HandlePacket(SmartPacket &packet);
        // queue packet for sending
        void SendPacket(SmartPacket &packet);
        // sends all queued packets (if socket not busy)
        void SendQueuedPackets();
        // handle all incoming packets
        void HandleQueuedPackets();

        // retrieves connection state
        ConnectionState GetConnectionState();
        // sets connection state
        void SetConnectionState(ConnectionState state);
        // sets remote address (IP)
        void SetRemoteAddr(char* addr);
        // retrieves remote address
        const char* GetRemoteAddr();

        // retrieves session ID
        uint32_t GetSessionId();
        // sets account ID which the user logged in
        void SetAccountId(uint32_t accountId);
        // retrieves account ID
        uint32_t GetAccountId();

        // sets session player
        void SetPlayer(Player* plr);
        // retrieves session player
        Player* GetPlayer();

        // is session marked as expired?
        bool IsMarkedAsExpired();
        // kick the player from game and end session
        void Kick();
        // logout player from game, remove from map, save and destroy
        void Logout();
        // retrieve session timeout timestamp
        time_t GetSessionTimeoutValue();
        // sets session timeout timestamp
        void SetSessionTimeoutValue(time_t tm);

    protected:
        //

    private:
        // session ID
        uint32_t m_sessionId;
        // is session marked as expired?
        bool m_isExpired;
        // session timeout timestamp
        time_t m_sessionTimeout;

        // account ID assigned
        uint32_t m_accountId;
        // session player
        Player* m_player;

        // client socket
        SOCK m_socket;
        // client socket addr structure
        sockaddr_in m_sockAddr;
        // session connection state
        ConnectionState m_connectionState;
        // remote IP address
        std::string m_remoteAddr;

        // incoming packets queue
        std::queue<SmartPacket*> m_packetHandleQueue;
        // outgoing packet queue
        std::queue<ProcessedPacket*> m_packetSendQueue;
        // mutex for locking packet send queue
        std::mutex m_packetSendQueueMutex;
};

#endif
