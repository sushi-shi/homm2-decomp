#include <Ints.h>
#include <BASE/Utf8.h>
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
#include <IRONFIST/heroes.h>
#include <SOURCE/hero.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/swapManager.h>
#include <SOURCE/townManager.h>
#include <SOURCE/Localization.h>
#include <BASE/dialog.h>
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
} SwapManagerConstant;

swapManager::swapManager(void) {
    m_window = NULL;
    m_selectorIcon = NULL;
    m_selectedSide = SWAP_SIDE_NONE;
    m_targetSide = SWAP_SIDE_NONE;
    m_itemType = SWAP_ITEM_NONE;
    m_selectedSlot = SLOT_NONE;
    m_targetSlot = SLOT_NONE;
    m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)] = NULL;
    m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)] = NULL;
}

swapManager::swapManager(hero* leftHero, hero* rightHero) {
    m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)] = leftHero;
    m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)] = rightHero;
}

void swapManager::Reset(void) {
    m_targetSlot = SLOT_NONE;
    m_selectedSlot = SLOT_NONE;
    m_itemType = SWAP_ITEM_NONE;
    m_targetSide = SWAP_SIDE_NONE;
    m_selectedSide = SWAP_SIDE_NONE;
}

i32 swapManager::DrawSwapWin(void) {
    m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
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
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_portrait));
    message.payload.widget.id = CONTROL_LEFT_HERO;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);

    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "port%04d.icn", H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_portrait));
    message.payload.widget.id = CONTROL_RIGHT_HERO;
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    utf8::Format(
        gText, GLOBAL_TEXT_BUFFER_SIZE,
        localization::Tr("hero.meeting.title"),
        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_name,
        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_name
    );
    message.payload.widget.data.text = gText;
    message.payload.widget.id = TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (SwapManagerSide swapSide = SWAP_SIDE_LEFT; swapSide < SWAP_SIDE_COUNT; ++swapSide) {
        for (skillWidget = 0; skillWidget < HERO_SECONDARY_SKILL_CAPACITY; ++skillWidget) {
            if (skillWidget < m_heroes[H2EnumIndex(swapSide)]->m_secondarySkillCount) {
                message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
                message.payload.widget.id = H2EnumIndex(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                // The Cyborg Wisdom slot draws the Cybernetics small icon.
                if (m_heroes[H2EnumIndex(swapSide)]->m_cursorType == FACTION_CYBORG
                    && m_heroes[H2EnumIndex(swapSide)]->GetNthSS(skillWidget) == HERO_SKILL_WISDOM)
                    message.payload.widget.data.value = CYBERNETICS_MINI_SKILL_FRAME;
                else
                    message.payload.widget.data.value = H2EnumIndex(m_heroes[H2EnumIndex(swapSide)]->GetNthSS(skillWidget));
                m_window->BroadcastMessage(message);

                message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                message.payload.widget.id = H2EnumIndex(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.payload.widget.data.text = gText;
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    "%d",
                    m_heroes[H2EnumIndex(swapSide)]->GetSSLevel(m_heroes[H2EnumIndex(swapSide)]->GetNthSS(skillWidget))
                );
                m_window->BroadcastMessage(message);
            } else {
                message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
                message.payload.widget.id = H2EnumIndex(swapSide) * HERO_SECONDARY_SKILL_CAPACITY + skillWidget
                                            + CONTROL_LEFT_SKILL_FIRST;
                message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
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
    platform::ChangeMenu(hmnuAdv);
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
    SET_ADVENTURE_BUTTON_FLAGS(message, gpAdvManager->m_adventureWindow, WIDGET_COMMAND_SET_FLAGS);
}

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

MessageDispatchResult swapManager::Main(tag_message& message) {
    b32 closeRequested = false;
    b32 quickView;
    SwapManagerSide side;
    i32 slotIndex;
    i32 artifactSlot;
    HeroSecondarySkill secondarySkill;

    if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
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
                            if (slotIndex >= m_heroes[H2EnumIndex(side)]->m_secondarySkillCount)
                                break;
                            secondarySkill = m_heroes[H2EnumIndex(side)]->GetNthSS(slotIndex);
                            m_heroes[H2EnumIndex(side)]->DoSSLevelDialog(secondarySkill, quickView);
                            break;

                        case CONTROL_LEFT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_id, 1, 0);
                            gpAdvManager->RedrawAdvScreen(1, 0);
                            Update();
                            DrawSwapWin();
                            Reset();
                            gpWindowManager->FadeScreen(FADE_IN, FADE_STEPS, NULL);
                            break;

                        case CONTROL_RIGHT_HERO:
                            if (quickView)
                                break;
                            HeroView(m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_id, 1, 0);
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
                                && m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(localization::Tr("swap.artifact.cannot_trade"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_VALUE);
                                break;
                            }
                            if (quickView) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->ViewArtifact(
                                    m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot],
                                    VIEW_QUICK,
                                    m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifactExtra[artifactSlot]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[artifactSlot]
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
                                    m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->ViewArtifact(
                                        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]
                                            ->m_artifacts[artifactSlot],
                                        VIEW_FULL,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]
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
                                && m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
                                       == ARTIFACT_MAGIC_BOOK) {
                                NormalDialog(localization::Tr("swap.artifact.cannot_trade"), NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_RESOURCE, 0, NORMAL_DIALOG_NO_VALUE);
                                break;
                            }
                            if (quickView) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
                                    == ARTIFACT_NONE)
                                    break;
                                m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                    m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot],
                                    VIEW_QUICK,
                                    m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifactExtra[artifactSlot]
                                );
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARTIFACT) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[artifactSlot]
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
                                    m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->ViewArtifact(
                                        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]
                                            ->m_artifacts[artifactSlot],
                                        VIEW_FULL,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]
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
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_LEFT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)],
                                        NULL,
                                        &m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army,
                                        message.payload.widget.id - CONTROL_LEFT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes
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
                                            & H2EnumIndex(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[H2EnumIndex(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[H2EnumIndex(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[H2EnumIndex(m_selectedSide)]
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
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                        [message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST]
                                    != CREATURE_NONE) {
                                    gpGame->ViewArmy(
                                        ARMY_VIEW_X,
                                        ARMY_VIEW_Y,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts
                                            [message.payload.widget.id
                                             - CONTROL_RIGHT_ARMY_FIRST],
                                        NULL,
                                        0,
                                        ARMY_FACING_RIGHT,
                                        1,
                                        m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)],
                                        NULL,
                                        &m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army,
                                        message.payload.widget.id - CONTROL_RIGHT_ARMY_FIRST
                                    );
                                }
                                break;
                            }
                            if (m_itemType != SWAP_ITEM_ARMY) {
                                if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes
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
                                            & H2EnumIndex(SPLIT_MODIFIER_MASK))
                                               != 0
                                           && (m_heroes[H2EnumIndex(m_targetSide)]
                                                       ->m_army.m_creatureTypes[m_targetSlot]
                                                   == CREATURE_NONE
                                               || m_heroes[H2EnumIndex(m_targetSide)]
                                                          ->m_army.m_creatureTypes[m_targetSlot]
                                                      == m_heroes[H2EnumIndex(m_selectedSide)]
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

void swapManager::ViewMon(void) {
    gpGame->ViewArmy(
        ARMY_VIEW_X,
        ARMY_VIEW_Y,
        m_heroes[H2EnumIndex(m_selectedSide)]->m_army.m_creatureTypes[m_targetSlot],
        m_heroes[H2EnumIndex(m_selectedSide)]->m_army.m_creatureCounts[m_targetSlot],
        NULL,
        m_heroes[H2EnumIndex(m_selectedSide)]->m_army.GetNumArmies() == 1,
        ARMY_FACING_RIGHT,
        0,
        m_heroes[H2EnumIndex(m_selectedSide)],
        NULL,
        &m_heroes[H2EnumIndex(m_selectedSide)]->m_army,
        m_targetSlot
    );
}

void swapManager::SwapArtifacts(void) {
    H2EnumStorage<ArtifactType, i32> selectedArtifact =
        m_heroes[H2EnumIndex(m_selectedSide)]->m_artifacts[m_selectedSlot];
    H2EnumStorage<ArtifactType, i32> targetArtifact =
        m_heroes[H2EnumIndex(m_targetSide)]->m_artifacts[m_targetSlot];

    GiveTakeArtifactStat(m_heroes[H2EnumIndex(m_selectedSide)], selectedArtifact, 1);
    GiveTakeArtifactStat(m_heroes[H2EnumIndex(m_targetSide)], targetArtifact, 1);
    m_heroes[H2EnumIndex(m_selectedSide)]->m_artifacts[m_selectedSlot] = targetArtifact;
    m_heroes[H2EnumIndex(m_targetSide)]->m_artifacts[m_targetSlot] = selectedArtifact;

    i8 extra = m_heroes[H2EnumIndex(m_selectedSide)]->m_artifactExtra[m_selectedSlot];
    m_heroes[H2EnumIndex(m_selectedSide)]->m_artifactExtra[m_selectedSlot] =
        m_heroes[H2EnumIndex(m_targetSide)]->m_artifactExtra[m_targetSlot];
    m_heroes[H2EnumIndex(m_targetSide)]->m_artifactExtra[m_targetSlot] = extra;

    GiveTakeArtifactStat(m_heroes[H2EnumIndex(m_selectedSide)], targetArtifact, 0);
    GiveTakeArtifactStat(m_heroes[H2EnumIndex(m_targetSide)], selectedArtifact, 0);
    m_heroes[H2EnumIndex(m_selectedSide)]->CheckAnduranPieces(1);
    m_heroes[H2EnumIndex(m_targetSide)]->CheckAnduranPieces(1);

    if (selectedArtifact == ARTIFACT_SPADE_NECROMANCY
        || targetArtifact == ARTIFACT_SPADE_NECROMANCY) {
        tag_message message;
        i32 slotSkill;
        message.type = MESSAGE_WIDGET;
        for (SwapManagerSide side = SWAP_SIDE_LEFT; side < SWAP_SIDE_COUNT; ++side) {
            for (slotSkill = 0; slotSkill < HERO_SECONDARY_SKILL_CAPACITY; ++slotSkill) {
                if (slotSkill < m_heroes[H2EnumIndex(side)]->m_secondarySkillCount) {
                    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
                    message.payload.widget.id = H2EnumIndex(side) * HERO_SECONDARY_SKILL_CAPACITY
                                                  + slotSkill + CONTROL_LEFT_SKILL_LEVEL_FIRST;
                    message.payload.widget.data.text = gText;
                    utf8::Format(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        "%d",
                        m_heroes[H2EnumIndex(side)]->GetSSLevel(m_heroes[H2EnumIndex(side)]->GetNthSS(slotSkill))
                    );
                    m_window->BroadcastMessage(message);
                }
            }
        }
    }
}

void swapManager::SwapMons(void) {
    armyGroup* selectedArmy = &m_heroes[H2EnumIndex(m_selectedSide)]->m_army;
    armyGroup* targetArmy = &m_heroes[H2EnumIndex(m_targetSide)]->m_army;
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

void swapManager::Update(void) {
    i32 slot;
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;

    for (slot = 0; slot < HERO_PRIMARY_STAT_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_PRIMARY_SKILL_FIRST;
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
        message.payload.widget.id = slot + RIGHT_PRIMARY_SKILL_FIRST;
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->Stats(HeroPrimaryStat(slot)));
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARMY_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + LEFT_ARMY_COUNT_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARMY_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + RIGHT_ARMY_COUNT_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
            utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_army.m_creatureCounts[slot]);
            message.payload.widget.data.text = gText;
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_LEFT_ARTIFACT_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_LEFT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }

    for (slot = 0; slot < HERO_ARTIFACT_SLOT_COUNT; ++slot) {
        message.payload.widget.id = slot + CONTROL_RIGHT_ARTIFACT_FIRST;
        if (m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[slot] == ARTIFACT_NONE) {
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        } else {
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
            m_window->BroadcastMessage(message);
            message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
            message.payload.widget.data.value =
                H2EnumIndex(m_heroes[H2EnumIndex(SWAP_SIDE_RIGHT)]->m_artifacts[slot]);
        }
        m_window->BroadcastMessage(message);
    }
}

void swapManager::SplitMons(void) {

    armyGroup* targetTroops;
    armyGroup* selectedArmy;
    i32 openSlot;

    tag_message message;

    selectedArmy = &m_heroes[H2EnumIndex(m_selectedSide)]->m_army;
    targetTroops = &m_heroes[H2EnumIndex(m_targetSide)]->m_army;

    gpTownManager->m_heroWindow1 =
        new heroWindow(SPLIT_WINDOW_X, SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == NULL)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = selectedArmy->m_creatureCounts[m_selectedSlot];

    message.type = MESSAGE_WIDGET;
    if (m_selectedSide == m_targetSide) {
        utf8::Copy(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
              localization::Tr("hero.army.split.prompt")
        );
    } else {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            localization::Tr("hero.army.split.between_heroes"),
            gArmyNamesPlural[H2EnumIndex(selectedArmy->m_creatureTypes[m_selectedSlot])],
            m_heroes[H2EnumIndex(m_selectedSide)]->m_name,
            m_heroes[H2EnumIndex(m_targetSide)]->m_name
        );
    }
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = SPLIT_TEXT_CONTROL;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", gpTownManager->m_splitAmount);
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
