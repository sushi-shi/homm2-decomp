#ifndef HOMM2_SOURCE_TOWNOBJECT_H
#define HOMM2_SOURCE_TOWNOBJECT_H
#include <va.h>

class border;
class icon;

#pragma pack(push, 1)
class townObject {
public:
    i32 m_animationFrameCount;
    i32 m_animationFrame;
    i32 m_visible;
    i32 m_buildingId;
    icon* m_icon;
    border* m_border;
    townObject(i32, i32, char*);
    ~townObject();
    void Draw(i32);
};
#pragma pack(pop)
SIZE(townObject, 0x18);
#endif
