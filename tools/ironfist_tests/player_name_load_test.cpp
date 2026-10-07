#include "session_fixture.h"
#include <BASE/Utf8.h>
#include <IRONFIST/save_xml.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <string>

int main() {
    gpGame = new game{};
    gpGame->m_worldMap.width = gpGame->m_worldMap.height = 0;
    MAP_WIDTH = MAP_HEIGHT = 0;
    const std::array<std::string, GAME_PLAYER_COUNT> names = {
        "Alice", "Żółć", "Игрок", "", "12345678901234567890", "A & B < C"
    };
    for (size_t i = 0; i < names.size(); ++i)
        utf8::Copy(cPlayerNames[i], names[i].c_str());
    const auto path = platform::Files().Resolve("GAMES/names.GX1", platform::FileMode::Write);
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    ironfist::save::XmlFile saved;
    assert(saved.Save("GAMES/names.GX1", ironfist::runtime::CaptureSession()) == tinyxml2::XML_SUCCESS);
    ironfist::save::XmlFile loaded;
    auto* root = loaded.tempDoc->NewElement("ironfist_save");
    loaded.tempDoc->InsertEndChild(root);
    for (auto* name = saved.tempDoc->RootElement()->FirstChildElement("playerNames");
         name; name = name->NextSiblingElement("playerNames")) {
        root->InsertEndChild(name->DeepClone(loaded.tempDoc));
    }
    std::memset(cPlayerNames, 0, sizeof(cPlayerNames));
    DecodeSessionFragment(loaded, root);
    for (size_t i = 0; i < names.size(); ++i)
        assert(names[i] == cPlayerNames[i]);

    ironfist::save::XmlFile compatibility;
    compatibility.tempDoc->Parse(R"(<ironfist_save>
      <playerNames index="-1" value="bad"/>
      <playerNames index="6" value="bad"/>
      <playerNames value="bad"/>
      <playerNames index="1" value="1234567890123456789🙂"/>
      <playerNames index="2">Legacy</playerNames>
      <playerNames index="3" value="">must stay empty</playerNames>
    </ironfist_save>)");
    DecodeSessionFragment(compatibility, compatibility.tempDoc->RootElement());
    assert(names[0] == cPlayerNames[0]);
    assert(std::strcmp(cPlayerNames[1], "1234567890123456789") == 0);
    assert(utf8::IsValid(cPlayerNames[1]));
    assert(std::strcmp(cPlayerNames[2], "Legacy") == 0);
    assert(cPlayerNames[3][0] == 0);
    assert(names[4] == cPlayerNames[4] && names[5] == cPlayerNames[5]);
    delete gpGame;
}
