// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HERO.OBJ   from: (directly linked into exe)
// functions: 41   data: 5
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <io.h>
#include <stdio.h>
#include <_types.h>
#include <_carcass_types.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/HERO.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/SPELLS.h>
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
    giHeroScreenSrcIndex = -1;
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
        if (gpGame->m_players[m_owner].unknown0f == 2)
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
// two gsSpellInfo relocations resolve to the same m_e bytes through retail's
// interior const_000fbe8d label. Revisit if preceding TU state changes code selection.
VA(0x0046c86c, 0xc5)
int hero::GetNthSpell(int type, int spellNumber) {
    int spell;
    int spellOrdinalCount = 0;

    for (spell = 0; spell < HERO_SPELL_COUNT; spell++) {
        if (HasSpell(spell)) {
            if (type == HERO_SPELL_TYPE_ALL ||
                (type == HERO_SPELL_TYPE_COMBAT &&
                 (gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_COMBAT)) ||
                (type == HERO_SPELL_TYPE_ADVENTURE &&
                 !(gsSpellInfo[spell].m_e & SPELL_ATTRIBUTE_COMBAT))) {
                spellOrdinalCount++;
            }
            if (spellOrdinalCount == spellNumber)
                return spell;
        }
    }
    return HERO_SPELL_NONE;
}

