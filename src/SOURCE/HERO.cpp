// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HERO.OBJ   from: (directly linked into exe)
// functions: 41   data: 5
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <_types.h>
#include <_carcass_types.h>
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

VA(0x0046c3a0, 0x6f)
hero::hero(void) {
    m_id = 0;
    m_owner = 0;
    m_x = 0;
    m_y = 0;
    m_cursorType = 0;
    m_portrait = 0;
    m_name[0] = 0;
    heroWin = 0;
    giHeroScreenSrcIndex = HERO_UI_ARMY_SELECTION_NONE;
}

VA(0x0046c40f, 0x53)
void hero::Read(int file, signed char expansion) {
    if (expansion)
        read(file, this, sizeof(hero));
    else
        read(file, this, HERO_BASE_RECORD_SIZE);
}

VA(0x0046c462, 0x53)
void hero::Write(int file, signed char expansion) {
    if (expansion)
        write(file, this, sizeof(hero));
    else
        write(file, this, HERO_BASE_RECORD_SIZE);
}

VA(0x0046c4b5, 0x18)
void hero::GetArmyStrengths(unsigned long int * const) {}

VA(0x0046c4cd, 0x59)
int hero::HasArtifact(int artifact) {
    int artifactIndex;

    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == artifact)
            return 1;
    }
    return 0;
}

VA(0x0046c526, 0x277)
int hero::CalcMobility(void) {
    short landMobility[8] = {
        1000, 1000, 1000, 1100, 1200, 1300, 1400, 1500
    };
    const short seaBaseMobilityCurrent = HERO_SEA_BASE_MOBILITY;
    const short lighthouseBonusIncrement = HERO_LIGHTHOUSE_MOBILITY_BONUS;
    const short astrolabeBonus = HERO_ASTROLABE_MOBILITY_BONUS;
    const short compassMobility = HERO_COMPASS_MOBILITY_BONUS;
    const short nomadBootsMobilityBonus = HERO_NOMAD_BOOTS_MOBILITY_BONUS;
    const short travelerBonus = HERO_TRAVELER_BOOTS_MOBILITY_BONUS;
    int mobilityResult;
    int slowestSpeedValue;
    int armySlotIndex;

    if (m_eventFlags & HERO_EVENT_EMBARKED) {
        mobilityResult = seaBaseMobilityCurrent;
        mobilityResult = static_cast<int>(mobilityResult *
            gfSSNavigationMod[m_secondarySkills[HERO_SKILL_NAVIGATION]]);
        if (m_owner != -1)
            mobilityResult += gpGame->MineTypesOwned(m_owner, HERO_LIGHTHOUSE_MINE_TYPE) *
                lighthouseBonusIncrement;
        if (HasArtifact(HERO_ARTIFACT_SAILORS_ASTROLABE))
            mobilityResult += astrolabeBonus;
    } else {
        slowestSpeedValue = 7;
        for (armySlotIndex = 0; armySlotIndex < ARMY_GROUP_SLOT_COUNT; armySlotIndex++) {
            if (m_army.m_creatureTypes[armySlotIndex] != ARMY_GROUP_EMPTY_SLOT &&
                gMonsterDatabase[m_army.m_creatureTypes[armySlotIndex]].speed < slowestSpeedValue) {
                slowestSpeedValue = gMonsterDatabase[m_army.m_creatureTypes[armySlotIndex]].speed;
            }
        }
        mobilityResult = landMobility[slowestSpeedValue];
        mobilityResult = static_cast<int>(mobilityResult *
            gfSSLogisticsMod[m_secondarySkills[HERO_SKILL_LOGISTICS]]);
        if (HasArtifact(HERO_ARTIFACT_NOMAD_BOOTS))
            mobilityResult += nomadBootsMobilityBonus;
        if (HasArtifact(HERO_ARTIFACT_TRAVELER_BOOTS))
            mobilityResult += travelerBonus;
        if (m_eventFlags & HERO_EVENT_STABLES)
            mobilityResult += HERO_STABLES_MOBILITY_BONUS;
    }

    if (HasArtifact(HERO_ARTIFACT_TRUE_COMPASS))
        mobilityResult += compassMobility;

    if (m_owner >= 0 && m_owner < 6 && !gbHumanPlayer[m_owner] &&
        gpGame->m_difficulty >= 2) {
        mobilityResult += HERO_AI_DIFFICULTY_MOBILITY_BONUS;
        if (gpGame->m_players[m_owner].m_aiDifficulty == 2)
            mobilityResult += HERO_AI_STATE_MOBILITY_BONUS;
    }
    return mobilityResult;
}

VA(0x0046c79d, 0xcf)
int hero::HasSpell(int spell) {
    int artifactIndex;

    if (!HasArtifact(HERO_ARTIFACT_MAGIC_BOOK))
        return 0;
    if (m_spells[spell])
        return 1;
    for (artifactIndex = 0; artifactIndex < HERO_ARTIFACT_SLOT_COUNT; artifactIndex++) {
        if (m_artifacts[artifactIndex] == HERO_ARTIFACT_SPELL_SCROLL &&
            m_artifactExtra[artifactIndex] == spell) {
            return 1;
        }
    }
    if (HasArtifact(HERO_ARTIFACT_BATTLE_GARB) && spell == HERO_SPELL_TOWN_PORTAL)
        return 1;
    return 0;
}

// @match-note 99.78%: the 0x0c frame, count/spell/this slots at -0x04/-0x08/-0x0c,
// CFG, and all 3/3 relocations agree. The first non-relocation residual is +0x9b:
// retail loads the count and compares the second argument, while this build loads
// the second argument and compares the count (two displacement bytes, same equality).
// Operand swaps, the AST permuter, semantic identifier changes, unary-plus,
// subtraction, and an explicit continue shape did not retain an improvement. The
// two gsSpellInfo relocations resolve to the same attributes bytes through retail's
// interior const_000fbe8d label; the remaining delta is TU code-selection noise.
VA(0x0046c86c, 0xc5)
int hero::GetNthSpell(int type, int spellNumber) {
    int spell;
    int spellOrdinalCount = 0;

    for (spell = 0; spell < HERO_SPELL_COUNT; spell++) {
        if (HasSpell(spell)) {
            if (type == HERO_SPELL_TYPE_ALL ||
                (type == HERO_SPELL_TYPE_COMBAT &&
                 (gsSpellInfo[spell].attributes & SPELL_ATTRIBUTE_COMBAT)) ||
                (type == HERO_SPELL_TYPE_ADVENTURE &&
                 !(gsSpellInfo[spell].attributes & SPELL_ATTRIBUTE_COMBAT))) {
                spellOrdinalCount++;
            }
            if (spellOrdinalCount == spellNumber)
                return spell;
        }
    }
    return HERO_SPELL_NONE;
}

// @early-stop
// All 0xd0 bytes match after masking the two aligned COFF relocations.
// Both relocation targets agree; retail delinks gsSpellInfo[0].attributes as the interior
// label const_000fbe8d while the typed source uses gsSpellInfo with addend 0x15.
VA(0x0046c931, 0xd0)
int hero::GetNumSpells(int type) {
    int numAdventureSpells;
    int numCombatSpells;
    int spellIndexCurrent;

    numCombatSpells = 0;
    numAdventureSpells = 0;
    for (spellIndexCurrent = 0; spellIndexCurrent < HERO_SPELL_COUNT; spellIndexCurrent++) {
        if (HasSpell(spellIndexCurrent)) {
            if (gsSpellInfo[spellIndexCurrent].attributes & SPELL_ATTRIBUTE_COMBAT)
                numCombatSpells++;
            else
                numAdventureSpells++;
        }
    }

    switch (type) {
    case HERO_SPELL_TYPE_COMBAT:
        return numCombatSpells;
    case HERO_SPELL_TYPE_ADVENTURE:
        return numAdventureSpells;
    case HERO_SPELL_TYPE_ALL:
        return numCombatSpells + numAdventureSpells;
    }
    return 0;
}

VA(0x0046ca01, 0x8a)
void hero::UseSpell(int spell) {
    if (spell == HERO_SPELL_NONE)
        return;

    m_spellPoints -= GetManaCost(spell, this);
    if (m_spellPoints < 0)
        m_spellPoints = 0;
    if (gpAdvManager->m_active == 1 && gbThisNetHumanPlayer[giCurPlayer])
        gpAdvManager->UpdateHeroLocator(-1, 1, 1);
}

VA(0x0046ca8b, 0x26)
void hero::AddSpell(int spell, int) {
    m_spells[spell] = 1;
}

VA(0x0046cab1, 0x82)
void HeroMessageUpdate(char *text) {
    tag_message message;

    if (gheroWin == 0)
        return;

    message.type = HERO_UI_MESSAGE;
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = HERO_UI_STATUS_TEXT_WIDGET;
    message.payload.widget.data.text = text;
    gheroWin->BroadcastMessage(message);
    gheroWin->DrawWindow(0, 300, 303);
    gpWindowManager->UpdateScreenRegion(0, 459, 640, 20);
}

// @match-note 97.27%: semantics, the 0x24 frame, message/index/this slots at
// -0x1c/-0x20/-0x24, CFG, and all 8/8 relocations agree. First divergence is +0x34:
// retail bytes `8b 45 e0; 39 05 <giHeroScreenSrcIndex>` load index then compare the
// global (9-byte span through +0x3c); base bytes `a1 <giHeroScreenSrcIndex>; 39 45 e0`
// load the global then compare index (8-byte span through +0x3b). The remaining
// instructions and operands agree, with branch displacements shifted by that byte.
// Tried `global == index`, `index == global`, and `0[&index] == global`; all retained
// the global-first form, leaving only this TU-cumulative load-order difference.
VA(0x0046cb33, 0xa8)
void hero::HeroScreenUpdate(void) {
    tag_message message;
    int index;

    message.type = HERO_UI_MESSAGE;
    UpdateArmies();
    for (index = 0; index < HERO_UI_ARMY_SLOT_COUNT; index++) {
        if (index == giHeroScreenSrcIndex)
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        else
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        message.payload.widget.data.value = HERO_UI_WIDGET_FRAME_ACTIVE;
        message.payload.widget.id = index + HERO_UI_ARMY_SELECTOR_FIRST;
        heroWin->BroadcastMessage(message);
    }
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, 640, 480);
}

