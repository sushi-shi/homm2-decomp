#include <Ints.h>
#include <stdio.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <BASE/Utf8.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/state.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/KB.h>
#include <SOURCE/REQUEST.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>


typedef enum ExpansionCampaignSmacker {
    SMACKER_POL_INTRO             = 0x27,
    SMACKER_POL_UPRISING          = 0x28,
    SMACKER_POL_ISLAND_OF_CHAOS   = 0x29,
    SMACKER_POL_ARROWS_FLIGHT     = 0x2a,
    SMACKER_POL_BRANCH_REUNITED   = 0x2b,
    SMACKER_POL_AURORA_BOREALIS   = 0x2c,
    SMACKER_POL_BETRAYALS_END     = 0x2d,
    SMACKER_POL_CORRUPTIONS_HEART = 0x2e,
    SMACKER_DES_INTRO             = 0x2f,
    SMACKER_DES_CONQUER_AND_UNIFY = 0x30,
    SMACKER_DES_BORDER_TOWNS      = 0x31,
    SMACKER_DES_FAMILY_REUNITED   = 0x32,
    SMACKER_DES_SOUTHERN_WAR      = 0x33,
    SMACKER_DES_BRANCH_REUNITED   = 0x34,
    SMACKER_DES_EPIC_BATTLE       = 0x35,
    SMACKER_WIZ_INTRO             = 0x36,
    SMACKER_WIZ_SHROUDED_ISLES    = 0x37,
    SMACKER_WIZ_ETERNAL_SCROLLS   = 0x38,
    SMACKER_WIZ_POWERS_END        = 0x39,
    SMACKER_WIZ_FOUNT_OF_WIZARDRY = 0x3a,
    SMACKER_VOY_INTRO             = 0x3b,
    SMACKER_VOY_STRANDED          = 0x3c,
    SMACKER_VOY_PIRATE_ISLES      = 0x3d,
    SMACKER_VOY_KING_AND_COUNTRY  = 0x3e,
    SMACKER_VOY_BLOOD_IS_THICKER  = 0x3f,
    SMACKER_CAMPAIGN_CHOICE       = 0x43
} ExpansionCampaignSmacker;

typedef enum ExpansionCampaignChoiceConstant {
    CAMPAIGN_CHOICE_UNUSED_VALUE       = -1,
    CAMPAIGN_CHOICE_NO_AMOUNT          = -1,
    CAMPAIGN_CHOICE_ZERO_AMOUNT        = 0,
    CAMPAIGN_ARTIFACT_AMOUNT_ONE       = 1,
    CAMPAIGN_PRIMARY_BONUS_ONE         = 1,
    CAMPAIGN_PRIMARY_BONUS_TWO         = 2,
    CAMPAIGN_CRUSADER_COUNT            = 3,
    CAMPAIGN_MAGE_COUNT                = 5,
    CAMPAIGN_WOOD_FIVE                 = 5,
    CAMPAIGN_ORE_FIVE                  = 5,
    CAMPAIGN_WOOD_TEN                  = 10,
    CAMPAIGN_SULFUR_TEN                = 10,
    CAMPAIGN_CRYSTAL_TEN               = 10,
    CAMPAIGN_SULFUR_FIFTEEN            = 15,
    CAMPAIGN_WOOD_TWENTY               = 20,
    CAMPAIGN_WOOD_FIFTY                = 50,
    CAMPAIGN_GOLD_FIVE_HUNDRED         = 500,
    CAMPAIGN_GOLD_ONE_THOUSAND         = 1000,
    CAMPAIGN_GOLD_TWO_THOUSAND         = 2000,
    CAMPAIGN_GOLD_THREE_THOUSAND       = 3000,
    CAMPAIGN_GOLD_PENALTY_ONE_THOUSAND = -1000,
    CAMPAIGN_WOOD_PENALTY_TEN          = -10,
    CAMPAIGN_MERCURY_PENALTY_TWO       = -2
} ExpansionCampaignChoiceConstant;

typedef enum ExpansionCampaignImplementationConstant {
    TRACK_COORDINATE_COUNT         = 2,
    CAMPAIGN_ICON_X                = 24,
    CAMPAIGN_ICON_Y                = 25,
    CAMPAIGN_ICON_WIDTH            = 376,
    CAMPAIGN_ICON_HEIGHT           = 49,
    TRACK_FRAME_PLAYED             = 0,
    TRACK_FRAME_AVAILABLE          = 1,
    TRACK_FRAME_LOCKED             = 2,
    TRACK_FRAME_CAMPAIGN_STRIDE    = 3,
    TRACK_SELECTED_CAMPAIGN_OFFSET = 1
} ExpansionCampaignImplementationConstant;

i32 expansionCampaignTrackXY[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)][EXPANSION_CAMPAIGN_MAX_MAP_COUNT]
                            [TRACK_COORDINATE_COUNT] = {
        {{113, 310},
         {187, 310},
         {261, 352},
         {261, 310},
         {335, 352},
         {370, 310},
         {445, 352},
         {479, 310}},
        {{111, 331},
         {185, 331},
         {259, 289},
         {259, 373},
         {333, 331},
         {407, 373},
         {407, 289},
         {481, 331}},
        {{186, 310}, {260, 310}, {334, 352}, {408, 310}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{222, 312}, {296, 312}, {370, 312}, {370, 354}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}
};

i32 expansionCampaignMapCounts[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)] = {8, 8, 4, 4};

