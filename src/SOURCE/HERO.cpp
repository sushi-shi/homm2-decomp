#include <va.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/DebugCheck.h>
#include <BASE/message.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <EDITOR/fullMap.h>
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

#define HERO_EXPERIENCE_GROWTH_FACTOR 1.2

H2_ENUM_BEGIN(HeroUiConstant)
    UI_STATUS_TEXT_WIDGET = 0x12f,
    UI_ARTIFACT_SLOT_0 = 0x14,
    UI_ARTIFACT_SLOT_1 = 0x15,
    UI_ARTIFACT_SLOT_2 = 0x16,
    UI_ARTIFACT_SLOT_3 = 0x17,
    UI_ARTIFACT_SLOT_4 = 0x18,
    UI_ARTIFACT_SLOT_5 = 0x19,
    UI_ARTIFACT_SLOT_6 = 0x1a,
    UI_ARTIFACT_SLOT_7 = 0x1b,
    UI_ARTIFACT_SLOT_8 = 0x1c,
    UI_ARTIFACT_SLOT_9 = 0x1d,
    UI_ARTIFACT_SLOT_10 = 0x1e,
    UI_ARTIFACT_SLOT_11 = 0x1f,
    UI_ARTIFACT_SLOT_12 = 0x20,
    UI_ARTIFACT_SLOT_13 = 0x21,
    UI_ARTIFACT_FIRST = UI_ARTIFACT_SLOT_0,
    UI_ARTIFACT_LAST = UI_ARTIFACT_SLOT_13,
    UI_PRIMARY_STAT_ATTACK = 0x51,
    UI_PRIMARY_STAT_DEFENSE = 0x52,
    UI_PRIMARY_STAT_SPELL_POWER = 0x53,
    UI_PRIMARY_STAT_KNOWLEDGE = 0x54,
    UI_PRIMARY_STAT_FIRST = UI_PRIMARY_STAT_ATTACK,
    UI_PRIMARY_STAT_LAST = UI_PRIMARY_STAT_KNOWLEDGE,
    UI_ADDITIONAL_STATS = 0x55,
    UI_ARMY_ICON_FIRST = 0x57,
    UI_ARMY_PORTRAIT_FIRST = 0x5c,
    UI_ARMY_COUNT_FIRST = 0x61,
    UI_ARMY_SELECTOR_SLOT_0 = 0x66,
    UI_ARMY_SELECTOR_SLOT_1 = 0x67,
    UI_ARMY_SELECTOR_SLOT_2 = 0x68,
    UI_ARMY_SELECTOR_SLOT_3 = 0x69,
    UI_ARMY_SELECTOR_SLOT_4 = 0x6a,
    UI_ARMY_SELECTOR_FIRST = UI_ARMY_SELECTOR_SLOT_0,
    UI_ARMY_SELECTOR_LAST = UI_ARMY_SELECTOR_SLOT_4,
    UI_HERO_TITLE = 2,
    UI_HERO_PORTRAIT = 0x41,
    UI_PRIMARY_STAT_VALUE_FIRST = 0x4c,
    UI_PLAYER_CREST = 0x56,
    UI_MORALE_FIRST = 0xc8,
    UI_MORALE_MIDDLE = 0xc9,
    UI_MORALE_LAST = 0xca,
    UI_LUCK_FIRST = 0xcb,
    UI_LUCK_MIDDLE = 0xcc,
    UI_LUCK_LAST = 0xcd,
    UI_EXPERIENCE_FIRST = 0xce,
    UI_EXPERIENCE_LAST = 0xcf,
    UI_SPELL_POINTS_FIRST = 0xd3,
    UI_SPELL_POINTS_LAST = 0xd4,
    UI_FORMATION_SPREAD = 0xd6,
    UI_FORMATION_GROUPED = 0xd8,
    UI_FORMATION_SPREAD_ICON = 0xd5,
    UI_FORMATION_GROUPED_ICON = 0xd7,
    UI_PREVIOUS_HERO = 300,
    UI_NEXT_HERO = 301,
    UI_ARMY_SELECTION_NONE = -1,
    UI_HERO_CYCLE_MIN_COUNT = 2,
    UI_SINGLE_HERO_COUNT = 1,
    UI_MODIFIER_ICON_COUNT = 3,
    UI_MIN_MODIFIER_ICONS = 1,
    UI_PREVIOUS_HERO_MASK = -2,
    UI_SECONDARY_SKILL_ROW1_FIRST = 0x190,
    UI_SECONDARY_SKILL_ROW1_LAST = 0x197,
    UI_SECONDARY_SKILL_ROW2_FIRST = 0x198,
    UI_SECONDARY_SKILL_ROW2_LAST = 0x19f,
    UI_SECONDARY_SKILL_ROW3_FIRST = 0x1a0,
    UI_SECONDARY_SKILL_ROW3_LAST = 0x1a7,
    UI_CLOSE = 0x7800,
    UI_DISMISS = 0x7803,
    UI_DIALOG_CLOSE_COMMAND = 10,
    UI_VIEW_ARMY_X = 0x77,
    UI_VIEW_ARMY_Y = 0x14,
    UI_VIEW_SPELLS_SPECIAL = 1,
    UI_CASTLE_DIALOG_ACTIVE = 1,
    UI_ARMY_EMPTY_FRAME = 2,
    UI_WIDGET_FRAME_ACTIVE = 4,
    UI_ARTIFACT_DIALOG_ICON = 0x1c,
    UI_ARMY_SLOT_COUNT = 5,
    UI_SCREEN_WIDTH = 640,
    UI_SCREEN_HEIGHT = 480,
    UI_STATUS_REGION_Y = 459,
    UI_STATUS_REGION_HEIGHT = 20,
    UI_FADE_STEPS = 8,
    UI_LOOPING_SOUND_KEEP_COUNT = 4,
    UI_VIEW_CLOSED = 0,
    UI_VIEW_DISMISSED = 1,
    UI_WINDOW_TEXT_ID = 6,
    UI_BACKDROP_PALETTE = 1,
    UI_DIALOG_DISMISS = 0x7803,
    UI_DIALOG_SPLIT = 0x7802,
    UI_SPLIT_WINDOW_X = 0xb1,
    UI_SPLIT_WINDOW_Y = 0x14,
    UI_SPLIT_TEXT = 1,
    UI_SPLIT_AMOUNT = 68,
    UI_CONTROL_VALUE_DEFAULT = 6,
    UI_CONTROL_FRAME_DEFAULT = 4,
    UI_CYCLE_BUTTON_FRAME = 2,
    UI_CYCLE_BUTTON_DISABLED_FRAME = 0x1000,
    UI_LUCK_NEGATIVE_FRAME = 3,
    UI_LUCK_NEUTRAL_FRAME = 6,
    UI_LUCK_POSITIVE_FRAME = 2,
    UI_MORALE_NEGATIVE_FRAME = 5,
    UI_MORALE_NEUTRAL_FRAME = 7,
    UI_MORALE_POSITIVE_FRAME = 4,
    UI_EMPTY_SKILL_FRAME = 0,
    UI_EMPTY_ARTIFACT_FRAME = 0,
    UI_ARTIFACT_CONTROL_VALUE = 2,
    UI_ARMY_RACE_FRAME_OFFSET = 4
H2_ENUM_END(HeroUiConstant)

H2_ENUM_CLASS_BEGIN(HeroScreenText)
    TEXT_PRIMARY_STAT = 1,
    TEXT_ADDITIONAL_STATS = 2,
    TEXT_GOOD_MORALE = 3,
    TEXT_NEUTRAL_MORALE = 4,
    TEXT_BAD_MORALE = 5,
    TEXT_GOOD_LUCK = 6,
    TEXT_NEUTRAL_LUCK = 7,
    TEXT_BAD_LUCK = 8,
    TEXT_EXPERIENCE = 9,
    TEXT_SELECT_ARMY = 10,
    TEXT_EMPTY = 11,
    TEXT_MOVE_ARMY = 12,
    TEXT_EXCHANGE_ARMIES = 13,
    TEXT_VIEW_SPELLS = 14,
    TEXT_ARTIFACT = 15,
    TEXT_DISMISS = 16,
    TEXT_EXIT = 17,
    TEXT_SCREEN = 18,
    TEXT_COMBINE_ARMIES = 19,
    TEXT_SPLIT_ARMY = 20,
    TEXT_SECONDARY_SKILL = 21,
    TEXT_SPELL_POINTS = 22,
    TEXT_SPREAD_FORMATION = 23,
    TEXT_GROUPED_FORMATION = 24
H2_ENUM_CLASS_END(HeroScreenText)

H2_ENUM_BEGIN(HeroMobilityConstant)
    BASE_RECORD_SIZE = 0xec,
    LAND_SPEED_COUNT = 8,
    SLOWEST_LAND_SPEED = LAND_SPEED_COUNT - 1,
    SEA_BASE_MOBILITY = 1500,
    LIGHTHOUSE_MOBILITY_BONUS = 500,
    ASTROLABE_MOBILITY_BONUS = 1000,
    COMPASS_MOBILITY_BONUS = 500,
    NOMAD_BOOTS_MOBILITY_BONUS = 600,
    TRAVELER_BOOTS_MOBILITY_BONUS = 300,
    STABLES_MOBILITY_BONUS = 400,
    AI_DIFFICULTY_MOBILITY_BONUS = 75,
    AI_STATE_MOBILITY_BONUS = 50
H2_ENUM_END(HeroMobilityConstant)