VA(0x0046cbdb, 0x1d2)
void hero::UpdateArmies(void) {
    tag_message message;
    int index;

    message.type = HERO_UI_MESSAGE;
    for (index = 0; index < HERO_UI_ARMY_SLOT_COUNT; index++) {
        if (m_army.m_creatureTypes[index] == ARMY_GROUP_EMPTY_SLOT) {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = index + HERO_UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value = HERO_UI_ARMY_EMPTY_FRAME;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id = index + HERO_UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.value = HERO_UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = index + HERO_UI_ARMY_COUNT_FIRST;
            heroWin->BroadcastMessage(message);
            message.payload.widget.id = index + HERO_UI_ARMY_SELECTOR_FIRST;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.id = index + HERO_UI_ARMY_ICON_FIRST;
            message.payload.widget.data.value = gMonsterDatabase[m_army.m_creatureTypes[index]].race + 4;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
            sprintf(gText, "monh%04d.icn", m_army.m_creatureTypes[index]);
            message.payload.widget.id = index + HERO_UI_ARMY_PORTRAIT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = HERO_UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);

            sprintf(gText, "%d", m_army.m_creatureCounts[index]);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id = index + HERO_UI_ARMY_COUNT_FIRST;
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);

            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = HERO_UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
        }
    }
}

VA(0x0046cdad, 0x43)
void hero::ViewStat(int stat, int quickView) {
    NormalDialog(gStatDesc[stat], quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        -1, -1, -1, 0, -1, 0, -1, 0);
}

VA(0x0046cdf0, 0x9b)
void hero::ViewArtifact(int artifact, int quickView, int extra) {
    if (artifact == HERO_ARTIFACT_SPELL_SCROLL) {
        sprintf(gText, gArtifactDesc[artifact], gSpellNames[extra]);
        NormalDialog(gText, quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
            -1, HERO_UI_ARTIFACT_DIALOG_ICON, -1, 0, -1, 0, -1, 0);
    } else {
        NormalDialog(gArtifactDesc[artifact],
            quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
            -1, HERO_UI_ARTIFACT_DIALOG_ICON, -1, 0, -1, 0, -1, 0);
    }
}

VA(0x0046ce8b, 0x5d)
int hero::Dismiss(void) {
    NormalDialog("Are you sure you want to dismiss this Hero?", NORMAL_DIALOG_CONFIRM,
        -1, -1, -1, 0, -1, 0, -1, 0);
    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_FIVE) {
        Deallocate(1);
        return 1;
    }
    return 0;
}

// @match-note 96.65% live, 98.19% retained maximum: semantics, the 0x20 frame,
// player/playerHeroIndex/heroOwner/index/occupiedTown/availableHeroSlot/mapCell
// slots at -0x04/-0x08/-0x0c/-0x10/-0x14/
// -0x18/-0x1c, CFG, and all 44/44 relocation targets agree. First divergence is
// +0x122: retail has a five-byte inline continuation before the occupied-town lookup.
// Removing the alias-only event bitfields also changes the existing GetCell inline expression's
// instruction order while preserving its value and stack slot. Two later continuations still
// differ around cursor clearing/map-cell materialization. Retail expands both final flag masks
// to load/OR-or-AND/store, while whole-word compound and explicit assignments select memory
// operations. The removed 32-bit/split bitfield aliases were also tried; the residual is
// compiler-selected accessor and mask-operation shape.
// A post-95 40-walk AST pass retained no mutation. An isolated eight-trial
// TU-state sweep also found no audited exact closure; generated probes were
// discarded. Revisit only after a material accessor or shared-layout change.
VA(0x0046cee8, 0x587)
void hero::Deallocate(int updateMap) {
    int availableHeroSlotCurrent;
    playerData *player;
    int playerHeroIndex;
    int heroOwner;
    int index;
    town *occupiedTownValue;
    mapCell *mapCellRecord;

    if (updateMap)
        SendMapChange(MAP_CHANGE_DEAD_HERO, m_id, static_cast<unsigned char>(m_x),
            static_cast<unsigned char>(m_y), HERO_MAP_CHANGE_UNUSED, 0, 0);

    heroOwner = m_owner;
    player = &gpGame->m_players[m_owner];

    if (updateMap)
        gpAdvManager->MobilizeCurrHero(0);
    if (updateMap)
        gpAdvManager->HideRoute(0, 0, 0);

    if (m_eventFlags & HERO_EVENT_EMBARKED) {
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

    if (m_owner != giCurPlayer || gpGame->m_players[m_owner].m_currentHero != m_id ||
        gpAdvManager->m_heroContextLocked == 0) {
        gpGame->RestoreCell(m_x, m_y, m_locationType, m_occupiedTown, 0, 1);
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
        if (gpGame->m_heroRecs[gpGame->m_players[m_owner]
                                   .m_availableHeroIds[availableHeroSlotCurrent]]
                .m_eventFlags & HERO_EVENT_WEEKLY_VISIT) {
            availableHeroSlotCurrent = 1 - availableHeroSlotCurrent;
        }
        if (gpGame->m_availableHeroes[gpGame->m_players[m_owner]
                                          .m_availableHeroIds[availableHeroSlotCurrent]] ==
            HERO_AVAILABILITY_RETREATED) {
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

    if (gbInCampaign && m_portrait == CAMPAIGN_HERO_CORLAGON &&
        gpGame->m_campaignType == CAMPAIGN_ROLAND &&
        gpGame->m_campaignScenario + 1 == CAMPAIGN_ROLAND_FINAL_SCENARIO &&
        !gbRetreatWin && !gbCombatSurrender) {
        gpGame->m_campaignAwards[CAMPAIGN_AWARD_DEFEAT_CORLAGON] = 1;
    }

    if (updateMap)
        CheckEndGame(0, 0);
}

VA(0x0046d46f, 0x9e)
int hero::GetExperience(int level) {
    int experience;
    int levelCounter;
    int increment;

    if (level <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT)
        return gMinExpForLevel[level - 1];

    levelCounter = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    increment = static_cast<int>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] -
         gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 2]) *
        HERO_EXPERIENCE_GROWTH_FACTOR);
    experience = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + increment;
    while (levelCounter < level) {
        increment = static_cast<int>(increment * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience += increment;
        levelCounter++;
    }
    return experience;
}

VA(0x0046d50d, 0xc0)
int hero::GetLevel(int experienceValue) {
    int experience;
    int levelCounter;
    int increment;

    for (levelCounter = 1; levelCounter <= HERO_EXPERIENCE_LEVEL_TABLE_COUNT; levelCounter++) {
        if (experienceValue < gMinExpForLevel[levelCounter - 1])
            return levelCounter - 1;
    }

    increment = static_cast<int>(
        (gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] -
         gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 2]) *
        HERO_EXPERIENCE_GROWTH_FACTOR);
    experience = gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT - 1] + increment;
    levelCounter = HERO_EXPERIENCE_EXTRAPOLATION_FIRST_LEVEL;
    while (experience < experienceValue) {
        increment = static_cast<int>(increment * HERO_EXPERIENCE_GROWTH_FACTOR);
        experience += increment;
        levelCounter++;
    }
    return levelCounter - 1;
}

// @early-stop
// 69.40%: semantics, the 0x04 frame with this at -0x04, CFG, and the
// zero-relocation set agree. First divergence is +0x37: retail reads m_eventFlags,
// subtracts 0x20 in EAX, and writes it back (18 bytes after the common this load),
// while base emits a seven-byte memory SUB. The same 11-byte shortening repeats for
// all twelve cleared temporary flags; morale/luck updates, order, tests, and branches
// otherwise agree. Tried `-=`, explicit `field = field - flag`, addition of a negative
// flag, and a 32-bit bitfield declaration; all select the same memory SUB lowering.
VA(0x0046d5cd, 0x254)
void hero::ApplyBattleWinTemps(void) {
    m_lastTownInteractionTurn = HERO_INTERACTION_TURN_NONE;
    m_lastHeroInteractionTurn = HERO_INTERACTION_TURN_NONE;

    if (m_eventFlags & HERO_EVENT_GRAVEYARD) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_GRAVEYARD;
    }
    if (m_eventFlags & HERO_EVENT_SHIPWRECK) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_SHIPWRECK;
    }
    if (m_eventFlags & HERO_EVENT_BUOY) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_BUOY;
    }
    if (m_eventFlags & HERO_EVENT_OASIS) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_OASIS;
    }
    if (m_eventFlags & HERO_EVENT_TEMPLE) {
        m_morale -= 2;
        m_eventFlags = m_eventFlags - HERO_EVENT_TEMPLE;
    }
    if (m_eventFlags & HERO_EVENT_FAERIE_RING) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_FAERIE_RING;
    }
    if (m_eventFlags & HERO_EVENT_IDOL) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_IDOL;
    }
    if (m_eventFlags & HERO_EVENT_FOUNTAIN) {
        m_luck--;
        m_eventFlags = m_eventFlags - HERO_EVENT_FOUNTAIN;
    }
    if (m_eventFlags & HERO_EVENT_WATERING_HOLE) {
        m_morale--;
        m_eventFlags = m_eventFlags - HERO_EVENT_WATERING_HOLE;
    }
    if (m_eventFlags & HERO_EVENT_DERELICT_SHIP) {
        m_morale++;
        m_eventFlags = m_eventFlags - HERO_EVENT_DERELICT_SHIP;
    }
    if (m_eventFlags & HERO_EVENT_PYRAMID) {
        m_luck += 2;
        m_eventFlags = m_eventFlags - HERO_EVENT_PYRAMID;
    }
    if (m_eventFlags & HERO_EVENT_MERMAID) {
        m_luck = m_luck - 1;
        m_eventFlags = m_eventFlags - HERO_EVENT_MERMAID;
    }
}