SCampaignChoice xCampaignChoices[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)][EXPANSION_CAMPAIGN_MAX_MAP_COUNT][EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT] = {
    {{{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MEDAL_OF_VALOR), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_STEALTH_SHIELD), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MINOR_SCROLL), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_WHITE_PEARL), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_BALLISTA), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_TWO_THOUSAND}},
     {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MAGE_RING), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MAJOR_SCROLL), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LOGISTICS), H2EnumIndex(HERO_SKILL_LEVEL_ADVANCED)}},
     {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MEDAL_OF_HONOR), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT,
       H2EnumIndex(ARTIFACT_CASTER_BRACELET),
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_DEFENDER_HELM), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_TWO_THOUSAND},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_POWER_AXE), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_PRIMARY_SKILL,
       H2EnumIndex(HERO_PRIMARY_DEFENSE),
       CAMPAIGN_PRIMARY_BONUS_TWO}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_TWO_THOUSAND},
      {CAMPAIGN_CHOICE_CREATURES, H2EnumIndex(CREATURE_CRUSADER), CAMPAIGN_CRUSADER_COUNT},
      {CAMPAIGN_CHOICE_ARTIFACT,
       H2EnumIndex(ARTIFACT_TRAVELER_BOOTS),
       CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_SULFUR), CAMPAIGN_SULFUR_TEN},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_POWER_AXE), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_SPELL_SCROLL, H2EnumIndex(SPELL_ANIMATE_DEAD), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_VIEW_HEROES), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_CREATURES, H2EnumIndex(CREATURE_MAGE), CAMPAIGN_MAGE_COUNT},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_ESTATES), H2EnumIndex(HERO_SKILL_LEVEL_ADVANCED)}}},
    {{{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_ONE_THOUSAND},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_WOOD_FIFTY},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_CRYSTAL), CAMPAIGN_CRYSTAL_TEN}},
     {{CAMPAIGN_CHOICE_PRIMARY_SKILL,
       H2EnumIndex(HERO_PRIMARY_ATTACK),
       CAMPAIGN_PRIMARY_BONUS_ONE},
      {CAMPAIGN_CHOICE_PRIMARY_SKILL,
       H2EnumIndex(HERO_PRIMARY_DEFENSE),
       CAMPAIGN_PRIMARY_BONUS_ONE},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_ONE_THOUSAND}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LOGISTICS), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_STEEL_SKIN), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MAGE_RING), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LOGISTICS), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_SCOUTING), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_POWER_AXE), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LUCK), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LEADERSHIP), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_ARTIFACT,
       H2EnumIndex(ARTIFACT_SPIKED_SHIELD),
       CAMPAIGN_ARTIFACT_AMOUNT_ONE}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_PENALTY_ONE_THOUSAND},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_WOOD_PENALTY_TEN},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_MERCURY), CAMPAIGN_MERCURY_PENALTY_TWO}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_ONE_THOUSAND},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_SCOUTING), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_WOOD_TWENTY}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LEADERSHIP), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_ARCHERY), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_PRIMARY_SKILL,
       H2EnumIndex(HERO_PRIMARY_SPELL_POWER),
       CAMPAIGN_PRIMARY_BONUS_ONE}}},
    {{{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_TWO_THOUSAND},
      {CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_POWER_AXE), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT,
       H2EnumIndex(ARTIFACT_STEALTH_SHIELD),
       CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_MAGE_RING), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_ARTIFACT,
       H2EnumIndex(ARTIFACT_FOREMOST_SCROLL),
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_FIREBLAST), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_MASS_HASTE), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_SPELL,
       H2EnumIndex(SPELL_SUMMON_EARTH_ELEMENTAL),
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_SPELL, H2EnumIndex(SPELL_CHAIN_LIGHTNING), CAMPAIGN_CHOICE_NO_AMOUNT}},
     {{CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_WOOD_FIVE},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_ORE), CAMPAIGN_ORE_FIVE},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_ONE_THOUSAND}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}}},
    {{{CAMPAIGN_CHOICE_ARTIFACT, H2EnumIndex(ARTIFACT_NOMAD_BOOTS), CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_FIVE_HUNDRED},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_SCOUTING), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_PATHFINDING), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_NAVIGATION), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_SULFUR), CAMPAIGN_SULFUR_FIFTEEN}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_ARCHERY), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_THREE_THOUSAND},
      {CAMPAIGN_CHOICE_PRIMARY_SKILL,
       H2EnumIndex(HERO_PRIMARY_ATTACK),
       CAMPAIGN_PRIMARY_BONUS_ONE}},
     {{CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_NECROMANCY), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_SECONDARY_SKILL, H2EnumIndex(HERO_SKILL_LOGISTICS), H2EnumIndex(HERO_SKILL_LEVEL_BASIC)},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_GOLD), CAMPAIGN_GOLD_TWO_THOUSAND}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}},
     {{CAMPAIGN_CHOICE_INVALID,
       CAMPAIGN_CHOICE_UNUSED_VALUE,
       CAMPAIGN_CHOICE_NO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT},
      {CAMPAIGN_CHOICE_RESOURCE, H2EnumIndex(RES_WOOD), CAMPAIGN_CHOICE_ZERO_AMOUNT}}}
};

H2EnumStorage<GameDifficulty, i8>
    expansionCampaignDifficulty[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)]
                               [EXPANSION_CAMPAIGN_MAX_MAP_COUNT] = {
                                                          {0, 1, 1, 1, 1, 2, 2, 3},
                                                          {0, 1, 1, 1, 2, 2, 2, 3},
                                                          {1, 1, 1, 2, -1, -1, -1, -1},
                                                          {0, 1, 2, 2, -1, -1, -1, -1}
};

ExpCampaign::ExpCampaign(void) {
    m_window = NULL;
}

ExpCampaign::~ExpCampaign() {}

void ExpCampaign::ResetMapChoices(void) {
    memset(m_mapChoices, 0, sizeof(m_mapChoices));
}

void ExpCampaign::ResetMapsPlayed(void) {
    memset(m_mapsPlayed, 0, sizeof(m_mapsPlayed));
}

void ExpCampaign::ResetAwards(void) {
    memset(m_awards, 0, sizeof(m_awards));
}

void ExpCampaign::ResetBonusChoices(void) {
    memset(m_bonusChoices, 0, sizeof(m_bonusChoices));
}

void ExpCampaign::GrantAward(ExpansionCampaignAward award) {
    m_awards[H2EnumIndex(award)] = 1;
}

void ExpCampaign::RemoveAward(ExpansionCampaignAward award) {
    m_awards[H2EnumIndex(award)] = 0;
}

i8 ExpCampaign::HasAward(ExpansionCampaignAward award) {
    return m_awards[H2EnumIndex(award)];
}

void ExpCampaign::SetMapWasPlayed(void) {
    m_mapsPlayed[H2EnumIndex(m_currentMap)] = 1;
}

void ExpCampaign::InitNewCampaign(ExpansionCampaignId campaignId) {
    m_campaignId = campaignId;
    m_currentMap = MAP_NONE;
    m_mapCount = ironfist::CampaignMapCounts[H2EnumIndex(campaignId)];
    ResetMapChoices();
    ResetMapsPlayed();
    ResetAwards();
    ResetBonusChoices();
    ironfist::state::Get().campaign.savedHeroData.clear();
}

