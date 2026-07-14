// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\X_CAMPGN.OBJ   from: (directly linked into exe)
// functions: 36   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/iconWidget.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
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

static const int expansionCampaignTrackXY[EXPANSION_CAMPAIGN_COUNT]
                                         [EXPANSION_CAMPAIGN_MAX_MAP_COUNT][2] = {
    {{113, 310}, {187, 310}, {261, 352}, {261, 310},
     {335, 352}, {370, 310}, {445, 352}, {479, 310}},
    {{111, 331}, {185, 331}, {259, 289}, {259, 373},
     {333, 331}, {407, 373}, {407, 289}, {481, 331}},
    {{186, 310}, {260, 310}, {334, 352}, {408, 310},
     {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
    {{222, 312}, {296, 312}, {370, 312}, {370, 354},
     {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}
};

static const int expansionCampaignMapCounts[EXPANSION_CAMPAIGN_COUNT] = {
    8, 8, 4, 4
};

DATA(0x0051d560) SCampaignChoice
xCampaignChoices[EXPANSION_CAMPAIGN_COUNT]
                [EXPANSION_CAMPAIGN_MAX_MAP_COUNT]
                [EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT] = {
    {
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MEDAL_VALOR, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_STEALTH_SHIELD, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MINOR_SCROLL, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_WHITE_PEARL, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_BALLISTA, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND}},
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MAGE_RING, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MAJOR_SCROLL, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LOGISTICS, HERO_SKILL_LEVEL_ADVANCED}},
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MEDAL_HONOR, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_CASTER_BRACELET, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_DEFENDER_HELM, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_POWER_AXE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_PRIMARY_SKILL, HERO_PRIMARY_DEFENSE, EXPANSION_CAMPAIGN_PRIMARY_BONUS_TWO}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND}, {CAMPAIGN_CHOICE_CREATURES, ARMY_CREATURE_CRUSADER, EXPANSION_CAMPAIGN_CRUSADER_COUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_TRAVELER_BOOTS, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_SULFUR, EXPANSION_CAMPAIGN_SULFUR_TEN}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_POWER_AXE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_SPELL_SCROLL, SPELL_ANIMATE_DEAD, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_SPELL, EXPANSION_CAMPAIGN_SPELL_VIEW_HEROES, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_CREATURES, ARMY_CREATURE_MAGE, EXPANSION_CAMPAIGN_MAGE_COUNT}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_ESTATES, HERO_SKILL_LEVEL_ADVANCED}}
    },
    {
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_ONE_THOUSAND}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_WOOD_FIFTY}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_CRYSTAL, EXPANSION_CAMPAIGN_CRYSTAL_TEN}},
        {{CAMPAIGN_CHOICE_PRIMARY_SKILL, HERO_PRIMARY_ATTACK, EXPANSION_CAMPAIGN_PRIMARY_BONUS_ONE}, {CAMPAIGN_CHOICE_PRIMARY_SKILL, HERO_PRIMARY_DEFENSE, EXPANSION_CAMPAIGN_PRIMARY_BONUS_ONE}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_ONE_THOUSAND}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LOGISTICS, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SPELL, SPELL_STEEL_SKIN, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MAGE_RING, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LOGISTICS, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_SCOUTING, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_POWER_AXE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LUCK, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LEADERSHIP, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_SPIKED_SHIELD, EXPANSION_CAMPAIGN_ARTIFACT_AMOUNT_ONE}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_PENALTY_ONE_THOUSAND}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_WOOD_PENALTY_TEN}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_MERCURY, EXPANSION_CAMPAIGN_MERCURY_PENALTY_TWO}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_ONE_THOUSAND}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_SCOUTING, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_WOOD_TWENTY}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LEADERSHIP, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_ARCHERY, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_PRIMARY_SKILL, HERO_PRIMARY_SPELL_POWER, EXPANSION_CAMPAIGN_PRIMARY_BONUS_ONE}}
    },
    {
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_POWER_AXE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_STEALTH_SHIELD, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_MAGE_RING, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_FOREMOST_SCROLL, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_SPELL, SPELL_FIREBLAST, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_SPELL, SPELL_MASS_HASTE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_SPELL, SPELL_SUMMON_EARTH_ELEMENTAL, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_SPELL, SPELL_CHAIN_LIGHTNING, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}},
        {{CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_WOOD_FIVE}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_ORE, EXPANSION_CAMPAIGN_ORE_FIVE}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_ONE_THOUSAND}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}}
    },
    {
        {{CAMPAIGN_CHOICE_ARTIFACT, EVENT_ARTIFACT_NOMAD_BOOTS, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_FIVE_HUNDRED}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_SCOUTING, HERO_SKILL_LEVEL_BASIC}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_PATHFINDING, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_NAVIGATION, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_SULFUR, EXPANSION_CAMPAIGN_SULFUR_FIFTEEN}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_ARCHERY, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_THREE_THOUSAND}, {CAMPAIGN_CHOICE_PRIMARY_SKILL, HERO_PRIMARY_ATTACK, EXPANSION_CAMPAIGN_PRIMARY_BONUS_ONE}},
        {{CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_NECROMANCY, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_SECONDARY_SKILL, HERO_SKILL_LOGISTICS, HERO_SKILL_LEVEL_BASIC}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_GOLD, EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}},
        {{CAMPAIGN_CHOICE_INVALID, EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE, EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}, {CAMPAIGN_CHOICE_RESOURCE, EXPANSION_CAMPAIGN_RESOURCE_WOOD, EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT}}
    }
};

