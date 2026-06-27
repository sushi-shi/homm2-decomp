#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 15 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class widget;
struct tag_message;

class heroWindow {
public:
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
