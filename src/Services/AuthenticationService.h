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

#ifndef BW_AUTH_SERVICE_H
#define BW_AUTH_SERVICE_H

#include "Singleton.h"

/*
 * Singleton class used for authenticating users
 */
class AuthenticationService
{
    friend class Singleton<AuthenticationService>;
    public:
        ~AuthenticationService();

        // authenticates user and returns status
        AuthStatus AuthenticateUser(std::string &username, std::string &password, uint32_t *accountId);

    protected:
        // protected singleton constructor
        AuthenticationService();

    private:
        //
};

#define sAuthenticationService Singleton<AuthenticationService>::getInstance()

#endif
