#ifndef HOMM2_COMMAND_H
#define HOMM2_COMMAND_H

#include <Ints.h>

struct tag_message;

#define COMBAT_NECROMANCY_LEVEL_FACTOR 0.1
#define COMBAT_SURRENDER_QUILL_FACTOR 0.1
#define COMBAT_SURRENDER_BASE_FACTOR 0.5
#define COMBAT_SURRENDER_DIPLOMACY_FACTOR 0.2
#define COMBAT_IDLE_ROLL_DIVISOR 100.0f
#define COMBAT_STAND_DELAY_BASE_FACTOR 0.25
#define COMBAT_STAND_DELAY_RANDOM_FACTOR 0.5
#define COMBAT_CYCLE_TIMER_FACTOR 150.0f

#define COMBAT_SOUND_POLL_DELAY 75.0f
#define COMBAT_DIRECTION_SLOPE_STEEPEST 3.73
#define COMBAT_DIRECTION_SLOPE_STEEP 1.73
#define COMBAT_DIRECTION_SLOPE_DIAGONAL 1.0f
#define COMBAT_DIRECTION_SLOPE_SHALLOW 0.58
#define COMBAT_DIRECTION_SLOPE_SHALLOWEST 0.27

H2_ENUM_BEGIN(CombatRemotePacketConstant)
    COMBAT_REMOTE_PACKET_PREFIX_RESERVED_SIZE = 4,
    COMBAT_REMOTE_PACKET_BODY_RESERVED_SIZE   = 2
H2_ENUM_END(CombatRemotePacketConstant)

#pragma pack(push, 1)
struct CombatRemotePacket {
    i8 messageLength;
    char reserved1[COMBAT_REMOTE_PACKET_PREFIX_RESERVED_SIZE];
    i8 category;
    i8 command;
    char reserved7[COMBAT_REMOTE_PACKET_BODY_RESERVED_SIZE];
    i32 nextAction;
    i32 nextActionExtra;
    i32 nextActionGridIndex;
    i32 nextActionGridIndex2;
};
#pragma pack(pop)
SIZE(CombatRemotePacket, 0x19);

i32 WinCombatHandler(struct tag_message&);
i32 InCombatArea(i32, i32);

#endif
