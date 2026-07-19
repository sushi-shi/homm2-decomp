#include <va.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/message.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/SPELLS.h>
#include <SOURCE/townManager.h>
#include <SOURCE/TOWNMGR.h>
#include <SOURCE/X_GLOBAL.h>

H2_ENUM_BEGIN(HeroUiConstant)
    UI_STATUS_TEXT_WIDGET          = 0x12f,
    UI_ARTIFACT_SLOT_0             = 0x14,
    UI_ARTIFACT_SLOT_1             = 0x15,
    UI_ARTIFACT_SLOT_2             = 0x16,
    UI_ARTIFACT_SLOT_3             = 0x17,
    UI_ARTIFACT_SLOT_4             = 0x18,
    UI_ARTIFACT_SLOT_5             = 0x19,
    UI_ARTIFACT_SLOT_6             = 0x1a,
    UI_ARTIFACT_SLOT_7             = 0x1b,
    UI_ARTIFACT_SLOT_8             = 0x1c,
    UI_ARTIFACT_SLOT_9             = 0x1d,
    UI_ARTIFACT_SLOT_10            = 0x1e,
    UI_ARTIFACT_SLOT_11            = 0x1f,
    UI_ARTIFACT_SLOT_12            = 0x20,
    UI_ARTIFACT_SLOT_13            = 0x21,
    UI_ARTIFACT_FIRST              = UI_ARTIFACT_SLOT_0,
    UI_ARTIFACT_LAST               = UI_ARTIFACT_SLOT_13,
    UI_PRIMARY_STAT_ATTACK         = 0x51,
    UI_PRIMARY_STAT_DEFENSE        = 0x52,
    UI_PRIMARY_STAT_SPELL_POWER    = 0x53,
    UI_PRIMARY_STAT_KNOWLEDGE      = 0x54,
    UI_PRIMARY_STAT_FIRST          = UI_PRIMARY_STAT_ATTACK,
    UI_PRIMARY_STAT_LAST           = UI_PRIMARY_STAT_KNOWLEDGE,
    UI_ADDITIONAL_STATS            = 0x55,
    UI_ARMY_ICON_FIRST             = 0x57,
    UI_ARMY_PORTRAIT_FIRST         = 0x5c,
    UI_ARMY_COUNT_FIRST            = 0x61,
    UI_ARMY_SELECTOR_SLOT_0        = 0x66,
    UI_ARMY_SELECTOR_SLOT_1        = 0x67,
    UI_ARMY_SELECTOR_SLOT_2        = 0x68,
    UI_ARMY_SELECTOR_SLOT_3        = 0x69,
    UI_ARMY_SELECTOR_SLOT_4        = 0x6a,
    UI_ARMY_SELECTOR_FIRST         = UI_ARMY_SELECTOR_SLOT_0,
    UI_ARMY_SELECTOR_LAST          = UI_ARMY_SELECTOR_SLOT_4,
    UI_HERO_TITLE                  = 2,
    UI_HERO_PORTRAIT               = 0x41,
    UI_PRIMARY_STAT_VALUE_FIRST    = 0x4c,
    UI_PLAYER_CREST                = 0x56,
    UI_MORALE_FIRST                = 0xc8,
    UI_MORALE_MIDDLE               = 0xc9,
    UI_MORALE_LAST                 = 0xca,
    UI_LUCK_FIRST                  = 0xcb,
    UI_LUCK_MIDDLE                 = 0xcc,
    UI_LUCK_LAST                   = 0xcd,
    UI_EXPERIENCE_FIRST            = 0xce,
    UI_EXPERIENCE_LAST             = 0xcf,
    UI_SPELL_POINTS_FIRST          = 0xd3,
    UI_SPELL_POINTS_LAST           = 0xd4,
    UI_FORMATION_SPREAD            = 0xd6,
    UI_FORMATION_GROUPED           = 0xd8,
    UI_FORMATION_SPREAD_ICON       = 0xd5,
    UI_FORMATION_GROUPED_ICON      = 0xd7,
    UI_PREVIOUS_HERO               = 300,
    UI_NEXT_HERO                   = 301,
    UI_ARMY_SELECTION_NONE         = -1,
    UI_HERO_CYCLE_MIN_COUNT        = 2,
    UI_SINGLE_HERO_COUNT           = 1,
    UI_MODIFIER_ICON_COUNT         = 3,
    UI_MIN_MODIFIER_ICONS          = 1,
    UI_PREVIOUS_HERO_MASK          = -2,
    UI_SECONDARY_SKILL_ROW1_FIRST  = 0x190,
    UI_SECONDARY_SKILL_ROW1_LAST   = 0x197,
    UI_SECONDARY_SKILL_ROW2_FIRST  = 0x198,
    UI_SECONDARY_SKILL_ROW2_LAST   = 0x19f,
    UI_SECONDARY_SKILL_ROW3_FIRST  = 0x1a0,
    UI_SECONDARY_SKILL_ROW3_LAST   = 0x1a7,
    UI_CLOSE                       = 0x7800,
    UI_DISMISS                     = 0x7803,
    UI_QUICK_VIEW_MODIFIER         = 0x200,
    UI_SPLIT_MODIFIER_MASK         = 3,
    UI_DIALOG_CLOSE_COMMAND        = 10,
    UI_HANDLER_CONTINUE            = 1,
    UI_HANDLER_CLOSE               = 2,
    UI_VIEW_ARMY_X                 = 0x77,
    UI_VIEW_ARMY_Y                 = 0x14,
    UI_VIEW_SPELLS_ALL             = 2,
    UI_VIEW_SPELLS_SPECIAL         = 1,
    UI_CASTLE_DIALOG_ACTIVE        = 1,
    UI_ARMY_EMPTY_FRAME            = 2,
    UI_WIDGET_FRAME_ACTIVE         = 4,
    UI_ARTIFACT_DIALOG_ICON        = 0x1c,
    UI_ARMY_SLOT_COUNT             = 5,
    UI_SCREEN_WIDTH                = 640,
    UI_SCREEN_HEIGHT               = 480,
    UI_STATUS_REGION_Y             = 459,
    UI_STATUS_REGION_HEIGHT        = 20,
    UI_FADE_STEPS                  = 8,
    UI_FADE_IN                     = 0,
    UI_FADE_OUT                    = 1,
    UI_LOOPING_SOUND_KEEP_COUNT    = 4,
    UI_VIEW_CLOSED                 = 0,
    UI_VIEW_DISMISSED              = 1,
    UI_WINDOW_TEXT_ID              = 6,
    UI_BACKDROP_PALETTE            = 1,
    UI_DIALOG_DISMISS              = 0x7803,
    UI_DIALOG_SPLIT                = 0x7802,
    UI_SPLIT_WINDOW_X              = 0xb1,
    UI_SPLIT_WINDOW_Y              = 0x14,
    UI_SPLIT_TEXT                  = 1,
    UI_SPLIT_AMOUNT                = 68,
    UI_CONTROL_VALUE_DEFAULT       = 6,
    UI_CONTROL_FRAME_DEFAULT       = 4,
    UI_CYCLE_BUTTON_FRAME          = 2,
    UI_CYCLE_BUTTON_DISABLED_FRAME = 0x1000,
    UI_LUCK_NEGATIVE_FRAME         = 3,
    UI_LUCK_NEUTRAL_FRAME          = 6,
    UI_LUCK_POSITIVE_FRAME         = 2,
    UI_MORALE_NEGATIVE_FRAME       = 5,
    UI_MORALE_NEUTRAL_FRAME        = 7,
    UI_MORALE_POSITIVE_FRAME       = 4,
    UI_EMPTY_SKILL_FRAME           = 0,
    UI_EMPTY_ARTIFACT_FRAME        = 0,
    UI_ARTIFACT_CONTROL_VALUE      = 2,
    UI_ARMY_RACE_FRAME_OFFSET      = 4
H2_ENUM_END(HeroUiConstant)

H2_ENUM_CLASS_BEGIN(HeroScreenText)
    TEXT_PRIMARY_STAT      = 1,
    TEXT_ADDITIONAL_STATS  = 2,
    TEXT_GOOD_MORALE       = 3,
    TEXT_NEUTRAL_MORALE    = 4,
    TEXT_BAD_MORALE        = 5,
    TEXT_GOOD_LUCK         = 6,
    TEXT_NEUTRAL_LUCK      = 7,
    TEXT_BAD_LUCK          = 8,
    TEXT_EXPERIENCE        = 9,
    TEXT_SELECT_ARMY       = 10,
    TEXT_EMPTY             = 11,
    TEXT_MOVE_ARMY         = 12,
    TEXT_EXCHANGE_ARMIES   = 13,
    TEXT_VIEW_SPELLS       = 14,
    TEXT_ARTIFACT          = 15,
    TEXT_DISMISS           = 16,
    TEXT_EXIT              = 17,
    TEXT_SCREEN            = 18,
    TEXT_COMBINE_ARMIES    = 19,
    TEXT_SPLIT_ARMY        = 20,
    TEXT_SECONDARY_SKILL   = 21,
    TEXT_SPELL_POINTS      = 22,
    TEXT_SPREAD_FORMATION  = 23,
    TEXT_GROUPED_FORMATION = 24
H2_ENUM_CLASS_END(HeroScreenText)

H2_ENUM_CLASS_BEGIN(HeroSpellType)
    SPELL_TYPE_COMBAT    = 0,
    SPELL_TYPE_ADVENTURE = 1,
    SPELL_TYPE_ALL       = 2
H2_ENUM_CLASS_END(HeroSpellType)

H2_ENUM_BEGIN(HeroMobilityConstant)
    BASE_RECORD_SIZE              = 0xec,
    LAND_SPEED_COUNT              = 8,
    SLOWEST_LAND_SPEED            = LAND_SPEED_COUNT - 1,
    SEA_BASE_MOBILITY             = 1500,
    LIGHTHOUSE_MOBILITY_BONUS     = 500,
    ASTROLABE_MOBILITY_BONUS      = 1000,
    COMPASS_MOBILITY_BONUS        = 500,
    NOMAD_BOOTS_MOBILITY_BONUS    = 600,
    TRAVELER_BOOTS_MOBILITY_BONUS = 300,
    STABLES_MOBILITY_BONUS        = 400,
    AI_DIFFICULTY_MOBILITY_BONUS  = 75,
    AI_STATE_MOBILITY_BONUS       = 50,
    LIGHTHOUSE_MINE_TYPE          = 100,
    HARD_GAME_DIFFICULTY          = 2,
    HIGH_AI_DIFFICULTY            = 2
H2_ENUM_END(HeroMobilityConstant)

H2_ENUM_BEGIN(HeroImplementationConstant)
    EXPERIENCE_PREVIOUS_ENTRY_OFFSET = 2,
    TEMPLE_MORALE_BONUS              = 2,
    PYRAMID_LUCK_PENALTY             = 2
H2_ENUM_END(HeroImplementationConstant)

VA(0x0046c3a0, 0x6f)
hero::hero(void) {
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_cursorType = 0;
    m_portrait = 0;
    m_name[0] = 0;
    heroWin = NULL;
    giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
}

VA(0x0046c40f, 0x53)
void hero::Read(i32 file, i8 expansion) {
    if (expansion)
        read(file, this, sizeof(hero));
    else
        read(file, this, BASE_RECORD_SIZE);
}

VA(0x0046c462, 0x53)
void hero::Write(i32 file, i8 expansion) {
    if (expansion)
        write(file, this, sizeof(hero));
    else
        write(file, this, BASE_RECORD_SIZE);
}

VA(0x0046c4b5, 0x18)
void hero::GetArmyStrengths(u32l* const) {}

VA(0x0046c4cd, 0x59)
i32 hero::HasArtifact(ArtifactType artifact) {
    i32 artifactIndex;

    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == IDX(artifact))
            return 1;
    }
    return 0;
}

VA(0x0046c526, 0x277)
i32 hero::CalcMobility(void) {
    i16 landMobility[LAND_SPEED_COUNT] = {1000, 1000, 1000, 1100, 1200, 1300, 1400, 1500}; // NOLINT(readability-magic-numbers)
    const i16 seaBaseMobilityCurrent = SEA_BASE_MOBILITY;
    const i16 lighthouseBonusIncrement = LIGHTHOUSE_MOBILITY_BONUS;
    const i16 astrolabeBonus = ASTROLABE_MOBILITY_BONUS;
    const i16 compassMobility = COMPASS_MOBILITY_BONUS;
    const i16 nomadBootsMobilityBonus = NOMAD_BOOTS_MOBILITY_BONUS;
    const i16 travelerBonus = TRAVELER_BOOTS_MOBILITY_BONUS;
    i32 mobilityResult;
    i32 slowestSpeedValue;
    i32 armySlotIndex;

    if (HAS(m_eventFlags, HERO_EVENT_EMBARKED)) {
        mobilityResult = seaBaseMobilityCurrent;
        mobilityResult = static_cast<i32>(
            mobilityResult * gfSSNavigationMod[m_secondarySkills[IDX(HERO_SKILL_NAVIGATION)]]
        );
        if (m_owner != -1)
            mobilityResult += gpGame->MineTypesOwned(m_owner, LIGHTHOUSE_MINE_TYPE)
                              * lighthouseBonusIncrement;
        if (HasArtifact(ARTIFACT_SAILORS_ASTROLABE))
            mobilityResult += astrolabeBonus;
    } else {
        slowestSpeedValue = SLOWEST_LAND_SPEED;
        for (armySlotIndex = 0; armySlotIndex < ARMY_GROUP_SLOT_COUNT; armySlotIndex++) {
            if (m_army.m_creatureTypes[armySlotIndex] != ARMY_GROUP_EMPTY_SLOT
                && gMonsterDatabase[m_army.m_creatureTypes[armySlotIndex]].speed
                       < slowestSpeedValue) {
                slowestSpeedValue = gMonsterDatabase[m_army.m_creatureTypes[armySlotIndex]].speed;
            }
        }
        mobilityResult = landMobility[slowestSpeedValue];
        mobilityResult = static_cast<i32>(
            mobilityResult * gfSSLogisticsMod[m_secondarySkills[IDX(HERO_SKILL_LOGISTICS)]]
        );
        if (HasArtifact(ARTIFACT_NOMAD_BOOTS))
            mobilityResult += nomadBootsMobilityBonus;
        if (HasArtifact(ARTIFACT_TRAVELER_BOOTS))
            mobilityResult += travelerBonus;
        if (HAS(m_eventFlags, HERO_EVENT_STABLES))
            mobilityResult += STABLES_MOBILITY_BONUS;
    }

    if (HasArtifact(ARTIFACT_TRUE_COMPASS))
        mobilityResult += compassMobility;

    if (m_owner >= 0 && m_owner < IDX(GAME_PLAYER_COUNT) && !gbHumanPlayer[m_owner]
        && gpGame->m_difficulty >= HARD_GAME_DIFFICULTY) {
        mobilityResult += AI_DIFFICULTY_MOBILITY_BONUS;
        if (gpGame->m_players[m_owner].m_aiDifficulty == HIGH_AI_DIFFICULTY)
            mobilityResult += AI_STATE_MOBILITY_BONUS;
    }
    return mobilityResult;
}

VA(0x0046c79d, 0xcf)
i32 hero::HasSpell(SpellType spell) {
    i32 artifactIndex;

    if (!HasArtifact(ARTIFACT_MAGIC_BOOK))
        return 0;
    if (m_spells[IDX(spell)])
        return 1;
    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == IDX(ARTIFACT_SPELL_SCROLL)
            && m_artifactExtra[artifactIndex] == IDX(spell)) {
            return 1;
        }
    }
    if (HasArtifact(ARTIFACT_BATTLE_GARB) && spell == SPELL_TOWN_PORTAL)
        return 1;
    return 0;
}

VA(0x0046c86c, 0xc5)
SpellType hero::GetNthSpell(i32 type, i32 spellNumber) {
    i32 spell;
    i32 spellOrdinalCount = 0;

    for (spell = 0; spell < IDX(SPELL_COUNT); spell++) {
        if (HasSpell(SpellType(spell))) {
            if (type == IDX(SPELL_TYPE_ALL)
                || (type == IDX(SPELL_TYPE_COMBAT)
                    && HAS(gsSpellInfo[spell].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))
                || (type == IDX(SPELL_TYPE_ADVENTURE)
                    && !HAS(gsSpellInfo[spell].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))) {
                spellOrdinalCount++;
            }
            if (spellOrdinalCount == spellNumber)
                return SpellType(spell);
        }
    }
    return SPELL_NONE;
}

VA(0x0046c931, 0xd0)
i32 hero::GetNumSpells(i32 type) {
    i32 numAdventureSpells;
    i32 numCombatSpells;
    i32 spellIndexCurrent;

    numCombatSpells = 0;
    numAdventureSpells = 0;
    for (spellIndexCurrent = 0; spellIndexCurrent < IDX(SPELL_COUNT); spellIndexCurrent++) {
        if (HasSpell(SpellType(spellIndexCurrent))) {
            if (HAS(gsSpellInfo[spellIndexCurrent].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))
                numCombatSpells++;
            else
                numAdventureSpells++;
        }
    }

    switch (type) {
        case IDX(SPELL_TYPE_COMBAT):
            return numCombatSpells;
        case IDX(SPELL_TYPE_ADVENTURE):
            return numAdventureSpells;
        case IDX(SPELL_TYPE_ALL):
            return numCombatSpells + numAdventureSpells;
    }
    return 0;
}

VA(0x0046ca01, 0x8a)
void hero::UseSpell(SpellType spell) {
    if (spell == SPELL_NONE)
        return;

    m_spellPoints -= GetManaCost(spell, this);
    if (m_spellPoints < 0)
        m_spellPoints = 0;
    if (gpAdvManager->m_active && gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->UpdateHeroLocator(-1, 1, 1);
}

VA(0x0046ca8b, 0x26)
void hero::AddSpell(i32 spell, i32) {
    m_spells[spell] = 1;
}

VA(0x0046cab1, 0x82)
void HeroMessageUpdate(char* text) {
    tag_message message;

    if (gheroWin == NULL)
        return;

    message.type = HERO_UI_MESSAGE;
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = UI_STATUS_TEXT_WIDGET;
    message.payload.widget.data.text = text;
    gheroWin->BroadcastMessage(message);
    gheroWin->DrawWindow(0, UI_PREVIOUS_HERO, UI_STATUS_TEXT_WIDGET);
    gpWindowManager->UpdateScreenRegion(
        0, UI_STATUS_REGION_Y, UI_SCREEN_WIDTH, UI_STATUS_REGION_HEIGHT
    );
}

VA(0x0046cb33, 0xa8)
void hero::HeroScreenUpdate(void) {
    tag_message message;
    i32 index;

    message.type = HERO_UI_MESSAGE;
    UpdateArmies();
    for (index = 0; index < UI_ARMY_SLOT_COUNT; index++) {
        if (index == OD_STEER(giHeroScreenSrcIndex))
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        else
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
        message.payload.widget.id = index + UI_ARMY_SELECTOR_FIRST;
        heroWin->BroadcastMessage(message);
    }
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, UI_SCREEN_WIDTH, UI_SCREEN_HEIGHT);
}

