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

#ifndef BW_NETWORK_MANAGER
#define BW_NETWORK_MANAGER

#include "Singleton.h"
#include "SmartPacket.h"

// platform-specific defines
#ifdef _WIN32
#define SOCK SOCKET
#define ADDRLEN int

#define SOCKETWOULDBLOCK WSAEWOULDBLOCK
#define SOCKETCONNRESET  WSAECONNRESET
#define SOCKETCONNABORT  WSAECONNABORTED
#define SOCKETINPROGRESS WSAEINPROGRESS
#define LASTERROR() WSAGetLastError()
#define INET_PTON(fam,addrptr,buff) InetPtonA(fam,addrptr,buff)
#define INET_NTOP(fam,addrptr,buff,socksize) InetNtopA(fam,addrptr,buff,socksize)
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <netdb.h>
#include <fcntl.h>

#define SOCK int
#define ADDRLEN socklen_t

#define INVALID_SOCKET -1

#define SOCKETWOULDBLOCK EAGAIN
#define SOCKETCONNABORT ECONNABORTED
#define SOCKETCONNRESET ECONNRESET
#define SOCKETINPROGRESS EINPROGRESS
#define LASTERROR() errno
#define INET_PTON(fam,addrptr,buff) inet_pton(fam,addrptr,buff)
#define INET_NTOP(fam,addrptr,buff,socksize) inet_ntop(fam,addrptr,buff,socksize)
#define CLOSESOCKET close
#endif

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

// timeout value for session
#define SESSION_INACTIVITY_EXPIRE 60

// WinSock nonblocking flag; this value is not defined in any WinSock headers, but is described as constant
#define WINSOCK_NONBLOCKING_ARG 1

// default bind address
#define DEFAULT_NET_BIND_ADDR "127.0.0.1"
// default bind port
#define DEFAULT_NET_BIND_PORT 7874

class Session;

/*
 * Singleton class used for maintaining network stuff, managing sessions, etc.
 */
class NetworkManager
{
    friend class Singleton<NetworkManager>;
    public:
        ~NetworkManager();

        // initialize networking
        bool Init(std::string bindAddress, uint16_t port);
        // update networking - accept connections, process packets, ..
        void Update();

        // send packet to target socket
        void SendPacket(SmartPacket &pkt, SOCK targetSocket);
        // send raw data to target socket
        bool SendPacket(int len, uint8_t* data, SOCK targetSocket);
        // send packet to session
        bool SendPacketToSession(SmartPacket &pkt, uint32_t sessionId);

    protected:
        // protected singleton constructor
        NetworkManager();

        // accept waiting connections
        void AcceptConnections();
        // sends all packets in send queue
        void SendQueuedPackets();
        // update sessions - receive data, kick expired sessions, etc.
        void UpdateSessions();
        // handle all queued packets
        void HandleQueuedPackets();

        // remove session from session map/set
        std::set<Session*>::iterator RemoveSession(std::set<Session*>::iterator itr);

        // generate new session ID
        uint32_t GenerateSessionId();

    private:
        // server socket
        SOCK m_socket;
        // server sock addr structure
        sockaddr_in m_sockAddr;
        // bind address
        std::string m_host;
        // bind port
        uint16_t m_port;

        // socket set used in network routines
        fd_set m_sockSet;
        // NFDS parameter for select()
        int m_nfds;

        // maximum assigned session ID
        uint32_t m_maxSessionId;

        // session set (used for iterations)
        std::set<Session*> m_sessionSet;
        // session map by session ID (used for lookups by session ID)
        std::map<uint32_t, Session*> m_sessionsBySessionId;
        // session map by socket (used for reverse lookups by socket)
        std::map<SOCK, Session*> m_sessionsBySocket;

        // networking mutex
        std::mutex m_netMutex;
};

#define sNetwork Singleton<NetworkManager>::getInstance()

#endif
