#include <va.h>
#include <string.h>
#include <BASE/Misc.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/town.h>

H2_ENUM_BEGIN(MoraleConstant)
    FIZBIN_MORALE_PENALTY  = 2,
    COLISEUM_MORALE_BONUS  = 2,
    THREE_ALIGNMENT_COUNT  = 3,
    FOUR_ALIGNMENT_COUNT   = 4,
    FIVE_ALIGNMENT_COUNT   = 5,
    THREE_ALIGNMENT_MORALE = -1,
    FOUR_ALIGNMENT_MORALE  = -2
H2_ENUM_END(MoraleConstant)

VA(0x0048c040, 0x3c)
armyGroup::armyGroup(void) {
    memset(m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof(m_creatureTypes));
    memset(m_creatureCounts, 0, sizeof(m_creatureCounts));
}

VA(0x0048c07c, 0x18)
void armyGroup::View(i32) {}

VA(0x0048c094, 0x73)
i32 armyGroup::HasAllUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != ARMY_GROUP_EMPTY_SLOT
            && !(gMonsterDatabase[m_creatureTypes[slot]].attributes & MONSTER_ATTRIBUTE_UNDEAD))
            return 0;
    }
    return 1;
}

VA(0x0048c107, 0x73)
i32 armyGroup::HasSomeUndead(void) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] != ARMY_GROUP_EMPTY_SLOT
            && HAS(gMonsterDatabase[m_creatureTypes[slot]].attributes, MONSTER_ATTRIBUTE_UNDEAD))
            return 1;
    }
    return 0;
}

VA(0x0048c17a, 0x24d)
i32 armyGroup::GetMorale(hero* armyHero, town* occupiedTown, armyGroup* enemyGroup) {
    i32 morale = 0;
    i32 alignmentValue;
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
            if (enemyGroup->m_creatureTypes[index] == IDX(CREATURE_BONE_DRAGON))
                enemyHasBoneDragon = 1;
        }
    }

    if (enemyHasBoneDragon)
        --morale;

    if (armyHero != NULL) {
        if (armyHero->HasArtifact(ARTIFACT_BATTLE_GARB))
            return ARMY_GROUP_MORALE_MAX;

        morale += armyHero->m_secondarySkills[IDX(HERO_SKILL_LEADERSHIP)];
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
    if (hasSomeUndead && alignmentValue > 0)
        alignmentValue = 0;
    morale += alignmentValue;

    if (occupiedTown != NULL && occupiedTown->m_type != IDX(FACTION_NECROMANCER)
        && (occupiedTown->m_buildings & IDX(TOWN_BUILDING_TAVERN)))
        ++morale;
    if (occupiedTown != NULL && occupiedTown->m_type == IDX(FACTION_BARBARIAN)
        && (occupiedTown->m_buildings & IDX(TOWN_BUILDING_COLISEUM)))
        morale += COLISEUM_MORALE_BONUS;

    if (morale < ARMY_GROUP_MORALE_MIN)
        morale = ARMY_GROUP_MORALE_MIN;
    else if (morale > ARMY_GROUP_MORALE_MAX)
        morale = ARMY_GROUP_MORALE_MAX;

    return morale;
}

VA(0x0048c3c7, 0x2f)
void armyGroup::Dismiss(i32 slot) {
    m_creatureTypes[slot] = ARMY_GROUP_EMPTY_SLOT;
    m_creatureCounts[slot] = 0;
}

VA(0x0048c3f6, 0x55)
i32 armyGroup::IsMember(i32 creatureType) {
    for (i32 slot = 0; slot < ARMY_GROUP_SLOT_COUNT; ++slot) {
        if (m_creatureTypes[slot] == creatureType)
            return 1;
    }
    return 0;
}