H2_ENUM_BEGIN(HeroImplementationConstant)
    EXPERIENCE_PREVIOUS_ENTRY_OFFSET = 2,
    TEMPLE_MORALE_BONUS = 2,
    PYRAMID_LUCK_PENALTY = 2
H2_ENUM_END(HeroImplementationConstant)

VA(0x00460e50, 0x64)
hero::hero(void) {
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_cursorType = FACTION_KNIGHT;
    m_portrait = 0;
    m_name[0] = 0;
    heroWin = NULL;
    giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
}

VA(0x00460eb4, 0x41)
void hero::Read(i32 file, i8 expansion) {
    if (expansion)
        read(file, this, sizeof(hero));
    else
        read(file, this, BASE_RECORD_SIZE);
}

VA(0x00460ef5, 0x41)
void hero::Write(i32 file, i8 expansion) {
    if (expansion)
        write(file, this, sizeof(hero));
    else
        write(file, this, BASE_RECORD_SIZE);
}

VA(0x00460f36, 0xd)
void hero::GetArmyStrengths(u32l* const) {}

VA(0x00460f43, 0x44)
i32 hero::HasArtifact(ArtifactType artifact) {
    i32 artifactIndex;

    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == artifact)
            return 1;
    }
    return 0;
}

VA(0x00460f87, 0x23c)
i32 hero::CalcMobility(void) {
    i16 mobilityTable[LAND_SPEED_COUNT] = {1000, 1000, 1000, 1100, 1200, 1300, 1400, 1500};
    const i16 seaBaseMobility = SEA_BASE_MOBILITY;
    const i16 lighthousePoints = LIGHTHOUSE_MOBILITY_BONUS;
    const i16 astrolabeMobility = ASTROLABE_MOBILITY_BONUS;
    const i16 compassMobility = COMPASS_MOBILITY_BONUS;
    const i16 nomadBootsBonus = NOMAD_BOOTS_MOBILITY_BONUS;
    const i16 travelerBonus = TRAVELER_BOOTS_MOBILITY_BONUS;
    i32 movePoints;
    i32 slowestSpeedValue;
    i32 creatureIndex;

    if (HAS(m_eventFlags, HERO_EVENT_EMBARKED)) {
        movePoints = seaBaseMobility;
        movePoints = static_cast<i32>(
            movePoints * gfSSNavigationMod[IDX(m_secondarySkills[IDX(HERO_SKILL_NAVIGATION)])]
        );
        if (m_owner != -1)
            movePoints += gpGame->MineTypesOwned(m_owner, MINE_TYPE_LIGHTHOUSE) * lighthousePoints;
        if (HasArtifact(ARTIFACT_SAILORS_ASTROLABE))
            movePoints += astrolabeMobility;
    } else {
        slowestSpeedValue = SLOWEST_LAND_SPEED;
        for (creatureIndex = 0; creatureIndex < ARMY_GROUP_SLOT_COUNT; creatureIndex++) {
            if (m_army.m_creatureTypes[creatureIndex] != CREATURE_NONE
                && gMonsterDatabase[IDX(m_army.m_creatureTypes[creatureIndex])].speed
                       < slowestSpeedValue) {
                slowestSpeedValue =
                    gMonsterDatabase[IDX(m_army.m_creatureTypes[creatureIndex])].speed;
            }
        }
        movePoints = mobilityTable[slowestSpeedValue];
        movePoints = static_cast<i32>(
            movePoints * gfSSLogisticsMod[IDX(m_secondarySkills[IDX(HERO_SKILL_LOGISTICS)])]
        );
        if (HasArtifact(ARTIFACT_NOMAD_BOOTS))
            movePoints += nomadBootsBonus;
        if (HasArtifact(ARTIFACT_TRAVELER_BOOTS))
            movePoints += travelerBonus;
        if (HAS(m_eventFlags, HERO_EVENT_STABLES))
            movePoints += STABLES_MOBILITY_BONUS;
    }

    if (HasArtifact(ARTIFACT_TRUE_COMPASS))
        movePoints += compassMobility;

    if (m_owner >= 0 && m_owner < GAME_PLAYER_COUNT && !gbHumanPlayer[m_owner]
        && gpGame->m_difficulty >= DIFFICULTY_HARD) {
        movePoints += AI_DIFFICULTY_MOBILITY_BONUS;
        if (gpGame->m_players[m_owner].m_aiDifficulty == PLAYER_PERSONALITY_EXPLORER)
            movePoints += AI_STATE_MOBILITY_BONUS;
    }
    return movePoints;
}

VA(0x004611c3, 0x9b)
i32 hero::HasSpell(SpellType spell) {
    i32 artifactIndex;

    if (!HasArtifact(ARTIFACT_MAGIC_BOOK))
        return 0;
    if (m_spells[IDX(spell)])
        return 1;
    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == ARTIFACT_SPELL_SCROLL
            && m_artifactExtra[artifactIndex] == IDX(spell)) {
            return 1;
        }
    }
    if (HasArtifact(ARTIFACT_BATTLE_GARB) && spell == SPELL_TOWN_PORTAL)
        return 1;
    return 0;
}

VA(0x0046125e, 0x95)
SpellType hero::GetNthSpell(HeroSpellType type, i32 spellNumber) {
    SpellType spell;
    i32 spellOrdinalCount = 0;

    for (spell = SPELL_FIREBALL; spell < SPELL_COUNT; spell++) {
        if (HasSpell(spell)) {
            if (type == SPELL_TYPE_ALL
                || (type == SPELL_TYPE_COMBAT
                    && HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))
                || (type == SPELL_TYPE_ADVENTURE
                    && !HAS(gsSpellInfo[IDX(spell)].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))) {
                spellOrdinalCount++;
            }
            if (spellNumber == spellOrdinalCount)
                return spell;
        }
    }
    return SPELL_NONE;
}

VA(0x004612f3, 0x9e)
i32 hero::GetNumSpells(HeroSpellType type) {
    i32 numAdventureSpells2;
    i32 numCombatSpells2;
    SpellType spell2;

    numCombatSpells2 = 0;
    numAdventureSpells2 = 0;
    for (spell2 = SPELL_FIREBALL; spell2 < SPELL_COUNT; spell2++) {
        if (HasSpell(spell2)) {
            if (HAS(gsSpellInfo[IDX(spell2)].attributes, SPELL_INFO_ATTRIBUTE_COMBAT))
                numCombatSpells2++;
            else
                numAdventureSpells2++;
        }
    }

    switch (type) {
        case SPELL_TYPE_COMBAT:
            return numCombatSpells2;
        case SPELL_TYPE_ADVENTURE:
            return numAdventureSpells2;
        case SPELL_TYPE_ALL:
            return numCombatSpells2 + numAdventureSpells2;
    }
    return 0;
}

VA(0x00461391, 0x6f)
void hero::UseSpell(SpellType spell) {
    if (spell == SPELL_NONE)
        return;

    m_spellPoints -= GetManaCost(spell, this);
    if (m_spellPoints < 0)
        m_spellPoints = 0;
    if (gpAdvManager->m_active == 1 && gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->UpdateHeroLocator(-1, 1, 1);
}

VA(0x00461400, 0x1a)
void hero::AddSpell(H2_ENUM_PARAM(SpellType, i32) spell, i32) {
    m_spells[IDX(spell)] = 1;
}

VA(0x0046141a, 0x72)
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
    gpWindowManager
        ->UpdateScreenRegion(0, UI_STATUS_REGION_Y, UI_SCREEN_WIDTH, UI_STATUS_REGION_HEIGHT);
}

VA(0x0046148c, 0x94)
void hero::HeroScreenUpdate(void) {
    tag_message message;
    i32 i;

    message.type = HERO_UI_MESSAGE;
    UpdateArmies();
    for (i = 0; i < UI_ARMY_SLOT_COUNT; i++) {
        if (giHeroScreenSrcIndex == i)
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        else
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
        message.payload.widget.id = i + UI_ARMY_SELECTOR_FIRST;
        heroWin->BroadcastMessage(message);
    }
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, UI_SCREEN_WIDTH, UI_SCREEN_HEIGHT);
}

VA(0x00461520, 0x1c3)
void hero::UpdateArmies(void) {
    tag_message message;
    i32 slot;

    message.type = HERO_UI_MESSAGE;
    for (slot = 0; slot < UI_ARMY_SLOT_COUNT; slot++) {
        if (m_army.m_creatureTypes[slot] == CREATURE_NONE) {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = slot + UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value = UI_ARMY_EMPTY_FRAME;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id = slot + UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = slot + UI_ARMY_COUNT_FIRST;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = slot + UI_ARMY_SELECTOR_FIRST;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = slot + UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value =
                IDX(gMonsterDatabase[IDX(m_army.m_creatureTypes[slot])].race)
                + UI_ARMY_RACE_FRAME_OFFSET;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
            sprintf(gText, "monh%04d.icn", IDX(m_army.m_creatureTypes[slot]));
            message.payload.widget.id = slot + UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);

            sprintf(gText, "%d", m_army.m_creatureCounts[slot]);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id = slot + UI_ARMY_COUNT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
        }
    }
}

VA(0x004616e3, 0x39)
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

VA(0x0046171c, 0x8b)
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

