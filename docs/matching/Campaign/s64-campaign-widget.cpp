// VC6 SP5 Campaign source family s64-campaign-widget; parent d7472bf9.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s64-campaign-widget/results.json; RVA 0x226d2
// Complete 2 arms; elapsed 1.627183s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1034 / 1d12751a3c8322ec / 27433281ba355728 / 51:51 / 51,0,0,0 / True
// 1 / {'source_family': 'overwritten_null', 'candidate': 'baseline'} / 1027 / c9ef1d252a670b93 / 48e74f8a6cb7530b / 51:51 / 50,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | void game::ShowCampaignInfo(i32 viewOnly, i32) {
// |     widget* trackWidget;
// |     i32 mapIndex;
// |     b32 savedInterface;
// |     tag_message message;
// |     i32 trackMapIndex;
// |
// |     gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
// |     gpMouseManager->ReallyShowPointer();
// |     savedInterface = gbUseEvilInterface;
// |     gbUseEvilInterface = m_campaignType == CAMPAIGN_ARCHIBALD;
// |     bCampaignViewOnly = viewOnly;
// |     iCurViewSide = m_campaignType;
// |     iCurViewMap = m_campaignScenario;
// |     if (m_campaignScenario == CAMPAIGN_SWITCHING_SCENARIO && !viewOnly) {
// |         if (m_campaignType == CAMPAIGN_ROLAND)
// |             iCampaignTrackType = SWITCH_TO_ROLAND;
// |         else if (m_campaignScenarioCompleted[IDX(CAMPAIGN_ARCHIBALD)][MAP_THREE])
// |             iCampaignTrackType = SWITCH_TO_ARCHIBALD_COMPLETE;
// |         else
// |             iCampaignTrackType = SWITCH_TO_ARCHIBALD_OPEN;
// |     } else {
// |         iCampaignTrackType = static_cast<CampaignTrackType>(
// |             IDX(m_campaignStartingSide) * IDX(CAMPAIGN_SIDE_COUNT) + IDX(m_campaignType)
// |         );
// |     }
// |
// |     campWin = new heroWindow(0, 0, "campaign.bin");
// |     if (campWin == NULL)
// |         MemError();
// |     trackWidget = NULL;
// |     for (mapIndex = 0; mapIndex < CAMPAIGN_TRACK_POINT_COUNT; ++mapIndex) {
// |         if (IDX(iCampaignTrackType) < IDX(SWITCH_TO_ROLAND)
// |             && mapIndex >= CAMPAIGN_REGULAR_MAP_COUNT)
// |             continue;
// |         if (iCampaignTrackType == SWITCH_TO_ROLAND && mapIndex == MAP_THIRTEEN)
// |             continue;
// |         if (iCampaignTrackType == SWITCH_TO_ARCHIBALD_COMPLETE && mapIndex == MAP_THIRTEEN)
// |             continue;
// |         if (iCampaignTrackType == SWITCH_TO_ARCHIBALD_OPEN && mapIndex == MAP_TWELVE)
// |             continue;
// |         if (trackXY[IDX(iCurViewSide)][mapIndex][TRACK_X] != -1) {
// |             trackMapIndex = mapIndex;
// |             if (trackMapIndex > CAMPAIGN_REGULAR_MAP_COUNT)
// |                 trackMapIndex = CAMPAIGN_REGULAR_MAP_COUNT;
// |             trackWidget = new iconWidget(
// |                 trackXY[IDX(mapIndex < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
// |                                                                : m_campaignType)][mapIndex][TRACK_X]
// |                     - CAMPAIGN_TRACK_ICON_OFFSET,
// |                 trackXY[IDX(mapIndex < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
// |                                                                : m_campaignType)][mapIndex][TRACK_Y]
// |                     - CAMPAIGN_TRACK_ICON_OFFSET,
// |                 CAMPAIGN_TRACK_ICON_SIZE,
// |                 CAMPAIGN_TRACK_ICON_SIZE,
// |                 "campxtrg.icn",
// |                 CAMPAIGN_TRACK_ICON_FRAME,
// |                 ICON_DRAW_NORMAL,
// |                 trackMapIndex + CAMPAIGN_TRACK_WIDGET_FIRST,
// |                 WIDGET_KIND_ICON_DIRECT,
// |                 TRACK_ICON_FILL_COLOR
// |             );
// |             if (trackWidget == NULL)
// |                 MemError();
// |             campWin->AddWidget(trackWidget, -1);
// |         }
// |     }
// |
// |     message.type = MESSAGE_WIDGET;
// |     if (!viewOnly) {
// |         message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
// |         message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
// |         message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
// |         campWin->BroadcastMessage(message);
// |     }
// |     gpSoundManager->SwitchAmbientMusic(
// |         m_campaignType == CAMPAIGN_ROLAND ? CAMPAIGN_GOOD_MUSIC : CAMPAIGN_EVIL_MUSIC
// |     );
// |     CampaignInfoUpdate(0);
// |     gpWindowManager->DoDialog(campWin, CampaignHandler, 0);
// |     delete campWin;
// |     gbUseEvilInterface = savedInterface;
// |
// |     if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
// |         NormalDialog(
// |             "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc \xf1\xed\xe0\xf7\xe0\xeb\xe0 \xf1\xf6\xe5\xed\xe0\xf0\xe8\xe9?",
// |             CAMPAIGN_RESTART_CONFIRM,
// |             CAMPAIGN_DIALOG_NO_RESOURCE,
// |             CAMPAIGN_DIALOG_NO_RESOURCE,
// |             CAMPAIGN_DIALOG_NO_RESOURCE,
// |             0,
// |             CAMPAIGN_DIALOG_NO_RESOURCE,
// |             0,
// |             CAMPAIGN_DIALOG_NO_RESOURCE,
// |             0
// |         );
// |         if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
// |             InitCampaignMap();
// |             gpAdvManager->m_visibilityMapValid = false;
// |             giBottomViewOverride = BOTTOM_VIEW_NONE;
// |             gpWindowManager->FadeScreen(FADE_OUT, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
// |             gpAdvManager->SetInitialMapOrigin();
// |             gpAdvManager->RedrawAdvScreen(1, 0);
// |             gpWindowManager->FadeScreen(FADE_IN, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
// |         }
// |     }
// | }
// ALTERNATIVE overwritten_null (unified source delta from BEFORE):
// | --- before
// | +++ overwritten_null
// | @@ -28,7 +28,6 @@
// |      campWin = new heroWindow(0, 0, "campaign.bin");
// |      if (campWin == NULL)
// |          MemError();
// | -    trackWidget = NULL;
// |      for (mapIndex = 0; mapIndex < CAMPAIGN_TRACK_POINT_COUNT; ++mapIndex) {
// |          if (IDX(iCampaignTrackType) < IDX(SWITCH_TO_ROLAND)
// |              && mapIndex >= CAMPAIGN_REGULAR_MAP_COUNT)
