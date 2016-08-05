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
#include "NetworkManager.h"
#include "SmartPacket.h"
#include "Session.h"
#include "Log.h"

NetworkManager::NetworkManager()
{
    m_maxSessionId = 0;
}

NetworkManager::~NetworkManager()
{
    //
}

bool NetworkManager::Init(std::string bindAddress, uint16_t port)
{
#ifdef _WIN32
    WORD version = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(version, &data) != 0)
    {
        sLog->Error("Unable to start WinSock service for unknown reason");
        return false;
    }
#endif

    m_host = bindAddress.c_str();
    m_port = port;

    if ((m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        sLog->Error("Unable to create socket");
        return false;
    }

    int param = 1;
    if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&param, sizeof(int)) == -1)
    {
        sLog->Error("NET: Failed to use SO_REUSEADDR flag, bind may fail due to orphan connections to old socket");
        // do not fail whole process, this is not mandatory
    }

    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port = htons(m_port);

    if (bindAddress == "0.0.0.0")
        m_sockAddr.sin_addr.s_addr = INADDR_ANY;
    else
        INET_PTON(AF_INET, bindAddress.c_str(), &m_sockAddr.sin_addr);

    if (m_sockAddr.sin_addr.s_addr == INADDR_NONE)
    {
        sLog->Error("NET: Invalid bind address specified. Please, specify valid IPv4 address");
        return false;
    }

    if (bind(m_socket, (sockaddr*)&m_sockAddr, sizeof(m_sockAddr)) == -1)
    {
        sLog->Error("Unable to bind to %s:%u", bindAddress.c_str(), port);
        return false;
    }

    if (listen(m_socket, 10) == -1)
    {
        sLog->Error("Unable to create listen backlog");
        return false;
    }

#ifdef _WIN32
    u_long arg = 1;
    if (ioctlsocket(m_socket, FIONBIO, &arg) == SOCKET_ERROR)
#else
    int oldFlag = fcntl(m_socket, F_GETFL, 0);
    if (fcntl(m_socket, F_SETFL, oldFlag | O_NONBLOCK) == -1)
#endif
    {
        sLog->Error("Unable to switch server socket to nonblocking mode");
    }

    FD_ZERO(&m_sockSet);
    m_nfds = 0;

    return true;
}

void NetworkManager::Update()
{
    AcceptConnections();
    SendQueuedPackets();
    UpdateSessions();
    HandleQueuedPackets();
}

void NetworkManager::AcceptConnections()
{
    std::unique_lock<std::mutex> lck(m_netMutex);

    sockaddr_in sockInfo;
    int sockAddrLen = sizeof(sockInfo);
    char tmpaddr[INET_ADDRSTRLEN];

    Session* sess;

    SOCK res = accept(m_socket, (sockaddr*)&sockInfo, &sockAddrLen);
    int error = LASTERROR();

    if (res == INVALID_SOCKET && error == SOCKETWOULDBLOCK)
    {
        // no incoming connections
    }
    else if (res == INVALID_SOCKET && error != SOCKETWOULDBLOCK)
    {
        // socket error
        sLog->Error("Unhandled socket error %u", LASTERROR());
    }
    else
    {
        sess = new Session(res, sockInfo, GenerateSessionId());

        m_sessionSet.insert(sess);
        m_sessionsBySessionId[sess->GetSessionId()] = sess;
        m_sessionsBySocket[res] = sess;

        FD_SET(res, &m_sockSet);
        if (res > m_nfds)
            m_nfds = (int)res;

        INET_NTOP(AF_INET, &sockInfo.sin_addr, tmpaddr, INET_ADDRSTRLEN);

        sess->SetRemoteAddr(tmpaddr);

        /*
#ifdef _WIN32
        u_long arg = WINSOCK_NONBLOCKING_ARG;
        if (ioctlsocket(res, FIONBIO, &arg) == SOCKET_ERROR)
#else
        int oldFlag = fcntl(m_socket, F_GETFL, 0);
        if (fcntl(res, F_SETFL, oldFlag | O_NONBLOCK) == -1)
#endif
        {
            sLog->Error("Failed to switch client socket to non-blocking mode");
        }
        */

        std::cout << "Accepting connection from " << tmpaddr << std::endl;
    }
}

void CloseSocket_gen(SOCK socket)
{
#ifdef _WIN32
    shutdown(socket, SD_BOTH);
    //closesocket(socket);
#else
    close(socket);
#endif
}