VA(0x004617a7, 0x4b)
i32 hero::Dismiss(void) {
    NormalDialog(
        "\xc2\xfb \xe4\xe5\xe9\xf1\xf2\xe2\xe8\xf2\xe5\xeb\xfc\xed\xee \xf5\xee\xf2\xe8\xf2\xe5 "
        "\xf3\xe2\xee\xeb\xe8\xf2\xfc \xe3\xe5\xf0\xee\xff?" /* "Вы действительно хотите уволить героя?" */
        ,
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

VA(0x004617f2, 0x4e4)
void hero::Deallocate(i32 updateMap) {
    i32 oldOwner;
    i32 i;
    playerData* playerPtr;
    i32 heroNum;
    town* curTown;
    i32 availSlot;
    fullMap* map;

    if (updateMap)
        SendMapChange(
            MAP_CHANGE_DEAD_HERO,
            m_id,
            static_cast<u8>(m_x),
            static_cast<u8>(m_y),
            MAP_CHANGE_CURRENT_PLAYER,
            0,
            0
        );

    oldOwner = m_owner;
    playerPtr = &gpGame->m_players[m_owner];

    if (updateMap)
        gpAdvManager->MobilizeCurrHero(0);
    if (updateMap)
        gpAdvManager->HideRoute(0, 0, 0);

    if (HAS(m_eventFlags, HERO_EVENT_EMBARKED)) {
        for (i = 0; i < GAME_BOAT_COUNT; i++) {
            if (gpGame->m_boats[i].heroId == m_id) {
                gpGame->m_boats[i].heroId = -1;
                gpGame->m_boatSlots[i] = -1;
            }
        }
    }

    if (m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)) {
        DebugCheck();
        curTown = &gpGame->m_castleRecs[m_occupiedTown];
        curTown->m_occupyingHeroId = -1;
    }

    if (giCurPlayer != m_owner || gpGame->m_players[m_owner].m_currentHero != m_id
        || gpAdvManager->m_heroContextLocked == 0) {
        gpGame->RestoreCell(m_x, m_y, m_locationType, m_occupiedTown, NULL, 1);
    }

    if (!gbCombatSurrender) {
        for (i = 0; i < ARMY_GROUP_SLOT_COUNT; i++)
            m_army.Dismiss(i);
    }

    heroNum = -1;
    for (i = 0; i < playerPtr->m_heroCount; i++) {
        if (playerPtr->m_heroIds[i] == m_id)
            heroNum = i;
    }
    for (i = heroNum; i < playerPtr->m_heroCount - 1; i++)
        playerPtr->m_heroIds[i] = playerPtr->m_heroIds[i + 1];
    playerPtr->m_heroIds[playerPtr->m_heroCount - 1] = -1;

    if (playerPtr->m_currentHero == m_id) {
        playerPtr->m_currentHero = -1;
        if (giCurPlayer == m_owner) {
            gpAdvManager->m_cursorActive = 0;
            map = &gpGame->m_worldMap;
            DebugCheck();
            map->GetCell(m_x, m_y)->m_flags &= ~HERO_MAP_CELL_PRESENT;
        }
        if (oldOwner == giCurPlayer)
            gpAdvManager->m_heroContextLocked = 0;
    }

    playerPtr->m_heroCount--;
    playerPtr->m_heroLocatorPage = 0;
    gpGame->m_availableHeroes[m_id] = HERO_AVAILABILITY_UNAVAILABLE;

    if (gbRetreatWin) {
        availSlot = Random(0, HERO_AVAILABLE_SLOT_COUNT - 1);
        if (HAS(gpGame->m_heroRecs[gpGame->m_players[m_owner].m_availableHeroIds[availSlot]]
                    .m_eventFlags,
                HERO_EVENT_WEEKLY_VISIT)) {
            availSlot = 1 - availSlot;
        }
        if (gpGame->m_availableHeroes[gpGame->m_players[m_owner].m_availableHeroIds[availSlot]]
            == HERO_AVAILABILITY_RETREATED) {
            gpGame->m_availableHeroes[gpGame->m_players[m_owner].m_availableHeroIds[availSlot]] =
                HERO_AVAILABILITY_UNAVAILABLE;
        }
        gpGame->m_players[m_owner].m_availableHeroIds[availSlot] = m_id;
        gpGame->m_availableHeroes[m_id] = HERO_AVAILABILITY_RETREATED;
        m_eventFlags = HeroEventFlag(static_cast<i32>(m_eventFlags) | IDX(HERO_EVENT_WEEKLY_VISIT));
    }

    m_eventFlags =
        HeroEventFlag(static_cast<i32>(m_eventFlags) & ~IDX(HERO_EVENT_GROUPED_FORMATION));
    m_owner = HERO_OWNER_NONE;
    m_destinationY = HERO_DESTINATION_NONE;
    m_destinationX = HERO_DESTINATION_NONE;

    if (!gbCombatSurrender)
        gpGame->SetRandomHeroArmies(m_id, RANDOM_HERO_NORMAL_ARMY);

    if (gbInCampaign && m_portrait == CAMPAIGN_HERO_CORLAGON
        && gpGame->m_campaignType == CAMPAIGN_ROLAND
        && gpGame->m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO && !gbRetreatWin
        && !gbCombatSurrender) {
        gpGame->m_campaignAwards[IDX(CAMPAIGN_AWARD_CORLAGON_DEFEATED)] = 1;
    }

    if (updateMap)
        CheckEndGame(END_GAME_FORCE_NONE, false);
}

VA(0x00461cd6, 0x8a)
i32 hero::GetExperience(i32 level) {
    i32 experience2;
    i32 stage;
    i32 incr;

    if (level <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT)
        return gMinExpForLevel[level - 1];

    stage = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    incr = static_cast<i32>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1]
         - gMinExpForLevel
             [HERO_EXPERIENCE_LEVEL_TABLE_COUNT - IDX(EXPERIENCE_PREVIOUS_ENTRY_OFFSET)])
        * HERO_EXPERIENCE_GROWTH_FACTOR
    );
    experience2 = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + incr;
    while (stage < level) {
        incr = static_cast<i32>(incr * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience2 += incr;
        stage++;
    }
    return experience2;
}

VA(0x00461d60, 0xac)
i32 hero::GetLevel(i32 experienceValue) {
    i32 experience;
    i32 levelCounter;
    i32 growth;

    for (levelCounter = 1; levelCounter <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT; levelCounter++) {
        if (experienceValue < gMinExpForLevel[levelCounter - 1])
            return levelCounter - 1;
    }

    growth = static_cast<i32>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1]
         - gMinExpForLevel
             [HERO_EXPERIENCE_LEVEL_TABLE_COUNT - IDX(EXPERIENCE_PREVIOUS_ENTRY_OFFSET)])
        * HERO_EXPERIENCE_GROWTH_FACTOR
    );
    experience = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + growth;
    levelCounter = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    while (experienceValue > experience) {
        growth = static_cast<i32>(growth * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience += growth;
        levelCounter++;
    }
    return levelCounter - 1;
}

VA(0x00461e0c, 0x2a9)
void hero::ApplyBattleWinTemps(void) {
    m_lastTownInteractionTurn = HERO_INTERACTION_TURN_NONE;
    m_lastHeroInteractionTurn = HERO_INTERACTION_TURN_NONE;

    if (HAS(m_eventFlags, HERO_EVENT_GRAVEYARD)) {
        m_morale++;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_GRAVEYARD);
    }
    if (HAS(m_eventFlags, HERO_EVENT_SHIPWRECK)) {
        m_morale++;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_SHIPWRECK);
    }
    if (HAS(m_eventFlags, HERO_EVENT_BUOY)) {
        m_morale--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_BUOY);
    }
    if (HAS(m_eventFlags, HERO_EVENT_OASIS)) {
        m_morale--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_OASIS);
    }
    if (HAS(m_eventFlags, HERO_EVENT_TEMPLE)) {
        m_morale -= TEMPLE_MORALE_BONUS;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_TEMPLE);
    }
    if (HAS(m_eventFlags, HERO_EVENT_FAERIE_RING)) {
        m_luck--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_FAERIE_RING);
    }
    if (HAS(m_eventFlags, HERO_EVENT_IDOL)) {
        m_luck--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_IDOL);
    }
    if (HAS(m_eventFlags, HERO_EVENT_FOUNTAIN)) {
        m_luck--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_FOUNTAIN);
    }
    if (HAS(m_eventFlags, HERO_EVENT_WATERING_HOLE)) {
        m_morale--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_WATERING_HOLE);
    }
    if (HAS(m_eventFlags, HERO_EVENT_DERELICT_SHIP)) {
        m_morale++;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_DERELICT_SHIP);
    }
    if (HAS(m_eventFlags, HERO_EVENT_PYRAMID)) {
        m_luck += PYRAMID_LUCK_PENALTY;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_PYRAMID);
    }
    if (HAS(m_eventFlags, HERO_EVENT_MERMAID)) {
        m_luck--;
        H2_ENUM_CLEAR_FLAG(m_eventFlags, HERO_EVENT_MERMAID);
    }
}

VA(0x004620b5, 0x13)
void hero::ApplyBattleLossTemps(void) {
    ApplyBattleWinTemps();
}

