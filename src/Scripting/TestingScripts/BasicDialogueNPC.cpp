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
 * Name:            Basic dialogue NPC test script
 * Description:     Testing script for early stages of development
 */
class BasicDialogueNPC : public CreatureScript
{
    public:
        BasicDialogueNPC(Creature* c) : dialogue(c->GetGUID()), CreatureScript(c) { }

        // IDs used for dialogue actions
        enum MyTalkIDs
        {
            DLG_INTRO = 1,
            DLG_INTRO_2,
            DLG_INTRO_3,
            DLG_INTRO_4,
            DLG_QUESTION_1,
            DLG_THANKS_1,
            DLG_THANKS_2,
            DLG_THANKS_END,
            DLG_BE_RUDE_1,
            DLG_BE_RUDE_2,
            DLG_BE_RUDE_END
        };

        // IDs used for possible decisions
        enum MyDecisionIDs
        {
            DEC_THANKS = 1,
            DEC_BE_RUDE = 2
        };

        void OnCreate() override
        {
            // we will start at DLG_INTRO action
            dialogue.SetStartingAction(DLG_INTRO);

            // add some talk stuff
            dialogue.AddDialogueAction_Talk(DLG_INTRO, 2000, TALK_SAY, L"Vítej, cizinèe!");
            dialogue.AddDialogueAction_Talk(DLG_INTRO_2, 2000, TALK_SAY, L"Toto je tvùj nový domov!");
            dialogue.AddDialogueAction_Talk(DLG_INTRO_3, 3500, TALK_SAY, L"Mé jméno je George Packard a jsem vrchním radou zdejší detektivní jednotky.");
            dialogue.AddDialogueAction_Talk(DLG_INTRO_4, 2000, TALK_SAY, L"Doufám, že se ti zde bude líbit, a že se brzy potkáme znovu!");

            // then we add some decision action
            dialogue.AddDialogueAction_Decision(DLG_QUESTION_1, 2000, L"Co odpovìdìt?", DEC_THANKS, L"Dìkuji!", DEC_BE_RUDE, L"Trhni si, páprdo!");

            // append "thanks" decision flow
            dialogue.AddDialogueAction_Talk(DLG_THANKS_1, 3000, TALK_SAY, L"Tak pøeji hodnì štìstí");
            dialogue.AddDialogueAction_Talk(DLG_THANKS_2, 3000, TALK_SAY, L"Já se zatím budu vìnovat svému pøípadu");
            dialogue.AddDialogueAction_EndDialogue(DLG_THANKS_END);

            // append "be rude" decision flow
            dialogue.AddDialogueAction_Talk(DLG_BE_RUDE_1, 3000, TALK_SAY, L"Jak to se mnou mluvíš?!");
            dialogue.AddDialogueAction_TalkPlayer(DLG_BE_RUDE_2, 3000, TALK_SAY, L"No poèkej, já ti ještì ukážu");
            dialogue.AddDialogueAction_EndDialogue(DLG_BE_RUDE_END);

            // set relationships between decisions and consequences (actions)

            // when selecting "thanks" decision, use DLG_THANKS_1 action
            dialogue.AddDecisionActionRelationship(DEC_THANKS, DLG_THANKS_1);
            // when selecting "be rude" decision, use DLG_BE_RUDE_1 action
            dialogue.AddDecisionActionRelationship(DEC_BE_RUDE, DLG_BE_RUDE_1);
        }

        void OnUpdate() override
        {
            // update dialogue holder
            dialogue.Update();
        }

        void OnInteract(Player* pl) override
        {
            // start dialogue - sends the first action, dialogue start packets, etc.
            dialogue.StartDialogue(pl->GetGUID());
        }

        void OnDialogueDecision(Player* source, uint32_t decision) override
        {
            // just pass the decision to dialogue holder
            dialogue.DialogueDecision(source->GetGUID(), decision);

            // there's also space for custom, non-standard behaviour
        }

    private:
        // creature script dialogue holder
        DialogueHolder dialogue;
};

void ScriptLoader_BasicDialogueNPC()
{
    REGISTER_CREATURE_SCRIPT("test_basic_dialogue_npc", BasicDialogueNPC);
}
