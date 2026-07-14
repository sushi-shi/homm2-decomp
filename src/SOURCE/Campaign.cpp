// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Campaign.obj   from: (directly linked into exe)
// functions: 7   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <_carcass_types.h>
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
// @match-note: Frame and CFG cover the full 0x563 span; external relocations were
// manually audited past the jump-table labels. First non-local residual is
// +0x19a..+0x19e (movie literal 0x12/0x13); case order and final-condition polarity
// were tried, so defer the remaining movie-ID ambiguity until the 95% pass.
VA(0x00447710, 0x563)
int game::HandleCampaignWin(void)
{
    int side;
    int map;

    memset(m_campaignMapEnabled, 0, sizeof(m_campaignMapEnabled));
    if (m_campaignType == CAMPAIGN_ROLAND) {
        switch (m_campaignScenario + 1) {
        case 0:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_INTRO);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][0] = 1;
            break;
        case 1:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_1);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][1] = 1;
            break;
        case 2:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_2);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][2] = 1;
            m_campaignMapEnabled[CAMPAIGN_ROLAND][3] = 1;
            break;
        case 3:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3B);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][3] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_DWARF_ALLIANCE] = 1;
            break;
        case 4:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][4] = 1;
            m_campaignMapEnabled[CAMPAIGN_ROLAND][11] = 1;
            break;
        case 5:
            if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5B);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][5] = 1;
            break;
        case 6:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_6);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][6] = 1;
            m_campaignMapEnabled[CAMPAIGN_ROLAND][7] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_BATTLE_GARB] = 1;
            break;
        case 7:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][8] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_CARRYOVER_ARMY] = 1;
            break;
        case 8:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][8] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_DRAGON_SLAYER] = 1;
            break;
        case 9:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_9);
            m_campaignMapEnabled[CAMPAIGN_ROLAND][9] = 1;
            break;
        case 10:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_INTRO);
            break;
        }
    } else {
        switch (m_campaignScenario + 1) {
        case 0:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_INTRO);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][0] = 1;
            break;
        case 1:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_1);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][1] = 1;
            break;
        case 2:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_2);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][2] = 1;
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][3] = 1;
            break;
        case 3:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][4] = 1;
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][11] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_DWARFBANE] = 1;
            break;
        case 4:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][4] = 1;
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][11] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_OGRE_ALLIANCE] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_DRAGON_ALLIANCE] = 1;
            break;
        case 5:
            if (m_campaignStartingSide == CAMPAIGN_ARCHIBALD)
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5B);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][5] = 1;
            break;
        case 6:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_6);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][6] = 1;
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][7] = 1;
            break;
        case 7:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7B);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][7] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_DEFEAT_ARCHIBALD] = 1;
            break;
        case 8:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_8);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][8] = 1;
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][9] = 1;
            break;
        case 9:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][10] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN] = 1;
            break;
        case 10:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10);
            m_campaignMapEnabled[CAMPAIGN_ARCHIBALD][10] = 1;
            m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN_ALT] = 1;
            break;
        case 11:
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_END);
            m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN] = 0;
            m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN_ALT] = 0;
            break;
        }
    }

    if (m_campaignScenario + 1 != CAMPAIGN_ARCHIBALD_FINAL_SCENARIO + 1 &&
        (gpGame->m_campaignScenario + 1 !=
             CAMPAIGN_ROLAND_FINAL_SCENARIO + 1 ||
         gpGame->m_campaignType != CAMPAIGN_ROLAND)) {
        m_campaignScenario = CAMPAIGN_NO_SCENARIO;
        for (side = 0; side < CAMPAIGN_SIDE_COUNT; ++side) {
            for (map = 0; map < CAMPAIGN_REGULAR_MAP_COUNT; ++map) {
                if (m_campaignMapEnabled[side][map]) {
                    gpGame->m_campaignScenarioBonus[side][map] = m_campaignScore;
                    if (m_campaignScenario == CAMPAIGN_NO_SCENARIO) {
                        m_campaignType = static_cast<unsigned char>(side);
                        m_campaignScenario = static_cast<signed char>(map);
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

// @match-note: Frame/CFG and external relocations are complete; the relocation
// helper truncates at delinked switch labels. First residual is +0x65..+0x6a
// (je/jne layout); positive/negative completion tests and arm/case order were tried.
VA(0x00447c73, 0x343)
void game::PlayPreScenarioSmacker(int side, int map)
{
    if (side == CAMPAIGN_ROLAND) {
        switch (map + 1) {
        case 1: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_INTRO); break;
        case 2: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_1); break;
        case 3: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_2); break;
        case 4:
            if (!m_campaignScenarioCompleted[m_campaignStartingSide][2])
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_3B);
            break;
        case 5: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4); break;
        case 6:
            if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ROLAND_5B);
            break;
        case 7: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_6); break;
        case 8: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_7); break;
        case 9: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_8); break;
        case 10: PlaySmacker(CAMPAIGN_SMACKER_ROLAND_9); break;
        case 12:
        case 13:
            PlaySmacker(CAMPAIGN_SMACKER_ROLAND_4);
            break;
        }
    } else {
        switch (map + 1) {
        case 1: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_INTRO); break;
        case 2: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_1); break;
        case 3: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_2); break;
        case 4: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_3); break;
        case 5:
            if (!m_campaignScenarioCompleted[m_campaignStartingSide][3])
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
            PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4_END);
            break;
        case 6:
            if (m_campaignStartingSide == CAMPAIGN_ARCHIBALD)
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_5B);
            break;
        case 7: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_6); break;
        case 8:
            if (!m_campaignScenarioCompleted[m_campaignType][6])
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_7B);
            break;
        case 9: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_8); break;
        case 10: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_9); break;
        case 11: PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_10); break;
        case 12:
        case 13:
            if (!m_campaignScenarioCompleted[m_campaignStartingSide][3])
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4A);
            else
                PlaySmacker(CAMPAIGN_SMACKER_ARCHIBALD_4B);
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
void game::ShowCampaignInfo(int viewOnly, int)
{
    int savedInterface;
    int map;
    int trackMap;
    widget *trackWidget;
    tag_message message;

    gpMouseManager->SetPointer("advmice.mse", 0,
                               CAMPAIGN_POINTER_HIDDEN_HOTSPOT);
    gpMouseManager->ReallyShowPointer();
    savedInterface = gbUseEvilInterface;
    gbUseEvilInterface = m_campaignType == CAMPAIGN_ARCHIBALD;
    bCampaignViewOnly = viewOnly;
    iCurViewSide = m_campaignType;
    iCurViewMap = m_campaignScenario;
    if (m_campaignScenario == CAMPAIGN_SWITCHING_SCENARIO && !viewOnly) {
        if (m_campaignType == CAMPAIGN_ROLAND)
            iCampaignTrackType = 4;
        else if (m_campaignScenarioCompleted[CAMPAIGN_ARCHIBALD][2])
            iCampaignTrackType = 6;
        else
            iCampaignTrackType = 5;
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
            trackMap = map;
            if (map > CAMPAIGN_REGULAR_MAP_COUNT)
                trackMap = CAMPAIGN_REGULAR_MAP_COUNT;
            trackWidget = new iconWidget(
                trackXY[map < 4 ? m_campaignStartingSide : m_campaignType][map][0] -
                    CAMPAIGN_TRACK_ICON_OFFSET,
                trackXY[map < 4 ? m_campaignStartingSide : m_campaignType][map][1] -
                    CAMPAIGN_TRACK_ICON_OFFSET,
                CAMPAIGN_TRACK_ICON_SIZE, CAMPAIGN_TRACK_ICON_SIZE,
                "campxtrg.icn", CAMPAIGN_TRACK_ICON_FRAME, 0,
                trackMap + CAMPAIGN_TRACK_WIDGET_FIRST,
                CAMPAIGN_TRACK_ICON_COLOR, 1);
            if (trackWidget == 0)
                MemError();
            campWin->AddWidget(trackWidget, -1);
        }
    }

    message.type = CAMPAIGN_MESSAGE_WIDGET;
    if (!viewOnly) {
        message.field4 = CAMPAIGN_MESSAGE_DESELECT;
        message.field8 = CAMPAIGN_DIALOG_RESTART;
        message.field18 = CAMPAIGN_MESSAGE_DESELECT;
        campWin->BroadcastMessage(message);
    }
    if (gbLowMemory) {
        message.field4 = CAMPAIGN_MESSAGE_DESELECT;
        message.field8 = CAMPAIGN_DIALOG_REPLAY;
        message.field18 = CAMPAIGN_MESSAGE_DESELECT;
        campWin->BroadcastMessage(message);
    }
    gpSoundManager->SwitchAmbientMusic(
        m_campaignType == CAMPAIGN_ROLAND ? CAMPAIGN_GOOD_MUSIC
                                          : CAMPAIGN_EVIL_MUSIC);
    CampaignInfoUpdate(0);
    gpWindowManager->DoDialog(campWin, CampaignHandler, 0);
    delete campWin;
    gbUseEvilInterface = savedInterface;

    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
        NormalDialog("Are you sure you want to restart this scenario?",
                     CAMPAIGN_RESTART_CONFIRM,
                     CAMPAIGN_DIALOG_NO_RESOURCE, CAMPAIGN_DIALOG_NO_RESOURCE,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0);
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

