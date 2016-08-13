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

#ifndef BW_SCRIPT_MANAGER_H
#define BW_SCRIPT_MANAGER_H

#include "Singleton.h"
#include "CreatureScript.h"

// external script loading function - just adds all instantiators
extern void LoadScripts();

class Creature;

typedef CreatureScript* CreatureScriptCreator(Creature*);
template <class T> CreatureScript* instantiateCreatureScript(Creature* c)
{
    return new T(c);
}

// macro for registering new creature script class (derived from CreatureScript)
#define REGISTER_CREATURE_SCRIPT(name,classname) sScriptManager->RegisterCreatureScript(name, instantiateCreatureScript<classname>)

typedef std::map<std::string, CreatureScriptCreator*> ScriptRegistry;

/*
 * Singleton class used for loading, managing and creating all scripts
 */
class ScriptManager
{
    friend class Singleton<ScriptManager>;
    public:
        ~ScriptManager();

        // initializes script registry
        void Initialize();
        // registers a creature script with specified name (using instantiator)
        void RegisterCreatureScript(const char* name, CreatureScriptCreator* instantiator);
        // creates creature script for new creature instance
        CreatureScript* CreateScript(const char* name, Creature* owner);

    protected:
        // protected singleton constructor
        ScriptManager();

    private:
        // script registry of creature scripts
        ScriptRegistry m_creatureScriptRegistry;
};

#define sScriptManager Singleton<ScriptManager>::getInstance()

#endif
