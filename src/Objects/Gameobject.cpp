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
#include "Gameobject.h"
#include "GameobjectStorage.h"
#include "Log.h"
#include "SmartPacket.h"

Gameobject::Gameobject() : WorldObject(OTYPE_GAMEOBJECT)
{
    //
}

Gameobject::~Gameobject()
{
    //
}

void Gameobject::Create(uint32_t guidLow, uint32_t entry)
{
    WorldObject::Create(MAKE_GUID64(HIGHGUID_GAMEOBJECT, entry, guidLow));

    GameobjectTemplateRecord* rec = sGameobjectStorage->GetGameobjectTemplate(entry);
    if (!rec)
    {
        sLog->Error("Unable to retrieve requested gameobject template entry %u (for guid low %u)", entry, guidLow);
        return;
    }

    SetName(rec->name.c_str());
    SetImageId(rec->imageId);
}

void Gameobject::Update()
{
    WorldObject::Update();
}

void Gameobject::BuildCreatePacketBlock(SmartPacket &pkt)
{
    WorldObject::BuildCreatePacketBlock(pkt);

    pkt.WriteFloat(m_position.x);
    pkt.WriteFloat(m_position.y);
}

void Gameobject::CreateUpdateFields()
{
    m_maxUpdateFieldIndex = GAMEOBJECT_FIELDS_END;

    m_updateFields = new uint32_t[m_maxUpdateFieldIndex];
    m_updateFieldsChangeBits = new uint32_t[1 + (m_maxUpdateFieldIndex / 32)];

    memset(m_updateFields, 0, sizeof(uint32_t) * m_maxUpdateFieldIndex);
    memset(m_updateFieldsChangeBits, 0, sizeof(uint32_t) * (1 + (m_maxUpdateFieldIndex / 32)));

    WorldObject::CreateUpdateFields();
}