static const signed char
expansionCampaignDifficulty[EXPANSION_CAMPAIGN_COUNT]
                           [EXPANSION_CAMPAIGN_MAX_MAP_COUNT] = {
    {0, 1, 1, 1, 1, 2, 2, 3},
    {0, 1, 1, 1, 2, 2, 2, 3},
    {1, 1, 1, 2, -1, -1, -1, -1},
    {0, 1, 2, 2, -1, -1, -1, -1}
};
VA(0x004bb680, 0x23)
ExpCampaign::ExpCampaign(void)
{
    m_window = 0;
}

VA(0x004bb6a3, 0x16)
ExpCampaign::~ExpCampaign() {}

VA(0x004bb6b9, 0x29)
void ExpCampaign::ResetMapChoices(void)
{
    memset(m_mapChoices, 0, sizeof(m_mapChoices));
}

VA(0x004bb6e2, 0x29)
void ExpCampaign::ResetMapsPlayed(void)
{
    memset(m_mapsPlayed, 0, sizeof(m_mapsPlayed));
}

VA(0x004bb70b, 0x29)
void ExpCampaign::ResetAwards(void)
{
    memset(m_awards, 0, sizeof(m_awards));
}

VA(0x004bb734, 0x29)
void ExpCampaign::ResetBonusChoices(void)
{
    memset(m_bonusChoices, 0, sizeof(m_bonusChoices));
}

VA(0x004bb75d, 0x23)
void ExpCampaign::GrantAward(int award)
{
    m_awards[award] = 1;
}

VA(0x004bb780, 0x23)
void ExpCampaign::RemoveAward(int award)
{
    m_awards[award] = 0;
}

VA(0x004bb7a3, 0x22)
signed char ExpCampaign::HasAward(int award)
{
    return m_awards[award];
}

VA(0x004bb7c5, 0x24)
void ExpCampaign::SetMapWasPlayed(void)
{
    m_mapsPlayed[m_currentMap] = 1;
}

VA(0x004bb7e9, 0x5a)
void ExpCampaign::InitNewCampaign(int campaignId)
{
    m_campaignId = campaignId;
    m_currentMap = -1;
    m_mapCount = expansionCampaignMapCounts[campaignId];
    ResetMapChoices();
    ResetMapsPlayed();
    ResetAwards();
    ResetBonusChoices();
}

