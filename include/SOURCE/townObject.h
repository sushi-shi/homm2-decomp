#ifndef HOMM2_SOURCE_TOWNOBJECT_H
#define HOMM2_SOURCE_TOWNOBJECT_H

#include <Ints.h>
#include <SOURCE/KB_TYPES.h>

class border;
class icon;

class townObject {
public:
    i32 m_animationFrameCount;
    i32 m_animationFrame;
    i32 m_visible;
    H2EnumStorage<BuildingSlotType, i32> m_buildingId;
    icon* m_icon;
    border* m_border;
    townObject(FactionType, BuildingSlotType, char*);
    ~townObject();
    void Draw(i32);
};
#endif
