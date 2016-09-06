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
#include "Application.h"
#include "NetworkManager.h"
#include "DatabaseConnection.h"
#include "ResourceStorage.h"
#include "ResourceStreamService.h"
#include "CRC32.h"
#include "Log.h"
#include "MapStorage.h"
#include "MapManager.h"
#include "CreatureStorage.h"
#include "GameobjectStorage.h"
#include "ScriptManager.h"
#include "WaypointStorage.h"
#include "ItemStorage.h"
#include "ObjectAccessor.h"

#include <thread>

Application::Application()
{
    //
}

Application::~Application()
{
    //
}

bool Application::Init(int argc, char** argv)
{
    sLog->Info("BubbleWorld game server");

    // TODO: parse commandline options

    sLog->Info("MySQL Client Library version: %s\n", mysql_get_client_info());

    sLog->Info(">> Connecting to database...");
    if (!sMainDatabase.Connect("127.0.0.1", 3306, "root", "root", "bubbleworld_main"))
    {
        sLog->Error("Could not connect to database!");
        return false;
    }
    sLog->Info("All database connections estabilished\n");

    sLog->Info(">> Loading resource database...");
    sResourceStorage->LoadFromDB();
    sLog->Info("");
    sLog->Info(">> Verifying resource checksums...");
    sResourceStorage->VerifyChecksums();
    sLog->Info("Finished checksum verification");

    sLog->Info("");
    sMapStorage->LoadFromDB();
    sLog->Info(">> Verifying map and map chunk checksums...");
    sMapStorage->VerifyChecksums();
    sLog->Info("Finished checksum verification");
    sLog->Info("");

    sObjectAccessor->InitGUIDMaps();
    sLog->Info("");

    sCreatureStorage->LoadFromDB();
    sLog->Info("");

    sGameobjectStorage->LoadFromDB();
    sLog->Info("");

    sItemStorage->LoadFromDB();
    sLog->Info("");

    sWaypointStorage->LoadFromDB();
    sLog->Info("");

    // some messages?
    sResourceStreamService->Init();

    sScriptManager->Initialize();
    sLog->Info("");

    sLog->Info(">> Initializing networking layer...");
    if (!sNetwork->Init(DEFAULT_NET_BIND_ADDR, DEFAULT_NET_BIND_PORT))
    {
        sLog->Error("Could not initialize network layer!");
        return false;
    }
    sLog->Info("Networking layer running, listening on %s:%u\n", DEFAULT_NET_BIND_ADDR, DEFAULT_NET_BIND_PORT);

    sLog->Info("Initialization sequence complete\n");
    return true;
}

int Application::Run()
{
    MSG msg;
    bool run = true;

    while (run)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
                run = false;
        }

        sNetwork->Update();

        sMapManager->UpdateMaps();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
