#include <va.h>
#include <string.h>
#include <BASE/Misc.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/town.h>

H2_ENUM_BEGIN(MoraleConstant)
    FIZBIN_MORALE_PENALTY = 2,
    COLISEUM_MORALE_BONUS = 2,
    THREE_ALIGNMENT_COUNT = 3,
    FOUR_ALIGNMENT_COUNT  = 4,
    FIVE_ALIGNMENT_COUNT  = 5
H2_ENUM_END(MoraleConstant)

#if H2_STRICT_ENUMS
template <typename Value>
void SwapValues(Value& lhs, Value& rhs) {
    Value temporary = lhs;
    lhs = rhs;
    rhs = temporary;
}
#endif

VA(0x00421710, 0x31)
armyGroup::armyGroup(void) {
    memset(m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof(m_creatureTypes));
    memset(m_creatureCounts, 0, sizeof(m_creatureCounts));
}

VA(0x00421741, 0xd)
void armyGroup::View(i32) {}

VA(0x0042174e, 0x5a)
i32 armyGroup::HasAllUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != CREATURE_NONE
            && !(gMonsterDatabase[IDX(m_creatureTypes[slot])].attributes
                 & MONSTER_ATTRIBUTE_UNDEAD))
            return 0;
    }
    return 1;
}

VA(0x004217a8, 0x5a)
i32 armyGroup::HasSomeUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != CREATURE_NONE
            && HAS(
                gMonsterDatabase[IDX(m_creatureTypes[slot])].attributes,
                MONSTER_ATTRIBUTE_UNDEAD
            ))
            return 1;
    }
    return 0;
}

VA(0x00421802, 0x231)
i32 armyGroup::GetMorale(hero* armyHero, town* occupiedTown, armyGroup* enemyGroup) {
    i32 morale = 0;
    ArmyGroupAlignmentResult alignmentValue;
    i32 hasSomeUndead = 0;
    i32 moraleModifier = 0;
    i32 enemyHasBoneDragon;
    i32 index;
    alignmentValue = IsHomogeneous(ARMY_GROUP_EMPTY_SLOT);

    if (HasAllUndead())
        return 0;

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
        --morale;

    if (armyHero != NULL) {
        if (armyHero->HasArtifact(ARTIFACT_BATTLE_GARB))
            return ARMY_GROUP_MORALE_MAX;

        morale += IDX(armyHero->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)]);
        morale += armyHero->m_morale;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_VALOR))
            ++morale;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_COURAGE))
            ++morale;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_HONOR))
            ++morale;
        if (armyHero->HasArtifact(ARTIFACT_MEDAL_OF_DISTINCTION))
            ++morale;
        if (armyHero->HasArtifact(ARTIFACT_FIZBIN_OF_MISFORTUNE))
            morale -= FIZBIN_MORALE_PENALTY;
        if (armyHero->HasArtifact(ARTIFACT_ARM_OF_MARTYR))
            hasSomeUndead = 1;
        if (armyHero->HasArtifact(ARTIFACT_MASTHEAD)
            && HAS(armyHero->m_eventFlags, HERO_EVENT_EMBARKED))
            ++morale;
    }

    if (hasSomeUndead)
        --morale;
    if (hasSomeUndead && alignmentValue > ARMY_GROUP_ALIGNMENT_NO_MODIFIER)
        alignmentValue = ARMY_GROUP_ALIGNMENT_NO_MODIFIER;
    morale += IDX(alignmentValue);

    if (occupiedTown != NULL && occupiedTown->m_type != FACTION_NECROMANCER
        && (occupiedTown->m_buildings & IDX(TOWN_BUILDING_TAVERN)))
        ++morale;
    if (occupiedTown != NULL && occupiedTown->m_type == FACTION_BARBARIAN
        && (occupiedTown->m_buildings & IDX(TOWN_BUILDING_COLISEUM)))
        morale += COLISEUM_MORALE_BONUS;

    if (morale < ARMY_GROUP_MORALE_MIN)
        morale = ARMY_GROUP_MORALE_MIN;
    else if (morale > ARMY_GROUP_MORALE_MAX)
        morale = ARMY_GROUP_MORALE_MAX;

    return morale;
}

VA(0x00421a33, 0x23)
void armyGroup::Dismiss(i32 slot) {
    m_creatureTypes[slot] = CREATURE_NONE;
    m_creatureCounts[slot] = 0;
}

VA(0x00421a56, 0x40)
i32 armyGroup::IsMember(H2_ENUM_PARAM(CreatureType, i32) creatureType) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] == creatureType)
            return 1;
    }
    return 0;
}

VA(0x00421a96, 0x124)
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
                ++raceUsed[IDX(gMonsterDatabase[IDX(m_creatureTypes[i])].race)];
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

VA(0x00421bba, 0x3b)
i32 armyGroup::CanJoin(H2_ENUM_PARAM(CreatureType, i32) creatureType) {
    if (IsMember(creatureType))
        return 1;
    if (IsMember(CREATURE_NONE))
        return 1;
    return 0;
}

VA(0x00421bf5, 0x48)
i32 armyGroup::GetNumArmies(void) {
    i32 numArmies = 0;
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != CREATURE_NONE)
            ++numArmies;
    }
    return numArmies;
}

VA(0x00421c3d, 0xdf)
i32 armyGroup::Add(
    H2_ENUM_PARAM(CreatureType, i32) creatureType, i32 quantity, i32 slot
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

VA(0x00421d1c, 0x6b)
void armyGroup::Swap(i32 slot, armyGroup* otherGroup, i32 otherSlot) {
#if H2_STRICT_ENUMS
    SwapValues(m_creatureTypes[slot], otherGroup->m_creatureTypes[otherSlot]);
    SwapValues(m_creatureCounts[slot], otherGroup->m_creatureCounts[otherSlot]);
#else
    i32 temporary = m_creatureTypes[slot];
    m_creatureTypes[slot] = otherGroup->m_creatureTypes[otherSlot];
    otherGroup->m_creatureTypes[otherSlot] = temporary;

    temporary = m_creatureCounts[slot];
    m_creatureCounts[slot] = otherGroup->m_creatureCounts[otherSlot];
    otherGroup->m_creatureCounts[otherSlot] = temporary;
#endif
}

VA(0x00421d87, 0x133)
void armyGroup::DamageGroup(float damagePercent) {
    i32 killed;
    i32 killChance = static_cast<i32>(
        damagePercent
        * IDX(ARMY_GROUP_RANDOM_PERCENT_MAX)
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