VA(0x0046d821, 0x1e)
void hero::ApplyBattleLossTemps(void) {
    ApplyBattleWinTemps();
}

// @early-stop
// 99.52%: semantics, CFG, the 0x118 frame, line/statBonuses/newLevel/
// levelsGained/currentLevel/sample/index/highLevel/skillChoices/attempts/skillIndex/
// skillWeight/randomValue slots at -0xc8/-0xd8/-0xdc/-0xe0/-0xe4/-0xec..-0xe8/
// -0xf0/-0xf4/-0xfc..-0xf8/-0x100/-0x104/-0x108/-0x10c, and all 58/58
// relocation targets agree. First code divergence is +0x4be: retail adds a five-byte
// jump through its local trampoline at +0x81e, which adds the second five-byte jump
// back to the search-loop cleanup; base jumps directly to cleanup. Function sizes differ
// by exactly those 10 bytes and every non-jump opcode, operand, and visible stack offset
// agrees. Empty-positive/else outer flow, branch polarity, regular `break`, indexed Wisdom
// assignment, real table typing, and computed stack-name/declaration order were tried;
// only the byte-proven local-label/trampoline shape remains. A post-95
// 30-walk AST pass retained no mutation. Fresh masked disassembly reconfirmed
// the two jumps as the only non-relocation instruction differences. Ordered
// COFF audit proves the disputed +0xc3 relocation: base cHeroLevel+4 and retail
// ??_C@_03HHHK... both resolve to RVA 0xff29c. symbol_names classifies the
// retail identity as a pe-reloc-constant at that interior pointer slot; the
// retail PE stores 0x50cb24 there, the " a level.\n" cHeroLevel[1] string.
VA(0x0046d83f, 0x828)
void hero::CheckLevel(void) {
    int statBonuses[HERO_PRIMARY_STAT_COUNT];
    char line[HERO_LEVEL_TEXT_BUFFER_SIZE];
    int levelsGained;
    int newLevel;
    int attempts;
    int skillChoicesResult[HERO_SECONDARY_SKILL_CHOICE_COUNT];
    int highLevelIndex;
    int indexValue;
    SAMPLE2 sampleValue;
    int currentLevelIndex;
    int skillIndexValue;
    int randomValue;
    int skillWeightIndex;

    newLevel = GetLevel(m_experience);
    if (m_level == newLevel) {
    } else {
        sampleValue = NULL_SAMPLE2;
        levelsGained = newLevel - m_level;
        for (currentLevelIndex = m_level + 1; currentLevelIndex <= newLevel;
             currentLevelIndex++) {
            sprintf(gText, cHeroLevel[0], m_name);
            sprintf(line, cHeroLevel[1]);
            strcat(gText, line);

            statBonuses[HERO_PRIMARY_ATTACK] = 0;
            statBonuses[HERO_PRIMARY_DEFENSE] = 0;
            statBonuses[HERO_PRIMARY_SPELL_POWER] = 0;
            statBonuses[HERO_PRIMARY_KNOWLEDGE] = 0;
            if (currentLevelIndex <= HERO_LEVEL_HIGH_THRESHOLD)
                highLevelIndex = 0;
            else
                highLevelIndex = 1;

            SRand(m_randomSeed + currentLevelIndex * HERO_LEVEL_RANDOM_SEED_FACTOR);
            randomValue = SRandom(1, HERO_LEVEL_RANDOM_MAX);
            if (randomValue <
                gHeroSkillBonus[m_cursorType][highLevelIndex][HERO_PRIMARY_ATTACK]) {
                statBonuses[HERO_PRIMARY_ATTACK]++;
            } else {
                randomValue -=
                    gHeroSkillBonus[m_cursorType][highLevelIndex][HERO_PRIMARY_ATTACK];
                if (randomValue <
                    gHeroSkillBonus[m_cursorType][highLevelIndex][HERO_PRIMARY_DEFENSE]) {
                    statBonuses[HERO_PRIMARY_DEFENSE]++;
                } else {
                    randomValue -=
                        gHeroSkillBonus[m_cursorType][highLevelIndex][HERO_PRIMARY_DEFENSE];
                    if (randomValue <
                        gHeroSkillBonus[m_cursorType][highLevelIndex]
                                       [HERO_PRIMARY_SPELL_POWER]) {
                        statBonuses[HERO_PRIMARY_SPELL_POWER]++;
                    } else {
                        statBonuses[HERO_PRIMARY_KNOWLEDGE]++;
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

            for (indexValue = 0; indexValue < HERO_SECONDARY_SKILL_CHOICE_COUNT;
                 indexValue++) {
                skillChoicesResult[indexValue] = HERO_SECONDARY_SKILL_NONE;
                if (indexValue == 0 && m_cursorType != HERO_CLASS_BARBARIAN &&
                    m_cursorType != HERO_CLASS_KNIGHT &&
                    m_secondarySkills[HERO_SKILL_WISDOM] < HERO_SKILL_LEVEL_EXPERT &&
                    currentLevelIndex - m_enabled >= HERO_SECONDARY_SKILL_OFFER_GAP) {
                    skillChoicesResult[indexValue] = HERO_SKILL_WISDOM;
                } else {
                    attempts = 0;
                    skillWeightIndex = Random(0, HERO_SECONDARY_SKILL_RANDOM_WEIGHT);
                    skillIndexValue = 0;
                    while (attempts < HERO_SECONDARY_SKILL_SEARCH_LIMIT) {
                        attempts++;
                        if ((indexValue == 0 || skillChoicesResult[0] != skillIndexValue) &&
                            ((m_secondarySkills[skillIndexValue] != HERO_SKILL_LEVEL_NONE &&
                                 m_secondarySkills[skillIndexValue] <
                                     HERO_SKILL_LEVEL_EXPERT) ||
                                (m_secondarySkills[skillIndexValue] == HERO_SKILL_LEVEL_NONE &&
                                    m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY))) {
                            skillWeightIndex -=
                                iGetSSByAlignment[skillIndexValue][m_cursorType];
                            if (skillWeightIndex <= 0) {
                                skillChoicesResult[indexValue] = skillIndexValue;
                                break;
                            }
                        }
                        skillIndexValue = (skillIndexValue + 1) % HERO_SKILL_COUNT;
                    }
                    attempts--;
                }
            }

            if (skillChoicesResult[0] == HERO_SKILL_WISDOM ||
                skillChoicesResult[1] == HERO_SKILL_WISDOM) {
                m_enabled = static_cast<unsigned char>(currentLevelIndex);
            }

            if (!gbInNewGameSetup && m_owner >= 0 && gbThisNetHumanPlayer[m_owner]) {
                sampleValue = LoadPlaySample(const_cast<char *>("nwherolv.82m"));
                if (skillChoicesResult[0] == HERO_SECONDARY_SKILL_NONE) {
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1, -1, 0, -1, 0, -1, 0);
                } else if (skillChoicesResult[1] == HERO_SECONDARY_SKILL_NONE) {
                    sprintf(line, "\n\nYou have learned %s %s.",
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[0]]],
                        gSecondarySkills[skillChoicesResult[0]]);
                    strcat(gText, line);
                    NormalDialog(gText, NORMAL_DIALOG_INFO, -1, -1,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[0]] +
                            skillChoicesResult[0] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        -1, 0, -1, 0);
                    GiveSS(skillChoicesResult[0], 1);
                } else {
                    sprintf(line, "\n\nYou may learn either %s %s or %s %s.",
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[0]]],
                        gSecondarySkills[skillChoicesResult[0]],
                        gSecondarySkillLevels[m_secondarySkills[skillChoicesResult[1]]],
                        gSecondarySkills[skillChoicesResult[1]]);
                    strcat(gText, line);
                    NormalDialog(gText, NORMAL_DIALOG_DISABLE_SEVENTH, -1, -1,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[0]] +
                            skillChoicesResult[0] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        NORMAL_DIALOG_SECONDARY_SKILL,
                        m_secondarySkills[skillChoicesResult[1]] +
                            skillChoicesResult[1] * HERO_SECONDARY_SKILL_ICON_STRIDE,
                        -1, 0);
                    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_BUTTON_SEVEN)
                        GiveSS(skillChoicesResult[0], 1);
                    else
                        GiveSS(skillChoicesResult[1], 1);
                }
            } else {
                if (skillChoicesResult[0] != HERO_SECONDARY_SKILL_NONE) {
                    if (skillChoicesResult[1] != HERO_SECONDARY_SKILL_NONE) {
                        if (gSSValues[skillChoicesResult[1]][0] <
                            gSSValues[skillChoicesResult[0]][0]) {
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
        m_level = static_cast<short>(newLevel);
        WaitEndSample(sampleValue, -1);
    }
}

VA(0x0046e067, 0x57)
int hero::NumArtifacts(void) {
    int cnt = 0;
    int i;

    for (i = 0; i < HERO_ARTIFACT_SLOT_COUNT; i++) {
        if (m_artifacts[i] >= 0)
            cnt++;
    }
    return cnt;
}

// @match-note 98.86%: semantics, the 0x10 frame, army/secondary-skill slots at
// -0x04/-0x08, message at -0x0c, switch CFG, and all 150 relocation positions
// agree. The relocation helper's two only-base reports are signed array-addend
// identities: gStatNames-0x144 and gSecondarySkillLevels-4 resolve to retail's
// delinked interior labels. The first non-relocation residual is in the third
// secondary-skill range near +0x59e: retail uses one jge plus tail trampolines,
// while the value-equivalent invalid-range spelling uses jl and continuation
// jumps. Direct range bounds and both positive/negative third-range forms select
// the same residual range-check lowering. A post-95 30-walk AST pass retained
// no mutation, and an isolated eight-trial TU-state sweep found no audited
// exact closure. Revisit only after a material TU-state change.
VA(0x0046e0be, 0x758)
void UpdateHeroScreenStatusBar(struct tag_message &message) {
    int armySlot;
    int secondarySkillSlot;

    switch (message.payload.widget.id) {
    case HERO_UI_PRIMARY_STAT_FIRST:
    case HERO_UI_PRIMARY_STAT_FIRST + 1:
    case HERO_UI_PRIMARY_STAT_FIRST + 2:
    case HERO_UI_PRIMARY_STAT_LAST:
        sprintf(gText, cHeroScreen[HERO_TEXT_PRIMARY_STAT],
            gStatNames[message.payload.widget.id - HERO_UI_PRIMARY_STAT_FIRST]);
        break;

    case HERO_UI_ADDITIONAL_STATS:
        sprintf(gText, cHeroScreen[HERO_TEXT_ADDITIONAL_STATS]);
        break;

    case HERO_UI_MORALE_FIRST:
    case HERO_UI_MORALE_FIRST + 1:
    case HERO_UI_MORALE_LAST:
        if (gpHVHero->m_army.GetMorale(gpHVHero, gpHVHero->GetOccupiedTown(), 0) > 0)
            sprintf(gText, cHeroScreen[HERO_TEXT_GOOD_MORALE]);
        else if (gpHVHero->m_army.GetMorale(
                     gpHVHero, gpHVHero->GetOccupiedTown(), 0) == 0)
            sprintf(gText, cHeroScreen[HERO_TEXT_NEUTRAL_MORALE]);
        else
            sprintf(gText, cHeroScreen[HERO_TEXT_BAD_MORALE]);
        break;

    case HERO_UI_LUCK_FIRST:
    case HERO_UI_LUCK_FIRST + 1:
    case HERO_UI_LUCK_LAST:
        if (gpGame->GetLuck(gpHVHero, 0, gpHVHero->GetOccupiedTown()) > 0)
            sprintf(gText, cHeroScreen[HERO_TEXT_GOOD_LUCK]);
        else if (gpGame->GetLuck(gpHVHero, 0, gpHVHero->GetOccupiedTown()) == 0)
            sprintf(gText, cHeroScreen[HERO_TEXT_NEUTRAL_LUCK]);
        else
            sprintf(gText, cHeroScreen[HERO_TEXT_BAD_LUCK]);
        break;

    case HERO_UI_EXPERIENCE_FIRST:
    case HERO_UI_EXPERIENCE_LAST:
        sprintf(gText, cHeroScreen[HERO_TEXT_EXPERIENCE]);
        break;

    case HERO_UI_SPELL_POINTS_FIRST:
    case HERO_UI_SPELL_POINTS_LAST:
        sprintf(gText, cHeroScreen[HERO_TEXT_SPELL_POINTS]);
        break;

    case HERO_UI_FORMATION_SPREAD:
        sprintf(gText, cHeroScreen[HERO_TEXT_SPREAD_FORMATION]);
        break;

    case HERO_UI_FORMATION_GROUPED:
        sprintf(gText, cHeroScreen[HERO_TEXT_GROUPED_FORMATION]);
        break;

    case HERO_UI_ARMY_SELECTOR_FIRST:
    case HERO_UI_ARMY_SELECTOR_FIRST + 1:
    case HERO_UI_ARMY_SELECTOR_FIRST + 2:
    case HERO_UI_ARMY_SELECTOR_FIRST + 3:
    case HERO_UI_ARMY_SELECTOR_LAST:
        armySlot = message.payload.widget.id - HERO_UI_ARMY_SELECTOR_FIRST;
        if (giHeroScreenSrcIndex == HERO_UI_ARMY_SELECTION_NONE) {
            if (gpHVHero->m_army.m_creatureTypes[armySlot] != ARMY_GROUP_EMPTY_SLOT)
                sprintf(gText, cHeroScreen[HERO_TEXT_SELECT_ARMY],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]);
            else
                strcpy(gText, cHeroScreen[HERO_TEXT_EMPTY]);
        } else if (giHeroScreenSrcIndex == armySlot) {
            sprintf(gText, cHeroScreen[HERO_TEXT_SELECT_ARMY],
                gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]);
        } else if (gpTownManager->m_castleDialogActive != 0) {
            if (gpHVHero->m_army.m_creatureTypes[armySlot] != ARMY_GROUP_EMPTY_SLOT)
                sprintf(gText, cHeroScreen[HERO_TEXT_SELECT_ARMY],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]);
            else
                strcpy(gText, cHeroScreen[HERO_TEXT_EMPTY]);
        } else if (gpHVHero->m_army.m_creatureTypes[armySlot] == ARMY_GROUP_EMPTY_SLOT) {
            if (message.payload.widget.parameter & HERO_UI_SPLIT_MODIFIER_MASK)
                sprintf(gText, cHeroScreen[HERO_TEXT_SPLIT_ARMY],
                    gArmyNamesPlural[
                        gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]]);
            else
                sprintf(gText, cHeroScreen[HERO_TEXT_MOVE_ARMY],
                    gArmyNames[gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]]);
        } else if (gpHVHero->m_army.m_creatureTypes[armySlot] ==
                   gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]) {
            sprintf(gText, cHeroScreen[HERO_TEXT_COMBINE_ARMIES],
                gArmyNamesPlural[gpHVHero->m_army.m_creatureTypes[armySlot]]);
        } else {
            sprintf(gText, cHeroScreen[HERO_TEXT_EXCHANGE_ARMIES],
                gArmyNames[gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex]],
                gArmyNames[gpHVHero->m_army.m_creatureTypes[armySlot]]);
        }
        break;

    case HERO_UI_ARTIFACT_FIRST:
    case HERO_UI_ARTIFACT_FIRST + 1:
    case HERO_UI_ARTIFACT_FIRST + 2:
    case HERO_UI_ARTIFACT_FIRST + 3:
    case HERO_UI_ARTIFACT_FIRST + 4:
    case HERO_UI_ARTIFACT_FIRST + 5:
    case HERO_UI_ARTIFACT_FIRST + 6:
    case HERO_UI_ARTIFACT_FIRST + 7:
    case HERO_UI_ARTIFACT_FIRST + 8:
    case HERO_UI_ARTIFACT_FIRST + 9:
    case HERO_UI_ARTIFACT_FIRST + 10:
    case HERO_UI_ARTIFACT_FIRST + 11:
    case HERO_UI_ARTIFACT_FIRST + 12:
    case HERO_UI_ARTIFACT_LAST:
        if (gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST] ==
            HERO_ARTIFACT_NONE)
            sprintf(gText, cHeroScreen[HERO_TEXT_EMPTY]);
        else if (gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST] ==
                 HERO_ARTIFACT_MAGIC_BOOK)
            strcpy(gText, cHeroScreen[HERO_TEXT_VIEW_SPELLS]);
        else
            sprintf(gText, cHeroScreen[HERO_TEXT_ARTIFACT],
                gArtifactNames[
                    gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST]]);
        break;

    case HERO_UI_DISMISS:
        sprintf(gText, cHeroScreen[HERO_TEXT_DISMISS], gpHVHero->m_name,
            gAlignmentNames[gpHVHero->m_cursorType]);
        break;

    case HERO_UI_CLOSE:
        strcpy(gText, cHeroScreen[HERO_TEXT_EXIT]);
        break;

    default:
        if (message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW1_FIRST &&
            message.payload.widget.id < HERO_UI_SECONDARY_SKILL_ROW2_FIRST) {
            secondarySkillSlot = message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW1_FIRST;
            goto secondary_skill_text;
        }
        if (message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW2_FIRST &&
            message.payload.widget.id < HERO_UI_SECONDARY_SKILL_ROW3_FIRST) {
            secondarySkillSlot = message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW2_FIRST;
            goto secondary_skill_text;
        }
        if (message.payload.widget.id < HERO_UI_SECONDARY_SKILL_ROW3_FIRST ||
            message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW3_LAST + 1)
            goto default_hero_text;
        secondarySkillSlot = message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW3_FIRST;

