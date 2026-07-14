// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\SWAPMGR.OBJ   from: (directly linked into exe)
// functions: 13   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
#include <stdio.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/GAME.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/swapManager.h>
VA(0x004543c0, 0x84)
swapManager::swapManager(void)
{
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
swapManager::swapManager(hero *leftHero, hero *rightHero)
{
    m_heroes[SWAP_SIDE_LEFT] = leftHero;
    m_heroes[SWAP_SIDE_RIGHT] = rightHero;
}

VA(0x00454482, 0x50)
void swapManager::Reset(void)
{
    m_selectedSide = m_targetSide = m_itemType = m_selectedSlot = m_targetSlot =
        SWAP_SLOT_NONE;
}

VA(0x004544d2, 0x30)
int swapManager::DrawSwapWin(void)
{
    m_window->DrawWindow(0);
    gpWindowManager->UpdateScreen();
    return 0;
}

VA(0x00454502, 0x3bc)
int swapManager::Open(int id)
{
    Reset();
    m_window = new heroWindow(0, 0, "swapwin.bin");
    if (m_window == 0)
        MemError();

    SetWinText(m_window, SWAP_WINDOW_TEXT_ID);

    tag_message message;
    message.type = SWAP_MESSAGE_WIDGET;
    message.field4 = SWAP_MESSAGE_SET_PORTRAIT;
    sprintf(gText, "port%04d.icn", m_heroes[SWAP_SIDE_LEFT]->m_portrait);
    message.field8 = SWAP_LEFT_PORTRAIT_WIDGET;
    message.text = gText;
    m_window->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", m_heroes[SWAP_SIDE_RIGHT]->m_portrait);
    message.field8 = SWAP_RIGHT_PORTRAIT_WIDGET;
    m_window->BroadcastMessage(message);

    message.field4 = SWAP_MESSAGE_SET_TEXT;
    sprintf(gText, "%s meets %s", m_heroes[SWAP_SIDE_LEFT]->m_name,
            m_heroes[SWAP_SIDE_RIGHT]->m_name);
    message.text = gText;
    message.field8 = SWAP_TITLE_WIDGET;
    m_window->BroadcastMessage(message);

    for (int side_6 = SWAP_SIDE_LEFT; side_6 < SWAP_SIDE_COUNT; ++side_6) {
        for (int skillSlot = 0; skillSlot < SWAP_SECONDARY_SKILL_WIDGET_COUNT;
             ++skillSlot) {
            if (skillSlot < m_heroes[side_6]->m_secondarySkillCount) {
                message.field4 = SWAP_MESSAGE_SET_ICON;
                message.field8 = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT +
                                 skillSlot + SWAP_CONTROL_LEFT_SKILL_FIRST;
                message.field18 = m_heroes[side_6]->GetNthSS(skillSlot);
                m_window->BroadcastMessage(message);

                message.field4 = SWAP_MESSAGE_SET_TEXT;
                message.field8 = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT +
                                 skillSlot + SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST;
                message.text = gText;
                sprintf(gText, "%d", m_heroes[side_6]->GetSSLevel(
                                         m_heroes[side_6]->GetNthSS(skillSlot)));
                m_window->BroadcastMessage(message);
            } else {
                message.field4 = SWAP_MESSAGE_DISABLE;
                message.field8 = side_6 * SWAP_SECONDARY_SKILL_WIDGET_COUNT +
                                 skillSlot + SWAP_CONTROL_LEFT_SKILL_FIRST;
                message.field18 = SWAP_EMPTY_SKILL_VALUE;
                m_window->BroadcastMessage(message);
            }
        }
    }

    message.type = SWAP_MESSAGE_WIDGET;
    message.field4 = SWAP_MESSAGE_DISABLE;
    message.field18 = SWAP_ADVENTURE_DISABLE_VALUE;
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);

    Update();
    gpWindowManager->AddWindow(m_window, -1, 1);
    KBChangeMenu(hmnuAdv);
    giMonoIconSkip = SWAP_MONO_ICON_SKIP;
    m_selectorIcon = gpResourceManager->GetIcon("swapbtn.icn");
    giMonoIconSkip = SWAP_MONO_ICON_DEFAULT;
    gpMouseManager->SetPointer(0);
    field_0xc = SWAP_MANAGER_MESSAGE;
    field_0x10 = id;
    m_active = 1;
    strcpy(name, "swapManager");
    return 0;
}

