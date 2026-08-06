#include <va.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/highScoreManager.h>
#include <BASE/Misc.h>
#include <BASE/executive.h>
#include <BASE/widget.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/resourceManager.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>

VA(0x00465110, 0x59)
highScoreManager::highScoreManager(void) {
    i32 entry;

    for (entry = 0; entry < HIGH_SCORE_DISPLAY_ENTRY_COUNT; entry++)
        m_animationFrames[entry] = Random(0, HIGH_SCORE_ANIMATION_FRAME_COUNT - 1);
    m_showCampaignScores = 0;
}

VA(0x00465169, 0x16d)
i32 highScoreManager::Open(i32 id) {
    m_showCampaignScores = giHighScoreType == HIGH_SCORE_CAMPAIGN
                           || giHighScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN;

    gpWindowManager->FadeScreen(FADE_OUT, HIGH_SCORE_FADE_STEPS, NULL);
    sprintf(gText, "hsbkg.icn");
    gpResourceManager->GetBackdrop(gText, gpWindowManager->m_screen, 1);
    m_window = new heroWindow(0, 0, "hiscore.bin");
    if (m_window == NULL)
        MemError();
    Update();
    gpWindowManager->AddWindow(m_window, -1, 1);
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = id;
    m_active = true;
    strcpy(m_name, "highScoreManager");
    KBChangeMenu(hmnuDflt);
    gpWindowManager->FadeScreen(FADE_IN, HIGH_SCORE_FADE_STEPS, NULL);
    glTimers[HIGH_SCORE_TIMER_SLOT] = KBTickCount() + HIGH_SCORE_ANIMATION_DELAY;
    return HIGH_SCORE_MANAGER_OPEN_OK;
}

VA(0x004652d6, 0x4f)
void highScoreManager::Close(void) {
    gpWindowManager->FadeScreen(FADE_OUT, HIGH_SCORE_FADE_STEPS, NULL);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = false;
}

VA(0x00465325, 0x1ca)
MessageDispatchResult highScoreManager::Main(struct tag_message& message) {
    i32 result;
    i32 entry;
    tag_message windowMessage;

    result = 0;
    if (gbShowHighScore != 0)
        gbShowHighScore = false;

    if (glTimers[HIGH_SCORE_TIMER_SLOT] < KBTickCount()) {
        glTimers[HIGH_SCORE_TIMER_SLOT] = KBTickCount() + HIGH_SCORE_ANIMATION_DELAY;
        for (entry = 0; entry < HIGH_SCORE_DISPLAY_ENTRY_COUNT; entry++) {
            m_animationFrames[entry] =
                (m_animationFrames[entry] + 1) % HIGH_SCORE_ANIMATION_FRAME_COUNT;
            windowMessage.type = MESSAGE_WIDGET;
            windowMessage.payload.widget.id = entry + HIGH_SCORE_FIRST_MONSTER_WIDGET;
            windowMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
            windowMessage.payload.widget.data.value =
                m_monsterTypes[entry] * HIGH_SCORE_MONSTER_FRAME_STRIDE
                + monAnimDrawFrame[m_animationFrames[entry]]
                + HIGH_SCORE_MONSTER_ACTIVE_FRAME_OFFSET;
            m_window->BroadcastMessage(windowMessage);
        }
        m_window->DrawWindow(HIGH_SCORE_DRAW_X, HIGH_SCORE_DRAW_Y, HIGH_SCORE_DRAW_HEIGHT);
        gpWindowManager->UpdateScreenRegion(
            HIGH_SCORE_UPDATE_X,
            HIGH_SCORE_UPDATE_Y,
            HIGH_SCORE_UPDATE_WIDTH,
            HIGH_SCORE_UPDATE_HEIGHT
        );
    }

    if (HAS(message.payload.keyboard.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
        return MESSAGE_DISPATCH_CONSUME;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case HIGH_SCORE_WIDGET_TOGGLE:
                    switch (message.payload.widget.id) {
                        case HIGH_SCORE_STANDARD_BUTTON:
                        case HIGH_SCORE_CAMPAIGN_BUTTON:
                            m_showCampaignScores = 1 - m_showCampaignScores;
                            Update();
                            m_window->DrawWindow(1);
                            break;
                        case HIGH_SCORE_CLOSE_BUTTON:
                            message.payload.widget.data.value = message.payload.widget.id;
                            result = 1;
                            break;
                    }
                    break;
            }
            break;
        default:
            break;
    }

    if (result == 1) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_RETURN_RESULT;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004654ef, 0x6bc)