VA(0x0046cbdb, 0x1d2)
void hero::UpdateArmies(void) {
    tag_message message;
    i32 index;

    message.type = HERO_UI_MESSAGE;
    for (index = 0; index < UI_ARMY_SLOT_COUNT; index++) {
        if (m_army.m_creatureTypes[index] == ARMY_GROUP_EMPTY_SLOT) {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = index + UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value = UI_ARMY_EMPTY_FRAME;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id = index + UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = index + UI_ARMY_COUNT_FIRST;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = index + UI_ARMY_SELECTOR_FIRST;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = index + UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value =
                gMonsterDatabase[m_army.m_creatureTypes[index]].race
                + UI_ARMY_RACE_FRAME_OFFSET;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
            sprintf(gText, "monh%04d.icn", m_army.m_creatureTypes[index]);
            message.payload.widget.id = index + UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);

            sprintf(gText, "%d", m_army.m_creatureCounts[index]);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id = index + UI_ARMY_COUNT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
        }
    }
}

VA(0x0046cdad, 0x43)
void hero::ViewStat(i32 stat, i32 quickView) {
    NormalDialog(
        gStatDesc[stat],
        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        -1,
        -1,
        -1,
        0,
        -1,
        0,
        -1,
        0
    );
}

VA(0x0046cdf0, 0x9b)
void hero::ViewArtifact(ArtifactType artifact, b32 quickView, i32 extra) {
    if (artifact == ARTIFACT_SPELL_SCROLL) {
        sprintf(gText, gArtifactDesc[IDX(artifact)], gSpellNames[extra]);
        NormalDialog(
            gText,
            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
            -1,
            UI_ARTIFACT_DIALOG_ICON,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
    } else {
        NormalDialog(
            gArtifactDesc[IDX(artifact)],
            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
            -1,
            UI_ARTIFACT_DIALOG_ICON,
            -1,
            0,
            -1,
            0,
            -1,
            0
        );
    }
}

VA(0x0046ce8b, 0x5d)
i32 hero::Dismiss(void) {
    NormalDialog(
        "Are you sure you want to dismiss this Hero?",
        NORMAL_DIALOG_CONFIRM,
        -1,
        -1,
        -1,
        0,
        -1,
        0,
        -1,
        0
    );
    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
        Deallocate(1);
        return 1;
    }
    return 0;
}

VA(0x0046cee8, 0x587)
void hero::Deallocate(i32 updateMap) {
    i32 availableHeroSlotCurrent;
    playerData* player;
    i32 playerHeroIndex;
    i32 heroOwner;
    i32 index;
    town* occupiedTownValue;
    mapCell* mapCellRecord;

    if (updateMap)
        SendMapChange(
            IDX(MAP_CHANGE_DEAD_HERO),
            m_id,
            static_cast<u8>(m_x),
            static_cast<u8>(m_y),
            HERO_MAP_CHANGE_UNUSED,
            0,
            0
        );

    heroOwner = m_owner;
    player = &gpGame->m_players[m_owner];

    if (updateMap)
        gpAdvManager->MobilizeCurrHero(0);
    if (updateMap)
        gpAdvManager->HideRoute(0, 0, 0);

    if (HAS(m_eventFlags, HERO_EVENT_EMBARKED)) {
        for (index = 0; index < GAME_BOAT_COUNT; index++) {
            if (gpGame->m_boats[index].heroId == m_id) {
                gpGame->m_boats[index].heroId = -1;
                gpGame->m_boatSlots[index] = -1;
            }
        }
    }

    if (m_locationType == HERO_LOCATION_TOWN) {
        occupiedTownValue = &gpGame->m_castleRecs[m_occupiedTown];
        occupiedTownValue->m_occupyingHeroId = -1;
    }

    if (m_owner != giCurPlayer || gpGame->m_players[m_owner].m_currentHero != m_id
        || gpAdvManager->m_heroContextLocked == 0) {
        gpGame->RestoreCell(m_x, m_y, m_locationType, m_occupiedTown, NULL, 1);
    }

    if (!gbCombatSurrender) {
        for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++)
            m_army.Dismiss(index);
    }

    playerHeroIndex = -1;
    for (index = 0; index < player->m_heroCount; index++) {
        if (player->m_heroIds[index] == m_id)
            playerHeroIndex = index;
    }
    for (index = playerHeroIndex; index < player->m_heroCount - 1; index++)
        player->m_heroIds[index] = player->m_heroIds[index + 1];
    player->m_heroIds[player->m_heroCount - 1] = -1;

    if (player->m_currentHero == m_id) {
        player->m_currentHero = -1;
        if (m_owner == giCurPlayer) {
            gpAdvManager->m_cursorActive = 0;
            mapCellRecord = gpGame->m_worldMap.GetCell(m_x, m_y);
            mapCellRecord->m_flags &= ~HERO_MAP_CELL_PRESENT;
        }
        if (giCurPlayer == heroOwner)
            gpAdvManager->m_heroContextLocked = 0;
    }

    player->m_heroCount--;
    player->m_heroLocatorPage = 0;
    gpGame->m_availableHeroes[m_id] = HERO_AVAILABILITY_UNAVAILABLE;

    if (gbRetreatWin) {
        availableHeroSlotCurrent = Random(0, HERO_AVAILABLE_SLOT_COUNT - 1);
        if (HAS(gpGame
                    ->m_heroRecs[gpGame->m_players[m_owner]
                                     .m_availableHeroIds[availableHeroSlotCurrent]]
                    .m_eventFlags,
                HERO_EVENT_WEEKLY_VISIT)) {
            availableHeroSlotCurrent = 1 - availableHeroSlotCurrent;
        }
        if (gpGame->m_availableHeroes[gpGame->m_players[m_owner]
                                          .m_availableHeroIds[availableHeroSlotCurrent]]
            == HERO_AVAILABILITY_RETREATED) {
            gpGame->m_availableHeroes[gpGame->m_players[m_owner]
                                          .m_availableHeroIds[availableHeroSlotCurrent]] =
                HERO_AVAILABILITY_UNAVAILABLE;
        }
        gpGame->m_players[m_owner].m_availableHeroIds[availableHeroSlotCurrent] = m_id;
        gpGame->m_availableHeroes[m_id] = HERO_AVAILABILITY_RETREATED;
        m_eventFlags = m_eventFlags | HERO_EVENT_WEEKLY_VISIT;
    }

    m_eventFlags = m_eventFlags & ~HERO_EVENT_GROUPED_FORMATION;
    m_owner = HERO_OWNER_NONE;
    m_destinationY = HERO_DESTINATION_NONE;
    m_destinationX = m_destinationY;

    if (!gbCombatSurrender)
        gpGame->SetRandomHeroArmies(m_id, RANDOM_HERO_NORMAL_ARMY);

    if (gbInCampaign && m_portrait == IDX(CAMPAIGN_HERO_CORLAGON)
        && gpGame->m_campaignType == IDX(CAMPAIGN_ROLAND)
        && gpGame->m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO && !gbRetreatWin
        && !gbCombatSurrender) {
        gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)] = 1;
    }

    if (updateMap)
        CheckEndGame(0, 0);
}

VA(0x0046d46f, 0x9e)
i32 hero::GetExperience(i32 level) {
    i32 experience;
    i32 levelCounter;
    i32 increment;

    if (level <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT)
        return gMinExpForLevel[level - 1];

    levelCounter = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    increment = static_cast<i32>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1]
         - gMinExpForLevel
               [HERO_EXPERIENCE_LEVEL_TABLE_COUNT - EXPERIENCE_PREVIOUS_ENTRY_OFFSET])
        * HERO_EXPERIENCE_GROWTH_FACTOR
    );
    experience = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + increment;
    while (levelCounter < level) {
        increment = static_cast<i32>(increment * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience += increment;
        levelCounter++;
    }
    return experience;
}

VA(0x0046d50d, 0xc0)
i32 hero::GetLevel(i32 experienceValue) {
    i32 experience;
    i32 levelCounter;
    i32 increment;

    for (levelCounter = 1; levelCounter <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT; levelCounter++) {
        if (experienceValue < gMinExpForLevel[levelCounter - 1])
            return levelCounter - 1;
    }

    increment = static_cast<i32>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1]
         - gMinExpForLevel
               [HERO_EXPERIENCE_LEVEL_TABLE_COUNT - EXPERIENCE_PREVIOUS_ENTRY_OFFSET])
        * HERO_EXPERIENCE_GROWTH_FACTOR
    );
    experience = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + increment;
    levelCounter = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    while (experience < experienceValue) {
        increment = static_cast<i32>(increment * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience += increment;
        levelCounter++;
    }
    return levelCounter - 1;
}

VA(0x0046d5cd, 0x254)
void hero::ApplyBattleWinTemps(void) {
    m_lastTownInteractionTurn = HERO_INTERACTION_TURN_NONE;
    m_lastHeroInteractionTurn = HERO_INTERACTION_TURN_NONE;

    if (HAS(m_eventFlags, HERO_EVENT_GRAVEYARD)) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_GRAVEYARD;
    }
    if (HAS(m_eventFlags, HERO_EVENT_SHIPWRECK)) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_SHIPWRECK;
    }
    if (HAS(m_eventFlags, HERO_EVENT_BUOY)) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_BUOY;
    }
    if (HAS(m_eventFlags, HERO_EVENT_OASIS)) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_OASIS;
    }
    if (HAS(m_eventFlags, HERO_EVENT_TEMPLE)) {
        m_morale -= TEMPLE_MORALE_BONUS;
        m_eventFlags = m_eventFlags - HERO_EVENT_TEMPLE;
    }
    if (HAS(m_eventFlags, HERO_EVENT_FAERIE_RING)) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_FAERIE_RING;
    }
    if (HAS(m_eventFlags, HERO_EVENT_IDOL)) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_IDOL;
    }
    if (HAS(m_eventFlags, HERO_EVENT_FOUNTAIN)) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_FOUNTAIN;
    }
    if (HAS(m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_WATERING_HOLE;
    }
    if (HAS(m_eventFlags, HERO_EVENT_DERELICT_SHIP)) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_DERELICT_SHIP;
    }
    if (HAS(m_eventFlags, HERO_EVENT_PYRAMID)) {
        m_luck += PYRAMID_LUCK_PENALTY;
        m_eventFlags = m_eventFlags - HERO_EVENT_PYRAMID;
    }
    if (HAS(m_eventFlags, HERO_EVENT_MERMAID)) {
        m_luck = m_luck - 1;
        m_eventFlags = m_eventFlags - HERO_EVENT_MERMAID;
    }
}