secondary_skill_text:
        if (secondarySkillSlot < gpHVHero->m_secondarySkillCount) {
            sprintf(gText, cHeroScreen[HERO_TEXT_SECONDARY_SKILL],
                gSecondarySkillLevels[
                    gpHVHero->m_secondarySkills[gpHVHero->GetNthSS(secondarySkillSlot)] - 1],
                gSecondarySkills[gpHVHero->GetNthSS(secondarySkillSlot)]);
            break;
        }
default_hero_text:
        strcpy(gText, cHeroScreen[HERO_TEXT_SCREEN]);
        break;
    }
    HeroMessageUpdate(gText);
}

// @match-note 94.47%: complete message, hero-cycle, stat, army, artifact, skill,
// formation, and exit behavior. Retail and base both use a 0x60 frame: message is
// at -0x4c, exit/quick-view at -0x10/-0x18, army temporaries at -0x08/-0x0c,
// secondary skill at -0x14, hero position at -0x1c, experience temporaries at
// -0x20/-0x24, and canDismiss at -0x48. The unused tag_message and scalar recover
// retail's -0x28..-0x44 handler scratch span. All 23 callee identities/counts and
// the main jump table agree; the relocation multiset differs only by retail's two
// extra gpHVHero loads in the formation assignments. Retail call order places the
// ViewArmy body before transfer/split, recovered with the positive view condition.
// The direct direction ternary recovers retail's `sbb; and -2; inc` hero-cycle
// arithmetic and raised the retained score from 94.00%. Residuals are hover
// operand load order, two local continuations, and the two load/modify/store
// formation updates; reversing the hover equality was byte-neutral. An isolated
// eight-trial TU-state sweep found no audited exact closure.
VA(0x0046e816, 0xaef)
int HeroHandler(struct tag_message &message) {
    int handlerValue16;
    int temp1;
    int armySlot7;
    int exitHero36 = 0;
    int secondarySkillSlot18;
    int quickView0;
    int heroPosition5;
    int nextExperience12;
    int level14;

    if (message.payload.widget.parameter & HERO_UI_QUICK_VIEW_MODIFIER)
        quickView0 = 1;
    else
        quickView0 = 0;

    if (message.type == HERO_UI_HOVER) {
        gpWindowManager->ConvertToHover(message);
        if (gpWindowManager->m_lastHoverId == message.payload.hover.id)
            return HERO_UI_HANDLER_CONTINUE;
        gpWindowManager->m_lastHoverId = message.payload.hover.id;
        UpdateHeroScreenStatusBar(message);
        return HERO_UI_HANDLER_CONTINUE;
    }

    if (message.type == HERO_UI_KEY_UP) {
        switch (message.payload.keyboard.keyCode) {
        case HERO_UI_SHIFT_LEFT:
        case HERO_UI_SHIFT_RIGHT:
            gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
            gpInputManager->ForceMouseMove();
            break;
        }
    }

    if (message.type == HERO_UI_KEY_DOWN) {
        switch (message.payload.keyboard.keyCode) {
        case HERO_UI_SHIFT_LEFT:
        case HERO_UI_SHIFT_RIGHT:
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
                case HERO_UI_DISMISS:
                    if (gpHVHero->Dismiss())
                        exitHero36 = 1;
                    break;
                case HERO_UI_CLOSE:
                    exitHero36 = 1;
                    break;
                case HERO_UI_PREVIOUS_HERO:
                case HERO_UI_NEXT_HERO: {
                    if (gpHVHero->m_owner != giCurPlayer) {
                    } else {
                        if (gpCurPlayer->m_heroCount <=
                            HERO_UI_HERO_CYCLE_MIN_COUNT - 1) {
                        } else {
                            heroPosition5 =
                                gpGame->HeroIDToHeroPos(gpCurPlayer, gpHVHero->m_id);
                            heroPosition5 =
                                ((static_cast<unsigned int>(message.payload.widget.id -
                                      HERO_UI_PREVIOUS_HERO) >= 1 ? 1 : -1) +
                                    gpCurPlayer->m_heroCount + heroPosition5) %
                                gpCurPlayer->m_heroCount;
                            gpHVHero = &gpGame->m_heroRecs[
                                gpCurPlayer->m_heroIds[heroPosition5]];
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
            case HERO_UI_PRIMARY_STAT_FIRST:
            case HERO_UI_PRIMARY_STAT_FIRST + 1:
            case HERO_UI_PRIMARY_STAT_FIRST + 2:
            case HERO_UI_PRIMARY_STAT_LAST:
                gpHVHero->ViewStat(
                    message.payload.widget.id - HERO_UI_PRIMARY_STAT_FIRST, quickView0);
                break;

            case HERO_UI_MORALE_FIRST:
            case HERO_UI_MORALE_FIRST + 1:
            case HERO_UI_MORALE_LAST:
                gpGame->ShowMoraleInfo(gpHVHero,
                    quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW);
                break;

            case HERO_UI_LUCK_FIRST:
            case HERO_UI_LUCK_FIRST + 1:
            case HERO_UI_LUCK_LAST:
                gpGame->ShowLuckInfo(gpHVHero,
                    quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW);
                break;

            case HERO_UI_FORMATION_SPREAD:
                if (quickView0) {
                    NormalDialog(
                        "{Spread Formation}\n\n'Spread' combat formation spreads your armies from the top to the bottom of the battlefield, with at least one empty space between each army.",
                        NORMAL_DIALOG_QUICK_VIEW, NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                        0, NORMAL_DIALOG_NO_RESOURCE, 0,
                        NORMAL_DIALOG_NO_RESOURCE, 0);
                } else {
                    gpHVHero->m_eventFlags &= ~HERO_EVENT_GROUPED_FORMATION;
                    SetupHeroView();
                    RedrawHeroScreen();
                }
                break;

            case HERO_UI_FORMATION_GROUPED:
                if (quickView0) {
                    NormalDialog(
                        "{Grouped Formation}\n\n'Grouped' combat formation bunches your army together in the center of your side of the battlefield.",
                        NORMAL_DIALOG_QUICK_VIEW, NORMAL_DIALOG_NO_RESOURCE,
                        NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_NO_RESOURCE,
                        0, NORMAL_DIALOG_NO_RESOURCE, 0,
                        NORMAL_DIALOG_NO_RESOURCE, 0);
                } else {
                    gpHVHero->m_eventFlags |= HERO_EVENT_GROUPED_FORMATION;
                    SetupHeroView();
                    RedrawHeroScreen();
                }
                break;

            case HERO_UI_SPELL_POINTS_FIRST:
            case HERO_UI_SPELL_POINTS_LAST:
                sprintf(gText,
                    "{Spell Points}\n%s currently has %d spell points out of a maximum of %d.  The maximum number of spell points is 10 times your knowledge.  It is occasionally possible to have more than your maximum spell points via special events.",
                    gpHVHero->m_name, gpHVHero->m_spellPoints,
                    gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE) *
                        HERO_SPELL_POINTS_PER_KNOWLEDGE);
                NormalDialog(gText,
                    quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                    NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE, 0,
                    NORMAL_DIALOG_NO_RESOURCE, 0,
                    NORMAL_DIALOG_NO_RESOURCE, 0);
                break;

            case HERO_UI_EXPERIENCE_FIRST:
            case HERO_UI_EXPERIENCE_LAST: {
                level14 = gpHVHero->GetLevel(gpHVHero->m_experience);
                nextExperience12 = gpHVHero->GetExperience(level14 + 1);
                sprintf(gText, "{Level %d}\nCurrent experience %d\nNext level %d",
                    level14, gpHVHero->m_experience, nextExperience12);
                NormalDialog(gText,
                    quickView0 == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
                    NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE,
                    NORMAL_DIALOG_NO_RESOURCE, 0,
                    NORMAL_DIALOG_NO_RESOURCE, 0,
                    NORMAL_DIALOG_NO_RESOURCE, 0);
                break;
            }

            case HERO_UI_ARMY_SELECTOR_FIRST:
            case HERO_UI_ARMY_SELECTOR_FIRST + 1:
            case HERO_UI_ARMY_SELECTOR_FIRST + 2:
            case HERO_UI_ARMY_SELECTOR_FIRST + 3:
            case HERO_UI_ARMY_SELECTOR_LAST: {
                tag_message dialogMessage;
                int armyValue;

                armySlot7 = message.payload.widget.id - HERO_UI_ARMY_SELECTOR_FIRST;
                if (quickView0 == 0 && giHeroScreenSrcIndex == HERO_UI_ARMY_SELECTION_NONE) {
                    if (gpHVHero->m_army.m_creatureTypes[armySlot7] != ARMY_GROUP_EMPTY_SLOT) {
                        giHeroScreenSrcIndex = armySlot7;
                        gpHVHero->HeroScreenUpdate();
                    }
                } else if ((quickView0 != 0 &&
                               gpHVHero->m_army.m_creatureTypes[armySlot7] !=
                                   ARMY_GROUP_EMPTY_SLOT) ||
                           (quickView0 == 0 && armySlot7 == giHeroScreenSrcIndex)) {
                    int canDismiss;

                    if (quickView0 == 0 && gpTownManager->m_castleDialogActive !=
                            HERO_UI_CASTLE_DIALOG_ACTIVE &&
                        gpHVHero->m_army.GetNumArmies() != 1)
                        canDismiss = 0;
                    else
                        canDismiss = 1;
                    gpGame->ViewArmy(HERO_UI_VIEW_ARMY_X, HERO_UI_VIEW_ARMY_Y,
                        gpHVHero->m_army.m_creatureTypes[armySlot7],
                        gpHVHero->m_army.m_creatureCounts[armySlot7], 0, canDismiss, 1,
                        quickView0, gpHVHero, 0, &gpHVHero->m_army, armySlot7);
                    if (quickView0 == 0)
                        giHeroScreenSrcIndex = HERO_UI_ARMY_SELECTION_NONE;
                    SetupHeroView();
                    RedrawHeroScreen();
                } else {
                    if (quickView0 == 0 && gpTownManager->m_castleDialogActive != 0) {
                        if (gpHVHero->m_army.m_creatureTypes[armySlot7] !=
                            ARMY_GROUP_EMPTY_SLOT) {
                            giHeroScreenSrcIndex = armySlot7;
                            gpHVHero->HeroScreenUpdate();
                        }
                    } else if (quickView0 == 0) {
                        temp1 = gpHVHero->m_army.m_creatureTypes[armySlot7];
                        if ((message.payload.widget.parameter & HERO_UI_SPLIT_MODIFIER_MASK) == 0 ||
                            (gpHVHero->m_army.m_creatureTypes[armySlot7] !=
                                 ARMY_GROUP_EMPTY_SLOT &&
                             gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] !=
                                 gpHVHero->m_army.m_creatureTypes[armySlot7])) {
                            if (gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] ==
                                gpHVHero->m_army.m_creatureTypes[armySlot7]) {
                                gpHVHero->m_army.m_creatureCounts[armySlot7] +=
                                    gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] = 0;
                                gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                    ARMY_GROUP_EMPTY_SLOT;
                            } else {
                                gpHVHero->m_army.m_creatureTypes[armySlot7] =
                                    gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex];
                                gpHVHero->m_army.m_creatureTypes[giHeroScreenSrcIndex] =
                                    static_cast<signed char>(temp1);
                                temp1 = gpHVHero->m_army.m_creatureCounts[armySlot7];
                                gpHVHero->m_army.m_creatureCounts[armySlot7] =
                                    gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex];
                                gpHVHero->m_army.m_creatureCounts[giHeroScreenSrcIndex] =
                                    static_cast<short>(temp1);
                            }
                        } else {
                            DoHeroSplit(armySlot7, giHeroScreenSrcIndex);
                        }
                        giHeroScreenSrcIndex = HERO_UI_ARMY_SELECTION_NONE;
                        gpHVHero->HeroScreenUpdate();
                    }
                }
                if (quickView0 == 0) {
                    gpWindowManager->m_lastHoverId = HERO_WINDOW_NO_HOVER_WIDGET;
                    UpdateHeroScreenStatusBar(message);
                }
                break;
            }

            case HERO_UI_ARTIFACT_FIRST:
            case HERO_UI_ARTIFACT_FIRST + 1:
            case HERO_UI_ARTIFACT_FIRST + 2:
            case HERO_UI_ARTIFACT_FIRST + 3:
            case HERO_UI_ARTIFACT_FIRST + 4:
            case HERO_UI_ARTIFACT_FIRST + 5:
            case HERO_UI_ARTIFACT_FIRST + 6:
            case HERO_UI_ARTIFACT_FIRST + 7:
            case HERO_UI_ARTIFACT_FIRST + 8:
            case HERO_UI_ARTIFACT_FIRST + 9:
            case HERO_UI_ARTIFACT_FIRST + 10:
            case HERO_UI_ARTIFACT_FIRST + 11:
            case HERO_UI_ARTIFACT_FIRST + 12:
            case HERO_UI_ARTIFACT_LAST:
                if (gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST] !=
                    HERO_ARTIFACT_NONE) {
                    if (quickView0 == 0 &&
                        gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST] ==
                            HERO_ARTIFACT_MAGIC_BOOK) {
                        gpGame->ViewSpells(gpHVHero, HERO_UI_VIEW_SPELLS_ALL,
                            ViewSpecialHandler, HERO_UI_VIEW_SPELLS_SPECIAL);
                    } else {
                        gpHVHero->ViewArtifact(
                            gpHVHero->m_artifacts[message.payload.widget.id - HERO_UI_ARTIFACT_FIRST],
                            quickView0,
                            gpHVHero->m_artifactExtra[
                                message.payload.widget.id - HERO_UI_ARTIFACT_FIRST]);
                    }
                }
                break;

            default: {
                if (message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW1_FIRST &&
                    message.payload.widget.id <= HERO_UI_SECONDARY_SKILL_ROW1_LAST)
                    secondarySkillSlot18 =
                        message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW1_FIRST;
                else if (message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW2_FIRST &&
                         message.payload.widget.id <= HERO_UI_SECONDARY_SKILL_ROW2_LAST)
                    secondarySkillSlot18 =
                        message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW2_FIRST;
                else if (message.payload.widget.id >= HERO_UI_SECONDARY_SKILL_ROW3_FIRST &&
                         message.payload.widget.id <= HERO_UI_SECONDARY_SKILL_ROW3_LAST)
                    secondarySkillSlot18 =
                        message.payload.widget.id - HERO_UI_SECONDARY_SKILL_ROW3_FIRST;
                else
                    break;

                if (secondarySkillSlot18 < gpHVHero->m_secondarySkillCount)
                    gpHVHero->DoSSLevelDialog(
                        gpHVHero->GetNthSS(secondarySkillSlot18), quickView0);
                break;
            }
            }
            break;
        }
    }

    if (exitHero36) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = HERO_UI_DIALOG_CLOSE_COMMAND;
        message.payload.widget.command = message.payload.widget.id;
        return HERO_UI_HANDLER_CLOSE;
    }
    return HERO_UI_HANDLER_CONTINUE;
}

