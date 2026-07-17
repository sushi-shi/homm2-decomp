// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Campaign.obj   from: (directly linked into exe)
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/message.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/Campaign.h>
VA(0x00447710, 0x563)
i32 game::HandleCampaignWin(void) {
    i32 sideIndex;
    i32 mapIndex;

    memset(m_campaignMapEnabled, 0, sizeof(m_campaignMapEnabled));
    if (m_campaignType == IDX(CAMPAIGN_ROLAND)) {
        switch (m_campaignScenario + 1) {
            case 0:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_INTRO);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][0] = 1;
                break;
            case 1:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_1);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][1] = 1;
                break;
            case 2:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_2);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][2] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][3] = 1;
                break;
            case 3:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3B);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][3] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)] = 1;
                break;
            case 4:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][4] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][11] = 1;
                break;
            case 5:
                if (m_campaignStartingSide == IDX(CAMPAIGN_ROLAND))
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5A);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5B);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][5] = 1;
                break;
            case 6:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_6);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][6] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][7] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] = 1;
                break;
            case 7:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][8] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)] = 1;
                break;
            case 8:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][8] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)] = 1;
                break;
            case 9:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_9);
                m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][9] = 1;
                break;
            case 10:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_END);
                break;
        }
    } else {
        switch (m_campaignScenario + 1) {
            case 0:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_INTRO);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][0] = 1;
                break;
            case 1:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_1);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][1] = 1;
                break;
            case 2:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_2);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][2] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][3] = 1;
                break;
            case 3:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][4] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][11] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_NECROMANCER_GUILD)] = 1;
                break;
            case 4:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][4] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][11] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_OGRE_ALLIANCE)] = 1;
                break;
            case 5:
                if (m_campaignStartingSide == IDX(CAMPAIGN_ARCHIBALD))
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5A);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5B);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][5] = 1;
                break;
            case 6:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_6);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][6] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][7] = 1;
                break;
            case 7:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7B);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][7] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_DRAGON_ALLIANCE)] = 1;
                break;
            case 8:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_8);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][8] = 1;
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][9] = 1;
                break;
            case 9:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][10] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 1;
                break;
            case 10:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10);
                m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][10] = 1;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 1;
                break;
            case 11:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_END);
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 0;
                m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 0;
                break;
        }
    }

    if (m_campaignScenario + 1 != CAMPAIGN_ARCHIBALD_FINAL_SCENARIO + 1
        && (gpGame->m_campaignScenario + 1 != CAMPAIGN_ROLAND_FINAL_SCENARIO + 1
            || gpGame->m_campaignType != IDX(CAMPAIGN_ROLAND))) {
        m_campaignScenario = CAMPAIGN_NO_SCENARIO;
        for (sideIndex = 0; sideIndex < IDX(CAMPAIGN_SIDE_COUNT); ++sideIndex) {
            for (mapIndex = 0; mapIndex < CAMPAIGN_REGULAR_MAP_COUNT; ++mapIndex) {
                if (m_campaignMapEnabled[sideIndex][OD_STEER(mapIndex)]) {
                    gpGame->m_campaignScenarioBonus[sideIndex][OD_STEER(mapIndex)] =
                        m_campaignScore;
                    if (m_campaignScenario == CAMPAIGN_NO_SCENARIO) {
                        m_campaignType = static_cast<u8>(sideIndex);
                        m_campaignScenario = static_cast<i8>(mapIndex);
                    }
                }
            }
        }
        gpGame->ShowCampaignInfo(0, 0);
        switch (gpWindowManager->m_dialogResult) {
            case CAMPAIGN_DIALOG_ACCEPT:
                return 1;
            case CAMPAIGN_DIALOG_CANCEL:
                return 0;
        }
    }
    return 0;
}

VA(0x00447c73, 0x343)
void game::PlayPreScenarioSmacker(i32 side, i32 map) {
    if (side == CAMPAIGN_ROLAND) {
        switch (map + 1) {
            case 1:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_INTRO);
                break;
            case 2:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_1);
                break;
            case 3:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_2);
                break;
            case 4:
                if (m_campaignScenarioCompleted[m_campaignStartingSide][2])
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3B);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3A);
                break;
            case 5:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4);
                break;
            case 6:
                if (m_campaignStartingSide == IDX(CAMPAIGN_ROLAND))
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5A);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5B);
                break;
            case 7:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_6);
                break;
            case 8:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_7);
                break;
            case 9:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8);
                break;
            case 10:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_9);
                break;
            case 12:
            case 13:
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4);
                break;
        }
    } else {
        switch (map + 1) {
            case 1:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_INTRO);
                break;
            case 2:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_1);
                break;
            case 3:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_2);
                break;
            case 4:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_3);
                break;
            case 5:
                if (m_campaignScenarioCompleted[m_campaignStartingSide][3])
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
                break;
            case 6:
                if (m_campaignStartingSide == IDX(CAMPAIGN_ARCHIBALD))
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5A);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5B);
                break;
            case 7:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_6);
                break;
            case 8:
                if (m_campaignScenarioCompleted[m_campaignType][6])
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7B);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7A);
                break;
            case 9:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_8);
                break;
            case 10:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_9);
                break;
            case 11:
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10);
                break;
            case 12:
            case 13:
                if (m_campaignScenarioCompleted[m_campaignStartingSide][3])
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
                else
                    PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
                break;
        }
    }
    gpWindowManager->m_updateFlags = 1;
}

