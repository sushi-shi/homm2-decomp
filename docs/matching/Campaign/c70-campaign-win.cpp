// VC6 SP5 Campaign source family c70-campaign-win; parent d7472bf9.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c70-campaign-win/results.json; RVA 0x21f00
// Complete 2 arms; elapsed 1.619709s; input restored.
// Disposition: retain all cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1264 / 599a40c570b51d2e / b8618a9d08c8cfef / 57:58 / 56,0,0,1 / False
// 1 / {'source_family': 'scenario', 'candidate': 'baseline'} / 1264 / 599a40c570b51d2e / 94a04420f58cf0da / 57:58 / 56,0,0,1 / False

// AXIS source_family
// BEFORE (complete exact span):
// | i32 game::HandleCampaignWin(void) {
// |     H2_ENUM_STORAGE_STEPPED(CampaignSide, i32) sideIndex;
// |     i32 mapIndex;
// |
// |     memset(m_campaignMapEnabled, 0, sizeof(m_campaignMapEnabled));
// |     if (m_campaignType == CAMPAIGN_ROLAND) {
// |         switch (m_campaignScenario + 1) {
// |             case SCENARIO_INTRO:
// |                 PlaySmacker(SMACKER_ROLAND_INTRO);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_ONE] = 1;
// |                 break;
// |             case SCENARIO_ONE:
// |                 PlaySmacker(SMACKER_ROLAND_1);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_TWO] = 1;
// |                 break;
// |             case SCENARIO_TWO:
// |                 PlaySmacker(SMACKER_ROLAND_2);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_THREE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_FOUR] = 1;
// |                 break;
// |             case SCENARIO_THREE:
// |                 PlaySmacker(SMACKER_ROLAND_3B);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_FOUR] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARVEN_ALLIANCE)] = 1;
// |                 break;
// |             case SCENARIO_FOUR:
// |                 PlaySmacker(SMACKER_ROLAND_4);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_FIVE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_TWELVE] = 1;
// |                 break;
// |             case SCENARIO_FIVE:
// |                 if (m_campaignStartingSide == CAMPAIGN_ROLAND)
// |                     PlaySmacker(SMACKER_ROLAND_5A);
// |                 else
// |                     PlaySmacker(SMACKER_ROLAND_5B);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_SIX] = 1;
// |                 break;
// |             case SCENARIO_SIX:
// |                 PlaySmacker(SMACKER_ROLAND_6);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_SEVEN] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_EIGHT] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_SORCERESS_GUILD)] = 1;
// |                 break;
// |             case SCENARIO_SEVEN:
// |                 PlaySmacker(SMACKER_ROLAND_8);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_NINE] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_CARRYOVER_FORCES)] = 1;
// |                 break;
// |             case SCENARIO_EIGHT:
// |                 PlaySmacker(SMACKER_ROLAND_8);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_NINE] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ROLAND_ULTIMATE_CROWN)] = 1;
// |                 break;
// |             case SCENARIO_NINE:
// |                 PlaySmacker(SMACKER_ROLAND_9);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ROLAND)][MAP_TEN] = 1;
// |                 break;
// |             case SCENARIO_TEN:
// |                 PlaySmacker(SMACKER_ROLAND_END);
// |                 break;
// |         }
// |     } else {
// |         switch (m_campaignScenario + 1) {
// |             case SCENARIO_INTRO:
// |                 PlaySmacker(SMACKER_ARCHIBALD_INTRO);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_ONE] = 1;
// |                 break;
// |             case SCENARIO_ONE:
// |                 PlaySmacker(SMACKER_ARCHIBALD_1);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_TWO] = 1;
// |                 break;
// |             case SCENARIO_TWO:
// |                 PlaySmacker(SMACKER_ARCHIBALD_2);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_THREE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_FOUR] = 1;
// |                 break;
// |             case SCENARIO_THREE:
// |                 PlaySmacker(SMACKER_ARCHIBALD_4A);
// |                 PlaySmacker(SMACKER_ARCHIBALD_4_END);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_FIVE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_TWELVE] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_NECROMANCER_GUILD)] = 1;
// |                 break;
// |             case SCENARIO_FOUR:
// |                 PlaySmacker(SMACKER_ARCHIBALD_4B);
// |                 PlaySmacker(SMACKER_ARCHIBALD_4_END);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_FIVE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_TWELVE] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_DWARFBANE)] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_OGRE_ALLIANCE)] = 1;
// |                 break;
// |             case SCENARIO_FIVE:
// |                 if (m_campaignStartingSide == CAMPAIGN_ARCHIBALD)
// |                     PlaySmacker(SMACKER_ARCHIBALD_5A);
// |                 else
// |                     PlaySmacker(SMACKER_ARCHIBALD_5B);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_SIX] = 1;
// |                 break;
// |             case SCENARIO_SIX:
// |                 PlaySmacker(SMACKER_ARCHIBALD_6);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_SEVEN] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_EIGHT] = 1;
// |                 break;
// |             case SCENARIO_SEVEN:
// |                 PlaySmacker(SMACKER_ARCHIBALD_7B);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_EIGHT] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_DRAGON_ALLIANCE)] = 1;
// |                 break;
// |             case SCENARIO_EIGHT:
// |                 PlaySmacker(SMACKER_ARCHIBALD_8);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_NINE] = 1;
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_TEN] = 1;
// |                 break;
// |             case SCENARIO_NINE:
// |                 PlaySmacker(SMACKER_ARCHIBALD_10);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_ELEVEN] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 1;
// |                 break;
// |             case SCENARIO_TEN:
// |                 PlaySmacker(SMACKER_ARCHIBALD_10);
// |                 m_campaignMapEnabled[IDX(CAMPAIGN_ARCHIBALD)][MAP_ELEVEN] = 1;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 1;
// |                 break;
// |             case SCENARIO_ELEVEN:
// |                 PlaySmacker(SMACKER_ARCHIBALD_END);
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_ULTIMATE_CROWN)] = 0;
// |                 m_campaignAwards[IDX(CAMPAIGN_AWARD_ARCHIBALD_CARRYOVER_FORCES)] = 0;
// |                 break;
// |         }
// |     }
// |
// |     if (m_campaignScenario + 1 != CAMPAIGN_ARCHIBALD_FINAL_SCENARIO + 1
// |         && (gpGame->m_campaignScenario + 1 != CAMPAIGN_ROLAND_FINAL_SCENARIO + 1
// |             || gpGame->m_campaignType != CAMPAIGN_ROLAND)) {
// |         m_campaignScenario = CAMPAIGN_NO_SCENARIO;
// |         for (sideIndex = CAMPAIGN_ROLAND; sideIndex < CAMPAIGN_SIDE_COUNT; ++sideIndex) {
// |             for (mapIndex = 0; mapIndex < CAMPAIGN_REGULAR_MAP_COUNT; ++mapIndex) {
// |                 if (m_campaignMapEnabled[IDX(sideIndex)][mapIndex]) {
// |                     gpGame->m_campaignScenarioBonus[IDX(sideIndex)][mapIndex] =
// |                         m_campaignScore;
// |                     if (m_campaignScenario == CAMPAIGN_NO_SCENARIO) {
// |                         m_campaignType = sideIndex;
// |                         m_campaignScenario = static_cast<i8>(mapIndex);
// |                     }
// |                 }
// |             }
// |         }
// |         gpGame->ShowCampaignInfo(0, 0);
// |         switch (gpWindowManager->m_dialogResult) {
// |             case CAMPAIGN_DIALOG_ACCEPT:
// |                 return 1;
// |             case CAMPAIGN_DIALOG_CANCEL:
// |                 return 0;
// |         }
// |     }
// |     return 0;
// | }
// ALTERNATIVE scenario (unified source delta from BEFORE):
// | --- before
// | +++ scenario
// | @@ -140,7 +140,7 @@
// |                          m_campaignScore;
// |                      if (m_campaignScenario == CAMPAIGN_NO_SCENARIO) {
// |                          m_campaignType = sideIndex;
// | -                        m_campaignScenario = static_cast<i8>(mapIndex);
// | +                        m_campaignScenario = mapIndex;
// |                      }
// |                  }
// |              }
