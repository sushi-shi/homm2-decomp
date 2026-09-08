#ifndef HOMM2_SOURCE_SAVE_NAMES_H
#define HOMM2_SOURCE_SAVE_NAMES_H

#include <array>
#include <cstddef>
#include <string_view>

// These names are part of the save/network protocol. They must not change
// when the display language changes.
namespace save_names {

inline constexpr char Autosave[] = "AUTOSAVE";
inline constexpr char NewGame[] = "NEWGAME";
inline constexpr char PlayerExit[] = "PLYREXIT";
inline constexpr char VictoryPrefix[] = "VICTORY_";

inline constexpr std::size_t LegacyFilenameSize = 14;
using LegacyFilename = std::array<char, LegacyFilenameSize>;

// Informational filename stored in a retail save. The native filesystem name
// is independent: this field is an eight-character ASCII stem and a three-
// character extension, terminated and zero-padded to the original wire size.
LegacyFilename ToLegacyFilename(std::string_view name);

}

#endif
