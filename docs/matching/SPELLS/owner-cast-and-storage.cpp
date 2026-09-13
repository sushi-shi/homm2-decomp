// VC6 SP5 measured source families for O05/C58/S56/O06.
// Exact-span axes are independent; no generated state probes were retained.
// This records tested source, not compilable replacement TU input.
// Final combined native proof: SPELLS40 and SPELLAI14 functions plus all allocated sections unchanged.
// See docs/reconstruction/O05-C58-S56-B75.md for complete retail proof limits.

// o05-fireball src/SOURCE/SPELLS.cpp RVA 0x9b2bd
// build/source-variant-batch/o05-fireball/results.json
// Complete 4 arms; elapsed 4.769844s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS owner_0
BEFORE
*(
                    gArmyEffected[0]
                    + IDX(m_hexCells[affectedHexes[frame]].m_occupantSide)
                          * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[affectedHexes[frame]].m_occupantIndex
                )

AFTER index
gArmyEffected[IDX(m_hexCells[affectedHexes[frame]].m_occupantSide)][m_hexCells[affectedHexes[frame]].m_occupantIndex]
*/
/* AXIS owner_1
BEFORE
*(gArmyEffected[0]
                  + IDX(m_hexCells[affectedHexes[frame]].m_occupantSide)
                        * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[affectedHexes[frame]].m_occupantIndex)

AFTER index
gArmyEffected[IDX(m_hexCells[affectedHexes[frame]].m_occupantSide)][m_hexCells[affectedHexes[frame]].m_occupantIndex]
*/

// o05-meteor src/SOURCE/SPELLS.cpp RVA 0x9b805
// build/source-variant-batch/o05-meteor/results.json
// Complete 4 arms; elapsed 3.758782s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS owner_0
BEFORE
*(
                    gArmyEffected[0]
                    + IDX(m_hexCells[hexes[direction]].m_occupantSide) * COMBAT_ARMY_SLOT_COUNT
                    + m_hexCells[hexes[direction]].m_occupantIndex
                )

AFTER index
gArmyEffected[IDX(m_hexCells[hexes[direction]].m_occupantSide)][m_hexCells[hexes[direction]].m_occupantIndex]
*/
/* AXIS owner_1
BEFORE
*(gArmyEffected[0]
                  + IDX(m_hexCells[hexes[direction]].m_occupantSide) * COMBAT_ARMY_SLOT_COUNT
                  + m_hexCells[hexes[direction]].m_occupantIndex)

AFTER index
gArmyEffected[IDX(m_hexCells[hexes[direction]].m_occupantSide)][m_hexCells[hexes[direction]].m_occupantIndex]
*/

// o05-clear src/SOURCE/SPELLAI.cpp RVA 0x97724
// build/source-variant-batch/o05-clear/results.json
// Complete 2 arms; elapsed 1.604165s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS owner_0
BEFORE
*(gArmyEffected[0] + IDX(side) * COMBAT_ARMY_SLOT_COUNT + idx)

AFTER index
gArmyEffected[IDX(side)][idx]
*/

// c58-reset src/SOURCE/SPELLS.cpp RVA 0x9cc2f
// build/source-variant-batch/c58-reset/results.json
// Complete 16 arms; elapsed 12.655421s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sqrt(static_cast<double>(deltaX5 * deltaX5 + deltaY3 * deltaY3))

AFTER remove
sqrt(deltaX5 * deltaX5 + deltaY3 * deltaY3)
*/
/* AXIS conversion_1
BEFORE
static_cast<double>(angleX7)

AFTER remove
angleX7
*/
/* AXIS conversion_2
BEFORE
static_cast<double>(angleY9)

AFTER remove
angleY9
*/
/* AXIS conversion_3
BEFORE
static_cast<float>((bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR)

AFTER remove
(bolt->minAngle + bolt->maxAngle) / BOLT_ANGLE_AVERAGE_DIVISOR
*/

// c58-draw src/SOURCE/SPELLS.cpp RVA 0x9ce89
// build/source-variant-batch/c58-draw/results.json
// Complete 16 arms; elapsed 12.511066s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sin(static_cast<double>(bolt->baseAngle))

AFTER remove
sin(bolt->baseAngle)
*/
/* AXIS conversion_1
BEFORE
cos(static_cast<double>(bolt->baseAngle))

AFTER remove
cos(bolt->baseAngle)
*/
/* AXIS conversion_2
BEFORE
static_cast<float>(COMBAT_SCREEN_WIDTH - 1)

AFTER remove
COMBAT_SCREEN_WIDTH - 1
*/
/* AXIS conversion_3
BEFORE
static_cast<float>(COMBAT_AREA_HEIGHT - 1)

AFTER remove
COMBAT_AREA_HEIGHT - 1
*/

