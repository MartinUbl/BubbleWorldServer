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

#ifndef BW_MAP_ENUMS_H
#define BW_MAP_ENUMS_H

// version magic used in files
#define MAP_VERSION_MAGIC 0x000100FF

// default field type
#define DEFAULT_FIELD_TYPE          MFT_WATER
// default field texture used
#define DEFAULT_FIELD_TEXTURE       3
// default field flags used
#define DEFAULT_FIELD_FLAGS         0

// length of map name field in header
#define MAP_NAME_LENGTH 64

// chunk size (field count per chunk)
#define MAP_CHUNK_SIZE_X 40
// chunk size (field count per chunk)
#define MAP_CHUNK_SIZE_Y 40

// cell size equals chunk size
#define MAP_CELL_SIZE_X MAP_CHUNK_SIZE_X
// cell size equals chunk size
#define MAP_CELL_SIZE_Y MAP_CHUNK_SIZE_Y

// how many cells are considered "sorrounding" in X direction
#define MAP_SORROUNDING_CELLS_X 2
// how many cells are considered "sorrounding" in Y direction
#define MAP_SORROUNDING_CELLS_Y 2

// how many object updates should be in single packet
#define UPDATEPACKET_COUNT_LIMIT 50

// width of map field
#define MAP_FIELD_PX_SIZE_X 32
// height of map field
#define MAP_FIELD_PX_SIZE_Y 32

// type of map field
enum MapFieldType
{
    MFT_GROUND = 0,    // ground, player and NPCs can walk there
    MFT_SOLID = 1,    // solid stuff for no apparent reason, nobody can walk here
    MFT_WATER = 2,    // water, nobody can walk there, but somebody could swim
    MFT_LAVA = 3,    // ouch! nobody can walk, but when somebody tries to swim, it hurts
    MAX_MFT
};

// map field flags
enum MapFieldFlags
{
    MFF_SOIL = 0x00000001,   // can grow plants here
    MFF_TRIGGER = 0x00000002,   // this field triggers some action when stood upon
};

#endif
