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

#ifndef BW_CREATURE_H
#define BW_CREATURE_H

#include "Unit.h"

class CreatureScript;
class Player;

/*
 * Class representing NPC object in game
 */
class Creature : public Unit
{
    public:
        Creature();
        virtual ~Creature();

        // sets creature spawn position
        void SetSpawnPosition(float x, float y);
        // retrieves creature spawn position
        Position const& GetSpawnPosition();

        virtual void Create(uint32_t guidLow, uint32_t entry);

        virtual void Update();

        virtual void Interact(Player* player);
        virtual void DialogueDecision(Player* player, uint32_t decision);
        virtual void MovementGeneratorPointReached(uint32_t pointId);

    protected:
        virtual void CreateUpdateFields();

        // creature script, if any
        CreatureScript* m_script;
        // position where the creature was originally spawned
        Position m_spawnPosition;

    private:
        //
};

#endif
