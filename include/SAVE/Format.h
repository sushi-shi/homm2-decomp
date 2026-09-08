#ifndef HOMM2_SAVE_FORMAT_H
#define HOMM2_SAVE_FORMAT_H

#include <SAVE/Archive.h>

namespace savegame {

inline constexpr u32 FormatVersion = 1;
inline constexpr char Extension[] = ".h2s";

std::vector<u8> WrapPayload(std::span<const u8> payload);
std::span<const u8> UnwrapPayload(std::span<const u8> file);

}

#endif
