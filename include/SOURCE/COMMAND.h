#ifndef HOMM2_COMMAND_H
#define HOMM2_COMMAND_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/REMOTE_TYPES.h>

struct tag_message;

enum class CombatAction : i32 {
    ACTION_NONE       = 0,
    ACTION_CAST_SPELL = 1,
    ACTION_MOVE       = 2,
    ACTION_SKIP_TURN  = 3,
    ACTION_RETREAT    = 4,
    ACTION_SURRENDER  = 5,
    ACTION_ATTACK     = 6,
    ACTION_DEFER_TURN = 7
};
using enum CombatAction;

#pragma pack(push, 1)
struct CombatRemotePacket {
    i8 sender;
    i32 id;
    H2EnumStorage<RemoteMessageType, i8> type;
    i8 command;
    i16 payloadSize;
    union {
        struct {
            CombatAction nextAction;
            i32 nextActionExtra;
            i32 nextActionGridIndex;
            i32 nextActionGridIndex2;
        };
        char text[REMOTE_MESSAGE_PAYLOAD_SIZE];
    };
};
#pragma pack(pop)

MessageDispatchResult WinCombatHandler(struct tag_message& message);
i32 InCombatArea(i32 x, i32 y);

#endif
