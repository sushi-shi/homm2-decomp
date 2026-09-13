// VC6 SP5 C62/S58/G72-G74 complete measured source families.
// Parent a03eb49c: clean source, no TU-state census or universal closure claim.
// Source record, not compilable replacement TU input.
// See docs/reconstruction/C62-C63-S58-B78.md for native proof and limits.

// c62-ctor src/SOURCE/CMBTMGR.cpp RVA 0x25de0
// build/source-variant-batch/c62-ctor/results.json
// Complete 2 arms; elapsed 2.00387s; input restored.
// Disposition: retain trial 1; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 431 / f2948da48a1b48f6 / 3c13c9558720da6c / 7:7 / 7,0,0,0 / True
// 1 / 431 / f2948da48a1b48f6 / 3c13c9558720da6c / 7:7 / 7,0,0,0 / True
/* AXIS named_command
BEFORE
CombatMessageCommand(0)

AFTER remove
COMBAT_MESSAGE_COMMAND_DEFAULT
*/

// c62-handler src/SOURCE/CMBTMGR.cpp RVA 0x2b527
// build/source-variant-batch/c62-handler/results.json
// Complete 2 arms; elapsed 1.954557s; input restored.
// Disposition: retain trial 1; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 691 / 2fb19aed4dd79a89 / d4e5397c5287175f / 44:41 / 40,0,0,1 / False
// 1 / 691 / 2fb19aed4dd79a89 / d4e5397c5287175f / 44:41 / 40,0,0,1 / False
/* AXIS command_domain
BEFORE
BaseWidgetCommand(SYSTEM_OPTION_SPEED_BUTTON)

AFTER remove
WIDGET_COMMAND_DIALOG_SELECT
*/

// c62-adjacency src/SOURCE/CMBTMGR.cpp RVA 0x2683f
// build/source-variant-batch/c62-adjacency/results.json
// Complete 2 arms; elapsed 1.954009s; input restored.
// Disposition: retain trial 1; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 447 / 0e21f400708f2ace / ba4ab3a4343a7636 / 39:39 / 38,1,0,0 / True
// 1 / 447 / 0e21f400708f2ace / 5adb29e7f2a5cd37 / 39:39 / 38,1,0,0 / True
/* AXIS byte_result
BEFORE
static_cast<i8>(toHex)

AFTER remove
toHex
*/

// c62-close src/SOURCE/CMBTMGR.cpp RVA 0x26e25
// build/source-variant-batch/c62-close/results.json
// Complete 4 arms; elapsed 3.385727s; input restored.
// Disposition: retain trial 3; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 904 / 12f163ace0d13b66 / cca4ddafe921f0be / 36:36 / 36,0,0,0 / True
// 1 / 904 / 12f163ace0d13b66 / cca4ddafe921f0be / 36:36 / 36,0,0,0 / True
// 2 / 904 / 12f163ace0d13b66 / cca4ddafe921f0be / 36:36 / 36,0,0,0 / True
// 3 / 904 / 12f163ace0d13b66 / cca4ddafe921f0be / 36:36 / 36,0,0,0 / True
/* AXIS map_guardians
BEFORE
m_battlefieldCell->m_objectMetadata].guardianCount = static_cast<u8>(total)

AFTER remove
m_battlefieldCell->m_objectMetadata].guardianCount = total
*/
/* AXIS hero_guardians
BEFORE
combatHero->m_occupiedTown].guardianCount = static_cast<u8>(total)

AFTER remove
combatHero->m_occupiedTown].guardianCount = total
*/

// c62-group src/SOURCE/CMBTMGR.cpp RVA 0x271ad
// build/source-variant-batch/c62-group/results.json
// Complete 2 arms; elapsed 2.004678s; input restored.
// Disposition: retain trial 1; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 705 / 209b5018c7005920 / e4cae2f1b856abd0 / 22:22 / 22,0,0,0 / True
// 1 / 705 / 209b5018c7005920 / e4cae2f1b856abd0 / 22:22 / 22,0,0,0 / True
/* AXIS quantity
BEFORE
static_cast<i16>(m_armies[IDX(side)][index].m_quantity)

AFTER remove
m_armies[IDX(side)][index].m_quantity
*/

// c62-obstacle src/SOURCE/CMBTMGR.cpp RVA 0x2a1c4
// build/source-variant-batch/c62-obstacle/results.json
// Complete 2 arms; elapsed 1.974721s; input restored.
// Disposition: retain trial 1; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 1130 / 020e2c2f7be7de6f / cd210ece63bf1b4a / 49:49 / 49,0,0,0 / True
// 1 / 1130 / 020e2c2f7be7de6f / cd210ece63bf1b4a / 49:49 / 49,0,0,0 / True
/* AXIS obstacle_index
BEFORE
static_cast<i8>(m_obstacleCount)

AFTER remove
m_obstacleCount
*/

// c62-grid src/SOURCE/CMBTMGR.cpp RVA 0x2746e
// build/source-variant-batch/c62-grid/results.json
// Complete 64 arms; elapsed 43.736834s; input restored.
// Disposition: retain trial 63; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 1 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 2 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 3 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 4 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 5 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 6 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 7 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 8 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 9 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 10 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 11 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 12 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 13 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 14 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 15 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 16 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 17 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 18 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 19 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 20 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 21 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 22 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 23 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 24 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 25 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 26 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 27 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 28 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 29 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 30 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 31 / 580 / fe4b877b67df904a / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 32 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 33 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 34 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 35 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 36 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 37 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 38 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 39 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 40 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 41 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 42 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 43 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 44 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 45 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 46 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 47 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 48 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 49 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 50 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 51 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 52 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 53 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 54 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 55 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 56 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 57 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 58 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 59 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 60 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 61 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 62 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 63 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
/* AXIS row_owner
BEFORE
    u32 y;

AFTER remove
    i32 y;
*/
/* AXIS row_test
BEFORE
static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT

AFTER remove
y < COMBAT_GRID_ROW_COUNT
*/
/* AXIS center_y
BEFORE
static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN)

AFTER remove
(y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN
*/
/* AXIS center_x
BEFORE
static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            )

AFTER remove
(x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
*/
/* AXIS grid_left
BEFORE
static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            )

AFTER remove
(x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
*/
/* AXIS grid_top
BEFORE
static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN)

AFTER remove
y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN
*/

// c62-cat src/SOURCE/CMBTMGR.cpp RVA 0x28bb8
// build/source-variant-batch/c62-cat/results.json
// Complete 64 arms; elapsed 45.515372s; input restored.
// Disposition: retain trial 63; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 1 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 2 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 3 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 4 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 5 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 6 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 7 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 8 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 9 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 10 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 11 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 12 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 13 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 14 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 15 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 16 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 17 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 18 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 19 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 20 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 21 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 22 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 23 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 24 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 25 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 26 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 27 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 28 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 29 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 30 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 31 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 32 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 33 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 34 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 35 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 36 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 37 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 38 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 39 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 40 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 41 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 42 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 43 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 44 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 45 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 46 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 47 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 48 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 49 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 50 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 51 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 52 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 53 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 54 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 55 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 56 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 57 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 58 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 59 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 60 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 61 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 62 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 63 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
/* AXIS projectile_x
BEFORE
static_cast<float>(startX8)

AFTER remove
startX8
*/
/* AXIS projectile_y
BEFORE
static_cast<float>(startY9)

AFTER remove
startY9
*/
/* AXIS midpoint
BEFORE
static_cast<float>(
        (startX8 + impactX10) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
    )

AFTER remove
(startX8 + impactX10) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
*/
/* AXIS bounce
BEFORE
static_cast<float>(
        (impactY6 - (impactX10 - startX8) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY6 * COMBAT_CATAPULT_TARGET_ARC_SCALE
    )

AFTER remove
(impactY6 - (impactX10 - startX8) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY6 * COMBAT_CATAPULT_TARGET_ARC_SCALE
*/
/* AXIS x_step
BEFORE
static_cast<float>((midpointX6 - startX8) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR)

AFTER remove
(midpointX6 - startX8) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR
*/
/* AXIS cloud_name
BEFORE
const_cast<char*>(missShot19 ? "smalclod.icn" : "lichclod.icn")

AFTER remove
missShot19 ? "smalclod.icn" : "lichclod.icn"
*/

