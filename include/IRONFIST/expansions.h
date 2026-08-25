#ifndef HOMM2_IRONFIST_EXPANSIONS_H
#define HOMM2_IRONFIST_EXPANSIONS_H

#include <bitset>
#include <map>
#include <string>
#include <vector>

#include <Ints.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB_TYPES.h>

class army;

class stackExtra {
public:
    std::map<army*, std::map<std::string, i32>> abilityCounter;
    std::map<army*, std::map<std::string, bool>> abilityNowAnimating;
    std::map<army*, i32> forceShieldHP;
};

class spellExtra {
    struct FireBombWallHex {
        i32 hexIdx;
        i32 turnsLeft;
        i32 currentFrame;
    };

public:
    std::vector<FireBombWallHex> fireBombWalls;
};

class combatExtra {
public:
    stackExtra stack;
    spellExtra spell;
};

class campaignExtra {
public:
    struct partialHeroData {
        char primarySkills[H2EnumIndex(HERO_PRIMARY_STAT_COUNT)];
        char skillIndex[H2EnumIndex(HERO_SKILL_COUNT)];
        char secondarySkillLevel[H2EnumIndex(HERO_SKILL_COUNT)];
        i32 numSecSkillsKnown;
        char artifacts[HERO_ARTIFACT_SLOT_COUNT];
        char scrollSpell[HERO_ARTIFACT_SLOT_COUNT];
        char spellsLearned[H2EnumIndex(KB_SPELL_TABLE_CAPACITY)];
        armyGroup army;
        i32 experience;
    };
    std::map<i32, partialHeroData> savedHeroData;
};

// Ironfist appended this state to the game object; the port keeps the retail
// game layout intact and holds it here instead.
class adventureExtra {
public:
    bool sharePlayerVision[H2EnumIndex(GAME_PLAYER_COUNT)][H2EnumIndex(GAME_PLAYER_COUNT)] = {};
    bool forcedComputerPlayerChases[H2EnumIndex(GAME_HERO_COUNT)][H2EnumIndex(GAME_HERO_COUNT)] =
        {};
    std::bitset<32> disallowedBuildings[H2EnumIndex(GAME_TOWN_COUNT)];
    bool allowAIArmySharing = true;
};

class ironfistExtra {
public:
    adventureExtra adventure;
    combatExtra combat;
    campaignExtra campaign;
};

extern ironfistExtra gIronfistExtra;

#endif
