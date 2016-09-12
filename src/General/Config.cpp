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
#include "Config.h"

#include <fstream>

ConfigMgr::ConfigMgr()
{
    // resize to desired size
    m_intValues.resize(CONFIG_MAX_INT_VAL);
    m_strValues.resize(CONFIG_MAX_STRING_VAL);

    // set all fields to uninitialized state
    uint32_t i;
    for (i = 0; i < CONFIG_MAX_INT_VAL; i++)
        m_intValues[i].initialized = false;
    for (i = 0; i < CONFIG_MAX_STRING_VAL; i++)
        m_strValues[i].initialized = false;
}

ConfigMgr::~ConfigMgr()
{
    //
}

void ConfigMgr::InitDefaults()
{
    // database config
    SetConfigStringField(CONFIG_STRING_DB_HOST, "database_host", "localhost");
    SetConfigStringField(CONFIG_STRING_DB_USERNAME, "database_username", "bubbleworld");
    SetConfigStringField(CONFIG_STRING_DB_PASSWORD, "database_password", "bubblepassword");
    SetConfigStringField(CONFIG_STRING_DB_DBNAME, "database_dbname", "bubbleworld_main");
    SetConfigIntField(CONFIG_INT_DB_PORT, "database_port", 3306);

    // log config
    SetConfigStringField(CONFIG_STRING_LOG_FILE, "log_file", "server.log");
    SetConfigIntField(CONFIG_INT_LOG_MASK, "logtype_mask", 3);

    // network settings
    SetConfigStringField(CONFIG_STRING_BIND_IP, "listen_ip", "127.0.0.1");
    SetConfigIntField(CONFIG_INT_BIND_PORT, "listen_port", 7874);
}

bool ConfigMgr::ValidateConfig()
{
    uint32_t i;
    uint32_t errorCount = 0;

    // validate port range for database port
    if (GetIntValue(CONFIG_INT_DB_PORT) <= 0 || GetIntValue(CONFIG_INT_DB_PORT) >= 65536)
    {
        std::cerr << "Config error: database port out of range (1 - 65535)" << std::endl;
        errorCount++;
    }

    // validate port range for network bind port
    if (GetIntValue(CONFIG_INT_BIND_PORT) <= 0 || GetIntValue(CONFIG_INT_BIND_PORT) >= 65536)
    {
        std::cerr << "Config error: bind port out of range (1 - 65535)" << std::endl;
        errorCount++;
    }

    // look for uninitialized config values and report them
    for (i = 0; i < CONFIG_MAX_INT_VAL; i++)
    {
        if (!m_intValues[i].initialized)
            std::cerr << "Config: integer value index " << i << " not initialized, undefined behaviour possible" << std::endl;
    }
    for (i = 0; i < CONFIG_MAX_STRING_VAL; i++)
    {
        if (!m_strValues[i].initialized)
            std::cerr << "Config: string value index " << i << " not initialized, undefined behaviour possible" << std::endl;
    }

    // report error count if any errors found
    if (errorCount > 0)
        std::cerr << "Config contains " << errorCount << " errors, cannot continue" << std::endl;

    // validation is OK when not errors found
    return (errorCount == 0);
}

bool ConfigMgr::LoadConfig(const char* path)
{
    std::string configFilename;

    // choose default config path when no path specified
    if (!path)
        configFilename = DEFAULT_CONFIG_FILENAME;
    else
        configFilename = path;

    std::ifstream f(configFilename.c_str());

    if (!f.is_open() || f.bad())
    {
        // do not use logger here, it is not configured yet
        std::cerr << "Could not load config file " << configFilename.c_str() << "!" << std::endl;
        return false;
    }

    // read all lines and parse them
    std::string line;
    while (!f.eof())
    {
        std::getline(f, line);

        line = str_trim(line);

        if (line.length() > 0)
            ProcessConfigLine(line);
    }

    return true;
}

void ConfigMgr::ProcessConfigLine(std::string &line)
{
    int64_t res;

    // lines beginning with '#' are considered comments
    if (line[0] == '#')
        return;

    // find '=' to be able to find left side (identifier) and right side (value)
    size_t eq = line.find_first_of('=');
    // not found or at the end of the line
    if (eq == std::string::npos || eq == line.size() - 1)
    {
        std::cerr << "Invalid config line: " << line.c_str() << std::endl;
        return;
    }

    // parse and trim identifier and value
    std::string identifier = str_trim(line.substr(0, eq));
    std::string value = str_trim(line.substr(eq + 1));

    // try to find index in integer value identifiers
    ConfigIntValues iindex = GetIntIndexByIdentifier(identifier);
    // if found, parse and set
    if (iindex != CONFIG_MAX_INT_VAL)
    {
        if (str2int(res, value.c_str()))
            SetIntValue(iindex, res);
        else
            std::cerr << "Invalid integral value: " << value.c_str() << std::endl;
        return;
    }

    // if not found within integer identifiers, try string identifiers
    ConfigStringValues sindex = GetStringIndexByIdentifier(identifier);
    if (sindex != CONFIG_MAX_STRING_VAL)
    {
        SetStringValue(sindex, value.c_str());
        return;
    }

    // otherwise report error
    std::cerr << "Unrecognized config identifier: " << identifier.c_str() << std::endl;
}

void ConfigMgr::SetIntValue(ConfigIntValues index, int64_t value)
{
    m_intValues[index].intValue = value;
}

void ConfigMgr::SetStringValue(ConfigStringValues index, const char* value)
{
    m_strValues[index].strValue = value ? value : "";
}

int64_t ConfigMgr::GetIntValue(ConfigIntValues index) const
{
    return m_intValues[index].intValue;
}

const char* ConfigMgr::GetStringValue(ConfigStringValues index) const
{
    return m_strValues[index].strValue.c_str();
}

ConfigIntValues ConfigMgr::GetIntIndexByIdentifier(std::string const& identifier) const
{
    // go through all initialized (! uninitialized fields does not have identifiers !) fields and compare identifiers
    for (uint32_t i = 0; i < CONFIG_MAX_INT_VAL; i++)
    {
        if (m_intValues[i].initialized && m_intValues[i].identifier == identifier)
            return (ConfigIntValues)i;
    }

    return CONFIG_MAX_INT_VAL;
}

ConfigStringValues ConfigMgr::GetStringIndexByIdentifier(std::string const& identifier) const
{
    // go through all initialized (! uninitialized fields does not have identifiers !) fields and compare identifiers
    for (uint32_t i = 0; i < CONFIG_MAX_STRING_VAL; i++)
    {
        if (m_strValues[i].initialized && m_strValues[i].identifier == identifier)
            return (ConfigStringValues)i;
    }

    return CONFIG_MAX_STRING_VAL;
}

void ConfigMgr::SetConfigIntField(ConfigIntValues index, const char* identifier, int defaultValue)
{
    if (!identifier || *identifier == '\0')
        return;

    m_intValues[index].identifier = identifier;
    m_intValues[index].intValue = defaultValue;

    // we successfully initialized this field
    m_intValues[index].initialized = true;
}

void ConfigMgr::SetConfigStringField(ConfigStringValues index, const char* identifier, const char* defaultValue)
{
    if (!identifier || *identifier == '\0')
        return;

    m_strValues[index].identifier = identifier;
    m_strValues[index].strValue = defaultValue ? defaultValue : "";

    // we successfully initialized this field
    m_strValues[index].initialized = true;
}