// @match-note retained 94.20%, live 94.06% after named-enum recovery (pre-95
// structural checkpoint): semantics, 0x34 frame,
// cases, and external targets are recovered. The first non-data divergence is
// after NewMap: retail stores gpGame + 0x49c at [ebp-0xc], while this build
// stores the playerRec pointer at [ebp-0x8] and eagerly saves
// gbInNewGameSetup. homm2 relocs reports 27/81 because retail's two jump tables
// are delinked as function-local targets; no base-only external target remains.
// Tried a real playerRec pointer, retaining GetMapHeader's result, direct
// m_mapDays[0], and both gpGame/player spellings. Revisit after 95% for switch
// temporary/source-shape and od_slots tuning; do not repeat these spellings.
VA(0x004bb843, 0x7cb)
void ExpCampaign::InitMap(void)
{
    SCampaignChoice *choice =
        &xCampaignChoices[m_campaignId][m_currentMap]
                         [m_bonusChoices[m_currentMap]];

    memset(gpGame->m_setupPlayerColor, 0,
           EXPANSION_CAMPAIGN_PLAYER_SETUP_RESET_SIZE);
    sprintf(gpGame->m_mapFilename, "CAMP%d_%02d.HXC", m_campaignId + 1,
            m_currentMap + 1);
    gpGame->m_newGameInitialized = 0;
    if (m_currentMap == 0)
        m_mapDays[0] = 0;
    strcpy(gMapName, gpGame->m_mapFilename);
    int mapHeaderResult =
        GetMapHeader(gpGame->m_mapFilename, &gpGame->m_mapHeader);
    gpGame->LoadGame("origdata.bin", 1, 0);
    gpGame->InitNewGame(0);
    gpGame->m_difficulty =
        expansionCampaignDifficulty[m_campaignId][m_currentMap];
    gpGame->m_playerCount = gpGame->m_mapHeader.playerCount;
    gpGame->NewMap(gMapName);

    playerRec *player = &gpGame->m_players[0];
    int savedNewGameSetup = gbInNewGameSetup;
    int heroIndex;
    hero *choiceHero;
    switch (choice->type) {
    case CAMPAIGN_CHOICE_RESOURCE:
        player->resources[choice->value] += choice->amount;
        break;
    case CAMPAIGN_CHOICE_ARTIFACT:
        if (gpGame->m_players[0].heroCount > 0)
            GiveArtifact(gpGame->GetPlayerHero(0, 0), choice->value, 0, -1);
        break;
    case CAMPAIGN_CHOICE_SPELL:
        if (gpGame->m_players[0].heroCount > 0)
            gpGame->GetPlayerHero(0, 0)->m_spells[choice->value] = 1;
        break;
    case CAMPAIGN_CHOICE_SECONDARY_SKILL:
        if (gpGame->m_players[0].heroCount > 0) {
            for (heroIndex = 0;
                 heroIndex < gpGame->m_players[0].heroCount; ++heroIndex) {
                choiceHero = gpGame->GetPlayerHero(0, heroIndex);
                if (m_campaignId == EXPANSION_CAMPAIGN_VOYAGE_HOME &&
                    m_currentMap == 3) {
                    if (choiceHero->m_portrait == EXPANSION_HERO_GALLAVANT)
                        break;
                } else if (m_campaignId != EXPANSION_CAMPAIGN_VOYAGE_HOME ||
                           m_currentMap != 2 ||
                           choiceHero->m_portrait == EXPANSION_HERO_CEALLACH) {
                    break;
                }
            }
            choiceHero->SetSS(choice->value, choice->amount);
        }
        break;
    case CAMPAIGN_CHOICE_CREATURES:
        if (gpGame->m_players[0].heroCount > 0)
            gpGame->GetPlayerHero(0, 0)->m_army.Add(
                choice->value, choice->amount, -1);
        break;
    case CAMPAIGN_CHOICE_PUZZLE_PIECES:
        gpGame->m_players[0].unknown13 =
            static_cast<signed char>(choice->value);
        break;
    case CAMPAIGN_CHOICE_EXPERIENCE:
        gbInNewGameSetup = 1;
        if (gpGame->m_players[0].heroCount > 0) {
            gpGame->GetPlayerHero(0, 0)->m_experience += choice->value;
            gpGame->GetPlayerHero(0, 0)->CheckLevel();
        }
        break;
    case CAMPAIGN_CHOICE_NONE:
        break;
    case CAMPAIGN_CHOICE_PRIMARY_SKILL:
        if (gpGame->m_players[0].heroCount > 0) {
            for (heroIndex = 0;
                 heroIndex < gpGame->m_players[0].heroCount; ++heroIndex) {
                choiceHero = gpGame->GetPlayerHero(0, heroIndex);
                if (m_campaignId != EXPANSION_CAMPAIGN_VOYAGE_HOME ||
                    m_currentMap != 2 ||
                    choiceHero->m_portrait == EXPANSION_HERO_CEALLACH)
                    break;
            }
            choiceHero->m_primaryStats[choice->value] += choice->amount;
        }
        break;
    case CAMPAIGN_CHOICE_SPELL_SCROLL:
        if (gpGame->m_players[0].heroCount > 0)
            GiveArtifact(gpGame->GetPlayerHero(0, 0),
                         EVENT_ARTIFACT_SPELL_SCROLL, 0,
                         static_cast<signed char>(choice->value));
        break;
    }
    gbInNewGameSetup = savedNewGameSetup;

    int award;
    for (award = 0; award < EXPANSION_CAMPAIGN_AWARD_COUNT; ++award) {
        if (m_awards[award] == 0)
            continue;
        switch (award) {
        case EXPANSION_AWARD_ELVEN_ALLIANCE:
            break;
        case EXPANSION_AWARD_BREASTPLATE_ANDURAN:
            if (player->heroCount > 0)
                GiveArtifact(gpGame->GetPlayerHero(0, 0),
                             EVENT_ARTIFACT_BREASTPLATE_ANDURAN, 0, -1);
            break;
        case EXPANSION_AWARD_WOOD_BONUS:
            break;
        case EXPANSION_AWARD_HELMET_ANDURAN:
            if (player->heroCount > 0)
                GiveArtifact(gpGame->GetPlayerHero(0, 0),
                             EVENT_ARTIFACT_HELMET_ANDURAN, 0, -1);
            break;
        case EXPANSION_AWARD_DEFEAT_KRAEGER:
            for (heroIndex = 0; heroIndex < EXPANSION_CAMPAIGN_HERO_COUNT;
                 ++heroIndex) {
                if (gpGame->m_heroRecs[heroIndex].m_portrait ==
                    EXPANSION_HERO_DAINWIN)
                    gpGame->m_heroRecs[heroIndex].Deallocate(0);
            }
            break;
        case EXPANSION_AWARD_BATTLE_GARB:
            if (player->heroCount > 0)
                GiveArtifact(gpGame->GetPlayerHero(0, 0),
                             EVENT_ARTIFACT_BATTLE_GARB, 0, -1);
            break;
        case EXPANSION_AWARD_WAYWARD_SON:
        case EXPANSION_AWARD_UNCLE_IVAN:
            break;
        case EXPANSION_AWARD_LEGENDARY_SCEPTER:
            if (player->heroCount > 0)
                GiveArtifact(gpGame->GetPlayerHero(0, 0),
                             EVENT_ARTIFACT_LEGENDARY_SCEPTER, 0, -1);
            break;
        case EXPANSION_AWARD_SET_GUARDIAN:
            if (player->heroCount > 0)
                gpGame->GetPlayerHero(0, 0)
                    ->m_spells[EXPANSION_CAMPAIGN_SET_GUARDIAN_SPELL] = 1;
            break;
        case EXPANSION_AWARD_SPHERE_NEGATION:
            if (player->heroCount > 0)
                GiveArtifact(gpGame->GetPlayerHero(0, 0),
                             EVENT_ARTIFACT_SPHERE_NEGATION, 0, -1);
            break;
        }
    }
    gbRetreatWin = 1;
}

