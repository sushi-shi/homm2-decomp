#ifndef HOMM2_BASE_WIDGET_KIND_H
#define HOMM2_BASE_WIDGET_KIND_H

#include <Ints.h>

// Known serialized values only. The domain remains open-ended, and widget stores
// the value in its proven packed short field rather than as this enum type.
H2_ENUM_BEGIN(WidgetKind)
    WIDGET_KIND_DEFAULT = 2,
    WIDGET_KIND_UNDIMMED = 8,
    WIDGET_KIND_ICON_DIRECT = 0x10,
    WIDGET_KIND_ICON_CENTERED = 0x11,
    WIDGET_KIND_ICON_FILL = 0x80,
    WIDGET_KIND_TEXT = 0x200,
    WIDGET_KIND_SOLID = 0x400,
    WIDGET_KIND_BITMAP = 0x800,
    WIDGET_KIND_ICON = 0x801,
    WIDGET_KIND_AUTO_REPEAT = 0x1000,
    WIDGET_KIND_TEXT_ENTRY = 0x4000
H2_ENUM_END(WidgetKind)

#ifdef HOMM2_STRICT_ENUM_TYPES
inline WidgetKind DecodeWidgetKind(i16 value) {
    return static_cast<WidgetKind>(value);
}

inline i16 EncodeWidgetKind(WidgetKind value) {
    return static_cast<i16>(value);
}
#else
#define DecodeWidgetKind(value) (value)
#define EncodeWidgetKind(value) (value)
#endif

#endif
