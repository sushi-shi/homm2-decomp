#include <H2/Ints.h>
#include <stdio.h>
#include <string.h>
#include <BASE/message.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <SOURCE/army.h>
#include <BASE/Utf8.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/smackManager.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/game.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/Localization.h>

#include <string>
#include <BASE/dialog.h>

typedef enum CampaignScenarioArmyCount {
    BARBARIAN_ORC_CHIEF_COUNT  = 12,
    BARBARIAN_OGRE_COUNT       = 18,
    BARBARIAN_GOBLIN_COUNT     = 40,
    WARLOCK_CENTAUR_COUNT      = 40,
    WARLOCK_GARGOYLE_COUNT     = 24,
    WARLOCK_GRIFFIN_COUNT      = 18,
    NECROMANCER_SKELETON_COUNT = 50,
    NECROMANCER_MUMMY_COUNT    = 18,
    NECROMANCER_VAMPIRE_COUNT  = 8
} CampaignScenarioArmyCount;

typedef enum CampaignMapIndex {
    MAP_ONE      = 0,
    MAP_TWO      = 1,
    MAP_THREE    = 2,
    MAP_FOUR     = 3,
    MAP_FIVE     = 4,
    MAP_SIX      = 5,
    MAP_SEVEN    = 6,
    MAP_EIGHT    = 7,
    MAP_NINE     = 8,
    MAP_TEN      = 9,
    MAP_ELEVEN   = 10,
    MAP_TWELVE   = 11,
    MAP_THIRTEEN = 12
} CampaignMapIndex;

typedef enum CampaignScenarioNumber {
    SCENARIO_INTRO    = 0,
    SCENARIO_ONE      = 1,
    SCENARIO_TWO      = 2,
    SCENARIO_THREE    = 3,
    SCENARIO_FOUR     = 4,
    SCENARIO_FIVE     = 5,
    SCENARIO_SIX      = 6,
    SCENARIO_SEVEN    = 7,
    SCENARIO_EIGHT    = 8,
    SCENARIO_NINE     = 9,
    SCENARIO_TEN      = 10,
    SCENARIO_ELEVEN   = 11,
    SCENARIO_TWELVE   = 12,
    SCENARIO_THIRTEEN = 13
} CampaignScenarioNumber;

enum class CampaignTrackType : i32 {
    ROLAND_TO_ROLAND             = 0,
    ROLAND_TO_ARCHIBALD          = 1,
    ARCHIBALD_TO_ROLAND          = 2,
    ARCHIBALD_TO_ARCHIBALD       = 3,
    SWITCH_TO_ROLAND             = 4,
    SWITCH_TO_ARCHIBALD_COMPLETE = 5,
    SWITCH_TO_ARCHIBALD_OPEN     = 6
};
using enum CampaignTrackType;

typedef enum CampaignTrackConstant {
    TRACK_ICON_FILL_COLOR            = 1,
    TRACK_SELECTED_FRAME_ROLAND    = 3,
    TRACK_SELECTED_FRAME_ARCHIBALD   = 6,
    TRACK_SELECTED_FRAME_ARCHIBALD_TO_ROLAND = 9,
    TRACK_SELECTED_FRAME_ROLAND_TO_ARCHIBALD  = 12
} CampaignTrackConstant;

