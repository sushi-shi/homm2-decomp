#include <SAVE/Combat.h>
#include <SOURCE/NetworkVersion.h>

#include <algorithm>
#include <cstdio>

int main() {
    savegame::CombatState original;
    original.x = 4;
    original.y = 5;
    original.firstHero.emplace();
    original.firstHero->m_name = "Александр — герой с полным именем";
    original.firstHero->m_owner = 0;
    original.firstHero->m_eventFlags = HERO_EVENT_EMBARKED | HERO_EVENT_STABLES;
    original.firstHero->m_army.m_creatureTypes[0] = CREATURE_PEASANT;
    original.firstHero->m_army.m_creatureCounts[0] = 123;
    original.secondHero.emplace();
    original.secondHero->m_owner = 1;
    original.secondHero->m_name = std::string(1700, 'A') + "猫犬鳥魚";
    original.town.emplace();
    original.town->m_name = "Константинополь";
    original.firstArmy = original.firstHero->m_army;
    original.secondArmy.emplace();
    original.firstGold = 7000;
    original.secondGold = 3000;
    std::string error;
    std::vector<u8> bytes;
    if (!savegame::EncodeCombat(original, bytes, error)) {
        std::fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    savegame::CombatTransfer transfer;
    for (std::size_t offset = 0; offset < bytes.size(); offset += savegame::CombatFragmentBytes) {
        const auto fragment = savegame::CombatFragment(bytes, offset);
        if (fragment.size() > 212 || !transfer.Add(fragment, error)) return 2;
        // Retransmitted identical fragments must not duplicate state bytes.
        if (!transfer.Add(fragment, error)) return 3;
    }
    savegame::CombatState decoded;
    if (!transfer.Complete() || !savegame::DecodeCombat(transfer.Bytes(), decoded, error)) return 4;
    if (!decoded.firstHero || !decoded.secondHero || !decoded.town || !decoded.firstArmy
        || decoded.firstHero->m_name != original.firstHero->m_name
        || decoded.secondHero->m_name != original.secondHero->m_name
        || decoded.town->m_name != original.town->m_name
        || decoded.firstArmy->m_creatureCounts[0] != 123
        || decoded.firstGold != original.firstGold || decoded.secondGold != original.secondGold)
        return 5;
    savegame::CombatTransfer outOfOrder;
    if (outOfOrder.Add(savegame::CombatFragment(bytes, 200), error)) return 6;
    auto fragment = savegame::CombatFragment(bytes, 0);
    fragment.back() ^= 1;
    if (transfer.Add(fragment, error)) return 7;
    bytes.back() ^= 1;
    if (savegame::DecodeCombat(bytes, decoded, error)
        || decoded.firstHero->m_name != original.firstHero->m_name) return 8;
    if (savegame::CombatTransfer{}.Add({fragment.data(), 4}, error)) return 9;

    SNetPlayerInfo peer{};
    if (portable_network::Compatible(peer) || portable_network::Compatible(std::span<const char>{})) return 10;
    portable_network::SetVersion(peer);
    if (!portable_network::Compatible(peer)) return 11;
    peer.reserved[4]++;
    if (portable_network::Compatible(peer)) return 12;
    std::puts("native combat state, bounded fragments, and peer version checks passed");
    return 0;
}
