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
#include "Player.h"

/*
 * Name:            Basic walking NPC test script
 * Description:     Testing script for early stages of development
 */
class BasicWalkNPC : public CreatureScript
{
    public:
        BasicWalkNPC(Creature* c) : dialogue(c->GetGUID()), CreatureScript(c) { }

        void OnCreate() override
        {
            dialogue.AddDialogueAction_Decision(1, 0, L"Co má udìlat?", 1, L"Jen tak si chodit", 2, L"Zkus jít po cestì", 3, L"Náhoda", 4, L"Pojï za mnou!", 5, L"Zastavit!");
            dialogue.AddDialogueAction_EndDialogue(2);
        }

        void OnUpdate() override
        {
            //
        }

        void OnInteract(Player* pl) override
        {
            dialogue.StartDialogue(pl->GetGUID());
        }

        void OnDialogueDecision(Player* pl, uint32_t decision) override
        {
            dialogue.EndDialogue(pl->GetGUID());

            if (decision == 1)
            {
                ScriptSay(L"Tak sleduj...");
                self->GetMotionMaster().MovePoint(32, 25);
            }
            else if (decision == 2)
            {
                ScriptSay(L"Jdu po cestì");
                self->GetMotionMaster().MoveWaypointPath(1);
            }
            else if (decision == 3)
            {
                ScriptSay(L"Jen tak se tu náhodnì budu flákat");
                self->GetMotionMaster().MoveRandom(3.0f, 500, 1500);
            }
            else if (decision == 4)
            {
                ScriptSay(L"Tak jo");
                self->GetMotionMaster().MoveFollow(pl, 1.0f, 2.0f);
            }
            else if (decision == 5)
            {
                ScriptSay(L"STOP! Hammer time!");
                self->GetMotionMaster().MoveIdle();
            }
        }

        void OnMovementPointReached(uint32_t id) override
        {
            if (id == 2)
                ScriptSay(L"Boom!");
        }

    private:
        DialogueHolder dialogue;
};

void ScriptLoader_BasicWalkNPC()
{
    REGISTER_CREATURE_SCRIPT("test_basic_walk_npc", BasicWalkNPC);
}