i32 game::HandleCampaignWin(void) {
    H2SteppedEnumStorage<CampaignSide, i32> sideIndex;
    i32 mapIndex;

    memset(m_campaignMapEnabled, 0, sizeof(m_campaignMapEnabled));
    if (m_campaignType == CAMPAIGN_ROLAND) {
        switch (m_campaignScenario + 1) {
            case SCENARIO_INTRO:
                PlaySmacker(SMACKER_ROLAND_INTRO);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_ONE] = 1;
                break;
            case SCENARIO_ONE:
                PlaySmacker(SMACKER_ROLAND_1);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_TWO] = 1;
                break;
            case SCENARIO_TWO:
                PlaySmacker(SMACKER_ROLAND_2);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_THREE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_FOUR] = 1;
                break;
            case SCENARIO_THREE:
                PlaySmacker(SMACKER_ROLAND_3B);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_FOUR] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)] = 1;
                break;
            case SCENARIO_FOUR:
                PlaySmacker(SMACKER_ROLAND_4);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_FIVE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_TWELVE] = 1;
                break;
            case SCENARIO_FIVE:
                if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                    PlaySmacker(SMACKER_ROLAND_5A);
                else
                    PlaySmacker(SMACKER_ROLAND_5B);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_SIX] = 1;
                break;
            case SCENARIO_SIX:
                PlaySmacker(SMACKER_ROLAND_6);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_SEVEN] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_EIGHT] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_SORCERESS_GUILD)] = 1;
                break;
            case SCENARIO_SEVEN:
                PlaySmacker(SMACKER_ROLAND_8);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_NINE] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)] = 1;
                break;
            case SCENARIO_EIGHT:
                PlaySmacker(SMACKER_ROLAND_8);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_NINE] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)] = 1;
                break;
            case SCENARIO_NINE:
                PlaySmacker(SMACKER_ROLAND_9);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ROLAND)][MAP_TEN] = 1;
                break;
            case SCENARIO_TEN:
                PlaySmacker(SMACKER_ROLAND_END);
                break;
        }
    } else {
        switch (m_campaignScenario + 1) {
            case SCENARIO_INTRO:
                PlaySmacker(SMACKER_ARCHIBALD_INTRO);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_ONE] = 1;
                break;
            case SCENARIO_ONE:
                PlaySmacker(SMACKER_ARCHIBALD_1);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_TWO] = 1;
                break;
            case SCENARIO_TWO:
                PlaySmacker(SMACKER_ARCHIBALD_2);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_THREE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_FOUR] = 1;
                break;
            case SCENARIO_THREE:
                PlaySmacker(SMACKER_ARCHIBALD_4A);
                PlaySmacker(SMACKER_ARCHIBALD_4_END);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_FIVE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_TWELVE] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_NECROMANCER_GUILD)] = 1;
                break;
            case SCENARIO_FOUR:
                PlaySmacker(SMACKER_ARCHIBALD_4B);
                PlaySmacker(SMACKER_ARCHIBALD_4_END);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_FIVE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_TWELVE] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DWARFBANE)] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_OGRE_ALLIANCE)] = 1;
                break;
            case SCENARIO_FIVE:
                if (m_campaignStartingSide == CAMPAIGN_ARCHIBALD)
                    PlaySmacker(SMACKER_ARCHIBALD_5A);
                else
                    PlaySmacker(SMACKER_ARCHIBALD_5B);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_SIX] = 1;
                break;
            case SCENARIO_SIX:
                PlaySmacker(SMACKER_ARCHIBALD_6);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_SEVEN] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_EIGHT] = 1;
                break;
            case SCENARIO_SEVEN:
                PlaySmacker(SMACKER_ARCHIBALD_7B);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_EIGHT] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_DRAGON_ALLIANCE)] = 1;
                break;
            case SCENARIO_EIGHT:
                PlaySmacker(SMACKER_ARCHIBALD_8);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_NINE] = 1;
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_TEN] = 1;
                break;
            case SCENARIO_NINE:
                PlaySmacker(SMACKER_ARCHIBALD_10);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_ELEVEN] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 1;
                break;
            case SCENARIO_TEN:
                PlaySmacker(SMACKER_ARCHIBALD_10);
                m_campaignMapEnabled[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_ELEVEN] = 1;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 1;
                break;
            case SCENARIO_ELEVEN:
                PlaySmacker(SMACKER_ARCHIBALD_END);
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 0;
                m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 0;
                break;
        }
    }

    if (m_campaignScenario + 1 != CAMPAIGN_ARCHIBALD_FINAL_SCENARIO + 1
        && (gpGame->m_campaignScenario + 1 != CAMPAIGN_ROLAND_FINAL_SCENARIO + 1
            || gpGame->m_campaignType != CAMPAIGN_ROLAND)) {
        m_campaignScenario = CAMPAIGN_NO_SCENARIO;
        for (sideIndex = CAMPAIGN_ROLAND; sideIndex < CAMPAIGN_SIDE_COUNT; ++sideIndex) {
            for (mapIndex = 0; mapIndex < CAMPAIGN_REGULAR_MAP_COUNT; ++mapIndex) {
                if (m_campaignMapEnabled[H2EnumIndex(sideIndex)][mapIndex]) {
                    gpGame->m_campaignDaysBeforeScenario[H2EnumIndex(sideIndex)][mapIndex] =
                        m_campaignTotalDays;
                    if (m_campaignScenario == CAMPAIGN_NO_SCENARIO) {
                        m_campaignType = sideIndex;
                        m_campaignScenario = mapIndex;
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

void game::PlayPreScenarioSmacker(CampaignSide side, i32 map) {
    if (side == CAMPAIGN_ROLAND) {
        switch (map + 1) {
            case SCENARIO_ONE:
                PlaySmacker(SMACKER_ROLAND_INTRO);
                break;
            case SCENARIO_TWO:
                PlaySmacker(SMACKER_ROLAND_1);
                break;
            case SCENARIO_THREE:
                PlaySmacker(SMACKER_ROLAND_2);
                break;
            case SCENARIO_FOUR:
                if (m_campaignScenarioCompleted[H2EnumIndex(m_campaignStartingSide)][MAP_THREE])
                    PlaySmacker(SMACKER_ROLAND_3B);
                else
                    PlaySmacker(SMACKER_ROLAND_3A);
                break;
            case SCENARIO_FIVE:
                PlaySmacker(SMACKER_ROLAND_4);
                break;
            case SCENARIO_SIX:
                if (m_campaignStartingSide == CAMPAIGN_ROLAND)
                    PlaySmacker(SMACKER_ROLAND_5A);
                else
                    PlaySmacker(SMACKER_ROLAND_5B);
                break;
            case SCENARIO_SEVEN:
                PlaySmacker(SMACKER_ROLAND_6);
                break;
            case SCENARIO_EIGHT:
                PlaySmacker(SMACKER_ROLAND_7);
                break;
            case SCENARIO_NINE:
                PlaySmacker(SMACKER_ROLAND_8);
                break;
            case SCENARIO_TEN:
                PlaySmacker(SMACKER_ROLAND_9);
                break;
            case SCENARIO_TWELVE:
            case SCENARIO_THIRTEEN:
                PlaySmacker(SMACKER_ROLAND_4);
                break;
        }
    } else {
        switch (map + 1) {
            case SCENARIO_ONE:
                PlaySmacker(SMACKER_ARCHIBALD_INTRO);
                break;
            case SCENARIO_TWO:
                PlaySmacker(SMACKER_ARCHIBALD_1);
                break;
            case SCENARIO_THREE:
                PlaySmacker(SMACKER_ARCHIBALD_2);
                break;
            case SCENARIO_FOUR:
                PlaySmacker(SMACKER_ARCHIBALD_3);
                break;
            case SCENARIO_FIVE:
                if (m_campaignScenarioCompleted[H2EnumIndex(m_campaignStartingSide)][MAP_FOUR])
                    PlaySmacker(SMACKER_ARCHIBALD_4B);
                else
                    PlaySmacker(SMACKER_ARCHIBALD_4A);
                PlaySmacker(SMACKER_ARCHIBALD_4_END);
                break;
            case SCENARIO_SIX:
                if (m_campaignStartingSide == CAMPAIGN_ARCHIBALD)
                    PlaySmacker(SMACKER_ARCHIBALD_5A);
                else
                    PlaySmacker(SMACKER_ARCHIBALD_5B);
                break;
            case SCENARIO_SEVEN:
                PlaySmacker(SMACKER_ARCHIBALD_6);
                break;
            case SCENARIO_EIGHT:
                if (m_campaignScenarioCompleted[H2EnumIndex(m_campaignType)][MAP_SEVEN])
                    PlaySmacker(SMACKER_ARCHIBALD_7B);
                else
                    PlaySmacker(SMACKER_ARCHIBALD_7A);
                break;
            case SCENARIO_NINE:
                PlaySmacker(SMACKER_ARCHIBALD_8);
                break;
            case SCENARIO_TEN:
                PlaySmacker(SMACKER_ARCHIBALD_9);
                break;
            case SCENARIO_ELEVEN:
                PlaySmacker(SMACKER_ARCHIBALD_10);
                break;
            case SCENARIO_TWELVE:
            case SCENARIO_THIRTEEN:
                if (m_campaignScenarioCompleted[H2EnumIndex(m_campaignStartingSide)][MAP_FOUR])
                    PlaySmacker(SMACKER_ARCHIBALD_4B);
                else
                    PlaySmacker(SMACKER_ARCHIBALD_4A);
                PlaySmacker(SMACKER_ARCHIBALD_4_END);
                break;
        }
    }
    gpWindowManager->m_colorCycling = 1;
}

void game::ShowCampaignInfo(i32 viewOnly, i32) {
    widget* trackWidget;
    i32 mapIndex;
    b32 savedInterface;
    tag_message message;
    i32 trackMapIndex;

    gpMouseManager->SetPointer("advmice.mse", ADVENTURE_POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->ReallyShowPointer();
    savedInterface = gbUseEvilInterface;
    gbUseEvilInterface = m_campaignType == CAMPAIGN_ARCHIBALD;
    bCampaignViewOnly = viewOnly;
    iCurViewSide = m_campaignType;
    iCurViewMap = m_campaignScenario;
    if (m_campaignScenario == CAMPAIGN_SWITCHING_SCENARIO && !viewOnly) {
        if (m_campaignType == CAMPAIGN_ROLAND)
            iCampaignTrackType = SWITCH_TO_ROLAND;
        else if (m_campaignScenarioCompleted[H2EnumIndex(CAMPAIGN_ARCHIBALD)][MAP_THREE])
            iCampaignTrackType = SWITCH_TO_ARCHIBALD_COMPLETE;
        else
            iCampaignTrackType = SWITCH_TO_ARCHIBALD_OPEN;
    } else {
        iCampaignTrackType = CampaignTrackTypeFromOrdinal(
            H2EnumIndex(m_campaignStartingSide) * H2EnumIndex(CAMPAIGN_SIDE_COUNT) + H2EnumIndex(m_campaignType)
        );
    }

    campWin = new heroWindow(0, 0, "campaign.bin");
    if (campWin == NULL)
        MemError();
    trackWidget = NULL;
    for (mapIndex = 0; mapIndex < CAMPAIGN_TRACK_POINT_COUNT; ++mapIndex) {
        if (H2EnumIndex(iCampaignTrackType) < H2EnumIndex(SWITCH_TO_ROLAND)
            && mapIndex >= CAMPAIGN_REGULAR_MAP_COUNT)
            continue;
        if (iCampaignTrackType == SWITCH_TO_ROLAND && mapIndex == MAP_THIRTEEN)
            continue;
        if (iCampaignTrackType == SWITCH_TO_ARCHIBALD_COMPLETE && mapIndex == MAP_THIRTEEN)
            continue;
        if (iCampaignTrackType == SWITCH_TO_ARCHIBALD_OPEN && mapIndex == MAP_TWELVE)
            continue;
        if (trackXY[H2EnumIndex(iCurViewSide)][mapIndex][H2EnumIndex(COORDINATE_AXIS_X)] != -1) {
            trackMapIndex = mapIndex;
            if (trackMapIndex > CAMPAIGN_REGULAR_MAP_COUNT)
                trackMapIndex = CAMPAIGN_REGULAR_MAP_COUNT;
            trackWidget = new iconWidget(
                trackXY[H2EnumIndex(mapIndex < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
                                                               : m_campaignType)][mapIndex][H2EnumIndex(COORDINATE_AXIS_X)]
                    - CAMPAIGN_TRACK_ICON_OFFSET,
                trackXY[H2EnumIndex(mapIndex < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
                                                               : m_campaignType)][mapIndex][H2EnumIndex(COORDINATE_AXIS_Y)]
                    - CAMPAIGN_TRACK_ICON_OFFSET,
                CAMPAIGN_TRACK_ICON_SIZE,
                CAMPAIGN_TRACK_ICON_SIZE,
                "campxtrg.icn",
                CAMPAIGN_TRACK_ICON_FRAME,
                ICON_DRAW_NORMAL,
                trackMapIndex + CAMPAIGN_TRACK_WIDGET_FIRST,
                WIDGET_KIND_ICON_DIRECT,
                TRACK_ICON_FILL_COLOR
            );
            if (trackWidget == NULL)
                MemError();
            campWin->AddWidget(trackWidget, -1);
        }
    }

    message.type = MESSAGE_WIDGET;
    if (!viewOnly) {
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        campWin->BroadcastMessage(message);
    }
    gpSoundManager->SwitchAmbientMusic(
        m_campaignType == CAMPAIGN_ROLAND ? MUSIC_TRACK_CAMPAIGN_GOOD : MUSIC_TRACK_CAMPAIGN_EVIL
    );
    CampaignInfoUpdate(0);
    gpWindowManager->DoDialog(campWin, CampaignHandler, 0);
    delete campWin;
    gbUseEvilInterface = savedInterface;

    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
        NormalDialog(localization::Tr("campaign.confirm.restart_scenario"), NORMAL_DIALOG_CONFIRM);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES) {
            InitCampaignMap();
            PRESENT_RESTARTED_CAMPAIGN_MAP();
        }
    }
}

void game::CampaignInfoUpdate(i32 redraw) {
    i32 index;
    SCampaignChoice* choice;
    tag_message message;
    char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];

    message.type = MESSAGE_WIDGET;
    for (index = 0; index < CAMPAIGN_TRACK_POINT_COUNT; ++index) {
        if (index < CAMPAIGN_MAP_COUNT
            && m_campaignMapEnabled[H2EnumIndex(iCurViewSide)][index]) {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_AVAILABLE;
        } else if (index < CAMPAIGN_REGULAR_MAP_COUNT
                   && m_campaignScenarioCompleted
                          [H2EnumIndex(index < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
                                                                 : m_campaignType)][index]) {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_COMPLETE;
        } else {
            message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_LOCKED;
        }
        if (index == iCurViewMap) {
            if (index + 1 == SCENARIO_FIVE && iCampaignTrackType == ROLAND_TO_ARCHIBALD)
                message.payload.widget.data.value += TRACK_SELECTED_FRAME_ROLAND_TO_ARCHIBALD;
            else if (index + 1 == SCENARIO_FIVE
                     && iCampaignTrackType == ARCHIBALD_TO_ROLAND)
                message.payload.widget.data.value += TRACK_SELECTED_FRAME_ARCHIBALD_TO_ROLAND;
            else if (index + 1 > CAMPAIGN_REGULAR_MAP_COUNT)
                message.payload.widget.data.value += m_campaignStartingSide == CAMPAIGN_ROLAND
                                                         ? TRACK_SELECTED_FRAME_ARCHIBALD
                                                         : TRACK_SELECTED_FRAME_ROLAND;
            else if (index + 1 < SCENARIO_FIVE)
                message.payload.widget.data.value += m_campaignStartingSide == CAMPAIGN_ROLAND
                                                         ? TRACK_SELECTED_FRAME_ROLAND
                                                         : TRACK_SELECTED_FRAME_ARCHIBALD;
            else
                message.payload.widget.data.value += m_campaignType == CAMPAIGN_ROLAND
                                                         ? TRACK_SELECTED_FRAME_ROLAND
                                                         : TRACK_SELECTED_FRAME_ARCHIBALD;
        }
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = index + CAMPAIGN_TRACK_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
    message.payload.widget.data.text = gText;
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "ctrack%02d.icn", H2EnumIndex(iCampaignTrackType));
    campWin->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP)
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "5");
    else
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", iCurViewMap + 1);
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", cCampaignName[1 - H2EnumIndex(iCurViewSide)][iCurViewMap]);
    } else if (m_campaignType != m_campaignStartingSide
               && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", cCampaignName[H2EnumIndex(iCurViewSide)][CAMPAIGN_SWITCHING_MAP]);
    } else {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", cCampaignName[H2EnumIndex(iCurViewSide)][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
    if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", cCampaignDescription[1 - H2EnumIndex(iCurViewSide)][iCurViewMap]);
    } else if (m_campaignType != m_campaignStartingSide
               && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        utf8::Format(
            gText, GLOBAL_TEXT_BUFFER_SIZE,
            "%s",
            cCampaignDescription[H2EnumIndex(iCurViewSide)][CAMPAIGN_SWITCHING_MAP]
        );
    } else {
        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", cCampaignDescription[H2EnumIndex(iCurViewSide)][iCurViewMap]);
    }
    campWin->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_DAYS_SPENT_WIDGET;
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%d", m_campaignDaysBeforeScenario[H2EnumIndex(iCurViewSide)][iCurViewMap]);
    campWin->BroadcastMessage(message);

    strcpy(gText, "");
    for (index = 0; index < CAMPAIGN_AWARD_COUNT; ++index) {
        if (m_campaignAwards[index]) {
            strcat(gText, cCampaignAwards[index]);
            strcat(gText, "\n");
        }
    }
    message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
    campWin->BroadcastMessage(message);

    for (index = 0; index < CAMPAIGN_BONUS_CHOICE_COUNT; ++index) {
        if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
            choice = &campaignChoices[1 - H2EnumIndex(iCurViewSide)][iCurViewMap][index];
        } else if (m_campaignType != m_campaignStartingSide
                   && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
            choice = &campaignChoices[H2EnumIndex(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][index];
        } else {
            choice = &campaignChoices[H2EnumIndex(iCurViewSide)][iCurViewMap][index];
        }

        switch (choice->type) {
            case CAMPAIGN_CHOICE_RESOURCE:
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.bonus.resource"),
                    choice->amount,
                    gResourceNames[H2EnumIndex(choice->resource)]
                );
                break;
            case CAMPAIGN_CHOICE_ARTIFACT:
                switch (choice->artifact) {
                    case ARTIFACT_MINOR_SCROLL:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.minor_scroll"));
                        break;
                    case ARTIFACT_MAGE_RING:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.mage_ring"));
                        break;
                    case ARTIFACT_DEFENDER_HELM:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.defender_helm"));
                        break;
                    case ARTIFACT_POWER_AXE:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.power_axe"));
                        break;
                    case ARTIFACT_DRAGON_SWORD:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.dragon_sword"));
                        break;
                    case ARTIFACT_DIVINE_BREASTPLATE:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.breastplate"));
                        break;
                    case ARTIFACT_FIZBIN_OF_MISFORTUNE:
                        utf8::Copy(
                            gText,
                            GLOBAL_TEXT_BUFFER_SIZE,
                            localization::Tr("campaign.bonus.artifact.fizbin_medal")
                        );
                        break;
                    case ARTIFACT_THUNDER_MACE:
                        utf8::Copy(
                            gText,
                            GLOBAL_TEXT_BUFFER_SIZE,
                            localization::Tr("campaign.bonus.artifact.thunder_mace")
                        );
                        break;
                    case ARTIFACT_ARMORED_GAUNTLETS:
                        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.artifact.gauntlets"));
                        break;
                    default:
                        utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", gArtifactNames[H2EnumIndex(choice->artifact)]);
                        break;
                }
                break;
            case CAMPAIGN_CHOICE_SPELL:
                if (choice->spell == SPELL_SUMMON_EARTH_ELEMENTAL)
                    utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("campaign.bonus.spell.summon_earth"));
                else
                    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s", gSpellNames[H2EnumIndex(choice->spell)]);
                break;
            case CAMPAIGN_CHOICE_SECONDARY_SKILL:
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.bonus.secondary_skill"),
                    gSecondarySkillLevels[choice->amount - 1],
                    gSecondarySkills[choice->value]
                );
                break;
            case CAMPAIGN_CHOICE_CREATURES:
                utf8::Copy(
                    armyName,
                    sizeof(armyName),
                    gArmyNamesPlural[H2EnumIndex(choice->creature)]
                );
                utf8::UppercaseFirst(armyName);
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.bonus.creatures"),
                    choice->amount,
                    armyName
                );
                break;
            case CAMPAIGN_CHOICE_PUZZLE_PIECES:
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.bonus.puzzle_pieces"),
                    choice->value
                );
                break;
            case CAMPAIGN_CHOICE_EXPERIENCE:
                utf8::Format(
                    gText, GLOBAL_TEXT_BUFFER_SIZE,
                    localization::Tr("campaign.bonus.experience"),
                    choice->value
                );
                break;
            case CAMPAIGN_CHOICE_NONE:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("common.not_applicable"));
                break;
            case CAMPAIGN_CHOICE_ALIGNMENT:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, gAlignmentNames[H2EnumIndex(choice->faction)]);
                break;
            default:
                break;
        }
        message.payload.widget.id = index + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
        campWin->BroadcastMessage(message);
    }

    for (index = 0; index < CAMPAIGN_BONUS_CHOICE_COUNT; ++index) {
        message.payload.widget.id = index + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        if (!bCampaignViewOnly
            && gpGame->m_campaignMapEnabled[H2EnumIndex(iCurViewSide)][iCurViewMap])
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        campWin->BroadcastMessage(message);

        if (m_campaignChoice[H2EnumIndex(iCurViewSide)][iCurViewMap] == index)
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        else
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_DRAW);
        campWin->BroadcastMessage(message);
    }
    if (redraw)
        campWin->DrawWindow();
}

