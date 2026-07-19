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
H2_ENUM_CLASS_BEGIN(SwapManagerMessage)
    COMMAND_HOVER       = 0xc,
    COMMAND_SELECT      = 0xd,
    COMMAND_HELP        = 0xe,
    CONTROL_CLOSE       = 0x7800,
    SPLIT_MODIFIER_MASK = 3
H2_ENUM_CLASS_END(SwapManagerMessage)

H2_ENUM_BEGIN(SwapManagerControl)
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
H2_ENUM_END(SwapManagerControl)

H2_ENUM_BEGIN(SwapManagerConstant)
    SECONDARY_SKILL_WIDGET_COUNT = 8,
    WINDOW_TEXT_ID               = 0x15,
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
    RESULT_CONTINUE              = 1,
    RESULT_CLOSE                 = 2,
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
H2_ENUM_END(SwapManagerConstant)

VA(0x004543c0, 0x84)
swapManager::swapManager(void) {
    m_window = NULL;
    m_selectorIcon = NULL;
    m_selectedSide = SWAP_SIDE_NONE;
    m_targetSide = SWAP_SIDE_NONE;
    m_itemType = SWAP_ITEM_NONE;
    m_selectedSlot = SLOT_NONE;
    m_targetSlot = SLOT_NONE;
    m_heroes[IDX(SWAP_SIDE_LEFT)] = NULL;
    m_heroes[IDX(SWAP_SIDE_RIGHT)] = NULL;
}

VA(0x00454444, 0x3e)
swapManager::swapManager(hero* leftHero, hero* rightHero) {
    m_heroes[IDX(SWAP_SIDE_LEFT)] = leftHero;
    m_heroes[IDX(SWAP_SIDE_RIGHT)] = rightHero;
}

