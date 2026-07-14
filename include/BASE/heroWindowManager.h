#ifndef HOMM2_BASE_HEROWINDOWMANAGER_H
#define HOMM2_BASE_HEROWINDOWMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 17 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
// forward declarations:
class heroWindow;
class palette;
class bitmap;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class heroWindowManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    heroWindow *m_windowListHead;  // +0x36  window-list head
    heroWindow *m_windowListTail;  // +0x3a  window-list tail
    heroWindow *m_focusWindow;  // +0x3e  focus window
    heroWindow *m_activeWindow;  // +0x42  active window
    bitmap *m_screen;  // +0x46  (target screen bitmap)
    bitmap *m_fizzleSource;  // +0x4a  fizzle-source saved bitmap
    bitmap *m_fizzleWork;  // +0x4e  temporary destination used while composing a fizzle
    int    m_screenshotIndex;  // +0x52
    int    m_updateFlags;  // +0x56
    int    m_dialogResult;  // +0x5a
    int    m_lastHoverId;  // +0x5e  last hover widget/message id; -1 means none
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
// ---- globals (declarations, RVA order) ----
extern int iCombatCycleFrame;
extern int gbEveryOtherCycle;
extern int iCycle1Count;
extern int iCycle2Count;
extern int iCycle3Count;
extern int iDialogNestCount;
extern signed char gCyclePal[0x60];  // CodeView size 0x60; typed signed char* (array-decayed)
extern short memSelector;

#endif // HOMM2_BASE_HEROWINDOWMANAGER_H