void ExpCampaign::InitMap(void) {
    SCampaignChoice* bonus =
        ironfist::CampaignChoice(m_campaignId, H2EnumIndex(m_currentMap), m_bonusChoices[H2EnumIndex(m_currentMap)]);

    memset(gpGame->m_setupPlayerColor, 0, EXPANSION_CAMPAIGN_PLAYER_SETUP_RESET_SIZE);
    sprintf(
        gpGame->m_mapFilename,
        "CAMP%d_%02d.HXC",
        H2EnumIndex(m_campaignId) + 1,
        H2EnumIndex(m_currentMap) + 1
    );
    gpGame->m_newGameInitialized = false;
    if (m_currentMap == MAP_FIRST)
        m_mapDays[0] = 0;
    strcpy(gMapName, gpGame->m_mapFilename);
    GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
    gpGame->LoadGame("origdata.bin", 1, 0);
    gpGame->InitNewGame(NULL);
    gpGame->m_difficulty = static_cast<GameDifficulty>(
        ironfist::CampaignDifficulties[H2EnumIndex(m_campaignId)][H2EnumIndex(m_currentMap)]
    );
    gpGame->m_playerCount = gpGame->m_mapHeader.playerCount;
    gpGame->NewMap(gMapName);

    playerData* player = &gpGame->m_players[0];
    i32 heroSlot;
    hero* pHero;
    switch (bonus->type) {
        case CAMPAIGN_CHOICE_RESOURCE:
            player->m_resources[H2EnumIndex(bonus->resource)] += bonus->amount;
            break;
        case CAMPAIGN_CHOICE_ARTIFACT:
            if (player->m_heroCount > 0)
                GiveArtifact(
                    gpGame->GetHero(player->m_heroIds[0]),
                    bonus->artifact,
                    false,
                    -1
                );
            break;
        case CAMPAIGN_CHOICE_SPELL:
            if (player->m_heroCount > 0)
                gpGame->GetHero(player->m_heroIds[0])
                    ->m_spells[H2EnumIndex(bonus->spell)] = 1;
            break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            if (player->m_heroCount > 0) {
                for (heroSlot = 0; heroSlot < player->m_heroCount; ++heroSlot) {
                    pHero = gpGame->GetHero(player->m_heroIds[heroSlot]);
                    if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
                        && m_currentMap == MAP_VOY_BLOOD_IS_THICKER) {
                        if (pHero->m_portrait == HERO_GALLAVANT)
                            break;
                    } else {
                        if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
                            && m_currentMap == MAP_VOY_KING_AND_COUNTRY) {
                            if (pHero->m_portrait == HERO_CEALLACH)
                                break;
                        } else {
                            break;
                        }
                    }
                }
                pHero->SetSS(
                    static_cast<HeroSecondarySkill>(bonus->value),
                    static_cast<HeroSkillLevel>(bonus->amount)
                );
            }
            break;
        case CAMPAIGN_CHOICE_CREATURES:
            if (player->m_heroCount > 0)
                gpGame->GetHero(player->m_heroIds[0])
                    ->m_army.Add(bonus->creature, bonus->amount, -1);
            break;
        case CAMPAIGN_CHOICE_PUZZLE_PIECES:
            player->m_cheatValue = static_cast<i8>(bonus->value);
            break;
        case CAMPAIGN_CHOICE_EXPERIENCE: {
            b32 savedNewGameSetup = gbInNewGameSetup;
            gbInNewGameSetup = true;
            if (player->m_heroCount > 0) {
                gpGame->GetHero(player->m_heroIds[0])->m_experience += bonus->value;
                gpGame->GetHero(player->m_heroIds[0])->CheckLevel();
            }
            gbInNewGameSetup = savedNewGameSetup;
            break;
        }
        case CAMPAIGN_CHOICE_NONE:
            break;
        case CAMPAIGN_CHOICE_PRIMARY_SKILL:
            if (player->m_heroCount > 0) {
                for (heroSlot = 0; heroSlot < player->m_heroCount; ++heroSlot) {
                    pHero = gpGame->GetHero(player->m_heroIds[heroSlot]);
                    if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
                        && m_currentMap == MAP_VOY_KING_AND_COUNTRY) {
                        if (pHero->m_portrait == HERO_CEALLACH)
                            break;
                    } else {
                        break;
                    }
                }
                pHero->m_primaryStats[bonus->value] += bonus->amount;
            }
            break;
        case CAMPAIGN_CHOICE_SPELL_SCROLL:
            if (player->m_heroCount > 0)
                GiveArtifact(
                    gpGame->GetHero(player->m_heroIds[0]),
                    ARTIFACT_SPELL_SCROLL,
                    false,
                    static_cast<i8>(bonus->spell)
                );
            break;
    }

    ExpansionCampaignAward whichAward;
    for (whichAward = AWARD_ELVEN_ALLIANCE; H2EnumIndex(whichAward) < EXPANSION_CAMPAIGN_AWARD_COUNT;
         ++whichAward) {
        if (m_awards[H2EnumIndex(whichAward)] != 0) {
            switch (whichAward) {
                case AWARD_ELVEN_ALLIANCE:
                    break;
                case AWARD_BREASTPLATE_ANDURAN:
                    if (player->m_heroCount > 0)
                        GiveArtifact(
                            gpGame->GetHero(player->m_heroIds[0]),
                            ARTIFACT_BREASTPLATE_ANDURAN,
                            false,
                            -1
                        );
                    break;
                case AWARD_WOOD_BONUS:
                    break;
                case AWARD_HELMET_ANDURAN:
                    if (player->m_heroCount > 0)
                        GiveArtifact(
                            gpGame->GetHero(player->m_heroIds[0]),
                            ARTIFACT_HELMET_ANDURAN,
                            false,
                            -1
                        );
                    break;
                case AWARD_DEFEAT_KRAEGER:
                    for (heroSlot = 0; heroSlot < EXPANSION_CAMPAIGN_HERO_COUNT;
                         ++heroSlot) {
                        if (gpGame->m_heroRecs[heroSlot].m_portrait == HERO_DAINWIN)
                            gpGame->m_heroRecs[heroSlot].Deallocate(0);
                    }
                    break;
                case AWARD_BATTLE_GARB:
                    if (player->m_heroCount > 0)
                        GiveArtifact(
                            gpGame->GetHero(player->m_heroIds[0]),
                            ARTIFACT_BATTLE_GARB,
                            false,
                            -1
                        );
                    break;
                case AWARD_WAYWARD_SON:
                case AWARD_UNCLE_IVAN:
                    break;
                case AWARD_LEGENDARY_SCEPTER:
                    if (player->m_heroCount > 0)
                        GiveArtifact(
                            gpGame->GetHero(player->m_heroIds[0]),
                            ARTIFACT_LEGENDARY_SCEPTER,
                            false,
                            -1
                        );
                    break;
                case AWARD_SET_GUARDIAN:
                    if (player->m_heroCount > 0)
                        gpGame->GetHero(player->m_heroIds[0])
                            ->m_spells[H2EnumIndex(SPELL_SET_EARTH_GUARDIAN)] = 1;
                    break;
                case AWARD_SPHERE_NEGATION:
                    if (player->m_heroCount > 0)
                        GiveArtifact(
                            gpGame->GetHero(player->m_heroIds[0]),
                            ARTIFACT_SPHERE_NEGATION,
                            false,
                            -1
                        );
                    break;
            }
        }
    }

    // Custom campaigns can carry heroes between maps.
    i32 saveIdx = 0;
    for (auto& carried : ironfist::HeroesToLoad[H2EnumIndex(m_campaignId)][H2EnumIndex(m_currentMap)]) {
        ironfist::LoadCampaignSavedHero(carried.first, carried.second, saveIdx);
        saveIdx++;
    }
    gbRetreatWin = true;
}

