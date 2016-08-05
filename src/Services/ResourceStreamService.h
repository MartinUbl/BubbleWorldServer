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

#ifndef BW_RESOURCESTREAMSERVICE_H
#define BW_RESOURCESTREAMSERVICE_H

#include "Singleton.h"

/*
 * Structure containing information about resource stream
 */
struct ResourceStreamRecord
{
    // resource type
    ResourceType type;
    // resource ID
    uint32_t id;
    // session ID that receives stream packets
    uint32_t sessionId;

    // opened file
    FILE* src;
};

// how many bytes can one stream packet contain
#define DEFAULT_STREAM_DATA_SIZE 1024

typedef std::queue<ResourceStreamRecord*> ResourceStreamQueue;
typedef std::map<uint32_t, ResourceStreamQueue> ResourceStreamSessionMap;

/*
 * Singleton class maintaining all resource streams from server to client
 */
class ResourceStreamService
{
    friend class Singleton<ResourceStreamService>;
    public:
        ~ResourceStreamService();

        // initialize resource stream manager - create thread
        bool Init();

        // start streaming of specified resource to session
        void StartStream(ResourceType type, uint32_t id, uint32_t sessionId);
        // terminates streaming of specified resource to session
        void TerminateStream(ResourceType type, uint32_t id, uint32_t sessionId);
        // terminates all streams
        void TerminateAllStreams(uint32_t sessionId);

        // updates streams (called from stream thread)
        void Update();

    protected:
        // protected singleton constructor
        ResourceStreamService();

        // sends next bunch of bytes in resource stream
        bool SendNextResourceStreamData(ResourceStreamRecord* rec);
        // finished streaming of resource
        void FinishStream(ResourceStreamRecord* rec);

    private:
        // is supposed to be running?
        bool m_running;

        // streaming thread instance
        std::thread* m_streamThread;
        // condition variable for waiting when no stream in progress
        std::condition_variable m_requestCond;
        // monitor mutex
        std::mutex m_requestMutex;

        // all streams map
        ResourceStreamSessionMap m_streamSessionMap;
};

#define sResourceStreamService Singleton<ResourceStreamService>::getInstance()

#endif
