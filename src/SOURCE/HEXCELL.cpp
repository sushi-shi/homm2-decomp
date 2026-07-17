// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HEXCELL.OBJ   from: (directly linked into exe)
// functions: 8   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/army.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/hexcell.h>

VA(0x0044a3c0, 0x46)
hexcell::hexcell(void)
{
    m_obstacleIndex = -1;
    m_blocked = 0;
    m_occupantSide = -1;
    m_occupantIndex = 0;
    m_occupantFrame = -1;
    m_deadOccupantCount = 0;
}

VA(0x0044a406, 0x16)
void hexcell::DrawGround(void)
{
    return;
}

VA(0x0044a41c, 0xc6)
void hexcell::DrawLowerDeadOccupants(void)
{
    i32 currentFrame;
    i32 i;
    army *occupant;

    if (m_deadOccupantCount > 0) {
        for (i = 0; i < m_deadOccupantCount - 1; ++i) {
            occupant = &gpCombatManager->m_armies[m_deadOccupantSides[i]][m_deadOccupantIndices[i]];
            currentFrame = occupant->m_facing;
            if (m_deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(m_x, m_y, 0);
        }
    }
}

// @early-stop
// Complete 0x10 frame, loop/CFG, and ordered relocations. At +0x40 retail scales
// dead-occupant side before index; candidate evaluates the equivalent m_armies
// subscripts in reverse order. Flattening the two-dimensional index regressed the
// live score; direct and commuted subscript spellings were otherwise unchanged.
VA(0x0044a4e2, 0xc8)
void hexcell::DrawUpperDeadOccupant(void)
{
    i32 currentFrame;
    i32 i;
    army *occupant;

    if (m_deadOccupantCount > 0) {
        for (i = m_deadOccupantCount - 1; i < m_deadOccupantCount; ++i) {
            occupant = &gpCombatManager->m_armies[m_deadOccupantSides[i]][m_deadOccupantIndices[i]];
            currentFrame = occupant->m_facing;
            if (m_deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(m_x, m_y, 0);
        }
    }
}

VA(0x0044a5aa, 0x165)
void hexcell::DrawOccupant(i32 creature, i32 frame)
{
    if (m_occupantSide != -1) {
        if (creature != 100) {
            if (gpCombatManager->m_armies[m_occupantSide][m_occupantIndex].m_drawState != creature)
                return;
        }
        if (gbLimitToExtent && m_occupantSide == gpCombatManager->m_currentArmySide
            && m_occupantIndex == gpCombatManager->m_currentArmyIndex)
            gbCurrArmyDrawn = 1;
        if (gpCombatManager->m_armies[m_occupantSide][m_occupantIndex].m_facing != m_occupantFrame)
            gpCombatManager->m_armies[m_occupantSide][m_occupantIndex].DrawToBuffer(
                m_x, m_y, frame);
    }
}

VA(0x0044a70f, 0x149)
void hexcell::DrawTower(i32 frame)
{
    i32 level = 0;
    i32 row;
    i32 drawX;
    i32 rightX;
    i32 temp;

    if (level)
        drawX = m_x;
    else
        drawX = m_x + 28;
    gpCombatManager->m_combatIcons[COMBAT_ICON_TOWER]->CombatClipDrawToBuffer(drawX, m_y, frame, m_limits, 1, 0, 0, 0);

    row = (m_y - 139) / 42;
    if (row == 4)
        return;
    if (row & 1) {
        if (level)
            rightX = m_x;
        else
            rightX = m_x + 28;
        gpCombatManager->m_combatIcons[COMBAT_ICON_TOWER]->CombatClipDrawToBuffer(rightX, m_y, 9, m_limits, 1, 0, 0, 0);
    } else {
        if (level)
            temp = m_x - 28;
        else
            temp = m_x;
        gpCombatManager->m_combatIcons[COMBAT_ICON_TOWER]->CombatClipDrawToBuffer(temp, m_y, 9, m_limits, 0, 0, 0, 0);
    }
}

VA(0x0044a858, 0x16)
void hexcell::DrawClouds(void)
{
    return;
}

VA(0x0044a86e, 0x4f)
void hexcell::DrawObstacle(void)
{
    gpCombatManager->m_obstacleIcons[m_obstacleIndex]->CombatClipDrawToBuffer(
        m_x, m_y, 0, m_limits, 0, 0, 0, 0);
}
