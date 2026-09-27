// VC6 SP5 HISCORE source family c71-hiscore-widget; parent e4d33acc.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c71-hiscore-widget/results.json; RVA 0x654ef
// Complete 4 arms; elapsed 2.571048s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1724 / 9254a4323607617e / 580a69b0537b520e / 51:47 / 47,0,0,0 / False
// 1 / {'source_family': 'hide', 'candidate': 'baseline'} / 1719 / 72d6f1dfb3ffe7d8 / 19261c3dae7301ae / 51:47 / 46,1,0,0 / False
// 2 / {'source_family': 'show', 'candidate': 'baseline'} / 1719 / fe2518bb374105bd / a211e96ad10fffd8 / 51:47 / 46,1,0,0 / False
// 3 / {'source_family': 'showhide', 'candidate': 'baseline'} / 1718 / 500ba45be9773eda / e4fa4f11d37a4145 / 51:47 / 46,1,0,0 / False

// AXIS source_family
// BEFORE (complete exact span):
// | void highScoreManager::Update(void) {
// |     HighScoreEntry highScore;
// |     i32 rank;
// |     i32 inputFile;
// |     b32 noScoreFile;
// |     tag_message hsMessage;
// |     char scorePath[HIGH_SCORE_FILENAME_LENGTH];
// |
// |     noScoreFile = false;
// |     if (m_showCampaignScores)
// |         sprintf(scorePath, "%sCAMPAIGN.HS", ".\\DATA\\");
// |     else
// |         sprintf(scorePath, "%sSTANDARD.HS", ".\\DATA\\");
// |     inputFile = open(scorePath, HIGH_SCORE_FILE_READ_FLAGS);
// |     if (inputFile == -1)
// |         noScoreFile = true;
// |
// |     sprintf(gText, "hsbkg.icn");
// |     gpResourceManager->GetBackdrop(gText, gpWindowManager->m_screen, 1);
// |
// |     hsMessage.type = MESSAGE_WIDGET;
// |     hsMessage.payload.widget.id = HIGH_SCORE_TITLE_WIDGET;
// |     hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
// |     hsMessage.payload.widget.data.value =
// |         m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_TITLE_FRAME : HIGH_SCORE_STANDARD_TITLE_FRAME;
// |     m_window->BroadcastMessage(hsMessage);
// |
// |     hsMessage.payload.widget.id = static_cast<i16>(
// |         m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON
// |     );
// |     hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
// |     hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |     m_window->BroadcastMessage(hsMessage);
// |
// |     hsMessage.payload.widget.id = static_cast<i16>(
// |         m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON
// |     );
// |     hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
// |     hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |     m_window->BroadcastMessage(hsMessage);
// |
// |     for (rank = 0; rank < HIGH_SCORE_DISPLAY_ENTRY_COUNT; rank++) {
// |         if (noScoreFile != 0)
// |             highScore.score = HIGH_SCORE_EMPTY;
// |         else
// |             read(inputFile, &highScore, sizeof(highScore));
// |
// |         if (highScore.score == HIGH_SCORE_EMPTY) {
// |             m_monsterTypes[rank] = 0;
// |             sprintf(
// |                 gText,
// |                 ""
// |             );
// |         } else {
// |             m_monsterTypes[rank] = GetMonType(
// |                 highScore.score,
// |                 m_showCampaignScores ? HIGH_SCORE_CAMPAIGN : HIGH_SCORE_STANDARD
// |             );
// |         }
// |
// |         hsMessage.payload.widget.command =
// |             highScore.score == HIGH_SCORE_EMPTY ? HIGH_SCORE_WIDGET_SHOW
// |                                                  : HIGH_SCORE_WIDGET_HIDE;
// |         hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_MONSTER_WIDGET;
// |         hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |         m_window->BroadcastMessage(hsMessage);
// |         hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_SHADOW_WIDGET;
// |         hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         if (highScore.score != HIGH_SCORE_EMPTY) {
// |             m_animationFrames[rank] =
// |                 (m_animationFrames[rank] + 1) % HIGH_SCORE_ANIMATION_FRAME_COUNT;
// |             hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_MONSTER_WIDGET;
// |             hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
// |             hsMessage.payload.widget.data.value =
// |                 m_monsterTypes[rank] * HIGH_SCORE_MONSTER_FRAME_STRIDE
// |                 + monAnimDrawFrame[m_animationFrames[rank]]
// |                 + HIGH_SCORE_MONSTER_ACTIVE_FRAME_OFFSET;
// |             m_window->BroadcastMessage(hsMessage);
// |             hsMessage.payload.widget.id = rank + HIGH_SCORE_FIRST_SHADOW_WIDGET;
// |             hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SET_FRAME;
// |             hsMessage.payload.widget.data.value =
// |                 m_monsterTypes[rank] * HIGH_SCORE_MONSTER_FRAME_STRIDE;
// |             m_window->BroadcastMessage(hsMessage);
// |         }
// |
// |         hsMessage.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// |         hsMessage.payload.widget.data.text = gText;
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         hsMessage.payload.widget.id =
// |             rank * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
// |         if (highScore.score != HIGH_SCORE_EMPTY)
// |             sprintf(gText, highScore.playerName);
// |         if (highScore.cheated)
// |             strcat(
// |                 gText,
// |                 "\n(\xd7\xe8\xf2\xe5\xf0)" /* "\n(Читер)" */
// |             );
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                          + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                          + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
// |         if (highScore.score != HIGH_SCORE_EMPTY)
// |             sprintf(gText, highScore.scenarioName);
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                          + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                          + HIGH_SCORE_TEXT_RATING_OFFSET;
// |         if (highScore.score != HIGH_SCORE_EMPTY) {
// |             if (m_showCampaignScores == 0)
// |                 sprintf(gText, "%d", highScore.days);
// |             else
// |                 sprintf(gText, "%d", highScore.score);
// |         }
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                          + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                          + HIGH_SCORE_TEXT_SCORE_OFFSET;
// |         sprintf(
// |             gText,
// |             ""
// |         );
// |         if (m_showCampaignScores == 0 && highScore.score != HIGH_SCORE_EMPTY)
// |             sprintf(gText, "%d", highScore.score);
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_RESIZE;
// |         hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                          + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                          + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
// |         if (m_showCampaignScores)
// |             hsMessage.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_SCENARIO_RESIZE;
// |         else
// |             hsMessage.payload.widget.data.value = HIGH_SCORE_STANDARD_SCENARIO_RESIZE;
// |         m_window->BroadcastMessage(hsMessage);
// |         hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                          + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                          + HIGH_SCORE_TEXT_RATING_OFFSET;
// |         if (m_showCampaignScores)
// |             hsMessage.payload.widget.data.value = HIGH_SCORE_CAMPAIGN_RATING_RESIZE;
// |         else
// |             hsMessage.payload.widget.data.value = HIGH_SCORE_STANDARD_RATING_RESIZE;
// |         m_window->BroadcastMessage(hsMessage);
// |
// |         if (giHighScoreRank == rank) {
// |             if (!((!m_showCampaignScores || giHighScoreType == HIGH_SCORE_STANDARD)
// |                   && (m_showCampaignScores || giHighScoreType != HIGH_SCORE_STANDARD))) {
// |                 hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
// |                 hsMessage.payload.widget.data.value = HIGH_SCORE_SECONDARY_SELECTION_FRAME;
// |             } else {
// |                 hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SELECT;
// |                 hsMessage.payload.widget.data.value = HIGH_SCORE_PRIMARY_SELECTION_FRAME;
// |             }
// |             hsMessage.payload.widget.id =
// |                 rank * HIGH_SCORE_TEXT_WIDGET_STRIDE + HIGH_SCORE_FIRST_TEXT_WIDGET;
// |             m_window->BroadcastMessage(hsMessage);
// |             hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                              + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                              + HIGH_SCORE_TEXT_SCENARIO_OFFSET;
// |             m_window->BroadcastMessage(hsMessage);
// |             hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                              + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                              + HIGH_SCORE_TEXT_RATING_OFFSET;
// |             m_window->BroadcastMessage(hsMessage);
// |             hsMessage.payload.widget.id = rank * HIGH_SCORE_TEXT_WIDGET_STRIDE
// |                                              + HIGH_SCORE_FIRST_TEXT_WIDGET
// |                                              + HIGH_SCORE_TEXT_SCORE_OFFSET;
// |             m_window->BroadcastMessage(hsMessage);
// |         }
// |     }
// |     if (noScoreFile == 0)
// |         close(inputFile);
// | }
// ALTERNATIVE hide (unified source delta from BEFORE):
// | --- before
// | +++ hide
// | @@ -32,9 +32,7 @@
// |      hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |      m_window->BroadcastMessage(hsMessage);
// |
// | -    hsMessage.payload.widget.id = static_cast<i16>(
// | -        m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON
// | -    );
// | +    hsMessage.payload.widget.id = m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON;
// |      hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
// |      hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |      m_window->BroadcastMessage(hsMessage);
// ALTERNATIVE show (unified source delta from BEFORE):
// | --- before
// | +++ show
// | @@ -25,9 +25,7 @@
// |          m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_TITLE_FRAME : HIGH_SCORE_STANDARD_TITLE_FRAME;
// |      m_window->BroadcastMessage(hsMessage);
// |
// | -    hsMessage.payload.widget.id = static_cast<i16>(
// | -        m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON
// | -    );
// | +    hsMessage.payload.widget.id = m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON;
// |      hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
// |      hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |      m_window->BroadcastMessage(hsMessage);
// ALTERNATIVE showhide (unified source delta from BEFORE):
// | --- before
// | +++ showhide
// | @@ -25,16 +25,12 @@
// |          m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_TITLE_FRAME : HIGH_SCORE_STANDARD_TITLE_FRAME;
// |      m_window->BroadcastMessage(hsMessage);
// |
// | -    hsMessage.payload.widget.id = static_cast<i16>(
// | -        m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON
// | -    );
// | +    hsMessage.payload.widget.id = m_showCampaignScores ? HIGH_SCORE_CAMPAIGN_BUTTON : HIGH_SCORE_STANDARD_BUTTON;
// |      hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_SHOW;
// |      hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |      m_window->BroadcastMessage(hsMessage);
// |
// | -    hsMessage.payload.widget.id = static_cast<i16>(
// | -        m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON
// | -    );
// | +    hsMessage.payload.widget.id = m_showCampaignScores ? HIGH_SCORE_STANDARD_BUTTON : HIGH_SCORE_CAMPAIGN_BUTTON;
// |      hsMessage.payload.widget.command = HIGH_SCORE_WIDGET_HIDE;
// |      hsMessage.payload.widget.data.value = HIGH_SCORE_WIDGET_DEFAULT_VALUE;
// |      m_window->BroadcastMessage(hsMessage);
