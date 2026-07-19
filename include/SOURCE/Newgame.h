#ifndef HOMM2_NEWGAME_H
#define HOMM2_NEWGAME_H

#include <Ints.h>
#include <SOURCE/REQUEST.h>

struct tag_message;

H2_ENUM_BEGIN(NewGameControl)
    NEW_GAME_DIFFICULTY_HELP_FIRST = 1,
    NEW_GAME_DIFFICULTY_FIRST      = 0x43,
    NEW_GAME_RACE_FIRST            = 6,
    NEW_GAME_COLOR_FIRST           = 0x0c,
    NEW_GAME_PLAYER_SELECT_FIRST   = 0x12,
    NEW_GAME_PLAYER_NAME_FIRST     = 0x18,
    NEW_GAME_RACE_ICON_FIRST       = 0x1e,
    NEW_GAME_RACE_CYCLE_FIRST      = 0x24,
    NEW_GAME_HANDICAP_FIRST        = 0x30,
    NEW_GAME_PLAYER_HUMAN_FIRST    = 0x2a,
    NEW_GAME_SCENARIO_NAME         = 0x40,
    NEW_GAME_RATING                = 0x42,
    NEW_GAME_MAP_SELECT            = 0x48,
    NEW_GAME_SHADOW                = 0x49,
    NEW_GAME_CHAT_FIRST            = 0x4a,
    NEW_GAME_RACE_NAME_FIRST       = 0x4e
H2_ENUM_END(NewGameControl)

#ifdef HOMM2_STRICT_ENUM_TYPES
inline i32 EncodeNewGameControl(NewGameControl control) {
    return static_cast<i32>(control);
}

inline NewGameControl DecodeNewGameControl(i32 value) {
    return static_cast<NewGameControl>(value);
}

inline i32 EncodeNewGameControlIndex(NewGameControl first, i32 index) {
    return index + static_cast<i32>(first);
}

inline i32 LastNewGameControl(NewGameControl first, i32 count) {
    return static_cast<i32>(first) + count - 1;
}
#else
#define EncodeNewGameControl(control) (control)
#define DecodeNewGameControl(value) (value)
#define EncodeNewGameControlIndex(first, index) ((index) + (first))
#define LastNewGameControl(first, count) ((first) + (count) - 1)
#endif

#pragma pack(push, 1)
struct NewGameRemotePacket {
    i8 sender;
    u8 reserved[4];
    i8 type;
    i8 command;
    u8 sequence[2];
    char payload[1];
};

#pragma pack(pop)

i32 NewGameHandler(struct tag_message&);

#endif