VA(0x0046f305, 0x4f)
void RedrawHeroScreen(void) {
    gpResourceManager->GetBackdrop("herobkg.icn", gpWindowManager->m_screen,
        HERO_UI_BACKDROP_PALETTE);
    heroWin->DrawWindow();
    gpWindowManager->UpdateScreenRegion(0, 0, HERO_UI_SCREEN_WIDTH,
        HERO_UI_SCREEN_HEIGHT);
}

// @early-stop
// All 0x218 code bytes and all 47 relocation sites match after
// masking the herowind.bin relocation. Objdiff gives this TU-local coalesced
// literal a generated $SG identity while retail retains its string symbol.
VA(0x0046f354, 0x218)
int HeroView(int heroId, int noDismiss, int fadeAlreadyOut) {
    mapCell *heroCell;

    gbNoDismiss = noDismiss;
    iOrigHeroViewID = heroId;
    gpAdvManager->TrimLoopingSounds(HERO_UI_LOOPING_SOUND_KEEP_COUNT);
    gpHVHero = gpGame->GetHero(heroId);
    gbHeroWindShowing = 1;
    if (fadeAlreadyOut == 0)
        gpWindowManager->FadeScreen(HERO_UI_FADE_OUT, HERO_UI_FADE_STEPS, 0);

    heroWin = new heroWindow(0, 0, "herowind.bin");
    if (heroWin == 0)
        MemError();
    SetWinText(heroWin, HERO_UI_WINDOW_TEXT_ID);
    gheroWin = heroWin;

    if (gpCurPlayer->m_currentHero == gpHVHero->m_id) {
        heroCell = gpAdvManager->GetCell(gpHVHero->m_x, gpHVHero->m_y);
        if (heroCell->m_triggerType != ADVMGR_HERO_TRIGGER) {
            gpHVHero->m_locationType = heroCell->m_triggerType;
            gpHVHero->m_occupiedTown = heroCell->m_objectMetadata;
        }
    }

    SetupHeroView();
    RedrawHeroScreen();
    gpWindowManager->FadeScreen(HERO_UI_FADE_IN, HERO_UI_FADE_STEPS, 0);
    gpWindowManager->DoDialog(heroWin, HeroHandler, 0);
    gpWindowManager->FadeScreen(HERO_UI_FADE_OUT, HERO_UI_FADE_STEPS, 0);
    delete heroWin;
    gheroWin = 0;

    if (gpWindowManager->m_dialogResult == HERO_UI_DIALOG_DISMISS) {
        return HERO_UI_VIEW_DISMISSED;
    } else {
        gpHVHero->m_mobility = gpHVHero->CalcMobility();
        gbHeroWindShowing = 0;
        gpHVHero = 0;
        return HERO_UI_VIEW_CLOSED;
    }
}

