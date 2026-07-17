// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SWAPMGR.OBJ   from: (directly linked into exe)
// functions: 13   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <stdio.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/swapManager.h>
#include <SOURCE/townManager.h>
VA(0x004543c0, 0x84)
swapManager::swapManager(void) {
    m_window = 0;
    m_selectorIcon = 0;
    m_selectedSide = SWAP_SIDE_NONE;
    m_targetSide = SWAP_SIDE_NONE;
    m_itemType = SWAP_ITEM_NONE;
    m_selectedSlot = SWAP_SLOT_NONE;
    m_targetSlot = SWAP_SLOT_NONE;
    m_heroes[SWAP_SIDE_LEFT] = 0;
    m_heroes[SWAP_SIDE_RIGHT] = 0;
}

VA(0x00454444, 0x3e)
swapManager::swapManager(hero* leftHero, hero* rightHero) {
    m_heroes[SWAP_SIDE_LEFT] = leftHero;
    m_heroes[SWAP_SIDE_RIGHT] = rightHero;
}

VA(0x00454482, 0x50)
void swapManager::Reset(void) {
    m_selectedSide = m_targetSide = m_itemType = m_selectedSlot = m_targetSlot = SWAP_SLOT_NONE;
}

VA(0x004544d2, 0x30)
i32 swapManager::DrawSwapWin(void) {
    m_window->DrawWindow(0);
    gpWindowManager->UpdateScreen();
    return 0;
}

VA(0x00454502, 0x3bc)
i32 swapManager::Open(i32 id) {
    Reset();
    m_window = new heroWindow(0, 0, "swapwin.bin");
    if (m_window == 0)
        MemError();

    SetWinText(m_window, SWAP_WINDOW_TEXT_ID);

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    sprintf(gText, "port%04d.icn", m_heroes[SWAP_SIDE_LEFT]->m_portrait);
    message.payload.widget.id = SWAP_LEFT_PORTRAIT_WIDGET;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", m_heroes[SWAP_SIDE_RIGHT]->m_portrait);
    message.payload.widget.id = SWAP_RIGHT_PORTRAIT_WIDGET;
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    sprintf(
        gText,
        "%s meets %s",
        m_heroes[SWAP_SIDE_LEFT]->m_name,
        m_heroes[SWAP_SIDE_RIGHT]->m_name
    );
    message.payload.widget.data.text = gText;
    message.payload.widget.id = SWAP_TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (i32 side_6 = SWAP_SIDE_LEFT; side_6 < SWAP_SIDE_COUNT; ++side_6) {
        for (i32 skillSlot = 0; skillSlot < SWAP_SECONDARY_SKILL_WIDGET_COUNT; ++skillSlot) {
            if (skillSlot < m_heroes[side_6]->m_secondarySkillCount) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT + skillSlot
                                            + SWAP_CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = m_heroes[side_6]->GetNthSS(skillSlot);
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT + skillSlot
                                            + SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.payload.widget.data.text = gText;
                sprintf(
                    gText,
                    "%d",
                    m_heroes[side_6]->GetSSLevel(m_heroes[side_6]->GetNthSS(skillSlot))
                );
                m_window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.id = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT + skillSlot
                                            + SWAP_CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = SWAP_EMPTY_SKILL_VALUE;
                m_window->BroadcastMessage(message);
            }
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = SWAP_ADVENTURE_DISABLE_VALUE;
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);

    Update();
    gpWindowManager->AddWindow(m_window, -1, 1);
    KBChangeMenu(hmnuAdv);
    giMonoIconSkip = SWAP_MONO_ICON_SKIP;
    m_selectorIcon = gpResourceManager->GetIcon("swapbtn.icn");
    giMonoIconSkip = SWAP_MONO_ICON_DEFAULT;
    gpMouseManager->SetPointer(0);
    m_messageMask = SWAP_MANAGER_MESSAGE;
    m_priority = id;
    m_active = 1;
    strcpy(m_name, "swapManager");
    return 0;
}

