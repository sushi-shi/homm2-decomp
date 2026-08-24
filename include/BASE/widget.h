#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H

#include <Ints.h>
#include <BASE/message.h>
#include <BASE/widgetKind.h>

class heroWindow;
struct tag_message;

enum {
    WIDGET_FLAG_NONE     = 0,
    WIDGET_FLAG_SELECTED = 1,
    WIDGET_FLAG_ENABLED  = 2,
    WIDGET_FLAG_DRAW     = 4,
    WIDGET_FLAG_DIMMED   = 8,
    WIDGET_FLAG_GRAYED   = 0x1000,
    WIDGET_FLAG_UPDATE   = 0x4000
};
typedef i32 WidgetFlag;

enum {
    WIDGET_COMMAND_DIMMED = 0x1000
};
typedef i32 WidgetCommandArgument;
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
    widget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        i16 id,
        WidgetKind kind
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
#endif
