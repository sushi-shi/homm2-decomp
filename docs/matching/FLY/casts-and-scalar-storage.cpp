// VC6 SP5 C67/S61 source products. Parent d2570cb2.
// Source record, not a compilable replacement TU; no full cross-phase/state census.
// See docs/reconstruction/C67-S61-B82.md and fixed retail operand checker.

// c67-fly-position 0x4b5ce
// build/source-variant-batch/c67-fly-position/results.json
// Complete 4 arms; elapsed 2.554085s; input restored.
// Disposition: retain all removals with combined native proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 1 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 2 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 3 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
/* AXIS fromX
BEFORE
static_cast<float>(fromX)
AFTER remove
fromX
*/

/* AXIS sourceY
BEFORE
static_cast<float>(sourceY)
AFTER remove
sourceY
*/

// c67-fly-occupants 0x4b5ce
// build/source-variant-batch/c67-fly-occupants/results.json
// Complete 16 arms; elapsed 8.503095s; input restored.
// Disposition: retain all removals with combined native proof.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 1 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 2 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 3 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 4 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 5 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 6 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 7 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 8 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 9 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 10 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 11 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 12 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 13 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 14 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 15 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
/* AXIS destination_side
BEFORE
    gpCombatManager->m_hexCells[destination].m_occupantSide =
        static_cast<i8>(gpCombatManager->m_currentArmySide);
AFTER remove
    gpCombatManager->m_hexCells[destination].m_occupantSide =
        gpCombatManager->m_currentArmySide;
*/

/* AXIS destination_index
BEFORE
    gpCombatManager->m_hexCells[destination].m_occupantIndex =
        static_cast<i8>(gpCombatManager->m_currentArmyIndex);
AFTER remove
    gpCombatManager->m_hexCells[destination].m_occupantIndex =
        gpCombatManager->m_currentArmyIndex;
*/

/* AXIS endRearHex_side
BEFORE
        gpCombatManager->m_hexCells[endRearHex].m_occupantSide =
            static_cast<i8>(gpCombatManager->m_currentArmySide);
AFTER remove
        gpCombatManager->m_hexCells[endRearHex].m_occupantSide =
            gpCombatManager->m_currentArmySide;
*/

/* AXIS endRearHex_index
BEFORE
        gpCombatManager->m_hexCells[endRearHex].m_occupantIndex =
            static_cast<i8>(gpCombatManager->m_currentArmyIndex);
AFTER remove
        gpCombatManager->m_hexCells[endRearHex].m_occupantIndex =
            gpCombatManager->m_currentArmyIndex;
*/

// s61-flight-locals 0x4b21e
// build/source-variant-batch/s61-flight-locals/results.json
// Complete 8 arms; elapsed 4.642059s; input restored.
// Disposition: retain original; every tested deletion changes native text.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 918 / cf92613b609ae705 / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 1 / 918 / ceaf613719dfd457 / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 2 / 918 / ceaf613719dfd457 / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 3 / 918 / 7fcfad6c1d21398f / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 4 / 918 / 78c0023bc94d8d50 / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 5 / 918 / b8ae83149fc10f5b / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 6 / 918 / b8ae83149fc10f5b / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
// 7 / 918 / bcb804a01ee90a3e / d370d4fb85d8a201 / 45:45 / 45,0,0,0 / True
/* AXIS spare
BEFORE
    i32 H2_UNUSED(spare);

AFTER remove

*/

/* AXIS cost
BEFORE
    i32 H2_UNUSED(cost);

AFTER remove

*/

/* AXIS otherHex
BEFORE
    i32 H2_UNUSED(otherHex);

AFTER remove

*/

// s61-fly-locals 0x4b5ce
// build/source-variant-batch/s61-fly-locals/results.json
// Complete 8 arms; elapsed 4.85763s; input restored.
// Disposition: retain original; every tested deletion changes native text.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 1 / 2524 / fd5df7147b9299a1 / d80737009aaeeceb / 90:86 / 86,0,0,0 / False
// 2 / 2524 / d7c36fa606a395f6 / d80737009aaeeceb / 90:86 / 86,0,0,0 / False
// 3 / 2518 / 7454db5e5622422a / 8827407804f9c69c / 90:86 / 86,0,0,0 / False
// 4 / 2524 / d7c36fa606a395f6 / d80737009aaeeceb / 90:86 / 86,0,0,0 / False
// 5 / 2518 / 7454db5e5622422a / 8827407804f9c69c / 90:86 / 86,0,0,0 / False
// 6 / 2518 / 53c12456e2747afb / 8827407804f9c69c / 90:86 / 86,0,0,0 / False
// 7 / 2515 / cb241ca0e2e1033c / b3dd12ec45494406 / 90:86 / 86,0,0,0 / False
/* AXIS pad9
BEFORE
    i32 H2_UNUSED(pad9);

AFTER remove

*/

/* AXIS dead2
BEFORE
    i32 H2_UNUSED(dead2);

AFTER remove

*/

/* AXIS slack
BEFORE
    i32 H2_UNUSED(slack);

AFTER remove

*/

// s61-fly-frame-store 0x4b5ce
// build/source-variant-batch/s61-fly-frame-store/results.json
// Complete 2 arms; elapsed 1.565857s; input restored.
// Disposition: retain original; every tested deletion changes native text.
// trial / bytes / text hash / normalized relocation hash / blocks candidate:retail / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 2530 / 221742905cbb15e9 / d4adc2796d33a2d8 / 90:86 / 86,0,0,0 / False
// 1 / 2512 / 65f868676232612a / 6ed6a3737dcb7922 / 90:86 / 85,1,0,0 / False
/* AXIS overwritten_frame_count
BEFORE
                    frameCount0 +=
                        m_frameInfo.animationFrameCount[IDX(ARMY_ANIMATION_WALK_BEGIN_STANDING)];

AFTER remove

*/
