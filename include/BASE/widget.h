#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H

#include <va.h>

class heroWindow;
struct tag_message;

// Expansion-only dispatch ABI constants keep VC4.2's enum state byte-neutral.
#define WIDGET_DISPATCH_CONTINUE 0
#define WIDGET_DISPATCH_CONSUME 1
#define WIDGET_DISPATCH_FORWARD 2

H2_ENUM_BEGIN(WidgetFlag)
    WIDGET_FLAG_SELECTED = 1,
    WIDGET_FLAG_ENABLED  = 2,
    WIDGET_FLAG_DRAW     = 4,
    WIDGET_FLAG_DIMMED   = 8,
    WIDGET_FLAG_GRAYED   = 0x1000,
    WIDGET_FLAG_UPDATE   = 0x4000
H2_ENUM_END(WidgetFlag)

H2_ENUM_BEGIN(WidgetCommandArgument)
    WIDGET_COMMAND_DIMMED = 0x1000
H2_ENUM_END(WidgetCommandArgument)

#pragma pack(push, 1)
class widget {
public:
    heroWindow* m_owner;
    widget* m_next;
    widget* m_prev;
    i16 m_id;
    i16 m_zOrder;
    i16 m_kind;
    i16 m_flags;
    i16 m_x;
    i16 m_y;
    i16 m_width;
    i16 m_height;
    widget(i16 x, i16 y, i16 width, i16 height, i16 id, i16 kind);
    widget(void);
    virtual void Draw(void) = 0;
    virtual ~widget(void) = 0;
    virtual i32 Main(struct tag_message& message) = 0;
    i32 Open(i32 zOrder, class heroWindow* owner);
    void Close(void);
    void Dim(void);
};
#pragma pack(pop)
SIZE(widget, 0x20);
#endif
