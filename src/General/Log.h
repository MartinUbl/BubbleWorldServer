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

#ifndef BW_LOG_H
#define BW_LOG_H

#include "Singleton.h"

// type of log message
enum LogType
{
    LOG_INFO        = 0,
    LOG_ERROR       = 1,
    LOG_DEBUG       = 2,
    LOG_PACKETIO    = 3,
    LOG_NETWORK     = 4,
    MAX_LOG_TYPE
};

// mask built from log types
enum LogTypeMask
{
    LOG_MASK_INFO = 1 << LOG_INFO,
    LOG_MASK_ERROR = 1 << LOG_ERROR,
    LOG_MASK_DEBUG = 1 << LOG_DEBUG,
    LOG_MASK_PACKETIO = 1 << LOG_PACKETIO,
    LOG_MASK_NETWORK = 1 << LOG_NETWORK,
};

/*
 * Singleton class maintaining logging services
 */
class Log
{
    friend class Singleton<Log>;
    public:
        ~Log();

        // logs string with INFO severity
        void Info(const char *str, ...);
        // logs string with ERROR severity
        void Error(const char *str, ...);
        // logs string with DEBUG severity
        void Debug(const char *str, ...);
        // logs string with PacketIO type
        void PacketIO(const char *str, ...);
        // logs string with Network type
        void Network(const char *str, ...);

    protected:
        // protected singleton constructor
        Log();

        // method that logs into file
        void FileLog(const char* str);

    private:
        // logfile opened
        FILE* m_logFile;
};

#define sLog Singleton<Log>::getInstance()

#endif
