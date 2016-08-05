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

#ifndef BW_WORLDOBJECT_H
#define BW_WORLDOBJECT_H

#include "UpdateFields.h"
#include "ObjectEnums.h"

#include <math.h>

/*
 * Position structure with base operations defined
 */
struct Position
{
    // base constructor, nullifying coordinates
    Position() : x(0.0f), y(0.0f) { };
    // constructor with coordinates initialization
    Position(float _x, float _y) : x(_x), y(_y) { };
    // copy constructor
    Position(const Position &pos) : x(pos.x), y(pos.y) { };

    // assignment operator copies coordinates from righthand operand
    Position& operator=(const Position &pos) { x = pos.x; y = pos.y; return *this; }
    // addition compount operator adds righthand operand coordinates
    Position& operator+=(const Position &pos) { x += pos.x; y += pos.y; return *this; }
    // subtraction compound operator subtracts operand coordinates
    Position& operator-=(const Position &pos) { x -= pos.x; y -= pos.y; return *this; }
    // addition operator adds coordinates of arguments and creates new position container
    friend Position operator+(const Position &posA, const Position &posB) { return Position(posA.x + posB.x, posA.y + posB.y); }
    // subtraction operator subtracts coordinates of arguments and creates new position container
    friend Position operator-(const Position &posA, const Position &posB) { return Position(posA.x - posB.x, posA.y - posB.y); }

    // calculates distance between two positions
    float GetDistance(Position &pos)
    {
        return sqrt((pos.x - x)*(pos.x - x) + (pos.y - y)*(pos.y - y));
    }

    // X coordinate
    float x;
    // Y coordinate
    float y;
};

class Unit;
class Player;
class Creature;
class SmartPacket;

/*
 * Base class for all objects in world
 */
class WorldObject
{
    public:
        virtual ~WorldObject();

        // this should be called at every child class Create method beginning!!!
        virtual void Create(uint64_t guid);
        // update object
        virtual void Update();
        // builds create packet block to be sent to player
        virtual void BuildCreatePacketBlock(SmartPacket &pkt);

        // retrieves object GUID
        uint64_t GetGUID();
        // retrieves entry
        uint32_t GetEntry();
        // retrieves low GUID
        uint32_t GetGUIDLow();
        // retrieves object type
        ObjectType GetType();

        // casts object to Unit class if possible
        Unit* ToUnit();
        // casts object to Player class if possible
        Player* ToPlayer();
        // casts object to Creature class if possible
        Creature* ToCreature();

        // sets object name
        void SetName(const char* name);
        // retrieves object name
        const char* GetName();

        // sets 32bit unsigned field value
        void SetUInt32Value(uint32_t field, uint32_t value);
        // sets 64bit unsigned field value
        void SetUInt64Value(uint32_t field, uint64_t value);
        // sets unsigned byte field value
        void SetUByteValue(uint32_t field, uint8_t offset, uint8_t value);
        // sets 32bit signed field value
        void SetInt32Value(uint32_t field, int32_t value);
        // sets 64bit signed field value
        void SetInt64Value(uint32_t field, int64_t value);
        // sets signed byte field value
        void SetByteValue(uint32_t field, uint8_t offset, int8_t value);
        // sets float field value
        void SetFloatValue(uint32_t field, float value);
        // retrieves 32bit unsigned field value
        uint32_t GetUInt32Value(uint32_t field);
        // retrieves 64bit unsigned field value
        uint64_t GetUInt64Value(uint32_t field);
        // retrieves unsigned byte field value
        uint8_t GetUByteValue(uint32_t field, uint8_t offset);
        // retrieves 32bit signed field value
        int32_t GetInt32Value(uint32_t field);
        // retrieves 64bit signed field value
        int64_t GetInt64Value(uint32_t field);
        // retrieves signed byte field value
        int8_t GetByteValue(uint32_t field, uint8_t offset);
        // retrieves float field value
        float GetFloatValue(uint32_t field);

        // sets flag for updating specified field
        void SetUpdateFieldUpdateNeeded(uint32_t field);
        // send packet to sorrounding
        void SendPacketToSorroundings(SmartPacket &pkt);

        // sets position X coordinate
        void SetPositionX(float x);
        // sets position Y coordinate
        void SetPositionY(float y);
        // set position using X and Y coordinates
        void SetPosition(float x, float y);
        // retrieves position
        Position const& GetPosition();
        // retrieves position X coordinate
        float GetPositionX();
        // retrieves position Y coordinate
        float GetPositionY();
        // retrieves current map ID
        uint32_t GetMapId();

        // relocates object within map
        void RelocateWithinMap(float x, float y);
        // teleports object to another map
        void TeleportTo(uint32_t mapId, float x, float y);
        // sets position within map after loading process complete
        void SetInitialPositionAfterLoad(uint32_t mapId, float x, float y);

    protected:
        // protected constructor; instantiate child class
        WorldObject(ObjectType type);
        // create update fields; every child class creates them its own way (different sizes)
        virtual void CreateUpdateFields();

        // current map ID
        uint32_t m_positionMap;
        // current position
        Position m_position;
        // object updatefields
        uint32_t* m_updateFields;
        // object updatefields "needs update" flags
        uint32_t* m_updateFieldsChangeBits;
        // maximum updatefield index
        uint32_t m_maxUpdateFieldIndex;

        // this object type
        ObjectType m_objectType;
        // object name
        std::string m_name;

    private:
        // is there any updatefield update needed to be broadcast?
        bool m_updateFieldsNeedsUpdate;
};

#endif
