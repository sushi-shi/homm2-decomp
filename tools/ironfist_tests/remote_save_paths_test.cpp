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
        assert(ironfist::save::SaveGame(name, 0));
        const bool remote = name[0] != 'm';
        const auto logical = std::string(remote ? "DATA/" : "GAMES/") + name;
        const auto path = platform::Files().Resolve(logical.c_str(), platform::FileMode::Read);
        assert(std::filesystem::exists(path));
        if (remote)
            assert(std::strcmp(gpGame->m_saveName, "keep.GX1") == 0);
        gpGame->m_day = 7;
        assert(ironfist::save::LoadGame(name, 1));
        assert(gpGame->m_day == 5);
        assert(std::strcmp(gpGame->m_saveName, remote ? "keep.GX1" : name) == 0);
    }
    for (const char* automatic : {"AUTOSAVE", "PLYREXIT"}) {
        gpGame->m_day = 6;
        std::strcpy(gpGame->m_saveName, "keep.GX1");
        assert(ironfist::save::SaveGame(automatic, 1));
        assert(std::strcmp(gpGame->m_saveName, "keep.GX1") == 0);
        gpGame->m_day = 7;
        const std::string filename = std::string(automatic) + ".GX1";
        assert(ironfist::save::LoadGame(filename.c_str(), 1));
        assert(gpGame->m_day == 6);
    }
    // A binary remote save still delegates to the retail loader.
    const auto binary = platform::Files().Resolve("DATA/RMTOLD.BIN", platform::FileMode::Write);
    std::ofstream(binary, std::ios::binary).write("binary", 6);
    assert(!ironfist::save::LoadGame("RMTOLD.BIN", 1));
    ironfist::script::Shutdown();
    ClearMapExtra();
    H2_FREE(mapExtra);
    mapExtra = nullptr;
    delete gpAdvManager;
    delete gpGame;
}