VA(0x0048c44b, 0x14e)
i32 armyGroup::IsHomogeneous(i32 countRaces) {
    i32 numCreatureTypes = 0;
    u8 raceUsed[ARMY_GROUP_RACE_COUNT];
    memset(raceUsed, 0, sizeof(raceUsed));
    i32 last = ARMY_GROUP_EMPTY_SLOT;
    i32 nRaces;
    i32 i;
    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != ARMY_GROUP_EMPTY_SLOT) {
            if (countRaces == ARMY_GROUP_EMPTY_SLOT)
                ++raceUsed[gMonsterDatabase[m_creatureTypes[i]].race];
            if (m_creatureTypes[i] != last) {
                ++numCreatureTypes;
                last = m_creatureTypes[i];
            }
        }
    }

    if (numCreatureTypes <= 1)
        return 0;

    nRaces = 0;
    for (i = 0; i < ARMY_GROUP_RACE_COUNT; ++i) {
        if (raceUsed[i])
            ++nRaces;
    }

    if (nRaces == 1)
        return 1;
    if (nRaces == THREE_ALIGNMENT_COUNT)
        return THREE_ALIGNMENT_MORALE;
    if (nRaces == FOUR_ALIGNMENT_COUNT)
        return FOUR_ALIGNMENT_MORALE;
    if (nRaces >= FIVE_ALIGNMENT_COUNT)
        return ARMY_GROUP_MORALE_MIN;
    return 0;
}

VA(0x0048c599, 0x54)
i32 armyGroup::CanJoin(i32 creatureType) {
    if (IsMember(creatureType))
        return 1;
    if (IsMember(ARMY_GROUP_EMPTY_SLOT))
        return 1;
    return 0;
}

VA(0x0048c5ed, 0x54)
i32 armyGroup::GetNumArmies(void) {
    i32 numArmies = 0;
    for (i32 i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != ARMY_GROUP_EMPTY_SLOT)
            ++numArmies;
    }
    return numArmies;
}

VA(0x0048c641, 0x11c)
i32 armyGroup::Add(i32 creatureType, i32 quantity, i32 slot) {
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
            if (m_creatureTypes[searchSlot] == ARMY_GROUP_EMPTY_SLOT
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

VA(0x0048c75d, 0x75)
void armyGroup::Swap(i32 slot, armyGroup* otherGroup, i32 otherSlot) {
    i32 temporary = m_creatureTypes[slot];
    m_creatureTypes[slot] = otherGroup->m_creatureTypes[otherSlot];
    otherGroup->m_creatureTypes[otherSlot] = temporary;

    temporary = m_creatureCounts[slot];
    m_creatureCounts[slot] = otherGroup->m_creatureCounts[otherSlot];
    otherGroup->m_creatureCounts[otherSlot] = temporary;
}

VA(0x0048c7d2, 0x14d)
void armyGroup::DamageGroup(float damagePercent) {
    i32 numKilled;
    i32 percentChance = static_cast<i32>(damagePercent * IDX(ARMY_GROUP_RANDOM_PERCENT_MAX));
    i32 i;
    i32 isFirstTroop = 1;
    i32 j;

    for (i = 0; i < ARMY_GROUP_SLOT_COUNT; ++i) {
        if (m_creatureTypes[i] != ARMY_GROUP_EMPTY_SLOT) {
            numKilled = 0;
            for (j = 0; j < m_creatureCounts[i]; ++j) {
                if (SRandom(0, ARMY_GROUP_RANDOM_PERCENT_MAX) < percentChance)
                    ++numKilled;
            }
            if (isFirstTroop && m_creatureCounts[i] == numKilled
                && damagePercent < 0.999) // NOLINT(readability-magic-numbers)
                --numKilled;
            m_creatureCounts[i] -= numKilled;
            if (m_creatureCounts[i] <= 0 || damagePercent >= 1.0) {
                m_creatureCounts[i] = 0;
                m_creatureTypes[i] = ARMY_GROUP_EMPTY_SLOT;
            }
            isFirstTroop = 0;
        } else {
            m_creatureCounts[i] = 0;
        }
    }
}
