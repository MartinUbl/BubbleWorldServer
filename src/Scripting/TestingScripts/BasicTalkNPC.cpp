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
#include "ScriptManager.h"
#include "CreatureScript.h"
#include "Creature.h"

/*
 * Name:            Basic talk NPC test script
 * Description:     Testing script for early stages of development
 */
class BasicTalkNPC : public CreatureScript
{
    public:
        BasicTalkNPC(Creature* c) : CreatureScript(c) { }

        void OnCreate() override
        {
            m_testSayTime = 0;
        }

        void OnUpdate() override
        {
            // timer example

            // if the timer is set...
            if (m_testSayTime)
            {
                // ...and 5 seconds have passed since the timer start
                if (getMSTimeDiff(m_testSayTime, getMSTime()) >= 5000)
                {
                    // stop the timer
                    m_testSayTime = 0;
                    // say something nice
                    ScriptSay(L"A tohle je èasovaná zpráva!");
                }
            }
        }

        void OnInteract(Player* pl) override
        {
            ScriptSay(L"Teï mluví skript!");
            // start timer
            m_testSayTime = getMSTime();
        }

    private:
        // testing timer for timed message
        uint32_t m_testSayTime;
};

void ScriptLoader_BasicTalkNPC()
{
    REGISTER_CREATURE_SCRIPT("test_basic_talk_npc", BasicTalkNPC);
}
