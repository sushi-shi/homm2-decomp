// Optional integration regression using locally installed game assets.
// Exercise the real DrawBackground/DrawFrame path after a walk invalidates
// the background cache, then compare attack redraws with complete frames.
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/Localization.h>
#include <SOURCE/wingraph.h>
#include <SOURCE/combatManager.h>
#include <BASE/executive.h>
#include <BASE/heroWindowManager.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/mouseManager.h>
#include <BASE/bitmap.h>
#include <BASE/palette.h>
#include <BASE/font.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {
constexpr int Width = 640;
constexpr int Height = 480;

void LoadArmy(army& unit, CreatureType type, int index, int hex, ArmyFacing facing) {
    unit.InitClean();
    unit.m_monsterType = type;
    unit.m_monster = gMonsterDatabase[H2EnumIndex(type)];
    unit.m_animationSequence = ARMY_ANIMATION_STAND;
    unit.m_animationFrame = 0;
    unit.m_facing = facing;
    unit.m_drawState = ARMY_DRAW_NORMAL;
    unit.m_quantity = 3;
    unit.m_side = COMBAT_DEFENDER_SIDE;
    unit.m_index = index;
    unit.m_hex = hex;
    unit.m_showQuantity = true;
    unit.m_palette = nullptr;
    unit.m_xOffset = unit.m_yOffset = 0;
    gpResourceManager->PointToFile(
        gpResourceManager->MakeId(cArmyFrameFileNames[H2EnumIndex(type)], 1));
    gpResourceManager->ReadBlock(&unit.m_frameInfo, sizeof(unit.m_frameInfo));
    ModifyFrameInfo(&unit.m_frameInfo, type);
    unit.m_creatureIcon = gpResourceManager->GetIcon(cMonFilename[H2EnumIndex(type)]);
    auto& cell = gpCombatManager->m_hexCells[hex];
    cell.m_occupantSide = COMBAT_DEFENDER_SIDE;
    cell.m_occupantIndex = index;
    cell.m_occupantFrame = ARMY_FACING_NONE;
}
}

