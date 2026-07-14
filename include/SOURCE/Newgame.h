#ifndef HOMM2_NEWGAME_H
#define HOMM2_NEWGAME_H
// Declarations of the free functions DEFINED in Newgame.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

typedef enum NewGameConstant {
    NEW_GAME_TEXT_BUFFER_COUNT = 3,
    NEW_GAME_TEXT_BUFFER_SIZE = 0x65,
    NEW_GAME_KEY_BUFFER_SIZE = 0x69,
    NEW_GAME_MAP_PACKET_SIZE = 0x74,
    NEW_GAME_PLAYER_INFO_BUFFER_SIZE = 0x100,
    NEW_GAME_PLAYER_INFO_PACKET_SIZE = 0xcc,
    NEW_GAME_REMOTE_CHANNEL = 0x7f,
    NEW_GAME_REMOTE_PACKET_TYPE = 2,
    NEW_GAME_REMOTE_MAP_HEADER = 0x34,
    NEW_GAME_REMOTE_PLAYER_INFO = 0x37,
    NEW_GAME_NETWORK_PLAYER_NONE = -1,
    NEW_GAME_COMPUTER_PLAYER = 10,
    NEW_GAME_PLAYER_DEFAULT = 0,
    NEW_GAME_PLAYER_FLEXIBLE = 1,
    NEW_GAME_DEFAULT_DIFFICULTY = 1,
    NEW_GAME_MULTIPLAYER_HOTSEAT = 2,
    NEW_GAME_DIALOG_CANCEL = 0x7801,
    NEW_GAME_DIALOG_OK = 0x7802,
    NEW_GAME_MAP_OPTIONS_CONTROL = 0x36,
    NEW_GAME_WIDGET_ENABLE = 5,
    NEW_GAME_WIDGET_DISABLE = 6,
    NEW_GAME_WIDGET_ACTIVE_FRAME = 0x1000,
    NEW_GAME_WIDGET_INACTIVE_FRAME = 2,
    NEW_GAME_MESSAGE_WIDGET = 0x200,
    NEW_GAME_SOURCE_LINE_BASE = 319
} NewGameConstant;

#pragma pack(push, 1)
struct NewGameRemotePacket {
    unsigned char reserved[5];
    signed char type;
    signed char command;
    unsigned char sequence[2];
    char payload[1];
};
#pragma pack(pop)

int NewGameHandler(struct tag_message &);

#endif // HOMM2_NEWGAME_H
