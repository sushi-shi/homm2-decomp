#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 15 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class widget;
class bitmap;
struct tag_message;

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

#pragma pack(push, 1)  // recovered layout is byte-packed
class heroWindow {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    m_zOrder;  // +0x00
    heroWindow *m_nextWindow;  // +0x04  next window in the manager list
    heroWindow *m_prevWindow;  // +0x08  prev window in the manager list
    char   name[0x14];  // +0x0c  window name (strcpy'd, e.g. "Default Construct")
    int    m_winFlags;  // +0x20  !union: conflicting widths
    int    m_winState;  // +0x24  !union: conflicting widths
    int    m_posX;  // +0x28
    int    m_posY;  // +0x2c
    int    m_winWidth;  // +0x30
    int    m_winHeight;  // +0x34
    widget *m_widgetListTail;  // +0x38  widget-list tail
    widget *m_widgetListHead;  // +0x3c  head of the widget list
    bitmap *m_savedBackground;  // +0x40  saved-background bitmap
    // size >= 0x44 (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    heroWindow(void);
    heroWindow(int, int, int, int, int);
    heroWindow(int, int, char *);
    // --- methods ---
    int Open(int, int);
    void RemoveAndDeleteWidget(int);
    void Close(void);
    void AddWidget(class widget *, int);
    void RemoveWidget(class widget *);
    int BroadcastMessage(struct tag_message &);
    void DrawWindow(void);
    void DrawWindow(int);
    void DrawWindow(int, int, int);
    int SaveBackground(void);
    void RestoreBackground(void);
    void MoveWindow(int, int);
};
#pragma pack(pop)
#endif // HOMM2_BASE_HEROWINDOW_H
