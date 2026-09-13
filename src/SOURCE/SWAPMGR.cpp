#include <va.h>
#include <SOURCE/armyGroup.h>
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
#include <BASE/dialog.h>
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
    SPLIT_CONFIRM             = DIALOG_BUTTON_2,
    CONTROL_CLOSE             = DIALOG_BUTTON_0,
    WINDOW_TEXT_ID            = 0x15,
    SPLIT_MODIFIER_MASK       = 3,
    TITLE_WIDGET              = 0x4d,
    ADVENTURE_WIDGET_FIRST    = 1,
    ADVENTURE_WIDGET_LAST     = 6,
    MONO_ICON_SKIP            = 2,
    MONO_ICON_DEFAULT         = -1,
    MANAGER_MESSAGE           = 0x100,
    SLOT_NONE                 = -1,
    ARTIFACT_COLUMN_COUNT     = 7,
    FADE_STEPS                = 8,
    VIEW_FULL                 = 0,
    VIEW_QUICK                = 1,
    CLOSE_REQUESTED           = 1,
    SELECTOR_WIDTH            = 0x2e,
    SELECTOR_HEIGHT           = 0x2e,
    ARMY_SELECTOR_FRAME       = 3,
    ARTIFACT_SELECTOR_FRAME   = 2,
    ARTIFACT_FIRST_ROW_LAST   = 6,
    LEFT_PRIMARY_SKILL_FIRST  = 0x43,
    RIGHT_PRIMARY_SKILL_FIRST = 0x48,
    LEFT_ARMY_COUNT_FIRST     = 0x74,
    RIGHT_ARMY_COUNT_FIRST    = 0x79,
    ARMY_VIEW_X               = 0x77,
    ARMY_VIEW_Y               = 0x14,
    SPLIT_WINDOW_X            = 0xb1,
    SPLIT_WINDOW_Y            = 0x14,
    SPLIT_TEXT_CONTROL        = 1,
H2_ENUM_END(SwapManagerConstant)

VA(0x004a2260, 0x79)
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

VA(0x004a22d9, 0x33)
swapManager::swapManager(hero* leftHero, hero* rightHero) {
    m_heroes[IDX(SWAP_SIDE_LEFT)] = leftHero;
    m_heroes[IDX(SWAP_SIDE_RIGHT)] = rightHero;
}

VA(0x004a230c, 0x3d)
void swapManager::Reset(void) {
    H2_ENUM_ASSIGN_CHAIN_5(
        m_selectedSide, m_targetSide, m_itemType, m_selectedSlot, m_targetSlot, SLOT_NONE
    );
}

VA(0x004a2349, 0x25)
i32 swapManager::DrawSwapWin(void) {
    m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    gpWindowManager->UpdateScreen();
    return 0;
}