void ExpCampaign::ShowInfo(i32 viewOnly, i32) {
    m_viewOnly = viewOnly;
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->ReallyShowPointer();
    b32 savedTheme = gbUseEvilInterface;
    gbUseEvilInterface = true;
    m_viewMap = m_currentMap;
    m_window = new heroWindow(0, 0, "x_camp.bin");
    if (m_window == NULL)
        MemError();

    widget* trackWidget = NULL;
    i32 mapIndex;
    for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
        trackWidget = new iconWidget(
            ironfist::CampaignTrack[H2EnumIndex(m_campaignId)][mapIndex].x,
            ironfist::CampaignTrack[H2EnumIndex(m_campaignId)][mapIndex].y,
            EXPANSION_CAMPAIGN_TRACK_ICON_SIZE,
            EXPANSION_CAMPAIGN_TRACK_ICON_SIZE,
            "x_cmpext.icn",
            0,
            ICON_DRAW_NORMAL,
            mapIndex + CAMPAIGN_TRACK_WIDGET_FIRST,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        if (trackWidget == NULL)
            MemError();
        m_window->AddWidget(trackWidget, -1);
    }

    widget* campIcon = NULL;
    // Custom campaigns take their header art from Ironfist's x_cmphdr.icn,
    // which indexes straight by campaign ID; the retail four keep the
    // frames retail shipped so they never depend on ironfist.agg.
    if (ironfist::IsCustomCampaign(m_campaignId)) {
        campIcon = new iconWidget(
            CAMPAIGN_ICON_X,
            CAMPAIGN_ICON_Y,
            CAMPAIGN_ICON_WIDTH,
            CAMPAIGN_ICON_HEIGHT,
            "x_cmphdr.icn",
            H2EnumIndex(m_campaignId),
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
    } else {
        campIcon = new iconWidget(
            CAMPAIGN_ICON_X,
            CAMPAIGN_ICON_Y,
            CAMPAIGN_ICON_WIDTH,
            CAMPAIGN_ICON_HEIGHT,
            "x_cmpext.icn",
            H2EnumIndex(m_campaignId) + EXPANSION_CAMPAIGN_ICON_FRAME_BASE,
            ICON_DRAW_NORMAL,
            -1,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
    }
    if (campIcon == NULL)
        MemError();
    m_window->AddWidget(campIcon, -1);

    tag_message message;
    message.type = MESSAGE_WIDGET;
    if (viewOnly == 0) {
        message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
        message.payload.widget.data.value = H2EnumIndex(WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW);
        m_window->BroadcastMessage(message);
    }
    gpSoundManager->SwitchAmbientMusic(EXPANSION_CAMPAIGN_MUSIC);
    UpdateInfo(0);
    gpWindowManager->DoDialog(m_window, MessageHandler, 0);
    delete m_window;
    gbUseEvilInterface = savedTheme;

    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
        NormalDialog(

            localization::Tr("campaign.confirm.restart_scenario"),
            CAMPAIGN_RESTART_CONFIRM,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0,
            CAMPAIGN_DIALOG_NO_RESOURCE,
            0
        );
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
            InitMap();
            gpAdvManager->m_visibilityMapValid = false;
            giBottomViewOverride = BOTTOM_VIEW_NONE;
            gpWindowManager->FadeScreen(FADE_OUT, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
            gpAdvManager->SetInitialMapOrigin();
            gpAdvManager->RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(FADE_IN, CAMPAIGN_DIALOG_FADE_STEPS, gPalette);
        }
    }
}

void ExpCampaign::UpdateInfo(i32 redraw) {
    SCampaignChoice* choice;
    tag_message message;
    char armyName[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];
    b8 hasVisibleAward;
    i32 i;
    b8 showScroll;

    message.type = MESSAGE_WIDGET;
    for (i = 0; i < m_mapCount; ++i) {
        if (m_mapChoices[i] != 0)
            message.payload.widget.data.value = TRACK_FRAME_AVAILABLE;
        else if (m_mapsPlayed[i] != 0)
            message.payload.widget.data.value = TRACK_FRAME_PLAYED;
        else
            message.payload.widget.data.value = TRACK_FRAME_LOCKED;
        if (i == H2EnumIndex(m_viewMap))
            message.payload.widget.data.value +=
                (H2EnumIndex(m_campaignId) + TRACK_SELECTED_CAMPAIGN_OFFSET)
                * TRACK_FRAME_CAMPAIGN_STRIDE;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = i + CAMPAIGN_TRACK_WIDGET_FIRST;
        m_window->BroadcastMessage(message);
    }

    message.payload.widget.command = CAMPAIGN_MESSAGE_SET_ICON;
    message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
    message.payload.widget.data.text = gText;
    sprintf(gText, "x_track%d.icn", H2EnumIndex(m_campaignId) + 1);
    m_window->BroadcastMessage(message);

    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
    sprintf(gText, "%d", H2EnumIndex(m_viewMap) + 1);
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
    sprintf(gText, "%s", ironfist::ScenarioNames[H2EnumIndex(m_campaignId)][H2EnumIndex(m_viewMap)].c_str());
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
    sprintf(
        gText,
        "%s",
        ironfist::ScenarioDescriptions[H2EnumIndex(m_campaignId)][H2EnumIndex(m_viewMap)].c_str()
    );
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_BONUS_WIDGET;
    sprintf(gText, "%d", m_mapDays[H2EnumIndex(m_viewMap)]);
    m_window->BroadcastMessage(message);

    hasVisibleAward = false;
    message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
    strcpy(gText, "");
    for (i = 0; i < EXPANSION_CAMPAIGN_AWARD_COUNT; ++i) {
        if (m_awards[i] != 0) {
            hasVisibleAward = true;
            strcat(gText, xCampaignAwards[i]);
            strcat(gText, "\n");
        }
    }
    if (hasVisibleAward == 0)
        utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("common.none")  );
    m_window->BroadcastMessage(message);

    for (i = 0; i < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++i) {
        choice = ironfist::CampaignChoice(m_campaignId, H2EnumIndex(m_viewMap), i);
        switch (choice->type) {
            case CAMPAIGN_CHOICE_RESOURCE:
                sprintf(
                    gText,
                    localization::Tr("campaign.bonus.resource"),
                    choice->amount,
                    gResourceNames[H2EnumIndex(choice->resource)]
                );
                break;
            case CAMPAIGN_CHOICE_ARTIFACT:
                switch (choice->artifact) {
                    case ARTIFACT_MINOR_SCROLL:
                        strcpy(
                            gText,
                            localization::Tr("campaign.bonus.artifact.minor_scroll")
                        );
                        break;
                    case ARTIFACT_MAGE_RING:
                        strcpy(
                            gText,
                            localization::Tr("campaign.bonus.artifact.mage_ring")
                        );
                        break;
                    case ARTIFACT_DEFENDER_HELM:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.defender_helm"));
                        break;
                    case ARTIFACT_POWER_AXE:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.power_axe")  );
                        break;
                    case ARTIFACT_DRAGON_SWORD:
                        strcpy(
                            gText,
                            localization::Tr("campaign.bonus.artifact.dragon_sword")
                        );
                        break;
                    case ARTIFACT_DIVINE_BREASTPLATE:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.breastplate")  );
                        break;
                    case ARTIFACT_FIZBIN_OF_MISFORTUNE:
                        strcpy(
                            gText,

                            localization::Tr("campaign.bonus.artifact.fizbin_medal")
                        );
                        break;
                    case ARTIFACT_THUNDER_MACE:
                        strcpy(
                            gText,

                            localization::Tr("campaign.bonus.artifact.thunder_mace")
                        );
                        break;
                    case ARTIFACT_ARMORED_GAUNTLETS:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.gauntlets")  );
                        break;
                    case ARTIFACT_MAJOR_SCROLL:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.major_scroll"));
                        break;
                    case ARTIFACT_FOREMOST_SCROLL:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.foremost_scroll"));
                        break;
                    case ARTIFACT_BALLISTA:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.ballista")  );
                        break;
                    case ARTIFACT_STEALTH_SHIELD:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.stealth_shield"));
                        break;
                    case ARTIFACT_NOMAD_BOOTS:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.nomad_boots"));
                        break;
                    case ARTIFACT_TRAVELER_BOOTS:
                        strcpy(gText, localization::Tr("campaign.bonus.artifact.traveler_boots"));
                        break;
                    case ARTIFACT_HIDEOUS_MASK:
                    case ARTIFACT_BLACK_PEARL:
                    default:
                        sprintf(gText, "%s", gArtifactNames[H2EnumIndex(choice->artifact)]);
                        break;
                }
                break;
            case CAMPAIGN_CHOICE_SPELL:
                if (choice->spell == SPELL_SUMMON_EARTH_ELEMENTAL)
                    utf8::Copy(
                        gText, GLOBAL_TEXT_BUFFER_SIZE,
                        localization::Tr("campaign.bonus.spell.summon_earth")
                    );
                else
                    sprintf(gText, "%s", gSpellNames[H2EnumIndex(choice->spell)]);
                break;
            case CAMPAIGN_CHOICE_SECONDARY_SKILL:
                if ((choice->amount == EXPANSION_CAMPAIGN_SPECIAL_SKILL_LEVEL
                     && choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL)
                    || (choice->amount == EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT_LEVEL
                        && choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT)) {
                    sprintf(
                        gText,
                        localization::Tr("campaign.bonus.secondary_skill"),
                        xShortSSLevelNames[choice->amount - 1],
                        gSecondarySkills[choice->value]
                    );
                } else {
                    sprintf(
                        gText,
                        localization::Tr("campaign.bonus.secondary_skill"),
                        gSecondarySkillLevels[choice->amount - 1],
                        gSecondarySkills[choice->value]
                    );
                }
                break;
            case CAMPAIGN_CHOICE_CREATURES:
                utf8::Copy(
                    armyName,
                    sizeof(armyName),
                    gArmyNamesPlural[H2EnumIndex(choice->creature)]
                );
                utf8::UppercaseFirst(armyName);
                sprintf(
                    gText,
                    localization::Tr("campaign.bonus.creatures"),
                    choice->amount,
                    armyName
                );
                break;
            case CAMPAIGN_CHOICE_PUZZLE_PIECES:
                sprintf(
                    gText,
                    localization::Tr("campaign.bonus.puzzle_pieces"),
                    choice->value
                );
                break;
            case CAMPAIGN_CHOICE_EXPERIENCE:
                sprintf(
                    gText,
                    localization::Tr("campaign.bonus.experience"),
                    choice->value
                );
                break;
            case CAMPAIGN_CHOICE_NONE:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, localization::Tr("common.not_applicable")  );
                break;
            case CAMPAIGN_CHOICE_ALIGNMENT:
                utf8::Copy(gText, GLOBAL_TEXT_BUFFER_SIZE, gAlignmentNames[H2EnumIndex(choice->faction)]);
                break;
            case CAMPAIGN_CHOICE_PRIMARY_SKILL:
                sprintf(gText, "%s +%d", gStatNames[choice->value], choice->amount);
                break;
            case CAMPAIGN_CHOICE_SPELL_SCROLL: {
                showScroll = true;
                switch (choice->spell) {
                    case SPELL_DISRUPTING_RAY:
                    case SPELL_ANIMATE_DEAD:
                        showScroll = false;
                        break;
                }
                if (showScroll != 0) {
                    sprintf(
                        gText,
                        localization::Tr("campaign.bonus.spell_scroll"),
                        gSpellNames[H2EnumIndex(choice->spell)]
                    );
                } else {
                    sprintf(gText, "%s", gSpellNames[H2EnumIndex(choice->spell)]);
                }
                break;
            }
        }
        message.payload.widget.id = i + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
        m_window->BroadcastMessage(message);
    }

    for (i = 0; i < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++i) {
        message.payload.widget.id = i + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        if (m_viewOnly == 0 && m_mapChoices[H2EnumIndex(m_viewMap)] != 0)
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        m_window->BroadcastMessage(message);

        if (m_bonusChoices[H2EnumIndex(m_viewMap)] == i)
            message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
        else
            message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
        m_window->BroadcastMessage(message);
    }
    if (redraw != 0)
        m_window->DrawWindow();
}

