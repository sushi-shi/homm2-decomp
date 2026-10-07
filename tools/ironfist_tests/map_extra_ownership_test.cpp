#include "session_fixture.h"
#include <BASE/Misc.h>
#include <IRONFIST/save_xml.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>

#include <cassert>

extern i32 iMemEntries;
extern i32 giTotalMemAllocated;

static void LoadMap(i32 count) {
    ironfist::save::XmlFile saved;
    auto* root = saved.tempDoc->NewElement("map");
    saved.tempDoc->InsertEndChild(root);
    root->SetAttribute("width", 36);
    root->SetAttribute("height", 36);
    root->SetAttribute("numCellExtras", count);
    for (i32 i = 0; i < count; ++i) {
        auto* extra = saved.tempDoc->NewElement("mapCellExtra");
        extra->SetAttribute("index", i);
        extra->SetAttribute("nextIdx", i + 1 < count ? i + 1 : 0);
        extra->SetAttribute("objectIndex", 20 + i);
        root->InsertEndChild(extra);
    }
    ironfist::SessionData data;
    saved.ReadMap(root, data);
    ironfist::runtime::RestoreSession(data);
}

int main() {
    gpGame = new game{};
    const i32 initialEntries = iMemEntries;
    const i32 initialBytes = giTotalMemAllocated;
    auto& map = gpGame->m_worldMap;
    for (i32 iteration = 0; iteration < 30; ++iteration) {
        for (const i32 count : {2, 3, 0, 1}) {
            LoadMap(count);
            assert(iMemEntries == initialEntries + 2 + (count != 0));
            assert(giTotalMemAllocated == initialBytes
                + static_cast<i32>(36 * 36 * (sizeof(mapCell) + sizeof(u8)) + count * sizeof(mapCellExtra)));
            assert(map.extraCount == count);
            if (count) {
                assert(map.extras[count - 1].objectIndex == 20 + count - 1);
                assert(map.extras[count - 1].nextIndex == 0);
            } else {
                assert(map.extras == nullptr);
            }
        }
        // The normal map editor/adventure growth path uses the same owner.
        const i32 added = map.GetNewCellExtraIndex();
        assert(added == 1 && map.extraCount > added);
        assert(iMemEntries == initialEntries + 3);
        assert(map.extras[0].objectIndex == 20);
        map.Close();
        H2_FREE(mapExtra);
        mapExtra = nullptr;
        assert(iMemEntries == initialEntries);
        assert(giTotalMemAllocated == initialBytes);
        assert(map.extras == nullptr && map.cells == nullptr);
    }
    delete gpGame;
    assert(iMemEntries == initialEntries && giTotalMemAllocated == initialBytes);
}
