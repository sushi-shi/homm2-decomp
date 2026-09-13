// VC6 SP5 Campaign source family r23-campaign-info-roles; parent d7472bf9.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r23-campaign-info-roles/results.json; RVA 0x22adc
// Complete 4 arms; elapsed 2.709212s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 2268 / 492dd832c89fd709 / cbc61308369960f0 / 97:99 / 96,0,0,1 / False
// 1 / {'source_family': 'choice', 'candidate': 'baseline'} / 2268 / 63f2c3ed967db33d / bfcf5b48efa26964 / 97:99 / 96,0,0,1 / False
// 2 / {'source_family': 'award', 'candidate': 'baseline'} / 2268 / 45e2c4b89dcf8d79 / bfcf5b48efa26964 / 97:99 / 96,0,0,1 / False
// 3 / {'source_family': 'award_choice', 'candidate': 'baseline'} / 2268 / b45ad3b073aceb32 / 012501840fba19bf / 97:99 / 96,0,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// | void game::CampaignInfoUpdate(i32 redraw) {
// |     i32 mapIndex;
// |     SCampaignChoice* choice;
// |     tag_message message;
// |     char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |
// |     message.type = MESSAGE_WIDGET;
// |     for (mapIndex = 0; mapIndex < CAMPAIGN_TRACK_POINT_COUNT; ++mapIndex) {
// |         if (m_campaignMapEnabled[IDX(iCurViewSide)][mapIndex]) {
// |             message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_COMPLETE;
// |         } else if (mapIndex < CAMPAIGN_REGULAR_MAP_COUNT
// |                    && m_campaignScenarioCompleted
// |                           [IDX(mapIndex < CAMPAIGN_SWITCHING_SCENARIO ? m_campaignStartingSide
// |                                                                  : m_campaignType)][mapIndex]) {
// |             message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_AVAILABLE;
// |         } else {
// |             message.payload.widget.data.value = CAMPAIGN_TRACK_FRAME_LOCKED;
// |         }
// |         if (mapIndex == iCurViewMap) {
// |             if (mapIndex + 1 == SCENARIO_FIVE && iCampaignTrackType == ROLAND_TO_ARCHIBALD)
// |                 message.payload.widget.data.value += TRACK_SELECTED_FRAME_FOUR_STEPS;
// |             else if (mapIndex + 1 == SCENARIO_FIVE
// |                      && iCampaignTrackType == ARCHIBALD_TO_ROLAND)
// |                 message.payload.widget.data.value += TRACK_SELECTED_FRAME_THREE_STEPS;
// |             else if (mapIndex + 1 > CAMPAIGN_REGULAR_MAP_COUNT)
// |                 message.payload.widget.data.value += m_campaignStartingSide == CAMPAIGN_ROLAND
// |                                                          ? TRACK_SELECTED_FRAME_TWO_STEPS
// |                                                          : TRACK_SELECTED_FRAME_ONE_STEP;
// |             else if (mapIndex + 1 < SCENARIO_FIVE)
// |                 message.payload.widget.data.value += m_campaignStartingSide == CAMPAIGN_ROLAND
// |                                                          ? TRACK_SELECTED_FRAME_ONE_STEP
// |                                                          : TRACK_SELECTED_FRAME_TWO_STEPS;
// |             else
// |                 message.payload.widget.data.value += m_campaignType == CAMPAIGN_ROLAND
// |                                                          ? TRACK_SELECTED_FRAME_ONE_STEP
// |                                                          : TRACK_SELECTED_FRAME_TWO_STEPS;
// |         }
// |         message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |         message.payload.widget.id = mapIndex + CAMPAIGN_TRACK_WIDGET_FIRST;
// |         campWin->BroadcastMessage(message);
// |     }
// |
// |     message.payload.widget.command = CAMPAIGN_MESSAGE_SET_ICON;
// |     message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
// |     message.payload.widget.data.text = gText;
// |     sprintf(gText, "ctrack%02d.icn", IDX(iCampaignTrackType));
// |     campWin->BroadcastMessage(message);
// |
// |     message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
// |     message.payload.widget.data.text = gText;
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
// |     if (iCurViewMap == CAMPAIGN_SWITCHING_MAP)
// |         sprintf(gText, "5");
// |     else
// |         sprintf(gText, "%d", iCurViewMap + 1);
// |     campWin->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
// |     if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// |         sprintf(gText, "%s", cCampaignName[1 - IDX(iCurViewSide)][iCurViewMap]);
// |     } else if (m_campaignType != m_campaignStartingSide
// |                && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// |         sprintf(gText, "%s", cCampaignName[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP]);
// |     } else {
// |         sprintf(gText, "%s", cCampaignName[IDX(iCurViewSide)][iCurViewMap]);
// |     }
// |     campWin->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
// |     if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// |         sprintf(gText, "%s", cCampaignDescription[1 - IDX(iCurViewSide)][iCurViewMap]);
// |     } else if (m_campaignType != m_campaignStartingSide
// |                && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// |         sprintf(
// |             gText,
// |             "%s",
// |             cCampaignDescription[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP]
// |         );
// |     } else {
// |         sprintf(gText, "%s", cCampaignDescription[IDX(iCurViewSide)][iCurViewMap]);
// |     }
// |     campWin->BroadcastMessage(message);
// |
// |     message.payload.widget.id = CAMPAIGN_SCENARIO_BONUS_WIDGET;
// |     sprintf(gText, "%d", m_campaignScenarioBonus[IDX(iCurViewSide)][iCurViewMap]);
// |     campWin->BroadcastMessage(message);
// |
// |     strcpy(gText, "");
// |     for (mapIndex = 0; mapIndex < CAMPAIGN_AWARD_COUNT; ++mapIndex) {
// |         if (m_campaignAwards[mapIndex]) {
// |             strcat(gText, cCampaignAwards[mapIndex]);
// |             strcat(gText, "\n");
// |         }
// |     }
// |     message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |     campWin->BroadcastMessage(message);
// |
// |     for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// |         if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// |             choice = &campaignChoices[1 - IDX(iCurViewSide)][iCurViewMap][mapIndex];
// |         } else if (m_campaignType != m_campaignStartingSide
// |                    && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// |             choice = &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][mapIndex];
// |         } else {
// |             choice = &campaignChoices[IDX(iCurViewSide)][iCurViewMap][mapIndex];
// |         }
// |
// |         switch (choice->type) {
// |             case CAMPAIGN_CHOICE_RESOURCE:
// |                 sprintf(gText, "%s: %d", gResourceNames[IDX(choice->resource)], choice->amount);
// |                 break;
// |             case CAMPAIGN_CHOICE_ARTIFACT:
// |                 switch (choice->artifact) {
// |                     case ARTIFACT_MINOR_SCROLL:
// |                         strcpy(gText, "\xcc\xe0\xeb\xfb\xe9 \xf1\xe2\xe8\xf2\xee\xea");
// |                         break;
// |                     case ARTIFACT_MAGE_RING:
// |                         strcpy(gText, "\xca\xee\xeb\xfc\xf6\xee \xec\xe0\xe3\xe0");
// |                         break;
// |                     case ARTIFACT_DEFENDER_HELM:
// |                         strcpy(
// |                             gText,
// |                             "\xd9\xeb\xe5\xec \xe7\xe0\xf9\xe8\xf2\xed\xe8\xea\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_POWER_AXE:
// |                         strcpy(gText, "\xd2\xee\xef\xee\xf0 \xf1\xe8\xeb\xfb");
// |                         break;
// |                     case ARTIFACT_DRAGON_SWORD:
// |                         strcpy(gText, "\xc4\xf0\xe0\xea\xee\xed\xe8\xe9 \xec\xe5\xf7");
// |                         break;
// |                     case ARTIFACT_DIVINE_BREASTPLATE:
// |                         strcpy(gText, "\xc4\xee\xf1\xef\xe5\xf5\xe8");
// |                         break;
// |                     case ARTIFACT_FIZBIN_OF_MISFORTUNE:
// |                         strcpy(
// |                             gText,
// |                             "\xd1\xe8\xec\xe2\xee\xeb \xed\xe5\xf3\xe4\xe0\xf7\xe8"
// |                         );
// |                         break;
// |                     case ARTIFACT_THUNDER_MACE:
// |                         strcpy(
// |                             gText,
// |                             "\xc3\xf0\xee\xec\xee\xe2\xe0\xff \xef\xe0\xeb\xe8\xf6\xe0"
// |                         );
// |                         break;
// |                     case ARTIFACT_ARMORED_GAUNTLETS:
// |                         strcpy(gText, "\xcf\xe5\xf0\xf7\xe0\xf2\xea\xe8");
// |                         break;
// |                     default:
// |                         sprintf(gText, "%s", gArtifactNames[IDX(choice->artifact)]);
// |                         break;
// |                 }
// |                 break;
// |             case CAMPAIGN_CHOICE_SPELL:
// |                 if (choice->spell == SPELL_SUMMON_EARTH_ELEMENTAL)
// |                     sprintf(gText, "\xcf\xf0\xe8\xe7\xe2\xe0\xf2\xfc \xe7\xe5\xec\xeb\xff\xed\xfb\xf5 \xfd\xeb.");
// |                 else
// |                     sprintf(gText, "%s", gSpellNames[IDX(choice->spell)]);
// |                 break;
// |             case CAMPAIGN_CHOICE_SECONDARY_SKILL:
// |                 sprintf(
// |                     gText,
// |                     "%s %s",
// |                     gSecondarySkillLevels[choice->amount - 1],
// |                     gSecondarySkills[choice->value]
// |                 );
// |                 break;
// |             case CAMPAIGN_CHOICE_CREATURES:
// |                 strcpy(armyName, gArmyNamesPlural[IDX(choice->creature)]);
// |                 sprintf(gText, "%d %s", choice->amount, armyName);
// |                 break;
// |             case CAMPAIGN_CHOICE_PUZZLE_PIECES:
// |                 sprintf(gText, "%d %s", choice->value, "\xce\xe1\xf0\xfb\xe2\xea\xe8 \xea\xe0\xf0\xf2\xfb");
// |                 break;
// |             case CAMPAIGN_CHOICE_EXPERIENCE:
// |                 sprintf(gText, "%d %s", choice->value, "\xce\xef\xfb\xf2");
// |                 break;
// |             case CAMPAIGN_CHOICE_NONE:
// |                 sprintf(gText, "\xed/\xe4");
// |                 break;
// |             case CAMPAIGN_CHOICE_ALIGNMENT:
// |                 sprintf(gText, gAlignmentNames[IDX(choice->faction)]);
// |                 break;
// |         }
// |         message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// |         campWin->BroadcastMessage(message);
// |     }
// |
// |     for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// |         message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
// |         message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |         if (!bCampaignViewOnly
// |             && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap])
// |             message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
// |         else
// |             message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
// |         campWin->BroadcastMessage(message);
// |
// |         if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == mapIndex)
// |             message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
// |         else
// |             message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
// |         message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
// |         campWin->BroadcastMessage(message);
// |     }
// |     if (redraw)
// |         campWin->DrawWindow();
// | }
// ALTERNATIVE choice (unified source delta from BEFORE):
// | --- before
// | +++ choice
// | @@ -1,6 +1,7 @@
// |  void game::CampaignInfoUpdate(i32 redraw) {
// |      i32 mapIndex;
// |      SCampaignChoice* choice;
// | +    i32 choiceIndex;
// |      tag_message message;
// |      char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |
// | @@ -95,14 +96,14 @@
// |      message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |      campWin->BroadcastMessage(message);
// |
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// | +    for (choiceIndex = 0; choiceIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++choiceIndex) {
// |          if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// | -            choice = &campaignChoices[1 - IDX(iCurViewSide)][iCurViewMap][mapIndex];
// | +            choice = &campaignChoices[1 - IDX(iCurViewSide)][iCurViewMap][choiceIndex];
// |          } else if (m_campaignType != m_campaignStartingSide
// |                     && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// | -            choice = &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][mapIndex];
// | +            choice = &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][choiceIndex];
// |          } else {
// | -            choice = &campaignChoices[IDX(iCurViewSide)][iCurViewMap][mapIndex];
// | +            choice = &campaignChoices[IDX(iCurViewSide)][iCurViewMap][choiceIndex];
// |          }
// |
// |          switch (choice->type) {
// | @@ -183,12 +184,12 @@
// |                  sprintf(gText, gAlignmentNames[IDX(choice->faction)]);
// |                  break;
// |          }
// | -        message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// | -        campWin->BroadcastMessage(message);
// | -    }
// | -
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// | -        message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
// | +        message.payload.widget.id = choiceIndex + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// | +        campWin->BroadcastMessage(message);
// | +    }
// | +
// | +    for (choiceIndex = 0; choiceIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++choiceIndex) {
// | +        message.payload.widget.id = choiceIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
// |          message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |          if (!bCampaignViewOnly
// |              && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap])
// | @@ -197,7 +198,7 @@
// |              message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
// |          campWin->BroadcastMessage(message);
// |
// | -        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == mapIndex)
// | +        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == choiceIndex)
// |              message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
// |          else
// |              message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
// ALTERNATIVE award (unified source delta from BEFORE):
// | --- before
// | +++ award
// | @@ -1,5 +1,6 @@
// |  void game::CampaignInfoUpdate(i32 redraw) {
// |      i32 mapIndex;
// | +    i32 awardIndex;
// |      SCampaignChoice* choice;
// |      tag_message message;
// |      char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// | @@ -86,9 +87,9 @@
// |      campWin->BroadcastMessage(message);
// |
// |      strcpy(gText, "");
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_AWARD_COUNT; ++mapIndex) {
// | -        if (m_campaignAwards[mapIndex]) {
// | -            strcat(gText, cCampaignAwards[mapIndex]);
// | +    for (awardIndex = 0; awardIndex < CAMPAIGN_AWARD_COUNT; ++awardIndex) {
// | +        if (m_campaignAwards[awardIndex]) {
// | +            strcat(gText, cCampaignAwards[awardIndex]);
// |              strcat(gText, "\n");
// |          }
// |      }
// ALTERNATIVE award_choice (unified source delta from BEFORE):
// | --- before
// | +++ award_choice
// | @@ -1,6 +1,8 @@
// |  void game::CampaignInfoUpdate(i32 redraw) {
// |      i32 mapIndex;
// | +    i32 awardIndex;
// |      SCampaignChoice* choice;
// | +    i32 choiceIndex;
// |      tag_message message;
// |      char armyName[CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
// |
// | @@ -86,23 +88,23 @@
// |      campWin->BroadcastMessage(message);
// |
// |      strcpy(gText, "");
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_AWARD_COUNT; ++mapIndex) {
// | -        if (m_campaignAwards[mapIndex]) {
// | -            strcat(gText, cCampaignAwards[mapIndex]);
// | +    for (awardIndex = 0; awardIndex < CAMPAIGN_AWARD_COUNT; ++awardIndex) {
// | +        if (m_campaignAwards[awardIndex]) {
// | +            strcat(gText, cCampaignAwards[awardIndex]);
// |              strcat(gText, "\n");
// |          }
// |      }
// |      message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
// |      campWin->BroadcastMessage(message);
// |
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// | +    for (choiceIndex = 0; choiceIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++choiceIndex) {
// |          if (iCurViewMap == CAMPAIGN_SWITCHING_MAP) {
// | -            choice = &campaignChoices[1 - IDX(iCurViewSide)][iCurViewMap][mapIndex];
// | +            choice = &campaignChoices[1 - IDX(iCurViewSide)][iCurViewMap][choiceIndex];
// |          } else if (m_campaignType != m_campaignStartingSide
// |                     && iCurViewMap == CAMPAIGN_SWITCHING_SCENARIO) {
// | -            choice = &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][mapIndex];
// | +            choice = &campaignChoices[IDX(iCurViewSide)][CAMPAIGN_SWITCHING_MAP][choiceIndex];
// |          } else {
// | -            choice = &campaignChoices[IDX(iCurViewSide)][iCurViewMap][mapIndex];
// | +            choice = &campaignChoices[IDX(iCurViewSide)][iCurViewMap][choiceIndex];
// |          }
// |
// |          switch (choice->type) {
// | @@ -183,12 +185,12 @@
// |                  sprintf(gText, gAlignmentNames[IDX(choice->faction)]);
// |                  break;
// |          }
// | -        message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// | -        campWin->BroadcastMessage(message);
// | -    }
// | -
// | -    for (mapIndex = 0; mapIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++mapIndex) {
// | -        message.payload.widget.id = mapIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
// | +        message.payload.widget.id = choiceIndex + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
// | +        campWin->BroadcastMessage(message);
// | +    }
// | +
// | +    for (choiceIndex = 0; choiceIndex < CAMPAIGN_BONUS_CHOICE_COUNT; ++choiceIndex) {
// | +        message.payload.widget.id = choiceIndex + CAMPAIGN_BONUS_WIDGET_FIRST;
// |          message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
// |          if (!bCampaignViewOnly
// |              && gpGame->m_campaignMapEnabled[IDX(iCurViewSide)][iCurViewMap])
// | @@ -197,7 +199,7 @@
// |              message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
// |          campWin->BroadcastMessage(message);
// |
// | -        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == mapIndex)
// | +        if (m_campaignChoice[IDX(iCurViewSide)][iCurViewMap] == choiceIndex)
// |              message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
// |          else
// |              message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