// @match-note: Complete 0x48 frame/CFG and 68/68 relocation audit. First reported
// residual is the local advmice.mse symbol identity at +0x13..+0x17; first code
// residual is the savedInterface slot at +0x33..+0x35. Declaration order and
// od_slots-guided local names were tried.
VA(0x00447fb6, 0x48d)
void game::ShowCampaignInfo(i32 viewOnly, i32) {
    i32 savedInterface;
    i32 map;
    i32 trackMapIndex;
    widget* trackWidget;
    tag_message message;

    gpMouseManager->SetPointer("advmice.mse", 0, CAMPAIGN_POINTER_HIDDEN_HOTSPOT);
    gpMouseManager->ReallyShowPointer();
    savedInterface = gbUseEvilInterface;
    gbUseEvilInterface = m_campaignType == IDX(CAMPAIGN_ARCHIBALD);
    bCampaignViewOnly = viewOnly;
    iCurViewSide = m_campaignType;
    iCurViewMap = m_campaignScenario;
    if (m_campaignScenario == CAMPAIGN_SWITCHING_SCENARIO && !viewOnly) {
        if (m_campaignType == IDX(CAMPAIGN_ROLAND))
            iCampaignTrackType = 4;
        else if (m_campaignScenarioCompleted[IDX(CAMPAIGN_ARCHIBALD)][2])
            iCampaignTrackType = 5;
        else
            iCampaignTrackType = 6;
    } else {
        iCampaignTrackType = m_campaignType + m_campaignStartingSide * 2;
    }

    campWin = new heroWindow(0, 0, "campaign.bin");
    if (campWin == 0)
        MemError();
    trackWidget = 0;
    for (map = 0; map < CAMPAIGN_TRACK_POINT_COUNT; ++map) {
        if (iCampaignTrackType < 4 && map >= CAMPAIGN_REGULAR_MAP_COUNT)
            continue;
        if (iCampaignTrackType == 4 && map == 12)
            continue;
        if (iCampaignTrackType == 5 && map == 12)
            continue;
        if (iCampaignTrackType == 6 && map == 11)
            continue;
        if (trackXY[iCurViewSide][map][0] != -1) {
            trackMapIndex = map;
            if (trackMapIndex > CAMPAIGN_REGULAR_MAP_COUNT)
                trackMapIndex = CAMPAIGN_REGULAR_MAP_COUNT;
            trackWidget = new iconWidget(
                trackXY[map < 4 ? m_campaignStartingSide : m_campaignType][map][0]
                    - CAMPAIGN_TRACK_ICON_OFFSET,
                trackXY[map < 4 ? m_campaignStartingSide : m_campaignType][map][1]
                    - CAMPAIGN_TRACK_ICON_OFFSET,
                CAMPAIGN_TRACK_ICON_SIZE,
                CAMPAIGN_TRACK_ICON_SIZE,
                "campxtrg.icn",
                CAMPAIGN_TRACK_ICON_FRAME,
                0,
                trackMapIndex + CAMPAIGN_TRACK_WIDGET_FIRST,
                CAMPAIGN_TRACK_ICON_COLOR,
                1
            );
            if (trackWidget == 0)
                MemError();
            campWin->AddWidget(trackWidget, -1);
        }
    }

    message.type = MESSAGE_WIDGET;
    if (!viewOnly) {
        message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
        message.payload.widget.data.value = CAMPAIGN_MESSAGE_DESELECT;
        campWin->BroadcastMessage(message);
    }
    if (gbLowMemory) {
        message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.id = CAMPAIGN_DIALOG_REPLAY;
        message.payload.widget.data.value = CAMPAIGN_MESSAGE_DESELECT;
        campWin->BroadcastMessage(message);
    }
    gpSoundManager->SwitchAmbientMusic(
        m_campaignType == IDX(CAMPAIGN_ROLAND) ? CAMPAIGN_GOOD_MUSIC : CAMPAIGN_EVIL_MUSIC
    );
    CampaignInfoUpdate(0);
    gpWindowManager->DoDialog(campWin, CampaignHandler, 0);
    delete campWin;
    gbUseEvilInterface = savedInterface;

    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
        NormalDialog(
            "Are you sure you want to restart this scenario?",
            CAMPAIGN_RESTART_CONFIRM,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0
        );
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
            InitCampaignMap();
            gpAdvManager->m_visibilityMapValid = 0;
            giBottomViewOverride = 0;
            gpWindowManager->FadeScreen(1, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
            gpAdvManager->SetInitialMapOrigin();
            gpAdvManager->RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(0, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
        }
    }
}

// @semantic: Current Campaign.cpp/Campaign.h award-enum epoch: the 0x68 frame,
// local slots, CFG, and semantic external relocation identities are complete.
// First residual +0x2c: retail loads map before side for the enabled-grid index.
// Final bounded pass exhausted five non-improving variants total: direct and
// symmetric indexing, plus match_variants commutative_order, relational_order,
// and the exact pointer-add axis. Revisit only after a relevant Campaign source,
// TU/header, or comparison epoch changes index evaluation/compiler state.
VA(0x00448443, 0xa0f)
void game::CampaignInfoUpdate(i32 redraw) {
    i32 map;
    tag_message message;
    SCampaignChoice* choice;
    char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];

    message.type = MESSAGE_WIDGET;
    for (map = 0; map < CAMPAIGN_TRACK_POINT_COUNT; ++map) {
        if (m_campaignMapEnabled[iCurViewSide][map]) {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_COMPLETE;
        } else if (map < CAMPAIGN_REGULAR_MAP_COUNT
                   && m_campaignScenarioCompleted[map < 4 ? m_campaignStartingSide : m_campaignType]
                                                 [map]) {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_AVAILABLE;
        } else {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_LOCKED;
        }
        if (iCurViewMap == static_cast<i32>(map)) {
            if (map + 1 == 5 && iCampaignTrackType == 1)
                message.payload.widget.data.value += 12;
            else if (map + 1 == 5 && iCampaignTrackType == 2)
                message.payload.widget.data.value += 9;
            else if (map + 1 > CAMPAIGN_REGULAR_MAP_COUNT) {
                if (m_campaignStartingSide == IDX(CAMPAIGN_ROLAND))
                    message.payload.widget.data.value += 6;
                else
                    message.payload.widget.data.value += 3;
            } else if (map + 1 < 5) {
                if (m_campaignStartingSide == IDX(CAMPAIGN_ROLAND))
                    message.payload.widget.data.value += 3;
                else
                    message.payload.widget.data.value += 6;
            } else if (m_campaignType == IDX(CAMPAIGN_ROLAND)) {
                message.payload.widget.data.value += 3;
            } else {
                message.payload.widget.data.value += 6;
            }
        }
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = map + CAMPAIGN_TRACK_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    message.payload.widget.command = CAMPAIGN_MESSAGE_SET_ICON;
    message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
    message.payload.widget.data.text = gText;
    sprintf(gText, "ctrack%02d.icn", iCampaignTrackType);
    campWin->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP)
        sprintf(gText, "5");
    else
        sprintf(gText, "%d", iCurViewMap + 1);
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        sprintf(gText, "%s", cCampaignName[1 - iCurViewSide][iCurViewMap]);
    } else if (m_campaignType != m_campaignStartingSide
               && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        sprintf(gText, "%s", cCampaignName[iCurViewSide][CAMPAIGN_SWITCHING_MAP]);
    } else {
        sprintf(gText, "%s", cCampaignName[iCurViewSide][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        sprintf(gText, "%s", cCampaignDescription[1 - iCurViewSide][iCurViewMap]);
    } else if (m_campaignType != m_campaignStartingSide
               && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        sprintf(gText, "%s", cCampaignDescription[iCurViewSide][CAMPAIGN_SWITCHING_MAP]);
    } else {
        sprintf(gText, "%s", cCampaignDescription[iCurViewSide][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_BONUS_WIDGET;
    sprintf(gText, "%d", m_campaignScenarioBonus[iCurViewSide][iCurViewMap]);
    campWin->BroadcastMessage(message);

    strcpy(gText, "");
    for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map) {
        if (m_campaignAwards[map]) {
            strcat(gText, cCampaignAwards[map]);
            strcat(gText, "\n");
        }
    }
    message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
    campWin->BroadcastMessage(message);

    for (map = 0; map < CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
            choice = &campaignChoices[1 - iCurViewSide][iCurViewMap][map];
        } else if (m_campaignType != m_campaignStartingSide
                   && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
            choice = &campaignChoices[iCurViewSide][CAMPAIGN_SWITCHING_MAP][map];
        } else {
            choice = &campaignChoices[iCurViewSide][iCurViewMap][map];
        }

        switch (choice->type) {
            case CAMPAIGN_CHOICE_RESOURCE:
                sprintf(gText, "%d %s", choice->amount, gResourceNames[choice->value]);
                break;
            case CAMPAIGN_CHOICE_ARTIFACT:
                switch (choice->value) {
                    case IDX(ARTIFACT_MAGE_RING):
                        strcpy(gText, "Mage's Ring");
                        break;
                    case IDX(ARTIFACT_FIZBIN_OF_MISFORTUNE):
                        strcpy(gText, "Fizbin Medal");
                        break;
                    case IDX(ARTIFACT_THUNDER_MACE):
                        strcpy(gText, "Thunder Mace");
                        break;
                    case IDX(ARTIFACT_ARMORED_GAUNTLETS):
                        strcpy(gText, "Gauntlets");
                        break;
                    case IDX(ARTIFACT_DEFENDER_HELM):
                        strcpy(gText, "Defender Helm");
                        break;
                    case IDX(ARTIFACT_DRAGON_SWORD):
                        strcpy(gText, "Dragon Sword");
                        break;
                    case IDX(ARTIFACT_POWER_AXE):
                        strcpy(gText, "Power Axe");
                        break;
                    case IDX(ARTIFACT_DIVINE_BREASTPLATE):
                        strcpy(gText, "Breastplate");
                        break;
                    case IDX(ARTIFACT_MINOR_SCROLL):
                        strcpy(gText, "Minor Scroll");
                        break;
                    case IDX(ARTIFACT_HIDEOUS_MASK):
                    case IDX(ARTIFACT_BLACK_PEARL):
                    default:
                        sprintf(gText, "%s", gArtifactNames[choice->value]);
                        break;
                }
                break;
            case CAMPAIGN_CHOICE_SPELL:
                if (choice->value == IDX(SPELL_SUMMON_EARTH_ELEMENTAL))
                    sprintf(gText, "Summon Earth");
                else
                    sprintf(gText, "%s", gSpellNames[choice->value]);
                break;
            case CAMPAIGN_CHOICE_SECONDARY_SKILL:
                sprintf(
                    gText,
                    "%s %s",
                    gSecondarySkillLevels[choice->amount - 1],
                    gSecondarySkills[choice->value]
                );
                break;
            case CAMPAIGN_CHOICE_CREATURES:
                strcpy(armyName, gArmyNamesPlural[choice->value]);
                armyName[0] -= 'a' - 'A';
                sprintf(gText, "%d %s", choice->amount, armyName);
                break;
            case CAMPAIGN_CHOICE_PUZZLE_PIECES:
                sprintf(gText, "%d %s", choice->value, "Puzzle Pieces");
                break;
            case CAMPAIGN_CHOICE_EXPERIENCE:
                sprintf(gText, "%d %s", choice->value, "Experience");
                break;
            case CAMPAIGN_CHOICE_NONE:
                sprintf(gText, "n/a");
                break;
            case CAMPAIGN_CHOICE_ALIGNMENT:
                sprintf(gText, gAlignmentNames[choice->value]);
                break;
        }
        message.payload.widget.id = map + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    for (map = 0; map < CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        message.payload.widget.id = map + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        if (!bCampaignViewOnly && gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap])
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        campWin->BroadcastMessage(message);

        if (m_campaignChoice[iCurViewSide][iCurViewMap] == map)
            message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
        else
            message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
        campWin->BroadcastMessage(message);
    }
    if (redraw)
        campWin->DrawWindow();
}

// @semantic: Current Campaign.cpp/Campaign.h award-enum epoch: the 0x1c frame,
// map slot, switch CFG, and semantic external relocations are complete after
// coalescing the hover guard's shared break. First residual +0xa8 is a one-byte
// branch displacement caused by the later map/side index load order. Final
// bounded pass exhausted five non-improving variants total: timeout/symmetric
// spellings, plus match_variants relational_order and the exact pointer-add
// axis; switch arm order was also stagnant. Revisit only after a relevant
// Campaign source, TU/header, or comparison epoch changes compiler state.
VA(0x00448e52, 0x521)
i32 CampaignHandler(struct tag_message& message) {
    i32 map;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active)
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    if (giDialogTimeout != 0 && giDialogTimeout < KBTickCount()) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
        message.payload.widget.command = message.payload.widget.id;
        giDialogTimeout = 0;
        return CAMPAIGN_HANDLER_CLOSE;
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case CAMPAIGN_MESSAGE_HOVER:
            case CAMPAIGN_MESSAGE_HELP:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_TRACK_WIDGET_FIRST:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 1:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 2:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 3:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 4:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 5:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 6:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 7:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 8:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 9:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 10:
                    case CAMPAIGN_TRACK_WIDGET_LAST:
                        map = message.payload.widget.id - CAMPAIGN_TRACK_WIDGET_FIRST;
                        if (giDebugLevel < 1 && !gpGame->m_campaignMapEnabled[iCurViewSide][map]) {
                            if (map >= CAMPAIGN_REGULAR_MAP_COUNT
                                || !gpGame->m_campaignScenarioCompleted
                                        [map < 4 ? gpGame->m_campaignStartingSide
                                                 : gpGame->m_campaignType][map])
                                break;
                        }
                        iCurViewMap = map;
                        iCurViewSide = iCurViewMap < 4 ? gpGame->m_campaignStartingSide
                                                       : gpGame->m_campaignType;
                        gpGame->CampaignInfoUpdate(1);
                        break;
                    case CAMPAIGN_BONUS_WIDGET_FIRST:
                    case CAMPAIGN_BONUS_WIDGET_FIRST + 1:
                    case CAMPAIGN_BONUS_WIDGET_LAST:
                        if (!bCampaignViewOnly
                            && gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap]) {
                            gpGame->m_campaignChoice[iCurViewSide][iCurViewMap] = static_cast<u8>(
                                message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST
                            );
                            gpGame->CampaignInfoUpdate(1);
                        }
                        break;
                }
                break;

            case CAMPAIGN_MESSAGE_ACTIVATE:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_DIALOG_REPLAY:
                        gpGame->PlayPreScenarioSmacker(iCurViewSide, iCurViewMap);
                        campWin->DrawWindow();
                        break;
                    case CAMPAIGN_DIALOG_ACCEPT:
                        if (!bCampaignViewOnly) {
                            if (gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap]) {
                                if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
                                    gpGame->m_campaignScenario = CAMPAIGN_SWITCHING_SCENARIO;
                                    gpGame->m_campaignType =
                                        static_cast<u8>(1 - gpGame->m_campaignType);
                                    gpGame->m_campaignMapEnabled[gpGame->m_campaignType]
                                                                [gpGame->m_campaignScenario] = 1;
                                    gpGame->m_campaignScenarioBonus[gpGame->m_campaignType]
                                                                   [gpGame->m_campaignScenario] =
                                        gpGame->m_campaignScore;
                                    gpGame->m_campaignChoice[gpGame->m_campaignType]
                                                            [gpGame->m_campaignScenario] =
                                        gpGame->m_campaignChoice[1 - gpGame->m_campaignType]
                                                                [CAMPAIGN_SWITCHING_MAP];
                                    for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map)
                                        gpGame->m_campaignAwards[map] = 0;
                                } else {
                                    gpGame->m_campaignScenario = static_cast<i8>(iCurViewMap);
                                    gpGame->m_campaignType = static_cast<u8>(iCurViewSide);
                                }
                            } else {
                                NormalDialog(
                                    "The currently selected map is not a valid choice for your "
                                    "next scenario.",
                                    NORMAL_DIALOG_INFO,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0
                                );
                                break;
                            }
                        }
                    case CAMPAIGN_DIALOG_CANCEL:
                    case CAMPAIGN_DIALOG_RESTART:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
                        message.payload.widget.command = message.payload.widget.id;
                        giDialogTimeout = 0;
                        return CAMPAIGN_HANDLER_CLOSE;
                }
                break;
        }
    }
    return CAMPAIGN_HANDLER_CONTINUE;
}

