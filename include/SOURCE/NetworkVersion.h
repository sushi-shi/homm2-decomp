#ifndef HOMM2_SOURCE_NETWORKVERSION_H
#define HOMM2_SOURCE_NETWORKVERSION_H

#include <SOURCE/REMOTE_TYPES.h>

#include <array>
#include <cstring>
#include <span>

namespace portable_network {

// Version 1 uses native saves and field-based combat state. Retail peers
// cannot interpret either representation and must be rejected during setup.
inline constexpr std::array<char, 4> Signature = {'H', '2', 'N', 1};

inline void SetVersion(SNetPlayerInfo& player) {
    std::memcpy(player.reserved + 1, Signature.data(), Signature.size());
}

inline bool Compatible(std::span<const char> signature) {
    return signature.size() == Signature.size()
        && std::memcmp(signature.data(), Signature.data(), Signature.size()) == 0;
}

inline bool Compatible(const SNetPlayerInfo& player) {
    return Compatible({player.reserved + 1, Signature.size()});
}

}

#endif
