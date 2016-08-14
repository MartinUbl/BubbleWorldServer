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
#include "DialogueHolder.h"
#include "SmartPacket.h"
#include "Session.h"
#include "Player.h"
#include "ObjectAccessor.h"

void DecisionMap::SetDecision(uint32_t index)
{
    // does the vector need resize?
    if (size() <= index / 32)
        resize(1 + index / 32);

    // set the bit
    (*this)[index / 32] = (1 << (index % 32));
}

void DecisionMap::UnsetDecision(uint32_t index)
{
    // if the vector is smaller than it should be to contain this decision, nothing to be unset
    if (size() <= index / 32)
        return;

    // clear the bit
    (*this)[index / 32] &= ~(1 << (index % 32));
}

bool DecisionMap::HasDecision(uint32_t index)
{
    // if the vector is smaller than it should be to contain this decision, the decision has not
    // been made
    if (size() <= index / 32)
        return false;

    // return the bit presence
    return ((*this)[index / 32] & (1 << (index % 32))) != 0;
}

DialogueHolder::DialogueHolder(uint64_t creatureGuid)
{
    m_creatureGuid = creatureGuid;
    m_startingAction = UINT32_MAX;
}

DialogueHolder::~DialogueHolder()
{
    //
}

DialogueStateRecord* DialogueHolder::GetDialogueState(uint64_t playerGuid)
{
    // no dialogue has been started or is in progress
    if (m_dialogueStateMap.find(playerGuid) == m_dialogueStateMap.end())
        return nullptr;

    return &m_dialogueStateMap[playerGuid];
}

void DialogueHolder::StartDialogue(uint64_t playerGuid)
{
    // clear the state
    m_dialogueStateMap[playerGuid].sourceGuid = playerGuid;
    m_dialogueStateMap[playerGuid].majorState = 0;
    m_dialogueStateMap[playerGuid].startTime = time(nullptr);
    m_dialogueStateMap[playerGuid].lastDialogEventTime = 0;
    m_dialogueStateMap[playerGuid].decisions.clear();

    // send dialogue start - as default choose "wait" state until something triggers decision dialog
    if (WorldObject* plr = sObjectAccessor->FindWorldObject(playerGuid))
    {
        SmartPacket pkt(SP_DIALOGUE_DATA);
        pkt.WriteUInt64(m_creatureGuid);
        pkt.WriteUInt8(DIALOGUE_WAIT);
        plr->ToPlayer()->SendPacketToMe(pkt);
    }

    // start dialogue
    MoveToAction(playerGuid, m_startingAction);
}

void DialogueHolder::DialogueDecision(uint64_t playerGuid, uint32_t decision)
{
    if (m_dialogueStateMap.find(playerGuid) == m_dialogueStateMap.end())
        return;

    uint32_t currentState = m_dialogueStateMap[playerGuid].majorState;

    // set decision to decision vector
    m_dialogueStateMap[playerGuid].decisions.SetDecision(decision);
    m_dialogueStateMap[playerGuid].lastDecision = decision;

    // get player
    if (WorldObject* plr = sObjectAccessor->FindWorldObject(playerGuid))
    {
        // after the decision was made, send "wait" state for dialogue
        SmartPacket pkt(SP_DIALOGUE_DATA);
        pkt.WriteUInt64(m_creatureGuid);
        pkt.WriteUInt8(DIALOGUE_WAIT);
        plr->ToPlayer()->SendPacketToMe(pkt);

        // retrieve the action
        DialogueActionRecord_Decision* rec = (DialogueActionRecord_Decision*)m_dialogueActions[currentState];

        // and make the player actually say that thing out loud
        for (uint8_t i = 0; i < MAX_DIALOGUE_DECISION_ENTRIES; i++)
        {
            if (rec->IDs[i] == decision)
            {
                plr->ToPlayer()->Talk(TALK_SAY, WStringToUTF8(rec->texts[i]).c_str());
                break;
            }
        }
    }

    // set the timer, so the update method could move to next action
    m_dialogueStateMap[playerGuid].lastDialogEventTime = getMSTime();
}

