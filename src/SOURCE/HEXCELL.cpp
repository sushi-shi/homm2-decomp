#include <Ints.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hexcell.h>

typedef enum TowerDrawConstant {
    TOWER_X_OFFSET      = 28,
    TOWER_ROW_Y_ORIGIN  = 139,
    TOWER_EXCLUDED_ROW  = 4,
    TOWER_OVERLAY_FRAME = 9
} TowerDrawConstant;

hexcell::hexcell(void) {
    m_obstacleIndex = -1;
    m_blocked = 0;
    m_occupantSide = COMBAT_SIDE_NONE;
    m_occupantIndex = 0;
    m_occupantFrame = ARMY_FACING_NONE;
    m_deadOccupantCount = 0;
}

void hexcell::DrawGround(void) {
    return;
}

void hexcell::DrawLowerDeadOccupants(void) {
    ArmyFacing currentFrame;
    i32 i;
    army* occupant;

    if (m_deadOccupantCount > 0) {
        for (i = 0; i < m_deadOccupantCount - 1; ++i) {
            occupant =
                &gpCombatManager->m_armies[H2EnumIndex(m_deadOccupantSides[i])][m_deadOccupantIndices[i]];
            currentFrame = occupant->m_facing;
            if (m_deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(m_x, m_y, 0);
        }
    }
}

void hexcell::DrawUpperDeadOccupant(void) {
    ArmyFacing currentFrame;
    i32 i;
    army* occupant;

    if (m_deadOccupantCount > 0) {
        for (i = m_deadOccupantCount - 1; i < m_deadOccupantCount; ++i) {
            occupant =
                &gpCombatManager->m_armies[H2EnumIndex(m_deadOccupantSides[i])][m_deadOccupantIndices[i]];
            currentFrame = occupant->m_facing;
            if (m_deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(m_x, m_y, 0);
        }
    }
}

void hexcell::DrawOccupant(ArmyDrawState drawState, i32 frame) {
    if (m_occupantSide != COMBAT_SIDE_NONE) {
        if (drawState != ARMY_DRAW_ALL) {
            if (gpCombatManager->m_armies[H2EnumIndex(m_occupantSide)][m_occupantIndex].m_drawState
                != drawState)
                return;
        }
        if (gbLimitToExtent && gpCombatManager->m_currentArmySide == m_occupantSide
            && gpCombatManager->m_currentArmyIndex == m_occupantIndex)
            gbCurrArmyDrawn = true;
        if (m_occupantFrame
            != gpCombatManager->m_armies[H2EnumIndex(m_occupantSide)][m_occupantIndex].m_facing)
            gpCombatManager->m_armies[H2EnumIndex(m_occupantSide)][m_occupantIndex]
                .DrawToBuffer(m_x, m_y, frame);
    }
}

void hexcell::DrawTower(i32 frame) {
    i32 level = 0;
    i32 row;

    gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]
        ->CombatClipDrawToBuffer(
            level ? m_x : m_x + TOWER_X_OFFSET,
            m_y,
            frame,
            m_limits,
            ICON_DRAW_FLIPPED,
            0,
            NULL,
            NULL
        );

    row = (m_y - TOWER_ROW_Y_ORIGIN) / COMBAT_HEX_VERTICAL_STEP;
    if (row == TOWER_EXCLUDED_ROW)
        return;
    if (row & 1) {
        gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]
            ->CombatClipDrawToBuffer(
                level ? m_x : m_x + TOWER_X_OFFSET,
                m_y,
                TOWER_OVERLAY_FRAME,
                m_limits,
                ICON_DRAW_FLIPPED,
                0,
                NULL,
                NULL
            );
    } else {
        gpCombatManager->m_combatIcons[H2EnumIndex(COMBAT_ICON_TOWER)]
            ->CombatClipDrawToBuffer(
                level ? m_x - TOWER_X_OFFSET : m_x,
                m_y,
                TOWER_OVERLAY_FRAME,
                m_limits,
                ICON_DRAW_NORMAL,
                0,
                NULL,
                NULL
            );
    }
}

void hexcell::DrawClouds(void) {
    return;
}

void hexcell::DrawObstacle(void) {
    gpCombatManager->m_obstacleIcons[m_obstacleIndex]
        ->CombatClipDrawToBuffer(m_x, m_y, 0, m_limits, ICON_DRAW_NORMAL, 0, NULL, NULL);
}