// c62-walltrace src/SOURCE/CMBTMGR.cpp RVA 0x2aabf
// build/source-variant-batch/c62-walltrace/results.json
// Complete 144 arms; elapsed 106.94513s; input restored.
// Disposition: retain trial 143; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 1 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 2 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 3 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 4 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 5 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 6 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 7 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 8 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 9 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 10 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 11 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 12 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 13 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 14 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 15 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 16 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 17 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 18 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 19 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 20 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 21 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 22 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 23 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 24 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 25 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 26 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 27 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 28 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 29 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 30 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 31 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 32 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 33 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 34 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 35 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 36 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 37 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 38 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 39 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 40 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 41 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 42 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 43 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 44 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 45 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 46 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 47 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 48 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 49 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 50 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 51 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 52 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 53 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 54 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 55 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 56 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 57 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 58 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 59 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 60 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 61 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 62 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 63 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 64 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 65 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 66 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 67 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 68 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 69 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 70 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 71 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 72 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 73 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 74 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 75 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 76 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 77 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 78 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 79 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 80 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 81 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 82 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 83 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 84 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 85 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 86 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 87 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 88 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 89 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 90 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 91 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 92 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 93 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 94 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 95 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 96 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 97 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 98 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 99 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 100 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 101 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 102 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 103 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 104 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 105 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 106 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 107 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 108 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 109 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 110 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 111 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 112 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 113 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 114 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 115 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 116 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 117 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 118 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 119 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 120 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 121 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 122 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 123 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 124 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 125 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 126 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 127 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 128 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 129 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 130 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 131 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 132 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 133 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 134 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 135 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 136 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 137 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 138 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 139 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 140 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 141 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 142 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
// 143 / 667 / 8c955267328b06de / d225734986c35be8 / 32:32 / 32,0,0,0 / True
/* AXIS row_ratio
BEFORE
static_cast<float>(rowSpan) / static_cast<float>(abs(columnDist))

AFTER implicit_left
rowSpan / static_cast<float>(abs(columnDist))

AFTER implicit_right
static_cast<float>(rowSpan) / abs(columnDist)
*/
/* AXIS column_ratio
BEFORE
static_cast<float>(columnDist) / static_cast<float>(abs(rowSpan))

AFTER implicit_left
columnDist / static_cast<float>(abs(rowSpan))

AFTER implicit_right
static_cast<float>(columnDist) / abs(rowSpan)
*/
/* AXIS column_divisor
BEFORE
columnStride /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS)
    ;

AFTER remove
columnStride /= COMBAT_WALL_TRACE_SUBDIVISIONS;
*/
/* AXIS row_divisor
BEFORE
rowStride /= static_cast<float>(COMBAT_WALL_TRACE_SUBDIVISIONS)

AFTER remove
rowStride /= COMBAT_WALL_TRACE_SUBDIVISIONS
*/
/* AXIS column_origin
BEFORE
static_cast<float>(sourceColumn)

AFTER remove
sourceColumn
*/
/* AXIS row_origin
BEFORE
static_cast<float>(srcLine)

AFTER remove
srcLine
*/

// c62-missile-numeric src/SOURCE/CMBTMGR.cpp RVA 0x2ad5a
// build/source-variant-batch/c62-missile-numeric/results.json
// Complete 24 arms; elapsed 19.976448s; input restored.
// Disposition: retain trial 15; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 1248 / 148e26f10d8d9a17 / 087fad77ddd0cdf9 / 56:56 / 56,0,0,0 / True
// 1 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 2 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 3 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 4 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 5 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 6 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 7 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 8 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 9 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 10 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 11 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 12 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 13 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 14 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 15 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 16 / 1246 / d72b64004616f5f9 / 38ce17063eee861d / 56:56 / 55,1,0,0 / True
// 17 / 1246 / d72b64004616f5f9 / 29775de55e577545 / 56:56 / 55,1,0,0 / True
// 18 / 1246 / d72b64004616f5f9 / 29775de55e577545 / 56:56 / 55,1,0,0 / True
// 19 / 1246 / d72b64004616f5f9 / 064f8a69dbd10675 / 56:56 / 55,1,0,0 / True
// 20 / 1246 / d72b64004616f5f9 / 29775de55e577545 / 56:56 / 55,1,0,0 / True
// 21 / 1246 / d72b64004616f5f9 / 064f8a69dbd10675 / 56:56 / 55,1,0,0 / True
// 22 / 1246 / d72b64004616f5f9 / 064f8a69dbd10675 / 56:56 / 55,1,0,0 / True
// 23 / 1246 / d72b64004616f5f9 / 03cecb4701077413 / 56:56 / 55,1,0,0 / True
/* AXIS slope_ratio
BEFORE
static_cast<double>(-slopeDy) / (static_cast<double>(absXLen))

AFTER implicit_left
-slopeDy / (static_cast<double>(absXLen))

AFTER implicit_right
static_cast<double>(-slopeDy) / absXLen
*/
/* AXIS degrees_owners
BEFORE
float degrees = static_cast<float>(
            atan(static_cast<double>(slope)) * COMBAT_MISSILE_DEGREES_PER_RADIAN
            / COMBAT_MISSILE_PI
        );

AFTER argument0_assignment1
float degrees = atan(static_cast<double>(slope)) * COMBAT_MISSILE_DEGREES_PER_RADIAN
            / COMBAT_MISSILE_PI;

AFTER argument1_assignment0
float degrees = static_cast<float>(
            atan(slope) * COMBAT_MISSILE_DEGREES_PER_RADIAN
            / COMBAT_MISSILE_PI
        );

AFTER argument1_assignment1
float degrees = atan(slope) * COMBAT_MISSILE_DEGREES_PER_RADIAN
            / COMBAT_MISSILE_PI;
*/
/* AXIS sqrt_argument
BEFORE
sqrt(static_cast<double>(xSize * xSize + yLen * yLen))

AFTER remove
sqrt(xSize * xSize + yLen * yLen)
*/

// c62-missile-coordinates src/SOURCE/CMBTMGR.cpp RVA 0x2ad5a
// build/source-variant-batch/c62-missile-coordinates/results.json
// Complete 256 arms; elapsed 175.365839s; input restored.
// Disposition: retain trial 255; final combined native proof, including private counter identity, is authoritative.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 1248 / 148e26f10d8d9a17 / 087fad77ddd0cdf9 / 56:56 / 56,0,0,0 / True
// 1 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 2 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 3 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 4 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 5 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 6 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 7 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 8 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 9 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 10 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 11 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 12 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 13 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 14 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 15 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 16 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 17 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 18 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 19 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 20 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 21 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 22 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 23 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 24 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 25 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 26 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 27 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 28 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 29 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 30 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 31 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 32 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 33 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 34 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 35 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 36 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 37 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 38 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 39 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 40 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 41 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 42 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 43 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 44 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 45 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 46 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 47 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 48 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 49 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 50 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 51 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 52 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 53 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 54 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 55 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 56 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 57 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 58 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 59 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 60 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 61 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 62 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 63 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 64 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 65 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 66 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 67 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 68 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 69 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 70 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 71 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 72 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 73 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 74 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 75 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 76 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 77 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 78 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 79 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 80 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 81 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 82 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 83 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 84 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 85 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 86 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 87 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 88 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 89 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 90 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 91 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 92 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 93 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 94 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 95 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 96 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 97 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 98 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 99 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 100 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 101 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 102 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 103 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 104 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 105 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 106 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 107 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 108 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 109 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 110 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 111 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 112 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 113 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 114 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 115 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 116 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 117 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 118 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 119 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 120 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 121 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 122 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 123 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 124 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 125 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 126 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 127 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 128 / 1248 / 148e26f10d8d9a17 / e448caeafcc9c977 / 56:56 / 56,0,0,0 / True
// 129 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 130 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 131 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 132 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 133 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 134 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 135 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 136 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 137 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 138 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 139 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 140 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 141 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 142 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 143 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 144 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 145 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 146 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 147 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 148 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 149 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 150 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 151 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 152 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 153 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 154 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 155 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 156 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 157 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 158 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 159 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 160 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 161 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 162 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 163 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 164 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 165 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 166 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 167 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 168 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 169 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 170 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 171 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 172 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 173 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 174 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 175 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 176 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 177 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 178 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 179 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 180 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 181 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 182 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 183 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 184 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 185 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 186 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 187 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 188 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 189 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 190 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 191 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 192 / 1248 / 148e26f10d8d9a17 / 0cf2aaaf7586cb41 / 56:56 / 56,0,0,0 / True
// 193 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 194 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 195 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 196 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 197 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 198 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 199 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 200 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 201 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 202 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 203 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 204 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 205 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 206 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 207 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 208 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 209 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 210 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 211 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 212 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 213 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 214 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 215 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 216 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 217 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 218 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 219 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 220 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 221 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 222 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 223 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 224 / 1248 / 148e26f10d8d9a17 / e07104e3fa8451b5 / 56:56 / 56,0,0,0 / True
// 225 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 226 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 227 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 228 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 229 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 230 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 231 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 232 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 233 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 234 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 235 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 236 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 237 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 238 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 239 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 240 / 1248 / 148e26f10d8d9a17 / 358801aa76286715 / 56:56 / 56,0,0,0 / True
// 241 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 242 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 243 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 244 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 245 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 246 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 247 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 248 / 1248 / 148e26f10d8d9a17 / 01a2dd7856f3810c / 56:56 / 56,0,0,0 / True
// 249 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 250 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 251 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 252 / 1248 / 148e26f10d8d9a17 / 59171cd5f0333f7b / 56:56 / 56,0,0,0 / True
// 253 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 254 / 1248 / 148e26f10d8d9a17 / d706b6d350ebc6a3 / 56:56 / 56,0,0,0 / True
// 255 / 1248 / 148e26f10d8d9a17 / f9ac731536ce7957 / 56:56 / 56,0,0,0 / True
/* AXIS initial_x
BEFORE

        static_cast<i16>(posX - missileHalfWidth),

AFTER remove

        posX - missileHalfWidth,
*/
/* AXIS initial_y
BEFORE

        static_cast<i16>(posY - missileHalfHeight)

AFTER remove

        posY - missileHalfHeight
*/
/* AXIS restore_x
BEFORE

                static_cast<i16>(oldX - missileHalfWidth),

AFTER remove

                oldX - missileHalfWidth,
*/
/* AXIS restore_y
BEFORE

                static_cast<i16>(oldY - missileHalfHeight)

AFTER remove

                oldY - missileHalfHeight
*/
/* AXIS next_x
BEFORE

                static_cast<i16>(posX - missileHalfWidth),

AFTER remove

                posX - missileHalfWidth,
*/
/* AXIS next_y
BEFORE

                static_cast<i16>(posY - missileHalfHeight)

AFTER remove

                posY - missileHalfHeight
*/
/* AXIS final_x
BEFORE

        static_cast<i16>(oldX - missileHalfWidth),

AFTER remove

        oldX - missileHalfWidth,
*/
/* AXIS final_y
BEFORE

        static_cast<i16>(oldY - missileHalfHeight)

AFTER remove

        oldY - missileHalfHeight
*/

