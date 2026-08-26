#ifndef HOMM2_IRONFIST_STATE_H
#define HOMM2_IRONFIST_STATE_H

#include <bitset>
#include <map>
#include <string>
#include <vector>

#include <Ints.h>
#include <IRONFIST/creatures.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB_TYPES.h>

class army;

namespace ironfist {

namespace state {

struct StackState {
    std::map<army*, std::map<CreatureAttribute, i32>> abilityCounter;
    std::map<army*, std::map<CreatureAttribute, bool>> abilityNowAnimating;
    std::map<army*, i32> forceShieldHP;
};

struct SpellState {
    struct FireBombWallHex {
        i32 hexIdx;
        i32 turnsLeft;
        i32 currentFrame;
    };

    std::vector<FireBombWallHex> fireBombWalls;
};

struct CombatState {
    StackState stack;
    SpellState spell;
};

struct CampaignState {
    struct PartialHeroData {
        i8 primarySkills[H2EnumIndex(HERO_PRIMARY_STAT_COUNT)];
        u8 skillIndex[H2EnumIndex(HERO_SKILL_COUNT)];
        i8 secondarySkillLevel[H2EnumIndex(HERO_SKILL_COUNT)];
        i32 numSecSkillsKnown;
        i8 artifacts[HERO_ARTIFACT_SLOT_COUNT];
        i8 scrollSpell[HERO_ARTIFACT_SLOT_COUNT];
        i8 spellsLearned[H2EnumIndex(KB_SPELL_TABLE_CAPACITY)];
        armyGroup army;
        i32 experience;
    };
    std::map<i32, PartialHeroData> savedHeroData;
};

// Ironfist appended this state to the game object; the port keeps the retail
// game layout intact and holds it here instead.
struct AdventureState {
    bool firstDayEventDone = false;
    bool mapEndCallbackFired = false;
    bool sharePlayerVision[H2EnumIndex(GAME_PLAYER_COUNT)][H2EnumIndex(GAME_PLAYER_COUNT)] = {};
    bool forcedComputerPlayerChases[H2EnumIndex(GAME_HERO_COUNT)][H2EnumIndex(GAME_HERO_COUNT)] =
        {};
    std::bitset<32> disallowedBuildings[H2EnumIndex(GAME_TOWN_COUNT)];
    bool allowAIArmySharing = true;
};

struct State {
    AdventureState adventure;
    CombatState combat;
    CampaignState campaign;
};

State& Get();

} // namespace state

} // namespace ironfist

#endif
