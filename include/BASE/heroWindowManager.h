#ifndef HOMM2_BASE_HEROWINDOWMANAGER_H
#define HOMM2_BASE_HEROWINDOWMANAGER_H

#include <va.h>
#include <BASE/message.h>
#include <BASE/WINMGR.h>
#include "baseManager.h"

class heroWindow;
class palette;
class bitmap;
struct tag_message;

H2_ENUM_BEGIN(HeroWindowManagerConstant)
    HERO_WINDOW_NO_HOVER_WIDGET  = -1,
    HERO_WINDOW_NO_DIALOG_RESULT = -1
H2_ENUM_END(HeroWindowManagerConstant)

H2_ENUM_CLASS_BEGIN(WindowFadeMode)
    FADE_IN  = 0,
    FADE_OUT = 1
H2_ENUM_CLASS_END(WindowFadeMode)

#pragma pack(push, 1)
class heroWindowManager : public baseManager {
public:
    heroWindow* m_windowListHead;
    heroWindow* m_windowListTail;
    heroWindow* m_focusWindow;
    heroWindow* m_activeWindow;
    bitmap* m_screen;
    bitmap* m_fizzleSource;
    bitmap* m_fizzleWork;
    i32 m_screenshotIndex;
    i32 m_updateFlags;
    i32 m_dialogResult;
    i32 m_lastHoverId;
    heroWindowManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    i32 ConvertToHover(struct tag_message&);
    i32 BroadcastMessage(MessageType, BaseWidgetCommand, i32, i32);
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
extern i32 iCombatCycleFrame;
extern b32 gbEveryOtherCycle;
extern i32 iCycle1Count;
extern i32 iCycle2Count;
extern i32 iCycle3Count;
extern i32 iDialogNestCount;
extern i8 gCyclePal[WINDOW_CYCLE_PALETTE_BYTES];
extern i16 memSelector;

#endif