i32 ExpCampaign::HandleVictory(void) {
    i32 days = 0;
    i32 map;

    if (m_currentMap > MAP_NONE) {
        days = Days();
        m_mapsPlayed[H2EnumIndex(m_currentMap)] = 1;
    }
    memset(m_mapChoices, 0, m_mapCount);
    switch (m_campaignId) {
        case EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY:
            HandleVictory1();
            break;
        case EXPANSION_CAMPAIGN_DESCENDANTS:
            HandleVictory2();
            break;
        case EXPANSION_CAMPAIGN_WIZARDS_ISLE:
            HandleVictory3();
            break;
        case EXPANSION_CAMPAIGN_VOYAGE_HOME:
            HandleVictory4();
            break;
        default:
            HandleVictoryCustomCampaign();
            break;
    }
    if (IsCompleted())
        return 0;

    m_currentMap = MAP_NONE;
    for (map = 0; map < m_mapCount; ++map) {
        if (m_mapChoices[map]) {
            m_mapDays[map] = days;
            if (m_currentMap == MAP_NONE)
                m_currentMap = static_cast<ExpansionCampaignMap>(map);
        }
    }
    ShowInfo(0, 0);
    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_ACCEPT)
        return 1;
    return 0;
}

void ExpCampaign::HandleVictory1(void) {
    switch (m_currentMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_NONE) + 1):
            PlaySmacker(SMACKER_POL_INTRO);
            m_mapChoices[H2EnumIndex(MAP_POL_UPRISING)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_UPRISING) + 1):
            PlaySmacker(SMACKER_POL_UPRISING);
            m_mapChoices[H2EnumIndex(MAP_POL_ISLAND_OF_CHAOS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ISLAND_OF_CHAOS) + 1):
            PlaySmacker(SMACKER_POL_ISLAND_OF_CHAOS);
            m_mapChoices[H2EnumIndex(MAP_POL_ARROWS_FLIGHT)] = 1;
            m_mapChoices[H2EnumIndex(MAP_POL_ABYSS)] = 1;
            m_awards[H2EnumIndex(AWARD_BREASTPLATE_ANDURAN)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ARROWS_FLIGHT) + 1):
            PlaySmacker(SMACKER_POL_ARROWS_FLIGHT);
            m_mapChoices[H2EnumIndex(MAP_POL_GIANTS_PASS)] = 1;
            m_awards[H2EnumIndex(AWARD_ELVEN_ALLIANCE)] = 1;
            m_awards[H2EnumIndex(AWARD_WOOD_BONUS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ABYSS) + 1):
            PlaySmacker(SMACKER_POL_BRANCH_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_POL_AURORA_BOREALIS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_GIANTS_PASS) + 1):
            PlaySmacker(SMACKER_POL_BRANCH_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_POL_AURORA_BOREALIS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_AURORA_BOREALIS) + 1):
            PlaySmacker(SMACKER_POL_AURORA_BOREALIS);
            m_mapChoices[H2EnumIndex(MAP_POL_BETRAYALS_END)] = 1;
            m_mapChoices[H2EnumIndex(MAP_POL_CORRUPTIONS_HEART)] = 1;
            m_awards[H2EnumIndex(AWARD_HELMET_ANDURAN)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_BETRAYALS_END) + 1):
            PlaySmacker(SMACKER_POL_BETRAYALS_END);
            m_mapChoices[H2EnumIndex(MAP_POL_CORRUPTIONS_HEART)] = 1;
            m_awards[H2EnumIndex(AWARD_BATTLE_GARB)] = 1;
            m_awards[H2EnumIndex(AWARD_BREASTPLATE_ANDURAN)] = 0;
            m_awards[H2EnumIndex(AWARD_HELMET_ANDURAN)] = 0;
            m_awards[H2EnumIndex(AWARD_DEFEAT_KRAEGER)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_CORRUPTIONS_HEART) + 1):
            PlaySmacker(SMACKER_POL_CORRUPTIONS_HEART);
            break;
    }
}

