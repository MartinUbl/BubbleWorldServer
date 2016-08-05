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

#ifndef BW_ANIM_ENUMS_H
#define BW_ANIM_ENUMS_H

// recognized animation types
enum AnimTypes
{
    ANIM_IDLE = 0,
    ANIM_WALK_UP = 1,
    ANIM_WALK_RIGHT = 2,
    ANIM_WALK_DOWN = 3,
    ANIM_WALK_LEFT = 4,
    ANIM_WALK_UPRIGHT = 5,
    ANIM_WALK_DOWNRIGHT = 6,
    ANIM_WALK_DOWNLEFT = 7,
    ANIM_WALK_UPLEFT = 8,
};

// does supplied animation ID belong to movement animation?
static const bool _isMovementAnim(uint32_t animId)
{
    if (animId >= ANIM_WALK_UP && animId <= ANIM_WALK_UPLEFT)
        return true;
    return false;
}

#endif
