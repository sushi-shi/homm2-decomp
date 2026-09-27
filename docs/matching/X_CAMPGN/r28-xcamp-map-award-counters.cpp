// VC6 SP5 family r28-xcamp-map-award-counters; parent2abf1283 (capacity arm after C78/O08 integration).
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/r28-xcamp-map-award-counters/results.json; RVA 0xb3e05
// Complete 4 arms; elapsed 2.755192s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// O08 changes only +0x20 DIR32 owner/addend: name table minus16 = original retail operand.
// Full98 native comparison allows exactly those two pinned physical-identity rewrites, nothing else.
// ShowInfo private EH/FS model gaps remain explicit; native equality does not close them.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 2109 / 2a32552fad1c723f / 16d75230b4acd43f / 84:84 / 83,1,0,0,0 / True
// 1 / {'family': 'split_01', 'candidate': 'baseline'} / 2109 / 87cfd3fa6fa02356 / 98d736078a4cf834 / 84:84 / 83,1,0,0,0 / True
// 2 / {'family': 'split_10', 'candidate': 'baseline'} / 2109 / 02109c11dbfba4ae / 98d736078a4cf834 / 84:84 / 83,1,0,0,0 / True
// 3 / {'family': 'split_11', 'candidate': 'baseline'} / 2109 / f850c0aa8b184503 / a0b8084495be35ac / 84:84 / 83,1,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x83d)
// | void ExpCampaign::UpdateInfo(i32 redraw) {
// |     SCampaignChoice* choice;
// |     tag_message message;
// |     char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |     b8 hasVisibleAward;
// |     i32 i;
// |     b8 showScroll;
// |
// |     message.type = MESSAGE_WIDGET;
// |     for (i = 0; i < m_mapCount; ++i) {
// |         if (m_mapChoices[i] != 0)
// |             message.payload.widget.data.value = TRACK_FRAME_AVAILABLE;
// |         else if (m_mapsPlayed[i] != 0)
// |             message.payload.widget.data.value = TRACK_FRAME_PLAYED;
// |         else
// |             message.payload.widget.data.value = TRACK_FRAME_LOCKED;
// |         if (i == IDX(m_viewMap))
// |             message.payload.widget.data.value +=
// |                 (IDX(m_campaignId) + TRACK_SELECTED_CAMPAIGN_OFFSET)
// |                 * TRACK_FRAME_CAMPAIGN_STRIDE;
// |         message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |         message.payload.widget.id = i + CAMPAIGN_TRACK_WIDGET_FIRST;
// |         m_window->BroadcastMessage(message);
// |     }
// |
// |     message.payload.widget.command = CAMPAIGN_MESSAGE_SET_ICON;
// |     message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
// |     message.payload.widget.data.text = gText;
// |     sprintf(gText, "x_track%d.icn", IDX(m_campaignId) + 1);
// |     m_window->BroadcastMessage(message);
// |
// |     message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// |     message.payload.widget.data.text = gText;
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
// |     sprintf(gText, "%d", IDX(m_viewMap) + 1);
// |     m_window->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
// |     sprintf(gText, "%s", xScenarioName[IDX(m_campaignId)][IDX(m_viewMap)]);
// |     m_window->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
// |     sprintf(gText, "%s", xScenarioDescription[IDX(m_campaignId)][IDX(m_viewMap)]);
// |     m_window->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_BONUS_WIDGET;
// |     sprintf(gText, "%d", m_mapDays[IDX(m_viewMap)]);
// |     m_window->BroadcastMessage(message);
// |
// |     hasVisibleAward = false;
// |     message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |     strcpy(gText, "");
// |     for (i = 0; i < EXPANSION_CAMPAIGN_AWARD_COUNT; ++i) {
// |         if (m_awards[i] != 0) {
// |             hasVisibleAward = true;
// |             strcat(gText, xCampaignAwards[i]);
// |             strcat(gText, "\n");
// |         }
// |     }
// |     if (hasVisibleAward == 0)
// |         sprintf(gText, "\xcd\xe5\xf2" /* "Нет" */);
// |     m_window->BroadcastMessage(message);
// |
// |     for (i = 0; i < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++i) {
// |         choice = &xCampaignChoices[IDX(m_campaignId)][IDX(m_viewMap)][i];
// |         switch (choice->type) {
// |             case CAMPAIGN_CHOICE_RESOURCE:
// |                 sprintf(gText, "%s: %d", gResourceNames[IDX(choice->resource)], choice->amount);
// |                 break;
// |             case CAMPAIGN_CHOICE_ARTIFACT:
// |                 switch (choice->artifact) {
// |                     case ARTIFACT_MINOR_SCROLL:
// |                         strcpy(
// |                             gText,
// |                             "\xcc\xe0\xeb\xfb\xe9 \xf1\xe2\xe8\xf2\xee\xea" /* "Малый свиток" */
// |                         );
// |                         break;
// |                     case ARTIFACT_MAGE_RING:
// |                         strcpy(
// |                             gText,
// |                             "\xca\xee\xeb\xfc\xf6\xee \xec\xe0\xe3\xe0" /* "Кольцо мага" */
// |                         );
// |                         break;
// |                     case ARTIFACT_DEFENDER_HELM:
// |                         strcpy(
// |                             gText,
// |                             /* "Щлем защитника" */
// |                             "\xd9\xeb\xe5\xec \xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_POWER_AXE:
// |                         strcpy(gText, "\xd2\xee\xef\xee\xf0 \xf1\xe8\xeb\xfb" /* "Топор силы" */);
// |                         break;
// |                     case ARTIFACT_DRAGON_SWORD:
// |                         strcpy(
// |                             gText,
// |                             "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9 \xec\xe5\xf7" /* "Драконий меч" */
// |                         );
// |                         break;
// |                     case ARTIFACT_DIVINE_BREASTPLATE:
// |                         strcpy(gText, "\xc4\xee\xf1\xef\xe5\xf5\xe8" /* "Доспехи" */);
// |                         break;
// |                     case ARTIFACT_FIZBIN_OF_MISFORTUNE:
// |                         strcpy(
// |                             gText,
// |                             /* "Символ неудачи" */
// |                             "\xd1\xe8\xec\xe2\xee\xeb \xed\xe5\xf3\xe4\xe0\xf7\xe8"
// |                         );
// |                         break;
// |                     case ARTIFACT_THUNDER_MACE:
// |                         strcpy(
// |                             gText,
// |                             /* "Громовая палица" */
// |                             "\xc3\xf0\xee\xec\xee\xe2\xe0\xff \xef\xe0\xeb\xe8\xf6\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_ARMORED_GAUNTLETS:
// |                         strcpy(gText, "\xcf\xe5\xf0\xf7\xe0\xf2\xea\xe8" /* "Перчатки" */);
// |                         break;
// |                     case ARTIFACT_MAJOR_SCROLL:
// |                         strcpy(
// |                             gText,
// |                             "\xcc\xe0\xeb\xfb\xe9 \xf1\xe2\xe8\xf2\xee\xea" /* "Малый свиток" */
// |                         );
// |                         break;
// |                     case ARTIFACT_FOREMOST_SCROLL:
// |                         strcpy(
// |                             gText,
// |                             /* "Свиток высш. зн." */
// |                             "\xd1\xe2\xe8\xf2\xee\xea \xe2\xfb\xf1\xf8. \xe7\xed."
// |                         );
// |                         break;
// |                     case ARTIFACT_BALLISTA:
// |                         strcpy(gText, "\xc1\xe0\xeb\xeb\xe8\xf1\xf2\xe0" /* "Баллиста" */);
// |                         break;
// |                     case ARTIFACT_STEALTH_SHIELD:
// |                         strcpy(
// |                             gText,
// |                             "\xcd\xe5\xe7\xf0\xe8\xec\xfb\xe9 \xf9\xe8\xf2" /* "Незримый щит" */
// |                         );
// |                         break;
// |                     case ARTIFACT_NOMAD_BOOTS:
// |                         strcpy(
// |                             gText,
// |                             /* "Башмаки кочевника" */
// |                             "\xc1\xe0\xf8\xec\xe0\xea\xe8 \xea\xee\xf7\xe5\xe2\xed\xe8\xea\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_TRAVELER_BOOTS:
// |                         strcpy(
// |                             gText,
// |                             /* "Башмаки путника" */
// |                             "\xc1\xe0\xf8\xec\xe0\xea\xe8 \xef\xf3\xf2\xed\xe8\xea\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_HIDEOUS_MASK:
// |                     case ARTIFACT_BLACK_PEARL:
// |                     default:
// |                         sprintf(gText, "%s", gArtifactNames[IDX(choice->artifact)]);
// |                         break;
// |                 }
// |                 break;
// |             case CAMPAIGN_CHOICE_SPELL:
// |                 if (choice->spell == SPELL_SUMMON_EARTH_ELEMENTAL)
// |                     sprintf(
// |                         gText,
// |                         "\xcf\xf0\xe8\xe7\xe2\xe0\xf2\xfc \xe7\xe5\xec\xeb\xff\xed\xfb\xf5 \xfd"
// |                         "\xeb." /* "Призвать земляных эл." */
// |                     );
// |                 else
// |                     sprintf(gText, "%s", gSpellNames[IDX(choice->spell)]);
// |                 break;
// |             case CAMPAIGN_CHOICE_SECONDARY_SKILL:
// |                 if ((choice->amount == EXPANSION_CAMPAIGN_SPECIAL_SKILL_LEVEL
// |                      && choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL)
// |                     || (choice->amount == EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT_LEVEL
// |                         && choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT)) {
// |                     sprintf(
// |                         gText,
// |                         "%s %s",
// |                         xShortSSLevelNames[choice->amount - 1],
// |                         gSecondarySkills[choice->value]
// |                     );
// |                 } else {
// |                     sprintf(
// |                         gText,
// |                         "%s %s",
// |                         gSecondarySkillLevels[choice->amount - 1],
// |                         gSecondarySkills[choice->value]
// |                     );
// |                 }
// |                 break;
// |             case CAMPAIGN_CHOICE_CREATURES:
// |                 strcpy(armyName, gArmyNamesPlural[IDX(choice->creature)]);
// |                 sprintf(gText, "%d %s", choice->amount, armyName);
// |                 break;
// |             case CAMPAIGN_CHOICE_PUZZLE_PIECES:
// |                 sprintf(
// |                     gText,
// |                     "%d %s",
// |                     choice->value,
// |                     "\xce\xe1\xf0\xfb\xe2\xea\xe8 \xea\xe0\xf0\xf2\xfb" /* "Обрывки карты" */
// |                 );
// |                 break;
// |             case CAMPAIGN_CHOICE_EXPERIENCE:
// |                 sprintf(gText, "%d %s", choice->value, "\xce\xef\xfb\xf2" /* "Опыт" */);
// |                 break;
// |             case CAMPAIGN_CHOICE_NONE:
// |                 sprintf(gText, "\xed/\xe4" /* "н/д" */);
// |                 break;
// |             case CAMPAIGN_CHOICE_ALIGNMENT:
// |                 sprintf(gText, gAlignmentNames[IDX(choice->faction)]);
// |                 break;
// |             case CAMPAIGN_CHOICE_PRIMARY_SKILL:
// |                 sprintf(gText, "%s +%d", gStatNames[choice->value], choice->amount);
// |                 break;
// |             case CAMPAIGN_CHOICE_SPELL_SCROLL: {
// |                 showScroll = true;
// |                 switch (choice->spell) {
// |                     case SPELL_DISRUPTING_RAY:
// |                     case SPELL_ANIMATE_DEAD:
// |                         showScroll = false;
// |                         break;
// |                 }
// |                 if (showScroll != 0) {
// |                     sprintf(
// |                         gText,
// |                         "%s %s",
// |                         gSpellNames[IDX(choice->spell)],
// |                         "\xd1\xe2\xe8\xf2\xee\xea" /* "Свиток" */
// |                     );
// |                 } else {
// |                     sprintf(gText, "%s", gSpellNames[IDX(choice->spell)]);
// |                 }
// |                 break;
// |             }
// |         }
// |         message.payload.widget.id = i + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// |         m_window->BroadcastMessage(message);
// |     }
// |
// |     for (i = 0; i < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++i) {
// |         message.payload.widget.id = i + CAMPAIGN_BONUS_WIDGET_FIRST;
// |         message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |         if (m_viewOnly == 0 && m_mapChoices[IDX(m_viewMap)] != 0)
// |             message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
// |         else
// |             message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
// |         m_window->BroadcastMessage(message);
// |
// |         if (m_bonusChoices[IDX(m_viewMap)] == i)
// |             message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
// |         else
// |             message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
// |         message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
// |         m_window->BroadcastMessage(message);
// |     }
// |     if (redraw != 0)
// |         m_window->DrawWindow();
// | }
// |
// ALTERNATIVE split_01 (unified delta from BEFORE):
// | --- before
// | +++ split_01
// | @@ -5,6 +5,7 @@
// |      char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |      b8 hasVisibleAward;
// |      i32 i;
// | +    i32 awardIndex;
// |      b8 showScroll;
// |
// |      message.type = MESSAGE_WIDGET;
// | @@ -51,10 +52,10 @@
// |      hasVisibleAward = false;
// |      message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |      strcpy(gText, "");
// | -    for (i = 0; i < EXPANSION_CAMPAIGN_AWARD_COUNT; ++i) {
// | -        if (m_awards[i] != 0) {
// | +    for (awardIndex = 0; awardIndex < EXPANSION_CAMPAIGN_AWARD_COUNT; ++awardIndex) {
// | +        if (m_awards[awardIndex] != 0) {
// |              hasVisibleAward = true;
// | -            strcat(gText, xCampaignAwards[i]);
// | +            strcat(gText, xCampaignAwards[awardIndex]);
// |              strcat(gText, "\n");
// |          }
// |      }
// ALTERNATIVE split_10 (unified delta from BEFORE):
// | --- before
// | +++ split_10
// | @@ -5,22 +5,23 @@
// |      char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |      b8 hasVisibleAward;
// |      i32 i;
// | +    i32 mapIndex;
// |      b8 showScroll;
// |
// |      message.type = MESSAGE_WIDGET;
// | -    for (i = 0; i < m_mapCount; ++i) {
// | -        if (m_mapChoices[i] != 0)
// | +    for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
// | +        if (m_mapChoices[mapIndex] != 0)
// |              message.payload.widget.data.value = TRACK_FRAME_AVAILABLE;
// | -        else if (m_mapsPlayed[i] != 0)
// | +        else if (m_mapsPlayed[mapIndex] != 0)
// |              message.payload.widget.data.value = TRACK_FRAME_PLAYED;
// |          else
// |              message.payload.widget.data.value = TRACK_FRAME_LOCKED;
// | -        if (i == IDX(m_viewMap))
// | +        if (mapIndex == IDX(m_viewMap))
// |              message.payload.widget.data.value +=
// |                  (IDX(m_campaignId) + TRACK_SELECTED_CAMPAIGN_OFFSET)
// |                  * TRACK_FRAME_CAMPAIGN_STRIDE;
// |          message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// | -        message.payload.widget.id = i + CAMPAIGN_TRACK_WIDGET_FIRST;
// | +        message.payload.widget.id = mapIndex + CAMPAIGN_TRACK_WIDGET_FIRST;
// |          m_window->BroadcastMessage(message);
// |      }
// |
// ALTERNATIVE split_11 (unified delta from BEFORE):
// | --- before
// | +++ split_11
// | @@ -5,22 +5,24 @@
// |      char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |      b8 hasVisibleAward;
// |      i32 i;
// | +    i32 mapIndex;
// | +    i32 awardIndex;
// |      b8 showScroll;
// |
// |      message.type = MESSAGE_WIDGET;
// | -    for (i = 0; i < m_mapCount; ++i) {
// | -        if (m_mapChoices[i] != 0)
// | +    for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
// | +        if (m_mapChoices[mapIndex] != 0)
// |              message.payload.widget.data.value = TRACK_FRAME_AVAILABLE;
// | -        else if (m_mapsPlayed[i] != 0)
// | +        else if (m_mapsPlayed[mapIndex] != 0)
// |              message.payload.widget.data.value = TRACK_FRAME_PLAYED;
// |          else
// |              message.payload.widget.data.value = TRACK_FRAME_LOCKED;
// | -        if (i == IDX(m_viewMap))
// | +        if (mapIndex == IDX(m_viewMap))
// |              message.payload.widget.data.value +=
// |                  (IDX(m_campaignId) + TRACK_SELECTED_CAMPAIGN_OFFSET)
// |                  * TRACK_FRAME_CAMPAIGN_STRIDE;
// |          message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// | -        message.payload.widget.id = i + CAMPAIGN_TRACK_WIDGET_FIRST;
// | +        message.payload.widget.id = mapIndex + CAMPAIGN_TRACK_WIDGET_FIRST;
// |          m_window->BroadcastMessage(message);
// |      }
// |
// | @@ -51,10 +53,10 @@
// |      hasVisibleAward = false;
// |      message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |      strcpy(gText, "");
// | -    for (i = 0; i < EXPANSION_CAMPAIGN_AWARD_COUNT; ++i) {
// | -        if (m_awards[i] != 0) {
// | +    for (awardIndex = 0; awardIndex < EXPANSION_CAMPAIGN_AWARD_COUNT; ++awardIndex) {
// | +        if (m_awards[awardIndex] != 0) {
// |              hasVisibleAward = true;
// | -            strcat(gText, xCampaignAwards[i]);
// | +            strcat(gText, xCampaignAwards[awardIndex]);
// |              strcat(gText, "\n");
// |          }
// |      }