void DialogueHolder::EndDialogue(uint64_t playerGuid)
{
    // if no dialogue started, nothing to be cleared
    if (m_dialogueStateMap.find(playerGuid) == m_dialogueStateMap.end())
        return;

    // send dialogue close packet to player
    if (WorldObject* plr = sObjectAccessor->FindWorldObject(playerGuid))
    {
        if (plr->GetType() == OTYPE_PLAYER)
        {
            SmartPacket pkt(SP_DIALOGUE_CLOSE);
            plr->ToPlayer()->SendPacketToMe(pkt);
        }
    }

    // do not directly erase, just put the guid into remove list and the update method will erase it for us
    //m_dialogueStateMap.erase(playerGuid);
    m_removeGuids.push_back(playerGuid);
}

void DialogueHolder::SetStartingAction(uint32_t id)
{
    m_startingAction = id;
}

void DialogueHolder::AddDialogueAction(uint32_t id, DialogueActionRecord* record)
{
    m_dialogueActions[id] = record;

    // if no starting action set, set the first one added
    if (m_startingAction == UINT32_MAX)
        m_startingAction = id;
}

void DialogueHolder::MoveToAction(uint64_t playerGuid, uint32_t action)
{
    // we reached the end, or the next action is dialogue end
    if (m_dialogueActions.size() <= action || m_dialogueActions[action]->type == DAT_END_DIALOGUE)
    {
        EndDialogue(playerGuid);
        return;
    }

    // set states and last action times
    m_dialogueStateMap[playerGuid].majorState = action;
    m_dialogueStateMap[playerGuid].lastDialogEventTime = getMSTime();

    WorldObject* owner = sObjectAccessor->FindWorldObject(m_creatureGuid);
    // the creature has to exist for this
    if (!owner || owner->GetType() != OTYPE_CREATURE)
        return;

    // perform action based on type
    switch (m_dialogueActions[action]->type)
    {
        case DAT_TALK:
        {
            // talk using parameters supplied
            DialogueActionRecord_Talk* rec = (DialogueActionRecord_Talk*)m_dialogueActions[action];
            owner->ToUnit()->Talk(rec->talkType, WStringToUTF8(rec->talkText).c_str());
            break;
        }
        case DAT_TALK_PLAYER:
        {
            // make player talk using parameters supplied
            if (WorldObject* plr = sObjectAccessor->FindWorldObject(playerGuid))
            {
                DialogueActionRecord_Talk* rec = (DialogueActionRecord_Talk*)m_dialogueActions[action];
                plr->ToUnit()->Talk(rec->talkType, WStringToUTF8(rec->talkText).c_str());
            }
            break;
        }
        case DAT_DECIDE:
        {
            // we set timer to zero until the player actually make decision; then the timer is set again
            m_dialogueStateMap[playerGuid].lastDialogEventTime = 0;

            DialogueActionRecord_Decision* rec = (DialogueActionRecord_Decision*)m_dialogueActions[action];

            // count possible decisions
            uint8_t count = 0;
            for (uint8_t i = 0; i < MAX_DIALOGUE_DECISION_ENTRIES && rec->IDs[i] != 0; i++)
                count++;

            // create packet
            SmartPacket pkt(SP_DIALOGUE_DATA);
            pkt.WriteUInt64(m_creatureGuid);
            pkt.WriteUInt8(DIALOGUE_DECIDE);
            pkt.WriteString(WStringToUTF8(rec->title).c_str());
            pkt.WriteUInt8(count);

            // append decisions
            for (uint8_t i = 0; i < MAX_DIALOGUE_DECISION_ENTRIES && rec->IDs[i] != 0; i++)
            {
                pkt.WriteUInt32(rec->IDs[i]);
                pkt.WriteString(WStringToUTF8(rec->texts[i]).c_str());
            }

            // send the packet to player
            if (WorldObject* plr = sObjectAccessor->FindWorldObject(playerGuid))
                plr->ToPlayer()->SendPacketToMe(pkt);
        }
    }
}

void DialogueHolder::AddDialogueAction_Talk(uint32_t id, uint32_t howLong, TalkType type, const wchar_t* str)
{
    DialogueActionRecord_Talk* rec = new DialogueActionRecord_Talk();

    rec->type = DAT_TALK;
    rec->nextActionTimer = howLong;
    rec->talkType = type;
    rec->talkText = str;

    AddDialogueAction(id, rec);
}

