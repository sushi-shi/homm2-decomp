// VC6 SP5 family r28-xcamp-hero-record-counter; parent2abf1283 (capacity arm after C78/O08 integration).
// Source record only: alternatives are relative to BEFORE, not cumulative.
// No cross-family Cartesian product or TU-state census is claimed.
// build/source-variant-batch/r28-xcamp-hero-record-counter/results.json; RVA 0xb3427
// Complete 2 arms; elapsed 1.932562s; source restored.
// Disposition: retain original; every measured nonoriginal changes function text.
// O08 changes only +0x20 DIR32 owner/addend: name table minus16 = original retail operand.
// Full98 native comparison allows exactly those two pinned physical-identity rewrites, nothing else.
// ShowInfo private EH/FS model gaps remain explicit; native equality does not close them.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow,missing / graph exact
// 0 / {'family': 'keep', 'candidate': 'baseline'} / 1683 / f5a7559553c2560b / a0e4d7059168b409 / 84:84 / 83,1,0,0,0 / True
// 1 / {'family': 'split_global_records', 'candidate': 'baseline'} / 1683 / 4fd030a43bb18064 / a941557840044225 / 84:84 / 83,1,0,0,0 / True

// AXIS family
// BEFORE (complete exact span):
// |  0x693)
// | void ExpCampaign::InitMap(void) {
// |     SCampaignChoice* bonus =
// |         &xCampaignChoices[IDX(m_campaignId)][IDX(m_currentMap)][m_bonusChoices[IDX(m_currentMap)]];
// |
// |     memset(gpGame->m_setupPlayerColor, 0, EXPANSION_CAMPAIGN_PLAYER_SETUP_RESET_SIZE);
// |     sprintf(
// |         gpGame->m_mapFilename,
// |         "CAMP%d_%02d.HXC",
// |         IDX(m_campaignId) + 1,
// |         IDX(m_currentMap) + 1
// |     );
// |     gpGame->m_newGameInitialized = false;
// |     if (m_currentMap == MAP_FIRST)
// |         m_mapDays[0] = 0;
// |     strcpy(gMapName, gpGame->m_mapFilename);
// |     i32 H2_UNUSED(mapHeaderResult) =
// |         GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
// |     gpGame->LoadGame("origdata.bin", 1, 0);
// |     gpGame->InitNewGame(NULL);
// |     gpGame->m_difficulty = expansionCampaignDifficulty[IDX(m_campaignId)][IDX(m_currentMap)];
// |     gpGame->m_playerCount = gpGame->m_mapHeader.playerCount;
// |     gpGame->NewMap(gMapName);
// |
// |     playerData* player = &gpGame->m_players[0];
// |     i32 heroSlot;
// |     hero* pHero;
// |     switch (bonus->type) {
// |         case CAMPAIGN_CHOICE_RESOURCE:
// |             player->m_resources[IDX(bonus->resource)] += bonus->amount;
// |             break;
// |         case CAMPAIGN_CHOICE_ARTIFACT:
// |             if (player->m_heroCount > 0)
// |                 GiveArtifact(
// |                     gpGame->GetHero(player->m_heroIds[0]),
// |                     bonus->artifact,
// |                     false,
// |                     -1
// |                 );
// |             break;
// |         case CAMPAIGN_CHOICE_SPELL:
// |             if (player->m_heroCount > 0)
// |                 gpGame->GetHero(player->m_heroIds[0])
// |                     ->m_spells[IDX(bonus->spell)] = 1;
// |             break;
// |         case CAMPAIGN_CHOICE_SECONDARY_SKILL:
// |             if (player->m_heroCount > 0) {
// |                 for (heroSlot = 0; heroSlot < player->m_heroCount; ++heroSlot) {
// |                     pHero = gpGame->GetHero(player->m_heroIds[heroSlot]);
// |                     if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
// |                         && m_currentMap == MAP_VOY_BLOOD_IS_THICKER) {
// |                         if (pHero->m_portrait == HERO_GALLAVANT)
// |                             break;
// |                     } else {
// |                         if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
// |                             && m_currentMap == MAP_VOY_KING_AND_COUNTRY) {
// |                             if (pHero->m_portrait == HERO_CEALLACH)
// |                                 break;
// |                         } else {
// |                             break;
// |                         }
// |                     }
// |                 }
// |                 pHero->SetSS(
// |                     static_cast<HeroSecondarySkill>(bonus->value),
// |                     static_cast<HeroSkillLevel>(bonus->amount)
// |                 );
// |             }
// |             break;
// |         case CAMPAIGN_CHOICE_CREATURES:
// |             if (player->m_heroCount > 0)
// |                 gpGame->GetHero(player->m_heroIds[0])
// |                     ->m_army.Add(bonus->creature, bonus->amount, -1);
// |             break;
// |         case CAMPAIGN_CHOICE_PUZZLE_PIECES:
// |             player->m_cheatValue = static_cast<i8>(bonus->value);
// |             break;
// |         case CAMPAIGN_CHOICE_EXPERIENCE: {
// |             b32 savedNewGameSetup = gbInNewGameSetup;
// |             gbInNewGameSetup = true;
// |             if (player->m_heroCount > 0) {
// |                 gpGame->GetHero(player->m_heroIds[0])->m_experience += bonus->value;
// |                 gpGame->GetHero(player->m_heroIds[0])->CheckLevel();
// |             }
// |             gbInNewGameSetup = savedNewGameSetup;
// |             break;
// |         }
// |         case CAMPAIGN_CHOICE_NONE:
// |             break;
// |         case CAMPAIGN_CHOICE_PRIMARY_SKILL:
// |             if (player->m_heroCount > 0) {
// |                 for (heroSlot = 0; heroSlot < player->m_heroCount; ++heroSlot) {
// |                     pHero = gpGame->GetHero(player->m_heroIds[heroSlot]);
// |                     if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
// |                         && m_currentMap == MAP_VOY_KING_AND_COUNTRY) {
// |                         if (pHero->m_portrait == HERO_CEALLACH)
// |                             break;
// |                     } else {
// |                         break;
// |                     }
// |                 }
// |                 pHero->m_primaryStats[bonus->value] += bonus->amount;
// |             }
// |             break;
// |         case CAMPAIGN_CHOICE_SPELL_SCROLL:
// |             if (player->m_heroCount > 0)
// |                 GiveArtifact(
// |                     gpGame->GetHero(player->m_heroIds[0]),
// |                     ARTIFACT_SPELL_SCROLL,
// |                     false,
// |                     static_cast<i8>(bonus->spell)
// |                 );
// |             break;
// |     }
// |
// |     ExpansionCampaignAward whichAward;
// |     for (whichAward = AWARD_ELVEN_ALLIANCE; IDX(whichAward) < EXPANSION_CAMPAIGN_AWARD_COUNT;
// |          ++whichAward) {
// |         if (m_awards[IDX(whichAward)] != 0) {
// |             switch (whichAward) {
// |                 case AWARD_ELVEN_ALLIANCE:
// |                     break;
// |                 case AWARD_BREASTPLATE_ANDURAN:
// |                     if (player->m_heroCount > 0)
// |                         GiveArtifact(
// |                             gpGame->GetHero(player->m_heroIds[0]),
// |                             ARTIFACT_BREASTPLATE_ANDURAN,
// |                             false,
// |                             -1
// |                         );
// |                     break;
// |                 case AWARD_WOOD_BONUS:
// |                     break;
// |                 case AWARD_HELMET_ANDURAN:
// |                     if (player->m_heroCount > 0)
// |                         GiveArtifact(
// |                             gpGame->GetHero(player->m_heroIds[0]),
// |                             ARTIFACT_HELMET_ANDURAN,
// |                             false,
// |                             -1
// |                         );
// |                     break;
// |                 case AWARD_DEFEAT_KRAEGER:
// |                     for (heroSlot = 0; heroSlot < EXPANSION_CAMPAIGN_HERO_COUNT;
// |                          ++heroSlot) {
// |                         if (gpGame->m_heroRecs[heroSlot].m_portrait == HERO_DAINWIN)
// |                             gpGame->m_heroRecs[heroSlot].Deallocate(0);
// |                     }
// |                     break;
// |                 case AWARD_BATTLE_GARB:
// |                     if (player->m_heroCount > 0)
// |                         GiveArtifact(
// |                             gpGame->GetHero(player->m_heroIds[0]),
// |                             ARTIFACT_BATTLE_GARB,
// |                             false,
// |                             -1
// |                         );
// |                     break;
// |                 case AWARD_WAYWARD_SON:
// |                 case AWARD_UNCLE_IVAN:
// |                     break;
// |                 case AWARD_LEGENDARY_SCEPTER:
// |                     if (player->m_heroCount > 0)
// |                         GiveArtifact(
// |                             gpGame->GetHero(player->m_heroIds[0]),
// |                             ARTIFACT_LEGENDARY_SCEPTER,
// |                             false,
// |                             -1
// |                         );
// |                     break;
// |                 case AWARD_SET_GUARDIAN:
// |                     if (player->m_heroCount > 0)
// |                         gpGame->GetHero(player->m_heroIds[0])
// |                             ->m_spells[IDX(SPELL_SET_EARTH_GUARDIAN)] = 1;
// |                     break;
// |                 case AWARD_SPHERE_NEGATION:
// |                     if (player->m_heroCount > 0)
// |                         GiveArtifact(
// |                             gpGame->GetHero(player->m_heroIds[0]),
// |                             ARTIFACT_SPHERE_NEGATION,
// |                             false,
// |                             -1
// |                         );
// |                     break;
// |             }
// |         }
// |     }
// |     gbRetreatWin = true;
// | }
// |
// ALTERNATIVE split_global_records (unified delta from BEFORE):
// | --- before
// | +++ split_global_records
// | @@ -24,6 +24,7 @@
// |
// |      playerData* player = &gpGame->m_players[0];
// |      i32 heroSlot;
// | +    i32 heroRecord;
// |      hero* pHero;
// |      switch (bonus->type) {
// |          case CAMPAIGN_CHOICE_RESOURCE:
// | @@ -141,10 +142,10 @@
// |                          );
// |                      break;
// |                  case AWARD_DEFEAT_KRAEGER:
// | -                    for (heroSlot = 0; heroSlot < EXPANSION_CAMPAIGN_HERO_COUNT;
// | -                         ++heroSlot) {
// | -                        if (gpGame->m_heroRecs[heroSlot].m_portrait == HERO_DAINWIN)
// | -                            gpGame->m_heroRecs[heroSlot].Deallocate(0);
// | +                    for (heroRecord = 0; heroRecord < EXPANSION_CAMPAIGN_HERO_COUNT;
// | +                         ++heroRecord) {
// | +                        if (gpGame->m_heroRecs[heroRecord].m_portrait == HERO_DAINWIN)
// | +                            gpGame->m_heroRecs[heroRecord].Deallocate(0);
// |                      }
// |                      break;
// |                  case AWARD_BATTLE_GARB:
