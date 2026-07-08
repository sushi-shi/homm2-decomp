#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 15 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class widget;
class bitmap;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class heroWindow {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    zOrder;  // +0x00
    heroWindow *nextWindow;  // +0x04  next window in the manager list
    heroWindow *prevWindow;  // +0x08  prev window in the manager list
    char   name[0x14];  // +0x0c  window name (strcpy'd, e.g. "Default Construct")
    int    winFlags;  // +0x20  !union: conflicting widths
    int    winState;  // +0x24  !union: conflicting widths
    int    posX;  // +0x28
    int    posY;  // +0x2c
    int    winWidth;  // +0x30
    int    winHeight;  // +0x34
    widget *widgetListTail;  // +0x38  widget-list tail
    widget *widgetListHead;  // +0x3c  head of the widget list
    bitmap *savedBackground;  // +0x40  saved-background bitmap
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
