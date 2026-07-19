#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H
#include <va.h>
class widget;
class bitmap;
struct tag_message;

H2_ENUM_CLASS_BEGIN(WindowFlag)
    WINDOW_FLAG_OWNS_WIDGETS = 0x4000,    // Close/RemoveWidgetById delete the widgets
    WINDOW_UPDATE_SUPPRESS_MASK = 0x7fff, // DrawWindow skips the screen update when == 1
    WINDOW_ALL_WIDGETS_LOW = -0xffff,     // DrawWindow id-range sentinel: no filter
    WINDOW_ALL_WIDGETS_HIGH = 0xffff,
    WINDOW_DRAW_ID_LIMIT = 0x7fff
H2_ENUM_CLASS_END(WindowFlag)

H2_ENUM_CLASS_BEGIN(WindowWidgetRecordType)
    WINDOW_WIDGET_RECORD_END = 0,
    WINDOW_WIDGET_RECORD_BORDER = 1,
    WINDOW_WIDGET_RECORD_BUTTON = 2,
    WINDOW_WIDGET_RECORD_TEXT = 8,
    WINDOW_WIDGET_RECORD_ICON = 0x10,
    WINDOW_WIDGET_RECORD_DIMMER = 0x40,
    WINDOW_WIDGET_RECORD_TEXT_ENTRY = 0x100,
    WINDOW_WIDGET_RECORD_TEXT_ENTRY_RECT = 0x201,
    WINDOW_WIDGET_RECORD_TEXT_ENTRY_MULTILINE = 0x202,
    WINDOW_WIDGET_RECORD_DROP_LIST = 0x203,
    WINDOW_WIDGET_RECORD_TEXT_ENTRY_INSET_FIVE = 0x204,
    WINDOW_WIDGET_RECORD_LIST_BOX = 0x205,
    WINDOW_WIDGET_RECORD_TEXT_ENTRY_INSET_FOUR = 0x206
H2_ENUM_CLASS_END(WindowWidgetRecordType)

#pragma pack(push, 1)
class heroWindow {
public:
    i32 m_zOrder;
    heroWindow* m_nextWindow;
    heroWindow* m_prevWindow;
    char name[0x14];
    i32 m_winFlags;
    i32 m_winState;
    i32 m_posX;
    i32 m_posY;
    i32 m_winWidth;
    i32 m_winHeight;
    widget* m_widgetListTail;
    widget* m_widgetListHead;
    bitmap* m_savedBackground;
    heroWindow(void);
    heroWindow(i32, i32, i32, i32, i32);
    heroWindow(i32, i32, char*);
    i32 Open(i32, i32);
    void RemoveAndDeleteWidget(i32);
    void Close(void);
    void AddWidget(class widget*, i32);
    void RemoveWidget(class widget*);
    i32 BroadcastMessage(struct tag_message&);
    void DrawWindow(void);
    void DrawWindow(i32);
    void DrawWindow(i32, i32, i32);
    i32 SaveBackground(void);
    void RestoreBackground(void);
    void MoveWindow(i32, i32);
};
#pragma pack(pop)
#endif