// @early-stop: all 0xd0 bytes match after masking the two aligned COFF relocations.
// Both relocation targets agree; retail delinks gsSpellInfo[0].m_e as the interior
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
            if (gsSpellInfo[spellIndexCurrent].m_e & SPELL_ATTRIBUTE_COMBAT)
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
    message.field4 = HERO_UI_WIDGET_TEXT;
    message.field8 = HERO_UI_STATUS_TEXT_WIDGET;
    message.text = text;
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
// the global-first form. Revisit after later HERO bodies change TU-cumulative state.
VA(0x0046cb33, 0xa8)
void hero::HeroScreenUpdate(void) {
    tag_message message;
    int index;

    message.type = HERO_UI_MESSAGE;
    UpdateArmies();
    for (index = 0; index < HERO_UI_ARMY_SLOT_COUNT; index++) {
        if (index == giHeroScreenSrcIndex)
            message.field4 = HERO_UI_WIDGET_ENABLE;
        else
            message.field4 = HERO_UI_WIDGET_DISABLE;
        message.field18 = HERO_UI_WIDGET_FRAME_ACTIVE;
        message.field8 = index + HERO_UI_ARMY_SELECTOR_FIRST;
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
            message.field4 = HERO_UI_WIDGET_FRAME;
            message.field8 = index + HERO_UI_ARMY_ICON_FIRST;
            message.field18 = HERO_UI_ARMY_EMPTY_FRAME;
            heroWin->BroadcastMessage(message);

            message.field4 = HERO_UI_WIDGET_DISABLE;
            message.field8 = index + HERO_UI_ARMY_PORTRAIT_FIRST;
            message.field18 = HERO_UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);
            message.field8 = index + HERO_UI_ARMY_COUNT_FIRST;
            heroWin->BroadcastMessage(message);
            message.field8 = index + HERO_UI_ARMY_SELECTOR_FIRST;
            heroWin->BroadcastMessage(message);
        } else {
            message.field4 = HERO_UI_WIDGET_FRAME;
            message.field8 = index + HERO_UI_ARMY_ICON_FIRST;
            message.field18 = gMonsterDatabase[m_army.m_creatureTypes[index]].race + 4;
            heroWin->BroadcastMessage(message);

            message.field4 = HERO_UI_WIDGET_ICON_FILE;
            sprintf(gText, "monh%04d.icn", m_army.m_creatureTypes[index]);
            message.field8 = index + HERO_UI_ARMY_PORTRAIT_FIRST;
            message.text = gText;
            heroWin->BroadcastMessage(message);

            message.field4 = HERO_UI_WIDGET_ENABLE;
            message.field18 = HERO_UI_WIDGET_FRAME_ACTIVE;
            heroWin->BroadcastMessage(message);

            sprintf(gText, "%d", m_army.m_creatureCounts[index]);
            message.field4 = HERO_UI_WIDGET_TEXT;
            message.field8 = index + HERO_UI_ARMY_COUNT_FIRST;
            message.text = gText;
            heroWin->BroadcastMessage(message);

            message.field4 = HERO_UI_WIDGET_ENABLE;
            message.field18 = HERO_UI_WIDGET_FRAME_ACTIVE;
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
// operations. The removed 32-bit/split bitfield aliases were also tried. Revisit at SOURCE 95%
// after shared HERO type/TU state settles for accessor-bracket and mask-operation steering.
VA(0x0046cee8, 0x587)
void hero::Deallocate(int updateMap) {
    int availableHeroSlotCurrent;
    playerRec *player;
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

    if (m_owner != giCurPlayer || gpGame->m_players[m_owner].currentHero != m_id ||
        gpAdvManager->m_heroContextLocked == 0) {
        gpGame->RestoreCell(m_x, m_y, m_locationType, m_occupiedTown, 0, 1);
    }

    if (!gbCombatSurrender) {
        for (index = 0; index < ARMY_GROUP_SLOT_COUNT; index++)
            m_army.Dismiss(index);
    }

    playerHeroIndex = -1;
    for (index = 0; index < player->heroCount; index++) {
        if (player->heroes[index] == m_id)
            playerHeroIndex = index;
    }
    for (index = playerHeroIndex; index < player->heroCount - 1; index++)
        player->heroes[index] = player->heroes[index + 1];
    player->heroes[player->heroCount - 1] = -1;

    if (player->currentHero == m_id) {
        player->currentHero = -1;
        if (m_owner == giCurPlayer) {
            gpAdvManager->m_cursorActive = 0;
            mapCellRecord = gpGame->m_worldMap.GetCell(m_x, m_y);
            mapCellRecord->field8 &= ~HERO_MAP_CELL_PRESENT;
        }
        if (giCurPlayer == heroOwner)
            gpAdvManager->m_heroContextLocked = 0;
    }

    player->heroCount--;
    player->heroWindowTop = 0;
    gpGame->m_availableHeroes[m_id] = HERO_AVAILABILITY_UNAVAILABLE;

    if (gbRetreatWin) {
        availableHeroSlotCurrent = Random(0, HERO_AVAILABLE_SLOT_COUNT - 1);
        if (gpGame->m_heroRecs[gpGame->m_players[m_owner]
                                   .availableHeroes[availableHeroSlotCurrent]]
                .m_eventFlags & HERO_EVENT_WEEKLY_VISIT) {
            availableHeroSlotCurrent = 1 - availableHeroSlotCurrent;
        }
        if (gpGame->m_availableHeroes[gpGame->m_players[m_owner]
                                          .availableHeroes[availableHeroSlotCurrent]] ==
            HERO_AVAILABILITY_RETREATED) {
            gpGame->m_availableHeroes[gpGame->m_players[m_owner]
                                          .availableHeroes[availableHeroSlotCurrent]] =
                HERO_AVAILABILITY_UNAVAILABLE;
        }
        gpGame->m_players[m_owner].availableHeroes[availableHeroSlotCurrent] = m_id;
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

// @match-note 69.40%: semantics, the 0x04 frame with this at -0x04, CFG, and the
// zero-relocation set agree. First divergence is +0x37: retail reads m_eventFlags,
// subtracts 0x20 in EAX, and writes it back (18 bytes after the common this load),
// while base emits a seven-byte memory SUB. The same 11-byte shortening repeats for
// all twelve cleared temporary flags; morale/luck updates, order, tests, and branches
// otherwise agree. Tried `-=`, explicit `field = field - flag`, addition of a negative
// flag, and a 32-bit bitfield declaration; all select memory SUB. Revisit at SOURCE 95%
// after shared HERO type/TU state settles, then source-steer the repeated assignment form.
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

VA(0x0046d83f, 0x828)
void hero::CheckLevel(void) {}

VA(0x0046e067, 0x57)
int hero::NumArtifacts(void) { return 0; }

VA(0x0046e0be, 0x758)
void UpdateHeroScreenStatusBar(struct tag_message &) {}

VA(0x0046e816, 0xaef)
int HeroHandler(struct tag_message &) { return 0; }

VA(0x0046f305, 0x4f)
void RedrawHeroScreen(void) {}

VA(0x0046f354, 0x218)
int HeroView(int, int, int) { return 0; }

VA(0x0046f56c, 0x9c5)
void SetupHeroView(void) {}

VA(0x0046ff31, 0x2b0)
void DoHeroSplit(int, int) {}

VA(0x004701e1, 0x6a)
void hero::SetSS(int, int) {}

VA(0x0047024b, 0xfa)
int hero::TakeSS(int, int) { return 0; }

VA(0x00470345, 0xbf)
int hero::GiveSS(int, int) { return 0; }

VA(0x00470404, 0x6a)
int hero::CreatureTypeCount(int) { return 0; }

VA(0x0047046e, 0x5e)
void hero::UpgradeCreatures(int, int) {}

VA(0x004704cc, 0x5e)
int hero::GetNthSS(int) { return 0; }

VA(0x0047052a, 0x51)
class town * hero::GetOccupiedTown(void) { return 0; }

VA(0x0047057b, 0x47)
signed char hero::Stats(int) { return 0; }

VA(0x004705c2, 0xc3)
signed char hero::GetSSLevel(int) { return 0; }

VA(0x00470685, 0xf4)
void hero::DoSSLevelDialog(int, int) {}

VA(0x00470779, 0x12f)
void hero::CheckAnduranPieces(int) {}

// ---- globals (definitions, RVA order) ----
DATA(0x004f6c88) class hero *gpHVHero;
DATA(0x004f6c8c) class heroWindow *gheroWin;
DATA(0x004f6cd0) short gMinExpForLevel[HERO_EXPERIENCE_LEVEL_TABLE_COUNT] = {
    0, 1000, 2000, 3200, 4500, 6000, 7700, 9000, 11000, 13200, 15500, 18500
};
DATA(0x005280dc) int iOrigHeroViewID;
DATA(0x005280e0) int gbNoDismiss;
