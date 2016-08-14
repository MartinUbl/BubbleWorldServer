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

#ifndef BW_DIALOGUE_HOLDER_H
#define BW_DIALOGUE_HOLDER_H

// maximum dialogue entries system could handle
#define MAX_DIALOGUE_DECISION_ENTRIES 5

/*
 * Class extending standard vector of 32bit integers with some additional methods
 * used for handling decisions; the whole vector is just one giant bitfield
 */
class DecisionMap : public std::vector<uint32_t>
{
    public:
        // sets decision into decision map
        void SetDecision(uint32_t index);
        // clears decision from decision map
        void UnsetDecision(uint32_t index);
        // is the decision present?
        bool HasDecision(uint32_t index);
};

/*
 * Structure containing dialogue state of one player
 */
struct DialogueStateRecord
{
    // player GUID
    uint64_t sourceGuid;
    // main state identifier, "stage" of dialogue
    uint32_t majorState;
    // when did the dialogue started?
    time_t startTime;
    // map of decisions made
    DecisionMap decisions;
    // last event time in dialogue
    uint32_t lastDialogEventTime;
    // last decision made
    uint32_t lastDecision;
};

typedef std::map<uint64_t, DialogueStateRecord> DialogueStateMap;

// enumerator of possible dialogue action options
enum DialogueActionType
{
    DAT_TALK = 0,                           // talk (say, yell, ..)
    DAT_DECIDE = 1,                         // decision box (up to MAX_DIALOGUE_DECISION_ENTRIES decisions)
    DAT_END_DIALOGUE = 2,                   // ends dialogue (closes box)
    DAT_TALK_PLAYER = 3,                    // the player would talk (instead of creature itself)
    MAX_DIALOGUE_ACTION_TYPE
};

/*
 * Structure containing base data for every dialogue action
 */
struct DialogueActionRecord
{
    // type of dialogue action
    DialogueActionType type;
    // time that must pass before next action sending
    uint32_t nextActionTimer;
};

typedef std::unordered_map<uint32_t, DialogueActionRecord*> DialogueActionVector;

/*
 * Structure containing data for "talk" dialogue event type
 */
struct DialogueActionRecord_Talk : public DialogueActionRecord
{
    // talk type (say, yell, ..)
    TalkType talkType;
    // text to be said, yelled, ..
    std::wstring talkText;
};

/*
 * Structure containing data for "decision" dialogue event type
 */
struct DialogueActionRecord_Decision : public DialogueActionRecord
{
    // title of dialogue box
    std::wstring title;
    // IDs of dialogue decisions
    uint32_t IDs[MAX_DIALOGUE_DECISION_ENTRIES];
    // texts of dialogue decisions
    std::wstring texts[MAX_DIALOGUE_DECISION_ENTRIES];
};

/*
 * Class for maintaining dialogues within one creature script
 */
class DialogueHolder
{
    public:
        DialogueHolder(uint64_t creatureGuid);
        ~DialogueHolder();

        // sets starting action for every dialogue
        void SetStartingAction(uint32_t id);

        // adds dialogue action - talk
        void AddDialogueAction_Talk(uint32_t id, uint32_t howLong, TalkType type, const wchar_t* str);
        // adds dialogue action - player talk
        void AddDialogueAction_TalkPlayer(uint32_t id, uint32_t howLong, TalkType type, const wchar_t* str);
        // adds dialogue action - decision
        void AddDialogueAction_Decision(uint32_t id, uint32_t howLong, const wchar_t* title, uint32_t id1, const wchar_t* text1, uint32_t id2 = 0, const wchar_t* text2 = nullptr, uint32_t id3 = 0,
            const wchar_t* text3 = nullptr, uint32_t id4 = 0, const wchar_t* text4 = nullptr, uint32_t id5 = 0, const wchar_t* text5 = nullptr);
        // adds dialogue action - end
        void AddDialogueAction_EndDialogue(uint32_t id);
        // adds relationship between decision and action to be performed right after
        void AddDecisionActionRelationship(uint32_t decision, uint32_t actionId);

        // retrieves dialogue state for one player
        DialogueStateRecord* GetDialogueState(uint64_t playerGuid);
        // starts dialogue for one player; clears his record
        void StartDialogue(uint64_t playerGuid, uint32_t actionId = UINT32_MAX);
        // registers player decision, and moves dialogue further
        void DialogueDecision(uint64_t playerGuid, uint32_t decision);
        // ends dialogue for one player; erases the record from state map
        void EndDialogue(uint64_t playerGuid);

        // performs update - could then handle all timed events
        void Update();

    protected:
        // vector of all dialogue actions
        DialogueActionVector m_dialogueActions;
        // main state map, key = player GUID, value = dialogue state instance
        DialogueStateMap m_dialogueStateMap;
        // relationships between decisions made and actions to be used after them
        std::map<uint32_t, uint32_t> m_decisionActions;

        // adds dialogue action record to map
        void AddDialogueAction(uint32_t id, DialogueActionRecord* record);
        // moves dialogue to action with specified ID for player
        void MoveToAction(uint64_t playerGuid, uint32_t action);

    private:
        // creature GUID this holder belongs to
        uint64_t m_creatureGuid;
        // action where every player starts the dialogue; if not set, the first is used
        uint32_t m_startingAction;
        // list of guids to be removed (to avoid conditionals in main flow)
        std::list<uint64_t> m_removeGuids;
};

#endif
