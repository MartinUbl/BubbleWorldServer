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

#ifndef BW_VECTOR2_H
#define BW_VECTOR2_H

/*
 * 2D vector structure
 */
struct Vector2
{
    // non-parametric constructor (puts zeroes to coordinates)
    Vector2();
    // constructor with coordinate initialization
    Vector2(float _x, float _y);
    // copy constructor
    Vector2(const Vector2 &vec);

    // TODO: more vector operations

    // transforms polar coordinates (angle and distance) to relative coordinates
    void SetFromPolar(float angleRad, float distance);

    // X coordinate
    float x;
    // Y coordinate
    float y;
};

#endif