// @match-note 99.97% (pre-95 structural checkpoint): frame (0x44), CFG, all 51
// relocations, and instruction stream align. Relocation-masked raw comparison
// leaves seven stack-displacement bytes at +0x3e, +0x15e, +0x1a5, +0x1ad,
// +0x1b4, +0x1c5, and +0x285 (only [ebp-0x8]/[ebp-0xc] local ordering).
// Explicit track/campaign widget pointer initialization was tried and retained;
// it raised 99.52% to 99.97%. Revisit after 95% with od_slots name tuning.
VA(0x004bc00e, 0x33f)
void ExpCampaign::ShowInfo(int viewOnly, int)
{
    m_viewOnly = viewOnly;
    gpMouseManager->SetPointer("advmice.mse", 0,
                               CAMPAIGN_POINTER_HIDDEN_HOTSPOT);
    gpMouseManager->ReallyShowPointer();
    int savedInterface = gbUseEvilInterface;
    gbUseEvilInterface = 1;
    m_viewMap = m_currentMap;
    m_window = new heroWindow(0, 0, "x_camp.bin");
    if (m_window == 0)
        MemError();

    widget *trackWidget = 0;
    int map;
    for (map = 0; map < m_mapCount; ++map) {
        trackWidget = new iconWidget(
            expansionCampaignTrackXY[m_campaignId][map][0],
            expansionCampaignTrackXY[m_campaignId][map][1],
            EXPANSION_CAMPAIGN_TRACK_ICON_SIZE,
            EXPANSION_CAMPAIGN_TRACK_ICON_SIZE, "x_cmpext.icn", 0, 0,
            map + CAMPAIGN_TRACK_WIDGET_FIRST,
            EXPANSION_CAMPAIGN_TRACK_ICON_COLOR, 1);
        if (trackWidget == 0)
            MemError();
        m_window->AddWidget(trackWidget, -1);
    }

    widget *campaignIcon = 0;
    campaignIcon = new iconWidget(
        24, 25, 0x178, 0x31, "x_cmpext.icn",
        m_campaignId + EXPANSION_CAMPAIGN_ICON_FRAME_BASE, 0, -1,
        EXPANSION_CAMPAIGN_TRACK_ICON_COLOR, 1);
    if (campaignIcon == 0)
        MemError();
    m_window->AddWidget(campaignIcon, -1);

    tag_message message;
    message.type = CAMPAIGN_MESSAGE_WIDGET;
    if (viewOnly == 0) {
        message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.id = CAMPAIGN_DIALOG_RESTART;
        message.payload.widget.data.value = CAMPAIGN_MESSAGE_DESELECT;
        m_window->BroadcastMessage(message);
    }
    if (gbLowMemory != 0) {
        message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.id = CAMPAIGN_DIALOG_REPLAY;
        message.payload.widget.data.value = CAMPAIGN_MESSAGE_DESELECT;
        m_window->BroadcastMessage(message);
    }
    gpSoundManager->SwitchAmbientMusic(EXPANSION_CAMPAIGN_MUSIC);
    UpdateInfo(0);
    gpWindowManager->DoDialog(m_window, MessageHandler, 0);
    delete m_window;
    gbUseEvilInterface = savedInterface;

    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_RESTART) {
        NormalDialog("Are you sure you want to restart this scenario?",
                     CAMPAIGN_RESTART_CONFIRM,
                     CAMPAIGN_DIALOG_NO_RESOURCE, CAMPAIGN_DIALOG_NO_RESOURCE,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0,
                     CAMPAIGN_DIALOG_NO_RESOURCE, 0);
        if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
            InitMap();
            gpAdvManager->m_visibilityMapValid = 0;
            giBottomViewOverride = 0;
            gpWindowManager->FadeScreen(1, CAMPAIGN_DIALOG_FADE_STEPS,
                                        gPalette);
            gpAdvManager->SetInitialMapOrigin();
            gpAdvManager->RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(0, CAMPAIGN_DIALOG_FADE_STEPS,
                                        gPalette);
        }
    }
}