// @match-note retained 98.99%: complete title, hero-cycle, stat, dismissal, portrait,
// luck, morale, experience, formation, spell-point, crest, army, secondary-
// skill, artifact, and status-message behavior. The retail 0x58 frame is
// restored with distinct luck, morale, and skill-bonus locals. All 143
// relocation sites agree; the two gSecondarySkillLevels entries resolve to
// retail's interior const_000ff87c label. The built body is five bytes larger
// from one equivalent hero-cycle condition trampoline. The first normalized
// instruction difference is at +0x1b4 after comparing m_heroCount with one:
// ours emits je (0f 84), retail emits jne (0f 85), so the first differing raw
// byte is +0x1b5; opposite arm order selects the same enable/disable values.
// Later differences are dismissal and formation assignment load order and the
// repeated modifier-icon comparisons. The player-color inline accessor
// continuation is present, with its jump placed before rather than after the
// field load. Structural recovery and direct source-polarity steering are
// exhausted. A post-95 30-walk AST pass retained no mutation, and an isolated
// eight-trial TU-state sweep found no audited exact closure. The remaining
// differences are TU-cumulative compiler shape.
VA(0x0046f56c, 0x9c5)
void SetupHeroView(void) {
    int cannotDismiss;
    tag_message message;
    int index;
    int displayIndex;
    int magnitude;
    int luck;
    int morale;
    int secondarySkillBonus;
    int secondarySkill;
    tag_message statusMessage;

    cannotDismiss = gbNoDismiss;
    if (gpHVHero->m_locationType == HERO_LOCATION_TOWN)
        cannotDismiss = 1;

    message.type = HERO_UI_MESSAGE;
    sprintf(gText, "%s the %s", gpHVHero->m_name,
        gAlignmentNames[gpHVHero->m_cursorType]);
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = HERO_UI_HERO_TITLE;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    if (gpHVHero->m_owner != giCurPlayer ||
        gpCurPlayer->m_heroCount == HERO_UI_SINGLE_HERO_COUNT) {
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
        message.payload.widget.data.value = HERO_UI_CYCLE_BUTTON_DISABLED_FRAME;
        message.payload.widget.id = HERO_UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = HERO_UI_NEXT_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
        message.payload.widget.data.value = HERO_UI_CYCLE_BUTTON_FRAME;
        message.payload.widget.id = HERO_UI_PREVIOUS_HERO;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = HERO_UI_NEXT_HERO;
        heroWin->BroadcastMessage(message);
    }

    message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.data.value = HERO_UI_CONTROL_FRAME_DEFAULT;
    for (index = 0; index < HERO_UI_ARMY_SLOT_COUNT; index++) {
        message.payload.widget.id = HERO_UI_PRIMARY_STAT_FIRST + index;
        heroWin->BroadcastMessage(message);
        message.payload.widget.id = HERO_UI_ARMY_SELECTOR_FIRST + index;
        heroWin->BroadcastMessage(message);
    }

    if (cannotDismiss == 0 &&
        gpTownManager->m_castleDialogActive == 0 &&
        (gpCurPlayer->m_townCount != 0 ||
         gpCurPlayer->m_heroCount != HERO_UI_SINGLE_HERO_COUNT))
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.id = HERO_UI_DISMISS;
    message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
    heroWin->BroadcastMessage(message);

    sprintf(gText, "port%04d.icn", gpHVHero->m_portrait);
    message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    message.payload.widget.id = HERO_UI_HERO_PORTRAIT;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    for (index = 0; index < HERO_PRIMARY_STAT_COUNT; index++) {
        sprintf(gText, "%d", gpHVHero->Stats(index));
        message.payload.widget.id = HERO_UI_PRIMARY_STAT_VALUE_FIRST + index;
        message.payload.widget.data.text = gText;
        heroWin->BroadcastMessage(message);
    }

    luck = gpGame->GetLuck(
        gpHVHero, 0, gpHVHero->GetOccupiedTown());
    magnitude = abs(luck);
    if (magnitude <= HERO_UI_MIN_MODIFIER_ICONS)
        magnitude = HERO_UI_MIN_MODIFIER_ICONS;
    for (index = 0; index < HERO_UI_MODIFIER_ICON_COUNT; index++) {
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
        message.payload.widget.id = HERO_UI_LUCK_FIRST + displayIndex;
        message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(message);
    }
    for (index = 0; index < HERO_UI_MODIFIER_ICON_COUNT; index++) {
        message.payload.widget.command = HERO_UI_WIDGET_FRAME;
        message.payload.widget.id = HERO_UI_LUCK_FIRST + index;
        if (luck < 0)
            message.payload.widget.data.value = HERO_UI_LUCK_NEGATIVE_FRAME;
        else if (luck == 0)
            message.payload.widget.data.value = HERO_UI_LUCK_NEUTRAL_FRAME;
        else
            message.payload.widget.data.value = HERO_UI_LUCK_POSITIVE_FRAME;
        heroWin->BroadcastMessage(message);
    }

    morale = gpHVHero->m_army.GetMorale(
        gpHVHero, gpHVHero->GetOccupiedTown(), 0);
    magnitude = abs(morale);
    if (magnitude <= HERO_UI_MIN_MODIFIER_ICONS)
        magnitude = HERO_UI_MIN_MODIFIER_ICONS;
    for (index = 0; index < HERO_UI_MODIFIER_ICON_COUNT; index++) {
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
        message.payload.widget.id = HERO_UI_MORALE_FIRST + displayIndex;
        message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
        heroWin->BroadcastMessage(message);
    }
    for (index = 0; index < HERO_UI_MODIFIER_ICON_COUNT; index++) {
        message.payload.widget.command = HERO_UI_WIDGET_FRAME;
        message.payload.widget.id = HERO_UI_MORALE_FIRST + index;
        if (morale < 0)
            message.payload.widget.data.value = HERO_UI_MORALE_NEGATIVE_FRAME;
        else if (morale == 0)
            message.payload.widget.data.value = HERO_UI_MORALE_NEUTRAL_FRAME;
        else
            message.payload.widget.data.value = HERO_UI_MORALE_POSITIVE_FRAME;
        heroWin->BroadcastMessage(message);
    }

    sprintf(gText, "%d", gpHVHero->m_experience);
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = HERO_UI_EXPERIENCE_LAST;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    if (gpHVHero->m_eventFlags & HERO_EVENT_GROUPED_FORMATION)
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    message.payload.widget.id = HERO_UI_FORMATION_SPREAD_ICON;
    message.payload.widget.data.value = HERO_UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(message);
    if (gpHVHero->m_eventFlags & HERO_EVENT_GROUPED_FORMATION)
        message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
    else
        message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
    message.payload.widget.id = HERO_UI_FORMATION_GROUPED_ICON;
    message.payload.widget.data.value = HERO_UI_CONTROL_FRAME_DEFAULT;
    heroWin->BroadcastMessage(message);

    sprintf(gText, "%d/%d", gpHVHero->m_spellPoints,
        gpHVHero->Stats(HERO_PRIMARY_KNOWLEDGE) *
            HERO_SPELL_POINTS_PER_KNOWLEDGE);
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = HERO_UI_SPELL_POINTS_LAST;
    message.payload.widget.data.text = gText;
    heroWin->BroadcastMessage(message);

    sprintf(gText, "crest.icn");
    message.payload.widget.command = HERO_UI_WIDGET_ICON_FILE;
    message.payload.widget.id = HERO_UI_PLAYER_CREST;
    heroWin->BroadcastMessage(message);
    message.payload.widget.command = HERO_UI_WIDGET_FRAME;
    message.payload.widget.id = HERO_UI_PLAYER_CREST;
    message.payload.widget.data.value = gpCurPlayer->Color();
    heroWin->BroadcastMessage(message);

    gpHVHero->UpdateArmies();
    for (index = 0; index < HERO_SECONDARY_SKILL_CAPACITY; index++) {
        if (index < gpHVHero->m_secondarySkillCount) {
            secondarySkill = gpHVHero->GetNthSS(index);
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW1_FIRST + index;
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = secondarySkill + 1;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW3_FIRST + index;
            message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.text = gSecondarySkills[secondarySkill];
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_TEXT;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW3_FIRST + index;
            secondarySkillBonus = gpHVHero->GetSSLevel(secondarySkill) -
                gpHVHero->m_secondarySkills[secondarySkill];
            if (secondarySkillBonus > 0) {
                sprintf(gText, "%s+%d",
                    gSecondarySkillLevels[
                        gpHVHero->m_secondarySkills[secondarySkill]],
                    secondarySkillBonus);
            } else {
                sprintf(gText, "%s",
                    gSecondarySkillLevels[
                        gpHVHero->m_secondarySkills[secondarySkill]]);
            }
            message.payload.widget.data.text = gText;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW1_FIRST + index;
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = HERO_UI_EMPTY_SKILL_FRAME;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW2_FIRST + index;
            message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.id =
                HERO_UI_SECONDARY_SKILL_ROW3_FIRST + index;
            message.payload.widget.data.value = HERO_UI_CONTROL_VALUE_DEFAULT;
            heroWin->BroadcastMessage(message);
        }
    }

    for (index = 0; index < HERO_ARTIFACT_SLOT_COUNT; index++) {
        message.payload.widget.id = HERO_UI_ARTIFACT_FIRST + index;
        if (gpHVHero->m_artifacts[index] != HERO_ARTIFACT_NONE) {
            message.payload.widget.command = HERO_UI_WIDGET_ENABLE;
            message.payload.widget.data.value = HERO_UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = gpHVHero->m_artifacts[index] + 1;
            heroWin->BroadcastMessage(message);
        } else {
            message.payload.widget.command = HERO_UI_WIDGET_FRAME;
            message.payload.widget.data.value = HERO_UI_EMPTY_ARTIFACT_FRAME;
            heroWin->BroadcastMessage(message);
            message.payload.widget.command = HERO_UI_WIDGET_DISABLE;
            message.payload.widget.data.value = HERO_UI_ARTIFACT_CONTROL_VALUE;
            heroWin->BroadcastMessage(message);
        }
    }

    statusMessage.type = HERO_UI_MESSAGE;
    statusMessage.payload.widget.id = HERO_UI_ARMY_SELECTION_NONE;
    UpdateHeroScreenStatusBar(statusMessage);
}