VA(0x004620c8, 0x6a3)
void hero::CheckLevel(void) {
    i32 nLevel;
    i32 statBonuses[HERO_PRIMARY_STAT_COUNT];
    i32 newLevel;
    i32 levelsGained;
    i32 highIndex;
    i32 slot;
    SAMPLE2 samp;
    HeroSecondarySkill choices[HERO_SECONDARY_SKILL_CHOICE_COUNT];
    char text[HERO_LEVEL_TEXT_BUFFER_SIZE];
    i32 weight;
    i32 rnd;
    HeroSecondarySkill skill;
    i32 tryCount;

    newLevel = GetLevel(m_experience);
    if (newLevel == m_level)
        return;

    samp = NULL;
    levelsGained = newLevel - m_level;
    for (nLevel = m_level + 1; nLevel <= newLevel; nLevel++) {
        sprintf(gText, cHeroLevel[0], m_name);
        sprintf(text, cHeroLevel[1]);
        strcat(gText, text);

        statBonuses[IDX(HERO_PRIMARY_ATTACK)] = 0;
        statBonuses[IDX(HERO_PRIMARY_DEFENSE)] = 0;
        statBonuses[IDX(HERO_PRIMARY_SPELL_POWER)] = 0;
        statBonuses[IDX(HERO_PRIMARY_KNOWLEDGE)] = 0;
        if (nLevel <= HERO_LEVEL_HIGH_THRESHOLD)
            highIndex = 0;
        else
            highIndex = 1;

        SRand(m_randomSeed + nLevel * HERO_LEVEL_RANDOM_SEED_FACTOR);
        rnd = SRandom(1, HERO_LEVEL_RANDOM_MAX);
        if (rnd
            < gHeroSkillBonus[IDX(m_cursorType)][highIndex][IDX(HERO_PRIMARY_ATTACK)]) {
            statBonuses[IDX(HERO_PRIMARY_ATTACK)]++;
        } else {
            rnd -=
                gHeroSkillBonus[IDX(m_cursorType)][highIndex][IDX(HERO_PRIMARY_ATTACK)];
            if (rnd < gHeroSkillBonus[IDX(m_cursorType)][highIndex]
                                             [IDX(HERO_PRIMARY_DEFENSE)]) {
                statBonuses[IDX(HERO_PRIMARY_DEFENSE)]++;
            } else {
                rnd -= gHeroSkillBonus[IDX(m_cursorType)][highIndex]
                                              [IDX(HERO_PRIMARY_DEFENSE)];
                if (rnd < gHeroSkillBonus[IDX(m_cursorType)][highIndex]
                                                 [IDX(HERO_PRIMARY_SPELL_POWER)]) {
                    statBonuses[IDX(HERO_PRIMARY_SPELL_POWER)]++;
                } else {
                    statBonuses[IDX(HERO_PRIMARY_KNOWLEDGE)]++;
                }
            }
        }

        for (slot = 0; slot < HERO_PRIMARY_STAT_COUNT; slot++) {
            if (statBonuses[slot] > 0) {
                m_primaryStats[slot] += static_cast<i8>(statBonuses[slot]);
                sprintf(text, "\n%s +%d", gStatNames[slot], statBonuses[slot]);
                strcat(gText, text);
            }
        }

        for (slot = 0; slot < HERO_SECONDARY_SKILL_CHOICE_COUNT; slot++) {
            choices[slot] = HERO_SKILL_NONE;
            if (slot == 0 && m_cursorType != FACTION_BARBARIAN
                && m_cursorType != FACTION_KNIGHT
                && m_secondarySkills[IDX(HERO_SKILL_WISDOM)] < HERO_SKILL_LEVEL_EXPERT
                && nLevel - m_enabled >= HERO_SECONDARY_SKILL_OFFER_GAP) {
                choices[slot] = HERO_SKILL_WISDOM;
                continue;
            }
            tryCount = 0;
            weight = Random(0, HERO_SECONDARY_SKILL_RANDOM_WEIGHT);
            skill = HERO_SKILL_PATHFINDING;
            while (tryCount < HERO_SECONDARY_SKILL_SEARCH_LIMIT) {
                tryCount++;
                if ((slot == 0 || skill != choices[0])
                    && ((m_secondarySkills[IDX(skill)] != HERO_SKILL_LEVEL_NONE
                         && m_secondarySkills[IDX(skill)] < HERO_SKILL_LEVEL_EXPERT)
                        || (m_secondarySkills[IDX(skill)] == HERO_SKILL_LEVEL_NONE
                            && m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY))) {
                    weight -= iGetSSByAlignment[IDX(skill)][IDX(m_cursorType)];
                    if (weight <= 0) {
                        choices[slot] = skill;
                        goto nextAttempt;
                    }
                }
                skill = HeroSecondarySkill((IDX(skill) + 1) % HERO_SKILL_COUNT);
            }
        nextAttempt:
            tryCount--;
        }

        if (choices[0] == HERO_SKILL_WISDOM
            || choices[1] == HERO_SKILL_WISDOM) {
            m_enabled = static_cast<u8>(nLevel);
        }

        if (!gbInNewGameSetup && m_owner >= 0 && gbThisNetHumanPlayer[m_owner]) {
            samp = LoadPlaySample(const_cast<char*>("nwherolv.82m"));
            if (choices[0] == HERO_SKILL_NONE) {
                NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
            } else if (choices[1] == HERO_SKILL_NONE) {
                sprintf(
                    text,
                    "\n\n\xc2\xe0\xf8 \xe3\xe5\xf0\xee\xe9 \xe8\xe7\xf3\xf7\xe8\xeb "
                    "%s %s." /* "\n\nВаш герой изучил %s %s." */,
                    gSecondarySkillLevels[IDX(m_secondarySkills[IDX(choices[0])])],
                    gSecondarySkills[IDX(choices[0])]
                );
                strcat(gText, text);
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_INFO,
                    -1,
                    -1,
                    NORMAL_DIALOG_SECONDARY_SKILL,
                    IDX(choices[0]) * HERO_SECONDARY_SKILL_ICON_STRIDE
                        + IDX(m_secondarySkills[IDX(choices[0])]),
                    -1,
                    0,
                    -1,
                    0
                );
                GiveSS(choices[0], HERO_SKILL_LEVEL_BASIC);
            } else {
                sprintf(
                    text,
                    "\n\n\xc2\xfb \xf2\xe0\xea\xe6\xe5 \xec\xee\xe6\xe5\xf2\xe5 "
                    "\xe2\xfb\xf3\xf7\xe8\xf2\xfc %s %s \xe8\xeb\xe8 %s %s."
                    /* "\n\nВы также можете выучить %s %s или %s %s." */,
                    gSecondarySkills[IDX(choices[0])],
                    gSecondarySkillLevels[IDX(m_secondarySkills[IDX(choices[0])])],
                    gSecondarySkills[IDX(choices[1])],
                    gSecondarySkillLevels[IDX(m_secondarySkills[IDX(choices[1])])]
                );
                strcat(gText, text);
                NormalDialog(
                    gText,
                    NORMAL_DIALOG_DISABLE_SEVENTH,
                    -1,
                    -1,
                    NORMAL_DIALOG_SECONDARY_SKILL,
                    IDX(choices[0]) * HERO_SECONDARY_SKILL_ICON_STRIDE
                        + IDX(m_secondarySkills[IDX(choices[0])]),
                    NORMAL_DIALOG_SECONDARY_SKILL,
                    IDX(choices[1]) * HERO_SECONDARY_SKILL_ICON_STRIDE
                        + IDX(m_secondarySkills[IDX(choices[1])]),
                    -1,
                    0
                );
                if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_SEVEN)
                    GiveSS(choices[0], HERO_SKILL_LEVEL_BASIC);
                else
                    GiveSS(choices[1], HERO_SKILL_LEVEL_BASIC);
            }
        } else {
            if (choices[0] != HERO_SKILL_NONE) {
                if (choices[1] != HERO_SKILL_NONE) {
                    if (gSSValues[IDX(choices[0])][0]
                        > gSSValues[IDX(choices[1])][0]) {
                        GiveSS(choices[0], HERO_SKILL_LEVEL_BASIC);
                    } else {
                        GiveSS(choices[1], HERO_SKILL_LEVEL_BASIC);
                    }
                } else {
                    GiveSS(choices[0], HERO_SKILL_LEVEL_BASIC);
                }
            }
        }
    }
    m_level = static_cast<i16>(newLevel);
    WaitEndSample(&samp, -1);
}

VA(0x0046276b, 0x4b)
i32 hero::NumArtifacts(void) {
    i32 cnt = 0;
    i32 i;

    for (i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (m_artifacts[i] >= ARTIFACT_VALID_BEGIN)
            cnt++;
    }
    return cnt;
}

