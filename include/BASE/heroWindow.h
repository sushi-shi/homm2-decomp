#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 15 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class widget;
class bitmap;
struct tag_message;

typedef enum WindowFlag {
    WINDOW_FLAG_OWNS_WIDGETS = 0x4000,    // Close/RemoveWidgetById delete the widgets
    WINDOW_UPDATE_SUPPRESS_MASK = 0x7fff, // DrawWindow skips the screen update when == 1
    WINDOW_ALL_WIDGETS_LOW = -0xffff,     // DrawWindow id-range sentinel: no filter
    WINDOW_ALL_WIDGETS_HIGH = 0xffff,
    WINDOW_DRAW_ID_LIMIT = 0x7fff
} WindowFlag;

typedef enum WindowWidgetRecordType {
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
} WindowWidgetRecordType;

#pragma pack(push, 1) // recovered layout is byte-packed
class heroWindow {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    i32 m_zOrder;              // +0x00
    heroWindow* m_nextWindow;  // +0x04  next window in the manager list
    heroWindow* m_prevWindow;  // +0x08  prev window in the manager list
    char name[0x14];           // +0x0c  window name (strcpy'd, e.g. "Default Construct")
    i32 m_winFlags;            // +0x20  !union: conflicting widths
    i32 m_winState;            // +0x24  !union: conflicting widths
    i32 m_posX;                // +0x28
    i32 m_posY;                // +0x2c
    i32 m_winWidth;            // +0x30
    i32 m_winHeight;           // +0x34
    widget* m_widgetListTail;  // +0x38  widget-list tail
    widget* m_widgetListHead;  // +0x3c  head of the widget list
    bitmap* m_savedBackground; // +0x40  saved-background bitmap
    // size >= 0x44 (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    heroWindow(void);
    heroWindow(i32, i32, i32, i32, i32);
    heroWindow(i32, i32, char*);
    // --- methods ---
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
#endif // HOMM2_BASE_HEROWINDOW_H
