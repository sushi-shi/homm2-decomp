#ifndef HOMM2_BASE_HEROWINDOW_H
#define HOMM2_BASE_HEROWINDOW_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 15 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class widget;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class heroWindow {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    int    field_0x0;  // +0x00
    int    field_0x4;  // +0x04
    int    field_0x8;  // +0x08
    char _pad_0xc[0x14];
    int    field_0x20;  // +0x20  !union: conflicting widths
    int    field_0x24;  // +0x24  !union: conflicting widths
    int    field_0x28;  // +0x28
    int    field_0x2c;  // +0x2c
    int    field_0x30;  // +0x30
    int    field_0x34;  // +0x34
    int    field_0x38;  // +0x38
    int    field_0x3c;  // +0x3c
    int    field_0x40;  // +0x40
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
