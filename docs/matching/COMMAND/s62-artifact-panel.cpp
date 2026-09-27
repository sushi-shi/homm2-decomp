// VC6 SP5 COMMAND source family s62-artifact-panel; parent34b6e303 plus U17 packet view.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/s62-artifact-panel/results.json; RVA 0x2e478
// Complete 4 arms; elapsed 3.711887s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'local_role_subset': 'keep', 'candidate': 'baseline'} / 669 / ecf0035613590312 / adab62e85d1b9c9e / 16:16 / 16,0,0,0 / True
// 1 / {'local_role_subset': 'bottom', 'candidate': 'baseline'} / 663 / 1bc286079b20f266 / 1ed2d1d3cacaa4e2 / 16:16 / 15,1,0,0 / True
// 2 / {'local_role_subset': 'width', 'candidate': 'baseline'} / 663 / 18a35a34cf1d0afa / 1ed2d1d3cacaa4e2 / 16:16 / 15,1,0,0 / True
// 3 / {'local_role_subset': 'width_bottom', 'candidate': 'baseline'} / 657 / 3fd77489e7b50350 / e322fe9da7e45238 / 16:16 / 15,1,0,0 / True

// AXIS local_role_subset
// BEFORE (complete exact span):
// | void combatManager::ShowWinLoseArtifact(
// |     class heroWindow* window,
// |     H2_ENUM_PARAM(ArtifactType, i32) artifact
// | ) {
// |     i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |     i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |     tag_message message;
// |     char* artifactName;
// |
// |     sprintf(gText, "\xc2\xfb \xe7\xe0\xf5\xe2\xe0\xf2\xe8\xeb\xe8 \xe2\xf0\xe0\xe6\xe5\xf1\xea\xe8\xe9 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2!");
// |     message.type = MESSAGE_WIDGET;
// |     message.payload.widget.command = COMBAT_WIN_LOSE_TEXT_COMMAND;
// |     message.payload.widget.id = WIN_LOSE_TEXT_ID;
// |     message.payload.widget.data.text = gText;
// |     m_winLoseWindow->BroadcastMessage(message);
// |
// |     m_winLoseBottomWidgets[0] = new iconWidget(
// |         ARTIFACT_BACKGROUND_X,
// |         ARTIFACT_BACKGROUND_Y,
// |         ARTIFACT_BACKGROUND_SIZE,
// |         ARTIFACT_BACKGROUND_SIZE,
// |         "winloseb.icn",
// |         0,
// |         ICON_DRAW_NORMAL,
// |         WIN_LOSE_ARTIFACT_ICON_ID,
// |         WIDGET_KIND_ICON_DIRECT,
// |         1
// |     );
// |     if (m_winLoseBottomWidgets[0] == NULL)
// |         MemError();
// |     window->AddWidget(m_winLoseBottomWidgets[0], -1);
// |
// |     m_winLoseBottomWidgets[1] = new iconWidget(
// |         ARTIFACT_ICON_X,
// |         ARTIFACT_ICON_Y,
// |         ARTIFACT_ICON_SIZE,
// |         ARTIFACT_ICON_SIZE,
// |         "artifact.icn",
// |         IDX(artifact) + 1,
// |         ICON_DRAW_NORMAL,
// |         WIN_LOSE_ARTIFACT_IMAGE_ID,
// |         WIDGET_KIND_ICON_DIRECT,
// |         1
// |     );
// |     if (m_winLoseBottomWidgets[1] == NULL)
// |         MemError();
// |     window->AddWidget(m_winLoseBottomWidgets[1], -1);
// |
// |     artifactName = static_cast<char*>(H2_ALLOC(ARTIFACT_NAME_CAPACITY));
// |     sprintf(artifactName, gArtifactNames[IDX(artifact)]);
// |     m_winLoseBottomTextWidgets[0] = new textWidget(
// |         ARTIFACT_TEXT_X,
// |         ARTIFACT_TEXT_Y,
// |         CASUALTY_WINDOW_WIDTH,
// |         ARTIFACT_TEXT_HEIGHT,
// |         artifactName,
// |         "smalfont.fnt",
// |         FONT_DRAW_DEFAULT,
// |         WIN_LOSE_ARTIFACT_TEXT_ID,
// |         WIDGET_KIND_TEXT,
// |         FONT_ALIGN_CENTER
// |     );
// |     if (m_winLoseBottomTextWidgets[0] == NULL)
// |         MemError();
// |     window->AddWidget(m_winLoseBottomTextWidgets[0], -1);
// |
// |     gpCombatManager->m_winLoseWindow->DrawWindow();
// |     SAMPLE2 playSample;
// |     sprintf(gText, "pickup%02d.82M", SRandom(PICKUP_SAMPLE_FIRST, PICKUP_SAMPLE_LAST));
// |     playSample = LoadPlaySample(gText);
// |     WaitEndSample(&playSample, -1);
// | }
// ALTERNATIVE bottom (unified source delta from BEFORE):
// | --- before
// | +++ bottom
// | @@ -3,7 +3,6 @@
// |      H2_ENUM_PARAM(ArtifactType, i32) artifact
// |  ) {
// |      i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// | -    i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      tag_message message;
// |      char* artifactName;
// |
// ALTERNATIVE width (unified source delta from BEFORE):
// | --- before
// | +++ width
// | @@ -2,7 +2,6 @@
// |      class heroWindow* window,
// |      H2_ENUM_PARAM(ArtifactType, i32) artifact
// |  ) {
// | -    i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// |      i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      tag_message message;
// |      char* artifactName;
// ALTERNATIVE width_bottom (unified source delta from BEFORE):
// | --- before
// | +++ width_bottom
// | @@ -2,8 +2,6 @@
// |      class heroWindow* window,
// |      H2_ENUM_PARAM(ArtifactType, i32) artifact
// |  ) {
// | -    i16 H2_UNUSED(width) = CASUALTY_WINDOW_WIDTH;
// | -    i16 H2_UNUSED(bottom_4) = CASUALTY_WINDOW_BOTTOM;
// |      tag_message message;
// |      char* artifactName;
// |
