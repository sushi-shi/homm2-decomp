#include <Ints.h>
#include <string.h>
#include <BASE/Misc.h>
#include <IRONFIST/hooks.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/town.h>


typedef enum MoraleConstant {
    FIZBIN_MORALE_PENALTY = 2,
    COLISEUM_MORALE_BONUS = 2,
    THREE_ALIGNMENT_COUNT = 3,
    FOUR_ALIGNMENT_COUNT  = 4,
    FIVE_ALIGNMENT_COUNT  = 5
} MoraleConstant;

template <typename Value>
void SwapValues(Value& lhs, Value& rhs) {
    Value temporary = lhs;
    lhs = rhs;
    rhs = temporary;
}

armyGroup::armyGroup(void) {
    memset(m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof(m_creatureTypes));
    memset(m_creatureCounts, 0, sizeof(m_creatureCounts));
}

void armyGroup::View(i32) {}

i32 armyGroup::HasAllUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != CREATURE_NONE
            && !(gMonsterDatabase[H2EnumIndex(m_creatureTypes[slot])].attributes
                 & MONSTER_ATTRIBUTE_UNDEAD))
            return 0;
    }
    return 1;
}

i32 armyGroup::HasSomeUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != CREATURE_NONE
            && (H2EnumIndex((gMonsterDatabase[H2EnumIndex(m_creatureTypes[slot])].attributes) & (MONSTER_ATTRIBUTE_UNDEAD))))
            return 1;
    }
    return 0;
}

i32 armyGroup::GetMorale(hero* armyHero, town* occupiedTown, armyGroup* enemyGroup) {
    i32 moraleCount = 0;
    ArmyGroupAlignmentResult alignValue;
    i32 hasSomeUndead = 0;
    i32 moraleDelta = 0;
    i32 enemyHasBoneDragon;
    i32 index;
    alignValue = IsHomogeneous(ARMY_GROUP_EMPTY_SLOT);

    if (HasAllUndead())
        return ironfist::hooks::ModifyMorale(armyHero, occupiedTown, 0);

    if (HasSomeUndead())
        hasSomeUndead = 1;

    enemyHasBoneDragon = 0;
    if (enemyGroup != NULL) {
        for (index = 0; index < ARMY_GROUP_SLOT_COUNT; ++index) {
            if (enemyGroup->m_creatureTypes[index] == CREATURE_BONE_DRAGON)
                enemyHasBoneDragon = 1;
        }
    }

    if (enemyHasBoneDragon)
        --moraleCount;

    if (armyHero != NULL) {
        if (armyHero->HasArtifact(ARTIFACT_BATTLE_GARB))
            return ironfist::hooks::ModifyMorale(armyHero, occupiedTown, ARMY_GROUP_MORALE_MAX);

        moraleCount += H2EnumIndex(armyHero->m_secondarySkills[H2EnumIndex(HERO_SKILL_LEADERSHIP)]);
        moraleCount += armyHero->m_morale;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_VALOR))
            ++moraleCount;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE))
            ++moraleCount;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_HONOR))
            ++moraleCount;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION))
            ++moraleCount;
        if (armyHero->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE))
            moraleCount -= FIZBIN_MORALE_PENALTY;
        if (armyHero->HasArtifact(ARTIFACT_ARM_OF_MARTYR))
            hasSomeUndead = 1;
        if (armyHero->HasArtifact(ARTIFACT_MASTHEAD)
            && (H2EnumIndex((armyHero->m_eventFlags) & (HERO_EVENT_EMBARKED))))
            ++moraleCount;
    }

    if (hasSomeUndead)
        --moraleCount;
    if (hasSomeUndead && alignValue > ARMY_GROUP_ALIGNMENT_NO_MODIFIER)
        alignValue = ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
    moraleCount += H2EnumIndex(alignValue);

    if (occupiedTown != NULL && occupiedTown->m_type != FACTION_NECROMANCER
        && (occupiedTown->m_buildings & H2EnumIndex(TOWN_BUILDING_TAVERN)))
        ++moraleCount;
    if (occupiedTown != NULL && occupiedTown->m_type == FACTION_BARBARIAN
        && (occupiedTown->m_buildings & H2EnumIndex(TOWN_BUILDING_COLISEUM)))
        moraleCount += COLISEUM_MORALE_BONUS;

    if (moraleCount < ARMY_GROUP_MORALE_MIN)
        moraleCount = ARMY_GROUP_MORALE_MIN;
    else if (moraleCount > ARMY_GROUP_MORALE_MAX)
        moraleCount = ARMY_GROUP_MORALE_MAX;

    return ironfist::hooks::ModifyMorale(armyHero, occupiedTown, moraleCount);
}