// @early-stop
// All 0x2b0 code bytes and all 50 relocation sites match after
// masking the three string relocations. Objdiff names this TU's coalesced
// split-window, prompt, and integer-format literals differently from retail.
VA(0x0046ff31, 0x2b0)
void DoHeroSplit(int destinationSlot, int sourceSlot) {
    short unusedTextControl = HERO_UI_SPLIT_TEXT;
    short unusedAmountControl = HERO_UI_SPLIT_AMOUNT;
    tag_message message;

    gpTownManager->m_heroWindow1 = new heroWindow(
        HERO_UI_SPLIT_WINDOW_X, HERO_UI_SPLIT_WINDOW_Y, "splitwin.bin");
    if (gpTownManager->m_heroWindow1 == 0)
        MemError();
    gpTownManager->m_splitAmount = 0;
    gpTownManager->m_splitMaximum =
        gpHVHero->m_army.m_creatureCounts[sourceSlot];

    message.type = HERO_UI_MESSAGE;
    sprintf(gText, "Move how many troops?");
    message.payload.widget.command = HERO_UI_WIDGET_TEXT;
    message.payload.widget.id = HERO_UI_SPLIT_TEXT;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    sprintf(gText, "%d", gpTownManager->m_splitAmount);
    message.payload.widget.id = HERO_UI_SPLIT_AMOUNT;
    message.payload.widget.data.text = gText;
    gpTownManager->m_heroWindow1->BroadcastMessage(message);
    gpWindowManager->DoDialog(gpTownManager->m_heroWindow1, SplitArmyHandler, 0);
    delete gpTownManager->m_heroWindow1;

    if (gpWindowManager->m_dialogResult == HERO_UI_DIALOG_SPLIT &&
        gpTownManager->m_splitAmount != 0) {
        if (gpHVHero->m_army.m_creatureTypes[sourceSlot] ==
            gpHVHero->m_army.m_creatureTypes[destinationSlot]) {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -=
                gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] +=
                gpTownManager->m_splitAmount;
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] =
                    ARMY_GROUP_EMPTY_SLOT;
        } else {
            gpHVHero->m_army.m_creatureCounts[sourceSlot] -=
                gpTownManager->m_splitAmount;
            gpHVHero->m_army.m_creatureCounts[destinationSlot] =
                static_cast<short>(gpTownManager->m_splitAmount);
            gpHVHero->m_army.m_creatureTypes[destinationSlot] =
                gpHVHero->m_army.m_creatureTypes[sourceSlot];
            if (gpHVHero->m_army.m_creatureCounts[sourceSlot] == 0)
                gpHVHero->m_army.m_creatureTypes[sourceSlot] =
                    ARMY_GROUP_EMPTY_SLOT;
        }
    }
}

