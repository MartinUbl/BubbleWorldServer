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
#include "Creature.h"
#include "CreatureStorage.h"
#include "Log.h"
#include "CreatureScript.h"
#include "ScriptManager.h"
#include "Player.h"

Creature::Creature() : Unit(OTYPE_CREATURE)
{
    m_script = nullptr;
}

Creature::~Creature()
{
    //
}

void Creature::Create(uint32_t guidLow, uint32_t entry)
{
    Unit::Create(MAKE_GUID64(HIGHGUID_CREATURE, entry, guidLow));

    CreatureTemplateRecord* rec = sCreatureStorage->GetCreatureTemplate(entry);
    if (!rec)
    {
        sLog->Error("Unable to retrieve requested creature template entry %u (for guid low %u)", entry, guidLow);
        return;
    }

    SetName(rec->name.c_str());
    SetLevel(rec->level, true);
    SetImageId(rec->imageId);
    SetFaction(rec->faction);
    SetMaxHealth(rec->health);
    SetHealth(rec->health);

    if (rec->scriptName.size() > 0)
    {
        m_script = sScriptManager->CreateScript(rec->scriptName.c_str(), this);
        if (m_script)
            m_script->OnCreate();
    }
}

void Creature::Update()
{
    Unit::Update();

    if (m_script)
        m_script->OnUpdate();
}

void Creature::Interact(Player* player)
{
    if (m_script)
        m_script->OnInteract(player);
}

void Creature::DialogueDecision(Player* player, uint32_t decision)
{
    if (m_script)
        m_script->OnDialogueDecision(player, decision);
}

void Creature::CreateUpdateFields()
{
    m_maxUpdateFieldIndex = UNIT_FIELDS_END;

    m_updateFields = new uint32_t[m_maxUpdateFieldIndex];
    m_updateFieldsChangeBits = new uint32_t[1 + (m_maxUpdateFieldIndex / 32)];

    memset(m_updateFields, 0, sizeof(uint32_t) * m_maxUpdateFieldIndex);
    memset(m_updateFieldsChangeBits, 0, sizeof(uint32_t) * (1 + (m_maxUpdateFieldIndex / 32)));

    Unit::CreateUpdateFields();
}