VA(0x004548be, 0x12d)
void swapManager::Close(void) {
    gpResourceManager->Dispose(m_selectorIcon);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = 0;
    gpAdvManager->Activate();

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.data.value = SWAP_ADVENTURE_DISABLE_VALUE;
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = SWAP_ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
}

VA(0x004549eb, 0x1f8)
void swapManager::DrawSelector(void) {
    const char selectorFrame_17 = 10;
    const i16 leftArmyX_37 = 37;
    const i16 rightArmyX_12 = 382;
    const i16 armyY_19 = 268;
    const i16 armySpacing_16 = 45;
    const i16 leftArtifactX_1 = 24;
    const i16 rightArtifactX_6 = 368;
    const i16 artifactY_7 = 348;
    const i16 artifactSpacing_11 = 36;
    i32 selectorX_2 = 0;
    i32 selectorY_11 = 0;

    if (m_selectedSide != SWAP_SIDE_NONE && m_selectedSlot != SWAP_SLOT_NONE) {
        switch (m_selectedSide) {
            case SWAP_SIDE_LEFT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX_2 = m_selectedSlot * armySpacing_16 + leftArmyX_37 - 1;
                        selectorY_11 = armyY_19 - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX_2 =
                            (m_selectedSlot % SWAP_ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + leftArtifactX_1 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot <= SWAP_ARTIFACT_FIRST_ROW_LAST ? 0
                                                                              : artifactSpacing_11)
                            - 1;
                        break;
                }
                break;
            case SWAP_SIDE_RIGHT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX_2 = m_selectedSlot * armySpacing_16 + rightArmyX_12 - 1;
                        selectorY_11 = armyY_19 - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX_2 =
                            (m_selectedSlot % SWAP_ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + rightArtifactX_6 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot <= SWAP_ARTIFACT_FIRST_ROW_LAST ? 0
                                                                              : artifactSpacing_11)
                            - 1;
                        break;
                }
                break;
        }

        m_selectorIcon->FillToBuffer(
            selectorX_2,
            selectorY_11,
            m_itemType == SWAP_ITEM_ARMY ? SWAP_ARMY_SELECTOR_FRAME : SWAP_ARTIFACT_SELECTOR_FRAME,
            selectorFrame_17,
            0,
            0
        );
        gpWindowManager->UpdateScreenRegion(
            selectorX_2,
            selectorY_11,
            SWAP_SELECTOR_WIDTH,
            SWAP_SELECTOR_HEIGHT
        );
    }
}

