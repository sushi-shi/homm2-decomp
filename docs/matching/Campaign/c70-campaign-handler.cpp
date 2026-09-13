// VC6 SP5 Campaign source family c70-campaign-handler; parent d7472bf9.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c70-campaign-handler/results.json; RVA 0x233b8
// Complete 4 arms; elapsed 2.736661s; input restored.
// Disposition: retain all cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1162 / 05d93b0de57051b8 / 6b3a8f4ef7bc41dc / 52:52 / 51,1,0,0 / True
// 1 / {'source_family': 'scenario', 'candidate': 'baseline'} / 1162 / 05d93b0de57051b8 / 22351dc3bbbf4b7d / 52:52 / 51,1,0,0 / True
// 2 / {'source_family': 'choice', 'candidate': 'baseline'} / 1162 / 05d93b0de57051b8 / 22351dc3bbbf4b7d / 52:52 / 51,1,0,0 / True
// 3 / {'source_family': 'choice_scenario', 'candidate': 'baseline'} / 1162 / 05d93b0de57051b8 / 5d2e7110f260cb1a / 52:52 / 51,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | MessageDispatchResult CampaignHandler(struct tag_message& message) {
// |     i32 map;
// |
// |     if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1)
// |         gpSoundManager->SwitchAmbientMusic(
// |             giTerrainToMusicTrack[IDX(gpAdvManager->m_currentTerrain)]
// |         );
// |     if (giDialogTimeout != 0 && KBTickCount() > giDialogTimeout) {
// |         message.type = MESSAGE_WIDGET;
// |         gpWindowManager->m_dialogResult = message.payload.widget.id;
// |         message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
// |         message.payload.widget.command = BaseWidgetCommand(CAMPAIGN_CLOSE_COMMAND);
// |         giDialogTimeout = 0;
// |         return MESSAGE_DISPATCH_FORWARD;
// |     }
// |     if (message.type == MESSAGE_WIDGET) {
// |         switch (message.payload.widget.command) {
// |             case CAMPAIGN_MESSAGE_HOVER:
// |             case CAMPAIGN_MESSAGE_HELP:
// |                 switch (message.payload.widget.id) {
// |                     case CAMPAIGN_TRACK_WIDGET_FIRST:
// |                     case CAMPAIGN_TRACK_WIDGET_FIRST + 1:
// |                     case CAMPAIGN_TRACK_WIDGET_2:
// |                     case CAMPAIGN_TRACK_WIDGET_3:
// |                     case CAMPAIGN_TRACK_WIDGET_4:
// |                     case CAMPAIGN_TRACK_WIDGET_5:
// |                     case CAMPAIGN_TRACK_WIDGET_6:
// |                     case CAMPAIGN_TRACK_WIDGET_7:
// |                     case CAMPAIGN_TRACK_WIDGET_8:
// |                     case CAMPAIGN_TRACK_WIDGET_9:
// |                     case CAMPAIGN_TRACK_WIDGET_10:
// |                     case CAMPAIGN_TRACK_WIDGET_LAST:
// |                         map = message.payload.widget.id - CAMPAIGN_TRACK_WIDGET_FIRST;
// |                         if (giDebugLevel < 1
// |                             && !gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][map]) {
// |                             if (map >= CAMPAIGN_REGULAR_MAP_COUNT
// |                                 || !gpGame->m_campaignScenarioCompleted
// |                                         [IDX(map < CAMPAIGN_SWITCHING_SCENARIO
// |                                                  ? gpGame->m_campaignStartingSide
// |                                                  : gpGame->m_campaignType)][map])
// |                                 break;
// |                         }
// |                         iCurViewMap = map;
// |                         iCurViewSide = iCurViewMap < CAMPAIGN_SWITCHING_SCENARIO
// |                                            ? gpGame->m_campaignStartingSide
// |                                            : gpGame->m_campaignType;
// |                         gpGame->CampaignInfoUpdate(1);
// |                         break;
// |                     case CAMPAIGN_BONUS_WIDGET_FIRST:
// |                     case CAMPAIGN_BONUS_WIDGET_FIRST + 1:
// |                     case CAMPAIGN_BONUS_WIDGET_LAST:
// |                         if (!bCampaignViewOnly
// |                             && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap]) {
// |                             gpGame->m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] =
// |                                 static_cast<u8>(
// |                                     message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST
// |                                 );
// |                             gpGame->CampaignInfoUpdate(1);
// |                         }
// |                         break;
// |                 }
// |                 break;
// |
// |             case CAMPAIGN_MESSAGE_ACTIVATE:
// |                 switch (message.payload.widget.id) {
// |                     case CAMPAIGN_DIALOG_REPLAY:
// |                         gpGame->PlayPreScenarioSmacker(iCurViewSide, iCurViewMap);
// |                         campWin->DrawWindow();
// |                         break;
// |                     case CAMPAIGN_DIALOG_ACCEPT:
// |                         if (!bCampaignViewOnly) {
// |                             if (gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap]) {
// |                                 if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// |                                     gpGame->m_campaignScenario = CAMPAIGN_SWITCHING_SCENARIO;
// |                                     gpGame->m_campaignType =
// |                                         OppositeCampaignSide(gpGame->m_campaignType);
// |                                     gpGame->m_campaignMapEnabled[gpGame->m_campaignScenario]
// |                                                                 [IDX(gpGame->m_campaignType)] = 1;
// |                                     gpGame->m_campaignScenarioBonus[IDX(gpGame->m_campaignType)]
// |                                                                    [gpGame->m_campaignScenario] =
// |                                         gpGame->m_campaignScore;
// |                                     gpGame->m_campaignChoice[IDX(gpGame->m_campaignType)]
// |                                                             [gpGame->m_campaignScenario] =
// |                                         gpGame->m_campaignChoice[1 - IDX(gpGame->m_campaignType)]
// |                                                                 [CAMPAIGN_SWITCHING_MAP];
// |                                     for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map)
// |                                         gpGame->m_campaignAwards[map] = 0;
// |                                 } else {
// |                                     gpGame->m_campaignScenario = static_cast<i8>(iCurViewMap);
// |                                     gpGame->m_campaignType = iCurViewSide;
// |                                 }
// |                             } else {
// |                                 NormalDialog(
// |                                     "\xc2\xfb\xe1\xf0\xe0\xed\xed\xe0\xff \xea\xe0\xf0\xf2\xe0 - "
// |                                         "\xef\xeb\xee\xf5\xee\xe9 \xe2\xfb\xe1\xee\xf0 \xe4\xeb\xff \xe2\xe0\xf8\xe5\xe3\xee "
// |                                         "\xf1\xeb\xe5\xe4\xf3\xfe\xf9\xe5\xe3\xee \xf1\xf6\xe5\xed\xe0\xf0\xe8\xff."
// |                                         /* "Выбранная карта - плохой выбор для вашего следующего сценария." */,
// |                                     NORMAL_DIALOG_INFO,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0,
// |                                     NORMAL_DIALOG_NO_RESOURCE,
// |                                     0
// |                                 );
// |                                 break;
// |                             }
// |                         }
// |                     case CAMPAIGN_DIALOG_CANCEL:
// |                     case CAMPAIGN_DIALOG_RESTART:
// |                         gpWindowManager->m_dialogResult = message.payload.widget.id;
// |                         message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
// |                         message.payload.widget.command =
// |                             BaseWidgetCommand(CAMPAIGN_CLOSE_COMMAND);
// |                         giDialogTimeout = 0;
// |                         return MESSAGE_DISPATCH_FORWARD;
// |                 }
// |                 break;
// |         }
// |     }
// |     return MESSAGE_DISPATCH_CONSUME;
// | }
// ALTERNATIVE scenario (unified source delta from BEFORE):
// | --- before
// | +++ scenario
// | @@ -86,7 +86,7 @@
// |                                      for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map)
// |                                          gpGame->m_campaignAwards[map] = 0;
// |                                  } else {
// | -                                    gpGame->m_campaignScenario = static_cast<i8>(iCurViewMap);
// | +                                    gpGame->m_campaignScenario = iCurViewMap;
// |                                      gpGame->m_campaignType = iCurViewSide;
// |                                  }
// |                              } else {
// ALTERNATIVE choice (unified source delta from BEFORE):
// | --- before
// | +++ choice
// | @@ -52,9 +52,7 @@
// |                          if (!bCampaignViewOnly
// |                              && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap]) {
// |                              gpGame->m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] =
// | -                                static_cast<u8>(
// | -                                    message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST
// | -                                );
// | +                                message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST;
// |                              gpGame->CampaignInfoUpdate(1);
// |                          }
// |                          break;
// ALTERNATIVE choice_scenario (unified source delta from BEFORE):
// | --- before
// | +++ choice_scenario
// | @@ -52,9 +52,7 @@
// |                          if (!bCampaignViewOnly
// |                              && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap]) {
// |                              gpGame->m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] =
// | -                                static_cast<u8>(
// | -                                    message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST
// | -                                );
// | +                                message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST;
// |                              gpGame->CampaignInfoUpdate(1);
// |                          }
// |                          break;
// | @@ -86,7 +84,7 @@
// |                                      for (map = 0; map < CAMPAIGN_AWARD_COUNT; ++map)
// |                                          gpGame->m_campaignAwards[map] = 0;
// |                                  } else {
// | -                                    gpGame->m_campaignScenario = static_cast<i8>(iCurViewMap);
// | +                                    gpGame->m_campaignScenario = iCurViewMap;
// |                                      gpGame->m_campaignType = iCurViewSide;
// |                                  }
// |                              } else {
