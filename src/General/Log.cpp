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
#include "Log.h"
#include "Config.h"

#include <iostream>
#include <cstdarg>

Log::Log()
{
    m_logFile = nullptr;

    std::string logFileName = sConfig->GetStringValue(CONFIG_STRING_LOG_FILE);

    if (logFileName.length() == 0)
    {
        std::cout << "No log file specified, server messages won't be logged into file" << std::endl;
    }
    else
    {
        m_logFile = fopen(logFileName.c_str(), "w");
        if (!m_logFile)
            std::cerr << "Could not open log file " << logFileName.c_str() << " for writing! Server log will not be put into file!" << std::endl;
    }
}

Log::~Log()
{
    if (m_logFile)
        fclose(m_logFile);
}

void Log::FileLog(const char* str)
{
    if (m_logFile)
    {
        fputs(str, m_logFile);
        fputc('\n', m_logFile);
    }
}

void Log::Info(const char *str, ...)
{
    if ((sConfig->GetIntValue(CONFIG_INT_LOG_MASK) & LOG_MASK_INFO) == 0)
        return;

    va_list argList;
    va_start(argList, str);
    char buf[2048];
    vsnprintf(buf, 2048, str, argList);
    va_end(argList);
    std::cout << buf << std::endl;

    FileLog(buf);
}

void Log::Error(const char *str, ...)
{
    if ((sConfig->GetIntValue(CONFIG_INT_LOG_MASK) & LOG_MASK_ERROR) == 0)
        return;

    va_list argList;
    va_start(argList, str);
    char buf[2048];
    vsnprintf(buf, 2048, str, argList);
    va_end(argList);
    std::cerr << buf << std::endl;

    FileLog(buf);
}

void Log::Debug(const char *str, ...)
{
    if ((sConfig->GetIntValue(CONFIG_INT_LOG_MASK) & LOG_MASK_DEBUG) == 0)
        return;

    va_list argList;
    va_start(argList, str);
    char buf[2048];
    vsnprintf(buf, 2048, str, argList);
    va_end(argList);
    std::cout << buf << std::endl;

    FileLog(buf);
}

void Log::PacketIO(const char *str, ...)
{
    if ((sConfig->GetIntValue(CONFIG_INT_LOG_MASK) & LOG_MASK_PACKETIO) == 0)
        return;

    va_list argList;
    va_start(argList, str);
    char buf[2048];
    vsnprintf(buf, 2048, str, argList);
    va_end(argList);
    std::cout << buf << std::endl;

    FileLog(buf);
}

void Log::Network(const char *str, ...)
{
    if ((sConfig->GetIntValue(CONFIG_INT_LOG_MASK) & LOG_MASK_NETWORK) == 0)
        return;

    va_list argList;
    va_start(argList, str);
    char buf[2048];
    vsnprintf(buf, 2048, str, argList);
    va_end(argList);
    std::cout << buf << std::endl;

    FileLog(buf);
}