// s58-group src/SOURCE/CMBTMGR.cpp RVA 0x271ad
// build/source-variant-batch/s58-group/results.json
// Complete 2 arms; elapsed 2.012285s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 705 / 209b5018c7005920 / e4cae2f1b856abd0 / 22:22 / 22,0,0,0 / True
// 1 / 705 / 24641b1af16b71d4 / e4cae2f1b856abd0 / 22:22 / 22,0,0,0 / True
/* AXIS unused_position
BEFORE
    i32 H2_UNUSED(pos);


AFTER remove

*/

// s58-next src/SOURCE/CMBTMGR.cpp RVA 0x287bf
// build/source-variant-batch/s58-next/results.json
// Complete 2 arms; elapsed 2.008567s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 837 / bbe72a22d0c68b66 / 090878c2b343268d / 52:52 / 52,0,0,0 / True
// 1 / 837 / f11d9b5737199a9d / 090878c2b343268d / 52:52 / 52,0,0,0 / True
/* AXIS unused_index
BEFORE
    i32 H2_UNUSED(i);


AFTER remove

*/

// s58-keep src/SOURCE/CMBTMGR.cpp RVA 0x29990
// build/source-variant-batch/s58-keep/results.json
// Complete 2 arms; elapsed 2.040106s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 1634 / 334e50c7ba26a500 / 909b80581f9b1db1 / 66:66 / 66,0,0,0 / True
// 1 / 1610 / 7b7b8adb3e215b95 / 7671117dfd85d44b / 66:66 / 66,0,0,0 / True
/* AXIS unused_tower_array
BEFORE
    i32 H2_UNUSED(unknownTowerData1)[KEEP_TOWER_SCRATCH_COUNT];


AFTER remove

*/

// s58-options src/SOURCE/CMBTMGR.cpp RVA 0x2b23a
// build/source-variant-batch/s58-options/results.json
// Complete 2 arms; elapsed 2.041163s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 268 / 9943b01a023de75b / 2d4d62ad411f87df / 8:8 / 8,0,0,0 / True
// 1 / 268 / 9e8bb92d0dc71c85 / f92931761063868b / 8:8 / 8,0,0,0 / True
/* AXIS unused_message
BEFORE
    tag_message H2_UNUSED(message);


AFTER remove

*/

// s58-grid src/SOURCE/CMBTMGR.cpp RVA 0x2746e
// build/source-variant-batch/s58-grid/results.json
// Complete 8 arms; elapsed 6.036383s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 580 / 598828086d7f1617 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 1 / 577 / 670cc8820c877734 / 0f281810c923a522 / 9:9 / 8,1,0,0 / True
// 2 / 580 / 6b41cdbe64b598e3 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 3 / 577 / e09b78b59a7410d7 / 0f281810c923a522 / 9:9 / 8,1,0,0 / True
// 4 / 580 / 6b41cdbe64b598e3 / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 5 / 577 / e09b78b59a7410d7 / 0f281810c923a522 / 9:9 / 8,1,0,0 / True
// 6 / 580 / 74dde705046e65cb / 0f281810c923a522 / 9:9 / 9,0,0,0 / True
// 7 / 577 / b043f592ebb505ba / 0f281810c923a522 / 9:9 / 8,1,0,0 / True
/* AXIS unused_owners
BEFORE
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(gridX4);
    i32 H2_UNUSED(randomOffset);
    i32 x;
    u32 y;
    i32 H2_UNUSED(coordinateY);

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid0_coordinate0_random_store1
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(gridX4);
    i32 x;
    u32 y;
    i32 H2_UNUSED(coordinateY);

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid0_coordinate1_random_store0
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(gridX4);
    i32 H2_UNUSED(randomOffset);
    i32 x;
    u32 y;

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid0_coordinate1_random_store1
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(gridX4);
    i32 x;
    u32 y;

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid1_coordinate0_random_store0
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(randomOffset);
    i32 x;
    u32 y;
    i32 H2_UNUSED(coordinateY);

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid1_coordinate0_random_store1
void combatManager::GenerateMap(void) {
    i32 x;
    u32 y;
    i32 H2_UNUSED(coordinateY);

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid1_coordinate1_random_store0
void combatManager::GenerateMap(void) {
    i32 H2_UNUSED(randomOffset);
    i32 x;
    u32 y;

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    randomOffset = SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}


AFTER grid1_coordinate1_random_store1
void combatManager::GenerateMap(void) {
    i32 x;
    u32 y;

    m_catapultFrame[IDX(COMBAT_ATTACKER_SIDE)] = m_inCastleCombat == 1 ? 0 : -1;

    for (y = 0; static_cast<i32>(y) < COMBAT_GRID_ROW_COUNT; y++) {
        for (x = 0; x < COMBAT_GRID_ROW_LENGTH; x++) {
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_y =
                static_cast<i16>((y + 1) * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_CENTER_Y_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_x = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP
                + ((y & 1) ? COMBAT_HEX_ROW_STAGGER : COMBAT_HEX_HORIZONTAL_STEP)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft = static_cast<i16>(
                (x - 1) * COMBAT_HEX_HORIZONTAL_STEP + ((y & 1) ? 0 : COMBAT_HEX_ROW_STAGGER)
                + COMBAT_HEX_GRID_LEFT_ORIGIN
            );
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop =
                static_cast<i16>(y * COMBAT_HEX_VERTICAL_STEP + COMBAT_HEX_GRID_TOP_ORIGIN);
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridRight =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridLeft + COMBAT_HEX_HORIZONTAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBodyBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_HEX_VERTICAL_STEP;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridBottom =
                m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_gridTop + COMBAT_MOUSE_HEX_HEIGHT;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantSide = COMBAT_SIDE_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_occupantFrame = ARMY_FACING_NONE;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_obstacleIndex = -1;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_blocked = 0;
            m_hexCells[y * COMBAT_GRID_ROW_LENGTH + x].m_deadOccupantCount = 0;
        }
    }
    SRandom(MAP_RANDOM_OFFSET_MINIMUM, MAP_RANDOM_OFFSET_MAXIMUM);
}

*/

