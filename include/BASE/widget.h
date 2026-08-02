#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H

#include <va.h>
#include <BASE/message.h>
#include <BASE/widgetKind.h>

class heroWindow;
struct tag_message;

H2_ENUM_CLASS_BEGIN_SPLIT(WidgetFlag, i16)
    WIDGET_FLAG_NONE     = 0,
    WIDGET_FLAG_SELECTED = 1,
    WIDGET_FLAG_ENABLED  = 2,
    WIDGET_FLAG_DRAW     = 4,
    WIDGET_FLAG_DIMMED   = 8,
    WIDGET_FLAG_GRAYED   = 0x1000,
    WIDGET_FLAG_UPDATE   = 0x4000
H2_ENUM_CLASS_END_SPLIT(WidgetFlag, i16)
H2_ENUM_FLAGS(WidgetFlag)

H2_ENUM_CLASS_BEGIN(WidgetCommandArgument)
    WIDGET_COMMAND_DIMMED = 0x1000
H2_ENUM_CLASS_END(WidgetCommandArgument)

#pragma pack(push, 1)
class widget {
public:
    heroWindow* m_owner;
    widget* m_next;
    widget* m_prev;
    i16 m_id;
    i16 m_zOrder;
    H2_ENUM_STORAGE(WidgetKind, i16) m_kind;
    H2_ENUM_STORAGE(WidgetFlag, i16) m_flags;
    i16 m_x;
    i16 m_y;
    i16 m_width;
    i16 m_height;
    widget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind
    );
    widget(void);
    virtual void Draw(void) = 0;
    virtual ~widget(void) = 0;
    virtual MessageDispatchResult Main(struct tag_message& message) = 0;
    i32 Open(i32 zOrder, class heroWindow* owner);
    void Close(void);
    void Dim(void);
};
#pragma pack(pop)
SIZE(widget, 0x20);
#endif