// @semantic
// Complete 0xaf0 body, 0x28 frame/slots, source CFG, and all 71 ordered relocation
// sites/effective targets align. Fifty-six identities agree literally; two are
// equivalent string owners and thirteen are delinked local table labels. The
// embedded pointer/index tables and their eleven resolved body offsets agree.
// Outside table data, twelve branch bytes differ at +0x29a/+0x29b, +0x2f9/+0x2fa,
// +0x43c/+0x43d, +0x49b/+0x49c, +0x60b/+0x60c, and +0x7d9/+0x7da: retail's six
// completed dialog/view arms jump directly to the shared post-command block,
// while ours first reaches the matching case-local tail. An explicit shared goto
// grew the case layout and shifted later relocations, so it was rejected. Ten
// bounded TU-state trials also failed to close the residual. Revisit after an
// earlier SWAPMGR/header change or new evidence for case-exit lowering.
VA(0x00454be3, 0xaf0)
i32 swapManager::Main(tag_message& message) {
    i32 closeRequested_5 = 0;
    i32 quickView = (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0;
    i32 side;
    i32 slotIndex_8;
    i32 artifactSlot_2;
    i32 secondarySkill_1;

    switch (message.type) {
        case MESSAGE_RIGHT_BUTTON_DOWN:
            if (quickView)
                break;
            Reset();
            Update();
            DrawSwapWin();
            break;

        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case SWAP_COMMAND_SELECT:
                    if (quickView)
                        break;
                    if (message.payload.widget.id == SWAP_CONTROL_CLOSE)
                        closeRequested_5 = 1;
                    break;

                case SWAP_COMMAND_HOVER:
                case SWAP_COMMAND_HELP:
                    switch (message.payload.widget.id) {
                        case SWAP_CONTROL_LEFT_SKILL_FIRST:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 1:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 2:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 3:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 4:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 5:
                        case SWAP_CONTROL_LEFT_SKILL_FIRST + 6:
                        case SWAP_CONTROL_LEFT_SKILL_LAST:
                            side = SWAP_SIDE_LEFT;
                            slotIndex_8 = message.payload.widget.id - SWAP_CONTROL_LEFT_SKILL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 1:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 2:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 3:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 4:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 5:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 6:
                        case SWAP_CONTROL_LEFT_SKILL_LEVEL_LAST:
                            side = SWAP_SIDE_LEFT;
                            slotIndex_8 =
                                message.payload.widget.id - SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case SWAP_CONTROL_RIGHT_SKILL_FIRST:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 1:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 2:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 3:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 4:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 5:
                        case SWAP_CONTROL_RIGHT_SKILL_FIRST + 6:
                        case SWAP_CONTROL_RIGHT_SKILL_LAST:
                            side = SWAP_SIDE_RIGHT;
                            slotIndex_8 =
                                message.payload.widget.id - SWAP_CONTROL_RIGHT_SKILL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 1:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 2:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 3:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 4:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 5:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 6:
                        case SWAP_CONTROL_RIGHT_SKILL_LEVEL_LAST:
                            side = SWAP_SIDE_RIGHT;
                            slotIndex_8 =
                                message.payload.widget.id - SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST;
                        showSecondarySkill:
                            if (slotIndex_8 >= m_heroes[side]->m_secondarySkillCount)
                                break;
                            secondarySkill_1 = m_heroes[side]->GetNthSS(slotIndex_8);
                            m_heroes[side]->DoSSLevelDialog(secondarySkill_1, quickView);
                            break;

                        case SWAP_CONTROL_LEFT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[SWAP_SIDE_LEFT]->m_id, 1, 0);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(0, SWAP_FADE_STEPS, 0);
                            break;

                        case SWAP_CONTROL_RIGHT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[SWAP_SIDE_RIGHT]->m_id, 1, 0);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(0, SWAP_FADE_STEPS, 0);
                            break;

                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 1:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 2:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 3:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 4:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 5:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 6:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 7:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 8:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 9:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 10:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 11:
                        case SWAP_CONTROL_LEFT_ARTIFACT_FIRST + 12:
                        case SWAP_CONTROL_LEFT_ARTIFACT_LAST:
                            artifactSlot_2 =
                                message.payload.widget.id - SWAP_CONTROL_LEFT_ARTIFACT_FIRST;
                            if (!quickView
                                && m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(
                                    "This item can't be traded.",
                                    NORMAL_DIALOG_INFO,
                                    NORMAL_DIALOG_NO_VALUE,
                                    NORMAL_DIALOG_NO_VALUE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_VALUE,
                                    0
                                );
                            } else if (quickView) {
                                if (m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[SWAP_SIDE_LEFT]->ViewArtifact(
                                    m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2],
                                    SWAP_VIEW_QUICK,
                                    m_heroes[SWAP_SIDE_LEFT]->m_artifactExtra[artifactSlot_2]
                                );
                            } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_LEFT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot_2;
                                    m_targetSlot = SWAP_SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_LEFT;
                                m_targetSlot = artifactSlot_2;
                                if (m_selectedSide == SWAP_SIDE_LEFT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[SWAP_SIDE_LEFT]->ViewArtifact(
                                        m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2],
                                        SWAP_VIEW_FULL,
                                        m_heroes[SWAP_SIDE_LEFT]->m_artifactExtra[artifactSlot_2]
                                    );
                                    Reset();
                                } else {
                                    SwapArtifacts();
                                    Reset();
                                }
                            }
                            break;

                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 1:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 2:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 3:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 4:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 5:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 6:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 7:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 8:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 9:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 10:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 11:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_FIRST + 12:
                        case SWAP_CONTROL_RIGHT_ARTIFACT_LAST:
                            artifactSlot_2 =
                                message.payload.widget.id - SWAP_CONTROL_RIGHT_ARTIFACT_FIRST;
                            if (!quickView
                                && m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(
                                    "This item can't be traded.",
                                    NORMAL_DIALOG_INFO,
                                    NORMAL_DIALOG_NO_VALUE,
                                    NORMAL_DIALOG_NO_VALUE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_VALUE,
                                    0
                                );
                            } else if (quickView) {
                                if (m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[SWAP_SIDE_RIGHT]->ViewArtifact(
                                    m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2],
                                    SWAP_VIEW_QUICK,
                                    m_heroes[SWAP_SIDE_RIGHT]->m_artifactExtra[artifactSlot_2]
                                );
                            } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_RIGHT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot_2;
                                    m_targetSlot = SWAP_SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_RIGHT;
                                m_targetSlot = artifactSlot_2;
                                if (m_selectedSide == SWAP_SIDE_RIGHT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[SWAP_SIDE_RIGHT]->ViewArtifact(
                                        m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2],
                                        SWAP_VIEW_FULL,
                                        m_heroes[SWAP_SIDE_RIGHT]->m_artifactExtra[artifactSlot_2]
                                    );
                                    Reset();
                                } else {
                                    SwapArtifacts();
                                    Reset();
                                }
                            }
                            break;

                        case SWAP_CONTROL_LEFT_ARMY_FIRST:
                        case SWAP_CONTROL_LEFT_ARMY_FIRST + 1:
                        case SWAP_CONTROL_LEFT_ARMY_FIRST + 2:
                        case SWAP_CONTROL_LEFT_ARMY_FIRST + 3:
                        case SWAP_CONTROL_LEFT_ARMY_LAST:
                            if (quickView) {
                                if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes
                                        [message.payload.widget.id - SWAP_CONTROL_LEFT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        SWAP_ARMY_VIEW_X,
                                        SWAP_ARMY_VIEW_Y,
                                        m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - SWAP_CONTROL_LEFT_ARMY_FIRST],
                                        m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - SWAP_CONTROL_LEFT_ARMY_FIRST],
                                        0,
                                        0,
                                        1,
                                        1,
                                        m_heroes[SWAP_SIDE_LEFT],
                                        0,
                                        &m_heroes[SWAP_SIDE_LEFT]->m_army,
                                        message.payload.widget.id - SWAP_CONTROL_LEFT_ARMY_FIRST
                                    );
                                }
                            } else if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes
                                        [message.payload.widget.id - SWAP_CONTROL_LEFT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    m_selectedSide = SWAP_SIDE_LEFT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARMY;
                                    m_selectedSlot =
                                        message.payload.widget.id - SWAP_CONTROL_LEFT_ARMY_FIRST;
                                    m_targetSlot = SWAP_SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_LEFT;
                                m_targetSlot =
                                    message.payload.widget.id - SWAP_CONTROL_LEFT_ARMY_FIRST;
                                if (m_selectedSide == SWAP_SIDE_LEFT
                                    && m_selectedSlot == m_targetSlot) {
                                    ViewMon();
                                    Reset();
                                } else if ((message.payload.widget.parameter
                                            & SWAP_SPLIT_MODIFIER_MASK)
                                               != 0
                                           && (m_heroes[m_targetSide]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[m_selectedSide]
                                                          ->m_army.m_creatureTypes[m_selectedSlot]
                                                      == m_heroes[m_targetSide]
                                                             ->m_army
                                                             .m_creatureTypes[m_targetSlot])) {
                                    SplitMons();
                                    Reset();
                                } else {
                                    SwapMons();
                                    Reset();
                                }
                            }
                            break;

                        case SWAP_CONTROL_RIGHT_ARMY_FIRST:
                        case SWAP_CONTROL_RIGHT_ARMY_FIRST + 1:
                        case SWAP_CONTROL_RIGHT_ARMY_FIRST + 2:
                        case SWAP_CONTROL_RIGHT_ARMY_FIRST + 3:
                        case SWAP_CONTROL_RIGHT_ARMY_LAST:
                            if (quickView) {
                                if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes
                                        [message.payload.widget.id - SWAP_CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        SWAP_ARMY_VIEW_X,
                                        SWAP_ARMY_VIEW_Y,
                                        m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - SWAP_CONTROL_RIGHT_ARMY_FIRST],
                                        m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - SWAP_CONTROL_RIGHT_ARMY_FIRST],
                                        0,
                                        0,
                                        1,
                                        1,
                                        m_heroes[SWAP_SIDE_RIGHT],
                                        0,
                                        &m_heroes[SWAP_SIDE_RIGHT]->m_army,
                                        message.payload.widget.id - SWAP_CONTROL_RIGHT_ARMY_FIRST
                                    );
                                }
                            } else if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes
                                        [message.payload.widget.id - SWAP_CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    m_selectedSide = SWAP_SIDE_RIGHT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARMY;
                                    m_selectedSlot =
                                        message.payload.widget.id - SWAP_CONTROL_RIGHT_ARMY_FIRST;
                                    m_targetSlot = SWAP_SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_RIGHT;
                                m_targetSlot =
                                    message.payload.widget.id - SWAP_CONTROL_RIGHT_ARMY_FIRST;
                                if (m_selectedSide == SWAP_SIDE_RIGHT
                                    && m_selectedSlot == m_targetSlot) {
                                    ViewMon();
                                    Reset();
                                } else if ((message.payload.widget.parameter
                                            & SWAP_SPLIT_MODIFIER_MASK)
                                               != 0
                                           && (m_heroes[m_targetSide]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[m_selectedSide]
                                                          ->m_army.m_creatureTypes[m_selectedSlot]
                                                      == m_heroes[m_targetSide]
                                                             ->m_army
                                                             .m_creatureTypes[m_targetSlot])) {
                                    SplitMons();
                                    Reset();
                                } else {
                                    SwapMons();
                                    Reset();
                                }
                            }
                            break;

                        default:
                            break;
                    }

                    if (!quickView) {
                        Update();
                        DrawSwapWin();
                        DrawSelector();
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    if (closeRequested_5 == SWAP_CLOSE_REQUESTED) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.widget.command = SWAP_COMMAND_EXIT;
        return SWAP_RESULT_CLOSE;
    }
    return SWAP_RESULT_CONTINUE;
}

