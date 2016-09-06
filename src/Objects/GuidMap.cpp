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
#include "GuidMap.h"

#define CHUNK_BITS (sizeof(uint64_t)*CHAR_BIT)

GuidMap::GuidMap()
{
    // make sure everything is zero-initialized
    memset(this, 0, sizeof(GuidMap));
}

GuidMap::~GuidMap()
{
    for (uint64_t i = 0; i < m_sliceCount; i++)
        free(m_slices[i]);

    free(m_slices);
}

void GuidMap::Init(uint64_t sliceBits)
{
    m_sliceSize = (sliceBits / CHUNK_BITS) + !!(sliceBits % CHUNK_BITS);

    // skip GUID 0, it is used as special value and musn't be assigned
    SetBit(0);
}

void GuidMap::SetBit(int64_t index)
{
    uint64_t sliceOffset = index / (m_sliceSize*CHUNK_BITS);
    uint64_t sliceIndex = index % (m_sliceSize*CHUNK_BITS);

    // if slice offset exceeds allocated slices, allocate one more slice
    while (sliceOffset + 1 > m_sliceCount)
        _AddSlice();

    _SetBit(m_slices[sliceOffset], sliceIndex);
}

bool GuidMap::GetBit(int64_t index)
{
    uint64_t sliceOffset = index / (m_sliceSize*CHUNK_BITS);
    uint64_t sliceIndex = index % (m_sliceSize*CHUNK_BITS);

    // if slice offset exceeds allocated slices, it surely isn't taken
    if (sliceOffset + 1 > m_sliceCount)
        return false;

    return _GetBit(m_slices[sliceOffset], sliceIndex);
}

int64_t GuidMap::UseEmpty()
{
    uint64_t i;
    int64_t ret;

    // try to find empty bit in current slice set
    for (i = 0; i < m_sliceCount; i++)
    {
         ret = _FindEmpty(m_slices[i], m_sliceSize);
        if (ret != -1)
        {
            _SetBit(m_slices[i], ret);
            return i*m_sliceSize*CHUNK_BITS + ret;
        }
    }

    // allocate another slice and take the first bit there
    _AddSlice();
    _SetBit(m_slices[i], 0);
    return i*m_sliceSize*CHUNK_BITS;
}

void GuidMap::_SetBit(uint64_t *arr, int64_t idx)
{
    arr[idx / CHUNK_BITS] |= ((uint64_t)1LL << (idx % CHUNK_BITS));
}

bool GuidMap::_GetBit(uint64_t *arr, int64_t idx)
{
    return (arr[idx / CHUNK_BITS] & ((uint64_t)1LL << (idx % CHUNK_BITS))) != 0;
}

int64_t GuidMap::_FindEmpty(uint64_t *arr, uint64_t arrsize)
{
    uint64_t i, j;

    // skip full chunks
    for (i = 0; i < arrsize; i++)
    {
        if (arr[i] != ~(uint64_t)0)
            break;
    }

    // we are full, no space available at all
    if (i == arrsize)
        return -1;

    // we found first free long, find empty space within
    for (j = 0; j < CHUNK_BITS; j++)
    {
        if (~arr[i] & (uint64_t)1LL << j)
            break;
    }

    // resulting bit offset is i*bits_in_chunk + j
    return i*CHUNK_BITS + j;
}

bool GuidMap::_AddSlice()
{
    uint64_t arrayNum;
    uint64_t *newarray, **arrays;

    // allocate new slice, initialize to zero
    newarray = (uint64_t*)calloc(m_sliceSize, sizeof(uint64_t));
    if (newarray == NULL)
        return false;

    // resize slices array to be able to contain new slice
    arrayNum = m_sliceCount + 1;
    arrays = (uint64_t**)realloc(m_slices, arrayNum*sizeof(uint64_t*));
    if (arrays == NULL)
    {
        free(newarray);
        return false;
    }

    // add new slice
    arrays[arrayNum - 1] = newarray;

    // update bitarray struct only when all allocations passed
    m_slices = arrays;
    m_sliceCount++;

    return true;
}
