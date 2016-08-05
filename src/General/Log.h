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
