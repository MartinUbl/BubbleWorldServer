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

#ifndef BW_CONFIG_H
#define BW_CONFIG_H

#include "Singleton.h"

// default config filename, if not overriden by command line arguments
#define DEFAULT_CONFIG_FILENAME "server.cfg"

// integer value indexes
enum ConfigIntValues
{
    CONFIG_INT_DB_PORT = 0,
    CONFIG_INT_LOG_MASK = 1,
    CONFIG_INT_BIND_PORT = 2,
    CONFIG_MAX_INT_VAL
};

// string value indexes
enum ConfigStringValues
{
    CONFIG_STRING_DB_HOST = 0,
    CONFIG_STRING_DB_USERNAME = 1,
    CONFIG_STRING_DB_PASSWORD = 2,
    CONFIG_STRING_DB_DBNAME = 3,
    CONFIG_STRING_LOG_FILE = 4,
    CONFIG_STRING_BIND_IP = 5,
    CONFIG_MAX_STRING_VAL
};

/*
 * Structure containing config value with parameters
 */
struct ConfigValue
{
    // config value string identifier
    std::string identifier;
    // is initialized (by default) ?
    bool initialized;

    // string value
    std::string strValue;
    // integer value
    int64_t intValue;
};

/*
 * Singleton class maintaining config load and managing
 */
class ConfigMgr
{
    friend class Singleton<ConfigMgr>;
    public:
        virtual ~ConfigMgr();

        // initializes defaults and sets possible values
        void InitDefaults();
        // loads config from file; if no filename supplied, uses default
        bool LoadConfig(const char* path = nullptr);
        // validates config values (i.e. port range, ..)
        bool ValidateConfig();

        // sets integer value at index
        void SetIntValue(ConfigIntValues index, int64_t value);
        // sets string value at index
        void SetStringValue(ConfigStringValues index, const char* value);
        // retrieves integer value from index
        int64_t GetIntValue(ConfigIntValues index) const;
        // retrieves string value from index
        const char* GetStringValue(ConfigStringValues index) const;

        // retrieves integer index by identifier parsed
        ConfigIntValues GetIntIndexByIdentifier(std::string const& identifier) const;
        // retrieves string index by identifier parsed
        ConfigStringValues GetStringIndexByIdentifier(std::string const& identifier) const;

    protected:
        // protected singleton constructor
        ConfigMgr();

        // initializes integer config field
        void SetConfigIntField(ConfigIntValues index, const char* identifier, int defaultValue);
        // initializes string config field
        void SetConfigStringField(ConfigStringValues index, const char* identifier, const char* defaultValue);

        // processes config file line, stores values
        void ProcessConfigLine(std::string &line);

    private:
        // integer values
        std::vector<ConfigValue> m_intValues;
        // string values
        std::vector<ConfigValue> m_strValues;
};

#define sConfig Singleton<ConfigMgr>::getInstance()

#endif