VA(0x004548be, 0x12d)
void swapManager::Close(void)
{
    gpResourceManager->Dispose(m_selectorIcon);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = 0;
    gpAdvManager->Activate();

    tag_message message;
    message.type = SWAP_MESSAGE_WIDGET;
    message.field4 = SWAP_MESSAGE_ENABLE;
    message.field18 = SWAP_ADVENTURE_DISABLE_VALUE;
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 1;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 2;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 3;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_FIRST + 4;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
    message.field8 = SWAP_ADVENTURE_WIDGET_LAST;
    gpAdvManager->m_adventureWindow->BroadcastMessage(message);
}

VA(0x004549eb, 0x1f8)
void swapManager::DrawSelector(void)
{
    const char selectorFrame_17 = 10;
    const short leftArmyX_37 = 37;
    const short rightArmyX_12 = 382;
    const short armyY_19 = 268;
    const short armySpacing_16 = 45;
    const short leftArtifactX_1 = 24;
    const short rightArtifactX_6 = 368;
    const short artifactY_7 = 348;
    const short artifactSpacing_11 = 36;
    int selectorX_2 = 0;
    int selectorY_11 = 0;

    if (m_selectedSide != SWAP_SIDE_NONE && m_selectedSlot != SWAP_SLOT_NONE) {
        switch (m_selectedSide) {
        case SWAP_SIDE_LEFT:
            switch (m_itemType) {
            case SWAP_ITEM_ARMY:
                selectorX_2 = m_selectedSlot * armySpacing_16 + leftArmyX_37 - 1;
                selectorY_11 = armyY_19 - 1;
                break;
            case SWAP_ITEM_ARTIFACT:
                selectorX_2 = (m_selectedSlot % SWAP_ARTIFACT_COLUMN_COUNT) *
                                  artifactSpacing_11 +
                              leftArtifactX_1 - 1;
                selectorY_11 = artifactY_7 +
                            (m_selectedSlot <= SWAP_ARTIFACT_FIRST_ROW_LAST
                                 ? 0
                                 : artifactSpacing_11) - 1;
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
                selectorX_2 = (m_selectedSlot % SWAP_ARTIFACT_COLUMN_COUNT) *
                                  artifactSpacing_11 +
                              rightArtifactX_6 - 1;
                selectorY_11 = artifactY_7 +
                            (m_selectedSlot <= SWAP_ARTIFACT_FIRST_ROW_LAST
                                 ? 0
                                 : artifactSpacing_11) - 1;
                break;
            }
            break;
        }

        m_selectorIcon->FillToBuffer(selectorX_2, selectorY_11,
                                     m_itemType == SWAP_ITEM_ARMY
                                         ? SWAP_ARMY_SELECTOR_FRAME
                                         : SWAP_ARTIFACT_SELECTOR_FRAME,
                                     selectorFrame_17, 0, 0);
        gpWindowManager->UpdateScreenRegion(selectorX_2, selectorY_11,
                                            SWAP_SELECTOR_WIDTH,
                                            SWAP_SELECTOR_HEIGHT);
    }
}

