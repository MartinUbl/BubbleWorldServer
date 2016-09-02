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
#include "Vector2.h"

#include <math.h>

Vector2::Vector2()
{
    x = 0.0f;
    y = 0.0f;
}

Vector2::Vector2(float _x, float _y)
{
    x = _x;
    y = _y;
}

Vector2::Vector2(const Vector2 &vec)
{
    x = vec.x;
    y = vec.y;
}

Vector2 Vector2::operator*(float m)
{
    return Vector2(x*m, y*m);
}

Vector2 Vector2::operator/(float m)
{
    return Vector2(x/m, y/m);
}

Vector2 Vector2::operator+(Vector2 const& sec)
{
    return Vector2(x + sec.x, y + sec.y);
}

Vector2 Vector2::operator-(Vector2 const& sec)
{
    return Vector2(x - sec.x, y - sec.y);
}

float Vector2::operator*(Vector2 const& sec)
{
    return x*sec.x + y*sec.y;
}

Vector2& Vector2::operator*=(float m)
{
    x *= m;
    y *= m;
    return *this;
}

Vector2& Vector2::operator/=(float m)
{
    x /= m;
    y /= m;
    return *this;
}

Vector2& Vector2::operator+=(Vector2 const& sec)
{
    x += sec.x;
    y += sec.y;
    return *this;
}

Vector2& Vector2::operator-=(Vector2 const& sec)
{
    x -= sec.x;
    y -= sec.y;
    return *this;
}

void Vector2::SetFromPolar(float angleRad, float distance)
{
    x = cos(angleRad) * distance;
    y = sin(angleRad) * distance;
}

void Vector2::MakeUnit()
{
    float size = sqrt(x*x + y*y);
    x /= size;
    y /= size;
}