VA(0x00454482, 0x50)
void swapManager::Reset(void) {
    m_selectedSide = m_targetSide = SwapManagerSide(
        IDX(m_itemType = SwapManagerItemType(m_selectedSlot = m_targetSlot = IDX(SLOT_NONE)))
    );
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
    if (m_window == NULL)
        MemError();

    SetWinText(m_window, WINDOW_TEXT_ID);

    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    sprintf(gText, "port%04d.icn", m_heroes[IDX(SWAP_SIDE_LEFT)]->m_portrait);
    message.payload.widget.id = LEFT_PORTRAIT_WIDGET;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_portrait);
    message.payload.widget.id = RIGHT_PORTRAIT_WIDGET;
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    sprintf(
        gText,
        "%s meets %s",
        m_heroes[IDX(SWAP_SIDE_LEFT)]->m_name,
        m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_name
    );
    message.payload.widget.data.text = gText;
    message.payload.widget.id = TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (i32 side_6 = IDX(SWAP_SIDE_LEFT); side_6 < IDX(SWAP_SIDE_COUNT); ++side_6) {
        for (i32 skillSlot = 0; skillSlot < SECONDARY_SKILL_WIDGET_COUNT; ++skillSlot) {
            if (skillSlot < m_heroes[side_6]->m_secondarySkillCount) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = side_6 * SECONDARY_SKILL_WIDGET_COUNT + skillSlot
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = m_heroes[side_6]->GetNthSS(skillSlot);
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = side_6 * SECONDARY_SKILL_WIDGET_COUNT + skillSlot
                                            + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.payload.widget.data.text = gText;
                sprintf(
                    gText,
                    "%d",
                    m_heroes[side_6]->GetSSLevel(m_heroes[side_6]->GetNthSS(skillSlot))
                );
                m_window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.id = side_6 * SECONDARY_SKILL_WIDGET_COUNT + skillSlot
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
    m_priority = BaseManagerPriority(id);
    m_active = true;
    strcpy(m_name, "swapManager");
    return 0;
}

VA(0x004548be, 0x12d)
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

    if (m_selectedSide != SWAP_SIDE_NONE && m_selectedSlot != SLOT_NONE) {
        switch (m_selectedSide) {
            case SWAP_SIDE_LEFT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX_2 = m_selectedSlot * armySpacing_16 + leftArmyX_37 - 1;
                        selectorY_11 = armyY_19 - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX_2 =
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + leftArtifactX_1 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot <= ARTIFACT_FIRST_ROW_LAST ? 0
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
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing_11
                            + rightArtifactX_6 - 1;
                        selectorY_11 =
                            artifactY_7
                            + (m_selectedSlot <= ARTIFACT_FIRST_ROW_LAST ? 0
                                                                              : artifactSpacing_11)
                            - 1;
                        break;
                }
                break;
        }

        m_selectorIcon->FillToBuffer(
            selectorX_2,
            selectorY_11,
            m_itemType == SWAP_ITEM_ARMY ? ARMY_SELECTOR_FRAME : ARTIFACT_SELECTOR_FRAME,
            selectorFrame_17,
            0,
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

// @semantic: branch/code-shape residual.
VA(0x00454be3, 0xaf0)
i32 swapManager::Main(tag_message& message) {
    i32 closeRequested_5 = 0;
    i32 quickView = (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0;
    SwapManagerSide side;
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
                case IDX(COMMAND_SELECT):
                    if (quickView)
                        break;
                    if (message.payload.widget.id == IDX(CONTROL_CLOSE))
                        closeRequested_5 = 1;
                    break;

                case IDX(COMMAND_HOVER):
                case IDX(COMMAND_HELP):
                    switch (message.payload.widget.id) {
                        case CONTROL_LEFT_SKILL_FIRST:
                        case CONTROL_LEFT_SKILL_FIRST + 1:
                        case CONTROL_LEFT_SKILL_FIRST + 2:
                        case CONTROL_LEFT_SKILL_FIRST + 3:
                        case CONTROL_LEFT_SKILL_FIRST + 4:
                        case CONTROL_LEFT_SKILL_FIRST + 5:
                        case CONTROL_LEFT_SKILL_FIRST + 6:
                        case CONTROL_LEFT_SKILL_LAST:
                            side = SWAP_SIDE_LEFT;
                            slotIndex_8 = message.payload.widget.id - CONTROL_LEFT_SKILL_FIRST;
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
                            side = SWAP_SIDE_LEFT;
                            slotIndex_8 =
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
                            side = SWAP_SIDE_RIGHT;
                            slotIndex_8 =
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
                            side = SWAP_SIDE_RIGHT;
                            slotIndex_8 =
                                message.payload.widget.id - CONTROL_RIGHT_SKILL_LEVEL_FIRST;
                        showSecondarySkill:
                            if (slotIndex_8 >= m_heroes[IDX(side)]->m_secondarySkillCount)
                                break;
                            secondarySkill_1 = m_heroes[IDX(side)]->GetNthSS(slotIndex_8);
                            m_heroes[IDX(side)]->DoSSLevelDialog(secondarySkill_1, quickView);
                            break;

                        case CONTROL_LEFT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[IDX(SWAP_SIDE_LEFT)]->m_id, 1, 0);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(0, FADE_STEPS, NULL);
                            break;

                        case CONTROL_RIGHT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_id, 1, 0);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(0, FADE_STEPS, NULL);
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
                            if (!quickView
                                && m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                       == IDX(ARTIFACT_MAGIC_BOOK)) {
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
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                    == IDX(ARTIFACT_NONE))
                                    break;
                                m_heroes[IDX(SWAP_SIDE_LEFT)]->ViewArtifact(
                                    ArtifactType(
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                    ),
                                    VIEW_QUICK,
                                    m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifactExtra[artifactSlot_2]
                                );
                            } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot_2]
                                    != IDX(ARTIFACT_NONE)) {
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
                                    m_heroes[IDX(SWAP_SIDE_LEFT)]->ViewArtifact(
                                        ArtifactType(
                                            m_heroes[IDX(SWAP_SIDE_LEFT)]
                                                ->m_artifacts[artifactSlot_2]
                                        ),
                                        VIEW_FULL,
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]
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
                            if (!quickView
                                && m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                       == IDX(ARTIFACT_MAGIC_BOOK)) {
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
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                    == IDX(ARTIFACT_NONE))
                                    break;
                                m_heroes[IDX(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                    ArtifactType(
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                    ),
                                    VIEW_QUICK,
                                    m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifactExtra[artifactSlot_2]
                                );
                            } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot_2]
                                    != IDX(ARTIFACT_NONE)) {
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
                                    m_heroes[IDX(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                        ArtifactType(
                                            m_heroes[IDX(SWAP_SIDE_RIGHT)]
                                                ->m_artifacts[artifactSlot_2]
                                        ),
                                        VIEW_FULL,
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]
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
                            if (quickView) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST]
                                    != IDX(CREATURE_NONE)) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        1,
                                        1,
                                        m_heroes[IDX(SWAP_SIDE_LEFT)],
                                        NULL,
                                        &m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army,
                                        message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST
                                    );
                                }
                            } else if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST]
                                    != IDX(CREATURE_NONE)) {
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
                                            & IDX(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[m_targetSide]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == IDX(CREATURE_NONE)
                                               || m_heroes[IDX(m_selectedSide)]
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

                        case CONTROL_RIGHT_ARMY_FIRST:
                        case CONTROL_RIGHT_ARMY_FIRST + 1:
                        case CONTROL_RIGHT_ARMY_FIRST + 2:
                        case CONTROL_RIGHT_ARMY_FIRST + 3:
                        case CONTROL_RIGHT_ARMY_LAST:
                            if (quickView) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != IDX(CREATURE_NONE)) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        1,
                                        1,
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)],
                                        NULL,
                                        &m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army,
                                        message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST
                                    );
                                }
                            } else if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != IDX(CREATURE_NONE)) {
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
                                            & IDX(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[m_targetSide]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == IDX(CREATURE_NONE)
                                               || m_heroes[IDX(m_selectedSide)]
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

    if (closeRequested_5 == CLOSE_REQUESTED) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = SWAP_COMMAND_EXIT;
        return RESULT_CLOSE;
    }
    return RESULT_CONTINUE;
}

