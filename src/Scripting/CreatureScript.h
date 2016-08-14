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

#ifndef BW_CREATURE_SCRIPT_H
#define BW_CREATURE_SCRIPT_H

#include "DialogueHolder.h"

class Creature;
class Player;

/*
 * Class containing all script methods ready to be overriden by child classes
 */
class CreatureScript
{
    public:
        virtual ~CreatureScript();

        // called when the script is created (hooked to new creature instance)
        virtual void OnCreate() {};
        // called when the creature gets updated
        virtual void OnUpdate() {};
        // called when player interacts with creature
        virtual void OnInteract(Player* /*source*/) {};
        // called when player made dialogue decision
        virtual void OnDialogueDecision(Player* /*source*/, uint32_t /*decision*/) {};

        // performs talk event with all needed conversions, etc.
        void ScriptSay(const wchar_t* msg, Player* target = nullptr);

    protected:
        // protected constructor; instantiate child classes only
        CreatureScript(Creature* c);

        // pointer to script owner
        Creature* self;

    private:
        //
};

#endif
