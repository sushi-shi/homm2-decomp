#ifndef HOMM2_BASE_ICONWIDGET_H
#define HOMM2_BASE_ICONWIDGET_H

#include <va.h>
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class icon;
class iconWidget : public widget {
public:
    icon* m_icon;
    i16 m_frame;
    i8 m_flip;
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
        i8 flip,
        i16 id,
        i16 kind,
        i16 fillColor
    );
    iconWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* iconName,
        i16 frame,
        i8 flip,
        i16 id,
        i16 kind,
        i16 fillColor
    );
    virtual ~iconWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
SIZE(iconWidget, 0x2d);
#endif
