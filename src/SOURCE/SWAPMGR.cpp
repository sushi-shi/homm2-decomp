#include <Ints.h>
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
typedef enum SwapManagerControl {
    CONTROL_LEFT_HERO               = 0x41,
    CONTROL_RIGHT_HERO              = 0x42,
    CONTROL_LEFT_ARMY_FIRST         = 0x4e,
    CONTROL_LEFT_ARMY_LAST          = 0x52,
    CONTROL_RIGHT_ARMY_FIRST        = 0x53,
    CONTROL_RIGHT_ARMY_LAST         = 0x57,
    CONTROL_LEFT_ARTIFACT_FIRST     = 0x58,
    CONTROL_LEFT_ARTIFACT_LAST      = 0x65,
    CONTROL_RIGHT_ARTIFACT_FIRST    = 0x66,
    CONTROL_RIGHT_ARTIFACT_LAST     = 0x73,
    CONTROL_LEFT_SKILL_FIRST        = 0xc8,
    CONTROL_LEFT_SKILL_LAST         = 0xcf,
    CONTROL_RIGHT_SKILL_FIRST       = 0xd0,
    CONTROL_RIGHT_SKILL_LAST        = 0xd7,
    CONTROL_LEFT_SKILL_LEVEL_FIRST  = 0xd8,
    CONTROL_LEFT_SKILL_LEVEL_LAST   = 0xdf,
    CONTROL_RIGHT_SKILL_LEVEL_FIRST = 0xe0,
    CONTROL_RIGHT_SKILL_LEVEL_LAST  = 0xe7
} SwapManagerControl;

typedef enum SwapManagerConstant {
    SECONDARY_SKILL_WIDGET_COUNT = 8,
    WINDOW_TEXT_ID               = 0x15,
    CONTROL_CLOSE                = 0x7800,
    SPLIT_MODIFIER_MASK          = 3,
    LEFT_PORTRAIT_WIDGET         = 0x41,
    RIGHT_PORTRAIT_WIDGET        = 0x42,
    TITLE_WIDGET                 = 0x4d,
    ADVENTURE_WIDGET_FIRST       = 1,
    ADVENTURE_WIDGET_LAST        = 6,
    ADVENTURE_DISABLE_VALUE      = 2,
    EMPTY_SKILL_VALUE            = 4,
    MONO_ICON_SKIP               = 2,
    MONO_ICON_DEFAULT            = -1,
    MANAGER_MESSAGE              = 0x100,
    SLOT_NONE                    = -1,
    ARTIFACT_COLUMN_COUNT        = 7,
    FADE_STEPS                   = 8,
    VIEW_FULL                    = 0,
    VIEW_QUICK                   = 1,
    CLOSE_REQUESTED              = 1,
    SELECTOR_WIDTH               = 0x2e,
    SELECTOR_HEIGHT              = 0x2e,
    ARMY_SELECTOR_FRAME          = 3,
    ARTIFACT_SELECTOR_FRAME      = 2,
    EMPTY_ITEM_VALUE             = 4,
    ARTIFACT_FIRST_ROW_LAST      = 6,
    PRIMARY_SKILL_COUNT          = 4,
    LEFT_PRIMARY_SKILL_FIRST     = 0x43,
    RIGHT_PRIMARY_SKILL_FIRST    = 0x48,
    LEFT_ARMY_COUNT_FIRST        = 0x74,
    RIGHT_ARMY_COUNT_FIRST       = 0x79,
    ARMY_VIEW_X                  = 0x77,
    ARMY_VIEW_Y                  = 0x14,
    SPLIT_WINDOW_X               = 0xb1,
    SPLIT_WINDOW_Y               = 0x14,
    SPLIT_TEXT_CONTROL           = 1,
    SPLIT_AMOUNT_CONTROL         = 0x44,
    SPLIT_CONFIRM                = 0x7802
} SwapManagerConstant;

