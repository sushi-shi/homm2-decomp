// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HERO.OBJ   from: (directly linked into exe)
// functions: 41   data: 5
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <io.h>
#include <_types.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/advManager.h>
#include <SOURCE/game.h>
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
void hero::AddSpell(int, int) {}

VA(0x0046cab1, 0x82)
void HeroMessageUpdate(char *) {}

VA(0x0046cb33, 0xa8)
void hero::HeroScreenUpdate(void) {}

VA(0x0046cbdb, 0x1d2)
void hero::UpdateArmies(void) {}

VA(0x0046cdad, 0x43)
void hero::ViewStat(int, int) {}

VA(0x0046cdf0, 0x9b)
void hero::ViewArtifact(int, int, int) {}

VA(0x0046ce8b, 0x5d)
int hero::Dismiss(void) { return 0; }

VA(0x0046cee8, 0x587)
void hero::Deallocate(int) {}

VA(0x0046d46f, 0x9e)
int hero::GetExperience(int) { return 0; }

VA(0x0046d50d, 0xc0)
int hero::GetLevel(int) { return 0; }

VA(0x0046d5cd, 0x254)
void hero::ApplyBattleWinTemps(void) {}

VA(0x0046d821, 0x1e)
void hero::ApplyBattleLossTemps(void) {}

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
DATA(0x004f6cd0) short *gMinExpForLevel;
DATA(0x005280dc) int iOrigHeroViewID;
DATA(0x005280e0) int gbNoDismiss;
