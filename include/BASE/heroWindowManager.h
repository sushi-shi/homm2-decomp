#ifndef HOMM2_BASE_HEROWINDOWMANAGER_H
#define HOMM2_BASE_HEROWINDOWMANAGER_H

#include <Ints.h>
#include <BASE/message.h>
#include <BASE/WINMGR.h>
#include "baseManager.h"

class heroWindow;
class palette;
class bitmap;
struct tag_message;

typedef enum HeroWindowManagerConstant {
    HERO_WINDOW_NO_HOVER_WIDGET  = -1,
    HERO_WINDOW_NO_DIALOG_RESULT = -1
} HeroWindowManagerConstant;

enum class WindowFadeMode : i32 {
    FADE_IN  = 0,
    FADE_OUT = 1
};
using enum WindowFadeMode;

#pragma pack(push, 1)
class heroWindowManager H2_FINAL : public baseManager {
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
    virtual i32 Open(i32 managerOrder) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    MessageDispatchResult ConvertToHover(struct tag_message& message);
    MessageDispatchResult BroadcastMessage(MessageType type, BaseWidgetCommand command, i32 widgetId, i32 value);
    void AddWindow(class heroWindow* window, i32 zOrder, i32 openFlags);
    void RemoveWindow(class heroWindow* window);
    i32 DoDialog(class heroWindow* window, MessageDispatchHandler handler, i32 fade);
    void UpdateScreen(void);
    void UpdateScreenRegion(i32 x, i32 y, i32 width, i32 height);
    void RedrawScreen(void);
    void FadeScreen(WindowFadeMode direction, i32 steps, class palette* currentPalette);
    void ScreenShot(void);
    void SaveFizzleSource(i32 x, i32 y, i32 width, i32 height);
    void FizzleForward(i32 x, i32 y, i32 width, i32 height, i32 delay, i8* startPalette, i8* endPalette);
    void ReleaseFizzleSource(void);
};
#pragma pack(pop)

#define FINISH_DIALOG_MESSAGE(message)                                                             \
    (gpWindowManager->m_dialogResult = (message).payload.widget.id,                                \
     (message).payload.widget.id = H2EnumIndex(WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

#define UPDATE_INCLUSIVE_REGION(left, top, right, bottom)                                          \
    (gpWindowManager->UpdateScreenRegion((left), (top), (right) - (left) + 1, (bottom) - (top) + 1))
extern i32 iCombatCycleFrame;
extern u8 gbEveryOtherCycle;
extern i32 iCycle1Count;
extern i32 iCycle2Count;
extern i32 iCycle3Count;
extern i32 iDialogNestCount;
extern i8 gCyclePal[WINDOW_CYCLE_PALETTE_BYTES];
extern i16 memSelector;

#endif