// @match-note 92.22% (pre-95 structural checkpoint): all UI semantics, case
// bodies/order, and external targets are recovered. Retail frame is 0x84 versus
// 0x74 here; the first code-shape divergence is +0x36, where retail folds the
// selected-frame +3 into lea and this build emits lea/add. homm2 relocs reports
// 39/177 because both retail jump tables are delinked as function-local targets;
// no base-only external target remains. Tried signed-char hasAwards, retail
// artifact body order, explicit choiceOffset/selectedSpell/showScroll locals,
// multidimensional versus flattened choice indexing, and an od_slots semantic
// suffix for the 52-byte army-name buffer. Revisit after 95% for frame/switch
// temporary and expression-shape tuning; do not repeat these spellings.
VA(0x004bc34d, 0x921)
void ExpCampaign::UpdateInfo(int redraw)
{
    tag_message message;
    int map;
    SCampaignChoice *choice;
    int choiceOffset;
    short selectedSpell;
    signed char showScroll;
    char armyName8[EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE];

    message.type = CAMPAIGN_MESSAGE_WIDGET;
    for (map = 0; map < m_mapCount; ++map) {
        if (m_mapChoices[map] != 0)
            message.payload.widget.data.value = 1;
        else if (m_mapsPlayed[map] != 0)
            message.payload.widget.data.value = 0;
        else
            message.payload.widget.data.value = 2;
        if (m_viewMap == map)
            message.payload.widget.data.value += m_campaignId * 3 + 3;
        message.payload.widget.command = CAMPAIGN_MESSAGE_SET_FRAME;
        message.payload.widget.id = map + CAMPAIGN_TRACK_WIDGET_FIRST;
        m_window->BroadcastMessage(message);
    }

    message.payload.widget.command = CAMPAIGN_MESSAGE_SET_ICON;
    message.payload.widget.id = CAMPAIGN_TRACK_ICON_WIDGET;
    message.payload.widget.data.text = gText;
    sprintf(gText, "x_track%d.icn", m_campaignId + 1);
    m_window->BroadcastMessage(message);

    message.payload.widget.command = CAMPAIGN_MESSAGE_SET_TEXT;
    message.payload.widget.data.text = gText;
    message.payload.widget.id = CAMPAIGN_SCENARIO_NUMBER_WIDGET;
    sprintf(gText, "%d", m_viewMap + 1);
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_NAME_WIDGET;
    sprintf(gText, "%s", xScenarioName[m_campaignId][m_viewMap]);
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_DESCRIPTION_WIDGET;
    sprintf(gText, "%s", xScenarioDescription[m_campaignId][m_viewMap]);
    m_window->BroadcastMessage(message);

    message.payload.widget.id = CAMPAIGN_SCENARIO_BONUS_WIDGET;
    sprintf(gText, "%d", m_mapDays[m_viewMap]);
    m_window->BroadcastMessage(message);

    signed char hasAwards = 0;
    message.payload.widget.id = CAMPAIGN_AWARDS_WIDGET;
    strcpy(gText, "");
    for (map = 0; map < EXPANSION_CAMPAIGN_AWARD_COUNT; ++map) {
        if (m_awards[map] != 0) {
            hasAwards = 1;
            strcat(gText, xCampaignAwards[map]);
            strcat(gText, "\n");
        }
    }
    if (hasAwards == 0)
        sprintf(gText, "None");
    m_window->BroadcastMessage(message);

    for (map = 0; map < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        choiceOffset = m_viewMap *
                           EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT +
                       m_campaignId * EXPANSION_CAMPAIGN_MAX_MAP_COUNT *
                           EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT +
                       map;
        choice = xCampaignChoices[0][0] + choiceOffset;
        switch (choice->type) {
        case CAMPAIGN_CHOICE_RESOURCE:
            sprintf(gText, "%d %s", choice->amount,
                    gResourceNames[choice->value]);
            break;
        case CAMPAIGN_CHOICE_ARTIFACT:
            switch (choice->value) {
            case EVENT_ARTIFACT_MINOR_SCROLL:
                strcpy(gText, "Minor Scroll");
                break;
            case EVENT_ARTIFACT_MAGE_RING:
                strcpy(gText, "Mage's Ring");
                break;
            case EVENT_ARTIFACT_DEFENDER_HELM:
                strcpy(gText, "Defender Helm");
                break;
            case EVENT_ARTIFACT_POWER_AXE:
                strcpy(gText, "Power Axe");
                break;
            case EVENT_ARTIFACT_DRAGON_SWORD:
                strcpy(gText, "Dragon Sword");
                break;
            case EVENT_ARTIFACT_DIVINE_BREASTPLATE:
                strcpy(gText, "Breastplate");
                break;
            case EVENT_ARTIFACT_FIZBIN_MISFORTUNE:
                strcpy(gText, "Fizbin Medal");
                break;
            case EVENT_ARTIFACT_THUNDER_MACE:
                strcpy(gText, "Thunder Mace");
                break;
            case EVENT_ARTIFACT_ARMORED_GAUNTLETS:
                strcpy(gText, "Gauntlets");
                break;
            case EVENT_ARTIFACT_MAJOR_SCROLL:
                strcpy(gText, "Major Scroll");
                break;
            case EVENT_ARTIFACT_FOREMOST_SCROLL:
                strcpy(gText, "Foremost Scroll");
                break;
            case EVENT_ARTIFACT_BALLISTA:
                strcpy(gText, "Ballista");
                break;
            case EVENT_ARTIFACT_STEALTH_SHIELD:
                strcpy(gText, "Stealth Shield");
                break;
            case EVENT_ARTIFACT_NOMAD_BOOTS:
                strcpy(gText, "Nomad Boots");
                break;
            case EVENT_ARTIFACT_TRAVELER_BOOTS:
                strcpy(gText, "Traveler's Boots");
                break;
            default:
                sprintf(gText, "%s", gArtifactNames[choice->value]);
                break;
            }
            break;
        case CAMPAIGN_CHOICE_SPELL:
            if (choice->value == CAMPAIGN_SUMMON_EARTH_SPELL)
                sprintf(gText, "Summon Earth");
            else
                sprintf(gText, "%s", gSpellNames[choice->value]);
            break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            if ((choice->amount == EXPANSION_CAMPAIGN_SPECIAL_SKILL_LEVEL &&
                 choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL) ||
                (choice->amount ==
                     EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT_LEVEL &&
                 choice->value == EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT)) {
                sprintf(gText, "%s %s",
                        xShortSSLevelNames[choice->amount - 1],
                        gSecondarySkills[choice->value]);
            } else {
                sprintf(gText, "%s %s",
                        gSecondarySkillLevels[choice->amount],
                        gSecondarySkills[choice->value]);
            }
            break;
        case CAMPAIGN_CHOICE_CREATURES:
            strcpy(armyName8, gArmyNamesPlural[choice->value]);
            armyName8[0] -= 'a' - 'A';
            sprintf(gText, "%d %s", choice->amount, armyName8);
            break;
        case CAMPAIGN_CHOICE_PUZZLE_PIECES:
            sprintf(gText, "%d %s", choice->value, "Puzzle Pieces");
            break;
        case CAMPAIGN_CHOICE_EXPERIENCE:
            sprintf(gText, "%d %s", choice->value, "Experience");
            break;
        case CAMPAIGN_CHOICE_NONE:
            sprintf(gText, "None");
            break;
        case CAMPAIGN_CHOICE_ALIGNMENT:
            sprintf(gText, gAlignmentNames[choice->value]);
            break;
        case CAMPAIGN_CHOICE_PRIMARY_SKILL:
            sprintf(gText, "%s +%d", gStatNames[choice->value],
                    choice->amount);
            break;
        case CAMPAIGN_CHOICE_SPELL_SCROLL:
            showScroll = 1;
            selectedSpell = choice->value;
            if (selectedSpell == EXPANSION_CAMPAIGN_NON_SCROLL_SPELL_FIRST ||
                selectedSpell == EXPANSION_CAMPAIGN_NON_SCROLL_SPELL_SECOND)
                showScroll = 0;
            if (showScroll != 0) {
                sprintf(gText, "%s %s", gSpellNames[choice->value],
                        "Scroll");
            } else {
                sprintf(gText, "%s", gSpellNames[choice->value]);
            }
            break;
        }
        message.payload.widget.id = map + CAMPAIGN_BONUS_TEXT_WIDGET_FIRST;
        m_window->BroadcastMessage(message);
    }

    for (map = 0; map < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++map) {
        message.payload.widget.id = map + CAMPAIGN_BONUS_WIDGET_FIRST;
        message.payload.widget.command = CAMPAIGN_MESSAGE_SET_FRAME;
        if (m_viewOnly == 0 && m_mapChoices[m_viewMap] != 0)
            message.payload.widget.data.value = CAMPAIGN_WIDGET_ENABLE_FRAME;
        else
            message.payload.widget.data.value = CAMPAIGN_WIDGET_DISABLE_FRAME;
        m_window->BroadcastMessage(message);

        if (m_bonusChoices[m_viewMap] == map)
            message.payload.widget.command = CAMPAIGN_MESSAGE_SELECT;
        else
            message.payload.widget.command = CAMPAIGN_MESSAGE_DESELECT;
        message.payload.widget.data.value = CAMPAIGN_WIDGET_REFRESH_FRAME;
        m_window->BroadcastMessage(message);
    }
    if (redraw != 0)
        m_window->DrawWindow();
}