void ExpCampaign::HandleVictory2(void) {
    switch (m_currentMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_NONE) + 1):
            PlaySmacker(SMACKER_DES_INTRO);
            m_mapChoices[H2EnumIndex(MAP_DES_CONQUER_AND_UNIFY)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_CONQUER_AND_UNIFY) + 1):
            PlaySmacker(SMACKER_DES_CONQUER_AND_UNIFY);
            m_mapChoices[H2EnumIndex(MAP_DES_BORDER_TOWNS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_BORDER_TOWNS) + 1):
            PlaySmacker(SMACKER_DES_BORDER_TOWNS);
            m_mapChoices[H2EnumIndex(MAP_DES_WAYWARD_SON)] = 1;
            m_mapChoices[H2EnumIndex(MAP_DES_UNCLE_IVAN)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_WAYWARD_SON) + 1):
            PlaySmacker(SMACKER_DES_FAMILY_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_DES_SOUTHERN_WAR)] = 1;
            m_awards[H2EnumIndex(AWARD_WAYWARD_SON)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_UNCLE_IVAN) + 1):
            PlaySmacker(SMACKER_DES_FAMILY_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_DES_SOUTHERN_WAR)] = 1;
            m_awards[H2EnumIndex(AWARD_UNCLE_IVAN)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_SOUTHERN_WAR) + 1):
            PlaySmacker(SMACKER_DES_SOUTHERN_WAR);
            m_mapChoices[H2EnumIndex(MAP_DES_IVORY_GATES)] = 1;
            m_mapChoices[H2EnumIndex(MAP_DES_ELVEN_LANDS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_IVORY_GATES) + 1):
            PlaySmacker(SMACKER_DES_BRANCH_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_DES_EPIC_BATTLE)] = 1;
            m_awards[H2EnumIndex(AWARD_LEGENDARY_SCEPTER)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_ELVEN_LANDS) + 1):
            PlaySmacker(SMACKER_DES_BRANCH_REUNITED);
            m_mapChoices[H2EnumIndex(MAP_DES_EPIC_BATTLE)] = 1;
            m_awards[H2EnumIndex(AWARD_ELVEN_ALLIANCE)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_EPIC_BATTLE) + 1):
            PlaySmacker(SMACKER_DES_EPIC_BATTLE);
            break;
    }
}

void ExpCampaign::HandleVictory3(void) {
    switch (m_currentMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_NONE) + 1):
            PlaySmacker(SMACKER_WIZ_INTRO);
            m_mapChoices[H2EnumIndex(MAP_WIZ_SHROUDED_ISLES)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_SHROUDED_ISLES) + 1):
            PlaySmacker(SMACKER_WIZ_SHROUDED_ISLES);
            m_mapChoices[H2EnumIndex(MAP_WIZ_ETERNAL_SCROLLS)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_ETERNAL_SCROLLS) + 1):
            PlaySmacker(SMACKER_WIZ_ETERNAL_SCROLLS);
            m_mapChoices[H2EnumIndex(MAP_WIZ_POWERS_END)] = 1;
            m_mapChoices[H2EnumIndex(MAP_WIZ_FOUNT_OF_WIZARDRY)] = 1;
            m_awards[H2EnumIndex(AWARD_SET_GUARDIAN)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_POWERS_END) + 1):
            PlaySmacker(SMACKER_WIZ_POWERS_END);
            m_mapChoices[H2EnumIndex(MAP_WIZ_FOUNT_OF_WIZARDRY)] = 1;
            m_awards[H2EnumIndex(AWARD_SPHERE_NEGATION)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_FOUNT_OF_WIZARDRY) + 1):
            PlaySmacker(SMACKER_WIZ_FOUNT_OF_WIZARDRY);
            break;
    }
}

