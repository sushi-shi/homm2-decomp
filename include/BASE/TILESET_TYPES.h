#ifndef HOMM2_BASE_TILESET_TYPES_H
#define HOMM2_BASE_TILESET_TYPES_H

#include <va.h>

H2_ENUM_BEGIN(TilesetSourceFileSize)
    TILESET_SOURCE_FILE_SLOT_SIZE = 0x2c,
    TILESET_SOURCE_FILE_SIZE = 0x29
H2_ENUM_END(TilesetSourceFileSize)

typedef struct STilesetSourceFiles {
    char allocation[TILESET_SOURCE_FILE_SLOT_SIZE];
    char destruction[TILESET_SOURCE_FILE_SIZE];
} STilesetSourceFiles;

SIZE(STilesetSourceFiles, 0x55);

#endif
