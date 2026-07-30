#ifndef HOMM2_BASE_WIDGET_KIND_H
#define HOMM2_BASE_WIDGET_KIND_H

#include <Ints.h>


enum class WidgetKind : i16 {
    WIDGET_KIND_NONE          = 0,
    WIDGET_KIND_TRANSPARENT   = 1,
    WIDGET_KIND_DEFAULT       = 2,
    WIDGET_KIND_UNDIMMED      = 8,
    WIDGET_KIND_ICON_DIRECT   = 0x10,
    WIDGET_KIND_ICON_CENTERED = 0x11,
    WIDGET_KIND_ICON_FILL     = 0x80,
    WIDGET_KIND_TEXT          = 0x200,
    WIDGET_KIND_SOLID         = 0x400,
    WIDGET_KIND_BITMAP        = 0x800,
    WIDGET_KIND_ICON          = 0x801,
    WIDGET_KIND_AUTO_REPEAT   = 0x1000,
    WIDGET_KIND_TEXT_ENTRY    = 0x4000
};
using enum WidgetKind;

#endif