VA(0x004a236e, 0x3e2)
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
    sprintf(gText, "port%04d.icn", IDX(m_heroes[IDX(SWAP_SIDE_LEFT)]->m_portrait));
    message.payload.widget.id = CONTROL_LEFT_HERO;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", IDX(m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_portrait));
    message.payload.widget.id = CONTROL_RIGHT_HERO;
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    sprintf(
        gText,
         localization::Tr("hero.meeting.title"),
        m_heroes[IDX(SWAP_SIDE_LEFT)]->m_name,
        m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_name
    );
    message.payload.widget.data.text = gText;
    message.payload.widget.id = TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (SwapManagerSide swapSide = SWAP_SIDE_LEFT; swapSide < SWAP_SIDE_COUNT; ++swapSide) {
        for (skillWidget = 0; skillWidget < HERO_SECONDARY_SKILL_CAPACITY; ++skillWidget) {
            if (skillWidget < m_heroes[IDX(swapSide)]->m_secondarySkillCount) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = IDX(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = IDX(m_heroes[IDX(swapSide)]->GetNthSS(skillWidget));
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = IDX(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.payload.widget.data.text = gText;
                sprintf(
                    gText,
                    "%d",
                    m_heroes[IDX(swapSide)]->GetSSLevel(m_heroes[IDX(swapSide)]->GetNthSS(skillWidget))
                );
                m_window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.id = IDX(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
                m_window->BroadcastMessage(message);
            }
        }
    }

    SET_ADVENTURE_BUTTON_FLAGS(
        message,
        gpAdvManager->m_adventureWindow,
        WIDGET_COMMAND_CLEAR_FLAGS
    );

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

VA(0x004a2750, 0x11f)
void swapManager::Close(void) {
    gpResourceManager->Dispose(m_selectorIcon);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = false;
    gpAdvManager->Activate();

    tag_message message;
    SET_ADVENTURE_BUTTON_FLAGS(message, gpAdvManager->m_adventureWindow, WIDGET_COMMAND_SET_FLAGS);
}

#if H2_RETAIL_COMPILER
#define armySpacing armySpacing_2
#define armyY armyY_9
#define artifactSpacing artifactSpacing_11
#define artifactY artifactY_7
#define leftArmyX leftArmyX_3
#define leftArtifactX leftArtifactX_7
#define rightArmyX rightArmyX_3
#define rightArtifactX rightArtifactX_6
#define selectorFrame selectorFrame_6
#define selectorX selectorX_2
#define selectorY selectorY_11
#endif
VA(0x004a286f, 0x185)
void swapManager::DrawSelector(void) {
    const char selectorFrame = 10;
    const i16 leftArmyX = 37;
    const i16 rightArmyX = 382;
    const i16 armyY = 268;
    const i16 armySpacing = 45;
    const i16 leftArtifactX = 24;
    const i16 rightArtifactX = 368;
    const i16 artifactY = 348;
    const i16 artifactSpacing = 36;
    i32 selectorX = 0;
    i32 selectorY = 0;

    if (m_selectedSide != SWAP_SIDE_NONE && m_selectedSlot != SLOT_NONE) {
        switch (m_selectedSide) {
            case SWAP_SIDE_LEFT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX = m_selectedSlot * armySpacing + leftArmyX - 1;
                        selectorY = armyY - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX =
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing
                            + leftArtifactX - 1;
                        selectorY =
                            artifactY
                            + (m_selectedSlot > ARTIFACT_FIRST_ROW_LAST ? artifactSpacing : 0)
                            - 1;
                        break;
                }
                break;
            case SWAP_SIDE_RIGHT:
                switch (m_itemType) {
                    case SWAP_ITEM_ARMY:
                        selectorX = m_selectedSlot * armySpacing + rightArmyX - 1;
                        selectorY = armyY - 1;
                        break;
                    case SWAP_ITEM_ARTIFACT:
                        selectorX =
                            (m_selectedSlot % ARTIFACT_COLUMN_COUNT) * artifactSpacing
                            + rightArtifactX - 1;
                        selectorY =
                            artifactY
                            + (m_selectedSlot > ARTIFACT_FIRST_ROW_LAST ? artifactSpacing : 0)
                            - 1;
                        break;
                }
                break;
        }

        m_selectorIcon->FillToBuffer(
            selectorX,
            selectorY,
            m_itemType == SWAP_ITEM_ARMY ? ARMY_SELECTOR_FRAME : ARTIFACT_SELECTOR_FRAME,
            selectorFrame,
            ICON_DRAW_NORMAL,
            NULL
        );
        gpWindowManager->UpdateScreenRegion(
            selectorX,
            selectorY,
            SELECTOR_WIDTH,
            SELECTOR_HEIGHT
        );
    }
}
#if H2_RETAIL_COMPILER
#undef armySpacing
#undef armyY
#undef artifactSpacing
#undef artifactY
#undef leftArmyX
#undef leftArtifactX
#undef rightArmyX
#undef rightArtifactX
#undef selectorFrame
#undef selectorX
#undef selectorY
#endif

#if H2_RETAIL_COMPILER
#define artifactSlot artifactSlot_2
#define closeRequested closeRequested_5
#define quickView quickView_9
#define secondarySkill secondarySkill_1
#define side side_6
#define slotIndex slotIndex_2
#endif
VA(0x004a29f4, 0x9c2)
MessageDispatchResult swapManager::Main(tag_message& message) {
    b32 closeRequested = false;
    b32 quickView;
    SwapManagerSide side;
    i32 slotIndex;
    i32 artifactSlot;
    HeroSecondarySkill secondarySkill;

    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
        quickView = true;
    else
        quickView = false;

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
                case WIDGET_NOTIFY_DESELECT:
                    if (quickView)
                        break;
                    if (message.payload.widget.id == CONTROL_CLOSE)
                        closeRequested = true;
                    break;

                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK:
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
                            slotIndex = message.payload.widget.id - CONTROL_LEFT_SKILL_FIRST;
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
                            slotIndex =
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
                            slotIndex =
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
                            slotIndex =
                                message.payload.widget.id - CONTROL_RIGHT_SKILL_LEVEL_FIRST;
                        showSecondarySkill:
                            if (slotIndex >= m_heroes[IDX(side)]->m_secondarySkillCount)
                                break;
                            secondarySkill = m_heroes[IDX(side)]->GetNthSS(slotIndex);
                            m_heroes[IDX(side)]->DoSSLevelDialog(secondarySkill, quickView);
                            break;

                        case CONTROL_LEFT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[IDX(SWAP_SIDE_LEFT)]->m_id, true, false);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(FADE_IN, FADE_STEPS, NULL);
                            break;

                        case CONTROL_RIGHT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_id, true, false);
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
                            artifactSlot =
                                message.payload.widget.id - CONTROL_LEFT_ARTIFACT_FIRST;
                            if (!quickView
                                && m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(

                                    localization::Tr("swap.artifact.cannot_trade"),
                                    NORMAL_DIALOG_INFO
                                );
                                break;
                            }
                            if (quickView) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[IDX(SWAP_SIDE_LEFT)]->ViewArtifact(
                                    m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot],
                                    VIEW_QUICK,
                                    m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifactExtra[artifactSlot]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_LEFT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_LEFT;
                                m_targetSlot = artifactSlot;
                                if (m_selectedSide == SWAP_SIDE_LEFT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[IDX(SWAP_SIDE_LEFT)]->ViewArtifact(
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]
                                            ->m_artifacts[artifactSlot],
                                        VIEW_FULL,
                                        m_heroes[IDX(SWAP_SIDE_LEFT)]
                                            ->m_artifactExtra[artifactSlot]
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
                            artifactSlot =
                                message.payload.widget.id - CONTROL_RIGHT_ARTIFACT_FIRST;
                            if (!quickView
                                && m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(

                                    localization::Tr("swap.artifact.cannot_trade"),
                                    NORMAL_DIALOG_INFO
                                );
                                break;
                            }
                            if (quickView) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[IDX(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                    m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot],
                                    VIEW_QUICK,
                                    m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifactExtra[artifactSlot]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
                                    != ARTIFACT_NONE) {
                                    m_selectedSide = SWAP_SIDE_RIGHT;
                                    m_targetSide = SWAP_SIDE_NONE;
                                    m_itemType = SWAP_ITEM_ARTIFACT;
                                    m_selectedSlot = artifactSlot;
                                    m_targetSlot = SLOT_NONE;
                                } else {
                                    Reset();
                                }
                            } else {
                                m_targetSide = SWAP_SIDE_RIGHT;
                                m_targetSlot = artifactSlot;
                                if (m_selectedSide == SWAP_SIDE_RIGHT
                                    && m_selectedSlot == m_targetSlot) {
                                    m_heroes[IDX(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]
                                            ->m_artifacts[artifactSlot],
                                        VIEW_FULL,
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)]
                                            ->m_artifactExtra[artifactSlot]
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
                                    != CREATURE_NONE) {
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
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[IDX(SWAP_SIDE_LEFT)],
                                        NULL,
                                        &m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army,
                                        message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
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
                                            & IDX(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[IDX(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[IDX(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[IDX(m_selectedSide)]
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
                            if (quickView) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
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
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[IDX(SWAP_SIDE_RIGHT)],
                                        NULL,
                                        &m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army,
                                        message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
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
                                            & IDX(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[IDX(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[IDX(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[IDX(m_selectedSide)]
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

    if (closeRequested == CLOSE_REQUESTED) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_RETURN_RESULT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}
#if H2_RETAIL_COMPILER
#undef artifactSlot
#undef closeRequested
#undef quickView
#undef secondarySkill
#undef side
#undef slotIndex
#endif

VA(0x004a33b6, 0x98)
void swapManager::ViewMon(void) {
    gpGame->ViewArmy(
        ARMY_VIEW_X,
        ARMY_VIEW_Y,
        m_heroes[IDX(m_selectedSide)]->m_army.m_creatureTypes[m_targetSlot],
        m_heroes[IDX(m_selectedSide)]->m_army.m_creatureCounts[m_targetSlot],
        NULL,
        m_heroes[IDX(m_selectedSide)]->m_army.GetNumArmies() == 1,
        ARMY_FACING_RIGHT,
        0,
        m_heroes[IDX(m_selectedSide)],
        NULL,
        &m_heroes[IDX(m_selectedSide)]->m_army,
        m_targetSlot
    );
}

#if H2_RETAIL_COMPILER
#define targetArtifact targetArtifact_2
#endif
VA(0x004a344e, 0x24b)
void swapManager::SwapArtifacts(void) {
    H2_ENUM_STORAGE(ArtifactType, i32) selectedArtifact =
        m_heroes[IDX(m_selectedSide)]->m_artifacts[m_selectedSlot];
    H2_ENUM_STORAGE(ArtifactType, i32) targetArtifact =
        m_heroes[IDX(m_targetSide)]->m_artifacts[m_targetSlot];

    GiveTakeArtifactStat(m_heroes[IDX(m_selectedSide)], selectedArtifact, true);
    GiveTakeArtifactStat(m_heroes[IDX(m_targetSide)], targetArtifact, true);
    m_heroes[IDX(m_selectedSide)]->m_artifacts[m_selectedSlot] = targetArtifact;
    m_heroes[IDX(m_targetSide)]->m_artifacts[m_targetSlot] = selectedArtifact;

    i8 extra = m_heroes[IDX(m_selectedSide)]->m_artifactExtra[m_selectedSlot];
    m_heroes[IDX(m_selectedSide)]->m_artifactExtra[m_selectedSlot] =
        m_heroes[IDX(m_targetSide)]->m_artifactExtra[m_targetSlot];
    m_heroes[IDX(m_targetSide)]->m_artifactExtra[m_targetSlot] = extra;

    GiveTakeArtifactStat(m_heroes[IDX(m_selectedSide)], targetArtifact, false);
    GiveTakeArtifactStat(m_heroes[IDX(m_targetSide)], selectedArtifact, false);
    m_heroes[IDX(m_selectedSide)]->CheckAnduranPieces(true);
    m_heroes[IDX(m_targetSide)]->CheckAnduranPieces(true);

    if (selectedArtifact == ARTIFACT_SPADE_NECROMANCY
        || targetArtifact == ARTIFACT_SPADE_NECROMANCY) {
        tag_message message;
        i32 slotSkill;
        message.type = MESSAGE_WIDGET;
        for (SwapManagerSide side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (slotSkill = 0; slotSkill < HERO_SECONDARY_SKILL_CAPACITY; ++slotSkill) {
                if (slotSkill < m_heroes[IDX(side)]->m_secondarySkillCount) {
                    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                    message.payload.widget.id = IDX(side) * HERO_SECONDARY_SKILL_CAPACITY
                                                  + slotSkill + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                    message.payload.widget.data.text = gText;
                    sprintf(
                        gText,
                        "%d",
                        m_heroes[IDX(side)]->GetSSLevel(m_heroes[IDX(side)]->GetNthSS(slotSkill))
                    );
                    m_window->BroadcastMessage(message);
                }
            }
        }
    }
}
#if H2_RETAIL_COMPILER
#undef targetArtifact
#endif

#if H2_RETAIL_COMPILER
#define selectedArmy selectedArmy_1
#define slot slot_1
#define targetArmy targetArmy_6
#endif
VA(0x004a3699, 0x14e)
void swapManager::SwapMons(void) {
    i32 H2_UNUSED(selectedArmyCount) = 0;
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (ARMY_GROUP_HAS_POSITIVE_STACK(m_heroes[IDX(m_selectedSide)]->m_army, slot))
            ++selectedArmyCount;
    }

    armyGroup* selectedArmy = &m_heroes[IDX(m_selectedSide)]->m_army;
    armyGroup* targetArmy = &m_heroes[IDX(m_targetSide)]->m_army;
    if (targetArmy->m_creatureTypes[m_targetSlot]
        == selectedArmy->m_creatureTypes[m_selectedSlot]) {
        if (selectedArmy->GetNumArmies() == 1)
            return;
        targetArmy->m_creatureCounts[m_targetSlot] +=
            selectedArmy->m_creatureCounts[m_selectedSlot];
        selectedArmy->m_creatureTypes[m_selectedSlot] = CREATURE_NONE;
        selectedArmy->m_creatureCounts[m_selectedSlot] = 0;
        return;
    }
    if (selectedArmy != targetArmy && selectedArmy->GetNumArmies() == 1
        && targetArmy->m_creatureTypes[m_targetSlot] == CREATURE_NONE)
        return;
    selectedArmy->Swap(m_selectedSlot, targetArmy, m_targetSlot);
}
#if H2_RETAIL_COMPILER
#undef selectedArmy
#undef slot
#undef targetArmy
#endif

VA(0x004a37e7, 0x449)
void swapManager::Update(void) {
    i32 slot;
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;

    for (slot = 0; slot < HERO_PRIMARY_STAT_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_LEFT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
        message.payload.widget.id = slot + RIGHT_PRIMARY_SKILL_FIRST;
        sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_RIGHT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARMY_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                IDX(m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_ARMY_COUNT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARMY_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                IDX(m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + RIGHT_ARMY_COUNT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            sprintf(gText, "%d", m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARTIFACT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                IDX(m_heroes[IDX(SWAP_SIDE_LEFT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARTIFACT_FIRST;
        if (m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = IDX(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                IDX(m_heroes[IDX(SWAP_SIDE_RIGHT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }
}

VA(0x004a3c30, 0x381)
void swapManager::SplitMons(void) {
    i16 H2_UNUSED(textControl);
    armyGroup* targetTroops;
    armyGroup* selectedArmy;
    i32 openSlot;
    i16 H2_UNUSED(unusedAmountControl);
    tag_message message;
    i32 H2_UNUSED(dlgState);

    unusedAmountControl = TOWN_SPLIT_AMOUNT_CONTROL;
    dlgState = 0;
    selectedArmy = &m_heroes[IDX(m_selectedSide)]->m_army;
    targetTroops = &m_heroes[IDX(m_targetSide)]->m_army;
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
             localization::Tr("hero.army.split.prompt")
        );
    } else {
        sprintf(
            gText,
             localization::Tr("hero.army.split.between_heroes"),
            gArmyNamesPlural[IDX(selectedArmy->m_creatureTypes[m_selectedSlot])],
            m_heroes[IDX(m_selectedSide)]->m_name,
            m_heroes[IDX(m_targetSide)]->m_name
        );
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SPLIT_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.payload.widget.id = TOWN_SPLIT_AMOUNT_CONTROL;
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

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(swapManager, 0x004ea86c)
