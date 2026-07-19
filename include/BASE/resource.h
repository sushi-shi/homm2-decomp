#ifndef HOMM2_BASE_RESOURCE_H
#define HOMM2_BASE_RESOURCE_H

#include <va.h>

#pragma pack(push, 1)
H2_ENUM_CLASS_BEGIN_T(ResourceCategory, i16)
    RESOURCE_CATEGORY_BITMAP  = 0,
    RESOURCE_CATEGORY_ICON    = 1,
    RESOURCE_CATEGORY_PALETTE = 2,
    RESOURCE_CATEGORY_TILESET = 3,
    RESOURCE_CATEGORY_FONT    = 5,
    RESOURCE_CATEGORY_SAMPLE  = 6
H2_ENUM_CLASS_END_T(ResourceCategory, i16)

H2_ENUM_BEGIN(ResourceReferenceCount)
    RESOURCE_REFERENCE_UNMANAGED = -1,
    RESOURCE_REFERENCE_EMPTY     = 0,
    RESOURCE_REFERENCE_INITIAL   = 1
H2_ENUM_END(ResourceReferenceCount)

class resource {
public:
    ResourceCategory m_resourceType;
    i16 m_refCount;
    i32 m_id;
    resource* m_next;
    resource(void);
    resource(ResourceCategory category, u32l id, i16 refCount, class resource* next);
    virtual ~resource(void) = 0;
};
#pragma pack(pop)
#endif