VA(0x0046d821, 0x1e)
void hero::ApplyBattleLossTemps(void) {
    ApplyBattleWinTemps();
}

VA(0x0046d83f, 0x828)
void hero::CheckLevel(void) {
    i32 statBonuses[HERO_PRIMARY_STAT_COUNT];
    char line[HERO_LEVEL_TEXT_BUFFER_SIZE];
    i32 levelsGained;
    i32 newLevel;
    i32 attempts;
    i32 skillChoicesResult[HERO_SECONDARY_SKILL_CHOICE_COUNT];
    i32 highLevelIndex;
    i32 indexValue;
    SAMPLE2 sampleValue;
    i32 currentLevelIndex;
    i32 skillIndexValue;
    i32 randomValue;
    i32 skillWeightIndex;

    newLevel = GetLevel(m_experience);
    if (m_level == newLevel) {
    } else {
        sampleValue = NULL_SAMPLE2;
        levelsGained = newLevel - m_level;
        for (currentLevelIndex = m_level + 1; currentLevelIndex <= newLevel; currentLevelIndex++) {
            sprintf(gText, cHeroLevel[0], m_name);
            sprintf(line, cHeroLevel[1]);
            strcat(gText, line);

            statBonuses[IDX(HERO_PRIMARY_ATTACK)] = 0;
            statBonuses[IDX(HERO_PRIMARY_DEFENSE)] = 0;
            statBonuses[IDX(HERO_PRIMARY_SPELL_POWER)] = 0;
            statBonuses[IDX(HERO_PRIMARY_KNOWLEDGE)] = 0;
            if (currentLevelIndex <= HERO_LEVEL_HIGH_THRESHOLD)
                highLevelIndex = 0;
            else
                highLevelIndex = 1;

            SRand(m_randomSeed + currentLevelIndex * HERO_LEVEL_RANDOM_SEED_FACTOR);
            randomValue = SRandom(1, HERO_LEVEL_RANDOM_MAX);
            if (randomValue
                < gHeroSkillBonus[m_cursorType][highLevelIndex][IDX(HERO_PRIMARY_ATTACK)]) {
                statBonuses[IDX(HERO_PRIMARY_ATTACK)]++;
            } else {
                randomValue -=
                    gHeroSkillBonus[m_cursorType][highLevelIndex][IDX(HERO_PRIMARY_ATTACK)];
                if (randomValue
                    < gHeroSkillBonus[m_cursorType][highLevelIndex][IDX(HERO_PRIMARY_DEFENSE)]) {
                    statBonuses[IDX(HERO_PRIMARY_DEFENSE)]++;
                } else {
                    randomValue -=
                        gHeroSkillBonus[m_cursorType][highLevelIndex][IDX(HERO_PRIMARY_DEFENSE)];
                    if (randomValue < gHeroSkillBonus[m_cursorType][highLevelIndex]
                                                     [IDX(HERO_PRIMARY_SPELL_POWER)]) {
                        statBonuses[IDX(HERO_PRIMARY_SPELL_POWER)]++;
                    } else {
                        statBonuses[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
                    }
                }
            }

            for (indexValue = 0; indexValue < HERO_PRIMARY_STAT_COUNT; indexValue++) {
                if (statBonuses[indexValue] > 0) {
                    m_primaryStats[indexValue] =
                        m_primaryStats[indexValue] + statBonuses[indexValue];
                    sprintf(line, "\n%s +%d", gStatNames[indexValue], statBonuses[indexValue]);
                    strcat(gText, line);
                }
            }

            for (indexValue = 0; indexValue < HERO_SECONDARY_SKILL_CHOICE_COUNT; indexValue++) {
                skillChoicesResult[indexValue] = HERO_SECONDARY_SKILL_NONE;
                if (indexValue == 0 && m_cursorType != IDX(FACTION_BARBARIAN)
                    && m_cursorType != IDX(FACTION_KNIGHT)
                    && m_secondarySkills[IDX(HERO_SKILL_WISDOM)] < IDX(HERO_SKILL_LEVEL_EXPERT)
                    && currentLevelIndex - m_enabled >= HERO_SECONDARY_SKILL_OFFER_GAP) {
                    skillChoicesResult[indexValue] = IDX(HERO_SKILL_WISDOM);
                } else {
                    attempts = 0;
                    skillWeightIndex = Random(0, HERO_SECONDARY_SKILL_RANDOM_WEIGHT);
                    skillIndexValue = 0;
                    while (attempts < HERO_SECONDARY_SKILL_SEARCH_LIMIT) {
                        attempts++;
                        if ((indexValue == 0 || skillChoicesResult[0] != skillIndexValue)
                            && ((m_secondarySkills[skillIndexValue] != IDX(HERO_SKILL_LEVEL_NONE)
                                 && m_secondarySkills[skillIndexValue]
                                        < IDX(HERO_SKILL_LEVEL_EXPERT))
                                || (m_secondarySkills[skillIndexValue] == IDX(HERO_SKILL_LEVEL_NONE)
                                    && m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY))) {
                            skillWeightIndex -= iGetSSByAlignment[skillIndexValue][m_cursorType];
                            if (skillWeightIndex <= 0) {
                                skillChoicesResult[indexValue] = skillIndexValue;
                                break;
                            }
                        }
                        skillIndexValue = (skillIndexValue + 1) % IDX(HERO_SKILL_COUNT);
                    }
                    attempts--;
                }
            }

            if (skillChoicesResult[0] == IDX(HERO_SKILL_WISDOM)
                || skillChoicesResult[1] == IDX(HERO_SKILL_WISDOM)) {
                m_enabled = static_cast<u8>(currentLevelIndex);
            }

            if (!gbInNewGameSetup && m_owner >= 0 && gbThisNetHumanPlayer[m_owner]) {
                sampleValue = LoadPlaySample(const_cast<char*>("nwherolv.82m"));
                if (skillChoicesResult[0] == HERO_SECONDARY_SKILL_NONE) {
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                } else if (skillChoicesResult[1] == HERO_SECONDARY_SKILL_NONE) {
                    sprintf(
                        line,
                        "\n\nYou have learned %s %s.",
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[0]]],
                        gSecondarySkills[skillChoicesResult[0]]
                    );
                    strcat(gText, line);
                    NormalDialog(
                        gText,
                        NORMAL_DIALOG_INFO,
                        -1,
                        -1,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[0]]
                            + skillChoicesResult[0] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        -1,
                        0,
                        -1,
                        0
                    );
                    GiveSS(skillChoicesResult[0], 1);
                } else {
                    sprintf(
                        line,
                        "\n\nYou may learn either %s %s or %s %s.",
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[0]]],
                        gSecondarySkills[skillChoicesResult[0]],
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[1]]],
                        gSecondarySkills[skillChoicesResult[1]]
                    );
                    strcat(gText, line);
                    NormalDialog(
                        gText,
                        NORMAL_DIALOG_DISABLE_SEVENTH,
                        -1,
                        -1,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[0]]
                            + skillChoicesResult[0] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[1]]
                            + skillChoicesResult[1] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        -1,
                        0
                    );
                    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_SEVEN)
                        GiveSS(skillChoicesResult[0], 1);
                    else
                        GiveSS(skillChoicesResult[1], 1);
                }
            } else {
                if (skillChoicesResult[0] != HERO_SECONDARY_SKILL_NONE) {
                    if (skillChoicesResult[1] != HERO_SECONDARY_SKILL_NONE) {
                        if (gSSValues[skillChoicesResult[1]][0]
                            < gSSValues[skillChoicesResult[0]][0]) {
                            GiveSS(skillChoicesResult[0], 1);
                        } else {
                            GiveSS(skillChoicesResult[1], 1);
                        }
                    } else {
                        GiveSS(skillChoicesResult[0], 1);
                    }
                }
            }
        }
        m_level = static_cast<i16>(newLevel);
        WaitEndSample(sampleValue, -1);
    }
}

VA(0x0046e067, 0x57)
i32 hero::NumArtifacts(void) {
    i32 cnt = 0;
    i32 i;

    for (i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (m_artifacts[i] >= 0)
            cnt++;
    }
    return cnt;
}

