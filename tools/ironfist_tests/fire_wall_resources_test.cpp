#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <IRONFIST/state.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/advManager.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <array>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>

int main() {
    gpResourceManager = new resourceManager;
    gpSoundManager = new soundManager;
    gpWindowManager = new heroWindowManager;
    gpMouseManager = new mouseManager;
    const char* name = gCombatFxNames[H2EnumIndex(COMBAT_EFFECT_FIRE_BOMB)];
    const u32l id = gpResourceManager->MakeId(name, 1);
    // A two-frame, one-pixel ICN in a synthetic aggregate. Both frames use
    // the same literal pixel followed by the standard end-of-image command.
    const u16 frames = 2;
    const u32 length = 2 * sizeof(IconEntry) + 3;
    std::array<IconEntry, 2> entries{};
    for (auto& entry : entries) {
        entry.w = entry.h = 1;
        entry.srcOffset = 2 * sizeof(IconEntry);
    }
    const std::array<u8, 3> pixels = {1, 42, 0x80};
    const auto path = platform::Files().Resolve("GAMES/wall-test.bin", platform::FileMode::Write);
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    {
        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(&frames), sizeof(frames));
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(reinterpret_cast<const char*>(entries.data()), sizeof(entries));
        file.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
        assert(file.good());
    }
    aggEntry entry{id, 0, sizeof(frames) + sizeof(length) + length};
    gpResourceManager->m_aggregateDir[0] = &entry;
    gpResourceManager->m_aggregateEntryCount[0] = 1;
    gpResourceManager->m_aggregateFd[0] = platform::FileOpen("GAMES/wall-test.bin", platform::FileMode::Read);
    assert(gpResourceManager->m_aggregateFd[0] != -1);
    icon* held = gpResourceManager->GetIcon(name);
    assert(held->m_refCount == 1 && held->m_frameCount == 2);

    gpCombatManager = new combatManager;
    gpWindowManager->m_screen = new bitmap(BITMAP_TYPE_MEMORY, 640, 480);
    auto& combat = *gpCombatManager;
    combat.m_nonVisualCombat = false;
    combat.m_combatWindowOpen = true;
    combat.m_combatMessagePending = false;
    combat.m_currentSide = COMBAT_ATTACKER_SIDE;
    for (i32 side = 0; side < COMBAT_SIDE_COUNT; ++side) {
        combat.m_armyCount[side] = 0;
        combat.m_heroes[side] = nullptr;
        combat.m_heroIcons[side] = combat.m_heroOverlayIcons[side] = nullptr;
        combat.m_drawHero[side] = combat.m_drawHeroOverlay[side] = false;
    }
    std::memset(combat.m_limitCreatureCount, 0, sizeof(combat.m_limitCreatureCount));
    combat.m_hexCells[20].m_x = 100;
    combat.m_hexCells[21].m_x = 102;
    combat.m_hexCells[20].m_gridTop = combat.m_hexCells[21].m_gridTop = 100;
    gbNoShowCombat = false;
    gConfig.combatArmyInfoLevel = 0;
    gConfig.combatSpeed = 0;
    auto& walls = ironfist::state::Get().combat.spell.fireBombWalls;
    walls = {{20, 2, 0}, {21, 2, 1}};

    // More cycles than the entire positive range of the resource counter.
    for (i32 i = 0; i < 40000; ++i) {
        combat.CycleCombatScreen();
        assert(held->m_refCount == 1);
        assert(walls[0].currentFrame == (i + 1) % 2);
    }
    for (i32 i = 0; i < 50; ++i) {
        combat.DrawFrame(0, 0, 0, 0, 0, 0, 0);
        assert(held->m_refCount == 1);
        assert(gpWindowManager->m_screen->m_pixels[100 * 640 + 100] == 42);
        assert(gpWindowManager->m_screen->m_pixels[100 * 640 + 102] == 42);
    }

    // Include extent calculation in the same frame, then an early finish.
    combat.m_heroIcons[0] = held;
    combat.m_drawHero[0] = true;
    combat.m_heroSpriteIndex[0] = 0;
    combat.m_heroAnimationState[0] = 0;
    combat.m_heroAnimationFrame[0] = 0;
    sCmbtHero[0].animationFrames[0][0] = 0;
    combat.DrawFrame(0, 1, 0, 0, 0, 0, 0);
    assert(held->m_refCount == 1);
    combat.DrawFrame(0, 1, 0, 1, 0, 0, 0);
    assert(held->m_refCount == 1);
    combat.m_heroIcons[0] = nullptr;
    combat.m_drawHero[0] = false;

    gpResourceManager->Dispose(held);
    assert(gpResourceManager->Query(id) == nullptr);
    combat.CycleCombatScreen();
    assert(gpResourceManager->Query(id) == nullptr);
    combat.DrawFrame(0, 0, 0, 0, 0, 0, 0);
    assert(gpResourceManager->Query(id) == nullptr);
    walls.clear();
    delete gpWindowManager->m_screen;
    delete gpMouseManager;
    delete gpWindowManager;
    delete gpCombatManager;
    platform::FileClose(gpResourceManager->m_aggregateFd[0]);
    gpResourceManager->m_aggregateDir[0] = nullptr;
    delete gpResourceManager;
    delete gpSoundManager;
}