swapManager::swapManager(void) {
    m_window = NULL;
    m_selectorIcon = NULL;
    m_selectedSide = SWAP_SIDE_NONE;
    m_targetSide = SWAP_SIDE_NONE;
    m_itemType = SWAP_ITEM_NONE;
    m_selectedSlot = SLOT_NONE;
    m_targetSlot = SLOT_NONE;
    m_heroes[(SWAP_SIDE_LEFT)] = NULL;
    m_heroes[(SWAP_SIDE_RIGHT)] = NULL;
}

swapManager::swapManager(hero* leftHero, hero* rightHero) {
    m_heroes[(SWAP_SIDE_LEFT)] = leftHero;
    m_heroes[(SWAP_SIDE_RIGHT)] = rightHero;
}

void swapManager::Reset(void) {
    do {
        (m_targetSlot) = static_cast<decltype(m_targetSlot)>(SLOT_NONE);
        (m_selectedSlot) = static_cast<decltype(m_selectedSlot)>(SLOT_NONE);
        (m_itemType) = static_cast<decltype(m_itemType)>(SLOT_NONE);
        (m_targetSide) = static_cast<decltype(m_targetSide)>(SLOT_NONE);
        (m_selectedSide) = static_cast<decltype(m_selectedSide)>(SLOT_NONE);
    } while (0);
}

i32 swapManager::DrawSwapWin(void) {
    m_window->DrawWindow(0);
    gpWindowManager->UpdateScreen();
    return 0;
}

i32 swapManager::Open(i32 id) {
    Reset();
    m_window = new heroWindow(0, 0, "swapwin.bin");
    if (m_window == NULL)
        MemError();

    SetWinText(m_window, WINDOW_TEXT_ID);

    tag_message message;
    i32 skillWidget;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    sprintf(gText, "port%04d.icn", (m_heroes[(SWAP_SIDE_LEFT)]->m_portrait));
    message.payload.widget.id = LEFT_PORTRAIT_WIDGET;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", (m_heroes[(SWAP_SIDE_RIGHT)]->m_portrait));
    message.payload.widget.id = RIGHT_PORTRAIT_WIDGET;
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    sprintf(
        gText,
          "Встречаются\x20\x25\x73\x20и\x20\x25\x73",
        m_heroes[(SWAP_SIDE_LEFT)]->m_name,
        m_heroes[(SWAP_SIDE_RIGHT)]->m_name
    );
    message.payload.widget.data.text = gText;
    message.payload.widget.id = TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (SwapManagerSide swapSide = SWAP_SIDE_LEFT; swapSide < SWAP_SIDE_COUNT; ++swapSide) {
        for (skillWidget = 0; skillWidget < SECONDARY_SKILL_WIDGET_COUNT; ++skillWidget) {
            if (skillWidget < m_heroes[(swapSide)]->m_secondarySkillCount) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = (swapSide) * SECONDARY_SKILL_WIDGET_COUNT + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = (m_heroes[(swapSide)]->GetNthSS(skillWidget));
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = (swapSide) * SECONDARY_SKILL_WIDGET_COUNT + skillWidget
                                            + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.payload.widget.data.text = gText;
                sprintf(
                    gText,
                    "%d",
                    m_heroes[(swapSide)]->GetSSLevel(m_heroes[(swapSide)]->GetNthSS(skillWidget))
                );
                m_window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.id = (swapSide) * SECONDARY_SKILL_WIDGET_COUNT + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = EMPTY_SKILL_VALUE;
                m_window->BroadcastMessage(message);
            }
        }
    }

    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
    message.payload.widget.data.value = ADVENTURE_DISABLE_VALUE;
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);

    Update();
    gpWindowManager->AddWindow(m_window, -1, 1);
    KBChangeMenu(hmnuAdv);
    giMonoIconSkip = MONO_ICON_SKIP;
    m_selectorIcon = gpResourceManager->GetIcon("swapbtn.icn");
    giMonoIconSkip = MONO_ICON_DEFAULT;
    gpMouseManager->SetPointer(0);
    m_messageMask = BASE_MANAGER_ACCEPT_SWAP;
    m_priority = id;
    m_active = true;
    strcpy(m_name, "swapManager");
    return 0;
}

