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
#include "DatabaseConnection.h"
#include "AuthenticationService.h"
#include "SHA1.h"

AuthenticationService::AuthenticationService()
{
    //
}

AuthenticationService::~AuthenticationService()
{
    //
}

AuthStatus AuthenticationService::AuthenticateUser(std::string &username, std::string &password, uint32_t *accountId)
{
    // hash password
    unsigned char sha1hash[20], sha1hex[41];
    sha1::calc(password.c_str(), (int)password.length(), sha1hash);
    sha1::toHexString(sha1hash, (char*)sha1hex);

    // retrieve record from database with exact same username and password hash
    DBResult res = sMainDatabase.PQuery("SELECT id, username, password FROM account WHERE username = '%s' AND password = '%s';", username.c_str(), sha1hex);

    // if there is a row to fetch...
    if (res.FetchRow())
    {
        // fill account ID and return OK
        *accountId = res.GetUInt32(0);
        return AUTH_STATUS_OK;
    }

    return AUTH_STATUS_UNKNOWN_USER;
}