void NetworkManager::SendQueuedPackets()
{
    for (std::set<Session*>::iterator itr = m_sessionSet.begin(); itr != m_sessionSet.end(); ++itr)
        (*itr)->SendQueuedPackets();
}

void NetworkManager::HandleQueuedPackets()
{
    for (std::set<Session*>::iterator itr = m_sessionSet.begin(); itr != m_sessionSet.end(); ++itr)
        (*itr)->HandleQueuedPackets();
}

void NetworkManager::UpdateSessions()
{
    std::unique_lock<std::mutex> lck(m_netMutex);

    if (m_sessionSet.empty())
        return;

    uint16_t header_buf[2];
    uint8_t* recvdata;
    int result, error;
    Session* sess;
    SmartPacket* pkt;
    time_t tmout;

    fd_set rdfds;
    memcpy(&rdfds, &m_sockSet, sizeof(fd_set));

    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100; // 0.1ms

    result = select(m_nfds + 1, &rdfds, nullptr, nullptr, &tv);
    if (result < 0)
    {
        // error
        sLog->Error("Socket error: %i", LASTERROR());
        return;
    }
    else if (result == 0)
    {
        // timeout
        return;
    }

    for (std::set<Session*>::iterator itr = m_sessionSet.begin(); itr != m_sessionSet.end(); )
    {
        sess = (*itr);

        // if the session is marked as expired, disconnect client
        if (sess->IsMarkedAsExpired())
        {
            sLog->Info("Client session (IP: %s) expired, disconnecting", sess->GetRemoteAddr());
            CloseSocket_gen(sess->GetSocket());
            itr = RemoveSession(itr);
            continue;
        }

        // if session is marked for expiration, wait for it
        // expired sessions are not valid anymore - they are just kept in list for possible retrieval
        // by another session
        if ((tmout = sess->GetSessionTimeoutValue()) != 0)
        {
            if (tmout < time(nullptr))
                sess->Kick();

            ++itr;
            continue;
        }

        if (!FD_ISSET(sess->GetSocket(), &rdfds))
        {
            ++itr;
            continue;
        }

        result = recv(sess->GetSocket(), (char*)&header_buf, SmartPacket::HeaderSize, 0);
        error = LASTERROR();

        // some data available
        if (result > 0)
        {
            header_buf[0] = ntohs(header_buf[0]);
            header_buf[1] = ntohs(header_buf[1]);

            // size read must be equal to header length
            if (result == SmartPacket::HeaderSize && header_buf[1] < SmartPacket::MaxPacketSize)
            {
                recvdata = nullptr;

                // packet contents may be empty as well
                if (header_buf[1] > 0)
                {
                    // following memory is deallocated in SmartPacket destructor, or in near error handler
                    recvdata = new uint8_t[header_buf[1]];
                    result = recv(sess->GetSocket(), (char*)recvdata, header_buf[1], 0);
                    error = LASTERROR();

                    // malformed packet - received less bytes than expected
                    if (result != (int)header_buf[1])
                    {
                        delete[] recvdata;

                        sLog->Error("Received malformed packet: opcode %u, size %u, real size %u; disconnecting client (IP: %s)", header_buf[0], header_buf[1], result, sess->GetRemoteAddr());
                        //CloseSocket_gen(sess->GetSocket());
                        itr = RemoveSession(itr);
                        continue;
                    }
                }

                // build packet (this will cause previous packet destructor call and new packet constructor call)
                pkt = new SmartPacket(header_buf[0], header_buf[1]);

                // pass the data, if any
                if (header_buf[1] > 0)
                    pkt->SetData(recvdata, header_buf[1]);

                // and let the session handle the packet - cleanup is done in GamePacket destructor
                //sess->HandlePacket(pkt);
                sess->AddPacketToHandleQueue(pkt);
            }
            else
            {
                sLog->Error("Received malformed packet: no valid headers sent; disconnecting client (IP: %s)", sess->GetRemoteAddr());
                CloseSocket_gen(sess->GetSocket());
                itr = RemoveSession(itr);
                continue;
            }
        }
        // connection abort, this may be due to network error
        else if (error == SOCKETCONNABORT)
        {
            sess->Kick();
            // set timeout if necessary
            /*if (!sess->GetSessionTimeoutValue())
            {
                sess->SetSessionTimeoutValue(SESSION_INACTIVITY_EXPIRE);
                sLog->Error("Client (IP: %s) aborted connection, marking session as expired and waiting for timeout", sess->GetRemoteAddr());
            }*/
        }
        // connection closed by remote endpoint (either controlled or errorneous scenario, but initiated by client)
        else if (error == SOCKETCONNRESET)
        {
            sess->Kick();
            /*if (sess->GetPlayer())
            {
                // set timeout if necessary
                if (!sess->GetSessionTimeoutValue())
                {
                    sess->SetSessionTimeoutValue(SESSION_INACTIVITY_EXPIRE);
                    sLog->Debug("Client (IP: %s) disconnected in room, marking session as expired and waiting for timeout", sess->GetRemoteAddr());
                }

                ++itr;
            }
            else
            {
                sLog->Debug("Client (IP: %s) disconnected", sess->GetRemoteAddr());
                itr = RemoveSession(itr);
            }*/

            continue;
        }
        else
        {
            // just check, if the error is caused by nonblocking socket, that would block, or that there's
            // no error at all (should not happen due to previous condition blocks, technically result == 0 means,
            // that we have some error set)
            if (error != SOCKETWOULDBLOCK && error != 0)
            {
                sLog->Error("Unhandled socket error: %u; disconnecting client (IP: %s)", error, sess->GetRemoteAddr());
                CloseSocket_gen(sess->GetSocket());
                itr = RemoveSession(itr);
                continue;
            }
        }

        ++itr;
    }
}

