#ifndef HOMM2_SOURCE_ARMYGROUP_H
#define HOMM2_SOURCE_ARMYGROUP_H

#include <va.h>

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
    i32 GetMorale(class hero*, class town*, class armyGroup*);
    void Dismiss(i32);
    i32 IsMember(i32);
    i32 IsHomogeneous(i32);
    i32 CanJoin(i32);
    i32 GetNumArmies(void);
    i32 Add(i32, i32, i32);
    void Swap(i32, class armyGroup*, i32);
    void DamageGroup(float);
};
#pragma pack(pop)
SIZE(armyGroup, 0xf);
#endif
