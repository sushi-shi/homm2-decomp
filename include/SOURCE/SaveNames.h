#ifndef HOMM2_SOURCE_SAVE_NAMES_H
#define HOMM2_SOURCE_SAVE_NAMES_H

// These names are part of the save/network protocol. They must not change
// when the display language changes.
namespace save_names {

inline constexpr char Autosave[] = "AUTOSAVE";
inline constexpr char NewGame[] = "NEWGAME";
inline constexpr char PlayerExit[] = "PLYREXIT";
inline constexpr char VictoryPrefix[] = "VICTORY_";

}

#endif