void swapManager::Close(void) {
    gpResourceManager->Dispose(m_selectorIcon);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = false;
    gpAdvManager->Activate();

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
    message.payload.widget.data.value = ADVENTURE_DISABLE_VALUE;
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.payload.widget.id = ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
}

void swapManager::DrawSelector(void) {
    const char selectorFrame_6 = 10;
    const i16 leftArmyX_3 = 37;
    const i16 rightArmyX_3 = 382;
    const i16 armyY_9 = 268;
    const i16 armySpacing_2 = 45;
    const i16 leftArtifactX_7 = 24;
    const i16 rightArtifactX_6 = 368;
    const i16 artifactY_7 = 348;
    const i16 artifactSpacing_11 = 36;
    i32 selectorX_2 = 0;
    i32 selectorY_11 = 0;

    if (m_selectedSide != SWAP_SIDE_NONE && m_selectedSlot != SLOT_NONE) {
        switch (m_selectedSide) {
            case SWAP_SIDE_LEFT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX_2 = m_selectedSlot * armySpacing_2 + leftArmyX_3 - 1;
                        selectorY_11 = armyY_9 - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX_2 =
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + leftArtifactX_7 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot > ARTIFACT_FIRST_ROW_LAST ? artifactSpacing_11 : 0)
                            - 1;
                        break;
                }
                break;
            case SWAP_SIDE_RIGHT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX_2 = m_selectedSlot * armySpacing_2 + rightArmyX_3 - 1;
                        selectorY_11 = armyY_9 - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX_2 =
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + rightArtifactX_6 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot > ARTIFACT_FIRST_ROW_LAST ? artifactSpacing_11 : 0)
                            - 1;
                        break;
                }
                break;
        }

        m_selectorIcon->FillToBuffer(
            selectorX_2,
            selectorY_11,
            m_itemType == SWAP_ITEM_ARMY ? ARMY_SELECTOR_FRAME : ARTIFACT_SELECTOR_FRAME,
            selectorFrame_6,
            ICON_DRAW_NORMAL,
            NULL
        );
        gpWindowManager->UpdateScreenRegion(
            selectorX_2,
            selectorY_11,
            SELECTOR_WIDTH,
            SELECTOR_HEIGHT
        );
    }
}

