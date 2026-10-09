#ifndef HOMM2_SOURCE_ARMYGROUP_H
#define HOMM2_SOURCE_ARMYGROUP_H

#include <match.h>
#include <Domains.h>
#include <SOURCE/kbTypes.h>

class hero;
class town;

H2_ENUM_BEGIN(ArmyGroupConstant)
    ARMY_GROUP_EMPTY_SLOT         = -1,
    ARMY_GROUP_SLOT_COUNT         = 5,
    ARMY_GROUP_RACE_COUNT         = 7,
    ARMY_GROUP_RANDOM_PERCENT_MAX = 100,
    ARMY_GROUP_MORALE_MIN         = -3,
    ARMY_GROUP_MORALE_MAX         = 3
H2_ENUM_END(ArmyGroupConstant)

H2_ENUM_CLASS_BEGIN(ArmyGroupAlignmentResult)
    ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE = -3,
    ARMY_GROUP_ALIGNMENT_FOUR         = -2,
    ARMY_GROUP_ALIGNMENT_THREE        = -1,
    ARMY_GROUP_ALIGNMENT_NO_MODIFIER  = 0,
    ARMY_GROUP_ALIGNMENT_SAME         = 1
H2_ENUM_CLASS_END(ArmyGroupAlignmentResult)

#pragma pack(push, 1)
class armyGroup {
public:
    H2_ENUM_STORAGE(CreatureType, i8) m_creatureTypes[ARMY_GROUP_SLOT_COUNT];
    i16 m_creatureCounts[ARMY_GROUP_SLOT_COUNT];
    armyGroup(void);
    void View(i32);
    b32 HasAllUndead(void);
    b32 HasSomeUndead(void);
    i32 GetMorale(class hero* armyHero, class town* occupiedTown, class armyGroup* enemyGroup);
    void Dismiss(i32 slot);
    b32 IsMember(H2_ENUM_PARAM(CreatureType, i32) creatureType);
    ArmyGroupAlignmentResult IsHomogeneous(i32 alignmentMode);
    b32 CanJoin(H2_ENUM_PARAM(CreatureType, i32) creatureType);
    i32 GetNumArmies(void);
    b32 Add(H2_ENUM_PARAM(CreatureType, i32) creatureType, i32 quantity, i32 slot);
    void Swap(i32 slot, class armyGroup* otherGroup, i32 otherSlot);
    void DamageGroup(float casualtyFraction);
};
#pragma pack(pop)
SIZE(armyGroup, 0xf);
// Two array-major clears, not a whole-object zero or a slot-major Dismiss loop.
#define CLEAR_ARMY_GROUP(group)                                                                    \
    (memset((group).m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof((group).m_creatureTypes)),      \
     memset((group).m_creatureCounts, 0, sizeof((group).m_creatureCounts)))
// Type-first query; use the signed quantity view, not the unsigned troop alias.
#define ARMY_GROUP_HAS_POSITIVE_STACK(group, slot)                                                 \
    ((group).m_creatureTypes[slot] != CREATURE_NONE && (group).m_creatureCounts[slot] > 0)
#endif // HOMM2_SOURCE_ARMYGROUP_H
