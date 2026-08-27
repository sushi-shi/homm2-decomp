#ifndef HOMM2_BASE_ICONWIDGET_H
#define HOMM2_BASE_ICONWIDGET_H

#include <Ints.h>
#include "IconDraw.h"
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class icon;
class iconWidget : public widget {
public:
    icon* m_icon;
    i16 m_frame;
    i8 m_orientation;
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
        IconDrawOrientation orientation,
        i16 id,
        WidgetKind kind,
        i16 fillColor
    );
    iconWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        const char* iconName,
        i16 frame,
        IconDrawOrientation orientation,
        i16 id,
        WidgetKind kind,
        i16 fillColor
    );
    virtual  ~iconWidget() override;
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void Read(void);
};
#pragma pack(pop)
#endif
