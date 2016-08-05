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

#ifndef BW_MAP_MANAGER_H
#define BW_MAP_MANAGER_H

#include "Singleton.h"
#include "Map.h"

/*
 * Singleton class used for maintaining all active maps
 */
class MapManager
{
    friend class Singleton<MapManager>;
    public:
        ~MapManager();

        // retrieves map; if not created, create it
        Map* GetMap(uint32_t mapId);
        // update all loaded maps
        void UpdateMaps();

    protected:
        // protected singleton constructor
        MapManager();

    private:
        // all created maps map
        std::unordered_map<uint32_t, Map*> m_createdMaps;
};

#define sMapManager Singleton<MapManager>::getInstance()

#endif