// @early-stop
// Equal-sized raw code is identical after masking the five
// local jump-table DIR32 fields at +0xaa..+0xbd. Retail delinks them as this
// function while our object uses $L labels; all external relocations match.
VA(0x004bcc6e, 0x172)
int ExpCampaign::HandleVictory(void)
{
    int days = 0;
    int mapIndex;

    if (m_currentMap > EXPANSION_MAP_NONE) {
        days = Days();
        m_mapsPlayed[m_currentMap] = 1;
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
    }
    if (IsCompleted())
        return 0;

    m_currentMap = EXPANSION_MAP_NONE;
    for (mapIndex = 0; mapIndex < m_mapCount; ++mapIndex) {
        if (m_mapChoices[mapIndex]) {
            m_mapDays[mapIndex] = days;
            if (m_currentMap == EXPANSION_MAP_NONE)
                m_currentMap = mapIndex;
        }
    }
    ShowInfo(0, 0);
    if (gpWindowManager->m_dialogResult == CAMPAIGN_DIALOG_ACCEPT)
        return 1;
    return 0;
}

// @early-stop
// Equal-sized raw code is identical after masking the ten
// local jump-table DIR32 fields at +0x135..+0x15c. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bcde0, 0x167)
void ExpCampaign::HandleVictory1(void)
{
    switch (m_currentMap + 1) {
    case EXPANSION_MAP_NONE + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_INTRO);
        m_mapChoices[EXPANSION_MAP_POL_UPRISING] = 1;
        break;
    case EXPANSION_MAP_POL_UPRISING + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_UPRISING);
        m_mapChoices[EXPANSION_MAP_POL_ISLAND_OF_CHAOS] = 1;
        break;
    case EXPANSION_MAP_POL_ISLAND_OF_CHAOS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_ISLAND_OF_CHAOS);
        m_mapChoices[EXPANSION_MAP_POL_ARROWS_FLIGHT] = 1;
        m_mapChoices[EXPANSION_MAP_POL_ABYSS] = 1;
        m_awards[EXPANSION_AWARD_BREASTPLATE_ANDURAN] = 1;
        break;
    case EXPANSION_MAP_POL_ARROWS_FLIGHT + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_ARROWS_FLIGHT);
        m_mapChoices[EXPANSION_MAP_POL_GIANTS_PASS] = 1;
        m_awards[EXPANSION_AWARD_ELVEN_ALLIANCE] = 1;
        m_awards[EXPANSION_AWARD_WOOD_BONUS] = 1;
        break;
    case EXPANSION_MAP_POL_ABYSS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_BRANCH_REUNITED);
        m_mapChoices[EXPANSION_MAP_POL_AURORA_BOREALIS] = 1;
        break;
    case EXPANSION_MAP_POL_GIANTS_PASS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_BRANCH_REUNITED);
        m_mapChoices[EXPANSION_MAP_POL_AURORA_BOREALIS] = 1;
        break;
    case EXPANSION_MAP_POL_AURORA_BOREALIS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_AURORA_BOREALIS);
        m_mapChoices[EXPANSION_MAP_POL_BETRAYALS_END] = 1;
        m_mapChoices[EXPANSION_MAP_POL_CORRUPTIONS_HEART] = 1;
        m_awards[EXPANSION_AWARD_HELMET_ANDURAN] = 1;
        break;
    case EXPANSION_MAP_POL_BETRAYALS_END + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_BETRAYALS_END);
        m_mapChoices[EXPANSION_MAP_POL_CORRUPTIONS_HEART] = 1;
        m_awards[EXPANSION_AWARD_BATTLE_GARB] = 1;
        m_awards[EXPANSION_AWARD_BREASTPLATE_ANDURAN] = 0;
        m_awards[EXPANSION_AWARD_HELMET_ANDURAN] = 0;
        m_awards[EXPANSION_AWARD_DEFEAT_KRAEGER] = 1;
        break;
    case EXPANSION_MAP_POL_CORRUPTIONS_HEART + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_CORRUPTIONS_HEART);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the ten
