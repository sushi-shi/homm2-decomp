#include <match.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hexcell.h>

H2_ENUM_BEGIN(TowerDrawConstant)
    TOWER_X_OFFSET      = 28,
    TOWER_ROW_Y_ORIGIN  = 139,
    TOWER_EXCLUDED_ROW  = 4,
    TOWER_OVERLAY_FRAME = 9
H2_ENUM_END(TowerDrawConstant)

VA(0x00464cc0, 0x3b)
hexcell::hexcell(void) {
    m_obstacleIndex = -1;
    m_blocked = 0;
    m_occupantSide = COMBAT_SIDE_NONE;
    m_occupantIndex = 0;
    m_occupantFootprintHalf = ARMY_FACING_NONE;
    m_deadOccupantCount = 0;
}

VA(0x00464cfb, 0xb)
void hexcell::DrawGround(void) {
    return;
}

#if H2_RETAIL_COMPILER
#define occupantFacing currentFrame
#endif
VA(0x00464d06, 0xa1)
void hexcell::DrawLowerDeadOccupants(void) {
    ArmyFacing occupantFacing;
    i32 i;
    army* occupant;

    if (m_deadOccupantCount > 0) {
        for (i = 0; i < m_deadOccupantCount - 1; ++i) {
            occupant =
                &gpCombatManager->m_armies[IDX(m_deadOccupantSides[i])][m_deadOccupantIndices[i]];
            occupantFacing = occupant->m_facing;
            if (m_deadOccupantFootprintHalves[i] != occupantFacing)
                occupant->DrawToBuffer(m_x, m_y, false);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef occupantFacing
#endif

#if H2_RETAIL_COMPILER
#define occupantFacing currentFrame
#endif
VA(0x00464da7, 0xa4)
void hexcell::DrawUpperDeadOccupant(void) {
    ArmyFacing occupantFacing;
    i32 i;
    army* occupant;

    if (m_deadOccupantCount > 0) {
        for (i = m_deadOccupantCount - 1; i < m_deadOccupantCount; ++i) {
            occupant =
                &gpCombatManager->m_armies[IDX(m_deadOccupantSides[i])][m_deadOccupantIndices[i]];
            occupantFacing = occupant->m_facing;
            if (m_deadOccupantFootprintHalves[i] != occupantFacing)
                occupant->DrawToBuffer(m_x, m_y, false);
        }
    }
}
#if H2_RETAIL_COMPILER
#undef occupantFacing
#endif

#if H2_RETAIL_COMPILER
#define quantityOverlayOnly frame
#endif
VA(0x00464e4b, 0x10d)
void hexcell::DrawOccupant(ArmyDrawState drawState, b32 quantityOverlayOnly) {
    if (m_occupantSide != COMBAT_SIDE_NONE) {
        if (drawState != ARMY_DRAW_ALL) {
            if (gpCombatManager->m_armies[IDX(m_occupantSide)][m_occupantIndex].m_drawState
                != drawState)
                return;
        }
        if (gbLimitToExtent && gpCombatManager->m_currentArmySide == m_occupantSide
            && gpCombatManager->m_currentArmyIndex == m_occupantIndex)
            gbCurrArmyDrawn = true;
        if (m_occupantFootprintHalf
            != gpCombatManager->m_armies[IDX(m_occupantSide)][m_occupantIndex].m_facing)
            gpCombatManager->m_armies[IDX(m_occupantSide)][m_occupantIndex]
                .DrawToBuffer(m_x, m_y, quantityOverlayOnly);
    }
}
#if H2_RETAIL_COMPILER
#undef quantityOverlayOnly
#endif

VA(0x00464f58, 0x126)
void hexcell::DrawTower(i32 frame) {
    i32 level = 0;
    i32 row;

    gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
        level ? m_x : m_x + TOWER_X_OFFSET,
        m_y,
        frame,
        m_limits,
        ICON_DRAW_FLIPPED
    );

    row = (m_y - TOWER_ROW_Y_ORIGIN) / COMBAT_HEX_VERTICAL_STEP;
    if (row == TOWER_EXCLUDED_ROW)
        return;
    if (row & 1) {
        gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
            level ? m_x : m_x + TOWER_X_OFFSET,
            m_y,
            TOWER_OVERLAY_FRAME,
            m_limits,
            ICON_DRAW_FLIPPED
        );
    } else {
        gpCombatManager->m_combatIcons[IDX(COMBAT_ICON_TOWER)]->CombatClipDrawToBuffer(
            level ? m_x - TOWER_X_OFFSET : m_x,
            m_y,
            TOWER_OVERLAY_FRAME,
            m_limits,
            ICON_DRAW_NORMAL
        );
    }
}

VA(0x0046507e, 0xb)
void hexcell::DrawClouds(void) {
    return;
}

VA(0x00465089, 0x44)
void hexcell::DrawObstacle(void) {
    gpCombatManager->m_obstacleIcons[m_obstacleIndex]
        ->CombatClipDrawToBuffer(m_x, m_y, 0, m_limits, ICON_DRAW_NORMAL);
}