void DialogueHolder::AddDialogueAction_TalkPlayer(uint32_t id, uint32_t howLong, TalkType type, const wchar_t* str)
{
    DialogueActionRecord_Talk* rec = new DialogueActionRecord_Talk();

    rec->type = DAT_TALK_PLAYER;
    rec->nextActionTimer = howLong;
    rec->talkType = type;
    rec->talkText = str;

    AddDialogueAction(id, rec);
}

void DialogueHolder::AddDialogueAction_Decision(uint32_t id, uint32_t howLong, const wchar_t* title, uint32_t id1, const wchar_t* text1, uint32_t id2, const wchar_t* text2, uint32_t id3, const wchar_t* text3, uint32_t id4,
    const wchar_t* text4, uint32_t id5, const wchar_t* text5)
{
    DialogueActionRecord_Decision* rec = new DialogueActionRecord_Decision();

    rec->type = DAT_DECIDE;
    rec->nextActionTimer = howLong;
    rec->title = title;

    // god forgive me

    // nullify the IDs - zero means "nonexistant" decision
    memset(rec->IDs, 0, sizeof(uint32_t) * MAX_DIALOGUE_DECISION_ENTRIES);
    if (id1 != 0 && text1 != nullptr)
    {
        rec->IDs[0] = id1;
        rec->texts[0] = text1;
    }
    if (id2 != 0 && text2 != nullptr)
    {
        rec->IDs[1] = id2;
        rec->texts[1] = text2;
    }
    if (id3 != 0 && text3 != nullptr)
    {
        rec->IDs[2] = id3;
        rec->texts[2] = text3;
    }
    if (id4 != 0 && text4 != nullptr)
    {
        rec->IDs[3] = id4;
        rec->texts[3] = text4;
    }
    if (id5 != 0 && text5 != nullptr)
    {
        rec->IDs[6] = id5;
        rec->texts[6] = text5;
    }

    AddDialogueAction(id, rec);
}

void DialogueHolder::AddDialogueAction_EndDialogue(uint32_t id)
{
    DialogueActionRecord* rec = new DialogueActionRecord();

    rec->type = DAT_END_DIALOGUE;
    rec->nextActionTimer = 0;

    AddDialogueAction(id, rec);
}

void DialogueHolder::AddDecisionActionRelationship(uint32_t decision, uint32_t actionId)
{
    m_decisionActions[decision] = actionId;
}

void DialogueHolder::Update()
{
    // update holder for all players in map
    for (std::pair<uint64_t, DialogueStateRecord> dsp : m_dialogueStateMap)
    {
        // if it's decision action with decision made, or not the decision action, run the timer
        if ((m_dialogueActions[dsp.second.majorState]->type != DAT_DECIDE || dsp.second.lastDialogEventTime != 0) && getMSTimeDiff(dsp.second.lastDialogEventTime, getMSTime()) >= m_dialogueActions[dsp.second.majorState]->nextActionTimer)
        {
            // decision actions should select next action based on relationship maps
            if (m_dialogueActions[dsp.second.majorState]->type == DAT_DECIDE)
            {
                uint32_t currentState = m_dialogueStateMap[dsp.first].majorState;
                uint32_t nextState;

                // retrieve next state
                if (m_decisionActions.find(dsp.second.lastDecision) != m_decisionActions.end())
                    nextState = m_decisionActions[dsp.second.lastDecision];
                else
                    nextState = currentState + 1;

                // we reached the end, or the next action is dialogue end
                if (m_dialogueActions.size() <= nextState || m_dialogueActions[nextState]->type == DAT_END_DIALOGUE)
                {
                    EndDialogue(dsp.first);
                    return;
                }

                // move to chosen action
                MoveToAction(dsp.first, nextState);
            }
            else // default behaviour - take next action
                MoveToAction(dsp.first, dsp.second.majorState + 1);
        }
    }

    // go through remove list
    if (m_removeGuids.size() > 0)
    {
        // remove listed guids and clear that list
        for (uint64_t &playerGuid : m_removeGuids)
            m_dialogueStateMap.erase(playerGuid);

        m_removeGuids.clear();
    }
}