VA(0x004556d3, 0xa3)
void swapManager::ViewMon(void) {
    gpGame->ViewArmy(
        SWAP_ARMY_VIEW_X,
        SWAP_ARMY_VIEW_Y,
        m_heroes[m_selectedSide]->m_army.m_creatureTypes[m_targetSlot],
        m_heroes[m_selectedSide]->m_army.m_creatureCounts[m_targetSlot],
        0,
        m_heroes[m_selectedSide]->m_army.GetNumArmies() == 1,
        1,
        0,
        m_heroes[m_selectedSide],
        0,
        &m_heroes[m_selectedSide]->m_army,
        m_targetSlot
    );
}

VA(0x00455776, 0x255)
void swapManager::SwapArtifacts(void) {
    i32 selectedArtifact = m_heroes[m_selectedSide]->m_artifacts[m_selectedSlot];
    i32 targetArtifact_2 = m_heroes[m_targetSide]->m_artifacts[m_targetSlot];

    GiveTakeArtifactStat(m_heroes[m_selectedSide], selectedArtifact, 1);
    GiveTakeArtifactStat(m_heroes[m_targetSide], targetArtifact_2, 1);
    m_heroes[m_selectedSide]->m_artifacts[m_selectedSlot] = targetArtifact_2;
    m_heroes[m_targetSide]->m_artifacts[m_targetSlot] = selectedArtifact;

    i8 extra = m_heroes[m_selectedSide]->m_artifactExtra[m_selectedSlot];
    m_heroes[m_selectedSide]->m_artifactExtra[m_selectedSlot] =
        m_heroes[m_targetSide]->m_artifactExtra[m_targetSlot];
    m_heroes[m_targetSide]->m_artifactExtra[m_targetSlot] = extra;

    GiveTakeArtifactStat(m_heroes[m_selectedSide], targetArtifact_2, 0);
    GiveTakeArtifactStat(m_heroes[m_targetSide], selectedArtifact, 0);
    m_heroes[m_selectedSide]->CheckAnduranPieces(1);
    m_heroes[m_targetSide]->CheckAnduranPieces(1);

    if (selectedArtifact == ARTIFACT_SPADE_NECROMANCY
        || targetArtifact_2 == ARTIFACT_SPADE_NECROMANCY) {
        tag_message message_1;
        message_1.type = MESSAGE_WIDGET;
        for (i32 side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (i32 skillSlot = 0; skillSlot < SWAP_SECONDARY_SKILL_WIDGET_COUNT; ++skillSlot) {
                if (skillSlot < m_heroes[side]->m_secondarySkillCount) {
                    message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                    message_1.payload.widget.id = side * SWAP_SECONDARY_SKILL_WIDGET_COUNT
                                                  + skillSlot + SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST;
                    message_1.payload.widget.data.text = gText;
                    sprintf(
                        gText,
                        "%d",
                        m_heroes[side]->GetSSLevel(m_heroes[side]->GetNthSS(skillSlot))
                    );
                    m_window->BroadcastMessage(message_1);
                }
            }
        }
    }
}