std::set<Session*>::iterator NetworkManager::RemoveSession(std::set<Session*>::iterator itr)
{
    m_sessionsBySessionId.erase((*itr)->GetSessionId());
    m_sessionsBySocket.erase((*itr)->GetSocket());

    return m_sessionSet.erase(itr);
}

void NetworkManager::SendPacket(SmartPacket &pkt, SOCK targetSocket)
{
    uint16_t op, sz;
    uint8_t* tosend = new uint8_t[SmartPacket::HeaderSize + pkt.GetSize()];

    sLog->Debug("Sending packet %u to socket %u", pkt.GetOpcode(), targetSocket);

    op = htons(pkt.GetOpcode());
    sz = htons(pkt.GetSize());

    // write opcode
    memcpy(tosend, &op, 2);
    // write contents size
    memcpy(tosend + 2, &sz, 2);
    // write contents
    memcpy(tosend + 4, pkt.GetData(), pkt.GetSize());

    // send everything
    SendPacket(SmartPacket::HeaderSize + pkt.GetSize(), tosend, targetSocket);
}

bool NetworkManager::SendPacket(int len, uint8_t* data, SOCK targetSocket)
{
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(targetSocket, &wfds);

    int result = select((int)targetSocket + 1, nullptr, &wfds, nullptr, nullptr);

    if (result < 0)
    {
        sLog->Error("select(): error (%i) when trying to send packet", result);
        return false;
    }
    else if (result == 0 || !FD_ISSET(targetSocket, &wfds))
    {
        // socket is busy, buffer full, or something other would block send, leave data in queue
        return false;
    }

    int offset = 0;
    while (offset < len)
    {
        result = send(targetSocket, (const char*)data + offset, len - offset, MSG_NOSIGNAL);
        if (result <= 0)
            break;
        offset += result;
    }

    if (result == -1)
    {
        if (LASTERROR() == SOCKETCONNABORT || LASTERROR() == SOCKETCONNRESET)
        {
            if (!m_sessionsBySocket[targetSocket]->IsMarkedAsExpired())
                m_sessionsBySocket[targetSocket]->Kick();
        }
        else
        {
            sLog->Error("send(): error %u", LASTERROR());
        }
        return false;
    }

    return true;
}

bool NetworkManager::SendPacketToSession(SmartPacket &pkt, uint32_t sessionId)
{
    std::unique_lock<std::mutex> lck(m_netMutex);

    if (m_sessionsBySessionId.find(sessionId) == m_sessionsBySessionId.end())
        return false;

    SendPacket(pkt, m_sessionsBySessionId[sessionId]->GetSocket());
    return true;
}

uint32_t NetworkManager::GenerateSessionId()
{
    // TODO: rework to more long-term stuff (hole-filling, etc.)

    return ++m_maxSessionId;
}

