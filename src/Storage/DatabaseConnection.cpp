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
#include "Log.h"

#include <cstdarg>

DatabaseConnection sMainDatabase;

DBResult::DBResult()
{
    m_finalized = false;
    m_valid = false;
    m_doNotDestroy = false;
    res = nullptr;
}

DBResult::DBResult(MYSQL_RES* result)
{
    m_finalized = false;
    m_doNotDestroy = false;
    res = result;
    row = nullptr;
    m_valid = (res != nullptr);
}

DBResult::~DBResult()
{
    if (!m_finalized)
        Finalize();
}

DBResult& DBResult::operator=(DBResult &rr)
{
    if (!m_finalized)
        Finalize();

    m_finalized = false;
    res = rr.res;
    row = nullptr;
    m_valid = (res != nullptr);

    rr.m_doNotDestroy = true;

    return *this;
}

bool DBResult::FetchRow()
{
    return (m_valid && ((row = mysql_fetch_row(res)) != NULL));
}

int DBResult::_GetIntValue(int column)
{
    if (row)
    {
        char* val = _GetColumnValue(column);
        return val ? std::stoi(row[column]) : 0;
    }

    sLog->Error("Attempt to retrieve field without fetched row!");
    return 0;
}

long long DBResult::_GetLongValue(int column)
{
    if (row)
    {
        char* val = _GetColumnValue(column);
        return val ? std::stoll(row[column]) : 0LL;
    }

    sLog->Error("Attempt to retrieve field without fetched row!");
    return 0;
}

uint8_t DBResult::GetUInt8(int column)
{
    return (uint8_t)_GetIntValue(column);
}

int8_t DBResult::GetInt8(int column)
{
    return (int8_t)_GetIntValue(column);
}

uint16_t DBResult::GetUInt16(int column)
{
    return (uint16_t)_GetIntValue(column);
}

int16_t DBResult::GetInt16(int column)
{
    return (int16_t)_GetIntValue(column);
}

uint32_t DBResult::GetUInt32(int column)
{
    return (uint32_t)_GetIntValue(column);
}

int32_t DBResult::GetInt32(int column)
{
    return (int32_t)_GetIntValue(column);
}

uint64_t DBResult::GetUInt64(int column)
{
    return (uint64_t)_GetLongValue(column);
}

int64_t DBResult::GetInt64(int column)
{
    return (int64_t)_GetLongValue(column);
}

float DBResult::GetFloat(int column)
{
    if (row)
    {
        char* val = _GetColumnValue(column);
        return val ? std::stof(val) : 0.0f;
    }

    sLog->Error("Attempt to retrieve field without fetched row!");
    return 0.0f;
}

std::string DBResult::GetString(int column)
{
    if (row)
    {
        char* val = _GetColumnValue(column);
        return val ? val : "";
    }

    sLog->Error("Attempt to retrieve field without fetched row!");
    return "";
}

char* DBResult::_GetColumnValue(int column)
{
    if (row[column])
    {
        m_columnNull = false;
        return row[column];
    }

    m_columnNull = true;
    return nullptr;
}

void DBResult::Finalize()
{
    if (res && !m_doNotDestroy)
        mysql_free_result(res);
    m_finalized = true;
}

DatabaseConnection::DatabaseConnection()
{
    //
}

bool DatabaseConnection::Connect(const char* host, uint16_t port, const char* user, const char* password, const char* databaseName)
{
    if (!mysql_init(&m_connection))
    {
        sLog->Error("Could not initialize MySQL connector");
        return false;
    }

    if (!mysql_real_connect(&m_connection, host, user, password, databaseName, 3306, nullptr, 0))
    {
        sLog->Error("Could not connect to MySQL database: %s", mysql_error(&m_connection));
        return false;
    }

    // we use UTF-8 everywhere
    Execute("SET NAMES utf8;");

    return true;
}

DBResult DatabaseConnection::Query(const char* qr)
{
    if (mysql_query(&m_connection, qr) == 0)
        return mysql_store_result(&m_connection);

    sLog->Error("MySQL query failed: %s", mysql_error(&m_connection));
    return NULL;
}

DBResult DatabaseConnection::PQuery(const char* qr, ...)
{
    va_list argList;
    va_start(argList, qr);
    char buf[2048];
    vsnprintf(buf, 2048, qr, argList);
    va_end(argList);

    return Query(buf);
}

void DatabaseConnection::Execute(const char* qr)
{
    if (mysql_query(&m_connection, qr) == 0)
        return;

    sLog->Error("MySQL query failed: %s", mysql_error(&m_connection));
}

void DatabaseConnection::PExecute(const char* qr, ...)
{
    va_list argList;
    va_start(argList, qr);
    char buf[2048];
    vsnprintf(buf, 2048, qr, argList);
    va_end(argList);

    Execute(buf);
}
