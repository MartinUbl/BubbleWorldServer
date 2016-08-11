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

#ifndef BW_DATABASECONNECTION_H
#define BW_DATABASECONNECTION_H

#include "mysql.h"

/*
 * Structure containing MySQL database result
 */
struct DBResult
{
    // empty constructor
    DBResult();
    // constructor utilizing previous MYSQL_RES
    DBResult(MYSQL_RES* result);
    virtual ~DBResult();

    // assignment operator
    DBResult& operator=(DBResult &rr);

    // fetch result row if available
    bool FetchRow();
    // retrieves unsigned byte value
    uint8_t GetUInt8(int column = 0);
    // retrueves signet byte value
    int8_t GetInt8(int column = 0);
    // retrieves unsigned 16bit value
    uint16_t GetUInt16(int column = 0);
    // retrieves signed 16bit value
    int16_t GetInt16(int column = 0);
    // retrieves unsigned 32bit value
    uint32_t GetUInt32(int column = 0);
    // retrieves signed 32bit value
    int32_t GetInt32(int column = 0);
    // retrieves unsigned 64bit value
    uint64_t GetUInt64(int column = 0);
    // retrieves signed 64bit value
    int64_t GetInt64(int column = 0);
    // retrieves string value
    std::string GetString(int column = 0);
    // retrieves float value
    float GetFloat(int column = 0);
    // retrieves raw column value
    char* _GetColumnValue(int column);

    // base for retrieving int value
    int _GetIntValue(int column = 0);
    // base for retrieving long value
    long long _GetLongValue(int column = 0);

    // finalize working with result
    void Finalize();

    // stored result
    MYSQL_RES* res;
    // stored fetched row
    MYSQL_ROW row;
    // is finalized?
    bool m_finalized;
    // avoid destruction of result set
    bool m_doNotDestroy;
    // is valid result?
    bool m_valid;
    // is last retrieved column NULL?
    bool m_columnNull;
};

/*
 * Class used for maintaining single database connection
 */
class DatabaseConnection
{
    public:
        // empty constructor
        DatabaseConnection();

        // connect to database with specified parameters; returns true on success, false on failure
        bool Connect(const char* host, uint16_t port, const char* user, const char* password, const char* databaseName);

        // queries database for result
        DBResult Query(const char* qr);
        // queries database for result using variable argument list
        DBResult PQuery(const char* qr, ...);
        // executes query without expecting result
        void Execute(const char* qr);
        // executes query without expecting result using variable argument list
        void PExecute(const char* qr, ...);

    protected:
        // stored MySQL DB connection
        MYSQL m_connection;

    private:
        //
};

// Main database connection instance
extern DatabaseConnection sMainDatabase;

#endif
