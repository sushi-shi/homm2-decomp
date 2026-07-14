#ifndef HOMM2_HERO_H
#define HOMM2_HERO_H
// Declarations of the free functions DEFINED in HERO.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

typedef enum HeroUiConstant {
    HERO_UI_MESSAGE = 0x200,
    HERO_UI_WIDGET_TEXT = 3,
    HERO_UI_WIDGET_FRAME = 4,
    HERO_UI_WIDGET_ENABLE = 5,
    HERO_UI_WIDGET_DISABLE = 6,
    HERO_UI_WIDGET_ICON_FILE = 9,
    HERO_UI_STATUS_TEXT_WIDGET = 0x12f,
    HERO_UI_ARMY_ICON_FIRST = 0x57,
    HERO_UI_ARMY_PORTRAIT_FIRST = 0x5c,
    HERO_UI_ARMY_COUNT_FIRST = 0x61,
    HERO_UI_ARMY_SELECTOR_FIRST = 0x66,
    HERO_UI_ARMY_EMPTY_FRAME = 2,
    HERO_UI_WIDGET_FRAME_ACTIVE = 4,
    HERO_UI_ARTIFACT_DIALOG_ICON = 0x1c,
    HERO_UI_ARMY_SLOT_COUNT = 5
} HeroUiConstant;

void HeroMessageUpdate(char *);
void UpdateHeroScreenStatusBar(struct tag_message &);
int HeroHandler(struct tag_message &);
void RedrawHeroScreen(void);
int HeroView(int, int, int);
void SetupHeroView(void);
void DoHeroSplit(int, int);

#endif // HOMM2_HERO_H