// @match-note: Semantics, frame, and CFG are complete. First residual is
// +0x2c..+0x33 (side/map index evaluation order); relocation review also leaves
// one extra cCampaignName and cCampaignDescription reference. Direct/cached index
// forms, local ordering, and condition polarity were tried.
VA(0x00448443, 0xa0f)
void game::CampaignInfoUpdate(int redraw)
{
    int map;
    tag_message message;
    SCampaignChoice *choice;
    char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];

    message.type = CAMPAIGN_MESSAGE_WIDGET;
    for (map = 0; map < CAMPAIGN_TRACK_POINT_COUNT; ++map) {
        if (m_campaignMapEnabled[iCurViewSide][map]) {
            message.field18 = CAMPAIGN_TRACK_FRAME_COMPLETE;
        } else if (map < CAMPAIGN_REGULAR_MAP_COUNT &&
                   m_campaignScenarioCompleted[
                       map < 4 ? m_campaignStartingSide
                               : m_campaignType][map]) {
            message.field18 = CAMPAIGN_TRACK_FRAME_AVAILABLE;
        } else {
            message.field18 = CAMPAIGN_TRACK_FRAME_LOCKED;
        }
        if (iCurViewMap == static_cast<int>(map)) {
            if (map + 1 == 5 && iCampaignTrackType == 1)
                message.field18 += 12;
            else if (map + 1 == 5 && iCampaignTrackType == 2)
                message.field18 += 9;
            else if (map + 1 > CAMPAIGN_REGULAR_MAP_COUNT) {
                if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                    message.field18 += 6;
                else
                    message.field18 += 3;
            } else if (map + 1 < 5) {
                if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                    message.field18 += 3;
                else
                    message.field18 += 6;
            } else if (m_campaignType == CAMPAIGN_ROLAND) {
                message.field18 += 3;
            } else {
                message.field18 += 6;
            }
        }
        message.field4 = CAMPAIGN_MESSAGE_SET_FRAME;
        message.field8 = map + CAMPAIGN_TRACK_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    message.field4 = CAMPAIGN_MESSAGE_SET_ICON;
    message.field8 = CAMPAIGN_TRACK_ICON_WIDGET;
    message.text = gText;
    sprintf(gText, "ctrack%02d.icn", iCampaignTrackType);
    campWin->BroadcastMessage(message);

    message.field4 = CAMPAIGN_MESSAGE_SET_TEXT;
    message.text = gText;
    message.field8 = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP)
        sprintf(gText, "5");
    else
        sprintf(gText, "%d", iCurViewMap + 1);
    campWin->BroadcastMessage(message);

    message.field8 = CAMPAIGN_SCENARIO_NAME_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        sprintf(gText, "%s", cCampaignName[1 - iCurViewSide]
                                            [CAMPAIGN_SWITCHING_MAP]);
    } else if (m_campaignType != m_campaignStartingSide &&
               iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        sprintf(gText, "%s", cCampaignName[iCurViewSide]
                                            [CAMPAIGN_SWITCHING_MAP]);
    } else {
        sprintf(gText, "%s", cCampaignName[iCurViewSide][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.field8 = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        sprintf(gText, "%s", cCampaignDescription[1 - iCurViewSide]
                                                   [CAMPAIGN_SWITCHING_MAP]);
    } else if (m_campaignType != m_campaignStartingSide &&
               iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        sprintf(gText, "%s", cCampaignDescription[iCurViewSide]
                                                   [CAMPAIGN_SWITCHING_MAP]);
    } else {
        sprintf(gText, "%s", cCampaignDescription[iCurViewSide][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.field8 = CAMPAIGN_SCENARIO_BONUS_WIDGET;
    sprintf(gText, "%d", m_campaignScenarioBonus[iCurViewSide][iCurViewMap]);
    campWin->BroadcastMessage(message);

    strcpy(gText, "");
    for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map) {
        if (m_campaignAwards[map]) {
            strcat(gText, cCampaignAwards[map]);
            strcat(gText, "\n");
        }
    }
    message.field8 = CAMPAIGN_AWARDS_WIDGET;
    campWin->BroadcastMessage(message);

    for (map = 0; map < CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
            choice = &campaignChoices[1 - iCurViewSide][iCurViewMap][map];
        } else if (m_campaignType != m_campaignStartingSide &&
                   iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
            choice = &campaignChoices[iCurViewSide]
                                     [CAMPAIGN_SWITCHING_MAP][map];
        } else {
            choice = &campaignChoices[iCurViewSide][iCurViewMap][map];
        }

        switch (choice->type) {
        case CAMPAIGN_CHOICE_RESOURCE:
            sprintf(gText, "%d %s", choice->amount,
                    gResourceNames[choice->value]);
            break;
        case CAMPAIGN_CHOICE_ARTIFACT:
            switch (choice->value) {
            case EVENT_ARTIFACT_MAGE_RING: strcpy(gText, "Mage's Ring"); break;
            case EVENT_ARTIFACT_FIZBIN_MISFORTUNE: strcpy(gText, "Fizbin Medal"); break;
            case EVENT_ARTIFACT_THUNDER_MACE: strcpy(gText, "Thunder Mace"); break;
            case EVENT_ARTIFACT_ARMORED_GAUNTLETS: strcpy(gText, "Gauntlets"); break;
            case EVENT_ARTIFACT_DEFENDER_HELM: strcpy(gText, "Defender Helm"); break;
            case EVENT_ARTIFACT_DRAGON_SWORD: strcpy(gText, "Dragon Sword"); break;
            case EVENT_ARTIFACT_POWER_AXE: strcpy(gText, "Power Axe"); break;
            case EVENT_ARTIFACT_DIVINE_BREASTPLATE: strcpy(gText, "Breastplate"); break;
            case EVENT_ARTIFACT_MINOR_SCROLL: strcpy(gText, "Minor Scroll"); break;
            case EVENT_ARTIFACT_HIDEOUS_MASK:
            case EVENT_ARTIFACT_BLACK_PEARL:
            default: sprintf(gText, "%s", gArtifactNames[choice->value]); break;
            }
            break;
        case CAMPAIGN_CHOICE_SPELL:
            if (choice->value == CAMPAIGN_SUMMON_EARTH_SPELL)
                sprintf(gText, "Summon Earth");
            else
                sprintf(gText, "%s", gSpellNames[choice->value]);
            break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            sprintf(gText, "%s %s",
                    gSecondarySkillLevels[choice->amount - 1],
                    gSecondarySkills[choice->value]);
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
        message.field8 = map + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    for (map = 0; map < CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        message.field8 = map + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.field4 = CAMPAIGN_MESSAGE_SET_FRAME;
        if (!bCampaignViewOnly &&
            gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap])
            message.field18 = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.field18 = CAMPAIGN_WIDGET_DISABLE_FRAME;
        campWin->BroadcastMessage(message);

        if (m_campaignChoice[iCurViewSide][iCurViewMap] == map)
            message.field4 = CAMPAIGN_MESSAGE_SELECT;
        else
            message.field4 = CAMPAIGN_MESSAGE_DESELECT;
        message.field18 = CAMPAIGN_WIDGET_REFRESH_FRAME;
        campWin->BroadcastMessage(message);
    }
    if (redraw)
        campWin->DrawWindow();
}

// @match-note: Frame/CFG are complete and external relocations were manually
// audited past delinked switch labels. First residual is +0xa8..+0xac (branch
// displacement from later CFG shape); timeout polarity and switch arm order were tried.
VA(0x00448e52, 0x521)
int CampaignHandler(struct tag_message &message)
{
    int map;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    if (giDialogTimeout != 0 && giDialogTimeout < KBTickCount()) {
        message.type = CAMPAIGN_MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.field8;
        message.field8 = CAMPAIGN_CLOSE_COMMAND;
        message.field4 = message.field8;
        giDialogTimeout = 0;
        return CAMPAIGN_HANDLER_CLOSE;
    }
    if (message.type == CAMPAIGN_MESSAGE_WIDGET) {
      switch (message.field4) {
    case CAMPAIGN_MESSAGE_HOVER:
    case CAMPAIGN_MESSAGE_HELP:
        switch (message.field8) {
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
            map = message.field8 - CAMPAIGN_TRACK_WIDGET_FIRST;
            if (giDebugLevel < 1 &&
                !gpGame->m_campaignMapEnabled[iCurViewSide][map]) {
                if (map < CAMPAIGN_REGULAR_MAP_COUNT) {
                    if (!gpGame->m_campaignScenarioCompleted[
                            map < 4 ? gpGame->m_campaignStartingSide
                                    : gpGame->m_campaignType][map])
                        break;
                } else {
                    break;
                }
            }
            iCurViewMap = map;
            iCurViewSide = iCurViewMap < 4 ? gpGame->m_campaignStartingSide
                                           : gpGame->m_campaignType;
            gpGame->CampaignInfoUpdate(1);
            break;
        case CAMPAIGN_BONUS_WIDGET_FIRST:
        case CAMPAIGN_BONUS_WIDGET_FIRST + 1:
        case CAMPAIGN_BONUS_WIDGET_LAST:
            if (!bCampaignViewOnly &&
                gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap]) {
                gpGame->m_campaignChoice[iCurViewSide][iCurViewMap] =
                    static_cast<unsigned char>(message.field8 -
                                               CAMPAIGN_BONUS_WIDGET_FIRST);
                gpGame->CampaignInfoUpdate(1);
            }
            break;
        }
        break;

    case CAMPAIGN_MESSAGE_ACTIVATE:
        switch (message.field8) {
        case CAMPAIGN_DIALOG_REPLAY:
            gpGame->PlayPreScenarioSmacker(iCurViewSide, iCurViewMap);
            campWin->DrawWindow();
            break;
        case CAMPAIGN_DIALOG_ACCEPT:
            if (!bCampaignViewOnly) {
                if (gpGame->m_campaignMapEnabled[iCurViewSide][iCurViewMap]) {
                    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
                        gpGame->m_campaignScenario = CAMPAIGN_SWITCHING_SCENARIO;
                        gpGame->m_campaignType = static_cast<unsigned char>(
                            1 - gpGame->m_campaignType);
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
                        gpGame->m_campaignScenario =
                            static_cast<signed char>(iCurViewMap);
                        gpGame->m_campaignType =
                            static_cast<unsigned char>(iCurViewSide);
                    }
                } else {
                    NormalDialog(
                        "The currently selected map is not a valid choice for your next scenario.",
                        NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_RESOURCE,
                        0, NORMAL_DIALOG_NO_RESOURCE, 0,
                        NORMAL_DIALOG_NO_RESOURCE, 0);
                    break;
                }
            }
        case CAMPAIGN_DIALOG_CANCEL:
        case CAMPAIGN_DIALOG_RESTART:
            gpWindowManager->m_dialogResult = message.field8;
            message.field8 = CAMPAIGN_CLOSE_COMMAND;
            message.field4 = message.field8;
            giDialogTimeout = 0;
            return CAMPAIGN_HANDLER_CLOSE;
        }
        break;
      }
    }
    return CAMPAIGN_HANDLER_CONTINUE;
}

VA(0x00449373, 0x47)
void game::InitEntireCampaign(int side)
{
    memset(&m_campaignType, 0, CAMPAIGN_STATE_RESET_SIZE);
    m_campaignType = static_cast<unsigned char>(side);
    m_campaignStartingSide = static_cast<unsigned char>(side);
    m_campaignScenario = CAMPAIGN_NO_SCENARIO;
}

// @match-note: Full frame/slot use and CFG are reconstructed; external relocations
// were manually audited past delinked local labels. First residual is +0x0c..+0x10
// (side/map load evaluation order); direct/cached choice indexing, declaration
// order, and obvious condition polarities were tried.
VA(0x004493ba, 0xbb7)
void game::InitCampaignMap(void)
{
    int selectedChoice;
    SCampaignChoice *choice;
    int mapHeaderResult;
    int playerSlot;
    int bestHeroPosition;
    playerRec *campaignPlayer;
    int heroPosition;
    int scanPosition;
    int heroPriority;
    int bestHeroPriority;
    int swappedHero;
    int bonusHeroIndex;

    selectedChoice = m_campaignChoice[iCurViewSide][iCurViewMap];
    if (m_campaignType != m_campaignStartingSide &&
        iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        choice = &campaignChoices[iCurViewSide][CAMPAIGN_SWITCHING_MAP]
                                 [selectedChoice];
    } else {
        choice = &campaignChoices[m_campaignType][m_campaignScenario]
                                 [selectedChoice];
    }

    gpGame->m_campaignScenarioWon = 0;
    memset(m_setupPlayerColor, 0, CAMPAIGN_SETUP_RESET_SIZE);
    if (m_campaignScenario + 1 == CAMPAIGN_SWITCHING_SCENARIO + 1 &&
        m_campaignStartingSide != m_campaignType) {
        sprintf(m_mapFilename, "CAMP%c%02dB.H2C",
                m_campaignType == CAMPAIGN_ROLAND ? 'E' : 'G',
                m_campaignScenario + 1);
    } else {
        sprintf(m_mapFilename, "CAMP%c%02d.H2C",
                m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E',
                m_campaignScenario + 1);
    }
    m_newGameInitialized = 0;
    if (m_campaignScenario == 0)
        m_campaignScore = 0;
    strcpy(gMapName, m_mapFilename);
    mapHeaderResult = GetMapHeader(m_mapFilename, &m_mapHeader);
    LoadGame("origdata.bin", 1, 0);
    InitNewGame(0);

    if (choice->type == CAMPAIGN_CHOICE_ALIGNMENT) {
        playerSlot = 0;
        if (m_campaignType == CAMPAIGN_ARCHIBALD) {
            if (m_mapHeader.playerEnabled[0])
                ++playerSlot;
            if (m_mapHeader.playerEnabled[1])
                ++playerSlot;
        }
        m_setupPlayerRace[playerSlot] = static_cast<signed char>(choice->value);
    }

    if (m_campaignScenario + 1 <= CAMPAIGN_EASY_SCENARIO_LIMIT)
        gpGame->m_difficulty = CAMPAIGN_DIFFICULTY_EASY;
    else if (m_campaignScenario + 1 <= CAMPAIGN_NORMAL_SCENARIO_LIMIT)
        gpGame->m_difficulty = CAMPAIGN_DIFFICULTY_NORMAL;
    else
        gpGame->m_difficulty = CAMPAIGN_DIFFICULTY_HARD;
    m_playerCount = m_mapHeader.playerCount;
    NewMap(gMapName);

    bestHeroPosition = 0;
    campaignPlayer = &gpGame->m_players[0];
    for (heroPosition = 0; heroPosition < campaignPlayer->heroCount;
         ++heroPosition) {
        bestHeroPriority = -1;
        for (scanPosition = heroPosition;
             scanPosition < campaignPlayer->heroCount; ++scanPosition) {
            if (gpGame->m_heroRecs[campaignPlayer->heroes[scanPosition]].m_portrait ==
                    CAMPAIGN_HERO_ROLAND ||
                gpGame->m_heroRecs[campaignPlayer->heroes[scanPosition]].m_portrait ==
                    CAMPAIGN_HERO_ARCHIBALD) {
                heroPriority = CAMPAIGN_HERO_PRIORITY_HIGH;
            } else if (
                gpGame->m_heroRecs[campaignPlayer->heroes[scanPosition]].m_portrait ==
                    CAMPAIGN_HERO_CORLAGON ||
                gpGame->m_heroRecs[campaignPlayer->heroes[scanPosition]].m_portrait ==
                    CAMPAIGN_HERO_HALTON) {
                heroPriority = CAMPAIGN_HERO_PRIORITY_NORMAL;
            } else {
                heroPriority = 0;
            }
            if (bestHeroPriority < heroPriority) {
                bestHeroPriority = heroPriority;
                bestHeroPosition = scanPosition;
            }
        }
        if (bestHeroPriority != -1) {
            swappedHero = campaignPlayer->heroes[heroPosition];
            campaignPlayer->heroes[heroPosition] =
                campaignPlayer->heroes[bestHeroPosition];
            campaignPlayer->heroes[bestHeroPosition] =
                static_cast<signed char>(swappedHero);
        }
    }
    if (campaignPlayer->heroCount)
        campaignPlayer->currentHero = campaignPlayer->heroes[0];

    switch (choice->type) {
    case CAMPAIGN_CHOICE_RESOURCE:
        m_players[0].resources[choice->value] += choice->amount;
        break;
    case CAMPAIGN_CHOICE_ARTIFACT:
        if (m_players[0].heroCount > 0)
            GiveArtifact(GetPlayerHero(0, 0), choice->value, 0, -1);
        break;
    case CAMPAIGN_CHOICE_SPELL:
        if (m_players[0].heroCount > 0) {
            bonusHeroIndex = 0;
            if (m_campaignType == CAMPAIGN_ROLAND &&
                m_campaignScenario + 1 == 6 &&
                m_players[0].heroCount > 1)
                bonusHeroIndex = 1;
            GetPlayerHero(0, bonusHeroIndex)->m_spells[choice->value] = 1;
        }
        break;
    case CAMPAIGN_CHOICE_SECONDARY_SKILL:
        if (m_players[0].heroCount > 0)
            GetPlayerHero(0, 0)->SetSS(choice->value, choice->amount);
        break;
    case CAMPAIGN_CHOICE_CREATURES:
        if (m_players[0].heroCount > 0)
            GetPlayerHero(0, 0)->m_army.Add(choice->value, choice->amount, -1);
        break;
    case CAMPAIGN_CHOICE_PUZZLE_PIECES:
        m_players[0].unknown13 = static_cast<signed char>(choice->value);
        break;
    case CAMPAIGN_CHOICE_EXPERIENCE: {
        int savedNewGameSetup = gbInNewGameSetup;
        gbInNewGameSetup = 1;
        if (m_players[0].heroCount > 0) {
            GetPlayerHero(0, 0)->m_experience += choice->value;
            GetPlayerHero(0, 0)->CheckLevel();
        }
        gbInNewGameSetup = savedNewGameSetup;
        break;
    }
    case CAMPAIGN_CHOICE_NONE:
        break;
    }

    if ((m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN] ||
         (m_campaignAwards[CAMPAIGN_AWARD_DRAGON_SLAYER] &&
          m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)) &&
        m_players[0].heroCount > 0) {
        GiveArtifact(GetPlayerHero(0, 0), EVENT_ARTIFACT_ULTIMATE_CROWN, 0, -1);
    }
    gbRetreatWin = 1;

    if (m_campaignAwards[CAMPAIGN_AWARD_DEFEAT_CORLAGON]) {
        for (heroPosition = 0; heroPosition < CAMPAIGN_HERO_COUNT;
             ++heroPosition) {
            if (gpGame->m_heroRecs[heroPosition].m_portrait ==
                CAMPAIGN_HERO_CORLAGON)
                gpGame->m_heroRecs[heroPosition].Deallocate(0);
        }
    }

    if (m_campaignAwards[CAMPAIGN_AWARD_TRIPLE_ARMY]) {
        hero *armyHero = GetPlayerHero(CAMPAIGN_CARRYOVER_PLAYER, 0);
        for (heroPosition = 0; heroPosition < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPosition) {
            if (armyHero->m_army.m_creatureCounts[heroPosition] > 0)
                armyHero->m_army.m_creatureCounts[heroPosition] *=
                    CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
        }
    }

    if (m_campaignScenario + 1 == 7 &&
        m_campaignType == CAMPAIGN_ARCHIBALD) {
        int savedNewGameSetup = gbInNewGameSetup;
        hero *armyHero;
        gbInNewGameSetup = 1;
        armyHero = GetPlayerHero(0, 0);
        for (heroPosition = 0; heroPosition < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPosition) {
            armyHero->m_army.m_creatureTypes[heroPosition] = ARMY_GROUP_EMPTY_SLOT;
            armyHero->m_army.m_creatureCounts[heroPosition] = 0;
        }
        switch (armyHero->m_cursorType) {
        case HERO_CLASS_BARBARIAN:
            armyHero->m_army.Add(ARMY_CREATURE_ORC_CHIEF,
                                 CAMPAIGN_BARBARIAN_ORC_CHIEF_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_OGRE,
                                 CAMPAIGN_BARBARIAN_OGRE_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_GOBLIN,
                                 CAMPAIGN_BARBARIAN_GOBLIN_COUNT, -1);
            break;
        case HERO_CLASS_WARLOCK:
            armyHero->m_army.Add(ARMY_CREATURE_CENTAUR,
                                 CAMPAIGN_WARLOCK_CENTAUR_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_GARGOYLE,
                                 CAMPAIGN_WARLOCK_GARGOYLE_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_GRIFFIN,
                                 CAMPAIGN_WARLOCK_GRIFFIN_COUNT, -1);
            break;
        case HERO_CLASS_NECROMANCER:
            armyHero->m_army.Add(ARMY_CREATURE_SKELETON,
                                 CAMPAIGN_NECROMANCER_SKELETON_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_ROYAL_MUMMY,
                                 CAMPAIGN_NECROMANCER_MUMMY_COUNT, -1);
            armyHero->m_army.Add(ARMY_CREATURE_VAMPIRE_LORD,
                                 CAMPAIGN_NECROMANCER_VAMPIRE_COUNT, -1);
            break;
        }
        GetPlayerHero(0, 0)->m_experience += CAMPAIGN_EXPERIENCE_BONUS;
        GetPlayerHero(0, 0)->CheckLevel();
        gbInNewGameSetup = savedNewGameSetup;
    }

    if ((m_campaignAwards[CAMPAIGN_AWARD_CARRYOVER_ARMY] &&
         m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1) ||
        m_campaignAwards[CAMPAIGN_AWARD_ULTIMATE_CROWN_ALT]) {
        hero *armyHero = GetPlayerHero(0, 0);
        for (heroPosition = 0; heroPosition < CAMPAIGN_ARMY_SLOT_COUNT;
             ++heroPosition) {
            armyHero->m_army.m_creatureTypes[heroPosition] =
                static_cast<signed char>(m_campaignCarryoverCreatureTypes[heroPosition]);
            armyHero->m_army.m_creatureCounts[heroPosition] =
                (m_campaignAwards[CAMPAIGN_AWARD_TRIPLE_ARMY]
                     ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
                     : 1) *
                m_campaignCarryoverCreatureCounts[heroPosition];
        }
    }

    if (m_campaignType == CAMPAIGN_ARCHIBALD &&
        m_campaignScenario + 1 == 6) {
        gpGame->m_mapHeader.victoryCondition = MAP_VICTORY_DEFEAT_SIDE;
        gpGame->m_mapHeader.victoryConditionValue =
            CAMPAIGN_SWITCH_VICTORY_VALUE;
        gpGame->m_mapHeader.allowNormalVictory = 1;
    }
    if (m_campaignType == CAMPAIGN_ROLAND &&
        m_campaignScenario + 1 == 9) {
        gpGame->m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
        gpGame->m_mapHeader.lossConditionValue = 0;
    }
    if (m_campaignType == CAMPAIGN_ROLAND &&
        m_campaignScenario + 1 == 7)
        gpGame->m_mapHeader.lossConditionValue = CAMPAIGN_ROLAND_TIME_LIMIT;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f4f28) short trackXY[2][13][2];
DATA(0x004f4f90) class heroWindow *campWin;
DATA(0x00527ea4) int iCurViewSide;
DATA(0x00527ea8) int iCampaignTrackType;
DATA(0x00527eac) int bCampaignViewOnly;
DATA(0x00527eb0) int iCurViewMap;