VA(0x004627b6, 0x742)
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
                if (gpHVHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE)
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                        gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[armySlot])]
                    );
                else
                    strcpy(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            } else if (giHeroScreenSrcIndex == armySlot) {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                    gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[armySlot])]
                );
            } else if (gpTownManager->m_castleDialogActive != 0) {
                if (gpHVHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE)
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SELECT_ARMY)],
                        gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[armySlot])]
                    );
                else
                    strcpy(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            } else if (gpHVHero->m_army.m_creatureTypes[armySlot] == CREATURE_NONE) {
                if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_SHIFT_KEYS))
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SPLIT_ARMY)],
                        gArmyNamesPlural[IDX(gpHVHero->m_army
                                                 .m_creatureTypes[giHeroScreenSrcIndex])]
                    );
                else
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_MOVE_ARMY)],
                        gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex])]
                    );
            } else if (gpHVHero->m_army.m_creatureTypes[armySlot]
                       == gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]) {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_COMBINE_ARMIES)],
                    gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[armySlot])]
                );
            } else {
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_EXCHANGE_ARMIES)],
                    gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex])],
                    gArmyNamesPlural[IDX(gpHVHero->m_army.m_creatureTypes[armySlot])]
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
                == ARTIFACT_NONE)
                sprintf(gText, cHeroScreen[IDX(TEXT_EMPTY)]);
            else if (gpHVHero->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST]
                     == ARTIFACT_MAGIC_BOOK)
                strcpy(gText, cHeroScreen[IDX(TEXT_VIEW_SPELLS)]);
            else
                sprintf(
                    gText,
                    cHeroScreen[IDX(TEXT_ARTIFACT)],
                    gArtifactNames
                        [IDX(gpHVHero->m_artifacts[message.payload.widget.id - UI_ARTIFACT_FIRST])]
                );
            break;

        case UI_DISMISS:
            sprintf(
                gText,
                cHeroScreen[IDX(TEXT_DISMISS)],
                gpHVHero->m_name,
                gAlignmentNames[IDX(gpHVHero->m_cursorType)]
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
            if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW3_FIRST
                && message.payload.widget.id < UI_SECONDARY_SKILL_ROW3_LAST + 1) {
                secondarySkillSlot = message.payload.widget.id - UI_SECONDARY_SKILL_ROW3_FIRST;

            secondary_skill_text:
                if (secondarySkillSlot < gpHVHero->m_secondarySkillCount) {
                    sprintf(
                        gText,
                        cHeroScreen[IDX(TEXT_SECONDARY_SKILL)],
                        gSecondarySkillLevels[IDX(
                                                  gpHVHero->m_secondarySkills
                                                      [IDX(gpHVHero->GetNthSS(secondarySkillSlot))]
                                              )
                                              - 1],
                        gSecondarySkills[IDX(gpHVHero->GetNthSS(secondarySkillSlot))]
                    );
                    break;
                }
            }
            strcpy(gText, cHeroScreen[IDX(TEXT_SCREEN)]);
            break;
    }
    HeroMessageUpdate(gText);
}

VA(0x00462ef8, 0x9ba)
MessageDispatchResult HeroHandler(struct tag_message& message) {
    i32 tmp;
    i32 quickView;
    i32 armySlot;
    i32 dummy;
    i32 bExit = 0;
    i32 heroLevel;
    i32 iHero;
    i32 secondarySkillSlot;
    tag_message newMsg;
    i32 j;
    i32 nextExperience;

    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
        quickView = 1;
    else
        quickView = 0;

    if (message.type == HERO_UI_HOVER) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
            return MESSAGE_DISPATCH_CONSUME;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;
        UpdateHeroScreenStatusBar(message);
        return MESSAGE_DISPATCH_CONSUME;
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
                if (quickView == 0) {
                    switch (message.payload.widget.id) {
                        case UI_DISMISS:
                            if (gpHVHero->Dismiss())
                                bExit = 1;
                            break;
                        case UI_CLOSE:
                            bExit = 1;
                            break;
                        case UI_PREVIOUS_HERO:
                        case UI_NEXT_HERO: {
                            if (gpHVHero->m_owner != giCurPlayer) {
                            } else {
                                if (gpCurPlayer->m_heroCount <= UI_HERO_CYCLE_MIN_COUNT - 1) {
                                } else {
                                    iHero =
                                        gpGame->HeroIDToHeroPos(gpCurPlayer, gpHVHero->m_id);
                                    iHero =
                                        (iHero + gpCurPlayer->m_heroCount
                                         + (message.payload.widget.id == UI_PREVIOUS_HERO ? -1 : 1))
                                        % gpCurPlayer->m_heroCount;
                                    gpHVHero =
                                        &gpGame->m_heroRecs[gpCurPlayer->m_heroIds[iHero]];
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
                            quickView
                        );
                        break;

                    case UI_MORALE_FIRST:
                    case UI_MORALE_MIDDLE:
                    case UI_MORALE_LAST:
                        gpGame->ShowMoraleInfo(
                            gpHVHero,
                            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW
                        );
                        break;

                    case UI_LUCK_FIRST:
                    case UI_LUCK_MIDDLE:
                    case UI_LUCK_LAST:
                        gpGame->ShowLuckInfo(
                            gpHVHero,
                            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW
                        );
                        break;

                    case UI_FORMATION_SPREAD:
                        if (quickView) {
                            NormalDialog(
                                "{\xd8\xe8\xf0\xee\xea\xe8\xe5 \xf0\xff\xe4\xfb}\n\n\xcf"
                                "\xf0\xe8 \xf2\xe0\xea\xee\xec \xe1\xee\xe5\xe2\xee\xec "
                                "\xef\xee\xf0\xff\xe4\xea\xe5 \xe2\xe0\xf8\xe5 \xe2\xee\xe9"
                                "\xf1\xea\xee \xe7\xe0\xed\xe8\xec\xe0\xe5\xf2 \xef\xee\xe7"
                                "\xe8\xf6\xe8\xe8 \xef\xee \xe2\xf1\xe5\xe9 \xf8\xe8\xf0"
                                "\xe8\xed\xe5 \xef\xee\xeb\xff \xe1\xee\xff \xe8 \xec\xe5"
                                "\xe6\xe4\xf3 \xf1\xee\xf1\xe5\xe4\xed\xe8\xec\xe8 \xee\xf2"
                                "\xf0\xff\xe4\xe0\xec\xe8 \xe8\xec\xe5\xe5\xf2\xf1\xff \xf5"
                                "\xee\xf2\xff \xe1\xfb \xee\xe4\xed\xe0 \xef\xf3\xf1\xf2"
                                "\xe0\xff \xea\xeb\xe5\xf2\xea\xe0."
                                /* "{Широкие ряды}\n\nПри таком боевом порядке ваше войско занимает позиции по всей ширине поля боя и между соседними отрядами имеется хотя бы одна пустая клетка." */,
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
                            gpHVHero->m_eventFlags = HeroEventFlag(
                                static_cast<i32>(gpHVHero->m_eventFlags)
                                & ~IDX(HERO_EVENT_GROUPED_FORMATION)
                            );
                            SetupHeroView();
                            RedrawHeroScreen();
                        }
                        break;

                    case UI_FORMATION_GROUPED:
                        if (quickView) {
                            NormalDialog(
                                "{\xcf\xeb\xee\xf2\xed\xfb\xe5 \xf0\xff\xe4\xfb}\n\n\xcf"
                                "\xf0\xe8 \xf2\xe0\xea\xee\xec \xe1\xee\xe5\xe2\xee\xec "
                                "\xef\xee\xf0\xff\xe4\xea\xe5 \xf0\xff\xe4\xfb \xe2\xe0\xf8"
                                "\xe5\xe9 \xe0\xf0\xec\xe8\xe8 \xf1\xec\xfb\xea\xe0\xfe\xf2"
                                "\xf1\xff \xe2\xee\xea\xf0\xf3\xe3 \xf6\xe5\xed\xf2\xf0\xe0"
                                "\xeb\xfc\xed\xee\xe3\xee \xee\xf2\xf0\xff\xe4\xe0 \xed\xe0"
                                " \xe2\xe0\xf8\xe5\xec \xea\xf0\xe0\xfe \xef\xee\xeb\xff "
                                "\xe1\xee\xff."
                                /* "{Плотные ряды}\n\nПри таком боевом порядке ряды вашей армии смыкаются вокруг центрального отряда на вашем краю поля боя." */,
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
                            gpHVHero->m_eventFlags = HeroEventFlag(
                                static_cast<i32>(gpHVHero->m_eventFlags)
                                | IDX(HERO_EVENT_GROUPED_FORMATION)
                            );
                            SetupHeroView();
                            RedrawHeroScreen();
                        }
                        break;

                    case UI_SPELL_POINTS_FIRST:
                    case UI_SPELL_POINTS_LAST:
                        sprintf(
                            gText,
                            "{\xce\xf7\xea\xe8 \xec\xe0\xe3\xe8\xe8}\n\n%s \xf1\xe5\xe9\xf7"
                            "\xe0\xf1 \xf0\xe0\xf1\xef\xee\xeb\xe0\xe3\xe0\xe5\xf2 %d \xee"
                            "\xf7\xea\xe0\xec\xe8 \xec\xe0\xe3\xe8\xe8 \xe8\xe7 \xe2\xee"
                            "\xe7\xec\xee\xe6\xed\xfb\xf5 %d \xee\xf7. \xcc\xe0\xea\xf1\xe8"
                            "\xec\xe0\xeb\xfc\xed\xee \xe2\xee\xe7\xec\xee\xe6\xed\xee\xe5 "
                            "\xf7\xe8\xf1\xeb\xee \xee\xf7\xea\xee\xe2 \xec\xe0\xe3\xe8\xe8"
                            " \xf0\xe0\xe2\xed\xee \xf3\xf0\xee\xe2\xed\xfe \xe7\xed\xe0"
                            "\xed\xe8\xe9 \xef\xee\xec\xed\xee\xe6\xe5\xed\xed\xee\xec\xf3 "
                            "\xed\xe0 10. \xcd\xee \xe8\xed\xee\xe3\xe4\xe0, \xe2 \xee\xf1"
                            "\xee\xe1\xfb\xf5 \xf1\xeb\xf3\xf7\xe0\xff\xf5, \xea\xee\xeb"
                            "\xe8\xf7\xe5\xf1\xf2\xe2\xee \xee\xf7\xea\xee\xe2 \xec\xe0\xe3"
                            "\xe8\xe8 \xec\xee\xe6\xe5\xf2 \xef\xf0\xe5\xe2\xfb\xf8\xe0\xf2"
                            "\xfc \xee\xe1\xfb\xf7\xed\xfb\xe9 \xeb\xe8\xec\xe8\xf2."
                            /* "{Очки магии}\n\n%s сейчас располагает %d очками магии из возможных %d оч. Максимально возможное число очков магии равно уровню знаний помноженному на 10. Но иногда, в особых случаях, количество очков магии может превышать обычный лимит." */,
                            gpHVHero->m_name,
                            gpHVHero->m_spellPoints,
                            gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE)
                                * HERO_SPELL_POINTS_PER_KNOWLEDGE
                        );
                        NormalDialog(
                            gText,
                            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
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
                        heroLevel = gpHVHero->GetLevel(gpHVHero->m_experience);
                        nextExperience = gpHVHero->GetExperience(heroLevel + 1);
                        sprintf(
                            gText,
                            "{%d \xf3\xf0\xee\xe2\xe5\xed\xfc}\n\n\xd2\xe5\xea\xf3\xf9\xe8"
                            "\xe9 \xee\xef\xfb\xf2: %d\n\xd1\xeb\xe5\xe4\xf3\xfe\xf9\xe8"
                            "\xe9 \xf3\xf0\xee\xe2\xe5\xed\xfc: %d"
                            /* "{%d уровень}\n\nТекущий опыт: %d\nСледующий уровень: %d" */,
                            heroLevel,
                            gpHVHero->m_experience,
                            nextExperience
                        );
                        NormalDialog(
                            gText,
                            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
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
                    case UI_ARMY_SELECTOR_SLOT_4:
                        armySlot = message.payload.widget.id - UI_ARMY_SELECTOR_FIRST;
                        if (quickView == 0 && giHeroScreenSrcIndex == UI_ARMY_SELECTION_NONE) {
                            if (gpHVHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE) {
                                giHeroScreenSrcIndex = armySlot;
                                gpHVHero->HeroScreenUpdate();
                            }
                        } else if ((quickView != 0
                                    && gpHVHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE)
                                   || (quickView == 0
                                       && giHeroScreenSrcIndex
                                              == message.payload.widget.id
                                                     - UI_ARMY_SELECTOR_FIRST)) {
                            gpGame->ViewArmy(
                                UI_VIEW_ARMY_X,
                                UI_VIEW_ARMY_Y,
                                gpHVHero->m_army.m_creatureTypes[armySlot],
                                gpHVHero->m_army.m_creatureCounts[armySlot],
                                NULL,
                                quickView == 0
                                        && gpTownManager->m_castleDialogActive
                                               != UI_CASTLE_DIALOG_ACTIVE
                                        && gpHVHero->m_army.GetNumArmies() != 1
                                    ? 0
                                    : 1,
                                ARMY_FACING_RIGHT,
                                quickView,
                                gpHVHero,
                                NULL,
                                &gpHVHero->m_army,
                                armySlot
                            );
                            if (quickView == 0)
                                giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
                            SetupHeroView();
                            RedrawHeroScreen();
                        } else {
                            if (quickView == 0 && gpTownManager->m_castleDialogActive != 0) {
                                if (gpHVHero->m_army.m_creatureTypes[armySlot] != CREATURE_NONE) {
                                    giHeroScreenSrcIndex = armySlot;
                                    gpHVHero->HeroScreenUpdate();
                                }
                            } else if (quickView == 0) {
                                tmp = IDX(gpHVHero->m_army.m_creatureTypes[armySlot]);
                                if (HAS(
                                        message.payload.widget.modifiers,
                                        MESSAGE_MODIFIER_SHIFT_KEYS
                                    )
                                    && (gpHVHero->m_army.m_creatureTypes[armySlot] == CREATURE_NONE
                                        || gpHVHero->m_army.m_creatureTypes[armySlot]
                                               == gpHVHero->m_army
                                                      .m_creatureTypes[giHeroScreenSrcIndex])) {
                                    DoHeroSplit(armySlot, giHeroScreenSrcIndex);
                                } else if (gpHVHero->m_army.m_creatureTypes[armySlot]
                                           == gpHVHero->m_army
                                                  .m_creatureTypes[giHeroScreenSrcIndex]) {
                                    gpHVHero->m_army.m_creatureCounts[armySlot] +=
                                        gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                    gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] = 0;
                                    gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                        CREATURE_NONE;
                                } else {
                                    gpHVHero->m_army.m_creatureTypes[armySlot] =
                                        gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex];
                                    gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                        static_cast<CreatureType>(tmp);
                                    tmp = gpHVHero->m_army.m_creatureCounts[armySlot];
                                    gpHVHero->m_army.m_creatureCounts[armySlot] =
                                        gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                    gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] =
                                        static_cast<i16>(tmp);
                                }
                                giHeroScreenSrcIndex = UI_ARMY_SELECTION_NONE;
                                gpHVHero->HeroScreenUpdate();
                            }
                        }
                        if (quickView == 0) {
                            gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
                            UpdateHeroScreenStatusBar(message);
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
                            != ARTIFACT_NONE) {
                            if (quickView == 0
                                && gpHVHero->m_artifacts
                                           [message.payload.widget.id - UI_ARTIFACT_FIRST]
                                       == ARTIFACT_MAGIC_BOOK) {
                                gpGame->ViewSpells(
                                    gpHVHero,
                                    SPELL_TYPE_ALL,
                                    ViewSpecialHandler,
                                    UI_VIEW_SPELLS_SPECIAL
                                );
                            } else {
                                gpHVHero->ViewArtifact(
                                    gpHVHero->m_artifacts
                                        [message.payload.widget.id - UI_ARTIFACT_FIRST],
                                    quickView,
                                    gpHVHero->m_artifactExtra
                                        [message.payload.widget.id - UI_ARTIFACT_FIRST]
                                );
                            }
                        }
                        break;

                    default:
                        if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW1_FIRST
                            && message.payload.widget.id < UI_SECONDARY_SKILL_ROW2_FIRST) {
                            secondarySkillSlot =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW1_FIRST;
                            goto secondary_skill_dialog;
                        }
                        if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW2_FIRST
                            && message.payload.widget.id < UI_SECONDARY_SKILL_ROW3_FIRST) {
                            secondarySkillSlot =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW2_FIRST;
                            goto secondary_skill_dialog;
                        }
                        if (message.payload.widget.id >= UI_SECONDARY_SKILL_ROW3_FIRST
                            && message.payload.widget.id < UI_SECONDARY_SKILL_ROW3_LAST + 1) {
                            secondarySkillSlot =
                                message.payload.widget.id - UI_SECONDARY_SKILL_ROW3_FIRST;

                        secondary_skill_dialog:
                            if (secondarySkillSlot < gpHVHero->m_secondarySkillCount)
                                gpHVHero->DoSSLevelDialog(
                                    gpHVHero->GetNthSS(secondarySkillSlot),
                                    quickView
                                );
                        }
                        break;
                }
                break;
        }
    }

    if (bExit) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = UI_DIALOG_CLOSE_COMMAND;
        message.payload.widget.command = BaseWidgetCommand(UI_DIALOG_CLOSE_COMMAND);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004638b2, 0x44)