void highScoreManager::Update(void) {
    HighScoreEntry highScore;
    i32 rank;
    i32 inputFile;
    i32 noScoreFile;
    tag_message hsMessage;
    char scorePath[HIGH_SCORE_FILENAME_LENGTH];

    noScoreFile = 0;
    if (m_showCampaignScores)
        sprintf(scorePath, "%sCAMPAIGN.HS", ".\\DATA\\");
    else
        sprintf(scorePath, "%sSTANDARD.HS", ".\\DATA\\");
    inputFile = open(scorePath, HIGH_SCORE_FILE_READ_FLAGS);
    if (inputFile == -1)
        noScoreFile = 1;

    sprintf(gText, "hsbkg.icn");
    gpResourceManager->GetBackdrop(gText, gpWindowManager->m_screen, 1);

    hsMessage.type = MESSAGE_WIDGET;
    hsMessage.payload.widget.id = HIGH_SCORE_TITLE_WIDGET;
    hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
    hsMessage.payload.widget.data.value =
        m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_TITLE_FRAME : HIGH_SCORE_STANDARD_TITLE_FRAME;
    m_window->BroadcastMessage(hsMessage);

    hsMessage.payload.widget.id = static_cast<i16>(
        m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON
    );
    hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
    hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
    m_window->BroadcastMessage(hsMessage);

    hsMessage.payload.widget.id = static_cast<i16>(
        m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON
    );
    hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
    hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
    m_window->BroadcastMessage(hsMessage);

    for (rank = 0; rank < HIGH_SCORE_DISPLAY_ENTRY_COUNT; rank++) {
        if (noScoreFile != 0)
            highScore.score = HIGH_SCORE_EMPTY;
        else
            read(inputFile, &highScore, sizeof(highScore));

        if (highScore.score == HIGH_SCORE_EMPTY) {
            m_monsterTypes[rank] = 0;
            sprintf(
                gText,
                DATA_COMPGEN(0x005249bc, highScoreEmptyEntryText, "")
            );
        } else {
            m_monsterTypes[rank] = GetMonType(
                highScore.score,
                m_showCampaignScores ? HIGH_SCORE_CAMPAIGN : HIGH_SCORE_STANDARD
            );
        }

        hsMessage.payload.widget.command =
            highScore.score == HIGH_SCORE_EMPTY ? HIGH_SCORE_WIDGET_SHOW
                                                 : HIGH_SCORE_WIDGET_HIDE;
        hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_MONSTER_WIDGET;
        hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
        m_window->BroadcastMessage(hsMessage);
        hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_SHADOW_WIDGET;
        hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
        m_window->BroadcastMessage(hsMessage);

        if (highScore.score != HIGH_SCORE_EMPTY) {
            m_animationFrames[rank] =
                (m_animationFrames[rank] + 1) % HIGH_SCORE_ANIMATION_FRAME_COUNT;
            hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_MONSTER_WIDGET;
            hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
            hsMessage.payload.widget.data.value =
                m_monsterTypes[rank] * HIGH_SCORE_MONSTER_FRAME_STRIDE
                + monAnimDrawFrame[m_animationFrames[rank]]
                + HIGH_SCORE_MONSTER_ACTIVE_FRAME_OFFSET;
            m_window->BroadcastMessage(hsMessage);
            hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_SHADOW_WIDGET;
            hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
            hsMessage.payload.widget.data.value =
                m_monsterTypes[rank] * HIGH_SCORE_MONSTER_FRAME_STRIDE;
            m_window->BroadcastMessage(hsMessage);
        }

        hsMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        hsMessage.payload.widget.data.text = gText;
        sprintf(
            gText,
            DATA_COMPGEN(0x005249c0, highScorePlayerNameEmpty, "")
        );
        hsMessage.payload.widget.id =
            rank * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
        if (highScore.score != HIGH_SCORE_EMPTY)
            sprintf(gText, highScore.playerName);
        if (highScore.cheated)
            strcat(
                gText,
                DATA_COMPGEN(
                    0x004f8120,
                    highScoreCheaterSuffix,
                    "\n(\xd7\xe8\xf2\xe5\xf0)" /* "\n(Читер)" */
                )
            );
        m_window->BroadcastMessage(hsMessage);

        sprintf(
            gText,
            DATA_COMPGEN(0x005249c4, highScoreScenarioNameEmpty, "")
        );
        hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
        if (highScore.score != HIGH_SCORE_EMPTY)
            sprintf(gText, highScore.scenarioName);
        m_window->BroadcastMessage(hsMessage);

        sprintf(
            gText,
            DATA_COMPGEN(0x005249c8, highScoreRatingEmpty, "")
        );
        hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_RATING_OFFSET;
        if (highScore.score != HIGH_SCORE_EMPTY) {
            if (m_showCampaignScores == 0)
                sprintf(gText, "%d", highScore.days);
            else
                sprintf(gText, "%d", highScore.score);
        }
        m_window->BroadcastMessage(hsMessage);

        sprintf(
            gText,
            DATA_COMPGEN(0x005249cc, highScoreScoreTextEmpty, "")
        );
        hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCORE_OFFSET;
        sprintf(
            gText,
            DATA_COMPGEN(0x005249d0, highScoreScoreValueEmpty, "")
        );
        if (m_showCampaignScores == 0 && highScore.score != HIGH_SCORE_EMPTY)
            sprintf(gText, "%d", highScore.score);
        m_window->BroadcastMessage(hsMessage);

        hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_RESIZE;
        hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
        if (m_showCampaignScores)
            hsMessage.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_SCENARIO_RESIZE;
        else
            hsMessage.payload.widget.data.value = HIGH_SCORE_STANDARD_SCENARIO_RESIZE;
        m_window->BroadcastMessage(hsMessage);
        hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_RATING_OFFSET;
        if (m_showCampaignScores)
            hsMessage.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_RATING_RESIZE;
        else
            hsMessage.payload.widget.data.value = HIGH_SCORE_STANDARD_RATING_RESIZE;
        m_window->BroadcastMessage(hsMessage);

        if (giHighScoreRank == rank) {
            if (!((!m_showCampaignScores || giHighScoreType == HIGH_SCORE_STANDARD)
                  && (m_showCampaignScores || giHighScoreType != HIGH_SCORE_STANDARD))) {
                hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
                hsMessage.payload.widget.data.value = HIGH_SCORE_SECONDARY_SELECTION_FRAME;
            } else {
                hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
                hsMessage.payload.widget.data.value = HIGH_SCORE_PRIMARY_SELECTION_FRAME;
            }
            hsMessage.payload.widget.id =
                rank * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
            m_window->BroadcastMessage(hsMessage);
            hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
            m_window->BroadcastMessage(hsMessage);
            hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_RATING_OFFSET;
            m_window->BroadcastMessage(hsMessage);
            hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_SCORE_OFFSET;
            m_window->BroadcastMessage(hsMessage);
        }
    }
    if (noScoreFile == 0)
        close(inputFile);
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(highScoreManager, 0x004ea630)
