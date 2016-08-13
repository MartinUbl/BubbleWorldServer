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

#ifndef BW_GENERAL_H
#define BW_GENERAL_H

// common headers
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <string>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdint.h>
#include <codecvt>
#include <locale>

// platform-specific headers
#ifdef _WIN32
#include <WS2tcpip.h>
#include <Windows.h>
#else
//
#endif

#include "Compatibility.h"
#include "SharedEnums.h"

// global defines
#ifdef _DEBUG
#define DATA_DIR "bin/resources/"
#else
#define DATA_DIR "resources/"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#ifndef F_PI
#define F_PI ((float)M_PI)
#endif

// this app version
#define APP_VERSION 1
// minimum accepted client version number
#define APP_MIN_ACCEPT_VERSION 1
// maximum accepted client version number
#define APP_MAX_ACCEPT_VERSION 1

#endif