VA(0x00449373, 0x47)
void game::InitEntireCampaign(i32 side) {
    memset(&m_campaignType, 0, CAMPAIGN_STATE_RESET_SIZE);
    m_campaignType = static_cast<u8>(side);
    m_campaignStartingSide = static_cast<u8>(side);
    m_campaignScenario = CAMPAIGN_NO_SCENARIO;
}

// @semantic: The full 0x5c frame and CFG are reconstructed; all twelve named
// locals occupy the retail role slots. All 80 ordered external relocation sites
// now agree after separating direct `this->m_players` access from hero lookup
// through gpGame. First residual +0x0c: retail loads iCurViewMap before
// iCurViewSide, while the equivalent array expression lowers side-first. Eight
// bounded index/evaluation-order variants were non-improving. Revisit only after
// a relevant Campaign source, TU/header, or comparison epoch changes compiler
// state.
VA(0x004493ba, 0xbb7)
void game::InitCampaignMap(void) {
    i32 selectedChoicePosition;
    SCampaignChoice* choiceBest;
    i32 mapHeaderResultCampaign;
    i32 playerSlotSlot;
    i32 bestHeroPositionCandidate;
    playerData* campaignPlayerCurrent;
    i32 heroPositionValue;
    i32 scanPositionId;
    i32 heroPriorityBest;
    i32 bestHeroPriorityLocal;
    i32 swappedHero;
    i32 bonusHeroIndexPosition;

    selectedChoicePosition = m_campaignChoice[iCurViewSide][iCurViewMap];
    if (m_campaignType != m_campaignStartingSide && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        choiceBest = &campaignChoices[iCurViewSide][CAMPAIGN_SWITCHING_MAP][selectedChoicePosition];
    } else {
        choiceBest = &campaignChoices[m_campaignType][m_campaignScenario][selectedChoicePosition];
    }

    gpGame->m_campaignScenarioWon = 0;
    memset(m_setupPlayerColor, 0, CAMPAIGN_SETUP_RESET_SIZE);
    if (m_campaignScenario + 1 == CAMPAIGN_SWITCHING_SCENARIO + 1
        && m_campaignStartingSide != m_campaignType) {
        sprintf(
            m_mapFilename,
            "CAMP%c%02dB.H2C",
            m_campaignType == IDX(CAMPAIGN_ROLAND) ? 'E' : 'G',
            m_campaignScenario + 1
        );
    } else {
        sprintf(
            m_mapFilename,
            "CAMP%c%02d.H2C",
            m_campaignType == IDX(CAMPAIGN_ROLAND) ? 'G' : 'E',
            m_campaignScenario + 1
        );
    }
    m_newGameInitialized = 0;
    if (m_campaignScenario == 0)
        m_campaignScore = 0;
    strcpy(gMapName, m_mapFilename);
    mapHeaderResultCampaign = GetMapHeader(m_mapFilename, &m_mapHeader);
    LoadGame("origdata.bin", 1, 0);
    InitNewGame(0);

    if (choiceBest->type == CAMPAIGN_CHOICE_ALIGNMENT) {
        playerSlotSlot = 0;
        if (m_campaignType == IDX(CAMPAIGN_ARCHIBALD)) {
            if (m_mapHeader.playerEnabled[0])
                ++playerSlotSlot;
            if (m_mapHeader.playerEnabled[1])
                ++playerSlotSlot;
        }
        m_setupPlayerRace[playerSlotSlot] = static_cast<i8>(choiceBest->value);
    }

    if (m_campaignScenario + 1 <= CAMPAIGN_EASY_SCENARIO_LIMIT)
        gpGame->m_difficulty = IDX(CAMPAIGN_DIFFICULTY_EASY);
    else if (m_campaignScenario + 1 <= CAMPAIGN_NORMAL_SCENARIO_LIMIT)
        gpGame->m_difficulty = IDX(CAMPAIGN_DIFFICULTY_NORMAL);
    else
        gpGame->m_difficulty = IDX(CAMPAIGN_DIFFICULTY_HARD);
    m_playerCount = m_mapHeader.playerCount;
    NewMap(gMapName);

    bestHeroPositionCandidate = 0;
    campaignPlayerCurrent = &gpGame->m_players[0];
    for (heroPositionValue = 0; heroPositionValue < campaignPlayerCurrent->m_heroCount;
         ++heroPositionValue) {
        bestHeroPriorityLocal = -1;
        for (scanPositionId = heroPositionValue;
             scanPositionId < campaignPlayerCurrent->m_heroCount;
             ++scanPositionId) {
            if (gpGame->m_heroRecs[campaignPlayerCurrent->m_heroIds[scanPositionId]].m_portrait
                    == IDX(CAMPAIGN_HERO_ROLAND)
                || gpGame->m_heroRecs[campaignPlayerCurrent->m_heroIds[scanPositionId]].m_portrait
                       == IDX(CAMPAIGN_HERO_ARCHIBALD)) {
                heroPriorityBest = CAMPAIGN_HERO_PRIORITY_HIGH;
            } else if (gpGame->m_heroRecs[campaignPlayerCurrent->m_heroIds[scanPositionId]]
                               .m_portrait
                           == IDX(CAMPAIGN_HERO_CORLAGON)
                       || gpGame->m_heroRecs[campaignPlayerCurrent->m_heroIds[scanPositionId]]
                                  .m_portrait
                              == IDX(CAMPAIGN_HERO_HALTON)) {
                heroPriorityBest = CAMPAIGN_HERO_PRIORITY_NORMAL;
            } else {
                heroPriorityBest = 0;
            }
            if (bestHeroPriorityLocal < heroPriorityBest) {
                bestHeroPriorityLocal = heroPriorityBest;
                bestHeroPositionCandidate = scanPositionId;
            }
        }
        if (bestHeroPriorityLocal != -1) {
            swappedHero = campaignPlayerCurrent->m_heroIds[heroPositionValue];
            campaignPlayerCurrent->m_heroIds[heroPositionValue] =
                campaignPlayerCurrent->m_heroIds[bestHeroPositionCandidate];
            campaignPlayerCurrent->m_heroIds[bestHeroPositionCandidate] =
                static_cast<i8>(swappedHero);
        }
    }
    if (campaignPlayerCurrent->m_heroCount)
        campaignPlayerCurrent->m_currentHero = campaignPlayerCurrent->m_heroIds[0];

    switch (choiceBest->type) {
        case CAMPAIGN_CHOICE_RESOURCE:
            m_players[0].m_resources[choiceBest->value] += choiceBest->amount;
            break;
        case CAMPAIGN_CHOICE_ARTIFACT:
            if (m_players[0].m_heroCount > 0)
                GiveArtifact(gpGame->GetHero(m_players[0].m_heroIds[0]), ArtifactType(choiceBest->value), 0, -1);
            break;
        case CAMPAIGN_CHOICE_SPELL:
            if (m_players[0].m_heroCount > 0) {
                bonusHeroIndexPosition = 0;
                if (m_campaignType == IDX(CAMPAIGN_ROLAND) && m_campaignScenario + 1 == 6
                    && m_players[0].m_heroCount > 1)
                    bonusHeroIndexPosition = 1;
                gpGame->GetHero(m_players[0].m_heroIds[bonusHeroIndexPosition])
                    ->m_spells[choiceBest->value] = 1;
            }
            break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            if (m_players[0].m_heroCount > 0)
                gpGame->GetHero(m_players[0].m_heroIds[0])
                    ->SetSS(choiceBest->value, choiceBest->amount);
            break;
        case CAMPAIGN_CHOICE_CREATURES:
            if (m_players[0].m_heroCount > 0)
                gpGame->GetHero(m_players[0].m_heroIds[0])
                    ->m_army.Add(choiceBest->value, choiceBest->amount, -1);
            break;
        case CAMPAIGN_CHOICE_PUZZLE_PIECES:
            m_players[0].m_cheatValue = static_cast<i8>(choiceBest->value);
            break;
        case CAMPAIGN_CHOICE_EXPERIENCE: {
            i32 savedNewGameSetup = gbInNewGameSetup;
            gbInNewGameSetup = true;
            if (m_players[0].m_heroCount > 0) {
                gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += choiceBest->value;
                gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
            }
            gbInNewGameSetup = savedNewGameSetup;
            break;
        }
        case CAMPAIGN_CHOICE_NONE:
            break;
    }

    if ((m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)]
         || (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)]
             && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1))
        && m_players[0].m_heroCount > 0) {
        GiveArtifact(gpGame->GetHero(m_players[0].m_heroIds[0]), ARTIFACT_ULTIMATE_CROWN, 0, -1);
    }
    gbRetreatWin = true;

    if (m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)]) {
        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_HERO_COUNT; ++heroPositionValue) {
            if (gpGame->m_heroRecs[heroPositionValue].m_portrait == IDX(CAMPAIGN_HERO_CORLAGON))
                gpGame->m_heroRecs[heroPositionValue].Deallocate(0);
        }
    }

    if (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]) {
        hero* armyHero = gpGame->GetHero(m_players[CAMPAIGN_CARRYOVER_PLAYER].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPositionValue) {
            if (armyHero->m_army.m_creatureCounts[heroPositionValue] > 0)
                armyHero->m_army.m_creatureCounts[heroPositionValue] *=
                    CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
        }
    }

    if (m_campaignScenario + 1 == 7 && m_campaignType == IDX(CAMPAIGN_ARCHIBALD)) {
        i32 savedNewGameSetup = gbInNewGameSetup;
        hero* armyHero;
        gbInNewGameSetup = true;
        armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPositionValue) {
            armyHero->m_army.m_creatureTypes[heroPositionValue] = ARMY_GROUP_EMPTY_SLOT;
            armyHero->m_army.m_creatureCounts[heroPositionValue] = 0;
        }
        switch (armyHero->m_cursorType) {
            case IDX(FACTION_BARBARIAN):
                armyHero->m_army.Add(IDX(CREATURE_ORC_CHIEF), CAMPAIGN_BARBARIAN_ORC_CHIEF_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_OGRE), CAMPAIGN_BARBARIAN_OGRE_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_GOBLIN), CAMPAIGN_BARBARIAN_GOBLIN_COUNT, -1);
                break;
            case IDX(FACTION_WARLOCK):
                armyHero->m_army.Add(IDX(CREATURE_CENTAUR), CAMPAIGN_WARLOCK_CENTAUR_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_GARGOYLE), CAMPAIGN_WARLOCK_GARGOYLE_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_GRIFFIN), CAMPAIGN_WARLOCK_GRIFFIN_COUNT, -1);
                break;
            case IDX(FACTION_NECROMANCER):
                armyHero->m_army.Add(IDX(CREATURE_SKELETON), CAMPAIGN_NECROMANCER_SKELETON_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_ROYAL_MUMMY), CAMPAIGN_NECROMANCER_MUMMY_COUNT, -1);
                armyHero->m_army.Add(IDX(CREATURE_VAMPIRE_LORD), CAMPAIGN_NECROMANCER_VAMPIRE_COUNT, -1);
                break;
        }
        gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += CAMPAIGN_EXPERIENCE_BONUS;
        gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
        gbInNewGameSetup = savedNewGameSetup;
    }

    if ((m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)]
         && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)
        || m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)]) {
        hero* armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPositionValue) {
            armyHero->m_army.m_creatureTypes[heroPositionValue] =
                static_cast<i8>(m_campaignCarryoverCreatureTypes[heroPositionValue]);
            armyHero->m_army.m_creatureCounts[heroPositionValue] =
                (m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
                     ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
                     : 1)
                * m_campaignCarryoverCreatureCounts[heroPositionValue];
        }
    }

    if (m_campaignType == IDX(CAMPAIGN_ARCHIBALD) && m_campaignScenario + 1 == 6) {
        gpGame->m_mapHeader.victoryCondition = IDX(MAP_VICTORY_DEFEAT_SIDE);
        gpGame->m_mapHeader.victoryConditionValue = CAMPAIGN_SWITCH_VICTORY_VALUE;
        gpGame->m_mapHeader.allowNormalVictory = 1;
    }
    if (m_campaignType == IDX(CAMPAIGN_ROLAND) && m_campaignScenario + 1 == 9) {
        gpGame->m_mapHeader.lossCondition = IDX(MAP_LOSS_STANDARD);
        gpGame->m_mapHeader.lossConditionValue = 0;
    }
    if (m_campaignType == IDX(CAMPAIGN_ROLAND) && m_campaignScenario + 1 == 7)
        gpGame->m_mapHeader.lossConditionValue = CAMPAIGN_ROLAND_TIME_LIMIT;
}

