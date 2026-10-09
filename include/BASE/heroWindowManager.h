#ifndef HOMM2_BASE_HEROWINDOWMANAGER_H
#define HOMM2_BASE_HEROWINDOWMANAGER_H

#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/message.h>
#include <BASE/baseManager.h>

enum {
    WINDOW_COLOR_CYCLE_DEFAULT          = 0,
    WINDOW_COLOR_CYCLE_COMBAT           = 1,
    WINDOW_COLOR_CYCLE_WORLD_VIEW       = 2,
    WINDOW_COLOR_CYCLE_COMBAT_ALTERNATE = 3
};
typedef i32 WindowColorCycleMode;
typedef enum WindowManagerConstant {
    WINDOW_CYCLE_PALETTE_BYTES = 0x60
} WindowManagerConstant;

class heroWindow;
class palette;
class bitmap;
struct tag_message;

typedef enum HeroWindowManagerConstant {
    HERO_WINDOW_NO_HOVER_WIDGET  = -1,
    HERO_WINDOW_NO_DIALOG_RESULT = -1
} HeroWindowManagerConstant;

enum {
    FADE_IN  = 0,
    FADE_OUT = 1
};
typedef i32 WindowFadeMode;
typedef enum WindowFadeSpeed {
    FADE_SPEED_INSTANT  = 0x80,
    FADE_SPEED_STANDARD = 8,
    FADE_SPEED_FINE     = 6,
    FADE_SPEED_FAST     = 4
} WindowFadeSpeed;


typedef enum WindowFizzleDelay {
    FIZZLE_DEFAULT_DELAY = 150
} WindowFizzleDelay;

#pragma pack(push, 1)
class heroWindowManager H2_FINAL : public baseManager {
public:
    heroWindow* m_windowListHead;
    heroWindow* m_windowListTail;
    heroWindow* m_focusWindow;
    heroWindow* m_previousFocusWindow;
    bitmap* m_screen;
    bitmap* m_fizzleSource;
    bitmap* m_fizzleWork;
    i32 m_screenshotIndex;
    i32 m_colorCycling;
    i32 m_dialogResult;
    i32 m_lastHoverId;
    heroWindowManager(void);
    virtual i32 Open(i32 managerOrder) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    MessageDispatchResult ConvertToHover(struct tag_message& message);
    MessageDispatchResult BroadcastMessage(MessageType type, BaseWidgetCommand command, i32 widgetId, i32 value);
    void AddWindow(class heroWindow* window, i32 zOrder, i32 updateScreen);
    void RemoveWindow(class heroWindow* window);
    i32 DoDialog(class heroWindow* window, MessageDispatchHandler handler, b32 fade);
    void UpdateScreen(void);
    void UpdateScreenRegion(i32 x, i32 y, i32 width, i32 height);
    void RedrawScreen(void);
    void FadeScreen(WindowFadeMode direction, i32 increment, class palette* currentPalette);
    void ScreenShot(void);
    void SaveFizzleSource(i32 x, i32 y, i32 width, i32 height);
    void FizzleForward(i32 x, i32 y, i32 width, i32 height, i32 delay, i8* startPalette, i8* endPalette);
    void ReleaseFizzleSource(void);
};
#pragma pack(pop)

#define FINISH_DIALOG_MESSAGE(message)                                                             \
    (gpWindowManager->m_dialogResult = (message).payload.widget.id,                                \
     (message).payload.widget.id = (WIDGET_COMMAND_DIALOG_SELECT),                              \
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

void CycleColors(b32 forceUpdate);
void CreateFizzleTables(void);
void CreateColorTables(void);
void CreateColorLookupTables(void);

#endif