VA(0x004701e1, 0x6a)
void hero::SetSS(int skill, int level) {
    if (level == HERO_SKILL_LEVEL_NONE)
        TakeSS(skill, HERO_SKILL_LEVEL_EXPERT);
    else if (m_secondarySkills[skill] != HERO_SKILL_LEVEL_NONE)
        m_secondarySkills[skill] = static_cast<signed char>(level);
    else
        GiveSS(skill, level);
}

VA(0x0047024b, 0xfa)
int hero::TakeSS(int skill, int levels) {
    int oldLevel;
    int otherSkill;

    oldLevel = m_secondarySkills[skill];
    if (m_secondarySkills[skill] != HERO_SKILL_LEVEL_NONE) {
        m_secondarySkills[skill] -= levels;
        if (m_secondarySkills[skill] < HERO_SKILL_LEVEL_NONE)
            m_secondarySkills[skill] = HERO_SKILL_LEVEL_NONE;
        if (m_secondarySkills[skill] == HERO_SKILL_LEVEL_NONE) {
            for (otherSkill = 0; otherSkill < HERO_SKILL_COUNT; otherSkill++) {
                if (m_secondarySkillOrder[otherSkill] >
                    m_secondarySkillOrder[skill]) {
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
int hero::GiveSS(int skill, int levels) {
    int oldLevel;

    oldLevel = m_secondarySkills[skill];
    if (m_secondarySkills[skill] != HERO_SKILL_LEVEL_NONE) {
        m_secondarySkills[skill] += levels;
    } else {
        if (m_secondarySkillCount < HERO_SECONDARY_SKILL_CAPACITY) {
            m_secondarySkills[skill] = static_cast<signed char>(levels);
            m_secondarySkillCount++;
            m_secondarySkillOrder[skill] =
                static_cast<unsigned char>(m_secondarySkillCount);
        }
    }
    if (m_secondarySkills[skill] > HERO_SKILL_LEVEL_EXPERT)
        m_secondarySkills[skill] = HERO_SKILL_LEVEL_EXPERT;
    return m_secondarySkills[skill] - oldLevel;
}

VA(0x00470404, 0x6a)
int hero::CreatureTypeCount(int creatureType) {
    int creatureCount;
    int armySlot;

    creatureCount = 0;
    for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT; armySlot++) {
        if (m_army.m_creatureTypes[armySlot] == creatureType &&
            m_army.m_creatureCounts[armySlot] > 0) {
            creatureCount++;
        }
    }
    return creatureCount;
}

VA(0x0047046e, 0x5e)
void hero::UpgradeCreatures(int oldCreatureType, int newCreatureType) {
    int numberUpgraded = 0;
    int armySlot;

    for (armySlot = 0; armySlot < ARMY_GROUP_SLOT_COUNT; armySlot++) {
        if (m_army.m_creatureTypes[armySlot] == oldCreatureType)
            m_army.m_creatureTypes[armySlot] =
                static_cast<signed char>(newCreatureType);
    }
}

VA(0x004704cc, 0x5e)
int hero::GetNthSS(int ordinal) {
    int skill;

    for (skill = 0; skill < HERO_SKILL_COUNT; skill++) {
        if (m_secondarySkillOrder[skill] ==
            ordinal + HERO_SECONDARY_SKILL_ORDER_BASE)
            return skill;
    }
    return HERO_SECONDARY_SKILL_NONE;
}

VA(0x0047052a, 0x51)
class town * hero::GetOccupiedTown(void) {
    if (m_locationType == HERO_LOCATION_TOWN)
        return gpGame->GetTown(m_occupiedTown);
    return 0;
}

VA(0x0047057b, 0x47)
signed char hero::Stats(int stat) {
    if (stat == HERO_PRIMARY_SPELL_POWER &&
        m_primaryStats[stat] < HERO_MINIMUM_SPELL_POWER) {
        return HERO_MINIMUM_SPELL_POWER;
    }
    return m_primaryStats[stat];
}

// @match-note 99.97%: semantics, the 0x0c frame, CFG, and all 3/3 relocation
// targets agree. The first differing instruction is +0x96 (first differing byte
// +0x99): retail loads shrineAndArtifactBonus from -0x08 before level from -0x04;
// base loads level first. Both += and reversed binary operand spelling compile
// to the base sequence; the addition is value-equivalent.
VA(0x004705c2, 0xc3)
signed char hero::GetSSLevel(int skill) {
    signed char shrineAndArtifactBonus = 0;
    signed char level;

    level = m_secondarySkills[skill];
    if (skill != HERO_SKILL_NECROMANCY)
        return level;
    if (level == HERO_SKILL_LEVEL_NONE)
        return level;
    if (HasArtifact(HERO_ARTIFACT_SPADE_NECROMANCY))
        shrineAndArtifactBonus++;
    if (m_cursorType == HERO_CLASS_NECROMANCER)
        shrineAndArtifactBonus += gpGame->CountShrines(m_owner);
    if (shrineAndArtifactBonus > HERO_NECROMANCY_BONUS_MAX)
        shrineAndArtifactBonus = HERO_NECROMANCY_BONUS_MAX;
    level += shrineAndArtifactBonus;
    if (level > HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX)
        level = HERO_NECROMANCY_EFFECTIVE_LEVEL_MAX;
    return level;
}

// @early-stop
// All 0xf4 code bytes and all 11 relocation sites match after masking the three
// delinked identities. Retail names the long format literal directly and uses
// interior labels for gSecondarySkillLevels and cSecSkillDesc; typed source uses
// the owning symbols and their corresponding addends.
VA(0x00470685, 0xf4)
void hero::DoSSLevelDialog(int skill, int quickView) {
    int skillBonus;
    char *skillLevelName;

    skillBonus = GetSSLevel(skill) - m_secondarySkills[skill];
    if (skillBonus > 0) {
        skillLevelName = gSecondarySkillLevels[m_secondarySkills[skill]];
        sprintf(gText,
            "{%s Necromancy (+%d)}\n\n%s Necromancy (+%d) allows %d percent of the creatures killed in combat to be brought back from the dead as Skeletons.",
            skillLevelName, skillBonus, skillLevelName, skillBonus,
            GetSSLevel(skill) * HERO_NECROMANCY_PERCENT_PER_LEVEL);
    } else {
        sprintf(gText,
            cSecSkillDesc[skill][m_secondarySkills[skill] - 1]);
    }
    NormalDialog(gText,
        quickView == 0 ? NORMAL_DIALOG_INFO : NORMAL_DIALOG_QUICK_VIEW,
        NORMAL_DIALOG_NO_RESOURCE, NORMAL_DIALOG_NO_VALUE,
        NORMAL_DIALOG_SECONDARY_SKILL,
        m_secondarySkills[skill] +
            skill * HERO_SECONDARY_SKILL_ICON_STRIDE -
            HERO_SECONDARY_SKILL_ICON_FRAME_BASE,
        NORMAL_DIALOG_NO_RESOURCE, 0,
        NORMAL_DIALOG_NO_RESOURCE, 0);
}

VA(0x00470779, 0x12f)
void hero::CheckAnduranPieces(int showDialog) {
    int artifactSlot;

    if (HasArtifact(HERO_ARTIFACT_BREASTPLATE_ANDURAN) &&
        HasArtifact(HERO_ARTIFACT_HELMET_ANDURAN) &&
        HasArtifact(HERO_ARTIFACT_SWORD_ANDURAN)) {
        for (artifactSlot = 0; artifactSlot < HERO_ARTIFACT_SLOT_COUNT;
             artifactSlot++) {
            if (m_artifacts[artifactSlot] ==
                    HERO_ARTIFACT_BREASTPLATE_ANDURAN ||
                m_artifacts[artifactSlot] == HERO_ARTIFACT_HELMET_ANDURAN ||
                m_artifacts[artifactSlot] == HERO_ARTIFACT_SWORD_ANDURAN) {
                GiveTakeArtifactStat(
                    this, m_artifacts[artifactSlot], EVENT_ARTIFACT_TAKE);
                m_artifacts[artifactSlot] = HERO_ARTIFACT_NONE;
            }
        }
        GiveArtifact(this, HERO_ARTIFACT_BATTLE_GARB, showDialog,
            HERO_ARTIFACT_NONE);
        if (gbThisNetHumanPlayer[m_owner]) {
            LoadPlaySample("treasure.82m");
            NormalDialog(
                "The three Anduran artifacts magically combine into one.",
                NORMAL_DIALOG_INFO, NORMAL_DIALOG_NO_RESOURCE,
                NORMAL_DIALOG_NO_VALUE, NORMAL_DIALOG_ARTIFACT,
                HERO_ARTIFACT_BATTLE_GARB, NORMAL_DIALOG_NO_RESOURCE, 0,
                NORMAL_DIALOG_NO_RESOURCE, 0);
        }
    }
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f6c88) class hero *gpHVHero = 0;
DATA(0x004f6c8c) class heroWindow *gheroWin = 0;
DATA(0x004f6cd0) short gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT] = {
    0, 1000, 2000, 3200, 4500, 6000, 7700, 9000, 11000, 13200, 15500, 18500
};
DATA(0x005280dc) int iOrigHeroViewID;
DATA(0x005280e0) int gbNoDismiss;
