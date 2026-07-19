#ifndef HOMM2_BASE_TILESET_H
#define HOMM2_BASE_TILESET_H

#include <va.h>
#include <BASE/resource.h>

#pragma pack(push, 1)
class tileset : public resource {
public:
    u16 m_tileWidth;
    u16 m_tileHeight;
    u16 m_tileCount;
    char* m_data;
    tileset(u32l);
    virtual ~tileset();
};
#pragma pack(pop)
SIZE(tileset, 0x1a);
#endif
