#include <SAVE/Snapshot.h>
#include <SOURCE/EVENTS.h>

#include <cstdio>

int main() {
    savegame::Snapshot original;
    original.width = original.height = 36;
    original.cells.resize(36 * 36);
    original.fog.resize(36 * 36, 0x35);
    original.overlays.resize(2);
    original.events.resize(2);
    original.events[1].resize(offsetof(EventExtra, message) + 1);
    original.m_playerCount = 2;
    original.m_day = 7;
    original.m_week = 4;
    original.m_month = 10;
    original.currentPlayer = 1;
    original.human[0] = original.human[1] = true;
    original.playerNames[0] = "Александр";
    original.playerNames[1] = "A name longer than the legacy player field";
    original.heroes[0].m_name = "Александр Македонский";
    original.heroes[0].m_spellPoints = 123;
    original.heroes[0].m_x = 9;
    original.heroes[0].m_eventFlags = HERO_EVENT_EMBARKED | HERO_EVENT_STABLES;
    original.heroes[0].m_artifactExtra[13] = 7;
    original.heroes[0].m_army.m_creatureTypes[0] = CREATURE_PEASANT;
    original.heroes[0].m_army.m_creatureCounts[0] = 1234;
    original.towns[0].m_name = "Константинополь";
    original.towns[0].m_spellSlots[19] = SPELL_MAGIC_ARROW;
    original.towns[0].m_spellCounts[5] = 4;
    original.rumour = "UTF-8: Привет мир — 猫犬鳥魚";
    original.cells[0].m_extraIndex = 1;
    original.cells[0].m_drawOverlayOnTop = 1;
    original.cells[0].m_overlayTilesetBits = 63;
    original.overlays[1].objectMetadata = 31;
    original.m_mapEventCount = 1;
    original.m_mapEventIndices[0] = 1;

    std::string error;
    std::vector<u8> bytes;
    if (!savegame::Encode(original, bytes, error)) {
        std::fprintf(stderr, "encode: %s\n", error.c_str());
        return 1;
    }
    savegame::Snapshot loaded;
    if (!savegame::Decode(bytes, loaded, error)) {
        std::fprintf(stderr, "decode: %s\n", error.c_str());
        return 2;
    }
    std::vector<u8> repeated;
    if (!savegame::Encode(loaded, repeated, error) || repeated != bytes
        || loaded.heroes[0].m_name != original.heroes[0].m_name
        || loaded.towns[0].m_name != original.towns[0].m_name
        || loaded.playerNames != original.playerNames || loaded.rumour != original.rumour
        || loaded.heroes[0].m_army.m_creatureCounts[0] != 1234
        || loaded.towns[0].m_spellCounts[5] != 4)
        return 3;

    bytes.back() ^= 1;
    if (savegame::Decode(bytes, loaded, error) || loaded.heroes[0].m_name != original.heroes[0].m_name)
        return 4;
    original.overlays[1].nextIndex = 1;
    if (savegame::Encode(original, repeated, error)) return 5;
    original.overlays[1].nextIndex = 0;
    original.events[1].pop_back();
    if (savegame::Encode(original, repeated, error)) return 10;
    original.events[1].push_back(0);
    original.overlays.push_back({});
    original.overlays[2].nextIndex = 2;
    if (!savegame::Encode(original, repeated, error)) return 7;
    original.overlays[1].nextIndex = MAPCELL_EXTRA_FREE;
    if (savegame::Encode(original, repeated, error)) return 8;
    original.overlays[1].nextIndex = 0;
    original.kind = savegame::Kind::ExpansionCampaign;
    original.expansion.campaignId = 1;
    original.expansion.currentMap = 3;
    original.expansion.mapCount = 8;
    original.expansion.awards[10] = 1;
    if (!savegame::Encode(original, bytes, error) || !savegame::Decode(bytes, loaded, error)
        || loaded.kind != savegame::Kind::ExpansionCampaign
        || loaded.expansion.awards[10] != 1) return 6;
    std::puts("native scenario/campaign and owned UTF-8 state round trips passed");
    return 0;
}
