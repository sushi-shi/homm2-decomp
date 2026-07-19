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

VA(0x00489a30, 0x66)
highScoreManager::highScoreManager(void) {
    i32 entry;

    for (entry = 0; entry < HIGH_SCORE_DISPLAY_ENTRY_COUNT; entry++)
        m_animationFrames[entry] = Random(0, HIGH_SCORE_ANIMATION_FRAME_COUNT - 1);
    m_showCampaignScores = 0;
}

VA(0x00489a96, 0x14c)
i32 highScoreManager::Open(i32 id) {
    if (giHighScoreType == HIGH_SCORE_CAMPAIGN || giHighScoreType == HIGH_SCORE_EXPANSION_CAMPAIGN)
        m_showCampaignScores = 1;
    else
        m_showCampaignScores = 0;

    gpWindowManager->FadeScreen(FADE_OUT, HIGH_SCORE_FADE_STEPS, NULL);
    sprintf(gText, "hsbkg.icn");
    gpResourceManager->GetBackdrop(gText, gpWindowManager->m_screen, 1);
    m_window = new heroWindow(0, 0, "hiscore.bin");
    if (m_window == NULL)
        MemError();
    Update();
    gpWindowManager->AddWindow(m_window, -1, 1);
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = BaseManagerPriority(id);
    m_active = true;
    strcpy(m_name, "highScoreManager");
    KBChangeMenu(hmnuDflt);
    gpWindowManager->FadeScreen(FADE_IN, HIGH_SCORE_FADE_STEPS, NULL);
    glTimers[HIGH_SCORE_TIMER_SLOT] = KBTickCount() + HIGH_SCORE_ANIMATION_DELAY;
    return HIGH_SCORE_MANAGER_OPEN_OK;
}

VA(0x00489be2, 0x5e)
void highScoreManager::Close(void) {
    gpWindowManager->FadeScreen(FADE_OUT, HIGH_SCORE_FADE_STEPS, NULL);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_active = false;
}

VA(0x00489c40, 0x22a)
i32 highScoreManager::Main(struct tag_message& message) {
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
                monAnimDrawFrame[m_animationFrames[entry]]
                + m_monsterTypes[entry] * HIGH_SCORE_MONSTER_FRAME_STRIDE
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

    if (message.payload.keyboard.modifiers & HIGH_SCORE_INPUT_BLOCK_FLAG)
        return HIGH_SCORE_MANAGER_CONTINUE;

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
        return HIGH_SCORE_MANAGER_CLOSE;
    }
    return HIGH_SCORE_MANAGER_CONTINUE;
}