// c58-add src/SOURCE/SPELLS.cpp RVA 0x9d2fe
// build/source-variant-batch/c58-add/results.json
// Complete 8 arms; elapsed 6.529312s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
static_cast<float>(startX)

AFTER remove
startX
*/
/* AXIS conversion_1
BEFORE
static_cast<float>(startY)

AFTER remove
startY
*/
/* AXIS conversion_2
BEFORE
sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY))

AFTER remove
sqrt(deltaX * deltaX + deltaY * deltaY)
*/

// c58-do src/SOURCE/SPELLS.cpp RVA 0x9d510
// build/source-variant-batch/c58-do/results.json
// Complete 4 arms; elapsed 3.812084s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sin(static_cast<double>(currentAngle16))

AFTER remove
sin(currentAngle16)
*/
/* AXIS conversion_1
BEFORE
cos(static_cast<double>(currentAngle16))

AFTER remove
cos(currentAngle16)
*/

// c58-next src/SOURCE/SPELLS.cpp RVA 0x9de32
// build/source-variant-batch/c58-next/results.json
// Complete 2 arms; elapsed 2.373249s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sqrt(static_cast<double>(xDelta * xDelta + y * y))

AFTER remove
sqrt(xDelta * xDelta + y * y)
*/

// c58-chain src/SOURCE/SPELLS.cpp RVA 0x9df96
// build/source-variant-batch/c58-chain/results.json
// Complete 2 arms; elapsed 2.239644s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sqrt(static_cast<double>(deltaX3 * deltaX3 + deltaY5 * deltaY5))

AFTER remove
sqrt(deltaX3 * deltaX3 + deltaY5 * deltaY5)
*/

// c58-blast src/SOURCE/SPELLS.cpp RVA 0xa0178
// build/source-variant-batch/c58-blast/results.json
// Complete 16 arms; elapsed 19.68388s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sqrt(static_cast<double>(deltaX_a * deltaX_a + deltaY_g * deltaY_g))

AFTER remove
sqrt(deltaX_a * deltaX_a + deltaY_g * deltaY_g)
*/
/* AXIS conversion_1
BEFORE
static_cast<float>(startX_n)

AFTER remove
startX_n
*/
/* AXIS conversion_2
BEFORE
static_cast<float>(startY_d)

AFTER remove
startY_d
*/
/* AXIS conversion_3
BEFORE
#define BLAST_FRAME_DELAY static_cast<float>(10.0)

AFTER remove
#define BLAST_FRAME_DELAY 10.0f
*/

// c58-ripple src/SOURCE/SPELLS.cpp RVA 0x9e51a
// build/source-variant-batch/c58-ripple/results.json
// Complete 2 arms; elapsed 2.244883s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
sin(static_cast<double>(
                 static_cast<float>(row_i % RIPPLE_WAVE_PERIOD)
                 / static_cast<float>(RIPPLE_WAVE_DIVISOR)
             ))

AFTER remove
sin(
                 static_cast<float>(row_i % RIPPLE_WAVE_PERIOD)
                 / static_cast<float>(RIPPLE_WAVE_DIVISOR)
             )
*/

// c58-summon src/SOURCE/SPELLS.cpp RVA 0x9fe0a
// build/source-variant-batch/c58-summon/results.json
// Complete 2 arms; elapsed 2.246062s; restored input.
// Disposition: all remove/index choices retained after combined full native proof.
/* AXIS conversion_0
BEFORE
SUMMON_HEX_STORAGE_COUNT = 8

AFTER remove
SUMMON_HEX_STORAGE_COUNT = 6
*/

// o06-vapor src/SOURCE/SPELLS.cpp RVA 0x9e2b3
// build/source-variant-batch/o06-vapor/results.json
// Complete 3 arms; elapsed 3.007197s; restored input.
// Disposition: original retained, both index alternatives nonexact; recovery remains OPEN.
/* AXIS row_owner
BEFORE
*(gyModify - row9 * VAPORIZE_STRIPE_WIDTH - bottomOffset + lastY)

AFTER signed_index
gyModify[-row9 * VAPORIZE_STRIPE_WIDTH - bottomOffset + lastY]

AFTER last_row_index
gyModify[lastY - row9 * VAPORIZE_STRIPE_WIDTH - bottomOffset]
*/