MessageDispatchResult CampaignHandler(struct tag_message& message) {
    i32 map;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && platform::Ticks() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
        message.payload.widget.command = BaseWidgetCommand(H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT));
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_NOTIFY_SELECT:
            case WIDGET_NOTIFY_RIGHT_CLICK:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_TRACK_WIDGET_FIRST:
                    case CAMPAIGN_TRACK_WIDGET_FIRST + 1:
                    case CAMPAIGN_TRACK_WIDGET_2:
                    case CAMPAIGN_TRACK_WIDGET_3:
                    case CAMPAIGN_TRACK_WIDGET_4:
                    case CAMPAIGN_TRACK_WIDGET_5:
                    case CAMPAIGN_TRACK_WIDGET_6:
                    case CAMPAIGN_TRACK_WIDGET_7:
                    case CAMPAIGN_TRACK_WIDGET_8:
                    case CAMPAIGN_TRACK_WIDGET_9:
                    case CAMPAIGN_TRACK_WIDGET_10:
                    case CAMPAIGN_TRACK_WIDGET_LAST:
                        map = message.payload.widget.id - CAMPAIGN_TRACK_WIDGET_FIRST;
                        if (giDebugLevel < 1
                            && !gpGame->m_campaignMapEnabled[H2EnumIndex(iCurViewSide)][map]) {
                            if (map >= CAMPAIGN_REGULAR_MAP_COUNT
                                || !gpGame->m_campaignScenarioCompleted
                                        [H2EnumIndex(map < CAMPAIGN_SWITCHING_SCENARIO
                                                 ? gpGame->m_campaignStartingSide
                                                 : gpGame->m_campaignType)][map])
                                break;
                        }
                        iCurViewMap = map;
                        iCurViewSide = iCurViewMap < CAMPAIGN_SWITCHING_SCENARIO
                                           ? gpGame->m_campaignStartingSide
                                           : gpGame->m_campaignType;
                        gpGame->CampaignInfoUpdate(1);
                        break;
                    case CAMPAIGN_BONUS_WIDGET_FIRST:
                    case CAMPAIGN_BONUS_WIDGET_FIRST + 1:
                    case CAMPAIGN_BONUS_WIDGET_LAST:
                        if (!bCampaignViewOnly
                            && gpGame->m_campaignMapEnabled[H2EnumIndex(iCurViewSide)][iCurViewMap]) {
                            gpGame->m_campaignChoice[H2EnumIndex(iCurViewSide)][iCurViewMap] =
                                message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST;
                            gpGame->CampaignInfoUpdate(1);
                        }
                        break;
                }
                break;

            case WIDGET_NOTIFY_DESELECT:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_DIALOG_REPLAY:
                        gpGame->PlayPreScenarioSmacker(iCurViewSide, iCurViewMap);
                        campWin->DrawWindow();
                        break;
                    case CAMPAIGN_DIALOG_ACCEPT:
                        if (!bCampaignViewOnly) {
                            if (gpGame->m_campaignMapEnabled[H2EnumIndex(iCurViewSide)][iCurViewMap]) {
                                if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
                                    gpGame->m_campaignScenario = CAMPAIGN_SWITCHING_SCENARIO;
                                    gpGame->m_campaignType =
                                        OppositeCampaignSide(gpGame->m_campaignType);
                                    gpGame->m_campaignMapEnabled[H2EnumIndex(gpGame->m_campaignType)]
                                                                [gpGame->m_campaignScenario] = 1;
                                    gpGame->m_campaignDaysBeforeScenario[H2EnumIndex(gpGame->m_campaignType)]
                                                                   [gpGame->m_campaignScenario] =
                                        gpGame->m_campaignTotalDays;
                                    gpGame->m_campaignChoice[H2EnumIndex(gpGame->m_campaignType)]
                                                            [gpGame->m_campaignScenario] =
                                        gpGame->m_campaignChoice[1 - H2EnumIndex(gpGame->m_campaignType)]
                                                                [CAMPAIGN_SWITCHING_MAP];
                                    for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map)
                                        gpGame->m_campaignAwards[map] = 0;
                                } else {
                                    gpGame->m_campaignScenario = iCurViewMap;
                                    gpGame->m_campaignType = iCurViewSide;
                                }
                            } else {
                                NormalDialog(localization::Tr("campaign.selection.invalid_next_scenario"), NORMAL_DIALOG_INFO);
                                break;
                            }
                        }
                        [[fallthrough]];
                    case CAMPAIGN_DIALOG_CANCEL:
                    case CAMPAIGN_DIALOG_RESTART:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT);
                        message.payload.widget.command =
                            BaseWidgetCommand(H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT));
                        giDialogTimeout = 0;
                        return MESSAGE_DISPATCH_FORWARD;
                }
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void game::InitEntireCampaign(CampaignSide side) {
    memset(&m_campaignType, 0, CAMPAIGN_STATE_RESET_SIZE);
    m_campaignType = side;
    m_campaignStartingSide = side;
    m_campaignScenario = CAMPAIGN_NO_SCENARIO;
}