VA(0x00489e6a, 0x7d3)
void highScoreManager::Update(void) {
    i32 entry;
    HighScoreEntry scoreEntry;
    i32 inputFile;
    tag_message messageValue;
    char filename[HIGH_SCORE_FILENAME_LENGTH];
    i32 missingFile;

    missingFile = 0;
    if (m_showCampaignScores)
        sprintf(filename, "%sCAMPAIGN.HS", ".\\DATA\\");
    else
        sprintf(filename, "%sSTANDARD.HS", ".\\DATA\\");
    inputFile = _open(filename, HIGH_SCORE_FILE_READ_FLAGS);
    if (inputFile == -1)
        missingFile = 1;

    sprintf(gText, "hsbkg.icn");
    gpResourceManager->GetBackdrop(gText, gpWindowManager->m_screen, 1);

    messageValue.type = MESSAGE_WIDGET;
    messageValue.payload.widget.id = HIGH_SCORE_TITLE_WIDGET;
    messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
    if (m_showCampaignScores)
        messageValue.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_TITLE_FRAME;
    else
        messageValue.payload.widget.data.value = HIGH_SCORE_STANDARD_TITLE_FRAME;
    m_window->BroadcastMessage(messageValue);

    if (m_showCampaignScores)
        messageValue.payload.widget.id = HIGH_SCORE_CAMPAIGN_BUTTON;
    else
        messageValue.payload.widget.id = HIGH_SCORE_STANDARD_BUTTON;
    messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
    messageValue.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
    m_window->BroadcastMessage(messageValue);

    if (m_showCampaignScores)
        messageValue.payload.widget.id = HIGH_SCORE_STANDARD_BUTTON;
    else
        messageValue.payload.widget.id = HIGH_SCORE_CAMPAIGN_BUTTON;
    messageValue.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
    messageValue.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
    m_window->BroadcastMessage(messageValue);

    for (entry = 0; entry < HIGH_SCORE_DISPLAY_ENTRY_COUNT; entry++) {
        if (missingFile != 0)
            scoreEntry.score = HIGH_SCORE_EMPTY;
        else
            _read(inputFile, &scoreEntry, sizeof(scoreEntry));

        if (scoreEntry.score == HIGH_SCORE_EMPTY) {
            m_monsterTypes[entry] = 0;
            sprintf(gText, "");
        } else {
            m_monsterTypes[entry] = GetMonType(scoreEntry.score, m_showCampaignScores == 0);
        }

        if (scoreEntry.score == HIGH_SCORE_EMPTY)
            messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
        else
            messageValue.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
        messageValue.payload.widget.id = entry + HIGH_SCORE_FIRST_MONSTER_WIDGET;
        messageValue.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
        m_window->BroadcastMessage(messageValue);
        messageValue.payload.widget.id = entry + HIGH_SCORE_FIRST_SHADOW_WIDGET;
        messageValue.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
        m_window->BroadcastMessage(messageValue);

        if (scoreEntry.score != HIGH_SCORE_EMPTY) {
            m_animationFrames[entry] =
                (m_animationFrames[entry] + 1) % HIGH_SCORE_ANIMATION_FRAME_COUNT;
            messageValue.payload.widget.id = entry + HIGH_SCORE_FIRST_MONSTER_WIDGET;
            messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
            messageValue.payload.widget.data.value =
                monAnimDrawFrame[m_animationFrames[entry]]
                + m_monsterTypes[entry] * HIGH_SCORE_MONSTER_FRAME_STRIDE
                + HIGH_SCORE_MONSTER_ACTIVE_FRAME_OFFSET;
            m_window->BroadcastMessage(messageValue);
            messageValue.payload.widget.id = entry + HIGH_SCORE_FIRST_SHADOW_WIDGET;
            messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
            messageValue.payload.widget.data.value =
                m_monsterTypes[entry] * HIGH_SCORE_MONSTER_FRAME_STRIDE;
            m_window->BroadcastMessage(messageValue);
        }

        messageValue.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
        messageValue.payload.widget.data.text = gText;
        sprintf(gText, "");
        messageValue.payload.widget.id =
            entry * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
        if (scoreEntry.score != HIGH_SCORE_EMPTY)
            sprintf(gText, scoreEntry.playerName);
        if (scoreEntry.cheated)
            strcat(gText, "\n(Cheater)");
        m_window->BroadcastMessage(messageValue);

        sprintf(gText, "");
        messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
        if (scoreEntry.score != HIGH_SCORE_EMPTY)
            sprintf(gText, scoreEntry.scenarioName);
        m_window->BroadcastMessage(messageValue);

        sprintf(gText, "");
        messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_RATING_OFFSET;
        if (scoreEntry.score != HIGH_SCORE_EMPTY) {
            if (m_showCampaignScores == 0)
                sprintf(gText, "%d", scoreEntry.days);
            else
                sprintf(gText, "%d", scoreEntry.score);
        }
        m_window->BroadcastMessage(messageValue);

        sprintf(gText, "");
        messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCORE_OFFSET;
        sprintf(gText, "");
        if (m_showCampaignScores == 0 && scoreEntry.score != HIGH_SCORE_EMPTY)
            sprintf(gText, "%d", scoreEntry.score);
        m_window->BroadcastMessage(messageValue);

        messageValue.payload.widget.command = HIGH_SCORE_WIDGET_RESIZE;
        messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
        if (m_showCampaignScores)
            messageValue.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_SCENARIO_RESIZE;
        else
            messageValue.payload.widget.data.value = HIGH_SCORE_STANDARD_SCENARIO_RESIZE;
        m_window->BroadcastMessage(messageValue);
        messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                         + HIGH_SCORE_FIRST_TEXT_WIDGET
                                         + HIGH_SCORE_TEXT_RATING_OFFSET;
        if (m_showCampaignScores)
            messageValue.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_RATING_RESIZE;
        else
            messageValue.payload.widget.data.value = HIGH_SCORE_STANDARD_RATING_RESIZE;
        m_window->BroadcastMessage(messageValue);

        if (entry == giHighScoreRank) {
            if (!((!m_showCampaignScores || giHighScoreType == HIGH_SCORE_STANDARD)
                  && (m_showCampaignScores || giHighScoreType != HIGH_SCORE_STANDARD))) {
                messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
                messageValue.payload.widget.data.value = HIGH_SCORE_SECONDARY_SELECTION_FRAME;
            } else {
                messageValue.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
                messageValue.payload.widget.data.value = HIGH_SCORE_PRIMARY_SELECTION_FRAME;
            }
            messageValue.payload.widget.id =
                entry * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
            m_window->BroadcastMessage(messageValue);
            messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
            m_window->BroadcastMessage(messageValue);
            messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_RATING_OFFSET;
            m_window->BroadcastMessage(messageValue);
            messageValue.payload.widget.id = entry * HIGH_SCORE_TEXT_WIDGET_STRIDE
                                             + HIGH_SCORE_FIRST_TEXT_WIDGET
                                             + HIGH_SCORE_TEXT_SCORE_OFFSET;
            m_window->BroadcastMessage(messageValue);
        }
    }
    if (missingFile == 0)
        _close(inputFile);
}


VTBL(highScoreManager, 0x004eb838);