VA(0x0046e0be, 0x758)
void UpdateHeroScreenStatusBar(struct tag_message& message) {
    i32 armySlot;
    i32 secondarySkillSlot;

    switch (message.payload.widget.id) {
        case UI_PRIMARY_STAT_ATTACK:
        case UI_PRIMARY_STAT_DEFENSE:
        case UI_PRIMARY_STAT_SPELL_POWER:
        case UI_PRIMARY_STAT_KNOWLEDGE:
            sprintf(
                gText,
                cHeroScreen[IDX(TEXT_PRIMARY_STAT)],
                gStatNames[message.payload.widget.id - UI_PRIMARY_STAT_FIRST]
            );
            break;

        case UI_ADDITIONAL_STATS:
            sprintf(gText, cHeroScreen[IDX(TEXT_ADDITIONAL_STATS)]);
            break;

        case UI_MORALE_FIRST:
        case UI_MORALE_MIDDLE:
        case UI_MORALE_LAST:
            if (gpHVHero->m_army.GetMorale(gpHVHero, gpHVHero->GetOccupiedTown(), NULL) > 0)
                sprintf(gText, cHeroScreen[IDX(TEXT_GOOD_MORALE)]);
            else if (gpHVHero->m_army.GetMorale(gpHVHero, gpHVHero->GetOccupiedTown(), NULL) == 0)
                sprintf(gText, cHeroScreen[IDX(TEXT_NEUTRAL_MORALE)]);
            else
                sprintf(gText, cHeroScreen[IDX(TEXT_BAD_MORALE)]);
            break;

        case UI_LUCK_FIRST:
        case UI_LUCK_MIDDLE:
        case UI_LUCK_LAST:
            if (gpGame->GetLuck(gpHVHero, NULL, gpHVHero->GetOccupiedTown()) > 0)
                sprintf(gText, cHeroScreen[IDX(TEXT_GOOD_LUCK)]);
            else if (gpGame->GetLuck(gpHVHero, NULL, gpHVHero->GetOccupiedTown()) == 0)
                sprintf(gText, cHeroScreen[IDX(TEXT_NEUTRAL_LUCK)]);
            else
                sprintf(gText, cHeroScreen[IDX(TEXT_BAD_LUCK)]);
            break;

        case UI_EXPERIENCE_FIRST:
        case UI_EXPERIENCE_LAST:
            sprintf(gText, cHeroScreen[IDX(TEXT_EXPERIENCE)]);
            break;

        case UI_SPELL_POINTS_FIRST:
        case UI_SPELL_POINTS_LAST:
            sprintf(gText, cHeroScreen[IDX(TEXT_SPELL_POINTS)]);
            break;

        case UI_FORMATION_SPREAD:
            sprintf(gText, cHeroScreen[IDX(TEXT_SPREAD_FORMATION)]);
            break;

        case UI_FORMATION_GROUPED:
            sprintf(gText, cHeroScreen[IDX(TEXT_GROUPED_FORMATION)]);
            break;

        case UI_ARMY_SELECTOR_SLOT_0:
        case UI_ARMY_SELECTOR_SLOT_1:
        case UI_ARMY_SELECTOR_SLOT_2:
        case UI_ARMY_SELECTOR_SLOT_3:
        case UI_ARMY_SELECTOR_SLOT_4:
            armySlot = message.payload.widget.id - UI_ARMY_SELECTOR_FIRST;
            if (giHeroScreenSrcIndex == UI_ARMY_SELECTION_NONE) {
                if (gpHVHero->m_army.m_creatureTypes[armySlot] != ARMY_GROUP_EMPTY_SLOT)
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                        gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]
                    );
                else
                    strcpy(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            } else if (giHeroScreenSrcIndex == armySlot) {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]
                );
            } else if (gpTownManager->m_castleDialogActive != 0) {
                if (gpHVHero->m_army.m_creatureTypes[armySlot] != ARMY_GROUP_EMPTY_SLOT)
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                        gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]
                    );
                else
                    strcpy(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            } else if (gpHVHero->m_army.m_creatureTypes[armySlot] == ARMY_GROUP_EMPTY_SLOT) {
                if (message.payload.widget.parameter & UI_SPLIT_MODIFIER_MASK)
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SPLIT_ARMY)],
                        gArmyNamesPlural[gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]]
                    );
                else
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_MOVE_ARMY)],
                        gArmyNames[gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]]
                    );
            } else if (gpHVHero->m_army.m_creatureTypes[armySlot]
                       == gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]) {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_COMBINE_ARMIES)],
                    gArmyNamesPlural[gpHVHero->m_army.m_creatureTypes[armySlot]]
                );
            } else {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_EXCHANGE_ARMIES)],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]
                );
            }
            break;

        case UI_ARTIFACT_SLOT_0:
        case UI_ARTIFACT_SLOT_1:
        case UI_ARTIFACT_SLOT_2:
        case UI_ARTIFACT_SLOT_3:
        case UI_ARTIFACT_SLOT_4:
        case UI_ARTIFACT_SLOT_5:
        case UI_ARTIFACT_SLOT_6:
        case UI_ARTIFACT_SLOT_7:
        case UI_ARTIFACT_SLOT_8:
        case UI_ARTIFACT_SLOT_9:
        case UI_ARTIFACT_SLOT_10:
        case UI_ARTIFACT_SLOT_11:
        case UI_ARTIFACT_SLOT_12:
        case UI_ARTIFACT_SLOT_13:
            if (gpHVHero->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST]
                == IDX(ARTIFACT_NONE))
                sprintf(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            else if (gpHVHero->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST]
                     == IDX(ARTIFACT_MAGIC_BOOK))
                strcpy(gText, cHeroScreen[IDX(TEXT_VIEW_SPELLS)]);
            else
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_ARTIFACT)],
                    gArtifactNames
                        [gpHVHero->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST]]
                );
            break;

        case UI_DISMISS:
            sprintf(
                gText,
                cHeroScreen[IDX(TEXT_DISMISS)],
                gpHVHero->m_name,
                gAlignmentNames[gpHVHero->m_cursorType]
            );
            break;

        case UI_CLOSE:
            strcpy(gText, cHeroScreen[IDX(TEXT_EXIT)]);
            break;

        default:
            if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW1_FIRST
                && message.payload.widget.id < UI_SECONDARY_SKILL_ROW2_FIRST) {
                secondarySkillSlot = message.payload.widget.id - UI_SECONDARY_SKILL_ROW1_FIRST;
                goto secondary_skill_text;
            }
            if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW2_FIRST
                && message.payload.widget.id < UI_SECONDARY_SKILL_ROW3_FIRST) {
                secondarySkillSlot = message.payload.widget.id - UI_SECONDARY_SKILL_ROW2_FIRST;
                goto secondary_skill_text;
            }
            if (message.payload.widget.id < UI_SECONDARY_SKILL_ROW3_FIRST
                || message.payload.widget.id >= UI_SECONDARY_SKILL_ROW3_LAST + 1)
                goto default_hero_text;
            secondarySkillSlot = message.payload.widget.id - UI_SECONDARY_SKILL_ROW3_FIRST;

        secondary_skill_text:
            if (secondarySkillSlot < gpHVHero->m_secondarySkillCount) {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_SECONDARY_SKILL)],
                    gSecondarySkillLevels
                        [gpHVHero->m_secondarySkills[gpHVHero->GetNthSS(secondarySkillSlot)] - 1],
                    gSecondarySkills[gpHVHero->GetNthSS(secondarySkillSlot)]
                );
                break;
            }
        default_hero_text:
            strcpy(gText, cHeroScreen[IDX(TEXT_SCREEN)]);
            break;
    }
    HeroMessageUpdate(gText);
}