void ExpCampaign::HandleVictory4(void) {
    switch (m_currentMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_NONE) + 1):
            PlaySmacker(SMACKER_VOY_INTRO);
            m_mapChoices[H2EnumIndex(MAP_VOY_STRANDED)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_STRANDED) + 1):
            PlaySmacker(SMACKER_VOY_STRANDED);
            m_mapChoices[H2EnumIndex(MAP_VOY_PIRATE_ISLES)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_PIRATE_ISLES) + 1):
            PlaySmacker(SMACKER_VOY_PIRATE_ISLES);
            m_mapChoices[H2EnumIndex(MAP_VOY_KING_AND_COUNTRY)] = 1;
            m_mapChoices[H2EnumIndex(MAP_VOY_BLOOD_IS_THICKER)] = 1;
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_KING_AND_COUNTRY) + 1):
            PlaySmacker(SMACKER_VOY_KING_AND_COUNTRY);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_BLOOD_IS_THICKER) + 1):
            PlaySmacker(SMACKER_VOY_BLOOD_IS_THICKER);
            break;
    }
}

void ExpCampaign::ReplaySmacker(void) {
    switch (m_campaignId) {
        case EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY:
            ReplaySmacker1();
            break;
        case EXPANSION_CAMPAIGN_DESCENDANTS:
            ReplaySmacker2();
            break;
        case EXPANSION_CAMPAIGN_WIZARDS_ISLE:
            ReplaySmacker3();
            break;
        case EXPANSION_CAMPAIGN_VOYAGE_HOME:
            ReplaySmacker4();
            break;
        default:
            ReplaySmackerCustomCampaign();
            break;
    }
    gpWindowManager->m_updateFlags = 1;
}

void ExpCampaign::HandleVictoryCustomCampaign(void) {
    i32 wonMap = H2EnumIndex(m_currentMap) + 1;
    if (ironfist::VictoryMovies[H2EnumIndex(m_campaignId)].count(wonMap))
        PlaySmacker(ironfist::VictoryMovies[H2EnumIndex(m_campaignId)][wonMap]);
    for (i32 opened : ironfist::MapsToComplete[H2EnumIndex(m_campaignId)][wonMap])
        m_mapChoices[opened] = 1;
    if (ironfist::AwardsToGive[H2EnumIndex(m_campaignId)].count(wonMap))
        m_awards[ironfist::AwardsToGive[H2EnumIndex(m_campaignId)][wonMap]] = 1;
}

void ExpCampaign::ReplaySmackerCustomCampaign(void) {
    if (H2EnumIndex(m_viewMap) < EXPANSION_CAMPAIGN_MAX_MAP_COUNT
        && ironfist::ReplayMovies[H2EnumIndex(m_campaignId)].count(H2EnumIndex(m_viewMap)))
        PlaySmacker(ironfist::ReplayMovies[H2EnumIndex(m_campaignId)][H2EnumIndex(m_viewMap)]);
}

void ExpCampaign::ReplaySmacker1(void) {
    switch (m_viewMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_UPRISING) + 1):
            PlaySmacker(SMACKER_POL_INTRO);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ISLAND_OF_CHAOS) + 1):
            PlaySmacker(SMACKER_POL_UPRISING);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ARROWS_FLIGHT) + 1):
            PlaySmacker(SMACKER_POL_ISLAND_OF_CHAOS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_ABYSS) + 1):
            PlaySmacker(SMACKER_POL_ISLAND_OF_CHAOS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_GIANTS_PASS) + 1):
            PlaySmacker(SMACKER_POL_ARROWS_FLIGHT);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_AURORA_BOREALIS) + 1):
            PlaySmacker(SMACKER_POL_BRANCH_REUNITED);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_BETRAYALS_END) + 1):
            PlaySmacker(SMACKER_POL_AURORA_BOREALIS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_POL_CORRUPTIONS_HEART) + 1):
            if (m_mapsPlayed[H2EnumIndex(MAP_POL_BETRAYALS_END)])
                PlaySmacker(SMACKER_POL_BETRAYALS_END);
            else
                PlaySmacker(SMACKER_POL_AURORA_BOREALIS);
            break;
    }
}

void ExpCampaign::ReplaySmacker2(void) {
    switch (m_viewMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_CONQUER_AND_UNIFY) + 1):
            PlaySmacker(SMACKER_DES_INTRO);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_BORDER_TOWNS) + 1):
            PlaySmacker(SMACKER_DES_CONQUER_AND_UNIFY);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_WAYWARD_SON) + 1):
            PlaySmacker(SMACKER_DES_BORDER_TOWNS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_UNCLE_IVAN) + 1):
            PlaySmacker(SMACKER_DES_BORDER_TOWNS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_SOUTHERN_WAR) + 1):
            PlaySmacker(SMACKER_DES_FAMILY_REUNITED);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_IVORY_GATES) + 1):
            PlaySmacker(SMACKER_DES_SOUTHERN_WAR);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_ELVEN_LANDS) + 1):
            PlaySmacker(SMACKER_DES_SOUTHERN_WAR);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_DES_EPIC_BATTLE) + 1):
            PlaySmacker(SMACKER_DES_BRANCH_REUNITED);
            break;
    }
}

void ExpCampaign::ReplaySmacker3(void) {
    switch (m_viewMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_SHROUDED_ISLES) + 1):
            PlaySmacker(SMACKER_WIZ_INTRO);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_ETERNAL_SCROLLS) + 1):
            PlaySmacker(SMACKER_WIZ_SHROUDED_ISLES);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_POWERS_END) + 1):
            PlaySmacker(SMACKER_WIZ_ETERNAL_SCROLLS);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_WIZ_FOUNT_OF_WIZARDRY) + 1):
            if (m_mapsPlayed[H2EnumIndex(MAP_WIZ_POWERS_END)])
                PlaySmacker(SMACKER_WIZ_POWERS_END);
            else
                PlaySmacker(SMACKER_WIZ_ETERNAL_SCROLLS);
            break;
    }
}

void ExpCampaign::ReplaySmacker4(void) {
    switch (m_viewMap + 1) {
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_STRANDED) + 1):
            PlaySmacker(SMACKER_VOY_INTRO);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_PIRATE_ISLES) + 1):
            PlaySmacker(SMACKER_VOY_STRANDED);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_KING_AND_COUNTRY) + 1):
            PlaySmacker(SMACKER_VOY_PIRATE_ISLES);
            break;
        case static_cast<ExpansionCampaignMap>(H2EnumIndex(MAP_VOY_BLOOD_IS_THICKER) + 1):
            PlaySmacker(SMACKER_VOY_PIRATE_ISLES);
    }
}

u8 ExpCampaign::IsCompleted(void) {
    if (m_mapsPlayed[m_mapCount - 1])
        return 1;
    if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME
        && m_mapsPlayed[H2EnumIndex(MAP_VOY_KING_AND_COUNTRY)])
        return 1;
    return 0;
}