VA(0x004559cb, 0x177)
void swapManager::SwapMons(void) {
    i32 selectedArmyCount = 0;
    for (i32 slot_1 = 0; slot_1 < ARMY_GROUP_SLOT_COUNT; ++slot_1) {
        if (m_heroes[m_selectedSide]->m_army.m_creatureTypes[slot_1] != CREATURE_NONE
            && m_heroes[m_selectedSide]->m_army.m_creatureCounts[slot_1] > 0)
            ++selectedArmyCount;
    }

    armyGroup* selectedArmy = &m_heroes[m_selectedSide]->m_army;
    armyGroup* targetArmy = &m_heroes[m_targetSide]->m_army;
    if (OD_STEER(selectedArmy->m_creatureTypes[m_selectedSlot])
        == targetArmy->m_creatureTypes[m_targetSlot]) {
        if (selectedArmy->GetNumArmies() == 1)
            return;
        targetArmy->m_creatureCounts[m_targetSlot] =
            OD_STEER(selectedArmy->m_creatureCounts[m_selectedSlot])
            + targetArmy->m_creatureCounts[m_targetSlot];
        selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
        selectedArmy->m_creatureCounts[m_selectedSlot] = 0;
        return;
    }
    if (OD_STEER(selectedArmy) != targetArmy && selectedArmy->GetNumArmies() == 1
        && targetArmy->m_creatureTypes[m_targetSlot] == CREATURE_NONE)
        return;
    selectedArmy->Swap(m_selectedSlot, targetArmy, m_targetSlot);
}