VA(0x0046e816, 0xaef)
i32 HeroHandler(struct tag_message& message) {
    i32 handlerValue16;
    i32 temp1;
    i32 armySlot7;
    i32 exitHero36 = 0;
    i32 secondarySkillSlot18;
    i32 quickView0;
    i32 heroPosition5;
    i32 nextExperience12;
    i32 level14;

    if (message.payload.widget.parameter & UI_QUICK_VIEW_MODIFIER)
        quickView0 = 1;
    else
        quickView0 = 0;

    if (message.type == HERO_UI_HOVER) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
            return UI_HANDLER_CONTINUE;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;
        UpdateHeroScreenStatusBar(message);
        return UI_HANDLER_CONTINUE;
    }

    if (message.type == MESSAGE_KEY_UP) {
        switch (message.payload.keyboard.keyCode) {
            case INPUT_SCAN_LEFT_SHIFT:
            case INPUT_SCAN_RIGHT_SHIFT:
                gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
                gpInputManager->ForceMouseMove();
                break;
        }
    }

    if (message.type == MESSAGE_KEY_DOWN) {
        switch (message.payload.keyboard.keyCode) {
            case INPUT_SCAN_LEFT_SHIFT:
            case INPUT_SCAN_RIGHT_SHIFT:
                gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
                gpInputManager->ForceMouseMove();
                break;
        }
    }

    if (message.type == HERO_UI_MESSAGE) {
        switch (message.payload.widget.command) {
            case HERO_UI_INPUT_DESELECT:
                if (quickView0 == 0) {
                    switch (message.payload.widget.id) {
                        case UI_DISMISS:
                            if (gpHVHero->Dismiss())
                                exitHero36 = 1;
                            break;
                        case UI_CLOSE:
                            exitHero36 = 1;
                            break;
                        case UI_PREVIOUS_HERO:
                        case UI_NEXT_HERO: {
                            if (gpHVHero->m_owner != giCurPlayer) {
                            } else {
                                if (gpCurPlayer->m_heroCount <= UI_HERO_CYCLE_MIN_COUNT - 1) {
                                } else {
                                    heroPosition5 =
                                        gpGame->HeroIDToHeroPos(gpCurPlayer, gpHVHero->m_id);
                                    heroPosition5 =
                                        ((static_cast<u32>(
                                              message.payload.widget.id - UI_PREVIOUS_HERO
                                          ) >= 1
                                              ? 1
                                              : -1)
                                         + gpCurPlayer->m_heroCount + heroPosition5)
                                        % gpCurPlayer->m_heroCount;
                                    gpHVHero =
                                        &gpGame->m_heroRecs[gpCurPlayer->m_heroIds[heroPosition5]];
                                    SetupHeroView();
                                    RedrawHeroScreen();
                                }
                            }
                            break;
                        }
                    }
                }
                break;

            case HERO_UI_INPUT_SELECT:
            case HERO_UI_INPUT_ALTERNATE_SELECT:
                switch (message.payload.widget.id) {
                    case UI_PRIMARY_STAT_ATTACK:
                    case UI_PRIMARY_STAT_DEFENSE:
                    case UI_PRIMARY_STAT_SPELL_POWER:
                    case UI_PRIMARY_STAT_KNOWLEDGE:
                        gpHVHero->ViewStat(
                            message.payload.widget.id - UI_PRIMARY_STAT_FIRST,
                            quickView0
                        );
                        break;

                    case UI_MORALE_FIRST:
                    case UI_MORALE_MIDDLE:
                    case UI_MORALE_LAST:
                        gpGame->ShowMoraleInfo(
                            gpHVHero,
                            quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW
                        );
                        break;

                    case UI_LUCK_FIRST:
                    case UI_LUCK_MIDDLE:
                    case UI_LUCK_LAST:
                        gpGame->ShowLuckInfo(
                            gpHVHero,
                            quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW
                        );
                        break;

                    case UI_FORMATION_SPREAD:
                        if (quickView0) {
                            NormalDialog(
                                "{Spread Formation}\n\n'Spread' combat formation spreads your "
                                "armies from the top to the bottom of the battlefield, with at "
                                "least one empty space between each army.",
                                NORMAL_DIALOG_QUICK_VIEW,
                                NORMAL_DIALOG_NO_RESOURCE,
                                NORMAL_DIALOG_NO_VALUE,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0
                            );
                        } else {
                            gpHVHero->m_eventFlags &= ~HERO_EVENT_GROUPED_FORMATION;
                            SetupHeroView();
                            RedrawHeroScreen();
                        }
                        break;

                    case UI_FORMATION_GROUPED:
                        if (quickView0) {
                            NormalDialog(
                                "{Grouped Formation}\n\n'Grouped' combat formation bunches your "
                                "army together in the center of your side of the battlefield.",
                                NORMAL_DIALOG_QUICK_VIEW,
                                NORMAL_DIALOG_NO_RESOURCE,
                                NORMAL_DIALOG_NO_VALUE,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0,
                                NORMAL_DIALOG_NO_RESOURCE,
                                0
                            );
                        } else {
                            gpHVHero->m_eventFlags |= HERO_EVENT_GROUPED_FORMATION;
                            SetupHeroView();
                            RedrawHeroScreen();
                        }
                        break;

                    case UI_SPELL_POINTS_FIRST:
                    case UI_SPELL_POINTS_LAST:
                        sprintf(
                            gText,
                            "{Spell Points}\n\n%s currently has %d spell points out of a maximum "
                            "of %d.  The maximum number of spell points is 10 times your "
                            "knowledge.  It is occasionally possible to have more than your "
                            "maximum spell points via special events.",
                            gpHVHero->m_name,
                            gpHVHero->m_spellPoints,
                            gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                                * HERO_SPELL_POINTS_PER_KNOWLEDGE
                        );
                        NormalDialog(
                            gText,
                            quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                        break;

                    case UI_EXPERIENCE_FIRST:
                    case UI_EXPERIENCE_LAST: {
                        level14 = gpHVHero->GetLevel(gpHVHero->m_experience);
                        nextExperience12 = gpHVHero->GetExperience(level14 + 1);
                        sprintf(
                            gText,
                            "{Level %d}\n\nCurrent experience %d\nNext level %d",
                            level14,
                            gpHVHero->m_experience,
                            nextExperience12
                        );
                        NormalDialog(
                            gText,
                            quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                            NORMAL_DIALOG_NO_RESOURCE,
                            NORMAL_DIALOG_NO_VALUE,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0,
                            NORMAL_DIALOG_NO_RESOURCE,
                            0
                        );
                        break;
                    }

                    case UI_ARMY_SELECTOR_SLOT_0:
                    case UI_ARMY_SELECTOR_SLOT_1:
                    case UI_ARMY_SELECTOR_SLOT_2:
                    case UI_ARMY_SELECTOR_SLOT_3:
                    case UI_ARMY_SELECTOR_SLOT_4: {
                        tag_message dialogMessage;
                        i32 armyValue;

                        armySlot7 = message.payload.widget.id - UI_ARMY_SELECTOR_FIRST;
                        if (quickView0 == 0
                            && giHeroScreenSrcIndex == UI_ARMY_SELECTION_NONE) {
                            if (gpHVHero->m_army.m_creatureTypes[armySlot7]
                                != ARMY_GROUP_EMPTY_SLOT) {
                                giHeroScreenSrcIndex = armySlot7;
                                gpHVHero->HeroScreenUpdate();
                            }
                        } else if ((quickView0 != 0
                                    && gpHVHero->m_army.m_creatureTypes[armySlot7]
                                           != ARMY_GROUP_EMPTY_SLOT)
                                   || (quickView0 == 0 && armySlot7 == giHeroScreenSrcIndex)) {
                            i32 canDismiss;

                            if (quickView0 == 0
                                && gpTownManager->m_castleDialogActive
                                       != UI_CASTLE_DIALOG_ACTIVE
                                && gpHVHero->m_army.GetNumArmies() != 1)
                                canDismiss = 0;
                            else
                                canDismiss = 1;
                            gpGame->ViewArmy(
                                UI_VIEW_ARMY_X,
                                UI_VIEW_ARMY_Y,
                                gpHVHero->m_army.m_creatureTypes[armySlot7],
                                gpHVHero->m_army.m_creatureCounts[armySlot7],
                                NULL,
                                canDismiss,
                                1,
                                quickView0,
                                gpHVHero,
                                NULL,
                                &gpHVHero->m_army,
                                armySlot7
                            );
                            if (quickView0 == 0)
                                giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
                            SetupHeroView();
                            RedrawHeroScreen();
                        } else {
                            if (quickView0 == 0 && gpTownManager->m_castleDialogActive != 0) {
                                if (gpHVHero->m_army.m_creatureTypes[armySlot7]
                                    != ARMY_GROUP_EMPTY_SLOT) {
                                    giHeroScreenSrcIndex = armySlot7;
                                    gpHVHero->HeroScreenUpdate();
                                }
                            } else if (quickView0 == 0) {
                                temp1 = gpHVHero->m_army.m_creatureTypes[armySlot7];
                                if ((message.payload.widget.parameter & UI_SPLIT_MODIFIER_MASK)
                                        == 0
                                    || (gpHVHero->m_army.m_creatureTypes[armySlot7]
                                            != ARMY_GROUP_EMPTY_SLOT
                                        && gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]
                                               != gpHVHero->m_army.m_creatureTypes[armySlot7])) {
                                    if (gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]
                                        == gpHVHero->m_army.m_creatureTypes[armySlot7]) {
                                        gpHVHero->m_army.m_creatureCounts[armySlot7] +=
                                            gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                        gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] = 0;
                                        gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                            ARMY_GROUP_EMPTY_SLOT;
                                    } else {
                                        gpHVHero->m_army.m_creatureTypes[armySlot7] =
                                            gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex];
                                        gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                            static_cast<i8>(temp1);
                                        temp1 = gpHVHero->m_army.m_creatureCounts[armySlot7];
                                        gpHVHero->m_army.m_creatureCounts[armySlot7] =
                                            gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                        gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] =
                                            static_cast<i16>(temp1);
                                    }
                                } else {
                                    DoHeroSplit(armySlot7, giHeroScreenSrcIndex);
                                }
                                giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
                                gpHVHero->HeroScreenUpdate();
                            }
                        }
                        if (quickView0 == 0) {
                            gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
                            UpdateHeroScreenStatusBar(message);
                        }
                        break;
                    }

                    case UI_ARTIFACT_SLOT_0:
                    case UI_ARTIFACT_SLOT_1:
                    case UI_ARTIFACT_SLOT_2:
                    case UI_ARTIFACT_SLOT_3:
                    case UI_ARTIFACT_SLOT_4:
                    case UI_ARTIFACT_SLOT_5:
                    case UI_ARTIFACT_SLOT_6:
                    case UI_ARTIFACT_SLOT_7:
                    case UI_ARTIFACT_SLOT_8:
                    case UI_ARTIFACT_SLOT_9:
                    case UI_ARTIFACT_SLOT_10:
                    case UI_ARTIFACT_SLOT_11:
                    case UI_ARTIFACT_SLOT_12:
                    case UI_ARTIFACT_SLOT_13:
                        if (gpHVHero
                                ->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST]
                            != IDX(ARTIFACT_NONE)) {
                            if (quickView0 == 0
                                && gpHVHero->m_artifacts
                                           [message.payload.widget.id - UI_ARTIFACT_FIRST]
                                       == IDX(ARTIFACT_MAGIC_BOOK)) {
                                gpGame->ViewSpells(
                                    gpHVHero,
                                    UI_VIEW_SPELLS_ALL,
                                    ViewSpecialHandler,
                                    UI_VIEW_SPELLS_SPECIAL
                                );
                            } else {
                                gpHVHero->ViewArtifact(
                                    ArtifactType(
                                        gpHVHero->m_artifacts
                                            [message.payload.widget.id - UI_ARTIFACT_FIRST]
                                    ),
                                    quickView0,
                                    gpHVHero->m_artifactExtra
                                        [message.payload.widget.id - UI_ARTIFACT_FIRST]
                                );
                            }
                        }
                        break;

                    default: {
                        if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW1_FIRST
                            && message.payload.widget.id <= UI_SECONDARY_SKILL_ROW1_LAST)
                            secondarySkillSlot18 =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW1_FIRST;
                        else if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW2_FIRST
                                 && message.payload.widget.id <= UI_SECONDARY_SKILL_ROW2_LAST)
                            secondarySkillSlot18 =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW2_FIRST;
                        else if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW3_FIRST
                                 && message.payload.widget.id <= UI_SECONDARY_SKILL_ROW3_LAST)
                            secondarySkillSlot18 =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW3_FIRST;
                        else
                            break;

                        if (secondarySkillSlot18 < gpHVHero->m_secondarySkillCount)
                            gpHVHero->DoSSLevelDialog(
                                gpHVHero->GetNthSS(secondarySkillSlot18),
                                quickView0
                            );
                        break;
                    }
                }
                break;
        }
    }

    if (exitHero36) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = UI_DIALOG_CLOSE_COMMAND;
        message.payload.widget.command = BaseWidgetCommand(message.payload.widget.id);
        return UI_HANDLER_CLOSE;
    }
    return UI_HANDLER_CONTINUE;
}

VA(0x0046f305, 0x4f)
void RedrawHeroScreen(void) {
    gpResourceManager
        ->GetBackdrop("herobkg.icn", gpWindowManager->m_screen, UI_BACKDROP_PALETTE);
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, UI_SCREEN_WIDTH, UI_SCREEN_HEIGHT);
}

