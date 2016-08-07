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

#ifndef BW_UNIT_H
#define BW_UNIT_H

#include "WorldObject.h"
#include "AnimEnums.h"
#include "Vector2.h"

// movement angles used for movement vector calculations
static const float movementAngles[] = {
    0.0f,           // 0 (none)
    F_PI * 1.5f,    // 1 (up)
    0.0f,           // 2 (right)
    F_PI * 1.75f,   // 3 (up + right)
    F_PI * 0.5f,    // 4 (down)
    0.0f,           // 5 (up + down = none)
    F_PI * 0.25f,   // 6 (right + down)
    0.0f,           // 7 (up + right + down = right)
    F_PI,           // 8 (left)
    F_PI * 1.25f,   // 9 (left + up)
    0.0f,           // 10 (left + right = none)
    F_PI * 1.5f,    // 11 (left + right + up = up)
    F_PI * 0.75f,   // 12 (left + down)
    F_PI,           // 13 (left + down + up = left)
    F_PI * 0.5f,    // 14 (left + down + right = down)
    0.0f            // 15 (all = none)
};

// movement animations
static const uint32_t movementAnims[] = {
    ANIM_IDLE,
    ANIM_WALK_UP,
    ANIM_WALK_RIGHT,
    ANIM_WALK_UPRIGHT,
    ANIM_WALK_DOWN,
    ANIM_IDLE,
    ANIM_WALK_DOWNRIGHT,
    ANIM_WALK_RIGHT,
    ANIM_WALK_LEFT,
    ANIM_WALK_UPLEFT,
    ANIM_IDLE,
    ANIM_WALK_UP,
    ANIM_WALK_DOWNLEFT,
    ANIM_WALK_LEFT,
    ANIM_WALK_DOWN,
    ANIM_IDLE
};

// this is the number which we use to multiply movement vector
#define MOVEMENT_UPDATE_UNIT_FRACTION 0.001f
// delay between two movement heartbeat packets
#define MOVEMENT_HEARTBEAT_SEND_DELAY 1000

enum MapFieldType;

/*
 * Class representing every "living" object in game
 */
class Unit : public WorldObject
{
    public:
        virtual ~Unit();

        virtual void Create(uint64_t guid);
        virtual void Update();
        virtual void BuildCreatePacketBlock(SmartPacket &pkt);

        // retrieves unit level
        uint16_t GetLevel();
        // sets unit level
        void SetLevel(uint16_t lvl, bool onLoad = false);

        // starts moving in direction
        void StartMoving(MoveDirectionElement dir);
        // stops moving in direction
        void StopMoving(MoveDirectionElement dir);
        // is unit moving?
        bool IsMoving();

        // talk using specified talk type and supplied string
        void Talk(TalkType type, const char* str);
        // can the unit move over this field type?
        bool CanMoveOn(MapFieldType type, uint32_t flags);
        // retrieves unit current health
        uint32_t GetHealth();
        // retrieves unit maximum health
        uint32_t GetMaxHealth();
        // sets unit current health
        void SetHealth(uint32_t health);
        // sets unit maximum health
        void SetMaxHealth(uint32_t maxHealth);
        // retrieves unit faction
        uint32_t GetFaction();
        // sets unit faction
        void SetFaction(uint32_t faction);

    protected:
        // protected constructor; instantiate child classes
        Unit(ObjectType type);
        virtual void CreateUpdateFields();

        // updates movement vector using stored moveMask
        void UpdateMovementVector();

    private:
        // movement mask - in which direction is the unit moving
        uint8_t m_moveMask;
        // timer used for heartbeat packets timing
        uint32_t m_moveHeartbeatTimer;
        // last time of movement update
        uint32_t m_lastMovementUpdate;
        // move vector used for movement interpolation
        Vector2 m_moveVector;
};

#endif
