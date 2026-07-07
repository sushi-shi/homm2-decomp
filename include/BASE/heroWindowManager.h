#ifndef HOMM2_BASE_HEROWINDOWMANAGER_H
#define HOMM2_BASE_HEROWINDOWMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 17 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
// forward declarations:
class heroWindow;
class palette;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class heroWindowManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    int    field_0x42;  // +0x42
    int    field_0x46;  // +0x46
    int    field_0x4a;  // +0x4a
    int    field_0x4e;  // +0x4e
    int    field_0x52;  // +0x52
    int    field_0x56;  // +0x56
    int    field_0x5a;  // +0x5a
    int    field_0x5e;  // +0x5e
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
#pragma pack(pop)
SIZE(heroWindowManager, 0x62);
#endif // HOMM2_BASE_HEROWINDOWMANAGER_H
