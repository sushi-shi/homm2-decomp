#include <SOURCE/game.h>
#include <SOURCE/army.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <IRONFIST/save_xml.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/runtime.h>
#include <IRONFIST/state.h>
#include <IRONFIST/lua.h>
#include <IRONFIST/callback.h>
#include <PLATFORM/Platform.h>
#include <filesystem>
#include <cstdio>
#include <cstring>

extern i32 iMemEntries;

int main() {
    gpGame = new game{};
    ironfist::script::InitializeFromSave("function OnNewDay() return 'old-map' end");
    ironfist::state::Get().adventure.sharePlayerVision[0][1] = true;
    ironfist::runtime::ResetAdventureState(); // The production load entry does this.
    ironfist::save::XmlFile saved;
    saved.tempDoc->Parse("<ironfist_save><numPlayers>1</numPlayers><day>1</day><week>1</week><month>1</month></ironfist_save>");
    saved.ReadRoot(saved.tempDoc->RootElement());
    auto* state = ironfist::script::MapState();
    lua_getglobal(state, "OnNewDay");
    lua_pcall(state, 0, 1, 0);
    std::printf("loading no-script root retains callback=%s\n", lua_tostring(state, -1));
    std::printf("shared vision after reset/read=%d\n", ironfist::state::Get().adventure.sharePlayerVision[0][1]);

    ironfist::runtime::ResetAdventureState();
    ironfist::script::InitializeFromSave("function OnNewDay() end");
    gpGame->ForceHeroChase(0, 1, true);
    ironfist::save::XmlFile output;
    auto* root = output.tempDoc->NewElement("ironfist_save");
    output.tempDoc->InsertEndChild(root);
    output.WriteMapVariables(root);
    std::printf("forced chase with no user variables writes entries=%d\n", root->FirstChild() != nullptr);

    // A save loaded from embedded text must retain that text when saved again.
    std::strcpy(gMapName, "missing.mx2");
    gpGame->m_worldMap.width = gpGame->m_worldMap.height = 0;
    MAP_WIDTH = MAP_HEIGHT = 0;
    ironfist::save::XmlFile resaved;
    const auto outputPath = platform::Files().Resolve("GAMES/probe.GX1", platform::FileMode::Write);
    std::filesystem::create_directories(std::filesystem::path(outputPath).parent_path());
    auto error = resaved.Save("GAMES/probe.GX1");
    std::printf("resave error=%d embedded script present=%d\n", error,
        resaved.tempDoc->RootElement()->FirstChildElement("script") != nullptr);
    army target;
    target.m_monsterType = CREATURE_PEASANT;
    gMonsterDatabase[H2EnumIndex(CREATURE_PEASANT)].hitPoints = 100;
    target.SetSpellInfluence(ARMY_SPELL_INFLUENCE_FORCE_SHIELD, 3);
    ironfist::state::Get().combat.stack.forceShieldHP[&target] = 25;
    const auto applied = target.SetSpellInfluence(ARMY_SPELL_INFLUENCE_FORCE_SHIELD, 3);
    std::printf("recast partially depleted shield: applied=%d remaining HP=%d\n", applied,
        ironfist::state::Get().combat.stack.forceShieldHP[&target]);
    const auto remoteWrite = platform::Files().Resolve("DATA/RMTTEST.BIN", platform::FileMode::Write);
    const auto remoteRead = platform::Files().Resolve("GAMES/RMTTEST.BIN", platform::FileMode::Read);
    std::printf("remote save write=%s read=%s\n", remoteWrite.c_str(), remoteRead.c_str());
    ironfist::save::XmlFile mapSave;
    mapSave.tempDoc->Parse("<map width='36' height='36' numCellExtras='1'/>");
    const int entriesBefore = iMemEntries;
    mapSave.ReadMap(mapSave.tempDoc->RootElement());
    const int entriesLoaded = iMemEntries;
    gpGame->m_worldMap.Close();
    std::printf("map allocation counts: before=%d loaded=%d closed=%d error=%s\n",
        entriesBefore, entriesLoaded, iMemEntries, gText);
    ironfist::script::Shutdown();
    delete gpGame;
}