i8 ExpCampaign::IsThisMapCompleted(void) {
    if (m_mapsPlayed[H2EnumIndex(m_currentMap)])
        return 1;
    return 0;
}

MessageDispatchResult ExpCampaign::MessageHandler(struct tag_message& message) {
    i32 map;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == true)
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );
    if (giDialogTimeout != 0 && platform::Ticks() > giDialogTimeout) {
        message.type = MESSAGE_WIDGET;
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
        message.payload.widget.command = BaseWidgetCommand(CAMPAIGN_CLOSE_COMMAND);
        giDialogTimeout = 0;
        return MESSAGE_DISPATCH_FORWARD;
    }
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case CAMPAIGN_MESSAGE_HOVER:
            case CAMPAIGN_MESSAGE_HELP:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_TRACK_WIDGET_0:
                    case CAMPAIGN_TRACK_WIDGET_1:
                    case CAMPAIGN_TRACK_WIDGET_2:
                    case CAMPAIGN_TRACK_WIDGET_3:
                    case CAMPAIGN_TRACK_WIDGET_4:
                    case CAMPAIGN_TRACK_WIDGET_5:
                    case CAMPAIGN_TRACK_WIDGET_6:
                    case CAMPAIGN_TRACK_WIDGET_7:
                    case CAMPAIGN_TRACK_WIDGET_8:
                    case CAMPAIGN_TRACK_WIDGET_9:
                    case CAMPAIGN_TRACK_WIDGET_10:
                    case CAMPAIGN_TRACK_WIDGET_11:
                        map = message.payload.widget.id - CAMPAIGN_TRACK_WIDGET_FIRST;
                        if (giDebugLevel >= 1 || xCampaign.m_mapChoices[map]
                            || xCampaign.m_mapsPlayed[map]) {
                            xCampaign.m_viewMap = static_cast<ExpansionCampaignMap>(map);
                            xCampaign.UpdateInfo(1);
                        }
                        break;
                    case CAMPAIGN_BONUS_WIDGET_0:
                    case CAMPAIGN_BONUS_WIDGET_1:
                    case CAMPAIGN_BONUS_WIDGET_2:
                        if (xCampaign.m_viewOnly == 0
                            && xCampaign.m_mapChoices[H2EnumIndex(xCampaign.m_viewMap)]) {
                            xCampaign.m_bonusChoices[H2EnumIndex(xCampaign.m_viewMap)] =
                                message.payload.widget.id - CAMPAIGN_BONUS_WIDGET_FIRST;
                            xCampaign.UpdateInfo(1);
                        }
                        break;
                }
                break;

            case CAMPAIGN_MESSAGE_ACTIVATE:
                switch (message.payload.widget.id) {
                    case CAMPAIGN_DIALOG_REPLAY:
                        xCampaign.ReplaySmacker();
                        xCampaign.m_window->DrawWindow();
                        break;
                    case CAMPAIGN_DIALOG_ACCEPT:
                        if (xCampaign.m_viewOnly == 0) {
                            if (xCampaign.m_mapChoices[H2EnumIndex(xCampaign.m_viewMap)]) {
                                xCampaign.m_currentMap = xCampaign.m_viewMap;
                            } else {
                                NormalDialog(
                                    localization::Tr(
                                        "campaign.selection.invalid_next_scenario"
                                    ),
                                    NORMAL_DIALOG_INFO,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0,
                                    NORMAL_DIALOG_NO_RESOURCE,
                                    0
                                );
                                break;
                            }
                        }
                    case CAMPAIGN_DIALOG_CANCEL:
                    case CAMPAIGN_DIALOG_RESTART:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = CAMPAIGN_CLOSE_COMMAND;
                        message.payload.widget.command =
                            BaseWidgetCommand(CAMPAIGN_CLOSE_COMMAND);
                        giDialogTimeout = 0;
                        return MESSAGE_DISPATCH_FORWARD;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

void ExpCampaign::Autosave(void) {
    if (m_currentMap != MAP_NONE) {
        m_mapsPlayed[H2EnumIndex(m_currentMap)] = 1;
        sprintf(
            gText,
            "%s_%d",
            ironfist::CampaignShortNames[H2EnumIndex(m_campaignId)].c_str(),
            H2EnumIndex(m_currentMap) + 1
        );
        i32 saveIdx = 0;
        for (auto& carried :
             ironfist::HeroesToSave[H2EnumIndex(m_campaignId)][H2EnumIndex(m_currentMap)]) {
            ironfist::SaveCampaignHero(carried.first, carried.second, saveIdx);
            saveIdx++;
        }
        gpGame->SaveGame(gText, 1, 0);
    }
}

ExpansionCampaignId ExpCampaign::Choose(void) {
    PlaySmacker(SMACKER_CAMPAIGN_CHOICE);
    return xLastChoice;
}

i16 ExpCampaign::Days(void) {
    return (m_mapDays[H2EnumIndex(m_currentMap)]
            + (gpGame->m_month - 1) * EXPANSION_CAMPAIGN_DAYS_PER_MONTH)
           + (gpGame->m_week - 1) * EXPANSION_CAMPAIGN_DAYS_PER_WEEK + gpGame->m_day;
}

i32 ExpCampaign::CampaignID(void) {
    return H2EnumIndex(m_campaignId);
}

const char* ExpCampaign::JosephName(void) {
    if (m_currentMap < EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)
        return xJosephName[0];
    return xJosephName[
        H2EnumIndex(m_currentMap) - H2EnumIndex(EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)
    ];
}

const char* ExpCampaign::IvanName(void) {
    if (m_currentMap < EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)
        return xUncleIvanName[0];
    return xUncleIvanName[
        H2EnumIndex(m_currentMap) - H2EnumIndex(EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP)
    ];
}

i8 ExpCampaign::IsSpecialGoldenBow(i32 x, i32 y) {
    if (m_campaignId == EXPANSION_CAMPAIGN_DESCENDANTS
        && m_currentMap == MAP_DES_ELVEN_LANDS
        && x == EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_X && y == EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_Y)
        return 1;
    return 0;
}

i8 ExpCampaign::IsSpecialUA(void) {
    if (m_campaignId == EXPANSION_CAMPAIGN_WIZARDS_ISLE
        && m_currentMap == MAP_WIZ_POWERS_END)
        return 1;
    return 0;
}

i8 ExpCampaign::IsSpecialLossCondition(i32 playerIndex) {
    playerData* player = &gpGame->m_players[playerIndex];

    if (playerIndex == EXPANSION_CAMPAIGN_MAIN_PLAYER
        && m_campaignId == EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY
        && m_currentMap == MAP_POL_ABYSS && player->m_heroCount == 0)
        return 1;
    return 0;
}