VA(0x00455b42, 0x47b)
void swapManager::Update(void) {
    i32 slot;
    tag_message message_1;
    message_1.type = MESSAGE_WIDGET;
    message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message_1.payload.widget.data.text = gText;

    for (slot = 0; slot < SWAP_PRIMARY_SKILL_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_LEFT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[SWAP_SIDE_LEFT]->Stats(slot));
        m_window->BroadcastMessage(message_1);
        message_1.payload.widget.id = slot + SWAP_RIGHT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[SWAP_SIDE_RIGHT]->Stats(slot));
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_CONTROL_LEFT_ARMY_FIRST;
        if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value =
                m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_LEFT_ARMY_COUNT_FIRST;
        if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureCounts[slot]);
            message_1.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_CONTROL_RIGHT_ARMY_FIRST;
        if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value =
                m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_RIGHT_ARMY_COUNT_FIRST;
        if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureCounts[slot]);
            message_1.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_CONTROL_LEFT_ARTIFACT_FIRST;
        if (m_heroes[SWAP_SIDE_LEFT]->m_artifacts[slot] == ARTIFACT_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value = m_heroes[SWAP_SIDE_LEFT]->m_artifacts[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + SWAP_CONTROL_RIGHT_ARTIFACT_FIRST;
        if (m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[slot] == ARTIFACT_NONE) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = SWAP_EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value = m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[slot];
        }
        m_window->BroadcastMessage(message_1);
    }
}