// g72-grid src/SOURCE/CMBTMGR.cpp RVA 0x282fe
// build/source-variant-batch/g72-grid/results.json
// Complete 4 arms; elapsed 3.507281s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 401 / 70233efafea7e2f3 / d9d8109ade70055f / 36:36 / 36,0,0,0 / True
// 1 / 389 / ad630dee3123851f / 3aa3eed10596b4e5 / 34:36 / 2,0,19,13 / False
// 2 / 392 / 2da5b0e72de3129d / c64edda8e56f9c3f / 34:36 / 18,0,7,9 / False
// 3 / 395 / 3ec92ea326a1ae39 / d9d8109ade70055f / 36:36 / 15,1,11,9 / False
/* AXIS control_owner
BEFORE
i32 combatManager::GetGridIndex(i32 x, i32 y) {
    i32 xResidual;
    i32 yOffset;
    i32 mapX;
    i32 lineIndex;
    i32 cellsColumn;
    i32 diagonalDist;
    i32 mapY;

    mapX = x;
    mapY = y;
    mapY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    lineIndex = mapY / COMBAT_HEX_VERTICAL_STEP;
    mapX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(lineIndex & 1))
        mapX -= COMBAT_HEX_ROW_STAGGER;
    cellsColumn = mapX / COMBAT_HEX_HORIZONTAL_STEP;

    if (cellsColumn < 0)
        goto specialRegion;

    yOffset = mapY % COMBAT_HEX_VERTICAL_STEP;
    if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
        xResidual = mapX % COMBAT_HEX_HORIZONTAL_STEP;
        diagonalDist =
            abs(xResidual - COMBAT_HEX_ROW_STAGGER) / COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
        if (yOffset < diagonalDist) {
            lineIndex--;
            if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                if (!(lineIndex & 1))
                    cellsColumn--;
            } else if (lineIndex & 1) {
                cellsColumn++;
            }
        }
    }

    if (cellsColumn <= COMBAT_GRID_REVERSE_COLUMN_END || cellsColumn >= COMBAT_GRID_COLUMN_END
        || lineIndex >= COMBAT_GRID_ROW_COUNT || lineIndex < 0) {
    specialRegion:
        if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX && y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN
            && y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
            return COMBAT_GRID_LEFT_SPECIAL_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN && y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
            return COMBAT_GRID_RIGHT_UPPER_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN && y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX
            && m_inCastleCombat)
            return COMBAT_BALLISTA_HEX;
        return -1;
    }
    return lineIndex * COMBAT_GRID_ROW_LENGTH + cellsColumn;
}


AFTER guarded_geometry
i32 combatManager::GetGridIndex(i32 x, i32 y) {
    i32 xResidual;
    i32 yOffset;
    i32 mapX;
    i32 lineIndex;
    i32 cellsColumn;
    i32 diagonalDist;
    i32 mapY;

    mapX = x;
    mapY = y;
    mapY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    lineIndex = mapY / COMBAT_HEX_VERTICAL_STEP;
    mapX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(lineIndex & 1))
        mapX -= COMBAT_HEX_ROW_STAGGER;
    cellsColumn = mapX / COMBAT_HEX_HORIZONTAL_STEP;

    if (cellsColumn >= 0) {
        yOffset = mapY % COMBAT_HEX_VERTICAL_STEP;
        if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
            xResidual = mapX % COMBAT_HEX_HORIZONTAL_STEP;
            diagonalDist =
                abs(xResidual - COMBAT_HEX_ROW_STAGGER) / COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
            if (yOffset < diagonalDist) {
                lineIndex--;
                if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                    if (!(lineIndex & 1))
                        cellsColumn--;
                } else if (lineIndex & 1) {
                    cellsColumn++;
                }
            }
        }
    }

    if (cellsColumn <= COMBAT_GRID_REVERSE_COLUMN_END || cellsColumn >= COMBAT_GRID_COLUMN_END
        || lineIndex >= COMBAT_GRID_ROW_COUNT || lineIndex < 0) {
        if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX && y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN
            && y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
            return COMBAT_GRID_LEFT_SPECIAL_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN && y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
            return COMBAT_GRID_RIGHT_UPPER_HEX;
        if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
            && y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN && y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX
            && m_inCastleCombat)
            return COMBAT_BALLISTA_HEX;
        return -1;
    }
    return lineIndex * COMBAT_GRID_ROW_LENGTH + cellsColumn;
}


AFTER early_grid_return
i32 combatManager::GetGridIndex(i32 x, i32 y) {
    i32 xResidual;
    i32 yOffset;
    i32 mapX;
    i32 lineIndex;
    i32 cellsColumn;
    i32 diagonalDist;
    i32 mapY;

    mapX = x;
    mapY = y;
    mapY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    lineIndex = mapY / COMBAT_HEX_VERTICAL_STEP;
    mapX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(lineIndex & 1))
        mapX -= COMBAT_HEX_ROW_STAGGER;
    cellsColumn = mapX / COMBAT_HEX_HORIZONTAL_STEP;

    if (cellsColumn >= 0) {
        yOffset = mapY % COMBAT_HEX_VERTICAL_STEP;
        if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
            xResidual = mapX % COMBAT_HEX_HORIZONTAL_STEP;
            diagonalDist =
                abs(xResidual - COMBAT_HEX_ROW_STAGGER) / COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
            if (yOffset < diagonalDist) {
                lineIndex--;
                if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                    if (!(lineIndex & 1))
                        cellsColumn--;
                } else if (lineIndex & 1) {
                    cellsColumn++;
                }
            }
        }

        if (cellsColumn > COMBAT_GRID_REVERSE_COLUMN_END && cellsColumn < COMBAT_GRID_COLUMN_END
            && lineIndex < COMBAT_GRID_ROW_COUNT && lineIndex >= 0)
            return lineIndex * COMBAT_GRID_ROW_LENGTH + cellsColumn;
    }
    if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX && y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN
        && y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
        return COMBAT_GRID_LEFT_SPECIAL_HEX;
    if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
        && y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN && y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
        return COMBAT_GRID_RIGHT_UPPER_HEX;
    if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
        && y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN && y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX
        && m_inCastleCombat)
        return COMBAT_BALLISTA_HEX;
    return -1;
}


AFTER single_pass_scope
i32 combatManager::GetGridIndex(i32 x, i32 y) {
    i32 xResidual;
    i32 yOffset;
    i32 mapX;
    i32 lineIndex;
    i32 cellsColumn;
    i32 diagonalDist;
    i32 mapY;

    mapX = x;
    mapY = y;
    mapY -= COMBAT_HEX_GRID_TOP_ORIGIN;
    lineIndex = mapY / COMBAT_HEX_VERTICAL_STEP;
    mapX -= COMBAT_GRID_INDEX_X_ORIGIN;
    if (!(lineIndex & 1))
        mapX -= COMBAT_HEX_ROW_STAGGER;
    cellsColumn = mapX / COMBAT_HEX_HORIZONTAL_STEP;

    do {
        if (cellsColumn < 0)
            break;

        yOffset = mapY % COMBAT_HEX_VERTICAL_STEP;
        if (yOffset < COMBAT_GRID_DIAGONAL_HEIGHT) {
            xResidual = mapX % COMBAT_HEX_HORIZONTAL_STEP;
            diagonalDist =
                abs(xResidual - COMBAT_HEX_ROW_STAGGER) / COMBAT_GRID_DIAGONAL_SLOPE_DIVISOR;
            if (yOffset < diagonalDist) {
                lineIndex--;
                if (xResidual < COMBAT_HEX_ROW_STAGGER) {
                    if (!(lineIndex & 1))
                        cellsColumn--;
                } else if (lineIndex & 1) {
                    cellsColumn++;
                }
            }
        }

        if (cellsColumn <= COMBAT_GRID_REVERSE_COLUMN_END || cellsColumn >= COMBAT_GRID_COLUMN_END
            || lineIndex >= COMBAT_GRID_ROW_COUNT || lineIndex < 0)
            break;
        return lineIndex * COMBAT_GRID_ROW_LENGTH + cellsColumn;
    } while (0);
    if (x >= 0 && x <= COMBAT_GRID_LEFT_SPECIAL_X_MAX && y >= COMBAT_GRID_LEFT_SPECIAL_Y_MIN
        && y <= COMBAT_GRID_LEFT_SPECIAL_Y_MAX)
        return COMBAT_GRID_LEFT_SPECIAL_HEX;
    if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
        && y >= COMBAT_GRID_RIGHT_UPPER_Y_MIN && y <= COMBAT_GRID_RIGHT_UPPER_Y_MAX)
        return COMBAT_GRID_RIGHT_UPPER_HEX;
    if (x >= COMBAT_GRID_RIGHT_SPECIAL_X_MIN && x <= COMBAT_MAX_EXTENT_X
        && y >= COMBAT_GRID_RIGHT_LOWER_Y_MIN && y <= COMBAT_GRID_RIGHT_LOWER_Y_MAX
        && m_inCastleCombat)
        return COMBAT_BALLISTA_HEX;
    return -1;
}

*/