VA(0x004556d3, 0xa3)
void swapManager::ViewMon(void) {
    gpGame->ViewArmy(
        ARMY_VIEW_X,
        ARMY_VIEW_Y,
        m_heroes[IDX(m_selectedSide)]->m_army.m_creatureTypes[m_targetSlot],
        m_heroes[IDX(m_selectedSide)]->m_army.m_creatureCounts[m_targetSlot],
        NULL,
        m_heroes[IDX(m_selectedSide)]->m_army.GetNumArmies() == 1,
        1,
        0,
        m_heroes[IDX(m_selectedSide)],
        NULL,
        &m_heroes[IDX(m_selectedSide)]->m_army,
        m_targetSlot
    );
}

VA(0x00455776, 0x255)
void swapManager::SwapArtifacts(void) {
    i32 selectedArtifact = m_heroes[IDX(m_selectedSide)]->m_artifacts[m_selectedSlot];
    i32 targetArtifact_2 = m_heroes[m_targetSide]->m_artifacts[m_targetSlot];

    GiveTakeArtifactStat(m_heroes[IDX(m_selectedSide)], ArtifactType(selectedArtifact), 1);
    GiveTakeArtifactStat(m_heroes[m_targetSide], ArtifactType(targetArtifact_2), 1);
    m_heroes[IDX(m_selectedSide)]->m_artifacts[m_selectedSlot] = targetArtifact_2;
    m_heroes[m_targetSide]->m_artifacts[m_targetSlot] = selectedArtifact;

    i8 extra = m_heroes[IDX(m_selectedSide)]->m_artifactExtra[m_selectedSlot];
    m_heroes[IDX(m_selectedSide)]->m_artifactExtra[m_selectedSlot] =
        m_heroes[m_targetSide]->m_artifactExtra[m_targetSlot];
    m_heroes[m_targetSide]->m_artifactExtra[m_targetSlot] = extra;

    GiveTakeArtifactStat(m_heroes[IDX(m_selectedSide)], ArtifactType(targetArtifact_2), 0);
    GiveTakeArtifactStat(m_heroes[m_targetSide], ArtifactType(selectedArtifact), 0);
    m_heroes[IDX(m_selectedSide)]->CheckAnduranPieces(1);
    m_heroes[m_targetSide]->CheckAnduranPieces(1);

    if (selectedArtifact == IDX(ARTIFACT_SPADE_NECROMANCY)
        || targetArtifact_2 == IDX(ARTIFACT_SPADE_NECROMANCY)) {
        tag_message message_1;
        message_1.type = MESSAGE_WIDGET;
        for (i32 side = IDX(SWAP_SIDE_LEFT); side < IDX(SWAP_SIDE_COUNT); ++side) {
            for (i32 skillSlot = 0; skillSlot < SECONDARY_SKILL_WIDGET_COUNT; ++skillSlot) {
                if (skillSlot < m_heroes[side]->m_secondarySkillCount) {
                    message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                    message_1.payload.widget.id = side * SECONDARY_SKILL_WIDGET_COUNT
                                                  + skillSlot + CONTROL_LEFT_SKILL_LEVEL_FIRST;
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
        if (m_heroes[IDX(m_selectedSide)]->m_army.m_creatureTypes[slot_1] != IDX(CREATURE_NONE)
            && m_heroes[IDX(m_selectedSide)]->m_army.m_creatureCounts[slot_1] > 0)
            ++selectedArmyCount;
    }

    armyGroup* selectedArmy = &m_heroes[IDX(m_selectedSide)]->m_army;
    armyGroup* targetArmy = &m_heroes[m_targetSide]->m_army;
    if (OD_STEER(selectedArmy->m_creatureTypes[m_selectedSlot])
        == targetArmy->m_creatureTypes[m_targetSlot]) {
        if (selectedArmy->GetNumArmies() == 1)
            return;
        targetArmy->m_creatureCounts[m_targetSlot] =
            OD_STEER(selectedArmy->m_creatureCounts[m_selectedSlot])
            + targetArmy->m_creatureCounts[m_targetSlot];
        selectedArmy->m_creatureTypes[m_selectedSlot] = IDX(CREATURE_NONE);
        selectedArmy->m_creatureCounts[m_selectedSlot] = 0;
        return;
    }
    if (OD_STEER(selectedArmy) != targetArmy && selectedArmy->GetNumArmies() == 1
        && targetArmy->m_creatureTypes[m_targetSlot] == IDX(CREATURE_NONE))
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

    for (slot = 0; slot < PRIMARY_SKILL_COUNT; ++slot) {
        message_1.payload.widget.id = slot + LEFT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_LEFT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message_1);
        message_1.payload.widget.id = slot + RIGHT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_RIGHT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + CONTROL_LEFT_ARMY_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == IDX(CREATURE_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value =
                m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + LEFT_ARMY_COUNT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == IDX(CREATURE_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts[slot]);
            message_1.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + CONTROL_RIGHT_ARMY_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == IDX(CREATURE_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value =
                m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + RIGHT_ARMY_COUNT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == IDX(CREATURE_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts[slot]);
            message_1.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + CONTROL_LEFT_ARTIFACT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[slot] == IDX(ARTIFACT_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value = m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[slot];
        }
        m_window->BroadcastMessage(message_1);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message_1.payload.widget.id = slot + CONTROL_RIGHT_ARTIFACT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[slot] == IDX(ARTIFACT_NONE)) {
            message_1.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
        } else {
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message_1.payload.widget.data.value = EMPTY_ITEM_VALUE;
            m_window->BroadcastMessage(message_1);
            message_1.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message_1.payload.widget.data.value = m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[slot];
        }
        m_window->BroadcastMessage(message_1);
    }
}

VA(0x00455fbd, 0x388)
void swapManager::SplitMons(void) {
    i16 unusedAmountControl_29 = SPLIT_AMOUNT_CONTROL;
    i32 unusedState = 0;
    armyGroup* selectedArmy = &m_heroes[IDX(m_selectedSide)]->m_army;
    armyGroup* targetArmy = &m_heroes[m_targetSide]->m_army;
    i32 emptySlot;
    unusedState = 0;
    i16 unusedTextControl_2 = SPLIT_TEXT_CONTROL;

    gpTownManager->m_heroWindow1 =
        new heroWindow(SPLIT_WINDOW_X, SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == NULL)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = selectedArmy->m_creatureCounts[m_selectedSlot];

    tag_message message;
    message.type = MESSAGE_WIDGET;
    if (m_selectedSide == IDX(m_targetSide)) {
        sprintf(gText, "Move how many troops?");
    } else {
        sprintf(
            gText,
            "Move how many %s troops from %s to %s?",
            gArmyNames[selectedArmy->m_creatureTypes[m_selectedSlot]],
            m_heroes[IDX(m_selectedSide)]->m_name,
            m_heroes[m_targetSide]->m_name
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
        if (selectedArmy->m_creatureTypes[m_selectedSlot]
            == targetArmy->m_creatureTypes[m_targetSlot]) {
            selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
            targetArmy->m_creatureCounts[m_targetSlot] += gpTownManager->m_splitAmount;
            if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
                selectedArmy->m_creatureTypes[m_selectedSlot] = IDX(CREATURE_NONE);
            return;
        }
        if (targetArmy->m_creatureTypes[m_targetSlot] != IDX(CREATURE_NONE)) {
            for (emptySlot = 0; emptySlot < ARMY_GROUP_SLOT_COUNT; ++emptySlot) {
                if (targetArmy->m_creatureTypes[emptySlot] == IDX(CREATURE_NONE))
                    break;
            }
            if (emptySlot < ARMY_GROUP_SLOT_COUNT)
                m_targetSlot = emptySlot;
        }
        targetArmy->m_creatureTypes[m_targetSlot] = selectedArmy->m_creatureTypes[m_selectedSlot];
        targetArmy->m_creatureCounts[m_targetSlot] = gpTownManager->m_splitAmount;
        selectedArmy->m_creatureCounts[m_selectedSlot] -= gpTownManager->m_splitAmount;
        if (selectedArmy->m_creatureCounts[m_selectedSlot] == 0)
            selectedArmy->m_creatureTypes[m_selectedSlot] = IDX(CREATURE_NONE);
    }
}


VTBL(swapManager, 0x004eb650);
