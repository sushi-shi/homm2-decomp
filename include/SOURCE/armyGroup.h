#ifndef HOMM2_SOURCE_ARMYGROUP_H
#define HOMM2_SOURCE_ARMYGROUP_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

class hero;
class town;

typedef enum ArmyGroupConstant {
    ARMY_GROUP_EMPTY_SLOT         = -1,
    ARMY_GROUP_SLOT_COUNT         = 5,
    ARMY_GROUP_RACE_COUNT         = 7,
    ARMY_GROUP_RANDOM_PERCENT_MAX = 100,
    ARMY_GROUP_MORALE_MIN         = -3,
    ARMY_GROUP_MORALE_MAX         = 3
} ArmyGroupConstant;

enum {
    ARMY_GROUP_ALIGNMENT_FIVE_OR_MORE = -3,
    ARMY_GROUP_ALIGNMENT_FOUR         = -2,
    ARMY_GROUP_ALIGNMENT_THREE        = -1,
    ARMY_GROUP_ALIGNMENT_NO_MODIFIER  = 0,
    ARMY_GROUP_ALIGNMENT_SAME         = 1
};
typedef i32 ArmyGroupAlignmentResult;
#pragma pack(push, 1)
class armyGroup {
public:
    union {
        i8 m_creatureTypes[ARMY_GROUP_SLOT_COUNT];
        i8 m_troopTypes[ARMY_GROUP_SLOT_COUNT];
    };
    union {
        i16 m_creatureCounts[ARMY_GROUP_SLOT_COUNT];
        u16 m_troopCounts[ARMY_GROUP_SLOT_COUNT];
        i16 m_quantities[ARMY_GROUP_SLOT_COUNT];
    };
    armyGroup(void);
    void View(i32);
    i32 HasAllUndead(void);
    i32 HasSomeUndead(void);
    i32 GetMorale(class hero* armyHero, class town* occupiedTown, class armyGroup* enemyGroup);
    void Dismiss(i32 slot);
    i32 IsMember(CreatureType creatureType);
    ArmyGroupAlignmentResult IsHomogeneous(i32 countRaces);
    i32 CanJoin(CreatureType creatureType);
    i32 GetNumArmies(void);
    i32 Add(CreatureType creatureType, i32 quantity, i32 slot);
    void Swap(i32 slot, class armyGroup* otherGroup, i32 otherSlot);
    void DamageGroup(float damagePercent);
};
#pragma pack(pop)

#define CLEAR_ARMY_GROUP(group)                                                                    \
    (memset((group).m_creatureTypes, ARMY_GROUP_EMPTY_SLOT, sizeof((group).m_creatureTypes)),      \
     memset((group).m_creatureCounts, 0, sizeof((group).m_creatureCounts)))

#define ARMY_GROUP_HAS_POSITIVE_STACK(group, slot)                                                 \
    ((group).m_creatureTypes[slot] != CREATURE_NONE && (group).m_creatureCounts[slot] > 0)
#endif
