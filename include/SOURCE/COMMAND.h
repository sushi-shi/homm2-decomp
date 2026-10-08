#ifndef HOMM2_SOURCE_COMMAND_H
#define HOMM2_SOURCE_COMMAND_H

#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/remoteTypes.h>

struct tag_message;

enum {
    ACTION_NONE       = 0,
    ACTION_CAST_SPELL = 1,
    ACTION_MOVE       = 2,
    ACTION_SKIP_TURN  = 3,
    ACTION_RETREAT    = 4,
    ACTION_SURRENDER  = 5,
    ACTION_ATTACK     = 6,
    ACTION_DEFER_TURN = 7
};
typedef i32 CombatAction;
#pragma pack(push, 1)
struct CombatRemotePacket {
    i8 sender;
    i32 id;
    i8 type;
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