void RedrawHeroScreen(void) {
    gpResourceManager->GetBackdrop("herobkg.icn", gpWindowManager->m_screen, UI_BACKDROP_PALETTE);
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, UI_SCREEN_WIDTH, UI_SCREEN_HEIGHT);
}

VA(0x004638f6, 0x219)
i32 HeroView(i32 heroId, i32 noDismiss, i32 fadeAlreadyOut) {
    mapCell* heroCell;

    gbNoDismiss = noDismiss;
    iOrigHeroViewID = heroId;
    gpAdvManager->TrimLoopingSounds(UI_LOOPING_SOUND_KEEP_COUNT);
    gpHVHero = gpGame->GetHero(heroId);
    gbHeroWindShowing = true;
    if (fadeAlreadyOut == 0)
        gpWindowManager->FadeScreen(FADE_OUT, UI_FADE_STEPS, NULL);

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
    gpWindowManager->FadeScreen(FADE_IN, UI_FADE_STEPS, NULL);
    gpWindowManager->DoDialog(heroWin, HeroHandler, 0);
    gpWindowManager->FadeScreen(FADE_OUT, UI_FADE_STEPS, NULL);
    delete heroWin;
    gheroWin = NULL;

    if (gpWindowManager->m_dialogResult == UI_DIALOG_DISMISS) {
        return UI_VIEW_DISMISSED;
    } else {
        gpHVHero->m_mobility = gpHVHero->CalcMobility();
    }
    gbHeroWindShowing = false;
    gpHVHero = NULL;
    return UI_VIEW_CLOSED;
}

