#ifndef HOMM2_VIEWWRLD_H
#define HOMM2_VIEWWRLD_H

#include <BASE/message.h>
#include <SOURCE/KB_TYPES.h>

struct tag_message;

H2_ENUM_CLASS_BEGIN(ViewWorldScale)
    VIEW_WORLD_SCALE_FAR    = 4,
    VIEW_WORLD_SCALE_MIDDLE = 6,
    VIEW_WORLD_SCALE_NEAR   = 12
H2_ENUM_CLASS_END(ViewWorldScale)

MessageDispatchResult ViewWorldDialogHandler(struct tag_message&);

#endif