// g73-next src/SOURCE/CMBTMGR.cpp RVA 0x287bf
// build/source-variant-batch/g73-next/results.json
// Complete 3 arms; elapsed 2.662152s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 837 / bbe72a22d0c68b66 / 090878c2b343268d / 52:52 / 52,0,0,0 / True
// 1 / 838 / 8aefeb80cf9dcb5a / 19e2e82b57a30ab8 / 53:52 / 39,0,5,8 / False
// 2 / 842 / 65e4489b616bbbb6 / bda949b25b66256e / 52:52 / 50,0,1,1 / False
/* AXIS control_owner
BEFORE
i32 combatManager::GetNextArmy(i32 checkMorale) {
    b32 skipEnt;
    i32 speedIter;
    b32 hasPending;
    i32 sideLoop;
    i32 stackCounter;
    army* curArmy;
    CombatSide stackSide;
    i32 H2_UNUSED(i);

restart:
    hasPending = false;
    stackSide = m_currentArmySide;
    m_currentSpeed = COMBAT_MAX_SPEED;
    for (speedIter = 0; speedIter < COMBAT_SPEED_LEVEL_COUNT; speedIter++) {
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            stackSide ^= 1;
            for (stackCounter = 0; stackCounter < m_armyCount[IDX(stackSide)]; stackCounter++) {
                skipEnt = false;
                curArmy = stackCounter + m_armies[IDX(stackSide)];
                if (HAS(curArmy->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_AI_EXCLUDED | MONSTER_ABILITY_FLAG_BAD_MORALE)
                    || IDX(curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)])
                    || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                    || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                    || (curArmy->m_monster.speed != m_currentSpeed
                        && !(
                            curArmy->m_monster.flags.abilityFlags
                            & MONSTER_ABILITY_FLAG_HIGH_MORALE
                        )))
                    skipEnt = true;

                if (!skipEnt && speedIter == 0
                    && !(
                        curArmy->m_monster.flags.abilityFlags & MONSTER_ABILITY_FLAG_HIGH_MORALE
                    ))
                    skipEnt = true;

                if HAS (curArmy->m_monster.flags.abilityFlags,
                        MONSTER_ABILITY_FLAG_DEFERRED_TURN) {
                    skipEnt = true;
                    hasPending = true;
                }

                if (!skipEnt && checkMorale && CheckApplyBadMorale(stackSide, stackCounter))
                    skipEnt = true;
                if (!skipEnt)
                    break;
            }

            if (stackCounter != m_armyCount[IDX(stackSide)]) {
                m_currentArmySide = stackSide;
                m_currentArmyIndex = stackCounter;
                if ((stackCounter + m_armies[IDX(stackSide)])
                        ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
                    m_currentSide = OppositeCombatSide(stackSide);
                else
                    m_currentSide = stackSide;
                GetControl();
                return 1;
            }
        }

        if (speedIter) {
            m_currentSpeed--;
            if (m_currentSpeed == 0)
                m_currentSpeed = COMBAT_SPEED_LEVEL_COUNT;
        }
    }

    if (hasPending) {
        checkMorale = 0;
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            for (stackCounter = 0; stackCounter < m_armyCount[sideLoop]; stackCounter++) {
                (m_armies[sideLoop] + stackCounter)->m_monster.flags.abilityFlags &=
                    ~MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            }
        }
        goto restart;
    }

    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    return 0;
}


AFTER retry_loop
i32 combatManager::GetNextArmy(i32 checkMorale) {
    b32 skipEnt;
    i32 speedIter;
    b32 hasPending;
    i32 sideLoop;
    i32 stackCounter;
    army* curArmy;
    CombatSide stackSide;
    i32 H2_UNUSED(i);

    for (;;) {
        hasPending = false;
        stackSide = m_currentArmySide;
        m_currentSpeed = COMBAT_MAX_SPEED;
        for (speedIter = 0; speedIter < COMBAT_SPEED_LEVEL_COUNT; speedIter++) {
            for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
                stackSide ^= 1;
                for (stackCounter = 0; stackCounter < m_armyCount[IDX(stackSide)]; stackCounter++) {
                    skipEnt = false;
                    curArmy = stackCounter + m_armies[IDX(stackSide)];
                    if (HAS(curArmy->m_monster.flags.abilityFlags,
                            MONSTER_ABILITY_FLAG_AI_EXCLUDED | MONSTER_ABILITY_FLAG_BAD_MORALE)
                        || IDX(curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)])
                        || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                        || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                        || (curArmy->m_monster.speed != m_currentSpeed
                            && !(
                                curArmy->m_monster.flags.abilityFlags
                                & MONSTER_ABILITY_FLAG_HIGH_MORALE
                            )))
                        skipEnt = true;

                    if (!skipEnt && speedIter == 0
                        && !(
                            curArmy->m_monster.flags.abilityFlags & MONSTER_ABILITY_FLAG_HIGH_MORALE
                        ))
                        skipEnt = true;

                    if HAS (curArmy->m_monster.flags.abilityFlags,
                            MONSTER_ABILITY_FLAG_DEFERRED_TURN) {
                        skipEnt = true;
                        hasPending = true;
                    }

                    if (!skipEnt && checkMorale && CheckApplyBadMorale(stackSide, stackCounter))
                        skipEnt = true;
                    if (!skipEnt)
                        break;
                }

                if (stackCounter != m_armyCount[IDX(stackSide)]) {
                    m_currentArmySide = stackSide;
                    m_currentArmyIndex = stackCounter;
                    if ((stackCounter + m_armies[IDX(stackSide)])
                            ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
                        m_currentSide = OppositeCombatSide(stackSide);
                    else
                        m_currentSide = stackSide;
                    GetControl();
                    return 1;
                }
            }

            if (speedIter) {
                m_currentSpeed--;
                if (m_currentSpeed == 0)
                    m_currentSpeed = COMBAT_SPEED_LEVEL_COUNT;
            }
        }

        if (!hasPending)
            break;
        checkMorale = 0;
        for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
            for (stackCounter = 0; stackCounter < m_armyCount[sideLoop]; stackCounter++) {
                (m_armies[sideLoop] + stackCounter)->m_monster.flags.abilityFlags &=
                    ~MONSTER_ABILITY_FLAG_DEFERRED_TURN;
            }
        }
    }

    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    return 0;
}


AFTER pending_do_while
i32 combatManager::GetNextArmy(i32 checkMorale) {
    b32 skipEnt;
    i32 speedIter;
    b32 hasPending;
    i32 sideLoop;
    i32 stackCounter;
    army* curArmy;
    CombatSide stackSide;
    i32 H2_UNUSED(i);

    do {
        hasPending = false;
        stackSide = m_currentArmySide;
        m_currentSpeed = COMBAT_MAX_SPEED;
        for (speedIter = 0; speedIter < COMBAT_SPEED_LEVEL_COUNT; speedIter++) {
            for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
                stackSide ^= 1;
                for (stackCounter = 0; stackCounter < m_armyCount[IDX(stackSide)]; stackCounter++) {
                    skipEnt = false;
                    curArmy = stackCounter + m_armies[IDX(stackSide)];
                    if (HAS(curArmy->m_monster.flags.abilityFlags,
                            MONSTER_ABILITY_FLAG_AI_EXCLUDED | MONSTER_ABILITY_FLAG_BAD_MORALE)
                        || IDX(curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PARALYZE)])
                        || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_PETRIFIED)]
                        || curArmy->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_BLIND)]
                        || (curArmy->m_monster.speed != m_currentSpeed
                            && !(
                                curArmy->m_monster.flags.abilityFlags
                                & MONSTER_ABILITY_FLAG_HIGH_MORALE
                            )))
                        skipEnt = true;

                    if (!skipEnt && speedIter == 0
                        && !(
                            curArmy->m_monster.flags.abilityFlags & MONSTER_ABILITY_FLAG_HIGH_MORALE
                        ))
                        skipEnt = true;

                    if HAS (curArmy->m_monster.flags.abilityFlags,
                            MONSTER_ABILITY_FLAG_DEFERRED_TURN) {
                        skipEnt = true;
                        hasPending = true;
                    }

                    if (!skipEnt && checkMorale && CheckApplyBadMorale(stackSide, stackCounter))
                        skipEnt = true;
                    if (!skipEnt)
                        break;
                }

                if (stackCounter != m_armyCount[IDX(stackSide)]) {
                    m_currentArmySide = stackSide;
                    m_currentArmyIndex = stackCounter;
                    if ((stackCounter + m_armies[IDX(stackSide)])
                            ->m_spellInfluence[IDX(ARMY_SPELL_INFLUENCE_HYPNOTIZE)])
                        m_currentSide = OppositeCombatSide(stackSide);
                    else
                        m_currentSide = stackSide;
                    GetControl();
                    return 1;
                }
            }

            if (speedIter) {
                m_currentSpeed--;
                if (m_currentSpeed == 0)
                    m_currentSpeed = COMBAT_SPEED_LEVEL_COUNT;
            }
        }

        if (hasPending) {
            checkMorale = 0;
            for (sideLoop = 0; sideLoop < COMBAT_SIDE_COUNT; sideLoop++) {
                for (stackCounter = 0; stackCounter < m_armyCount[sideLoop]; stackCounter++) {
                    (m_armies[sideLoop] + stackCounter)->m_monster.flags.abilityFlags &=
                        ~MONSTER_ABILITY_FLAG_DEFERRED_TURN;
                }
            }
        }
    } while (hasPending);

    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    CheckCastleAttack();
    m_currentSide = OppositeCombatSide(m_currentSide);
    return 0;
}

*/