void armyGroup::Dismiss(i32 slot) {
    m_creatureTypes[slot] = CREATURE_NONE;
    m_creatureCounts[slot] = 0;
}

i32 armyGroup::IsMember(CreatureType creatureType) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] == creatureType)
            return 1;
    }
    return 0;
}

ArmyGroupAlignmentResult armyGroup::IsHomogeneous(i32 countRaces) {
    i32 numCreatureTypes = 0;
    u8 raceUsed[ARMY_GROUP_RACE_COUNT];
    memset(raceUsed, 0, sizeof(raceUsed));
    CreatureType prev = CREATURE_NONE;
    i32 numRaces;
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != CREATURE_NONE) {
            if (countRaces == ARMY_GROUP_EMPTY_SLOT)
                ++raceUsed[H2EnumIndex(gMonsterDatabase[H2EnumIndex(m_creatureTypes[i])].race)];
            if (m_creatureTypes[i] != prev) {
                ++numCreatureTypes;
                prev = m_creatureTypes[i];
            }
        }
    }

    if (numCreatureTypes <= 1)
        return ARMY_GROUP_ALIGNMENT_NO_MODIFIER;

    numRaces = 0;
    for (i = 0; i < ARMY_GROUP_RACE_COUNT; ++i) {
        if (raceUsed[i])
            ++numRaces;
    }

    if (numRaces == 1)
        return ARMY_GROUP_ALIGNMENT_SAME;
    if (numRaces == THREE_ALIGNMENT_COUNT)
        return ARMY_GROUP_ALIGNMENT_THREE;
    if (numRaces == FOUR_ALIGNMENT_COUNT)
        return ARMY_GROUP_ALIGNMENT_FOUR;
    if (numRaces >= FIVE_ALIGNMENT_COUNT)
        return ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE;
    return ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
}

i32 armyGroup::CanJoin(CreatureType creatureType) {
    if (IsMember(creatureType))
        return 1;
    if (IsMember(CREATURE_NONE))
        return 1;
    return 0;
}

i32 armyGroup::GetNumArmies(void) {
    i32 numArmies = 0;
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != CREATURE_NONE)
            ++numArmies;
    }
    return numArmies;
}

i32 armyGroup::Add(
    CreatureType creatureType, i32 quantity, i32 slot
) {
    i32 searchSlot;
    if (slot == ARMY_GROUP_EMPTY_SLOT) {
        for (searchSlot = 0; searchSlot < ARMY_GROUP_SLOT_COUNT; ++searchSlot) {
            if (m_creatureTypes[searchSlot] == creatureType) {
                slot = searchSlot;
                break;
            }
        }
    }
    if (slot == ARMY_GROUP_EMPTY_SLOT) {
        for (searchSlot = 0; searchSlot < ARMY_GROUP_SLOT_COUNT; ++searchSlot) {
            if (m_creatureTypes[searchSlot] == CREATURE_NONE
                || m_creatureTypes[searchSlot] == creatureType) {
                slot = searchSlot;
                break;
            }
        }
    }
    if (slot >= ARMY_GROUP_SLOT_COUNT || slot < 0)
        return 0;

    m_creatureTypes[slot] = creatureType;
    if (m_creatureCounts[slot] < 0)
        m_creatureCounts[slot] = 0;
    m_creatureCounts[slot] += quantity;
    return 1;
}

void armyGroup::Swap(i32 slot, armyGroup* otherGroup, i32 otherSlot) {
    SwapValues(m_creatureTypes[slot], otherGroup->m_creatureTypes[otherSlot]);
    SwapValues(m_creatureCounts[slot], otherGroup->m_creatureCounts[otherSlot]);
}

void armyGroup::DamageGroup(float damagePercent) {
    i32 killed;
    i32 killChance = static_cast<i32>(
        damagePercent
        * H2EnumIndex(ARMY_GROUP_RANDOM_PERCENT_MAX)
    );
    i32 i;
    i32 isFirstTroop = 1;
    i32 j;

    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != CREATURE_NONE) {
            killed = 0;
            for (j = 0; j < m_creatureCounts[i]; ++j) {
                if (SRandom(0, ARMY_GROUP_RANDOM_PERCENT_MAX) < killChance)
                    ++killed;
            }
            if (isFirstTroop && killed == m_creatureCounts[i]
                && damagePercent < 0.999)
                --killed;
            m_creatureCounts[i] -= killed;
            if (m_creatureCounts[i] <= 0 || damagePercent >= 1.0) {
                m_creatureCounts[i] = 0;
                m_creatureTypes[i] = CREATURE_NONE;
            }
            isFirstTroop = 0;
        } else {
            m_creatureCounts[i] = 0;
        }
    }
}
