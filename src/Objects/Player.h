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

#ifndef BW_PLAYER_H
#define BW_PLAYER_H

#include "Unit.h"

class Session;
class SmartPacket;

/*
 * Class representing player object in game
 */
class Player : public Unit
{
    public:
        Player();
        virtual ~Player();

        virtual void Create(uint32_t guidLow, Session* session);
        virtual void Update();

        // loads player from DB
        bool LoadFromDB();
        // saves player to DB
        void SaveToDB();

        // retrieves player session
        Session* GetSession();
        // sends packet to this player
        void SendPacketToMe(SmartPacket &pkt);

    protected:
        virtual void CreateUpdateFields();

    private:
        // player session
        Session* m_session;
};

#endif