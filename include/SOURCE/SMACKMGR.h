#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H
#include <va.h>
#include <smack.h>
// Declarations of the free functions DEFINED in SMACKMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void ConvertSmackerPalette(unsigned char *);
class icon;

typedef enum SmackManagerConstant {
    SMACK_SCREEN_WIDTH = 640,
    SMACK_SCREEN_HEIGHT = 480,
    SMACK_PALETTE_SIZE = 768,
    SMACK_AUDIO_OPEN_FLAGS = 0xfe000,
    SMACK_PRELOAD_OPEN_FLAG = 0x200,
    SMACK_NORMAL_FADE = 0x80,
    SMACK_SHORT_FADE = 8,
    SMACK_FAST_FADE = 4,
    SMACK_DIALOG_RETRY = 0x7806,
    SMACK_EVENT_KEY = 1,
    SMACK_EVENT_MOUSE_MOVE = 4,
    SMACK_EVENT_MOUSE_CLICK = 8,
    SMACK_EVENT_QUIT = 0x20,
    SMACK_ALT_KEY = 0x3e,
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
} SmackManagerConstant;

#pragma pack(push, 1)
struct tag_rect {
    short x;
    short y;
    short width;
    short height;
};

struct SSmackOptions {
    char fileName[9];
    char companionFileName[9];
    char slowFileName[9];
    char slowCompanionFileName[9];
    signed char fadeIn;
    signed char fadeOut;
    signed char preload;
    signed char waitForInput;
    signed char drawCompanion;
    short companionX;
    short companionY;
};

struct SmackMilesDigitalDriver {
    char reserved[0x4c];
    void *directSound;
};
#pragma pack(pop)
SIZE(tag_rect, 8);
SIZE(SSmackOptions, 45);
SIZE(SmackMilesDigitalDriver, 0x50);

extern "C" __declspec(dllimport) int __stdcall AIL_get_preference(unsigned int);

void DoAdvance(Smack *, int, int, int, int);
void SmackManagerMain(void);
void ShutDownSmacker(void);
int PlaySmacker(int);
int ExpansionCampaignRect(int, int);
signed char PointInRect(int, int, struct tag_rect *);
void PrintSummaryInfo(SmackSum *);

// ---- globals (declarations, RVA order) ----
extern int bSmackSound;
extern class icon *brotherIcon;
extern class icon *backImage;
extern struct SSmackOptions SmackOptions[];
extern int bTesting;
extern Smack *smk1;
extern Smack *smk2;
extern signed char bSmackNum;
extern int gbLastFramePlayed;
extern SmackSum smksum;
extern int gbPlayedThrough;
extern signed char bMainDone;

#endif // HOMM2_SMACKMGR_H
