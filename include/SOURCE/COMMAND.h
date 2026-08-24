#ifndef HOMM2_COMMAND_H
#define HOMM2_COMMAND_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

typedef enum CombatRemotePacketConstant {
    COMBAT_REMOTE_PACKET_PREFIX_RESERVED_SIZE = 4,
    COMBAT_REMOTE_PACKET_BODY_RESERVED_SIZE   = 2
} CombatRemotePacketConstant;

enum class CombatAction : i32 {
    ACTION_NONE       = 0,
    ACTION_CAST_SPELL = 1,
    ACTION_MOVE       = 2,
    ACTION_WAIT       = 3,
    ACTION_RETREAT    = 4,
    ACTION_SURRENDER  = 5,
    ACTION_ATTACK     = 6,
    ACTION_DEFEND     = 7
};
using enum CombatAction;

#pragma pack(push, 1)
struct CombatRemotePacket {
    i8 messageLength;
    char reserved1[COMBAT_REMOTE_PACKET_PREFIX_RESERVED_SIZE];
    H2EnumStorage<RemoteMessageType, i8> type;
    i8 command;
    char reserved7[COMBAT_REMOTE_PACKET_BODY_RESERVED_SIZE];
    CombatAction nextAction;
    i32 nextActionExtra;
    i32 nextActionGridIndex;
    i32 nextActionGridIndex2;
};
#pragma pack(pop)

MessageDispatchResult WinCombatHandler(struct tag_message&);
i32 InCombatArea(i32, i32);

#endif
