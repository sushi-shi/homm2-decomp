#ifndef HOMM2_VIEWWRLD_H
#define HOMM2_VIEWWRLD_H

#include <BASE/message.h>
#include <SOURCE/KB_TYPES.h>

struct tag_message;

enum class ViewWorldScale : i32 {
    VIEW_WORLD_SCALE_FAR    = 4,
    VIEW_WORLD_SCALE_MIDDLE = 6,
    VIEW_WORLD_SCALE_NEAR   = 12
};
using enum ViewWorldScale;

MessageDispatchResult ViewWorldDialogHandler(struct tag_message&);

#endif