VA(0x00463b0f, 0x906)
void SetupHeroView(void) {
    i32 tempBonus;
    i32 bNoDismiss;
    tag_message msg;
    tag_message statusMsg;
    i32 i;
    i32 displayIndex;
    i32 heroLuck;
    i32 magnitude;
    i32 moraleValue;
    HeroSecondarySkill skill;

    bNoDismiss = gbNoDismiss;
    if (gpHVHero->m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
        bNoDismiss = 1;

    msg.type = HERO_UI_MESSAGE;
    sprintf(gText, "%s - %s", gpHVHero->m_name, gAlignmentNames[IDX(gpHVHero->m_cursorType)]);
    msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
    msg.payload.widget.id = UI_HERO_TITLE;
    msg.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(msg);

    if (gpHVHero->m_owner != giCurPlayer || gpCurPlayer->m_heroCount == UI_SINGLE_HERO_COUNT) {
        msg.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        msg.payload.widget.data.value = UI_CYCLE_BUTTON_DISABLED_FRAME;
        msg.payload.widget.id = UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(msg);
        msg.payload.widget.id = UI_NEXT_HERO;
        heroWin->BroadcastMessage(msg);
        msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        msg.payload.widget.data.value = UI_CYCLE_BUTTON_FRAME;
        msg.payload.widget.id = UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(msg);
        msg.payload.widget.id = UI_NEXT_HERO;
        heroWin->BroadcastMessage(msg);
    }

    msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    msg.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    for (i = 0; i < UI_ARMY_SLOT_COUNT; i++) {
        msg.payload.widget.id = UI_PRIMARY_STAT_FIRST + i;
        heroWin->BroadcastMessage(msg);
        msg.payload.widget.id = UI_ARMY_SELECTOR_FIRST + i;
        heroWin->BroadcastMessage(msg);
    }

    if (bNoDismiss != 0 || gpTownManager->m_castleDialogActive != 0
        || (gpCurPlayer->m_townCount == 0 && gpCurPlayer->m_heroCount == UI_SINGLE_HERO_COUNT))
        msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    else
        msg.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    msg.payload.widget.id = UI_DISMISS;
    msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
    heroWin->BroadcastMessage(msg);

    sprintf(gText, "port%04d.icn", IDX(gpHVHero->m_portrait));
    msg.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    msg.payload.widget.id = UI_HERO_PORTRAIT;
    msg.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(msg);

    msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
    for (i = 0; i < HERO_PRIMARY_STAT_COUNT; i++) {
        sprintf(gText, "%d", gpHVHero->Stats(HeroPrimaryStat(i)));
        msg.payload.widget.id = UI_PRIMARY_STAT_VALUE_FIRST + i;
        msg.payload.widget.data.text = gText;
        heroWin->BroadcastMessage(msg);
    }

    heroLuck = gpGame->GetLuck(gpHVHero, NULL, gpHVHero->GetOccupiedTown());
    magnitude = abs(heroLuck);
    if (magnitude <= 0)
        magnitude = UI_MIN_MODIFIER_ICONS;
    for (i = 0; i < UI_MODIFIER_ICON_COUNT; i++) {
        msg.payload.widget.command =
            i >= magnitude ? HERO_UI_WIDGET_DISABLE : HERO_UI_WIDGET_ENABLE;
        if (i == 1 && heroLuck != 0)
            displayIndex = 0;
        else if (i == 0 && heroLuck != 0)
            displayIndex = 1;
        else
            displayIndex = i;
        msg.payload.widget.id = UI_LUCK_FIRST + displayIndex;
        msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(msg);
    }
    for (i = 0; i < UI_MODIFIER_ICON_COUNT; i++) {
        msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
        msg.payload.widget.id = UI_LUCK_FIRST + i;
        if (heroLuck < 0)
            msg.payload.widget.data.value = UI_LUCK_NEGATIVE_FRAME;
        else if (heroLuck == 0)
            msg.payload.widget.data.value = UI_LUCK_NEUTRAL_FRAME;
        else
            msg.payload.widget.data.value = UI_LUCK_POSITIVE_FRAME;
        heroWin->BroadcastMessage(msg);
    }

    moraleValue = gpHVHero->m_army.GetMorale(gpHVHero, gpHVHero->GetOccupiedTown(), NULL);
    magnitude = abs(moraleValue);
    if (magnitude <= 0)
        magnitude = UI_MIN_MODIFIER_ICONS;
    for (i = 0; i < UI_MODIFIER_ICON_COUNT; i++) {
        msg.payload.widget.command =
            i >= magnitude ? HERO_UI_WIDGET_DISABLE : HERO_UI_WIDGET_ENABLE;
        if (i == 1 && moraleValue != 0)
            displayIndex = 0;
        else if (i == 0 && moraleValue != 0)
            displayIndex = 1;
        else
            displayIndex = i;
        msg.payload.widget.id = UI_MORALE_FIRST + displayIndex;
        msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(msg);
    }
    for (i = 0; i < UI_MODIFIER_ICON_COUNT; i++) {
        msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
        msg.payload.widget.id = UI_MORALE_FIRST + i;
        if (moraleValue < 0)
            msg.payload.widget.data.value = UI_MORALE_NEGATIVE_FRAME;
        else if (moraleValue == 0)
            msg.payload.widget.data.value = UI_MORALE_NEUTRAL_FRAME;
        else
            msg.payload.widget.data.value = UI_MORALE_POSITIVE_FRAME;
        heroWin->BroadcastMessage(msg);
    }

    sprintf(gText, "%d", gpHVHero->m_experience);
    msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
    msg.payload.widget.id = UI_EXPERIENCE_LAST;
    msg.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(msg);

    msg.payload.widget.command = HAS(gpHVHero->m_eventFlags, HERO_EVENT_GROUPED_FORMATION)
        ? HERO_UI_WIDGET_DISABLE
        : HERO_UI_WIDGET_ENABLE;
    msg.payload.widget.id = UI_FORMATION_SPREAD_ICON;
    msg.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(msg);
    msg.payload.widget.command = HAS(gpHVHero->m_eventFlags, HERO_EVENT_GROUPED_FORMATION)
        ? HERO_UI_WIDGET_ENABLE
        : HERO_UI_WIDGET_DISABLE;
    msg.payload.widget.id = UI_FORMATION_GROUPED_ICON;
    msg.payload.widget.data.value = UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(msg);

    sprintf(
        gText,
        "%d/%d",
        gpHVHero->m_spellPoints,
        gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE) * HERO_SPELL_POINTS_PER_KNOWLEDGE
    );
    msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
    msg.payload.widget.id = UI_SPELL_POINTS_LAST;
    msg.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(msg);

    sprintf(gText, "crest.icn");
    msg.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    msg.payload.widget.id = UI_PLAYER_CREST;
    heroWin->BroadcastMessage(msg);
    msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
    msg.payload.widget.id = UI_PLAYER_CREST;
    msg.payload.widget.data.value = gpCurPlayer->Color();
    heroWin->BroadcastMessage(msg);

    gpHVHero->UpdateArmies();
    for (i = 0; i < HERO_SECONDARY_SKILL_CAPACITY; i++) {
        if (i < gpHVHero->m_secondarySkillCount) {
            skill = gpHVHero->GetNthSS(i);
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW1_FIRST + i;
            msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
            msg.payload.widget.data.value = IDX(skill) + 1;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + i;
            msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + i;
            msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + i;
            msg.payload.widget.data.text = gSecondarySkills[IDX(skill)];
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_TEXT;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + i;
            tempBonus = gpHVHero->GetSSLevel(skill)
                                  - IDX(gpHVHero->m_secondarySkills[IDX(skill)]);
            if (tempBonus > 0) {
                sprintf(
                    gText,
                    "%s+%d",
                    gSecondarySkillLevels
                        [IDX(gpHVHero->m_secondarySkills[IDX(skill)]) - 1],
                    tempBonus
                );
            } else {
                sprintf(
                    gText,
                    "%s",
                    gSecondarySkillLevels[IDX(gpHVHero->m_secondarySkills[IDX(skill)]) - 1]
                );
            }
            msg.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(msg);
        } else {
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW1_FIRST + i;
            msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
            msg.payload.widget.data.value = UI_EMPTY_SKILL_FRAME;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW2_FIRST + i;
            msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            msg.payload.widget.id = UI_SECONDARY_SKILL_ROW3_FIRST + i;
            msg.payload.widget.data.value = UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(msg);
        }
    }

    for (i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        msg.payload.widget.id = UI_ARTIFACT_FIRST + i;
        if (gpHVHero->m_artifacts[i] != ARTIFACT_NONE) {
            msg.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            msg.payload.widget.data.value = UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
            msg.payload.widget.data.value = IDX(gpHVHero->m_artifacts[i]) + 1;
            heroWin->BroadcastMessage(msg);
        } else {
            msg.payload.widget.command = HERO_UI_WIDGET_FRAME;
            msg.payload.widget.data.value = UI_EMPTY_ARTIFACT_FRAME;
            heroWin->BroadcastMessage(msg);
            msg.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            msg.payload.widget.data.value = UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(msg);
        }
    }

    statusMsg.type = HERO_UI_MESSAGE;
    statusMsg.payload.widget.id = UI_ARMY_SELECTION_NONE;
    UpdateHeroScreenStatusBar(statusMsg);
}

VA(0x00464415, 0x2b6)
void DoHeroSplit(i32 destinationSlot, i32 sourceSlot) {
    i16 splitTextSlot = UI_SPLIT_TEXT;
    i16 splitAmountSlot = UI_SPLIT_AMOUNT;
    tag_message message;

    gpTownManager->m_heroWindow1 =
        new heroWindow(UI_SPLIT_WINDOW_X, UI_SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == NULL)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum = gpHVHero->m_army.m_creatureCounts[sourceSlot];

    message.type = HERO_UI_MESSAGE;
    sprintf(gText, "\xd1\xea\xee\xeb\xfc\xea\xee \xe2\xee\xe8\xed\xee\xe2 \xef\xe5\xf0\xe5\xed\xe5\xf1\xf2\xe8?");
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

    if (gpWindowManager->m_dialogResult == UI_DIALOG_SPLIT && gpTownManager->m_splitAmount != 0) {
        if (gpHVHero->m_army.m_creatureTypes[destinationSlot]
            == gpHVHero->m_army.m_creatureTypes[sourceSlot]) {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -= gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] += gpTownManager->m_splitAmount;
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] = CREATURE_NONE;
        } else {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -= gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] =
                static_cast<i16>(gpTownManager->m_splitAmount);
            gpHVHero->m_army.m_creatureTypes[destinationSlot] =
                gpHVHero->m_army.m_creatureTypes[sourceSlot];
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] = CREATURE_NONE;
        }
    }
}