// local jump-table DIR32 fields at +0x119..+0x140. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bcf47, 0x14b)
void ExpCampaign::HandleVictory2(void)
{
    switch (m_currentMap + 1) {
    case EXPANSION_MAP_NONE + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_INTRO);
        m_mapChoices[EXPANSION_MAP_DES_CONQUER_AND_UNIFY] = 1;
        break;
    case EXPANSION_MAP_DES_CONQUER_AND_UNIFY + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_CONQUER_AND_UNIFY);
        m_mapChoices[EXPANSION_MAP_DES_BORDER_TOWNS] = 1;
        break;
    case EXPANSION_MAP_DES_BORDER_TOWNS + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BORDER_TOWNS);
        m_mapChoices[EXPANSION_MAP_DES_WAYWARD_SON] = 1;
        m_mapChoices[EXPANSION_MAP_DES_UNCLE_IVAN] = 1;
        break;
    case EXPANSION_MAP_DES_WAYWARD_SON + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_FAMILY_REUNITED);
        m_mapChoices[EXPANSION_MAP_DES_SOUTHERN_WAR] = 1;
        m_awards[EXPANSION_AWARD_WAYWARD_SON] = 1;
        break;
    case EXPANSION_MAP_DES_UNCLE_IVAN + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_FAMILY_REUNITED);
        m_mapChoices[EXPANSION_MAP_DES_SOUTHERN_WAR] = 1;
        m_awards[EXPANSION_AWARD_UNCLE_IVAN] = 1;
        break;
    case EXPANSION_MAP_DES_SOUTHERN_WAR + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_SOUTHERN_WAR);
        m_mapChoices[EXPANSION_MAP_DES_IVORY_GATES] = 1;
        m_mapChoices[EXPANSION_MAP_DES_ELVEN_LANDS] = 1;
        break;
    case EXPANSION_MAP_DES_IVORY_GATES + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BRANCH_REUNITED);
        m_mapChoices[EXPANSION_MAP_DES_EPIC_BATTLE] = 1;
        m_awards[EXPANSION_AWARD_LEGENDARY_SCEPTER] = 1;
        break;
    case EXPANSION_MAP_DES_ELVEN_LANDS + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BRANCH_REUNITED);
        m_mapChoices[EXPANSION_MAP_DES_EPIC_BATTLE] = 1;
        m_awards[EXPANSION_AWARD_ELVEN_ALLIANCE] = 1;
        break;
    case EXPANSION_MAP_DES_EPIC_BATTLE + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_EPIC_BATTLE);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the six
// local jump-table DIR32 fields at +0xac..+0xc3. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd092, 0xce)
void ExpCampaign::HandleVictory3(void)
{
    switch (m_currentMap + 1) {
    case EXPANSION_MAP_NONE + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_INTRO);
        m_mapChoices[EXPANSION_MAP_WIZ_SHROUDED_ISLES] = 1;
        break;
    case EXPANSION_MAP_WIZ_SHROUDED_ISLES + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_SHROUDED_ISLES);
        m_mapChoices[EXPANSION_MAP_WIZ_ETERNAL_SCROLLS] = 1;
        break;
    case EXPANSION_MAP_WIZ_ETERNAL_SCROLLS + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_ETERNAL_SCROLLS);
        m_mapChoices[EXPANSION_MAP_WIZ_POWERS_END] = 1;
        m_mapChoices[EXPANSION_MAP_WIZ_FOUNT_OF_WIZARDRY] = 1;
        m_awards[EXPANSION_AWARD_SET_GUARDIAN] = 1;
        break;
    case EXPANSION_MAP_WIZ_POWERS_END + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_POWERS_END);
        m_mapChoices[EXPANSION_MAP_WIZ_FOUNT_OF_WIZARDRY] = 1;
        m_awards[EXPANSION_AWARD_SPHERE_NEGATION] = 1;
        break;
    case EXPANSION_MAP_WIZ_FOUNT_OF_WIZARDRY + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_FOUNT_OF_WIZARDRY);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the six
// local jump-table DIR32 fields at +0x97..+0xae. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd160, 0xb9)
void ExpCampaign::HandleVictory4(void)
{
    switch (m_currentMap + 1) {
    case EXPANSION_MAP_NONE + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_INTRO);
        m_mapChoices[EXPANSION_MAP_VOY_STRANDED] = 1;
        break;
    case EXPANSION_MAP_VOY_STRANDED + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_STRANDED);
        m_mapChoices[EXPANSION_MAP_VOY_PIRATE_ISLES] = 1;
        break;
    case EXPANSION_MAP_VOY_PIRATE_ISLES + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_PIRATE_ISLES);
        m_mapChoices[EXPANSION_MAP_VOY_KING_AND_COUNTRY] = 1;
        m_mapChoices[EXPANSION_MAP_VOY_BLOOD_IS_THICKER] = 1;
        break;
    case EXPANSION_MAP_VOY_KING_AND_COUNTRY + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_KING_AND_COUNTRY);
        break;
    case EXPANSION_MAP_VOY_BLOOD_IS_THICKER + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_BLOOD_IS_THICKER);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the five
// local jump-table DIR32 fields at +0x62..+0x75. Retail delinks them as this
// function while our object uses $L labels; all callees and the global match.
VA(0x004bd219, 0x8c)
void ExpCampaign::ReplaySmacker(void)
{
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
    }
    gpWindowManager->m_updateFlags = 1;
}

