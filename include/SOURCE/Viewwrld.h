#ifndef HOMM2_VIEWWRLD_H
#define HOMM2_VIEWWRLD_H

#include <Ints.h>

struct tag_message;

H2_ENUM_CLASS_BEGIN(ViewWorldMode)
    VIEW_WORLD_MINES     = 0x30,
    VIEW_WORLD_RESOURCES = 0x31,
    VIEW_WORLD_ARTIFACTS = 0x32,
    VIEW_WORLD_TOWNS     = 0x33,
    VIEW_WORLD_HEROES    = 0x34,
    VIEW_WORLD_ALL       = 0x35
H2_ENUM_CLASS_END(ViewWorldMode)

H2_ENUM_CLASS_BEGIN(ViewWorldScale)
    VIEW_WORLD_SCALE_FAR           = 4,
    VIEW_WORLD_SCALE_MIDDLE        = 6,
    VIEW_WORLD_SCALE_NEAR          = 12,
    VIEW_WORLD_FAR_MAX_MAP_SIZE    = 108,
    VIEW_WORLD_MIDDLE_MAX_MAP_SIZE = 72,
    VIEW_WORLD_NEAR_MAX_MAP_SIZE   = 36
H2_ENUM_CLASS_END(ViewWorldScale)

i32 ViewWorldDialogHandler(struct tag_message&);

#endif
