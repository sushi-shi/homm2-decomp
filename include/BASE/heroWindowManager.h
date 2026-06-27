#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 17 methods, 3 own-virtual, 0 static data.
#include "../_macros.h"
#include "baseManager.h"
// forward declarations:
class heroWindow;
class palette;
struct tag_message;

class heroWindowManager : public baseManager {
public:
    // --- constructors ---
    heroWindowManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    int ConvertToHover(struct tag_message &);
    int BroadcastMessage(int, int, int, int);
    void AddWindow(class heroWindow *, int, int);
    void RemoveWindow(class heroWindow *);
    int DoDialog(class heroWindow *, int (*)(struct tag_message &), int);
    void UpdateScreen(void);
    void UpdateScreenRegion(int, int, int, int);
    void RedrawScreen(void);
    void FadeScreen(int, int, class palette *);
    void ScreenShot(void);
    void SaveFizzleSource(int, int, int, int);
    void FizzleForward(int, int, int, int, int, signed char *, signed char *);
    void ReleaseFizzleSource(void);
};
