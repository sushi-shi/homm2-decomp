// VC6 SP5 family s71-xcamp-widget-initializers; parent2abf1283 (capacity arm after C78/O08 integration).
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/s71-xcamp-widget-initializers/results.json; RVA 0xb3aba
// Complete 4 arms; elapsed 2.688932s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// O08 changes only +0x20 DIR32 owner/addend: name table minus16 = original retail operand.
// Full98 native comparison allows exactly those two pinned physical-identity rewrites, nothing else.
// ShowInfo private EH/FS model gaps remain explicit; native equality does not close them.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 843 / 5bd8031910c43836 / 809b8bf2323a432f / 25:25 / 25,0,0,0,0 / True
// 1 / {'family': 'remove_01', 'candidate': 'baseline'} / 836 / 3f268f955a45766b / af50135ae9af91f8 / 25:25 / 24,1,0,0,0 / True
// 2 / {'family': 'remove_10', 'candidate': 'baseline'} / 836 / 111315308b6f6d89 / 0d201a7ca5af34da / 25:25 / 24,1,0,0,0 / True
// 3 / {'family': 'remove_11', 'candidate': 'baseline'} / 829 / 949ca0cf432283ea / ee2da74a6a7a9088 / 25:25 / 23,2,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x34b)
// | void ExpCampaign::ShowInfo(i32 viewOnly, i32) {
// |     m_viewOnly = viewOnly;
// |     gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
// |     gpMouseManager->ReallyShowPointer();
// |     b32 savedTheme = gbUseEvilInterface;
// |     gbUseEvilInterface = true;
// |     m_viewMap = m_currentMap;
// |     m_window = new heroWindow(0, 0, "x_camp.bin");
// |     if (m_window == NULL)
// |         MemError();
// |
// |     widget* trackWidget = NULL;
// |     i32 mapIndex;
// |     for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
// |         trackWidget = new iconWidget(
// |             expansionCampaignTrackXY[IDX(m_campaignId)][mapIndex][0],
// |             expansionCampaignTrackXY[IDX(m_campaignId)][mapIndex][1],
// |             EXPANSION_CAMPAIGN_TRACK_ICON_SIZE,
// |             EXPANSION_CAMPAIGN_TRACK_ICON_SIZE,
// |             "x_cmpext.icn",
// |             0,
// |             ICON_DRAW_NORMAL,
// |             mapIndex + CAMPAIGN_TRACK_WIDGET_FIRST,
// |             WIDGET_KIND_ICON_DIRECT,
// |             1
// |         );
// |         if (trackWidget == NULL)
// |             MemError();
// |         m_window->AddWidget(trackWidget, -1);
// |     }
// |
// |     widget* campIcon = NULL;
// |     campIcon = new iconWidget(
// |         CAMPAIGN_ICON_X,
// |         CAMPAIGN_ICON_Y,
// |         CAMPAIGN_ICON_WIDTH,
// |         CAMPAIGN_ICON_HEIGHT,
// |         "x_cmpext.icn",
// |         IDX(m_campaignId) + EXPANSION_CAMPAIGN_ICON_FRAME_BASE,
// |         ICON_DRAW_NORMAL,
// |         -1,
// |         WIDGET_KIND_ICON_DIRECT,
// |         1
// |     );
// |     if (campIcon == NULL)
// |         MemError();
// |     m_window->AddWidget(campIcon, -1);
// |
// |     tag_message message;
// |     message.type = MESSAGE_WIDGET;
// |     if (viewOnly == 0) {
// |         message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
// |         message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
// |         message.payload.widget.data.value = IDX(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
// |         m_window->BroadcastMessage(message);
// |     }
// |     gpSoundManager->SwitchAmbientMusic(EXPANSION_CAMPAIGN_MUSIC);
// |     UpdateInfo(0);
// |     gpWindowManager->DoDialog(m_window, MessageHandler, 0);
// |     delete m_window;
// |     gbUseEvilInterface = savedTheme;
// |
// |     if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
// |         NormalDialog(
// |             /* "Вы действительно хотите начать сначала сценарий?" */
// |             "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee "
// |             "\xf5\xee\xf2\xe8\xf2\xe5 \xed\xe0\xf7\xe0\xf2\xfc "
// |             "\xf1\xed\xe0\xf7\xe0\xeb\xe0 \xf1\xf6\xe5\xed\xe0\xf0\xe8\xe9?",
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
// |             InitMap();
// |             gpAdvManager->m_visibilityMapValid = false;
// |             giBottomViewOverride = BOTTOM_VIEW_NONE;
// |             gpWindowManager->FadeScreen(FADE_OUT, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
// |             gpAdvManager->SetInitialMapOrigin();
// |             gpAdvManager->RedrawAdvScreen(1, 0);
// |             gpWindowManager->FadeScreen(FADE_IN, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
// |         }
// |     }
// | }
// |
// ALTERNATIVE remove_01 (unified delta from BEFORE):
// | --- before
// | +++ remove_01
// | @@ -30,7 +30,7 @@
// |          m_window->AddWidget(trackWidget, -1);
// |      }
// |
// | -    widget* campIcon = NULL;
// | +    widget* campIcon;
// |      campIcon = new iconWidget(
// |          CAMPAIGN_ICON_X,
// |          CAMPAIGN_ICON_Y,
// ALTERNATIVE remove_10 (unified delta from BEFORE):
// | --- before
// | +++ remove_10
// | @@ -10,7 +10,7 @@
// |      if (m_window == NULL)
// |          MemError();
// |
// | -    widget* trackWidget = NULL;
// | +    widget* trackWidget;
// |      i32 mapIndex;
// |      for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
// |          trackWidget = new iconWidget(
// ALTERNATIVE remove_11 (unified delta from BEFORE):
// | --- before
// | +++ remove_11
// | @@ -10,7 +10,7 @@
// |      if (m_window == NULL)
// |          MemError();
// |
// | -    widget* trackWidget = NULL;
// | +    widget* trackWidget;
// |      i32 mapIndex;
// |      for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
// |          trackWidget = new iconWidget(
// | @@ -30,7 +30,7 @@
// |          m_window->AddWidget(trackWidget, -1);
// |      }
// |
// | -    widget* campIcon = NULL;
// | +    widget* campIcon;
// |      campIcon = new iconWidget(
// |          CAMPAIGN_ICON_X,
// |          CAMPAIGN_ICON_Y,
