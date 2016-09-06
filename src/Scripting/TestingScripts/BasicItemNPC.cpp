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
 * Name:            Basic item-giving-and-destroying NPC test script
 * Description:     Testing script for early stages of development
 */
class BasicItemNPC : public CreatureScript
{
    public:
        BasicItemNPC(Creature* c) : dialogue(c->GetGUID()), CreatureScript(c) { }

        void OnCreate() override
        {
            dialogue.AddDialogueAction_Decision(1, 0, L"Co má udìlat?", 1, L"Dát mi knížku", 2, L"Vzít si knížku", 3, L"Dej mi diamanty!", 4, L"Vezmi si diamanty!");
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
                ScriptSay(L"Tumáš!");
                pl->CreateItemInInventory(1, 1);
            }
            else if (decision == 2)
            {
                ScriptSay(L"Díky!");
                pl->RemoveItemFromInventory(1, 1, true, ITEM_INV_REMOVE_GIVE);
            }
            else if (decision == 3)
            {
                pl->CreateItemInInventory(3, 2);
            }
            else if (decision == 4)
            {
                pl->RemoveItemFromInventory(3, 4, true, ITEM_INV_REMOVE_GIVE);
            }
        }

    private:
        DialogueHolder dialogue;
};

void ScriptLoader_BasicItemNPC()
{
    REGISTER_CREATURE_SCRIPT("test_basic_item_npc", BasicItemNPC);
}
