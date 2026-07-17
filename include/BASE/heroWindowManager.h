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

HOMM2_ENUM_VALUES_BEGIN(HeroWindowManagerConstant)
    HERO_WINDOW_NO_HOVER_WIDGET = -1
HOMM2_ENUM_VALUES_END(HeroWindowManagerConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class heroWindowManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    heroWindow* m_windowListHead; // +0x36  window-list head
    heroWindow* m_windowListTail; // +0x3a  window-list tail
    heroWindow* m_focusWindow;    // +0x3e  focus window
    heroWindow* m_activeWindow;   // +0x42  active window
    bitmap* m_screen;             // +0x46  (target screen bitmap)
    bitmap* m_fizzleSource;       // +0x4a  fizzle-source saved bitmap
    bitmap* m_fizzleWork;         // +0x4e  temporary destination used while composing a fizzle
    i32 m_screenshotIndex;        // +0x52
    i32 m_updateFlags;            // +0x56
    i32 m_dialogResult;           // +0x5a
    i32 m_lastHoverId;            // +0x5e  last hover widget/message id; -1 means none
    // --- constructors ---
    heroWindowManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    i32 ConvertToHover(struct tag_message&);
    i32 BroadcastMessage(i32, i32, i32, i32);
    void AddWindow(class heroWindow*, i32, i32);
    void RemoveWindow(class heroWindow*);
    i32 DoDialog(class heroWindow*, i32 (*)(struct tag_message&), i32);
    void UpdateScreen(void);
    void UpdateScreenRegion(i32, i32, i32, i32);
    void RedrawScreen(void);
    void FadeScreen(i32, i32, class palette*);
    void ScreenShot(void);
    void SaveFizzleSource(i32, i32, i32, i32);
    void FizzleForward(i32, i32, i32, i32, i32, i8*, i8*);
    void ReleaseFizzleSource(void);
};
#pragma pack(pop)
SIZE(heroWindowManager, 0x62);
// ---- globals (declarations, RVA order) ----
extern i32 iCombatCycleFrame;
extern b32 gbEveryOtherCycle;
extern i32 iCycle1Count;
extern i32 iCycle2Count;
extern i32 iCycle3Count;
extern i32 iDialogNestCount;
extern i8 gCyclePal[0x60]; // CodeView size 0x60; typed signed char* (array-decayed)
extern i16 memSelector;

#endif // HOMM2_BASE_HEROWINDOWMANAGER_H
