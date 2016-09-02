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
#include "Random.h"

#include <random>

// global random engine used
std::default_random_engine rnd((unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());

uint32_t urand(uint32_t min, uint32_t max)
{
    std::uniform_int_distribution<> dist(min, max);
    return (uint32_t)dist(rnd);
}

float frand(float min, float max)
{
    std::uniform_real_distribution<> dist(min, max);
    return (float)dist(rnd);
}