VA(0x004646cb, 0x4f)
void hero::SetSS(
    H2_ENUM_PARAM(HeroSecondarySkill, i32) skill,
    H2_ENUM_PARAM(HeroSkillLevel, i32) level
) {
    if (level == HERO_SKILL_LEVEL_NONE)
        TakeSS(skill, IDX(HERO_SKILL_LEVEL_EXPERT));
    else if (m_secondarySkills[IDX(skill)] != HERO_SKILL_LEVEL_NONE)
        m_secondarySkills[IDX(skill)] = level;
    else
        GiveSS(skill, level);
}

VA(0x0046471a, 0xee)
i32 hero::TakeSS(H2_ENUM_PARAM(HeroSecondarySkill, i32) skill, i32 levels) {
    H2_ENUM_STORAGE(HeroSkillLevel, i32) oldLevel;
    H2_ENUM_STORAGE_STEPPED(HeroSecondarySkill, i32) otherSkill;

    oldLevel = m_secondarySkills[IDX(skill)];
    if (m_secondarySkills[IDX(skill)] != HERO_SKILL_LEVEL_NONE) {
        m_secondarySkills[IDX(skill)] -= levels;
        if (m_secondarySkills[IDX(skill)] < HERO_SKILL_LEVEL_NONE)
            m_secondarySkills[IDX(skill)] = HERO_SKILL_LEVEL_NONE;
        if (m_secondarySkills[IDX(skill)] == HERO_SKILL_LEVEL_NONE) {
            for (otherSkill = HERO_SKILL_PATHFINDING; otherSkill < HERO_SKILL_COUNT; otherSkill++) {
                if (m_secondarySkillOrder[IDX(otherSkill)] > m_secondarySkillOrder[IDX(skill)]) {
                    m_secondarySkillOrder[IDX(otherSkill)]--;
                }
            }
            m_secondarySkillOrder[IDX(skill)] = 0;
            m_secondarySkillCount--;
        }
    }
    return IDX(oldLevel) - IDX(m_secondarySkills[IDX(skill)]);
}

VA(0x00464808, 0xa9)
i32 hero::GiveSS(
    H2_ENUM_PARAM(HeroSecondarySkill, i32) skill,
    H2_ENUM_PARAM(HeroSkillLevel, i32) levels
) {
    H2_ENUM_STORAGE(HeroSkillLevel, i32) oldLevel;

    oldLevel = m_secondarySkills[IDX(skill)];
    if (m_secondarySkills[IDX(skill)] != HERO_SKILL_LEVEL_NONE) {
        m_secondarySkills[IDX(skill)] += IDX(levels);
    } else {
        if (m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY) {
            m_secondarySkills[IDX(skill)] = levels;
            m_secondarySkillCount++;
            m_secondarySkillOrder[IDX(skill)] = static_cast<u8>(m_secondarySkillCount);
        }
    }
    if (m_secondarySkills[IDX(skill)] > HERO_SKILL_LEVEL_EXPERT)
        m_secondarySkills[IDX(skill)] = HERO_SKILL_LEVEL_EXPERT;
    return IDX(m_secondarySkills[IDX(skill)]) - IDX(oldLevel);
}

VA(0x004648b1, 0x5a)
i32 hero::CreatureTypeCount(H2_ENUM_PARAM(CreatureType, i32) creatureType) {
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

VA(0x0046490b, 0x4b)
void hero::UpgradeCreatures(
    H2_ENUM_PARAM(CreatureType, i32) oldCreatureType,
    H2_ENUM_PARAM(CreatureType, i32) newCreatureType
) {
    i32 numberUpgraded = 0;
    i32 armySlot;

    for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT; armySlot++) {
        if (m_army.m_creatureTypes[armySlot] == oldCreatureType)
            m_army.m_creatureTypes[armySlot] = newCreatureType;
    }
}

VA(0x00464956, 0x49)
HeroSecondarySkill hero::GetNthSS(i32 ordinal) {
    HeroSecondarySkill skill;

    for (skill = HERO_SKILL_PATHFINDING; skill < HERO_SKILL_COUNT; skill++) {
        if (m_secondarySkillOrder[IDX(skill)] == ordinal + HERO_SECONDARY_SKILL_ORDER_BASE)
            return skill;
    }
    return HERO_SKILL_NONE;
}

VA(0x0046499f, 0x35)
class town* hero::GetOccupiedTown(void) {
    if (m_locationType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
        return gpGame->GetTown(m_occupiedTown);
    return NULL;
}

VA(0x004649d4, 0x2f)
i8 hero::Stats(HeroPrimaryStat stat) {
    if (stat == HERO_PRIMARY_SPELL_POWER && m_primaryStats[IDX(stat)] < HERO_MINIMUM_SPELL_POWER) {
        return HERO_MINIMUM_SPELL_POWER;
    }
    return m_primaryStats[IDX(stat)];
}

VA(0x00464a03, 0x9b)
i8 hero::GetSSLevel(H2_ENUM_PARAM(HeroSecondarySkill, i32) skill) {
    i8 bonus = 0;
    i8 ssLevel;

    ssLevel = static_cast<i8>(m_secondarySkills[IDX(skill)]);
    if (skill != HERO_SKILL_NECROMANCY)
        return IDX(ssLevel);
    if (ssLevel == IDX(HERO_SKILL_LEVEL_NONE))
        return IDX(ssLevel);
    if (HasArtifact(ARTIFACT_SPADE_NECROMANCY))
        bonus++;
    if (m_cursorType == FACTION_NECROMANCER)
        bonus += gpGame->CountShrines(m_owner);
    if (bonus > HERO_NECROMANCY_BONUS_MAX)
        bonus = HERO_NECROMANCY_BONUS_MAX;
    ssLevel += bonus;
    if (ssLevel > HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX)
        ssLevel = HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX;
    return IDX(ssLevel);
}

VA(0x00464a9e, 0xdd)
void hero::DoSSLevelDialog(H2_ENUM_PARAM(HeroSecondarySkill, i32) skill, i32 quickView) {
    i32 skillBonusValue;
    char* skillText;

    skillBonusValue = GetSSLevel(skill) - IDX(m_secondarySkills[IDX(skill)]);
    if (skillBonusValue > 0) {
        skillText = gSecondarySkillLevels[IDX(m_secondarySkills[IDX(skill)]) - 1];
        sprintf(
            gText,
            "{%s Necromancy (+%d)}\n\n%s Necromancy (+%d) allows %d percent of the creatures "
            "killed in combat to be brought back from the dead as Skeletons.",
            skillText,
            skillBonusValue,
            skillText,
            skillBonusValue,
            GetSSLevel(skill) * HERO_NECROMANCY_PERCENT_PER_LEVEL
        );
    } else {
        sprintf(gText, cSecSkillDesc[IDX(skill)][IDX(m_secondarySkills[IDX(skill)]) - 1]);
    }
    NormalDialog(
        gText,
        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        NORMAL_DIALOG_NO_RESOURCE,
        NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_SECONDARY_SKILL,
        IDX(skill) * HERO_SECONDARY_SKILL_ICON_STRIDE + IDX(m_secondarySkills[IDX(skill)])
            - HERO_SECONDARY_SKILL_ICON_FRAME_BASE,
        NORMAL_DIALOG_NO_RESOURCE,
        0,
        NORMAL_DIALOG_NO_RESOURCE,
        0
    );
}

VA(0x00464b7b, 0x107)
void hero::CheckAnduranPieces(i32 showDialog) {
    i32 artifactSlot;

    if (HasArtifact(ARTIFACT_BREASTPLATE_ANDURAN) && HasArtifact(ARTIFACT_HELMET_ANDURAN)
        && HasArtifact(ARTIFACT_SWORD_ANDURAN)) {
        for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT; artifactSlot++) {
            if (m_artifacts[artifactSlot] == ARTIFACT_BREASTPLATE_ANDURAN
                || m_artifacts[artifactSlot] == ARTIFACT_HELMET_ANDURAN
                || m_artifacts[artifactSlot] == ARTIFACT_SWORD_ANDURAN) {
                GiveTakeArtifactStat(this, m_artifacts[artifactSlot], EVENT_ARTIFACT_TAKE);
                m_artifacts[artifactSlot] = ARTIFACT_NONE;
            }
        }
        GiveArtifact(this, ARTIFACT_BATTLE_GARB, showDialog, IDX(ARTIFACT_NONE));
        if (gbThisNetHumanPlayer[m_owner]) {
            LoadPlaySample("treasure.82m");
            NormalDialog(
                "\xd2\xf0\xe8 \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2\xe0 \xc0\xed\xe4\xf3\xf0\xe0\xed\xe0 \xf7\xf3\xe4\xe5\xf1\xed\xfb\xec \xf1\xef\xee\xf1\xee\xe1\xee\xec \xee\xe1\xfa\xe5\xe4\xe8\xed\xe8\xeb\xe8\xf1\xfc \xe2 \xee\xe4\xe8\xed \xe0\xf0\xf2\xe5\xf4\xe0\xea\xf2.",
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

DATA(0x005249b4) class hero* gpHVHero = NULL;
DATA(0x005249b8) class heroWindow* gheroWin = NULL;
DATA(0x004f7c64) i16 gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT] =
    {0, 1000, 2000, 3200, 4500, 6000, 7700, 9000, 11000, 13200, 15500, 18500};
DATA(0x005249ac) i32 iOrigHeroViewID;
DATA(0x005249b0) b32 gbNoDismiss;