// ---- globals (definitions, RVA order) ----
// @data-layout-note Retail initialized storage is 0xf4f28+0x260. Candidate
// .data is 0x25d: trackXY, campWin, and 40 literal allocations have exact
// payloads after mapping the nine artifact-name literals as a pure permutation
// inside +0x10c..+0x18c. The sole uncovered extent is three terminal zero
// alignment bytes at +0x25d. All 65 retail HIGHLOW targets are owned: trackXY
// uses addends 0/2, the final literal uses 0/8/0xc (two suffix references from
// other code), and every other initialized-data reference uses addend zero.
DATA(0x004f4f28) i16 trackXY[2][13][2] = {39,  336, 113, 336, 150, 294, 187, 336, 261, 336, 335,
                                          336, 409, 378, 409, 294, 483, 336, 557, 336, -1,  -1,
                                          261, 378, -1,  -1,  39,  336, 113, 336, 187, 294, 187,
                                          378, 261, 336, 335, 336, 372, 294, 409, 336, 483, 294,
                                          483, 378, 557, 336, 261, 294, 261, 378};
DATA(0x004f4f90) class heroWindow* campWin = 0;
// @data-layout-note Retail and candidate BSS both cover 0x127ea4+0x10 with
// these four public identities. Retail COMMON order is side/track/viewOnly/map
// at +0/+4/+8/+0xc; VC4.2 currently emits track/viewOnly/side/map. Declaration
// order is byte-neutral for these fixed external names, so retain the proven
// storage and RVAs rather than introducing aliases or initialized data. Track,
// viewOnly, and side have exact candidate/retail reference counts (11/4/24),
// all with addend zero. iCurViewMap has 27 candidate versus 29 retail references;
// that two-site code residual belongs to incomplete function reconstruction,
// not to BSS ownership.
DATA(0x00527ea4) i32 iCurViewSide;
DATA(0x00527ea8) i32 iCampaignTrackType;
DATA(0x00527eac) i32 bCampaignViewOnly;
DATA(0x00527eb0) i32 iCurViewMap;
