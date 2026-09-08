#ifndef HOMM2_SAVE_COMBAT_H
#define HOMM2_SAVE_COMBAT_H

#include <SAVE/Format.h>
#include <SOURCE/hero.h>
#include <SOURCE/town.h>

#include <optional>

namespace savegame {

// The portable network sends explicit state, never a runtime object's bytes.
struct CombatState {
    i32 x = 0;
    i32 y = 0;
    i32 setupX = 0;
    i32 setupY = 0;
    i32 randomSeed = 0;
    i8 result = 0;
    i8 retreat = 0;
    i8 surrender = 0;
    i32 firstGold = 0;
    i32 secondGold = 0;
    std::optional<HeroState> firstHero;
    std::optional<HeroState> secondHero;
    std::optional<TownState> town;
    std::optional<armyGroup> firstArmy;
    std::optional<armyGroup> secondArmy;
};

bool EncodeCombat(const CombatState& state, std::vector<u8>& bytes, std::string& error);
bool DecodeCombat(std::span<const u8> bytes, CombatState& state, std::string& error);

inline constexpr std::size_t MaximumCombatBytes = 4 * 1024 * 1024;
inline constexpr std::size_t CombatFragmentBytes = 200;

std::vector<u8> CombatFragment(std::span<const u8> bytes, std::size_t offset);

class CombatTransfer {
public:
    bool Add(std::span<const u8> fragment, std::string& error);
    bool Complete() const { return !m_bytes.empty() && m_received == m_bytes.size(); }
    std::span<const u8> Bytes() const { return m_bytes; }

private:
    std::vector<u8> m_bytes;
    std::size_t m_received = 0;
};

}

#endif
