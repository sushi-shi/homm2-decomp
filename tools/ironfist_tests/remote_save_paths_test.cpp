#include <IRONFIST/runtime.h>
#include <BASE/Misc.h>
#include <IRONFIST/save_xml.h>
#include <IRONFIST/scripting.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/netwin.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/X_GLOBAL.h>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

int main() {
    gpGame = new game{};
    gpAdvManager = new advManager;
    gpAdvManager->m_heroContextLocked = false;
    gpAdvManager->m_mapData = &gpGame->m_worldMap;
    gpCurPlayer = &gpGame->m_players[0];
    gpCurPlayer->m_currentHero = -1;
    gpGame->m_playerCount = 1;
    gpGame->m_week = gpGame->m_month = 1;
    giCurPlayer = giThisGamePos = 0;
    iWSLastMsgNumHumanPlayers = 1;
    gbRemoteOn = false;
    xIsExpansionMap = true;
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i)
        gbHumanPlayer[i] = gbThisNetHumanPlayer[i] = (i == 0);
    MAP_WIDTH = MAP_HEIGHT = 36;
    bMapInitialized = true;
    gpGame->SetMapSize(36, 36);
    gpGame->m_worldMap.Init(36, 36);
    std::memset(gpGame->m_worldMap.cells, 0, 36 * 36 * sizeof(mapCell));
    iMaxMapExtra = 1;
    ppMapExtra = static_cast<void**>(H2_ALLOC(sizeof(void*)));
    ppMapExtra[0] = nullptr;
    pwSizeOfMapExtra = static_cast<i16*>(H2_ALLOC(sizeof(i16)));
    pwSizeOfMapExtra[0] = 0;

    for (const char* directory : {"DATA/", "GAMES/"}) {
        const auto path = platform::Files().Resolve(
            (std::string(directory) + "RMTTEST.BIN").c_str(), platform::FileMode::Write);
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    }
    // These are the actual save and load entry points, including format probing
    // and post-load game setup. The network transport itself is not involved.
    for (const char* name : {"RMTTEST.BIN", "rMtMixed.Bin", "manual.GX1"}) {
        gpGame->m_day = 5;
        std::strcpy(gpGame->m_saveName, "keep.GX1");
        assert(ironfist::runtime::SaveGame(name, 0));
        const bool remote = name[0] != 'm';
        const auto logical = std::string(remote ? "DATA/" : "GAMES/") + name;
        const auto path = platform::Files().Resolve(logical.c_str(), platform::FileMode::Read);
        assert(std::filesystem::exists(path));
        if (remote)
            assert(std::strcmp(gpGame->m_saveName, "keep.GX1") == 0);
        gpGame->m_day = 7;
        ironfist::save::XmlFile reader;
        ironfist::SessionData parsed;
        const auto phase = ironfist::runtime::Phase();
        assert(reader.Read(logical.c_str(), parsed) == tinyxml2::XML_SUCCESS);
        assert(parsed.records.m_day == 5 && gpGame->m_day == 7);
        assert(ironfist::runtime::Phase() == phase);
        assert(ironfist::runtime::LoadGame(name, 1) == ironfist::runtime::LoadResult::LOAD_LOADED);
        assert(gpGame->m_day == 5);
        assert(std::strcmp(gpGame->m_saveName, remote ? "keep.GX1" : name) == 0);
    }
    for (const char* automatic : {"AUTOSAVE", "PLYREXIT"}) {
        gpGame->m_day = 6;
        std::strcpy(gpGame->m_saveName, "keep.GX1");
        assert(ironfist::runtime::SaveGame(automatic, 1));
        assert(std::strcmp(gpGame->m_saveName, "keep.GX1") == 0);
        gpGame->m_day = 7;
        const std::string filename = std::string(automatic) + ".GX1";
        assert(ironfist::runtime::LoadGame(filename.c_str(), 1) == ironfist::runtime::LoadResult::LOAD_LOADED);
        assert(gpGame->m_day == 6);
    }
    // Rejected decoding leaves both the output snapshot and active world intact.
    const auto malformed = platform::Files().Resolve("GAMES/invalid.GX1", platform::FileMode::Write);
    std::ofstream(malformed) << "<ironfist_save><map width='36' height='36' numCellExtras='0'>"
                               "<mapCell index='99999'/></map></ironfist_save>";
    ironfist::save::XmlFile rejected;
    ironfist::SessionData unchanged;
    unchanged.records.m_day = 99;
    auto* activeCells = gpGame->m_worldMap.cells;
    const auto activeDay = gpGame->m_day;
    assert(rejected.Read("GAMES/invalid.GX1", unchanged) != tinyxml2::XML_SUCCESS);
    assert(unchanged.records.m_day == 99 && gpGame->m_day == activeDay);
    assert(gpGame->m_worldMap.cells == activeCells);

    // A binary remote save still delegates to the retail loader.
    const auto binary = platform::Files().Resolve("DATA/RMTOLD.BIN", platform::FileMode::Write);
    std::ofstream(binary, std::ios::binary).write("binary", 6);
    assert(ironfist::runtime::LoadGame("RMTOLD.BIN", 1) == ironfist::runtime::LoadResult::LOAD_RETAIL);
    ironfist::script::Shutdown();
    ClearMapExtra();
    H2_FREE(mapExtra);
    mapExtra = nullptr;
    delete gpAdvManager;
    delete gpGame;
}
