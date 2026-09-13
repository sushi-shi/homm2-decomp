#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H

#include <va.h>
#include <BASE/message.h>

class widget;
class bitmap;
struct tag_message;

H2_ENUM_CLASS_BEGIN(WindowFlag)
    WINDOW_FLAG_NONE            = 0,
    WINDOW_FLAG_FIXED_LAYER     = 1,
    WINDOW_FLAG_SAVE_BACKGROUND = 2,
    WINDOW_FLAG_STRIP_WINDOW    = 8,
    WINDOW_FLAG_OWNS_WIDGETS    = 0x4000,
    WINDOW_UPDATE_SUPPRESS_MASK = 0x7fff
H2_ENUM_CLASS_END(WindowFlag)
H2_ENUM_FLAGS(WindowFlag)

H2_ENUM_BEGIN(WindowDrawId)
    WINDOW_ALL_WIDGETS_LOW  = -0xffff,
    WINDOW_ALL_WIDGETS_HIGH = 0xffff,
    WINDOW_DRAW_ID_LIMIT    = 0x7fff
H2_ENUM_END(WindowDrawId)

H2_ENUM_CLASS_BEGIN(WindowState)
    WINDOW_STATE_CLOSED = 0,
    WINDOW_STATE_OPEN   = 1
H2_ENUM_CLASS_END(WindowState)
H2_ENUM_FLAGS(WindowState)

H2_ENUM_BEGIN(HeroWindowConstant)
    HERO_WINDOW_NAME_CAPACITY = 0x14
H2_ENUM_END(HeroWindowConstant)

#pragma pack(push, 1)
class heroWindow {
public:
    i32 m_zOrder;
    heroWindow* m_nextWindow;
    heroWindow* m_prevWindow;
    char name[HERO_WINDOW_NAME_CAPACITY];
    WindowFlag m_winFlags;
    WindowState m_winState;
    i32 m_posX;
    i32 m_posY;
    i32 m_winWidth;
    i32 m_winHeight;
    widget* m_widgetListTail;
    widget* m_widgetListHead;
    bitmap* m_savedBackground;
    heroWindow(void);
    heroWindow(i32 x, i32 y, i32 width, i32 height, H2_ENUM_PARAM(WindowFlag, i32) flags);
    heroWindow(i32 x, i32 y, H2_CONST char* resourceName);
    i32 Open(i32 x, i32 flags);
    void RemoveAndDeleteWidget(i32 id);
    void Close(void);
    void AddWidget(class widget* newWidget, i32 zOrder);
    void RemoveWidget(class widget* currentWidget);
    MessageDispatchResult BroadcastMessage(struct tag_message& message);
    void DrawWindow(void);
    void DrawWindow(i32 flags);
    void DrawWindow(i32 update, i32 firstId, i32 lastId);
    i32 SaveBackground(void);
    void RestoreBackground(void);
    void MoveWindow(i32 dx, i32 dy);
};
#pragma pack(pop)
#endif
