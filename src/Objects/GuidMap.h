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

#ifndef BW_GUIDMAP_H
#define BW_GUIDMAP_H

/*
 * Class used as bitmap for specific guidspace
 */
class GuidMap
{
    public:
        GuidMap();
        ~GuidMap();

        // initializes guidmap using supplied granularity
        void Init(uint64_t sliceBits);
        // sets bit in guidmap
        void SetBit(int64_t index);
        // finds empty spot, marks as used and returns its index (allocated GUID)
        int64_t UseEmpty();
        // retrieves bit value (is GUID in use?)
        bool GetBit(int64_t index);

    private:
        // internal method for setting bit value
        void _SetBit(uint64_t *arr, int64_t idx);
        // internal method for retrieving bit value
        bool _GetBit(uint64_t *arr, int64_t idx);
        // internal method for finding empty spot in bitmap
        int64_t _FindEmpty(uint64_t *arr, uint64_t arrsize);
        // adds next slice (extends guidmap)
        bool _AddSlice();

        // slice map (bitmap divided into 64bit smaller maps)
        uint64_t **m_slices;
        // total slice count at the moment
        uint64_t m_sliceCount;
        // current slice size
        uint64_t m_sliceSize;
};

#endif
