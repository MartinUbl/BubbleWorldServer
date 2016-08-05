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

#ifndef BW_COMPATIBILITY_H
#define BW_COMPATIBILITY_H

#define num_min(a,b) (a<b?a:b)
#define num_max(a,b) (a>b?a:b)

// retrieves time in milliseconds; used for comparisons with another number retrieved this way,
// not for timing by real time!
#ifdef _WIN32
inline unsigned int getMSTime() { return GetTickCount(); }
#else
inline uint32 getMSTime()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

// retrieves time difference in milliseconds
inline unsigned int getMSTimeDiff(unsigned int oldMSTime, unsigned int newMSTime)
{
    // getMSTime() have limited data range and this is case when it overflow in this tick
    if (oldMSTime > newMSTime)
        return (0xFFFFFFFF - oldMSTime) + newMSTime;
    else
        return newMSTime - oldMSTime;
}

// creates directory
inline bool custom_createDirectory(const char* path)
{
#ifdef _WIN32
    if (CreateDirectoryA(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
#else
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
#endif
        return true;
    else
        return false;
}

#endif