// @early-stop
// Equal-sized raw code is identical after masking the nine
// local jump-table DIR32 fields at +0xca..+0xed. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd2a5, 0xf8)
void ExpCampaign::ReplaySmacker1(void)
{
    switch (m_viewMap + 1) {
    case EXPANSION_MAP_POL_UPRISING + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_INTRO);
        break;
    case EXPANSION_MAP_POL_ISLAND_OF_CHAOS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_UPRISING);
        break;
    case EXPANSION_MAP_POL_ARROWS_FLIGHT + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_ISLAND_OF_CHAOS);
        break;
    case EXPANSION_MAP_POL_ABYSS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_ISLAND_OF_CHAOS);
        break;
    case EXPANSION_MAP_POL_GIANTS_PASS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_ARROWS_FLIGHT);
        break;
    case EXPANSION_MAP_POL_AURORA_BOREALIS + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_BRANCH_REUNITED);
        break;
    case EXPANSION_MAP_POL_BETRAYALS_END + 1:
        PlaySmacker(EXPANSION_SMACKER_POL_AURORA_BOREALIS);
        break;
    case EXPANSION_MAP_POL_CORRUPTIONS_HEART + 1:
        if (m_mapsPlayed[EXPANSION_MAP_POL_BETRAYALS_END])
            PlaySmacker(EXPANSION_SMACKER_POL_BETRAYALS_END);
        else
            PlaySmacker(EXPANSION_SMACKER_POL_AURORA_BOREALIS);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the nine
// local jump-table DIR32 fields at +0xab..+0xce. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd39d, 0xd9)
void ExpCampaign::ReplaySmacker2(void)
{
    switch (m_viewMap + 1) {
    case EXPANSION_MAP_DES_CONQUER_AND_UNIFY + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_INTRO);
        break;
    case EXPANSION_MAP_DES_BORDER_TOWNS + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_CONQUER_AND_UNIFY);
        break;
    case EXPANSION_MAP_DES_WAYWARD_SON + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BORDER_TOWNS);
        break;
    case EXPANSION_MAP_DES_UNCLE_IVAN + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BORDER_TOWNS);
        break;
    case EXPANSION_MAP_DES_SOUTHERN_WAR + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_FAMILY_REUNITED);
        break;
    case EXPANSION_MAP_DES_IVORY_GATES + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_SOUTHERN_WAR);
        break;
    case EXPANSION_MAP_DES_ELVEN_LANDS + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_SOUTHERN_WAR);
        break;
    case EXPANSION_MAP_DES_EPIC_BATTLE + 1:
        PlaySmacker(EXPANSION_SMACKER_DES_BRANCH_REUNITED);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the five
// local jump-table DIR32 fields at +0x8e..+0xa1. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd476, 0xac)
void ExpCampaign::ReplaySmacker3(void)
{
    switch (m_viewMap + 1) {
    case EXPANSION_MAP_WIZ_SHROUDED_ISLES + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_INTRO);
        break;
    case EXPANSION_MAP_WIZ_ETERNAL_SCROLLS + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_SHROUDED_ISLES);
        break;
    case EXPANSION_MAP_WIZ_POWERS_END + 1:
        PlaySmacker(EXPANSION_SMACKER_WIZ_ETERNAL_SCROLLS);
        break;
    case EXPANSION_MAP_WIZ_FOUNT_OF_WIZARDRY + 1:
        if (m_mapsPlayed[EXPANSION_MAP_WIZ_POWERS_END])
            PlaySmacker(EXPANSION_SMACKER_WIZ_POWERS_END);
        else
            PlaySmacker(EXPANSION_SMACKER_WIZ_ETERNAL_SCROLLS);
        break;
    }
}

// @early-stop
// Equal-sized raw code is identical after masking the five
// local jump-table DIR32 fields at +0x6f..+0x82. Retail delinks them as this
// function while our object uses $L labels; all PlaySmacker relocations match.
VA(0x004bd522, 0x88)
void ExpCampaign::ReplaySmacker4(void)
{
    switch (m_viewMap + 1) {
    case EXPANSION_MAP_VOY_STRANDED + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_INTRO);
        break;
    case EXPANSION_MAP_VOY_PIRATE_ISLES + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_STRANDED);
        break;
    case EXPANSION_MAP_VOY_KING_AND_COUNTRY + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_PIRATE_ISLES);
        break;
    case EXPANSION_MAP_VOY_BLOOD_IS_THICKER + 1:
        PlaySmacker(EXPANSION_SMACKER_VOY_PIRATE_ISLES);
    }
}

VA(0x004bd5aa, 0x59)
unsigned char ExpCampaign::IsCompleted(void) { return 0; }

VA(0x004bd603, 0x36)
signed char ExpCampaign::IsThisMapCompleted(void) { return 0; }

VA(0x004bd639, 0x32e)
int ExpCampaign::MessageHandler(struct tag_message &) { return 0; }

VA(0x004bd967, 0x6c)
void ExpCampaign::Autosave(void) {}

VA(0x004bd9d3, 0x25)
int ExpCampaign::Choose(void) { return 0; }

VA(0x004bd9f8, 0x64)
short int ExpCampaign::Days(void) { return 0; }

VA(0x004bda5c, 0x1b)
int ExpCampaign::CampaignID(void) { return 0; }

VA(0x004bda77, 0x3a)
char * ExpCampaign::JosephName(void) { return 0; }

VA(0x004bdab1, 0x3a)
char * ExpCampaign::IvanName(void) { return 0; }

VA(0x004bdaeb, 0x4e)
signed char ExpCampaign::IsSpecialGoldenBow(int, int) { return 0; }

VA(0x004bdb39, 0x38)
signed char ExpCampaign::IsSpecialUA(void) { return 0; }

VA(0x004bdb71, 0x73)
signed char ExpCampaign::IsSpecialLossCondition(int) { return 0; }