VA(0x0046f354, 0x218)
i32 HeroView(i32 heroId, i32 noDismiss, i32 fadeAlreadyOut) {
    mapCell* heroCell;

    gbNoDismiss = noDismiss;
    iOrigHeroViewID = heroId;
    gpAdvManager->TrimLoopingSounds(UI_LOOPING_SOUND_KEEP_COUNT);
    gpHVHero = gpGame->GetHero(heroId);
    gbHeroWindShowing = true;
    if (fadeAlreadyOut == 0)
        gpWindowManager->FadeScreen(UI_FADE_OUT, UI_FADE_STEPS, NULL);

    heroWin = new heroWindow(0, 0, "herowind.bin");
    if (heroWin == NULL)
        MemError();
    SetWinText(heroWin, UI_WINDOW_TEXT_ID);
    gheroWin = heroWin;

    if (gpCurPlayer->m_currentHero == gpHVHero->m_id) {
        heroCell = gpAdvManager->GetCell(gpHVHero->m_x, gpHVHero->m_y);
        if (heroCell->m_triggerType != (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)) {
            gpHVHero->m_locationType = heroCell->m_triggerType;
            gpHVHero->m_occupiedTown = heroCell->m_objectMetadata;
        }
    }

    SetupHeroView();
    RedrawHeroScreen();
    gpWindowManager->FadeScreen(UI_FADE_IN, UI_FADE_STEPS, NULL);
    gpWindowManager->DoDialog(heroWin, HeroHandler, 0);
    gpWindowManager->FadeScreen(UI_FADE_OUT, UI_FADE_STEPS, NULL);
    delete heroWin;
    gheroWin = NULL;

    if (gpWindowManager->m_dialogResult == UI_DIALOG_DISMISS) {
        return UI_VIEW_DISMISSED;
    } else {
        gpHVHero->m_mobility = gpHVHero->CalcMobility();
        gbHeroWindShowing = false;
        gpHVHero = NULL;
        return UI_VIEW_CLOSED;
    }
}

VA(0x0046f56c, 0x9c5)
void SetupHeroView(void) {
    i32 cannotDismiss;
    tag_message message;
    i32 index;
    i32 displayIndex;
    i32 magnitude;
    i32 luck;
    i32 morale;
    i32 secondarySkillBonus;
    i32 secondarySkill;
    tag_message statusMessage;

    cannotDismiss = gbNoDismiss;
    if (gpHVHero->m_locationType == HERO_LOCATION_TOWN)
        cannotDismiss = 1;

    message.type = HERO_UI_MESSAGE;
    sprintf(gText, "%s the %s", gpHVHero->m_name, gAlignmentNames[gpHVHero->m_cursorType]);
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = UI_HERO_TITLE;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    if (gpHVHero->m_owner != giCurPlayer || gpCurPlayer->m_heroCount == UI_SINGLE_HERO_COUNT) {
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        message.payload.widget.data.value = UI_CYCLE_BUTTON_DISABLED_FRAME;
        message.payload.widget.id = UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = UI_NEXT_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        message.payload.widget.data.value = UI_CYCLE_BUTTON_FRAME;
        message.payload.widget.id = UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = UI_NEXT_HERO;
        heroWin->BroadcastMessage(message);
    }

    message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    for (index = 0; index < UI_ARMY_SLOT_COUNT; index++) {
        message.payload.widget.id = UI_PRIMARY_STAT_FIRST + index;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = UI_ARMY_SELECTOR_FIRST + index;
        heroWin->BroadcastMessage(message);
    }

    if (cannotDismiss == 0 && gpTownManager->m_castleDialogActive == 0
        && (gpCurPlayer->m_townCount != 0 || gpCurPlayer->m_heroCount != UI_SINGLE_HERO_COUNT))
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.id = UI_DISMISS;
    message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
    heroWin->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", gpHVHero->m_portrait);
    message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    message.payload.widget.id = UI_HERO_PORTRAIT;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    for (index = 0; index < HERO_PRIMARY_STAT_COUNT; index++) {
        sprintf(gText, "%d", gpHVHero->Stats(HeroPrimaryStat(index)));
        message.payload.widget.id = UI_PRIMARY_STAT_VALUE_FIRST + index;
        message.payload.widget.data.text = gText;
        heroWin->BroadcastMessage(message);
    }

    luck = gpGame->GetLuck(gpHVHero, NULL, gpHVHero->GetOccupiedTown());
    magnitude = abs(luck);
    if (magnitude <= UI_MIN_MODIFIER_ICONS)
        magnitude = UI_MIN_MODIFIER_ICONS;
    for (index = 0; index < UI_MODIFIER_ICON_COUNT; index++) {
        if (index >= magnitude)
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        else
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        if (index == 1 && luck != 0)
            displayIndex = 0;
        else if (index == 0 && luck != 0)
            displayIndex = 1;
        else
            displayIndex = index;
        message.payload.widget.id = UI_LUCK_FIRST + displayIndex;
        message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(message);
    }
    for (index = 0; index < UI_MODIFIER_ICON_COUNT; index++) {
        message.payload.widget.command = HERO_UI_WIDGET_FRAME;
        message.payload.widget.id = UI_LUCK_FIRST + index;
        if (luck < 0)
            message.payload.widget.data.value = UI_LUCK_NEGATIVE_FRAME;
        else if (luck == 0)
            message.payload.widget.data.value = UI_LUCK_NEUTRAL_FRAME;
        else
            message.payload.widget.data.value = UI_LUCK_POSITIVE_FRAME;
        heroWin->BroadcastMessage(message);
    }

    morale = gpHVHero->m_army.GetMorale(gpHVHero, gpHVHero->GetOccupiedTown(), NULL);
    magnitude = abs(morale);
    if (magnitude <= UI_MIN_MODIFIER_ICONS)
        magnitude = UI_MIN_MODIFIER_ICONS;
    for (index = 0; index < UI_MODIFIER_ICON_COUNT; index++) {
        if (index >= magnitude)
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        else
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        if (index == 1 && morale != 0)
            displayIndex = 0;
        else if (index == 0 && morale != 0)
            displayIndex = 1;
        else
            displayIndex = index;
        message.payload.widget.id = UI_MORALE_FIRST + displayIndex;
        message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(message);
    }
    for (index = 0; index < UI_MODIFIER_ICON_COUNT; index++) {
        message.payload.widget.command = HERO_UI_WIDGET_FRAME;
        message.payload.widget.id = UI_MORALE_FIRST + index;
        if (morale < 0)
            message.payload.widget.data.value = UI_MORALE_NEGATIVE_FRAME;
        else if (morale == 0)
            message.payload.widget.data.value = UI_MORALE_NEUTRAL_FRAME;
        else
            message.payload.widget.data.value = UI_MORALE_POSITIVE_FRAME;
        heroWin->BroadcastMessage(message);
    }

    sprintf(gText, "%d", gpHVHero->m_experience);
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = UI_EXPERIENCE_LAST;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    if (HAS(gpHVHero->m_eventFlags, HERO_EVENT_GROUPED_FORMATION))
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    message.payload.widget.id = UI_FORMATION_SPREAD_ICON;
    message.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(message);
    if (HAS(gpHVHero->m_eventFlags, HERO_EVENT_GROUPED_FORMATION))
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.id = UI_FORMATION_GROUPED_ICON;
    message.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(message);

    sprintf(
        gText,
        "%d/%d",
        gpHVHero->m_spellPoints,
        gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE
    );
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = UI_SPELL_POINTS_LAST;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    sprintf(gText, "crest.icn");
    message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    message.payload.widget.id = UI_PLAYER_CREST;
    heroWin->BroadcastMessage(message);
    message.payload.widget.command = HERO_UI_WIDGET_FRAME;
    message.payload.widget.id = UI_PLAYER_CREST;
    message.payload.widget.data.value = gpCurPlayer->Color();
    heroWin->BroadcastMessage(message);

    gpHVHero->UpdateArmies();
    for (index = 0; index < HERO_SECONDARY_SKILL_CAPACITY; index++) {
        if (index < gpHVHero->m_secondarySkillCount) {
            secondarySkill = gpHVHero->GetNthSS(index);
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW1_FIRST + index;
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = secondarySkill + 1;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + index;
            message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.text = gSecondarySkills[secondarySkill];
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + index;
            secondarySkillBonus =
                gpHVHero->GetSSLevel(secondarySkill) - gpHVHero->m_secondarySkills[secondarySkill];
            if (secondarySkillBonus > 0) {
                sprintf(
                    gText,
                    "%s+%d",
                    gSecondarySkillLevels[gpHVHero->m_secondarySkills[secondarySkill] - 1],
                    secondarySkillBonus
                );
            } else {
                sprintf(
                    gText,
                    "%s",
                    gSecondarySkillLevels[gpHVHero->m_secondarySkills[secondarySkill] - 1]
                );
            }
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW1_FIRST + index;
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = UI_EMPTY_SKILL_FRAME;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + index;
            message.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
        }
    }

    for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++) {
        message.payload.widget.id = UI_ARTIFACT_FIRST + index;
        if (gpHVHero->m_artifacts[index] != IDX(ARTIFACT_NONE)) {
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = gpHVHero->m_artifacts[index] + 1;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = UI_EMPTY_ARTIFACT_FRAME;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.data.value = UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(message);
        }
    }

    statusMessage.type = HERO_UI_MESSAGE;
    statusMessage.payload.widget.id = UI_ARMY_SELECTION_NONE;
    UpdateHeroScreenStatusBar(statusMessage);
}

