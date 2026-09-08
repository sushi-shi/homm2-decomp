#include <SAVE/Runtime.h>

#include <BASE/Misc.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/advManager.h>
#include <SOURCE/searchArray.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>

namespace {

bool Expect(bool condition, const char* description) {
    if (!condition) std::fprintf(stderr, "save runtime: %s\n", description);
    return condition;
}

savegame::Snapshot Scenario() {
    savegame::Snapshot state;
    state.width = state.height = 36;
    state.header.width = state.header.height = 36;
    state.cells.resize(36 * 36);
    state.fog.resize(36 * 36, 3);
    state.overlays.resize(3);
    state.overlays[2].nextIndex = MAPCELL_EXTRA_FREE;
    state.cells[42].m_extraIndex = 1;
    state.overlays[1].objectIndex = 17;
    state.overlays[1].SetObjectTileset(TILESET_NONE);
    state.events.resize(2);
    state.events[1] = {0, 0, 'm', 'a', 'p', 0};
    state.m_playerCount = 2;
    state.currentPlayer = 1;
    state.m_day = 7;
    state.m_week = 4;
    state.m_month = 4;
    state.human[0] = state.human[1] = true;
    state.playerNames[0] = "Александр";
    state.playerNames[1] = "a player name that cannot fit in a retail save record";
    state.heroes[0].m_name = "Александр";
    state.heroes[0].m_owner = 0;
    state.heroes[0].m_spellPoints = 77;
    state.heroes[0].m_artifacts[0] = ARTIFACT_MAGIC_BOOK;
    state.heroes[0].m_army.m_creatureTypes[0] = CREATURE_PEASANT;
    state.heroes[0].m_army.m_creatureCounts[0] = 42;
    state.towns[0].m_name = "Константинополь";
    state.towns[0].m_owner = 0;
    state.towns[0].m_onMap = 1;
    state.towns[0].m_x = 4;
    state.towns[0].m_y = 5;
    state.players[0].m_color = 0;
    state.players[0].m_heroCount = 1;
    state.players[0].m_heroIds[0] = 0;
    state.players[0].m_townCount = 1;
    state.players[0].m_townIds[0] = 0;
    state.players[0].m_resources[H2EnumIndex(RES_GOLD)] = 23456;
    state.players[0].m_aiData.m_income[H2EnumIndex(RES_GOLD)] = 500;
    state.rumour = "A rumour with UTF-8: 猫犬鳥魚, Привет мир";
    state.mapFilename = "a scenario filename longer than DOS supported.mp2";
    return state;
}

}

int main() {
    char directory[] = "/tmp/homm2-save-runtime-XXXXXX";
    if (mkdtemp(directory) == nullptr) return 1;
    setenv("XDG_DATA_HOME", directory, 1);
    setenv("HOMM2_DATA", directory, 1);
    setenv("HOMM2_LANGUAGE", "en", 1);
    setenv("SDL_VIDEODRIVER", "dummy", 1);
    setenv("SDL_AUDIODRIVER", "dummy", 1);
    platform::Startup();
    localization::Initialize("");
    gbInPollSound = true;
    auto gameState = std::make_unique<game>();
    auto search = std::make_unique<searchArray>();
    gpGame = gameState.get();
    gpSearchArray = search.get();
    auto adventure = std::make_unique<advManager>();
    gpAdvManager = adventure.get();
    gbRemoteOn = false;

    bool ok = true;
    std::string error;
    for (savegame::Kind kind : {savegame::Kind::Scenario, savegame::Kind::Campaign,
                               savegame::Kind::ExpansionCampaign}) {
        auto original = Scenario();
        original.kind = kind;
        original.expansionMap = kind == savegame::Kind::ExpansionCampaign;
        original.m_campaignScenario = 3;
        original.m_campaignScenarioDays[0][3] = 99;
        if (kind == savegame::Kind::ExpansionCampaign) {
            original.expansion.campaignId = 1;
            original.expansion.currentMap = 3;
            original.expansion.mapCount = 8;
            original.expansion.awards[10] = 1;
        }
        std::vector<u8> expected;
        ok &= Expect(savegame::Encode(original, expected, error), error.c_str());
        savegame::Apply(*gpGame, original);
        // Saving and loading must neither reset hero-screen globals nor
        // shallow-copy a string's storage into another runtime record.
        gpGame->m_heroRecs[1] = gpGame->m_heroRecs[0];
        gpGame->m_heroRecs[1].m_name += " changed copy";
        ok &= Expect(gpGame->m_heroRecs[0].m_name == original.heroes[0].m_name, "hero copies own their names");
        static_cast<HeroState&>(gpGame->m_heroRecs[1]) = original.heroes[1];
        savegame::Snapshot captured;
        ok &= Expect(savegame::Capture(*gpGame, captured, error), error.c_str());
        std::vector<u8> actual;
        ok &= Expect(savegame::Encode(captured, actual, error), error.c_str());
        ok &= Expect(actual == expected, "engine apply/capture preserves encoded state");
        ok &= Expect(savegame::WriteFile("GAMES/names.h2s", captured, error), error.c_str());
        gpGame->m_heroRecs[0].m_name = "temporary";
        gpGame->m_castleRecs[0].m_name = "temporary";
        savegame::Snapshot decoded;
        ok &= Expect(savegame::ReadFile("GAMES/names.h2s", decoded, error), error.c_str());
        savegame::Apply(*gpGame, std::move(decoded));
        ok &= Expect(gpGame->m_heroRecs[0].m_name == original.heroes[0].m_name, "hero name survives disk round trip");
        ok &= Expect(gpGame->m_castleRecs[0].m_name == original.towns[0].m_name, "town name survives disk round trip");
        ok &= Expect(cPlayerNames == original.playerNames && gpGame->m_rumour == original.rumour,
                     "player names and rumour survive disk round trip");
        ok &= Expect(giCurPlayer == original.currentPlayer && giNumHumanPlayers == 2,
                     "player metadata is restored from state");
        gpCurPlayer = &gpGame->m_players[giCurPlayer];
        ok &= Expect(gpGame->SaveGame("entry-point", 1) != 0, "game save entry point writes native file");
        gpGame->m_heroRecs[0].m_name = "changed after saving";
        ok &= Expect(gpGame->LoadGame("entry-point.h2s"), "game load entry point reads native file");
        ok &= Expect(gpGame->m_heroRecs[0].m_name == original.heroes[0].m_name,
                     "game entry points preserve full hero names");
    }

    // A truncated file must leave the destination snapshot and the game alone.
    const i32 file = platform::FileOpen("GAMES/truncated.h2s", platform::FileMode::Write);
    ok &= Expect(file >= 0 && platform::FileWriteExact(file, "H2SAVE", 6), "write truncated fixture");
    if (file >= 0) platform::FileClose(file);
    auto unchanged = Scenario();
    ok &= Expect(!savegame::ReadFile("GAMES/truncated.h2s", unchanged, error), "reject truncated file");
    ok &= Expect(unchanged.heroes[0].m_name == "Александр" && gpGame->m_heroRecs[0].m_name == "Александр",
                 "rejected file leaves existing state intact");

    ClearMapExtra();
    H2_FREE(mapExtra);
    mapExtra = nullptr;
    search.reset();
    gpSearchArray = nullptr;
    adventure.reset();
    gpAdvManager = nullptr;
    gameState.reset();
    gpGame = nullptr;
    platform::Shutdown();
    std::filesystem::remove_all(directory);
    if (ok) std::puts("engine state and UTF-8 names survive native save/load for all campaign kinds");
    return ok ? 0 : 1;
}