int main() {
    if (std::getenv("HOMM2_DATA") == nullptr) {
        std::fprintf(stderr, "Set HOMM2_DATA to your installed game assets. "
                     "See docs/combat-redraw-test.md.\n");
        return 2;
    }
    if (!platform::Startup()) return 2;
    localization::Initialize("/i0");
    std::strcpy(gcCommandLine, "/i0");
    if (!EarlySetup()) return 2;
    InitGraphics();
    gpExec->InitSystem();
    gPalette = gpResourceManager->GetPalette("kb.pal");
    smallFont = gpResourceManager->GetFont("smalfont.fnt");
    gpSoundManager->Open(-1);
    gbColorMice = false;
    gpMouseManager->m_hideCount = 1;

    auto& combat = *gpCombatManager;
    combat.m_nonVisualCombat = false;
    combat.m_combatWindowOpen = true;
    combat.m_showArmyQuantities = true;
    combat.m_inCastleCombat = false;
    combat.m_limitCreature = false;
    combat.m_limitCreatureHex = -1;
    combat.m_backgroundBuffer = new bitmap(BITMAP_TYPE_MEMORY, Width, COMBAT_AREA_HEIGHT);
    combat.m_combatBuffer = new bitmap(BITMAP_TYPE_MEMORY,
        COMBAT_BACKGROUND_COPY_WIDTH, COMBAT_BACKGROUND_COPY_HEIGHT);
    std::strcpy(combat.m_battlefieldBackgroundName, "CBKGSNMT.icn");
    combat.m_battlefieldFringe = FRINGE_NONE;
    combat.m_combatIcons[H2EnumIndex(COMBAT_ICON_STATUS)] = gpResourceManager->GetIcon("textbar.icn");
    gConfig.combatArmyInfoLevel = 0;
    gConfig.combatShadeLevel = 0;
    gConfig.showCombatGrid = false;
    gbCurrArmyDrawn = true;
    giSpellEffectShowType = SPELL_EFFECT_DISPLAY_HIGHLIGHT;
    for (auto& side : combat.m_armies)
        for (auto& unit : side) {
            unit.InitClean();
            unit.m_monster = {};
            unit.m_quantity = 0;
        }
    for (int side = 0; side < 2; ++side) {
        combat.m_heroes[side] = nullptr;
        combat.m_drawHero[side] = false;
        combat.m_drawHeroOverlay[side] = false;
    }
    auto* pixels = gpWindowManager->m_screen->m_pixels;
    std::fill(pixels, pixels + Width * Height, 40);
    int cases = 0, failures = 0;
    for (const auto type : {CREATURE_PEASANT, CREATURE_PIKEMAN})
        for (const auto facing : {ARMY_FACING_LEFT, ARMY_FACING_RIGHT})
            for (const int delta : {-14, -13, -12, -1, 1, 12, 13, 14}) {
                combat.GenerateMap();
                auto& moving = combat.m_armies[1][0];
                auto& neighbour = combat.m_armies[1][1];
                LoadArmy(moving, type, 0, 44, facing);
                LoadArmy(neighbour, type, 1, 44 + delta, ARMY_FACING_LEFT);
                for (const auto sequence : {
                         ARMY_ANIMATION_ATTACK_UP, ARMY_ANIMATION_ATTACK_UP_RETURN,
                         ARMY_ANIMATION_ATTACK_FORWARD, ARMY_ANIMATION_ATTACK_FORWARD_RETURN,
                         ARMY_ANIMATION_ATTACK_DOWN, ARMY_ANIMATION_ATTACK_DOWN_RETURN}) {
                    const int count = moving.m_frameInfo.animationFrameCount[H2EnumIndex(sequence)];
                    if (count == 0) continue;
                    moving.m_animationSequence = ARMY_ANIMATION_STAND;
                    moving.m_animationFrame = 0;
                    combat.m_backgroundDrawn = false;
                    gbLimitToExtent = false;
                    gbComputeExtent = true;
                    combat.DrawFrame(0, 0, 0, 0, 0, 1, 0);
                    combat.ResetLimitCreature();
                    combat.m_limitCreatureCount[1][0] = 1;
                    combat.DrawFrame(0, 1, 0, 1, 0, 0, 0);

                    // Walk leaves a cached scene rather than a clean terrain
                    // buffer. Its next background draw must rebuild the cache
                    // while preserving screen pixels outside the dirty area.
                    combat.m_backgroundDrawn = false;
                    for (int frame = 0; frame < count; ++frame) {
                        moving.m_animationSequence = sequence;
                        moving.m_animationFrame = frame;
                        combat.DrawFrame(0, 1, 0, 0, 0, 1, 0);
                        const std::vector<u8> partial(pixels, pixels + Width * Height);
                        gbLimitToExtent = false;
                        gbComputeExtent = true;
                        combat.DrawFrame(0, 0, 0, 0, 0, 1, 0);
                        ++cases;
                        if (!std::equal(partial.begin(), partial.end(), pixels)) {
                            if (failures < 5)
                                std::fprintf(stderr,
                                    "attack redraw mismatch: type=%d facing=%d delta=%d "
                                    "sequence=%d frame=%d\n", H2EnumIndex(type),
                                    H2EnumIndex(facing), delta, H2EnumIndex(sequence), frame);
                            ++failures;
                        }
                        // Keep the incremental history; the reference render
                        // must not repair the next frame under test.
                        std::copy(partial.begin(), partial.end(), pixels);
                    }
                }
                gpResourceManager->Dispose(moving.m_creatureIcon);
                gpResourceManager->Dispose(neighbour.m_creatureIcon);
            }
    std::fprintf(stderr, "combat background redraw: %d frames, %d mismatches\n", cases, failures);
    std::fflush(nullptr);
    // The fixture owns only a subset of a normal battle's resources. Do not
    // run shutdown routines that assume a fully initialized game session.
    std::_Exit(failures == 0 ? 0 : 1);
}