VA(0x00455fbd, 0x388)
void swapManager::SplitMons(void) {
    i16 unusedAmountControl_29 = SWAP_SPLIT_AMOUNT_CONTROL;
    i32 unusedState = 0;
    armyGroup* selectedArmy = &m_heroes[m_selectedSide]->m_army;
    armyGroup* targetArmy = &m_heroes[m_targetSide]->m_army;
    i32 emptySlot;
    unusedState = 0;
    i16 unusedTextControl_2 = SWAP_SPLIT_TEXT_CONTROL;

    gpTownManager->m_heroWindow1 =
        new heroWindow(SWAP_SPLIT_WINDOW_X, SWAP_SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == 0)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = selectedArmy->m_creatureCounts[m_selectedSlot];

    tag_message message;
    message.type = MESSAGE_WIDGET;
    if (m_selectedSide == m_targetSide) {
        sprintf(gText, "Move how many troops?");
    } else {
        sprintf(
            gText,
            "Move how many %s troops from %s to %s?",
            gArmyNames[selectedArmy->m_creatureTypes[m_selectedSlot]],
            m_heroes[m_selectedSide]->m_name,
            m_heroes[m_targetSide]->m_name
        );
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SWAP_SPLIT_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.payload.widget.id = SWAP_SPLIT_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(gpTownManager->m_heroWindow1, SplitArmyHandler, 0);
    delete gpTownManager->m_heroWindow1;

    if (gpWindowManager->m_dialogResult == SWAP_SPLIT_CONFIRM) {
        if (selectedArmy->m_creatureTypes[m_selectedSlot]
            == targetArmy->m_creatureTypes[m_targetSlot]) {
            selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
            targetArmy->m_creatureCounts[m_targetSlot] += gpTownManager->m_splitAmount;
            if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
                selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
            return;
        }
        if (targetArmy->m_creatureTypes[m_targetSlot] != CREATURE_NONE) {
            for (emptySlot = 0; emptySlot < ARMY_GROUP_SLOT_COUNT; ++emptySlot) {
                if (targetArmy->m_creatureTypes[emptySlot] == CREATURE_NONE)
                    break;
            }
            if (emptySlot < ARMY_GROUP_SLOT_COUNT)
                m_targetSlot = emptySlot;
        }
        targetArmy->m_creatureTypes[m_targetSlot] = selectedArmy->m_creatureTypes[m_selectedSlot];
        targetArmy->m_creatureCounts[m_targetSlot] = gpTownManager->m_splitAmount;
        selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
        if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
            selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
    }
}

// ===== vtable swapManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00454502, 0x3bc)  int swapManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004548be, 0x12d)  void swapManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x00454be3, 0xaf0)  int swapManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(swapManager, 0x004eb650);