VA(0x0046ff31, 0x2b0)
void DoHeroSplit(i32 destinationSlot, i32 sourceSlot) {
    i16 splitTextSlot = UI_SPLIT_TEXT;
    i16 splitAmount = UI_SPLIT_AMOUNT;
    tag_message message;

    gpTownManager->m_heroWindow1 =
        new heroWindow(UI_SPLIT_WINDOW_X, UI_SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == NULL)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = gpHVHero->m_army.m_creatureCounts[sourceSlot];

    message.type = HERO_UI_MESSAGE;
    sprintf(gText, "Move how many troops?");
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = UI_SPLIT_TEXT;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.payload.widget.id = UI_SPLIT_AMOUNT;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(gpTownManager->m_heroWindow1, SplitArmyHandler, 0);
    delete gpTownManager->m_heroWindow1;

    if (gpWindowManager->m_dialogResult == UI_DIALOG_SPLIT
        && gpTownManager->m_splitAmount != 0) {
        if (gpHVHero->m_army.m_creatureTypes[sourceSlot]
            == gpHVHero->m_army.m_creatureTypes[destinationSlot]) {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -= gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] += gpTownManager->m_splitAmount;
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] = ARMY_GROUP_EMPTY_SLOT;
        } else {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -= gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] =
                static_cast<i16>(gpTownManager->m_splitAmount);
            gpHVHero->m_army.m_creatureTypes[destinationSlot] =
                gpHVHero->m_army.m_creatureTypes[sourceSlot];
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] = ARMY_GROUP_EMPTY_SLOT;
        }
    }
}

VA(0x004701e1, 0x6a)
void hero::SetSS(i32 skill, i32 level) {
    if (level == IDX(HERO_SKILL_LEVEL_NONE))
        TakeSS(skill, IDX(HERO_SKILL_LEVEL_EXPERT));
    else if (m_secondarySkills[skill] != IDX(HERO_SKILL_LEVEL_NONE))
        m_secondarySkills[skill] = static_cast<i8>(level);
    else
        GiveSS(skill, level);
}

VA(0x0047024b, 0xfa)
i32 hero::TakeSS(i32 skill, i32 levels) {
    i32 oldLevel;
    i32 otherSkill;

    oldLevel = m_secondarySkills[skill];
    if (m_secondarySkills[skill] != IDX(HERO_SKILL_LEVEL_NONE)) {
        m_secondarySkills[skill] -= levels;
        if (m_secondarySkills[skill] < IDX(HERO_SKILL_LEVEL_NONE))
            m_secondarySkills[skill] = IDX(HERO_SKILL_LEVEL_NONE);
        if (m_secondarySkills[skill] == IDX(HERO_SKILL_LEVEL_NONE)) {
            for (otherSkill = 0; otherSkill < IDX(HERO_SKILL_COUNT); otherSkill++) {
                if (m_secondarySkillOrder[otherSkill] > m_secondarySkillOrder[skill]) {
                    m_secondarySkillOrder[otherSkill]--;
                }
            }
            m_secondarySkillOrder[skill] = 0;
            m_secondarySkillCount--;
        }
    }
    return oldLevel - m_secondarySkills[skill];
}

VA(0x00470345, 0xbf)
i32 hero::GiveSS(i32 skill, i32 levels) {
    i32 oldLevel;

    oldLevel = m_secondarySkills[skill];
    if (m_secondarySkills[skill] != IDX(HERO_SKILL_LEVEL_NONE)) {
        m_secondarySkills[skill] += levels;
    } else {
        if (m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY) {
            m_secondarySkills[skill] = static_cast<i8>(levels);
            m_secondarySkillCount++;
            m_secondarySkillOrder[skill] = static_cast<u8>(m_secondarySkillCount);
        }
    }
    if (m_secondarySkills[skill] > IDX(HERO_SKILL_LEVEL_EXPERT))
        m_secondarySkills[skill] = IDX(HERO_SKILL_LEVEL_EXPERT);
    return m_secondarySkills[skill] - oldLevel;
}

VA(0x00470404, 0x6a)
i32 hero::CreatureTypeCount(i32 creatureType) {
    i32 creatureCount;
    i32 armySlot;

    creatureCount = 0;
    for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT; armySlot++) {
        if (m_army.m_creatureTypes[armySlot] == creatureType
            && m_army.m_creatureCounts[armySlot] > 0) {
            creatureCount++;
        }
    }
    return creatureCount;
}

VA(0x0047046e, 0x5e)
void hero::UpgradeCreatures(i32 oldCreatureType, i32 newCreatureType) {
    i32 numberUpgraded = 0;
    i32 armySlot;

    for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT; armySlot++) {
        if (m_army.m_creatureTypes[armySlot] == oldCreatureType)
            m_army.m_creatureTypes[armySlot] = static_cast<i8>(newCreatureType);
    }
}

VA(0x004704cc, 0x5e)
i32 hero::GetNthSS(i32 ordinal) {
    i32 skill;

    for (skill = 0; skill < IDX(HERO_SKILL_COUNT); skill++) {
        if (m_secondarySkillOrder[skill] == ordinal + HERO_SECONDARY_SKILL_ORDER_BASE)
            return skill;
    }
    return HERO_SECONDARY_SKILL_NONE;
}

VA(0x0047052a, 0x51)
class town* hero::GetOccupiedTown(void) {
    if (m_locationType == HERO_LOCATION_TOWN)
        return gpGame->GetTown(m_occupiedTown);
    return NULL;
}

VA(0x0047057b, 0x47)
i8 hero::Stats(HeroPrimaryStat stat) {
    if (stat == HERO_PRIMARY_SPELL_POWER && m_primaryStats[IDX(stat)] < HERO_MINIMUM_SPELL_POWER) {
        return HERO_MINIMUM_SPELL_POWER;
    }
    return m_primaryStats[IDX(stat)];
}

VA(0x004705c2, 0xc3)
i8 hero::GetSSLevel(i32 skill) {
    i8 shrineAndArtifactBonus = 0;
    i8 level;

    level = m_secondarySkills[skill];
    if (skill != IDX(HERO_SKILL_NECROMANCY))
        return level;
    if (level == IDX(HERO_SKILL_LEVEL_NONE))
        return level;
    if (HasArtifact(ARTIFACT_SPADE_NECROMANCY))
        shrineAndArtifactBonus++;
    if (m_cursorType == IDX(FACTION_NECROMANCER))
        shrineAndArtifactBonus += gpGame->CountShrines(m_owner);
    if (shrineAndArtifactBonus > HERO_NECROMANCY_BONUS_MAX)
        shrineAndArtifactBonus = HERO_NECROMANCY_BONUS_MAX;
    level = OD_STEER(shrineAndArtifactBonus) + level;
    if (level > HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX)
        level = HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX;
    return level;
}

VA(0x00470685, 0xf4)
void hero::DoSSLevelDialog(i32 skill, i32 quickView) {
    i32 skillBonusValue;
    char* skillLevelText;

    skillBonusValue = GetSSLevel(skill) - m_secondarySkills[skill];
    if (skillBonusValue > 0) {
        skillLevelText = gSecondarySkillLevels[m_secondarySkills[skill] - 1];
        sprintf(
            gText,
            "{%s Necromancy (+%d)}\n\n%s Necromancy (+%d) allows %d percent of the creatures "
            "killed in combat to be brought back from the dead as Skeletons.",
            skillLevelText,
            skillBonusValue,
            skillLevelText,
            skillBonusValue,
            GetSSLevel(skill) * HERO_NECROMANCY_PERCENT_PER_LEVEL
        );
    } else {
        sprintf(gText, cSecSkillDesc[skill][m_secondarySkills[skill] - 1]);
    }
    NormalDialog(
        gText,
        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        NORMAL_DIALOG_NO_RESOURCE,
        NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_SECONDARY_SKILL,
        m_secondarySkills[skill] + skill * HERO_SECONDARY_SKILL_ICON_STRIDE
            - HERO_SECONDARY_SKILL_ICON_FRAME_BASE,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_RESOURCE,
        0
    );
}

VA(0x00470779, 0x12f)
void hero::CheckAnduranPieces(i32 showDialog) {
    i32 artifactSlot;

    if (HasArtifact(ARTIFACT_BREASTPLATE_ANDURAN) && HasArtifact(ARTIFACT_HELMET_ANDURAN)
        && HasArtifact(ARTIFACT_SWORD_ANDURAN)) {
        for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT; artifactSlot++) {
            if (m_artifacts[artifactSlot] == IDX(ARTIFACT_BREASTPLATE_ANDURAN)
                || m_artifacts[artifactSlot] == IDX(ARTIFACT_HELMET_ANDURAN)
                || m_artifacts[artifactSlot] == IDX(ARTIFACT_SWORD_ANDURAN)) {
                GiveTakeArtifactStat(
                    this,
                    ArtifactType(m_artifacts[artifactSlot]),
                    EVENT_ARTIFACT_TAKE
                );
                m_artifacts[artifactSlot] = IDX(ARTIFACT_NONE);
            }
        }
        GiveArtifact(this, ARTIFACT_BATTLE_GARB, showDialog, IDX(ARTIFACT_NONE));
        if (gbThisNetHumanPlayer[m_owner]) {
            LoadPlaySample("treasure.82m");
            NormalDialog(
                "The three Anduran artifacts magically combine into one.",
                NORMAL_DIALOG_INFO,
                NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_VALUE,
                NORMAL_DIALOG_ARTIFACT,
                IDX(ARTIFACT_BATTLE_GARB),
                NORMAL_DIALOG_NO_RESOURCE,
                0,
                NORMAL_DIALOG_NO_RESOURCE,
                0
            );
        }
    }
}

DATA(0x004f6c88) class hero* gpHVHero = NULL;
DATA(0x004f6c8c) class heroWindow* gheroWin = NULL;
DATA(0x004f6cd0) i16 gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT] =
    {0, 1000, 2000, 3200, 4500, 6000, 7700, 9000, 11000, 13200, 15500, 18500}; // NOLINT(readability-magic-numbers)
DATA(0x005280dc) i32 iOrigHeroViewID;
DATA(0x005280e0) b32 gbNoDismiss;
