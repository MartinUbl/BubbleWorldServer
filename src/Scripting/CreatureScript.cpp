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
#include "CreatureScript.h"
#include "Creature.h"

CreatureScript::CreatureScript(Creature* c) : self(c)
{
    //
}

CreatureScript::~CreatureScript()
{
    //
}

void CreatureScript::ScriptSay(const wchar_t* msg, Player* target)
{
    // messages hardcoded into scripts are encoded in some internal encoding,
    // so perform conversion to UTF-8 using generic function

    if (target)
        self->TalkTo(TALK_SAY, WStringToUTF8(msg).c_str(), target);
    else
        self->Talk(TALK_SAY, WStringToUTF8(msg).c_str());
}