MessageDispatchResult swapManager::Main(tag_message& message) {
    b32 closeRequested_5 = false;
    b32 quickView_9;
    SwapManagerSide side_6;
    i32 slotIndex_2;
    i32 artifactSlot_2;
    HeroSecondarySkill secondarySkill_1;

    if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
        quickView_9 = true;
    else
        quickView_9 = false;

    switch (message.type) {
        case MESSAGE_RIGHT_BUTTON_DOWN:
            if (quickView_9)
                break;
            Reset();
            Update();
            DrawSwapWin();
            break;

        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DESELECT:
                    if (quickView_9)
                        break;
                    if (message.payload.widget.id == CONTROL_CLOSE)
                        closeRequested_5 = true;
                    break;

                case WIDGET_COMMAND_SELECT:
                case WIDGET_COMMAND_ALTERNATE_SELECT:
                    switch (message.payload.widget.id) {
                        case CONTROL_LEFT_SKILL_FIRST:
                        case CONTROL_LEFT_SKILL_FIRST + 1:
                        case CONTROL_LEFT_SKILL_FIRST + 2:
                        case CONTROL_LEFT_SKILL_FIRST + 3:
                        case CONTROL_LEFT_SKILL_FIRST + 4:
                        case CONTROL_LEFT_SKILL_FIRST + 5:
                        case CONTROL_LEFT_SKILL_FIRST + 6:
                        case CONTROL_LEFT_SKILL_LAST:
                            side_6 = SWAP_SIDE_LEFT;
                            slotIndex_2 = message.payload.widget.id - CONTROL_LEFT_SKILL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case CONTROL_LEFT_SKILL_LEVEL_FIRST:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 1:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 2:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 3:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 4:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 5:
                        case CONTROL_LEFT_SKILL_LEVEL_FIRST + 6:
                        case CONTROL_LEFT_SKILL_LEVEL_LAST:
                            side_6 = SWAP_SIDE_LEFT;
                            slotIndex_2 =
                                message.payload.widget.id - CONTROL_LEFT_SKILL_LEVEL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case CONTROL_RIGHT_SKILL_FIRST:
                        case CONTROL_RIGHT_SKILL_FIRST + 1:
                        case CONTROL_RIGHT_SKILL_FIRST + 2:
                        case CONTROL_RIGHT_SKILL_FIRST + 3:
                        case CONTROL_RIGHT_SKILL_FIRST + 4:
                        case CONTROL_RIGHT_SKILL_FIRST + 5:
                        case CONTROL_RIGHT_SKILL_FIRST + 6:
                        case CONTROL_RIGHT_SKILL_LAST:
                            side_6 = SWAP_SIDE_RIGHT;
                            slotIndex_2 =
                                message.payload.widget.id - CONTROL_RIGHT_SKILL_FIRST;
                            goto showSecondarySkill;
                            break;

                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 1:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 2:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 3:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 4:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 5:
                        case CONTROL_RIGHT_SKILL_LEVEL_FIRST + 6:
                        case CONTROL_RIGHT_SKILL_LEVEL_LAST:
                            side_6 = SWAP_SIDE_RIGHT;
                            slotIndex_2 =
                                message.payload.widget.id - CONTROL_RIGHT_SKILL_LEVEL_FIRST;
                        showSecondarySkill:
                            if (slotIndex_2 >= m_heroes[(side_6)]->m_secondarySkillCount)
                                break;
                            secondarySkill_1 = m_heroes[(side_6)]->GetNthSS(slotIndex_2);
                            m_heroes[(side_6)]->DoSSLevelDialog(secondarySkill_1, quickView_9);
                            break;

                        case CONTROL_LEFT_HERO:
                            if (quickView_9)
                                break;
                            HeroView(m_heroes[(SWAP_SIDE_LEFT)]->m_id, true, false);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(FADE_IN, FADE_STEPS, NULL);
                            break;

                        case CONTROL_RIGHT_HERO:
                            if (quickView_9)
                                break;
                            HeroView(m_heroes[(SWAP_SIDE_RIGHT)]->m_id, true, false);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(FADE_IN, FADE_STEPS, NULL);
                            break;

                        case CONTROL_LEFT_ARTIFACT_FIRST:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 1:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 2:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 3:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 4:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 5:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 6:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 7:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 8:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 9:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 10:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 11:
                        case CONTROL_LEFT_ARTIFACT_FIRST + 12:
                        case CONTROL_LEFT_ARTIFACT_LAST:
                            artifactSlot_2 =
                                message.payload.widget.id - CONTROL_LEFT_ARTIFACT_FIRST;
                            if (!quickView_9
                                && m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(
                                      "Нельзя\x20передать\x20этот\x20предмет\x2e",
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
                                break;
                            }
                            if (quickView_9) {
                                if (m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[(SWAP_SIDE_LEFT)]->ViewArtifact(
                                    m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2],
                                    VIEW_QUICK,
                                    m_heroes[(SWAP_SIDE_LEFT)]->m_artifactExtra[artifactSlot_2]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_LEFT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot_2;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_LEFT;
                                m_targetSlot = artifactSlot_2;
                                if (m_selectedSide == SWAP_SIDE_LEFT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[(SWAP_SIDE_LEFT)]->ViewArtifact(
                                        m_heroes[(SWAP_SIDE_LEFT)]
                                            ->m_artifacts[artifactSlot_2],
                                        VIEW_FULL,
                                        m_heroes[(SWAP_SIDE_LEFT)]
                                            ->m_artifactExtra[artifactSlot_2]
                                    );
                                    Reset();
                                } else {
                                    SwapArtifacts();
                                    Reset();
                                }
                            }
                            break;

                        case CONTROL_RIGHT_ARTIFACT_FIRST:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 1:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 2:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 3:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 4:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 5:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 6:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 7:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 8:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 9:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 10:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 11:
                        case CONTROL_RIGHT_ARTIFACT_FIRST + 12:
                        case CONTROL_RIGHT_ARTIFACT_LAST:
                            artifactSlot_2 =
                                message.payload.widget.id - CONTROL_RIGHT_ARTIFACT_FIRST;
                            if (!quickView_9
                                && m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(
                                      "Нельзя\x20передать\x20этот\x20предмет\x2e",
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
                                break;
                            }
                            if (quickView_9) {
                                if (m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                    m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2],
                                    VIEW_QUICK,
                                    m_heroes[(SWAP_SIDE_RIGHT)]->m_artifactExtra[artifactSlot_2]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_RIGHT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot_2;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_RIGHT;
                                m_targetSlot = artifactSlot_2;
                                if (m_selectedSide == SWAP_SIDE_RIGHT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                        m_heroes[(SWAP_SIDE_RIGHT)]
                                            ->m_artifacts[artifactSlot_2],
                                        VIEW_FULL,
                                        m_heroes[(SWAP_SIDE_RIGHT)]
                                            ->m_artifactExtra[artifactSlot_2]
                                    );
                                    Reset();
                                } else {
                                    SwapArtifacts();
                                    Reset();
                                }
                            }
                            break;

                        case CONTROL_LEFT_ARMY_FIRST:
                        case CONTROL_LEFT_ARMY_FIRST + 1:
                        case CONTROL_LEFT_ARMY_FIRST + 2:
                        case CONTROL_LEFT_ARMY_FIRST + 3:
                        case CONTROL_LEFT_ARMY_LAST:
                            if (quickView_9) {
                                if (m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[(SWAP_SIDE_LEFT)],
                                        NULL,
                                        &m_heroes[(SWAP_SIDE_LEFT)]->m_army,
                                        message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    m_selectedSide = SWAP_SIDE_LEFT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARMY;
                                    m_selectedSlot =
                                        message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_LEFT;
                                m_targetSlot =
                                    message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST;
                                if (m_selectedSide == SWAP_SIDE_LEFT
                                    && m_selectedSlot == m_targetSlot) {
                                    ViewMon();
                                    Reset();
                                } else if ((message.payload.widget.parameter
                                            & (SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[(m_selectedSide)]
                                                             ->m_army
                                                             .m_creatureTypes[m_selectedSlot])) {
                                    SplitMons();
                                    Reset();
                                } else {
                                    SwapMons();
                                    Reset();
                                }
                            }
                            break;

                        case CONTROL_RIGHT_ARMY_FIRST:
                        case CONTROL_RIGHT_ARMY_FIRST + 1:
                        case CONTROL_RIGHT_ARMY_FIRST + 2:
                        case CONTROL_RIGHT_ARMY_FIRST + 3:
                        case CONTROL_RIGHT_ARMY_LAST:
                            if (quickView_9) {
                                if (m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[(SWAP_SIDE_RIGHT)],
                                        NULL,
                                        &m_heroes[(SWAP_SIDE_RIGHT)]->m_army,
                                        message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    m_selectedSide = SWAP_SIDE_RIGHT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARMY;
                                    m_selectedSlot =
                                        message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_RIGHT;
                                m_targetSlot =
                                    message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST;
                                if (m_selectedSide == SWAP_SIDE_RIGHT
                                    && m_selectedSlot == m_targetSlot) {
                                    ViewMon();
                                    Reset();
                                } else if ((message.payload.widget.parameter
                                            & (SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[(m_selectedSide)]
                                                             ->m_army
                                                             .m_creatureTypes[m_selectedSlot])) {
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

                    if (!quickView_9) {
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

    if (closeRequested_5 == CLOSE_REQUESTED) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = SWAP_COMMAND_EXIT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void swapManager::ViewMon(void) {
    gpGame->ViewArmy(
        ARMY_VIEW_X,
        ARMY_VIEW_Y,
        m_heroes[(m_selectedSide)]->m_army.m_creatureTypes[m_targetSlot],
        m_heroes[(m_selectedSide)]->m_army.m_creatureCounts[m_targetSlot],
        NULL,
        m_heroes[(m_selectedSide)]->m_army.GetNumArmies() == 1,
        ARMY_FACING_RIGHT,
        0,
        m_heroes[(m_selectedSide)],
        NULL,
        &m_heroes[(m_selectedSide)]->m_army,
        m_targetSlot
    );
}

void swapManager::SwapArtifacts(void) {
    i32 selectedArtifact =
        m_heroes[(m_selectedSide)]->m_artifacts[m_selectedSlot];
    i32 targetArtifact_2 =
        m_heroes[(m_targetSide)]->m_artifacts[m_targetSlot];

    GiveTakeArtifactStat(m_heroes[(m_selectedSide)], selectedArtifact, true);
    GiveTakeArtifactStat(m_heroes[(m_targetSide)], targetArtifact_2, true);
    m_heroes[(m_selectedSide)]->m_artifacts[m_selectedSlot] = targetArtifact_2;
    m_heroes[(m_targetSide)]->m_artifacts[m_targetSlot] = selectedArtifact;

    i8 extra = m_heroes[(m_selectedSide)]->m_artifactExtra[m_selectedSlot];
    m_heroes[(m_selectedSide)]->m_artifactExtra[m_selectedSlot] =
        m_heroes[(m_targetSide)]->m_artifactExtra[m_targetSlot];
    m_heroes[(m_targetSide)]->m_artifactExtra[m_targetSlot] = extra;

    GiveTakeArtifactStat(m_heroes[(m_selectedSide)], targetArtifact_2, false);
    GiveTakeArtifactStat(m_heroes[(m_targetSide)], selectedArtifact, false);
    m_heroes[(m_selectedSide)]->CheckAnduranPieces(true);
    m_heroes[(m_targetSide)]->CheckAnduranPieces(true);

    if (selectedArtifact == ARTIFACT_SPADE_NECROMANCY
        || targetArtifact_2 == ARTIFACT_SPADE_NECROMANCY) {
        tag_message message;
        i32 slotSkill;
        message.type = MESSAGE_WIDGET;
        for (SwapManagerSide side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (slotSkill = 0; slotSkill < SECONDARY_SKILL_WIDGET_COUNT; ++slotSkill) {
                if (slotSkill < m_heroes[(side)]->m_secondarySkillCount) {
                    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                    message.payload.widget.id = (side) * SECONDARY_SKILL_WIDGET_COUNT
                                                  + slotSkill + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                    message.payload.widget.data.text = gText;
                    sprintf(
                        gText,
                        "%d",
                        m_heroes[(side)]->GetSSLevel(m_heroes[(side)]->GetNthSS(slotSkill))
                    );
                    m_window->BroadcastMessage(message);
                }
            }
        }
    }
}

void swapManager::SwapMons(void) {
    i32 selectedArmyCount [[maybe_unused]] = 0;
    for (i32 slot_1 = 0; slot_1 < ARMY_GROUP_SLOT_COUNT; ++slot_1) {
        if (m_heroes[(m_selectedSide)]->m_army.m_creatureTypes[slot_1] != CREATURE_NONE
            && m_heroes[(m_selectedSide)]->m_army.m_creatureCounts[slot_1] > 0)
            ++selectedArmyCount;
    }

    armyGroup* selectedArmy_1 = &m_heroes[(m_selectedSide)]->m_army;
    armyGroup* targetArmy_6 = &m_heroes[(m_targetSide)]->m_army;
    if (targetArmy_6->m_creatureTypes[m_targetSlot]
        == selectedArmy_1->m_creatureTypes[m_selectedSlot]) {
        if (selectedArmy_1->GetNumArmies() == 1)
            return;
        targetArmy_6->m_creatureCounts[m_targetSlot] +=
            selectedArmy_1->m_creatureCounts[m_selectedSlot];
        selectedArmy_1->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
        selectedArmy_1->m_creatureCounts[m_selectedSlot] = 0;
        return;
    }
    if (selectedArmy_1 != targetArmy_6 && selectedArmy_1->GetNumArmies() == 1
        && targetArmy_6->m_creatureTypes[m_targetSlot] == CREATURE_NONE)
        return;
    selectedArmy_1->Swap(m_selectedSlot, targetArmy_6, m_targetSlot);
}

void swapManager::Update(void) {
    i32 slot;
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;

    for (slot = 0; slot < PRIMARY_SKILL_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[(SWAP_SIDE_LEFT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
        message.payload.widget.id = slot + RIGHT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[(SWAP_SIDE_RIGHT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARMY_FIRST;
        if (m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                (m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_ARMY_COUNT_FIRST;
        if (m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARMY_FIRST;
        if (m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                (m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + RIGHT_ARMY_COUNT_FIRST;
        if (m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARTIFACT_FIRST;
        if (m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                (m_heroes[(SWAP_SIDE_LEFT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARTIFACT_FIRST;
        if (m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                (m_heroes[(SWAP_SIDE_RIGHT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }
}

void swapManager::SplitMons(void) {
    i16 textControl [[maybe_unused]];
    armyGroup* targetTroops;
    armyGroup* selectedArmy;
    i32 openSlot;
    i16 unusedAmountControl [[maybe_unused]];
    tag_message message;
    i32 dlgState [[maybe_unused]];

    unusedAmountControl = SPLIT_AMOUNT_CONTROL;
    dlgState = 0;
    selectedArmy = &m_heroes[(m_selectedSide)]->m_army;
    targetTroops = &m_heroes[(m_targetSide)]->m_army;
    dlgState = 0;
    textControl = SPLIT_TEXT_CONTROL;

    gpTownManager->m_heroWindow1 =
        new heroWindow(SPLIT_WINDOW_X, SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == NULL)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = selectedArmy->m_creatureCounts[m_selectedSlot];

    message.type = MESSAGE_WIDGET;
    if (m_selectedSide == m_targetSide) {
        sprintf(
            gText,
              "Сколько\x20воинов\x20перенести\x3f"
        );
    } else {
        sprintf(
            gText,
              "Сколько\x20\x25\x73\x20перенести\x20из\x20армии\x20\x25\x73\x20в\x20армию\x20\x25\x73\x3f",
            gArmyNamesPlural[(selectedArmy->m_creatureTypes[m_selectedSlot])],
            m_heroes[(m_selectedSide)]->m_name,
            m_heroes[(m_targetSide)]->m_name
        );
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SPLIT_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.payload.widget.id = SPLIT_AMOUNT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(gpTownManager->m_heroWindow1, SplitArmyHandler, 0);
    delete gpTownManager->m_heroWindow1;

    if (gpWindowManager->m_dialogResult == SPLIT_CONFIRM) {
        if (targetTroops->m_creatureTypes[m_targetSlot]
            == selectedArmy->m_creatureTypes[m_selectedSlot]) {
            selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
            targetTroops->m_creatureCounts[m_targetSlot] += gpTownManager->m_splitAmount;
            if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
                selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
            return;
        }
        if (targetTroops->m_creatureTypes[m_targetSlot] != CREATURE_NONE) {
            for (openSlot = 0; openSlot < ARMY_GROUP_SLOT_COUNT; ++openSlot) {
                if (targetTroops->m_creatureTypes[openSlot] == CREATURE_NONE)
                    break;
            }
            if (openSlot < ARMY_GROUP_SLOT_COUNT)
                m_targetSlot = openSlot;
        }
        targetTroops->m_creatureTypes[m_targetSlot] = selectedArmy->m_creatureTypes[m_selectedSlot];
        targetTroops->m_creatureCounts[m_targetSlot] = gpTownManager->m_splitAmount;
        selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
        if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
            selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
    }
}
