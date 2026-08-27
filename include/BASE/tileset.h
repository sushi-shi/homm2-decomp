#ifndef HOMM2_BASE_TILESET_H
#define HOMM2_BASE_TILESET_H

#include <Ints.h>
#include <BASE/resource.h>

#pragma pack(push, 1)
class tileset : public resource {
public:
    u16 m_tileCount;
    u16 m_tileWidth;
    u16 m_tileHeight;
    char* m_data;
    tileset(u32l);
    virtual  ~tileset();
};
#pragma pack(pop)
#endif
