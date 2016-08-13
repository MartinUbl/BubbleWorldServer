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
#include "Log.h"

ScriptManager::ScriptManager()
{
    //
}

ScriptManager::~ScriptManager()
{
    //
}

void ScriptManager::Initialize()
{
    sLog->Info(">> Initializing script registry...");

    // just load scripts
    LoadScripts();

    // in the future, there may be some validations and checking for unused/nonexistant scripts

    sLog->Info("Script registry initialized");
}

void ScriptManager::RegisterCreatureScript(const char* name, CreatureScriptCreator* instantiator)
{
    // script overriding is considered error, but do not fail the whole loading process due to name conflict
    if (m_creatureScriptRegistry.find(name) != m_creatureScriptRegistry.end())
        sLog->Error("Overriding script with name '%s'", name);

    // register the script
    m_creatureScriptRegistry[name] = instantiator;
}

CreatureScript* ScriptManager::CreateScript(const char* name, Creature* owner)
{
    // check if the script exists
    if (m_creatureScriptRegistry.find(name) == m_creatureScriptRegistry.end())
    {
        sLog->Error("Attempt to create non-existant script with name '%s'", name);
        return nullptr;
    }

    // instantiate script class
    return (m_creatureScriptRegistry[name])(owner);
}
