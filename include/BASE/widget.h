#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H

#include <Ints.h>
#include <BASE/message.h>
#include <BASE/widgetKind.h>

class heroWindow;
struct tag_message;


#define WIDGET_CONTAINS_LOCAL_POINT(w, x, y)                                                       \
    ((x) >= (w).m_x && (y) >= (w).m_y && (x) < (w).m_x + (w).m_width                               \
     && (y) < (w).m_y + (w).m_height)


#define READ_WIDGET_GEOMETRY(w, resources) \
    ((w).m_x = (resources)->ReadWord(), (w).m_y = (resources)->ReadWord(), \
     (w).m_width = (resources)->ReadWord(), (w).m_height = (resources)->ReadWord())

enum {
    WIDGET_FLAG_NONE     = 0,
    WIDGET_FLAG_SELECTED = 1,
    WIDGET_FLAG_ENABLED  = 2,
    WIDGET_FLAG_DRAW     = 4,
    WIDGET_FLAG_DIMMED   = 8,
    WIDGET_FLAG_UPDATE   = 0x4000
};
typedef i32 WidgetFlag;

enum {

    WIDGET_FLAGS_ARGUMENT_DIMMED = 0x1000
};
typedef i32 WidgetFlagArgument;
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
