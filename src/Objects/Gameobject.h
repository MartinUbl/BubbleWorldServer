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

#ifndef BW_GAMEOBJECT_H
#define BW_GAMEOBJECT_H

#include "WorldObject.h"

class SmartPacket;

/*
 * Class representing static game object in game
 */
class Gameobject : public WorldObject
{
    public:
        Gameobject();
        virtual ~Gameobject();

        virtual void BuildCreatePacketBlock(SmartPacket &pkt);

        virtual void Create(uint32_t guidLow, uint32_t entry);

        virtual void Update();

    protected:
        virtual void CreateUpdateFields();

    private:
        //
};

#endif
