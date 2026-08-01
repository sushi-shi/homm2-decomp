#ifndef HOMM2_BASE_ICONWIDGET_H
#define HOMM2_BASE_ICONWIDGET_H

#include <va.h>
#include "IconDraw.h"
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class icon;
class iconWidget : public widget {
public:
    icon* m_icon;
    i16 m_frame;
    H2_ENUM_STORAGE(IconDrawOrientation, i8) m_orientation;
    i16 m_fillColor;
    u32l m_iconId;
    iconWidget(void);
    iconWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        u32l iconId,
        i16 frame,
        H2_ENUM_PARAM(IconDrawOrientation, i8) orientation,
        i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind,
        i16 fillColor
    );
    iconWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* iconName,
        i16 frame,
        H2_ENUM_PARAM(IconDrawOrientation, i8) orientation,
        i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind,
        i16 fillColor
    );
    __declspec(dllexport) virtual inline ~iconWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message&) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
#endif
