#include "session_fixture.h"
#include <IRONFIST/callback.h>
#include <IRONFIST/paths.h>
#include <IRONFIST/save_xml.h>
#include <IRONFIST/scripting.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

using namespace ironfist;

static void WriteFile(const std::string& path, const std::string& contents) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream output(path);
    output << contents;
    assert(output.good());
}

static void ReadRoot(const char* embedded) {
    gpGame->m_day = 7;
    save::XmlFile file;
    auto* root = file.m_document->NewElement("ironfist_save");
    file.m_document->InsertEndChild(root);
    xml::PushBack(file.m_document, root, "mapFilename", "loaded.mx2");
    xml::PushBack(file.m_document, root, "day", 3);
    xml::PushBack(file.m_document, root, "week", 1);
    xml::PushBack(file.m_document, root, "month", 1);
    if (embedded) {
        xml::PushBack(file.m_document, root, "script", embedded);
        if (*embedded) {
            auto* variable = file.m_document->NewElement("mapVariable");
            variable->SetAttribute("id", "counter");
            variable->SetAttribute("type", "int");
            variable->SetAttribute("value", "99");
            root->InsertEndChild(variable);
        }
    }
    DecodeSessionFragment(file, root);
}

static void CheckSavedSource(const std::string& expected) {
    // Saving must use the active source even if the current filename is stale.
    std::strcpy(gMapName, "other.mx2");
    const auto path = platform::Files().Resolve("GAMES/test.GX1", platform::FileMode::Write);
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    save::XmlFile output;
    assert(output.Save("GAMES/test.GX1", ironfist::runtime::CaptureSession()) == tinyxml2::XML_SUCCESS);
    const auto* node = output.m_document->RootElement()->FirstChildElement("script");
    if (expected.empty())
        assert(node == nullptr);
    else
        assert(node && node->GetText() && expected == node->GetText());
}

int main() {
    gpGame = new game{};
    gpGame->m_worldMap.width = gpGame->m_worldMap.height = 0;
    MAP_WIDTH = MAP_HEIGHT = 0;
    const std::string generic = "function OnNewDay() return 'artifact' end";
    WriteFile(ResolveDataPath("SCRIPTS/GENERIC/artifacts.lua"), generic);
    const std::string oldMap = "function OnNewDay() return 'old-map' end";
    const std::string loaded = "assert(GetDay()==3); counter=5; mapVariables={'counter'}; "
                               "function OnNewDay() return 'loaded-map' end";

    for (const char* empty : {static_cast<const char*>(nullptr), ""}) {
        script::InitializeFromSave(oldMap);
        assert(script::InvokeResult<std::string>("OnNewDay") == "old-map");
        ReadRoot(empty);
        assert(script::MapState() == nullptr);
        assert(script::InvokeResult<std::string>("OnNewDay") == "artifact");
        CheckSavedSource("");
    }

    script::InitializeFromSave(oldMap);
    ReadRoot(loaded.c_str());
    assert(script::InvokeResult<std::string>("OnNewDay") == "loaded-map");
    assert(luaL_dostring(script::MapState(), "assert(counter==99)") == LUA_OK);
    assert(std::strcmp(gMapName, "loaded.mx2") == 0);
    CheckSavedSource(loaded);
    // Repeated loads and saves do not depend on an installed map script.
    ReadRoot(loaded.c_str());
    CheckSavedSource(loaded);

    const auto installed = ResolveDataPath("SCRIPTS/installed.mx2.lua");
    WriteFile(installed, oldMap);
    script::InitializeMap("installed.mx2");
    WriteFile(installed, loaded);
    CheckSavedSource(oldMap);
    std::filesystem::remove(installed);
    CheckSavedSource(oldMap);

    const std::string prefixed = "\xEF\xBB\xBF#!/usr/bin/lua\n" + oldMap;
    WriteFile(installed, prefixed);
    script::InitializeMap("installed.mx2");
    assert(script::InvokeResult<std::string>("OnNewDay") == "old-map");
    CheckSavedSource(prefixed);
    ReadRoot(prefixed.c_str());
    assert(script::InvokeResult<std::string>("OnNewDay") == "old-map");
    CheckSavedSource(prefixed);

    for (const auto* contents : {"", "binary"}) {
        script::InitializeFromSave(oldMap);
        WriteFile(platform::Files().Resolve("GAMES/retail.GM1", platform::FileMode::Write), contents);
        assert(runtime::LoadGame("retail.GM1", 1) == runtime::LoadResult::LOAD_RETAIL);
        runtime::RetailGameLoaded();
        assert(script::MapState() == nullptr);
        assert(script::InvokeResult<std::string>("OnNewDay") == "artifact");
        CheckSavedSource("");
    }
    script::Shutdown();
    assert(script::ActiveScriptContents().empty());
    delete gpGame;
}
