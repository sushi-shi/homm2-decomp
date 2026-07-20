#ifndef HOMM2_CURSOR_H
#define HOMM2_CURSOR_H

#include <va.h>

// Vertical-walk frames begin at 45; frames 0x37-0x3a contain their shadows.
H2_ENUM_BEGIN(CursorConstant)
    CURSOR_BOAT_WAKE_TYPE               = 7,
    CURSOR_DIRECTION_COUNT              = 8,
    CURSOR_TURN_FRAME_COUNT             = 16,
    CURSOR_FRAMES_PER_DIRECTION         = 9,
    CURSOR_LAST_FRAME_COUNT             = 8,
    CURSOR_FLIP_FLAG                    = 0x80,
    CURSOR_FRAME_MASK                   = 0x7f,
    CURSOR_BOAT_BASE_FRAME_5            = 0x9b,
    CURSOR_BOAT_BASE_FRAME_6            = 0x92,
    CURSOR_BOAT_BASE_FRAME_7            = 0x89,
    CURSOR_MAP_DRAW_OFFSET              = 7,
    CURSOR_DRAW_X                       = 0xe0,
    CURSOR_FLIPPED_DRAW_X               = 0x100,
    CURSOR_SHADOW_FLIP_X_ADJUST         = 0x20,
    CURSOR_DRAW_Y                       = 0xFF,
    CURSOR_BOAT_DRAW_Y                  = 0xf5,
    CURSOR_CLIP_SIZE                    = 0x1e0,
    CURSOR_FLAG_FRAME_BASE              = 0x38,
    CURSOR_HORSE_SHADOW_OFFSET          = 0x32,
    CURSOR_BOAT_SHADOW_OFFSET           = 0x24,
    CURSOR_SHADOW_ANIM_FIRST            = 9,
    CURSOR_SHADOW_ANIM_END              = 0x24,
    CURSOR_SLOW_TURN_MULTIPLIER         = 3,
    CURSOR_MAP_VISIBLE_FLAG             = 0x40,
    CURSOR_CELL_UNCOVERED_FLAG          = 0x04,
    CURSOR_CELL_BLOCKED_FLAG            = 0x08,
    CURSOR_OBJECT_PASSABLE_FLAG         = 0x80,
    CURSOR_WATER_TERRAIN                = 0,
    CURSOR_BOAT_COUNT                   = 48,
    CURSOR_MOVE_HALF_TILE_PIXELS        = 16,
    CURSOR_ARTIFACT_CAPACITY            = 14,
    CURSOR_RESOURCE_COUNT               = 7,
    CURSOR_RESOURCE_LAST                = CURSOR_RESOURCE_COUNT - 1,
    CURSOR_RESOURCE_DIALOG_PENALTY      = 100000,
    CURSOR_FIZZLE_X                     = 0xc0,
    CURSOR_FIZZLE_Y                     = 0xc0,
    CURSOR_FIZZLE_WIDTH                 = 0x60,
    CURSOR_FIZZLE_HEIGHT                = 0x60,
    CURSOR_FIZZLE_COMPUTER_TYPE         = 50,
    CURSOR_INVALID_POSITION             = -1,
    CURSOR_EMPTY_OBJECT_INDEX           = 0xFF,
    CURSOR_NORTH_DIRECTION_MASK         = 0x83,
    CURSOR_SOUTH_DIRECTION_MASK         = 0x38,
    CURSOR_MAP_CHANGE_RECENT_COUNT      = 4,
    CURSOR_MAP_CHANGE_QUEUE_COUNT       = 196,
    CURSOR_MAP_CHANGE_PENDING_SENTINEL  = 999,
    CURSOR_MAP_CHANGE_SEQUENCE_SENTINEL = 999999999,
    CURSOR_PLAYER_COUNT                 = 6,
    CURSOR_DEAD_PLAYER_DIALOG_TIME      = 5000,
    CURSOR_REMOTE_PLAYER_ALL            = 0x7f,
    CURSOR_REMOTE_PACKET_TYPE           = 0x29,
    CURSOR_REMOTE_RELIABLE              = 1,
    CURSOR_REMOTE_NO_TIMEOUT            = -1
H2_ENUM_END(CursorConstant)

H2_ENUM_BEGIN(MapChangeConstant)
    MAP_CHANGE_CURRENT_PLAYER = -999
H2_ENUM_END(MapChangeConstant)

H2_ENUM_BEGIN(MapSpriteConstant)
    MAP_SPRITE_INITIAL_DIRECTION = 2
H2_ENUM_END(MapSpriteConstant)

H2_ENUM_CLASS_BEGIN_SPLIT(MapChangeType, u8)
    MAP_CHANGE_NONE          = 0,
    MAP_CHANGE_MOVE_HERO     = 1,
    MAP_CHANGE_UNUSED        = 2,
    MAP_CHANGE_RECRUIT_HERO  = 3,
    MAP_CHANGE_BUILD_BOAT    = 4,
    MAP_CHANGE_ERASE_OBJECT  = 5,
    MAP_CHANGE_DEAD_HERO     = 6,
    MAP_CHANGE_CLAIM_TOWN    = 7,
    MAP_CHANGE_CLAIM_MINE    = 8,
    MAP_CHANGE_TELEPORT_HERO = 9,
    MAP_CHANGE_DEAD_PLAYER   = 10,
    MAP_CHANGE_MY_TURN       = 11
H2_ENUM_CLASS_END_SPLIT(MapChangeType, u8)

#pragma pack(push, 1)
struct SMapChange {
    H2_ENUM_STORAGE(MapChangeType, u8) type;
    i8 id;
    u8 x;
    u8 y;
    i8 direction;
    i8 stopAfterMove;
    i8 player;
    i32 sequence;
};
#pragma pack(pop)
SIZE(SMapChange, 11);

extern SMapChange sMapChangeLastFew[CURSOR_MAP_CHANGE_RECENT_COUNT];

void SendMapChange(MapChangeType, i8, u8, u8, i32, u8, u8);

#endif
