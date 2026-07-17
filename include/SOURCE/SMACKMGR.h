#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H
#include <va.h>
#include <smack.h>
#include <mss.h>
// Declarations of the free functions DEFINED in SMACKMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void ConvertSmackerPalette(u8*);
class icon;

HOMM2_ENUM_VALUES_BEGIN(SmackManagerConstant)
    SMACK_SCREEN_WIDTH = 640,
    SMACK_SCREEN_HEIGHT = 480,
    SMACK_PALETTE_SIZE = 768,
    SMACK_AUDIO_OPEN_FLAGS = 0xfe000,
    SMACK_PRELOAD_OPEN_FLAG = 0x200,
    SMACK_NORMAL_FADE = 0x80,
    SMACK_SHORT_FADE = 8,
    SMACK_FAST_FADE = 4,
    SMACK_DIALOG_RETRY = 0x7806,
    SMACK_CHOOSE_CAMPAIGN = 35,
    SMACK_CREDITS = 36,
    SMACK_EARTH = 37,
    SMACK_FIRST_NETWORK = 39,
    SMACK_EXPANSION_CAMPAIGN = 67,
    SMACK_EXPANSION_FIRST_MOVIE = 68,
    SMACK_SPECIAL_MUSIC = 72,
    SMACK_CONGRATS = 2,
    SMACK_INTRO_MUSIC = 3,
    SMACK_LOW_MEMORY_MOVIE = 30,
    SMACK_BACKGROUND_COLOR = 36,
    SMACK_MAIN_MUSIC = 42,
    SMACK_LOSE_MUSIC = 43,
    SMACK_INTRO_SECOND_MUSIC = 19,
    SMACK_POINTER_ID = 40,
    SMACK_POINTER_DEFAULT = 0,
    SMACK_POINTER_HIDDEN_HOTSPOT = -999,
    SMACK_EXPANSION_RECT_COUNT = 4
HOMM2_ENUM_VALUES_END(SmackManagerConstant)

#pragma pack(push, 1)
struct tag_rect {
    i16 x;
    i16 y;
    i16 width;
    i16 height;
};

struct SSmackOptions {
    char fileName[9];
    char companionFileName[9];
    char slowFileName[9];
    char slowCompanionFileName[9];
    i8 fadeIn;
    i8 fadeOut;
    i8 preload;
    i8 waitForInput;
    i8 drawCompanion;
    i16 companionX;
    i16 companionY;
};

struct SmackMilesDigitalDriver {
    char reserved[0x4c];
    void* directSound;
};
#pragma pack(pop)
SIZE(tag_rect, 8);
SIZE(SSmackOptions, 45);
SIZE(SmackMilesDigitalDriver, 0x50);
SIZE(SmackSum, 0x54);

void DoAdvance(Smack*, i32, i32, i32, i32);
void SmackManagerMain(void);
void ShutDownSmacker(void);
i32 PlaySmacker(i32);
i32 ExpansionCampaignRect(i32, i32);
i8 PointInRect(i32, i32, struct tag_rect*);
void PrintSummaryInfo(SmackSum*);

// ---- globals (declarations, RVA order) ----
extern i32 bSmackSound;
extern class icon* brotherIcon;
extern class icon* backImage;
extern struct SSmackOptions SmackOptions[];
extern i32 bTesting;
extern Smack* smk1;
extern Smack* smk2;
extern i8 bSmackNum;
extern b32 gbLastFramePlayed;
extern SmackSum smksum;
extern b32 gbPlayedThrough;
extern i8 bMainDone;

#endif // HOMM2_SMACKMGR_H