// g74-cat src/SOURCE/CMBTMGR.cpp RVA 0x28bb8
// build/source-variant-batch/g74-cat/results.json
// Complete 2 arms; elapsed 2.019626s; input restored.
// Disposition: original retained; all tested alternatives change native bytes. Other structural parents/states remain untested.
// Per-arm trial / size / text hash / normalized relocation hash / candidate:retail blocks / exact,size-only,target-shift,flow-kind / graph exact
// 0 / 3544 / fc7a30abefa1501b / 68403f63ae6cd9b1 / 168:168 / 168,0,0,0 / True
// 1 / 3576 / 613ee3616393db2e / aca6ef9e2197195f / 169:168 / 86,14,23,45 / False
/* AXIS control_owner
BEFORE
void combatManager::CatAttack(H2_ENUM_PARAM(CombatSide, i32) side) {
    if (!m_inCastleCombat)
        return;

    LogStr("CA1");
    if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
        DrawSmallView(IDX(COMBAT_DEFENDER_SIDE), 1);
    }

    i32 random2 = SRandom(COMBAT_CATAPULT_TARGET_ROLL_MIN, COMBAT_CATAPULT_TARGET_ROLL_MAX);
    i32 wallCount14 = 0;
    i32 towerCount8 = 0;
    i32 wallIndex = -1;
    CombatCastleWallSlot towerIndex6 = COMBAT_WALL_SLOT_NONE;
    i32 gateIndex11 = -1;
    i32 keepIndex13 = -1;
    CombatCastleHex targetHex4 = COMBAT_CASTLE_HEX_NONE;
    b32 missShot19 = false;
    CombatCatapultDamage damageLevel15 = CATAPULT_DAMAGE_NORMAL;
    i32 firstRoll7;
    i32 advancedRoll;
    i32 index26;

    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;

    if (wallCount14 != 0) {
        random2 = random2 % wallCount14 + 1;
        wallIndex = -1;
        while (random2 != 0) {
            wallIndex++;
            if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED)
                random2--;
        }
    } else if (towerCount8 != 0) {
        random2 %= towerCount8;
        if (random2 == 1
            || m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED)
            towerIndex6 = COMBAT_WALL_SLOT_BOTTOM_TOWER;
        else
            towerIndex6 = COMBAT_WALL_SLOT_TOP_TOWER;
    } else if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN) {
        gateIndex11 = 1;
    } else {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING)
            keepIndex13 = 0;
    }

    if (towerIndex6 == COMBAT_WALL_SLOT_NONE && wallIndex == -1 && gateIndex11 == -1
        && keepIndex13 == -1)
        return;

    sample* loadedSample18 = NULL;
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    loadedSample18 = gpResourceManager->GetSample(gText);
    SAMPLE2 catapultSound = NULL;
    SAMPLE2 impactSound19 = NULL;
    icon* boulder3 = gpResourceManager->GetIcon("boulder.icn");
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_LAUNCH_SOUND);
    catapultSound = LoadPlaySample(gText);

    i32 impactX10 = -1;
    i32 impactY6 = -1;
    if (wallIndex != -1) {
        impactX10 = wallPos[wallIndex][IDX(COORDINATE_AXIS_X)];
        impactY6 = wallPos[wallIndex][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iWallToHexCell[wallIndex]);
    }
    if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
        impactX10 = towerPos[IDX(towerIndex6)][IDX(COORDINATE_AXIS_X)];
        impactY6 = towerPos[IDX(towerIndex6)][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iTowerToHexCell[IDX(towerIndex6)]);
    }
    if (gateIndex11 != -1) {
        impactX10 = doorPos[0][IDX(COORDINATE_AXIS_X)];
        impactY6 = doorPos[0][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = COMBAT_CASTLE_HEX_GATE;
    }
    if (keepIndex13 != -1) {
        impactX10 = COMBAT_CATAPULT_KEEP_IMPACT_X;
        impactY6 = COMBAT_CATAPULT_KEEP_IMPACT_Y;
        targetHex4 = COMBAT_CASTLE_HEX_KEEP;
    }

    i32 startX8;
    float bounceY7;
    i32 frame;

    if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
        == HERO_SKILL_LEVEL_NONE) {
        firstRoll7 =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            firstRoll7 -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (firstRoll7 < COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
        else if (firstRoll7 > COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD) {
            missShot19 = true;
            damageLevel15 = CATAPULT_DAMAGE_NONE;
        }
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               <= HERO_SKILL_LEVEL_ADVANCED) {
        advancedRoll =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            advancedRoll -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (advancedRoll < COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               == HERO_SKILL_LEVEL_EXPERT) {
        damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    }

    if (missShot19) {
        u8 validMissHexes[COMBAT_CATAPULT_MISS_HEX_COUNT] =
            {32, 46, 10, 23, 35, 48, 60, 74, 87, 101, 114};
        i32 startDirection3 =
            SRandom(COMBAT_CATAPULT_DIRECTION_ROLL_MIN, COMBAT_CATAPULT_DIRECTION_ROLL_MAX);
        i32 adjacentHex;
        for (frame = 0; frame < COMBAT_CATAPULT_DIRECTION_COUNT; frame++) {
            adjacentHex = GetAdjacentCellIndexNoArmy(
                IDX(targetHex4),
                static_cast<CombatHexDirection>(
                    (startDirection3 + frame) % COMBAT_CATAPULT_DIRECTION_COUNT
                )
            );
            if (adjacentHex != -1 && m_hexCells[adjacentHex].m_occupantSide == COMBAT_SIDE_NONE) {
                for (index26 = 0; index26 < COMBAT_CATAPULT_MISS_HEX_COUNT; index26++) {
                    if (adjacentHex == validMissHexes[index26]) {
                        impactX10 = m_hexCells[adjacentHex].m_x;
                        impactY6 = m_hexCells[adjacentHex].m_y - COMBAT_CATAPULT_MISS_Y_OFFSET;
                        goto foundMissHex;
                    }
                }
            }
        }
        missShot19 = false;
        damageLevel15 = CATAPULT_DAMAGE_NORMAL;
    foundMissHex:
        frame++;
    }

    giMinExtentX = 0;
    giMaxExtentX = COMBAT_CATAPULT_INITIAL_MAX_X;
    giMinExtentY = COMBAT_CATAPULT_INITIAL_MIN_Y;
    giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
    m_catapultFrame[IDX(side)] = 0;
    while (m_catapultFrame[IDX(side)] < COMBAT_CATAPULT_LOAD_FRAME_COUNT) {
        DrawFrame(1, 0, 1, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
        m_catapultFrame[IDX(side)]++;
    }

    startX8 = COMBAT_CATAPULT_START_X;
    i32 startY9 = COMBAT_CATAPULT_START_Y;
    i32 spriteFrame16 = 0;
    float projectileX11 = static_cast<float>(startX8);
    float projectileY4 = static_cast<float>(startY9);
    float midpointX6 = static_cast<float>(
        (startX8 + impactX10) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
    );
    bounceY7 = static_cast<float>(
        (impactY6 - (impactX10 - startX8) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY6 * COMBAT_CATAPULT_TARGET_ARC_SCALE
    );
    float xStep0 =
        static_cast<float>((midpointX6 - startX8) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR);
    float yStep = (bounceY7 - startY9) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
    i32 previousX7 = -1;
    i32 previousY4 = -1;
    SLimitData limits9;
    for (frame = 0; frame < COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT; frame++) {
        if (frame == COMBAT_CATAPULT_DESCENT_FRAME)
            yStep = (bounceY7 - impactY6) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
        if (frame != 0) {
            giMinExtentX = previousX7 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            giMaxExtentX =
                static_cast<i32l>(
                    projectileX11
                    + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)

                );
            giMinExtentY =
                static_cast<i32l>(
                    projectileY4 - IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            giMaxExtentY =
                static_cast<i32l>(
                    projectileY4 + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            if (previousY4 < projectileY4)
                giMinExtentY = previousY4 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            else
                giMaxExtentY = previousY4 + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            if (frame <= COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME) {
                giMinExtentX = 0;
                giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
            }
            if (giMinExtentX < 0)
                giMinExtentX = 0;
            if (giMinExtentY < 0)
                giMinExtentY = 0;
            if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
                giMaxExtentX = COMBAT_MAX_EXTENT_X;
            if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                giMaxExtentY = COMBAT_MAX_EXTENT_Y;
            if (giMaxExtentY < COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y)
                giMaxExtentY = COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y;
        }
        DrawFrame(0, 0, 1, 0, COMBAT_CATAPULT_PROJECTILE_DELAY, 1, 1);
        boulder3->CombatClipDrawToBuffer(
            static_cast<i32l>(projectileX11),
            static_cast<i32l>(projectileY4),
            spriteFrame16,
            &limits9,
            ICON_DRAW_NORMAL,
            0,
            NULL,
            NULL
        );
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        previousX7 = static_cast<i32l>(projectileX11);
        previousY4 = static_cast<i32l>(projectileY4);
        projectileX11 += xStep0;
        projectileY4 = (COMBAT_CATAPULT_DESCENT_FRAME - frame) * yStep + projectileY4;
        spriteFrame16++;
        spriteFrame16 %= COMBAT_CATAPULT_BOULDER_FRAME_COUNT;
        if (frame < COMBAT_CATAPULT_ADVANCE_FRAME_COUNT)
            m_catapultFrame[IDX(side)]++;
    }

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    impactSound19 = LoadPlaySample(gText);
    m_unknownF34B = 0;
    m_unknownF34F = 0;

    giMinExtentX = impactX10 - COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMaxExtentX = impactX10 + COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMinExtentY = impactY6 - COMBAT_CATAPULT_IMPACT_EXTENT_TOP;
    giMaxExtentY = impactY6 + COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM;
    if (gateIndex11 != -1)
        giMinExtentX -= COMBAT_CATAPULT_IMPACT_EXTENT_X;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
        giMaxExtentX = COMBAT_MAX_EXTENT_X;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    icon* cloud =
        gpResourceManager->GetIcon(const_cast<char*>(missShot19 ? "smalclod.icn" : "lichclod.icn"));

    for (frame = 0; frame < COMBAT_CATAPULT_CLOUD_FRAME_COUNT; frame++) {
        if (frame >= COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT
            && (gateIndex11 != -1 || missShot19 != 0))
            continue;
        glTimers[COMBAT_CATAPULT_TIMER_SLOT] = static_cast<i32l>(
            KBTickCount()
            + IDX(COMBAT_CATAPULT_ANIMATION_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        DrawFrame(0, 0, 1, 0, 0, 1, 0);
        if (gateIndex11 != -1 && frame >= COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME && !missShot19) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX10 - COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET,
                impactY6 + COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET,
                frame - COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        if (frame < COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX10,
                impactY6 + (!missShot19 ? COMBAT_CATAPULT_CLOUD_Y_OFFSET : 0),
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DelayTil(&glTimers[COMBAT_CATAPULT_TIMER_SLOT]);

        if (((frame == COMBAT_CATAPULT_WALL_IMPACT_FRAME && gateIndex11 == -1)
             || (frame == COMBAT_CATAPULT_GATE_IMPACT_FRAME && gateIndex11 != -1))
            && !missShot19) {
            if (wallIndex != -1) {
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        >= COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST
                    && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           <= COMBAT_WALL_STATE_SECTION_DAMAGE_LAST) {
                    m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] +=
                        IDX(damageLevel15);
                    if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        > COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_SECTION_DESTROYED;
                } else {
                    if (damageLevel15 > CATAPULT_DAMAGE_NORMAL)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_DESTROYED;
                    else
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]++;
                }
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        == COMBAT_WALL_STATE_DESTROYED
                    || m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           == COMBAT_WALL_STATE_SECTION_DESTROYED)
                    m_hexCells[iWallToHexCell[wallIndex]].m_blocked = 0;
            } else if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
                m_wallStates[IDX(towerIndex6)] = COMBAT_WALL_STATE_DESTROYED;
            } else if (gateIndex11 != -1) {
                m_drawbridgeState = COMBAT_CASTLE_GATE_HIDDEN;
            } else if (keepIndex13 != -1) {
                m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_DESTROYED;
            }
        }
    }

    gpResourceManager->Dispose(cloud);
    m_catapultFrame[IDX(side)] = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
    gpResourceManager->Dispose(boulder3);
    WaitEndSample(&impactSound19, -1);
    WaitEndSample(&catapultSound, -1);
    if (loadedSample18)
        gpResourceManager->Dispose(loadedSample18);
    LogStr("CA2");
}


AFTER found_miss_loop
void combatManager::CatAttack(H2_ENUM_PARAM(CombatSide, i32) side) {
    if (!m_inCastleCombat)
        return;

    LogStr("CA1");
    if (m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] != COMBAT_SIDE_NONE
        || m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] != COMBAT_SIDE_NONE) {
        m_smallViewSide[IDX(COMBAT_ATTACKER_SIDE)] = m_smallViewSide[IDX(COMBAT_DEFENDER_SIDE)] = COMBAT_SIDE_NONE;
        DrawSmallView(IDX(COMBAT_ATTACKER_SIDE), 1);
        DrawSmallView(IDX(COMBAT_DEFENDER_SIDE), 1);
    }

    i32 random2 = SRandom(COMBAT_CATAPULT_TARGET_ROLL_MIN, COMBAT_CATAPULT_TARGET_ROLL_MAX);
    i32 wallCount14 = 0;
    i32 towerCount8 = 0;
    i32 wallIndex = -1;
    CombatCastleWallSlot towerIndex6 = COMBAT_WALL_SLOT_NONE;
    i32 gateIndex11 = -1;
    i32 keepIndex13 = -1;
    CombatCastleHex targetHex4 = COMBAT_CASTLE_HEX_NONE;
    b32 missShot19 = false;
    CombatCatapultDamage damageLevel15 = CATAPULT_DAMAGE_NORMAL;
    i32 firstRoll7;
    i32 advancedRoll;
    i32 index26;

    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_SECOND)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_THIRD)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)] != COMBAT_WALL_STATE_DESTROYED
        && m_wallStates[IDX(COMBAT_WALL_SLOT_SECTION_FOURTH)]
               != COMBAT_WALL_STATE_SECTION_DESTROYED)
        wallCount14++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;
    if (m_wallStates[IDX(COMBAT_WALL_SLOT_BOTTOM_TOWER)] == COMBAT_WALL_STATE_TOWER_STANDING)
        towerCount8++;

    if (wallCount14 != 0) {
        random2 = random2 % wallCount14 + 1;
        wallIndex = -1;
        while (random2 != 0) {
            wallIndex++;
            if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                    != COMBAT_WALL_STATE_DESTROYED
                && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                       != COMBAT_WALL_STATE_SECTION_DESTROYED)
                random2--;
        }
    } else if (towerCount8 != 0) {
        random2 %= towerCount8;
        if (random2 == 1
            || m_wallStates[IDX(COMBAT_WALL_SLOT_TOP_TOWER)] == COMBAT_WALL_STATE_DESTROYED)
            towerIndex6 = COMBAT_WALL_SLOT_BOTTOM_TOWER;
        else
            towerIndex6 = COMBAT_WALL_SLOT_TOP_TOWER;
    } else if (m_drawbridgeState != COMBAT_CASTLE_GATE_HIDDEN) {
        gateIndex11 = 1;
    } else {
        if (m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] == COMBAT_WALL_STATE_KEEP_STANDING)
            keepIndex13 = 0;
    }

    if (towerIndex6 == COMBAT_WALL_SLOT_NONE && wallIndex == -1 && gateIndex11 == -1
        && keepIndex13 == -1)
        return;

    sample* loadedSample18 = NULL;
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    loadedSample18 = gpResourceManager->GetSample(gText);
    SAMPLE2 catapultSound = NULL;
    SAMPLE2 impactSound19 = NULL;
    icon* boulder3 = gpResourceManager->GetIcon("boulder.icn");
    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_LAUNCH_SOUND);
    catapultSound = LoadPlaySample(gText);

    i32 impactX10 = -1;
    i32 impactY6 = -1;
    if (wallIndex != -1) {
        impactX10 = wallPos[wallIndex][IDX(COORDINATE_AXIS_X)];
        impactY6 = wallPos[wallIndex][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iWallToHexCell[wallIndex]);
    }
    if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
        impactX10 = towerPos[IDX(towerIndex6)][IDX(COORDINATE_AXIS_X)];
        impactY6 = towerPos[IDX(towerIndex6)][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = CombatCastleHex(iTowerToHexCell[IDX(towerIndex6)]);
    }
    if (gateIndex11 != -1) {
        impactX10 = doorPos[0][IDX(COORDINATE_AXIS_X)];
        impactY6 = doorPos[0][IDX(COORDINATE_AXIS_Y)];
        targetHex4 = COMBAT_CASTLE_HEX_GATE;
    }
    if (keepIndex13 != -1) {
        impactX10 = COMBAT_CATAPULT_KEEP_IMPACT_X;
        impactY6 = COMBAT_CATAPULT_KEEP_IMPACT_Y;
        targetHex4 = COMBAT_CASTLE_HEX_KEEP;
    }

    i32 startX8;
    float bounceY7;
    i32 frame;

    if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
        == HERO_SKILL_LEVEL_NONE) {
        firstRoll7 =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            firstRoll7 -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (firstRoll7 < COMBAT_CATAPULT_NO_SKILL_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
        else if (firstRoll7 > COMBAT_CATAPULT_NO_SKILL_MISS_THRESHOLD) {
            missShot19 = true;
            damageLevel15 = CATAPULT_DAMAGE_NONE;
        }
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               <= HERO_SKILL_LEVEL_ADVANCED) {
        advancedRoll =
            SRandom(COMBAT_CATAPULT_BALLISTICS_ROLL_MIN, COMBAT_CATAPULT_BALLISTICS_ROLL_MAX);
        if (!gbHumanPlayer[IDX(m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_owner)])
            advancedRoll -= COMBAT_CATAPULT_AI_ROLL_PENALTY;
        if (advancedRoll < COMBAT_CATAPULT_ADVANCED_DOUBLE_THRESHOLD)
            damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    } else if (m_heroes[IDX(COMBAT_ATTACKER_SIDE)]->m_secondarySkills[IDX(HERO_SKILL_BALLISTICS)]
               == HERO_SKILL_LEVEL_EXPERT) {
        damageLevel15 = CATAPULT_DAMAGE_DOUBLE;
    }

    if (missShot19) {
        u8 validMissHexes[COMBAT_CATAPULT_MISS_HEX_COUNT] =
            {32, 46, 10, 23, 35, 48, 60, 74, 87, 101, 114};
        i32 startDirection3 =
            SRandom(COMBAT_CATAPULT_DIRECTION_ROLL_MIN, COMBAT_CATAPULT_DIRECTION_ROLL_MAX);
        i32 adjacentHex;
        b32 foundMiss = false;
        for (frame = 0; frame < COMBAT_CATAPULT_DIRECTION_COUNT && !foundMiss; frame++) {
            adjacentHex = GetAdjacentCellIndexNoArmy(
                IDX(targetHex4),
                static_cast<CombatHexDirection>(
                    (startDirection3 + frame) % COMBAT_CATAPULT_DIRECTION_COUNT
                )
            );
            if (adjacentHex != -1 && m_hexCells[adjacentHex].m_occupantSide == COMBAT_SIDE_NONE) {
                for (index26 = 0; index26 < COMBAT_CATAPULT_MISS_HEX_COUNT; index26++) {
                    if (adjacentHex == validMissHexes[index26]) {
                        impactX10 = m_hexCells[adjacentHex].m_x;
                        impactY6 = m_hexCells[adjacentHex].m_y - COMBAT_CATAPULT_MISS_Y_OFFSET;
                        foundMiss = true;
                        break;
                    }
                }
            }
        }
        if (!foundMiss) {
            missShot19 = false;
            damageLevel15 = CATAPULT_DAMAGE_NORMAL;
        }
        frame++;
    }

    giMinExtentX = 0;
    giMaxExtentX = COMBAT_CATAPULT_INITIAL_MAX_X;
    giMinExtentY = COMBAT_CATAPULT_INITIAL_MIN_Y;
    giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
    m_catapultFrame[IDX(side)] = 0;
    while (m_catapultFrame[IDX(side)] < COMBAT_CATAPULT_LOAD_FRAME_COUNT) {
        DrawFrame(1, 0, 1, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
        m_catapultFrame[IDX(side)]++;
    }

    startX8 = COMBAT_CATAPULT_START_X;
    i32 startY9 = COMBAT_CATAPULT_START_Y;
    i32 spriteFrame16 = 0;
    float projectileX11 = static_cast<float>(startX8);
    float projectileY4 = static_cast<float>(startY9);
    float midpointX6 = static_cast<float>(
        (startX8 + impactX10) / COMBAT_CATAPULT_MIDPOINT_DIVISOR
    );
    bounceY7 = static_cast<float>(
        (impactY6 - (impactX10 - startX8) * COMBAT_CATAPULT_DISTANCE_ARC_SCALE)
        - impactY6 * COMBAT_CATAPULT_TARGET_ARC_SCALE
    );
    float xStep0 =
        static_cast<float>((midpointX6 - startX8) / COMBAT_CATAPULT_HORIZONTAL_STEP_DIVISOR);
    float yStep = (bounceY7 - startY9) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
    i32 previousX7 = -1;
    i32 previousY4 = -1;
    SLimitData limits9;
    for (frame = 0; frame < COMBAT_CATAPULT_PROJECTILE_FRAME_COUNT; frame++) {
        if (frame == COMBAT_CATAPULT_DESCENT_FRAME)
            yStep = (bounceY7 - impactY6) / COMBAT_CATAPULT_VERTICAL_STEP_DIVISOR;
        if (frame != 0) {
            giMinExtentX = previousX7 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            giMaxExtentX =
                static_cast<i32l>(
                    projectileX11
                    + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)

                );
            giMinExtentY =
                static_cast<i32l>(
                    projectileY4 - IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            giMaxExtentY =
                static_cast<i32l>(
                    projectileY4 + IDX(COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS)
                );
            if (previousY4 < projectileY4)
                giMinExtentY = previousY4 - COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            else
                giMaxExtentY = previousY4 + COMBAT_CATAPULT_PROJECTILE_EXTENT_RADIUS;
            if (frame <= COMBAT_CATAPULT_EARLY_EXTENT_LAST_FRAME) {
                giMinExtentX = 0;
                giMaxExtentY = COMBAT_CATAPULT_INITIAL_MAX_Y;
            }
            if (giMinExtentX < 0)
                giMinExtentX = 0;
            if (giMinExtentY < 0)
                giMinExtentY = 0;
            if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
                giMaxExtentX = COMBAT_MAX_EXTENT_X;
            if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
                giMaxExtentY = COMBAT_MAX_EXTENT_Y;
            if (giMaxExtentY < COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y)
                giMaxExtentY = COMBAT_CATAPULT_MIN_VISIBLE_EXTENT_Y;
        }
        DrawFrame(0, 0, 1, 0, COMBAT_CATAPULT_PROJECTILE_DELAY, 1, 1);
        boulder3->CombatClipDrawToBuffer(
            static_cast<i32l>(projectileX11),
            static_cast<i32l>(projectileY4),
            spriteFrame16,
            &limits9,
            ICON_DRAW_NORMAL,
            0,
            NULL,
            NULL
        );
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        previousX7 = static_cast<i32l>(projectileX11);
        previousY4 = static_cast<i32l>(projectileY4);
        projectileX11 += xStep0;
        projectileY4 = (COMBAT_CATAPULT_DESCENT_FRAME - frame) * yStep + projectileY4;
        spriteFrame16++;
        spriteFrame16 %= COMBAT_CATAPULT_BOULDER_FRAME_COUNT;
        if (frame < COMBAT_CATAPULT_ADVANCE_FRAME_COUNT)
            m_catapultFrame[IDX(side)]++;
    }

    sprintf(gText, "catsnd%02d.82M", COMBAT_CATAPULT_IMPACT_SOUND);
    impactSound19 = LoadPlaySample(gText);
    m_unknownF34B = 0;
    m_unknownF34F = 0;

    giMinExtentX = impactX10 - COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMaxExtentX = impactX10 + COMBAT_CATAPULT_IMPACT_EXTENT_X;
    giMinExtentY = impactY6 - COMBAT_CATAPULT_IMPACT_EXTENT_TOP;
    giMaxExtentY = impactY6 + COMBAT_CATAPULT_IMPACT_EXTENT_BOTTOM;
    if (gateIndex11 != -1)
        giMinExtentX -= COMBAT_CATAPULT_IMPACT_EXTENT_X;
    if (giMinExtentX < 0)
        giMinExtentX = 0;
    if (giMaxExtentX > COMBAT_MAX_EXTENT_X)
        giMaxExtentX = COMBAT_MAX_EXTENT_X;
    if (giMinExtentY < 0)
        giMinExtentY = 0;
    if (giMaxExtentY > COMBAT_MAX_EXTENT_Y)
        giMaxExtentY = COMBAT_MAX_EXTENT_Y;

    icon* cloud =
        gpResourceManager->GetIcon(const_cast<char*>(missShot19 ? "smalclod.icn" : "lichclod.icn"));

    for (frame = 0; frame < COMBAT_CATAPULT_CLOUD_FRAME_COUNT; frame++) {
        if (frame >= COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT
            && (gateIndex11 != -1 || missShot19 != 0))
            continue;
        glTimers[COMBAT_CATAPULT_TIMER_SLOT] = static_cast<i32l>(
            KBTickCount()
            + IDX(COMBAT_CATAPULT_ANIMATION_DELAY) * gfCombatSpeedMod[gConfig.combatSpeed]
        );
        DrawFrame(0, 0, 1, 0, 0, 1, 0);
        if (gateIndex11 != -1 && frame >= COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME && !missShot19) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX10 - COMBAT_CATAPULT_GATE_CLOUD_X_OFFSET,
                impactY6 + COMBAT_CATAPULT_GATE_CLOUD_Y_OFFSET,
                frame - COMBAT_CATAPULT_GATE_CLOUD_FIRST_FRAME,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        if (frame < COMBAT_CATAPULT_CLOUD_VISIBLE_FRAME_COUNT) {
            IconToBitmap(
                cloud,
                gpWindowManager->m_screen,
                impactX10,
                impactY6 + (!missShot19 ? COMBAT_CATAPULT_CLOUD_Y_OFFSET : 0),
                frame,
                ICON_DRAW_CLIP,
                0,
                0,
                COMBAT_SCREEN_WIDTH,
                COMBAT_AREA_HEIGHT,
                0
            );
        }
        gpWindowManager->UpdateScreenRegion(
            giMinExtentX,
            giMinExtentY,
            giMaxExtentX - giMinExtentX + 1,
            giMaxExtentY - giMinExtentY + 1
        );
        DelayTil(&glTimers[COMBAT_CATAPULT_TIMER_SLOT]);

        if (((frame == COMBAT_CATAPULT_WALL_IMPACT_FRAME && gateIndex11 == -1)
             || (frame == COMBAT_CATAPULT_GATE_IMPACT_FRAME && gateIndex11 != -1))
            && !missShot19) {
            if (wallIndex != -1) {
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        >= COMBAT_WALL_STATE_SECTION_DAMAGE_FIRST
                    && m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           <= COMBAT_WALL_STATE_SECTION_DAMAGE_LAST) {
                    m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] +=
                        IDX(damageLevel15);
                    if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        > COMBAT_WALL_STATE_SECTION_DAMAGE_LAST)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_SECTION_DESTROYED;
                } else {
                    if (damageLevel15 > CATAPULT_DAMAGE_NORMAL)
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)] =
                            COMBAT_WALL_STATE_DESTROYED;
                    else
                        m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]++;
                }
                if (m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                        == COMBAT_WALL_STATE_DESTROYED
                    || m_wallStates[wallIndex + IDX(COMBAT_WALL_SLOT_SECTION_FIRST)]
                           == COMBAT_WALL_STATE_SECTION_DESTROYED)
                    m_hexCells[iWallToHexCell[wallIndex]].m_blocked = 0;
            } else if (towerIndex6 != COMBAT_WALL_SLOT_NONE) {
                m_wallStates[IDX(towerIndex6)] = COMBAT_WALL_STATE_DESTROYED;
            } else if (gateIndex11 != -1) {
                m_drawbridgeState = COMBAT_CASTLE_GATE_HIDDEN;
            } else if (keepIndex13 != -1) {
                m_wallStates[IDX(COMBAT_WALL_SLOT_KEEP)] = COMBAT_WALL_STATE_KEEP_DESTROYED;
            }
        }
    }

    gpResourceManager->Dispose(cloud);
    m_catapultFrame[IDX(side)] = 0;
    DrawFrame(1, 0, 0, 0, COMBAT_CATAPULT_ANIMATION_DELAY, 1, 1);
    gpResourceManager->Dispose(boulder3);
    WaitEndSample(&impactSound19, -1);
    WaitEndSample(&catapultSound, -1);
    if (loadedSample18)
        gpResourceManager->Dispose(loadedSample18);
    LogStr("CA2");
}

*/
