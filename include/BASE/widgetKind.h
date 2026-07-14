#ifndef HOMM2_BASE_WIDGET_KIND_H
#define HOMM2_BASE_WIDGET_KIND_H

// Known serialized values only. The domain remains open-ended, and widget stores
// the value in its proven packed short field rather than as this enum type.
typedef enum WidgetKind {
    WIDGET_KIND_DEFAULT = 2,
    WIDGET_KIND_UNDIMMED = 8,
    WIDGET_KIND_TEXT = 0x200,
    WIDGET_KIND_SOLID = 0x400,
    WIDGET_KIND_BITMAP = 0x800,
    WIDGET_KIND_ICON = 0x801,
    WIDGET_KIND_AUTO_REPEAT = 0x1000,
    WIDGET_KIND_TEXT_ENTRY = 0x4000
} WidgetKind;

#endif // HOMM2_BASE_WIDGET_KIND_H
