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

#ifndef BW_UPDATEFIELDS_H
#define BW_UPDATEFIELDS_H

enum ObjectUpdateFields
{
    OBJECT_FIELD_GUID                           = 0x0000,   // size: 2*32b
    OBJECT_FIELD_IMAGEID                        = 0x0002,
    OBJECT_FIELDS_END                           = 0x0003
};

enum UnitUpdateFields
{
    UNIT_FIELD_LEVEL                            = OBJECT_FIELDS_END + 0x0000,
    UNIT_FIELD_MOVEMENT_SPEED                   = OBJECT_FIELDS_END + 0x0001,
    UNIT_FIELDS_END                             = OBJECT_FIELDS_END + 0x0002
};

enum PlayerUpdateFields
{
    PLAYER_FIELDS_END                           = UNIT_FIELDS_END + 0x0000
};

enum GameObjectUpdateFields
{
    GAMEOBJECT_FIELDS_END                       = OBJECT_FIELDS_END + 0x0000
};

#endif
