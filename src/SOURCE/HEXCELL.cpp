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
    obstacleIndex = -1;
    field_0xe = 0;
    occupantSide = -1;
    occupantIndex = 0;
    occupantFrame = -1;
    deadOccupantCount = 0;
}

VA(0x0044a406, 0x16)
void hexcell::DrawGround(void) {}

VA(0x0044a41c, 0xc6)
void hexcell::DrawLowerDeadOccupants(void)
{
    int currentFrame;
    int i;
    army *occupant;

    if (deadOccupantCount > 0) {
        for (i = 0; i < deadOccupantCount - 1; ++i) {
            occupant = &gpCombatManager->armies[deadOccupantSides[i]][deadOccupantIndices[i]];
            currentFrame = occupant->field_0x86;
            if (deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(field_0x0, field_0x2, 0);
        }
    }
}

// @early-stop
// logic and frame slots byte-exact; only the two array-subscript operands load in reverse order
VA(0x0044a4e2, 0xc8)
void hexcell::DrawUpperDeadOccupant(void)
{
    int currentFrame;
    int i;
    army *occupant;

    if (deadOccupantCount > 0) {
        for (i = deadOccupantCount - 1; i < deadOccupantCount; ++i) {
            occupant = &gpCombatManager->armies[deadOccupantSides[i]][deadOccupantIndices[i]];
            currentFrame = occupant->field_0x86;
            if (deadOccupantFrames[i] != currentFrame)
                occupant->DrawToBuffer(field_0x0, field_0x2, 0);
        }
    }
}

VA(0x0044a5aa, 0x165)
void hexcell::DrawOccupant(int creature, int frame)
{
    if (occupantSide != -1) {
        if (creature != 100) {
            if (gpCombatManager->armies[occupantSide][occupantIndex].field_0x6 != creature)
                return;
        }
        if (gbLimitToExtent && occupantSide == gpCombatManager->currentArmySide
            && occupantIndex == gpCombatManager->currentArmyIndex)
            gbCurrArmyDrawn = 1;
        if (gpCombatManager->armies[occupantSide][occupantIndex].field_0x86 != occupantFrame)
            gpCombatManager->armies[occupantSide][occupantIndex].DrawToBuffer(
                field_0x0, field_0x2, frame);
    }
}

VA(0x0044a70f, 0x149)
void hexcell::DrawTower(int frame)
{
    int level = 0;
    int row;
    int drawX;
    int rightX;
    int temp;

    if (level)
        drawX = field_0x0;
    else
        drawX = field_0x0 + 28;
    gpCombatManager->towerIcon->CombatClipDrawToBuffer(drawX, field_0x2, frame, &limits, 1, 0, 0, 0);

    row = (field_0x2 - 139) / 42;
    if (row == 4)
        return;
    if (row & 1) {
        if (level)
            rightX = field_0x0;
        else
            rightX = field_0x0 + 28;
        gpCombatManager->towerIcon->CombatClipDrawToBuffer(rightX, field_0x2, 9, &limits, 1, 0, 0, 0);
    } else {
        if (level)
            temp = field_0x0 - 28;
        else
            temp = field_0x0;
        gpCombatManager->towerIcon->CombatClipDrawToBuffer(temp, field_0x2, 9, &limits, 0, 0, 0, 0);
    }
}

VA(0x0044a858, 0x16)
void hexcell::DrawClouds(void) {}

VA(0x0044a86e, 0x4f)
void hexcell::DrawObstacle(void)
{
    gpCombatManager->obstacleIcons[obstacleIndex]->CombatClipDrawToBuffer(
        field_0x0, field_0x2, 0, &limits, 0, 0, 0, 0);
}
