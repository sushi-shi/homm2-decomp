#include "session_fixture.h"
#include <IRONFIST/callback.h>
#include <IRONFIST/runtime.h>
#include <IRONFIST/save_xml.h>
#include <IRONFIST/scripting.h>
#include <IRONFIST/state.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>

#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <string>

using namespace ironfist;

static void ReadState(save::XmlFile& saved, bool scriptFirst) {
    save::XmlFile loaded;
    auto* root = loaded.tempDoc->NewElement("ironfist_save");
    loaded.tempDoc->InsertEndChild(root);
    auto addScript = [&]() {
        xml::PushBack(loaded.tempDoc, root, "script",
            "ShareVision(0,2); "
            "ForceComputerPlayerChase(GetHeroInPool(3),GetHeroInPool(4),true); "
            "ToggleAIArmySharing(true); DisallowBuilding(0,3); "
            "initialized=true");
    };
    if (scriptFirst)
        addScript();
    for (auto* child = saved.tempDoc->RootElement()->FirstChildElement(); child;
         child = child->NextSiblingElement()) {
        const std::string name = child->Name();
        if (name == "sharedVision" || name == "forcedHeroChases" || name == "mapVariable"
            || name == "allowAIArmySharing" || name == "disallowedBuildings")
            root->InsertEndChild(child->DeepClone(loaded.tempDoc));
    }
    if (!scriptFirst)
        addScript();
    runtime::ResetAdventureState();
    DecodeSessionFragment(loaded, root);
    assert(luaL_dostring(script::MapState(), "assert(initialized)") == LUA_OK);
}

int main() {
    gpGame = new game{};
    gpGame->m_heroRecs[3].m_id = 3;
    gpGame->m_heroRecs[4].m_id = 4;
    gpGame->m_castleRecs[1].m_id = 1;
    gpGame->m_worldMap.width = gpGame->m_worldMap.height = 0;
    std::array<u8, 16 * 16> visibility{};
    MAP_WIDTH = MAP_HEIGHT = 16;
    mapExtra = visibility.data();
    runtime::ResetAdventureState();
    script::Shutdown();
    gpGame->ShareVision(0, 1);
    gpGame->ForceHeroChase(0, 1, true);
    gpGame->SetAIArmySharing(false);
    gpGame->m_castleRecs[1].DisallowBuilding(7);

    const auto path = platform::Files().Resolve("GAMES/state.GX1", platform::FileMode::Write);
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    save::XmlFile saved;
    assert(saved.Save("GAMES/state.GX1", ironfist::runtime::CaptureSession()) == tinyxml2::XML_SUCCESS);
    auto* legacy = saved.tempDoc->RootElement()->FirstChildElement("mapVariable");
    assert(legacy && std::strcmp(legacy->Attribute("id"), "_AICHASE_0_1_") == 0);

    for (const bool scriptFirst : {false, true}) {
        ReadState(saved, scriptFirst);
        const auto& vision = state::Get().adventure.sharePlayerVision;
        assert(vision[0][1] && !vision[1][0] && !vision[0][2]);
        assert(gpGame->IsHeroChaseForced(0, 1));
        assert(!gpGame->IsHeroChaseForced(3, 4));
        assert(!gpGame->IsAIArmySharingAllowed());
        assert(gpGame->m_castleRecs[1].IsBuildingDisallowed(7));
        assert(!gpGame->m_castleRecs[0].IsBuildingDisallowed(3));
        // New exploration after loading must reach only the saved recipient.
        visibility.fill(0);
        for (i32 player = 0; player < GAME_PLAYER_COUNT; ++player)
            gbHumanPlayer[player] = true;
        gpGame->SetVisibility(8, 8, 0, 1);
        assert(visibility[8 * MAP_WIDTH + 8] == 3);
    }

    // Empty snapshots must preserve cancellations, despite top-level defaults.
    runtime::ResetAdventureState();
    script::Shutdown();
    save::XmlFile empty;
    assert(empty.Save("GAMES/state.GX1", ironfist::runtime::CaptureSession()) == tinyxml2::XML_SUCCESS);
    ReadState(empty, false);
    assert(!state::Get().adventure.sharePlayerVision[0][2]);
    assert(!gpGame->IsHeroChaseForced(3, 4));

    save::XmlFile defaults;
    defaults.tempDoc->Parse("<ironfist_save/>");
    ReadState(defaults, true);
    assert(state::Get().adventure.sharePlayerVision[0][2]);
    assert(gpGame->IsHeroChaseForced(3, 4));

    // Upstream saves have only reserved variable records. They restore engine
    // state without creating a Lua variable or requiring a map script.
    runtime::ResetAdventureState();
    script::Shutdown();
    save::XmlFile old;
    old.tempDoc->Parse(R"(<ironfist_save>
      <mapVariable id="_AICHASE_2_3_" type="boolean" value="true"/>
      <mapVariable id="_AICHASE_-1_3_" type="boolean" value="true"/>
    </ironfist_save>)");
    DecodeSessionFragment(old, old.tempDoc->RootElement());
    assert(gpGame->IsHeroChaseForced(2, 3));

    // Invalid endpoints in new groups are ignored.
    save::XmlFile invalid;
    invalid.tempDoc->Parse(R"(<ironfist_save>
      <sharedVision><share source="-1" destination="0"/><share source="0" destination="99"/></sharedVision>
      <forcedHeroChases><chase source="999" destination="0"/><chase destination="1"/></forcedHeroChases>
    </ironfist_save>)");
    DecodeSessionFragment(invalid, invalid.tempDoc->RootElement());
    assert(!gpGame->IsHeroChaseForced(2, 3));
    script::Shutdown();
    mapExtra = nullptr;
    delete gpGame;
}
