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

#ifndef BW_CRC32_H
#define BW_CRC32_H

#include <stdint.h>
#include <string>

// convert integer representation to hexadecimal string
std::string GetCRC32String(uint32_t crc);
// compute part of CRC32 routine, do not finalize; used for stream checksum
uint32_t CRC32_Bytes_Continuous(uint8_t* data, uint32_t count, uint32_t crc);
// finalize CRC32 continuous calculation
uint32_t CRC32_Bytes_ContinuousFinalize(uint32_t crc);
// get CRC32 checksum of supplied byte array
uint32_t CRC32_Bytes(uint8_t* data, uint32_t count);
// get CRC32 checksum of file
uint32_t CRC32_File(FILE* f);

#endif