// @match-note 91.37%
// First residual: retail +0xa0 jumps directly to the inner-switch epilogue;
// ours branches over a compiler-folded closeRequested_5 = 0 store at +0xa5..+0xab.
// Frame 0x28 and slots match: slotIndex_8 -0x4, closeRequested_5 -0x8,
// quickView -0xc, side -0x10, artifactSlot_2 -0x14, secondarySkill_1 -0x18;
// this and the three switch temporaries occupy -0x1c..-0x28. The compressed
// 0x41..0xe7 switch, all case ranges/bodies, CFG semantics, and 71/71 relocation
// targets are recovered; external targets agree as a multiset (only body order and
// local jump-table labels differ). Tried direct/empty-arm condition polarities,
// od_slots names, unsigned hero id, a shared skill body, close inequality/early
// break spellings, and inverted army split polarity. Revisit at 95% total fuzzy.
VA(0x00454be3, 0xaf0)
int swapManager::Main(tag_message &message)
{
    int closeRequested_5 = 0;
    int quickView = (message.fieldC & SWAP_QUICK_VIEW_MODIFIER) != 0;
    int side;
    int slotIndex_8;
    int artifactSlot_2;
    int secondarySkill_1;

    switch (message.type) {
    case SWAP_MESSAGE_REDRAW:
        if (quickView) {
        } else {
            Reset();
            Update();
            DrawSwapWin();
        }
        break;

    case SWAP_MESSAGE_WIDGET:
        switch (message.field4) {
        case SWAP_COMMAND_SELECT:
            if (quickView) {
            } else {
                if (message.field8 == SWAP_CONTROL_CLOSE)
                    closeRequested_5 = 1;
            }
            break;

        case SWAP_COMMAND_HOVER:
        case SWAP_COMMAND_HELP:
            switch (message.field8) {
            case SWAP_CONTROL_LEFT_SKILL_FIRST:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 1:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 2:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 3:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 4:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 5:
            case SWAP_CONTROL_LEFT_SKILL_FIRST + 6:
            case SWAP_CONTROL_LEFT_SKILL_LAST:
                side = SWAP_SIDE_LEFT;
                slotIndex_8 = message.field8 - SWAP_CONTROL_LEFT_SKILL_FIRST;
                goto showSecondarySkill;

            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 1:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 2:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 3:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 4:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 5:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST + 6:
            case SWAP_CONTROL_LEFT_SKILL_LEVEL_LAST:
                side = SWAP_SIDE_LEFT;
                slotIndex_8 = message.field8 - SWAP_CONTROL_LEFT_SKILL_LEVEL_FIRST;
                goto showSecondarySkill;

            case SWAP_CONTROL_RIGHT_SKILL_FIRST:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 1:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 2:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 3:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 4:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 5:
            case SWAP_CONTROL_RIGHT_SKILL_FIRST + 6:
            case SWAP_CONTROL_RIGHT_SKILL_LAST:
                side = SWAP_SIDE_RIGHT;
                slotIndex_8 = message.field8 - SWAP_CONTROL_RIGHT_SKILL_FIRST;
                goto showSecondarySkill;

            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 1:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 2:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 3:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 4:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 5:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST + 6:
            case SWAP_CONTROL_RIGHT_SKILL_LEVEL_LAST:
                side = SWAP_SIDE_RIGHT;
                slotIndex_8 = message.field8 - SWAP_CONTROL_RIGHT_SKILL_LEVEL_FIRST;
            showSecondarySkill:
                if (slotIndex_8 >= m_heroes[side]->m_secondarySkillCount) {
                } else {
                    secondarySkill_1 = m_heroes[side]->GetNthSS(slotIndex_8);
                    m_heroes[side]->DoSSLevelDialog(secondarySkill_1, quickView);
                }
                break;

            case SWAP_CONTROL_LEFT_HERO:
                if (quickView) {
                } else {
                    HeroView(m_heroes[SWAP_SIDE_LEFT]->m_id, 1, 0);
                    gpAdvManager->RedrawAdvScreen(1, 0);
                    Update();
                    DrawSwapWin();
                    Reset();
                    gpWindowManager->FadeScreen(0, SWAP_FADE_STEPS, 0);
                }
                break;

            case SWAP_CONTROL_RIGHT_HERO:
                if (quickView) {
                } else {
                    HeroView(m_heroes[SWAP_SIDE_RIGHT]->m_id, 1, 0);
                    gpAdvManager->RedrawAdvScreen(1, 0);
                    Update();
                    DrawSwapWin();
                    Reset();
                    gpWindowManager->FadeScreen(0, SWAP_FADE_STEPS, 0);
                }
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
                artifactSlot_2 = message.field8 - SWAP_CONTROL_LEFT_ARTIFACT_FIRST;
                if (!quickView && m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2] ==
                                      SWAP_ARTIFACT_MAGIC_BOOK) {
                    NormalDialog("This item can't be traded.", NORMAL_DIALOG_INFO,
                                 NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_VALUE, 0);
                } else if (quickView) {
                    if (m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2] !=
                        SWAP_ARTIFACT_NONE)
                        m_heroes[SWAP_SIDE_LEFT]->ViewArtifact(
                            m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2],
                            SWAP_VIEW_QUICK,
                            m_heroes[SWAP_SIDE_LEFT]->m_artifactExtra[artifactSlot_2]);
                } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                    if (m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2] ==
                        SWAP_ARTIFACT_NONE) {
                        Reset();
                    } else {
                        m_selectedSide = SWAP_SIDE_LEFT;
                        m_targetSide = SWAP_SIDE_NONE;
                        m_itemType = SWAP_ITEM_ARTIFACT;
                        m_selectedSlot = artifactSlot_2;
                        m_targetSlot = SWAP_SLOT_NONE;
                    }
                } else {
                    m_targetSide = SWAP_SIDE_LEFT;
                    m_targetSlot = artifactSlot_2;
                    if (m_selectedSide == SWAP_SIDE_LEFT &&
                        m_selectedSlot == m_targetSlot) {
                        m_heroes[SWAP_SIDE_LEFT]->ViewArtifact(
                            m_heroes[SWAP_SIDE_LEFT]->m_artifacts[artifactSlot_2],
                            SWAP_VIEW_FULL,
                            m_heroes[SWAP_SIDE_LEFT]->m_artifactExtra[artifactSlot_2]);
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
                artifactSlot_2 = message.field8 - SWAP_CONTROL_RIGHT_ARTIFACT_FIRST;
                if (!quickView && m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2] ==
                                      SWAP_ARTIFACT_MAGIC_BOOK) {
                    NormalDialog("This item can't be traded.", NORMAL_DIALOG_INFO,
                                 NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_VALUE,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_RESOURCE, 0,
                                 NORMAL_DIALOG_NO_VALUE, 0);
                } else if (quickView) {
                    if (m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2] !=
                        SWAP_ARTIFACT_NONE)
                        m_heroes[SWAP_SIDE_RIGHT]->ViewArtifact(
                            m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2],
                            SWAP_VIEW_QUICK,
                            m_heroes[SWAP_SIDE_RIGHT]->m_artifactExtra[artifactSlot_2]);
                } else if (m_itemType != SWAP_ITEM_ARTIFACT) {
                    if (m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2] ==
                        SWAP_ARTIFACT_NONE) {
                        Reset();
                    } else {
                        m_selectedSide = SWAP_SIDE_RIGHT;
                        m_targetSide = SWAP_SIDE_NONE;
                        m_itemType = SWAP_ITEM_ARTIFACT;
                        m_selectedSlot = artifactSlot_2;
                        m_targetSlot = SWAP_SLOT_NONE;
                    }
                } else {
                    m_targetSide = SWAP_SIDE_RIGHT;
                    m_targetSlot = artifactSlot_2;
                    if (m_selectedSide == SWAP_SIDE_RIGHT &&
                        m_selectedSlot == m_targetSlot) {
                        m_heroes[SWAP_SIDE_RIGHT]->ViewArtifact(
                            m_heroes[SWAP_SIDE_RIGHT]->m_artifacts[artifactSlot_2],
                            SWAP_VIEW_FULL,
                            m_heroes[SWAP_SIDE_RIGHT]->m_artifactExtra[artifactSlot_2]);
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
                slotIndex_8 = message.field8 - SWAP_CONTROL_LEFT_ARMY_FIRST;
                if (quickView) {
                    if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slotIndex_8] !=
                        SWAP_CREATURE_NONE)
                        gpGame->ViewArmy(
                            SWAP_ARMY_VIEW_X, SWAP_ARMY_VIEW_Y,
                            m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slotIndex_8],
                            m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureCounts[slotIndex_8],
                            0, 0, 1, 1, m_heroes[SWAP_SIDE_LEFT], 0,
                            &m_heroes[SWAP_SIDE_LEFT]->m_army, slotIndex_8);
                } else if (m_itemType != SWAP_ITEM_ARMY) {
                    if (m_heroes[SWAP_SIDE_LEFT]->m_army.m_creatureTypes[slotIndex_8] ==
                        SWAP_CREATURE_NONE) {
                        Reset();
                    } else {
                        m_selectedSide = SWAP_SIDE_LEFT;
                        m_targetSide = SWAP_SIDE_NONE;
                        m_itemType = SWAP_ITEM_ARMY;
                        m_selectedSlot = slotIndex_8;
                        m_targetSlot = SWAP_SLOT_NONE;
                    }
                } else {
                    m_targetSide = SWAP_SIDE_LEFT;
                    m_targetSlot = slotIndex_8;
                    if (m_selectedSide == SWAP_SIDE_LEFT &&
                        m_selectedSlot == m_targetSlot) {
                        ViewMon();
                        Reset();
                    } else if ((message.fieldC & SWAP_SPLIT_MODIFIER_MASK) == 0 ||
                               (m_heroes[m_targetSide]
                                        ->m_army.m_creatureTypes[m_targetSlot] !=
                                    SWAP_CREATURE_NONE &&
                                m_heroes[m_selectedSide]
                                        ->m_army.m_creatureTypes[m_selectedSlot] !=
                                    m_heroes[m_targetSide]
                                        ->m_army.m_creatureTypes[m_targetSlot])) {
                        SwapMons();
                        Reset();
                    } else {
                        SplitMons();
                        Reset();
                    }
                }
                break;

            case SWAP_CONTROL_RIGHT_ARMY_FIRST:
            case SWAP_CONTROL_RIGHT_ARMY_FIRST + 1:
            case SWAP_CONTROL_RIGHT_ARMY_FIRST + 2:
            case SWAP_CONTROL_RIGHT_ARMY_FIRST + 3:
            case SWAP_CONTROL_RIGHT_ARMY_LAST:
                slotIndex_8 = message.field8 - SWAP_CONTROL_RIGHT_ARMY_FIRST;
                if (quickView) {
                    if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slotIndex_8] !=
                        SWAP_CREATURE_NONE)
                        gpGame->ViewArmy(
                            SWAP_ARMY_VIEW_X, SWAP_ARMY_VIEW_Y,
                            m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slotIndex_8],
                            m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureCounts[slotIndex_8],
                            0, 0, 1, 1, m_heroes[SWAP_SIDE_RIGHT], 0,
                            &m_heroes[SWAP_SIDE_RIGHT]->m_army, slotIndex_8);
                } else if (m_itemType != SWAP_ITEM_ARMY) {
                    if (m_heroes[SWAP_SIDE_RIGHT]->m_army.m_creatureTypes[slotIndex_8] ==
                        SWAP_CREATURE_NONE) {
                        Reset();
                    } else {
                        m_selectedSide = SWAP_SIDE_RIGHT;
                        m_targetSide = SWAP_SIDE_NONE;
                        m_itemType = SWAP_ITEM_ARMY;
                        m_selectedSlot = slotIndex_8;
                        m_targetSlot = SWAP_SLOT_NONE;
                    }
                } else {
                    m_targetSide = SWAP_SIDE_RIGHT;
                    m_targetSlot = slotIndex_8;
                    if (m_selectedSide == SWAP_SIDE_RIGHT &&
                        m_selectedSlot == m_targetSlot) {
                        ViewMon();
                        Reset();
                    } else if ((message.fieldC & SWAP_SPLIT_MODIFIER_MASK) == 0 ||
                               (m_heroes[m_targetSide]
                                        ->m_army.m_creatureTypes[m_targetSlot] !=
                                    SWAP_CREATURE_NONE &&
                                m_heroes[m_selectedSide]
                                        ->m_army.m_creatureTypes[m_selectedSlot] !=
                                    m_heroes[m_targetSide]
                                        ->m_army.m_creatureTypes[m_targetSlot])) {
                        SwapMons();
                        Reset();
                    } else {
                        SplitMons();
                        Reset();
                    }
                }
                break;
            }

            if (!quickView) {
                Update();
                DrawSwapWin();
                DrawSelector();
            }
            break;
        }
        break;
    }

    if (closeRequested_5) {
        message.type = SWAP_MESSAGE_EXIT;
        message.field4 = SWAP_COMMAND_EXIT;
        return SWAP_RESULT_CLOSE;
    }
    return SWAP_RESULT_CONTINUE;
}

VA(0x004556d3, 0xa3)
void swapManager::ViewMon(void) {}

VA(0x00455776, 0x255)
void swapManager::SwapArtifacts(void) {}

VA(0x004559cb, 0x177)
void swapManager::SwapMons(void) {}

VA(0x00455b42, 0x47b)
void swapManager::Update(void) {}

VA(0x00455fbd, 0x388)
void swapManager::SplitMons(void) {}


// ===== vtable swapManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00454502, 0x3bc)  int swapManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x004548be, 0x12d)  void swapManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x00454be3, 0xaf0)  int swapManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(swapManager, 0x004eb650);