void game::InitCampaignMap(void) {
    playerData* player;
    i32 bestPriority;
    i32 swappedHero;
    i32 heroPositionValue;
    i32 scanPositionId;
    b32 savedNewGameSetup;
    i32 raceSlot;
    SCampaignChoice* bonusChoice;
    i32 heroPriority;
    i32 bestHeroSlot;
    i32 selectedChoice;

    b32 spellHeroSlot;

    if (m_campaignScenario < 0 || m_campaignScenario >= CAMPAIGN_MAP_COUNT) {
        ShutDown("Invalid campaign scenario number.");
        return;
    }
    const i32 scenarioNumber = m_campaignScenario + 1;

    selectedChoice = m_campaignChoice[H2EnumIndex(iCurViewSide)][iCurViewMap];
    if (m_campaignType != m_campaignStartingSide && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
        bonusChoice =
            &campaignChoices[H2EnumIndex(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][selectedChoice];
    } else {
        bonusChoice =
            &campaignChoices[H2EnumIndex(m_campaignType)][m_campaignScenario][selectedChoice];
    }

    gpGame->m_campaignScenarioWon = 0;
    memset(m_setupPlayerColor, 0, CAMPAIGN_SETUP_RESET_SIZE);

    std::string mapFilename = "CAMP";
    if (m_campaignScenario == CAMPAIGN_SWITCHING_SCENARIO
        && m_campaignStartingSide != m_campaignType) {
        mapFilename += m_campaignType == CAMPAIGN_ROLAND ? 'E' : 'G';
        if (scenarioNumber < 10)
            mapFilename += '0';
        mapFilename += std::to_string(scenarioNumber);
        mapFilename += 'B';
    } else {
        mapFilename += m_campaignType == CAMPAIGN_ROLAND ? 'G' : 'E';
        if (scenarioNumber < 10)
            mapFilename += '0';
        mapFilename += std::to_string(scenarioNumber);
    }
    mapFilename += ".H2C";
    if (mapFilename.size() >= sizeof(m_mapFilename)) {
        ShutDown("Campaign map filename is too long.");
        return;
    }
    memcpy(m_mapFilename, mapFilename.c_str(), mapFilename.size() + 1);
    m_newGameInitialized = false;
    if (m_campaignScenario == 0)
        m_campaignTotalDays = 0;
    strcpy(gMapName, m_mapFilename);
    GetMapHeader(m_mapFilename, &m_mapHeader);
    LoadGame("origdata.bin", 1, 0);
    InitNewGame(NULL);

    if (bonusChoice->type == CAMPAIGN_CHOICE_ALIGNMENT) {
        raceSlot = 0;
        if (m_campaignType == CAMPAIGN_ARCHIBALD) {
            if (m_mapHeader.playerEnabled[0])
                ++raceSlot;
            if (m_mapHeader.playerEnabled[1])
                ++raceSlot;
        }
        m_setupPlayerRace[raceSlot] = bonusChoice->faction;
    }

    if (m_campaignScenario + 1 <= CAMPAIGN_EASY_SCENARIO_LIMIT)
        gpGame->m_difficulty = DIFFICULTY_EASY;
    else if (m_campaignScenario + 1 <= CAMPAIGN_NORMAL_SCENARIO_LIMIT)
        gpGame->m_difficulty = DIFFICULTY_NORMAL;
    else
        gpGame->m_difficulty = DIFFICULTY_HARD;
    m_playerCount = m_mapHeader.playerCount;
    NewMap(gMapName);

    bestHeroSlot = 0;
    player = &gpGame->m_players[0];
    for (heroPositionValue = 0; heroPositionValue < player->m_heroCount;
         ++heroPositionValue) {
        bestPriority = -1;
        for (scanPositionId = heroPositionValue;
             scanPositionId < player->m_heroCount;
             ++scanPositionId) {
            if (gpGame->m_heroRecs[player->m_heroIds[scanPositionId]].m_portrait
                    == CAMPAIGN_HERO_ROLAND
                || gpGame->m_heroRecs[player->m_heroIds[scanPositionId]].m_portrait
                       == CAMPAIGN_HERO_ARCHIBALD) {
                heroPriority = CAMPAIGN_HERO_PRIORITY_HIGH;
            } else if (gpGame->m_heroRecs[player->m_heroIds[scanPositionId]]
                               .m_portrait
                           == CAMPAIGN_HERO_CORLAGON
                       || gpGame->m_heroRecs[player->m_heroIds[scanPositionId]]
                                  .m_portrait
                              == CAMPAIGN_HERO_HALTON) {
                heroPriority = CAMPAIGN_HERO_PRIORITY_NORMAL;
            } else {
                heroPriority = 0;
            }
            if (heroPriority > bestPriority) {
                bestPriority = heroPriority;
                bestHeroSlot = scanPositionId;
            }
        }
        if (bestPriority != -1) {
            swappedHero = player->m_heroIds[heroPositionValue];
            player->m_heroIds[heroPositionValue] =
                player->m_heroIds[bestHeroSlot];
            player->m_heroIds[bestHeroSlot] =
                swappedHero;
        }
    }
    if (player->m_heroCount)
        player->m_currentHero = player->m_heroIds[0];

    switch (bonusChoice->type) {
        case CAMPAIGN_CHOICE_RESOURCE:
            m_players[0].m_resources[H2EnumIndex(bonusChoice->resource)] += bonusChoice->amount;
            break;
        case CAMPAIGN_CHOICE_ARTIFACT:
            if (m_players[0].m_heroCount > 0)
                GiveArtifact(
                    gpGame->GetHero(m_players[0].m_heroIds[0]),
                    bonusChoice->artifact,
                    false,
                    -1
                );
            break;
        case CAMPAIGN_CHOICE_SPELL:
            if (m_players[0].m_heroCount > 0) {
                spellHeroSlot = false;
                if (m_campaignType == CAMPAIGN_ROLAND
                    && m_campaignScenario + 1 == SCENARIO_SIX
                    && m_players[0].m_heroCount > 1)
                    spellHeroSlot = true;
                gpGame->GetHero(m_players[0].m_heroIds[spellHeroSlot])
                    ->m_spells[H2EnumIndex(bonusChoice->spell)] = 1;
            }
            break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            if (m_players[0].m_heroCount > 0)
                gpGame->GetHero(m_players[0].m_heroIds[0])
                    ->SetSS(
                        HeroSecondarySkillFromCode(bonusChoice->value),
                        HeroSkillLevelFromCode(bonusChoice->amount)
                    );
            break;
        case CAMPAIGN_CHOICE_CREATURES:
            if (m_players[0].m_heroCount > 0)
                gpGame->GetHero(m_players[0].m_heroIds[0])
                    ->m_army.Add(bonusChoice->creature, bonusChoice->amount, -1);
            break;
        case CAMPAIGN_CHOICE_PUZZLE_PIECES:
            m_players[0].m_bonusPuzzlePieces = bonusChoice->value;
            break;
        case CAMPAIGN_CHOICE_EXPERIENCE: {
            savedNewGameSetup = gbInNewGameSetup;
            gbInNewGameSetup = true;
            if (m_players[0].m_heroCount > 0) {
                gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += bonusChoice->value;
                gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
            }
            gbInNewGameSetup = savedNewGameSetup;
            break;
        }
        case CAMPAIGN_CHOICE_NONE:
            break;
        default:
            break;
    }

    if ((m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)]
         || (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)]
             && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1))
        && m_players[0].m_heroCount > 0) {
        GiveArtifact(gpGame->GetHero(m_players[0].m_heroIds[0]), ARTIFACT_ULTIMATE_CROWN, false, -1);
    }
    gbRetreatWin = true;

    if (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_CORLAGON_DEFEATED)]) {
        for (heroPositionValue = 0; heroPositionValue < GAME_HERO_COUNT; ++heroPositionValue) {
            if (gpGame->m_heroRecs[heroPositionValue].m_portrait == CAMPAIGN_HERO_CORLAGON)
                gpGame->m_heroRecs[heroPositionValue].Deallocate(0);
        }
    }

    if (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
        && m_players[CAMPAIGN_STRENGTHENED_ROLAND_PLAYER].m_heroCount > 0) {
        hero* armyHero = gpGame->GetHero(m_players[CAMPAIGN_STRENGTHENED_ROLAND_PLAYER].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < ARMY_GROUP_SLOT_COUNT;
             ++heroPositionValue) {
            if (armyHero->m_army.m_creatureCounts[heroPositionValue] >= 1)
                armyHero->m_army.m_creatureCounts[heroPositionValue] *=
                    CAMPAIGN_TRIPLE_ARMY_MULTIPLIER;
        }
    }

    if (m_campaignScenario + 1 == SCENARIO_SEVEN
        && m_campaignType == CAMPAIGN_ARCHIBALD && m_players[0].m_heroCount > 0) {
        b32 savedNewGame = gbInNewGameSetup;
        hero* armyHero;
        gbInNewGameSetup = true;
        armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < ARMY_GROUP_SLOT_COUNT;
             ++heroPositionValue) {
            armyHero->m_army.m_creatureTypes[heroPositionValue] = CREATURE_NONE;
            armyHero->m_army.m_creatureCounts[heroPositionValue] = 0;
        }
        switch (armyHero->m_faction) {
            case FACTION_WARLOCK:
                armyHero->m_army.Add(CREATURE_CENTAUR, WARLOCK_CENTAUR_COUNT, -1);
                armyHero->m_army.Add(CREATURE_GARGOYLE, WARLOCK_GARGOYLE_COUNT, -1);
                armyHero->m_army.Add(CREATURE_GRIFFIN, WARLOCK_GRIFFIN_COUNT, -1);
                break;
            case FACTION_BARBARIAN:
                armyHero->m_army
                    .Add(CREATURE_ORC_CHIEF, BARBARIAN_ORC_CHIEF_COUNT, -1);
                armyHero->m_army.Add(CREATURE_OGRE, BARBARIAN_OGRE_COUNT, -1);
                armyHero->m_army.Add(CREATURE_GOBLIN, BARBARIAN_GOBLIN_COUNT, -1);
                break;
            case FACTION_NECROMANCER:
                armyHero->m_army
                    .Add(CREATURE_SKELETON, NECROMANCER_SKELETON_COUNT, -1);
                armyHero->m_army
                    .Add(CREATURE_ROYAL_MUMMY, NECROMANCER_MUMMY_COUNT, -1);
                armyHero->m_army
                    .Add(CREATURE_VAMPIRE_LORD, NECROMANCER_VAMPIRE_COUNT, -1);
                break;
            default:
                break;
        }
        gpGame->GetHero(m_players[0].m_heroIds[0])->m_experience += CAMPAIGN_EXPERIENCE_BONUS;
        gpGame->GetHero(m_players[0].m_heroIds[0])->CheckLevel();
        gbInNewGameSetup = savedNewGame;
    }

    if (m_players[0].m_heroCount > 0
        && ((m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)]
          && m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO + 1)
        || m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)])) {
        hero* armyHero = gpGame->GetHero(m_players[0].m_heroIds[0]);
        for (heroPositionValue = 0; heroPositionValue < ARMY_GROUP_SLOT_COUNT;
             ++heroPositionValue) {
            armyHero->m_army.m_creatureTypes[heroPositionValue] =
                m_campaignCarryoverCreatureTypes[heroPositionValue];
            armyHero->m_army.m_creatureCounts[heroPositionValue] =
                m_campaignCarryoverCreatureCounts[heroPositionValue]
                * (m_campaignAwards[H2EnumIndex(CAMPAIGN_AWARD_ROLAND_STRENGTHENED)]
                       ? CAMPAIGN_TRIPLE_ARMY_MULTIPLIER
                       : 1);
        }
    }

    if (m_campaignType == CAMPAIGN_ARCHIBALD
        && m_campaignScenario + 1 == SCENARIO_SIX) {
        gpGame->m_mapHeader.victoryCondition = MAP_VICTORY_DEFEAT_SIDE;
        gpGame->m_mapHeader.victoryConditionValue = CAMPAIGN_SWITCH_VICTORY_VALUE;
        gpGame->m_mapHeader.allowNormalVictory = 1;
    }
    if (m_campaignType == CAMPAIGN_ROLAND
        && m_campaignScenario + 1 == SCENARIO_NINE) {
        gpGame->m_mapHeader.lossCondition = MAP_LOSS_STANDARD;
        gpGame->m_mapHeader.lossConditionValue = 0;
    }
    if (m_campaignType == CAMPAIGN_ROLAND
        && m_campaignScenario + 1 == SCENARIO_SEVEN)
        gpGame->m_mapHeader.lossConditionValue = CAMPAIGN_ROLAND_TIME_LIMIT;
}

i16 trackXY[H2EnumIndex(CAMPAIGN_SIDE_COUNT)][CAMPAIGN_TRACK_POINT_COUNT]
                                  [H2EnumIndex(COORDINATE_AXIS_COUNT)] = {
    {{39, 336}, {113, 336}, {150, 294}, {187, 336}, {261, 336}, {335, 336}, {409, 378},
     {409, 294}, {483, 336}, {557, 336}, {-1, -1}, {261, 378}, {-1, -1}},
    {{39, 336}, {113, 336}, {187, 294}, {187, 378}, {261, 336}, {335, 336}, {372, 294},
     {409, 336}, {483, 294}, {483, 378}, {557, 336}, {261, 294}, {261, 378}}
};
class heroWindow* campWin = NULL;
H2EnumStorage<CampaignSide, i32> iCurViewSide;
CampaignTrackType iCampaignTrackType;
i32 bCampaignViewOnly;
i32 iCurViewMap;
