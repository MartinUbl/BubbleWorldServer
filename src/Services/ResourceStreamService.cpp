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
#include "ResourceStorage.h"
#include "ResourceStreamService.h"
#include "SmartPacket.h"
#include "NetworkManager.h"
#include "Log.h"

ResourceStreamService::ResourceStreamService()
{
    m_running = false;
}

ResourceStreamService::~ResourceStreamService()
{
    //
}

bool ResourceStreamService::Init()
{
    m_running = true;
    m_streamThread = new std::thread(&ResourceStreamService::Update, this);

    return true;
}

void ResourceStreamService::StartStream(ResourceType type, uint32_t id, uint32_t sessionId)
{
    ResourceStreamRecord* rec = new ResourceStreamRecord;

    rec->type = type;
    rec->id = id;
    rec->sessionId = sessionId;

    ResourceRecord* resrec = sResourceStorage->GetResource(type, id);
    if (!resrec)
    {
        sLog->Error("ResourceStreamService: could not find resource type %u, id %u", type, id);
        delete rec;
        return;
    }

    rec->src = fopen((std::string(DATA_DIR) + resrec->filename).c_str(), "rb");
    if (!rec->src)
    {
        sLog->Error("ResourceStreamService: could not open file '%s' for resource type %u, id %u", resrec->filename.c_str(), type, id);
        delete rec;
        return;
    }

    SmartPacket pkt(SP_RESOURCE_SEND_START);
    pkt.WriteString(resrec->filename.c_str());
    pkt.WriteUInt8(type);
    pkt.WriteUInt32(id);
    sNetwork->SendPacketToSession(pkt, sessionId);

    std::unique_lock<std::mutex> lck(m_requestMutex);

    m_streamSessionMap[sessionId].push(rec);
    m_requestCond.notify_all();
}

void ResourceStreamService::FinishStream(ResourceStreamRecord* rec)
{
    fclose(rec->src);

    SmartPacket pkt(SP_RESOURCE_SEND_FINISHED);
    pkt.WriteUInt8(rec->type);
    pkt.WriteUInt32(rec->id);
    sNetwork->SendPacketToSession(pkt, rec->sessionId);
}

void ResourceStreamService::TerminateStream(ResourceType type, uint32_t id, uint32_t sessionId)
{
    // TODO
}

void ResourceStreamService::TerminateAllStreams(uint32_t sessionId)
{
    // TODO
}

void ResourceStreamService::Update()
{
    ResourceStreamRecord* rec;

    while (m_running)
    {
        std::unique_lock<std::mutex> lck(m_requestMutex);

        if (m_streamSessionMap.empty())
        {
            m_requestCond.wait(lck);
            continue;
        }

        for (ResourceStreamSessionMap::iterator itr = m_streamSessionMap.begin(); itr != m_streamSessionMap.end(); )
        {
            if (itr->second.empty())
            {
                itr = m_streamSessionMap.erase(itr);
                continue;
            }
            else
            {
                rec = itr->second.front();
                if (!SendNextResourceStreamData(rec))
                    itr->second.pop();
            }
        }
    }
}

bool ResourceStreamService::SendNextResourceStreamData(ResourceStreamRecord* rec)
{
    uint8_t* data = new uint8_t[DEFAULT_STREAM_DATA_SIZE];

    size_t res = fread(data, 1, DEFAULT_STREAM_DATA_SIZE, rec->src);
    if (res == 0)
    {
        FinishStream(rec);
        return false;
    }

    SmartPacket pkt(SP_RESOURCE_DATA);
    pkt.WriteUInt8(rec->type);
    pkt.WriteUInt32(rec->id);
    pkt.WriteUInt16((uint16_t)res);
    for (size_t i = 0; i < res; i++)
        pkt.WriteUInt8(data[i]);
    sNetwork->SendPacketToSession(pkt, rec->sessionId);

    delete data;

    return true;
}
