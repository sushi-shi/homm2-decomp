#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H

#include <va.h>
#include <smack.h>
#include <mss.h>

void ConvertSmackerPalette(u8* paletteData);
class icon;
H2_ENUM_CLASS_FORWARD(ExpansionCampaignId);

// PlaySmacker's smackNumber: a SmackOptions row (bSmackNum); role names share a row.
H2_ENUM_BEGIN(SmackVideo)
    OLD_MAIN_INTRO_FALLBACK_VIDEO    = 1,
    CONGRATS                         = 2,
    DEFEAT_VIDEO                     = 3,
    CAMPAIGN_INTRO                   = 4,
    SMACKER_ROLAND_INTRO             = 5,
    SMACKER_ROLAND_1                 = 6,
    SMACKER_ROLAND_2                 = 7,
    SMACKER_ROLAND_3A                = 8,
    SMACKER_ROLAND_3B                = 9,
    SMACKER_ROLAND_4                 = 10,
    SMACKER_ROLAND_5A                = 11,
    SMACKER_ROLAND_5B                = 12,
    SMACKER_ROLAND_6                 = 13,
    SMACKER_ROLAND_7                 = 14,
    SMACKER_ROLAND_8                 = 15,
    SMACKER_ROLAND_9                 = 16,
    SMACKER_ROLAND_END               = 18,
    SMACKER_ARCHIBALD_INTRO          = 19,
    SMACKER_ARCHIBALD_1              = 20,
    SMACKER_ARCHIBALD_2              = 21,
    SMACKER_ARCHIBALD_3              = 22,
    SMACKER_ARCHIBALD_4A             = 23,
    SMACKER_ARCHIBALD_4B             = 24,
    SMACKER_ARCHIBALD_4_END          = 25,
    SMACKER_ARCHIBALD_5A             = 26,
    SMACKER_ARCHIBALD_5B             = 27,
    SMACKER_ARCHIBALD_6              = 28,
    SMACKER_ARCHIBALD_7A             = 29,
    SMACKER_ARCHIBALD_7B             = 30,
    SMACKER_ARCHIBALD_8              = 31,
    SMACKER_ARCHIBALD_9              = 32,
    SMACKER_ARCHIBALD_10             = 33,
    SMACKER_ARCHIBALD_END            = 34,
    CHOOSE_CAMPAIGN                  = 35,
    SMACK_CREDITS                    = 36,
    OLD_MAIN_CREDITS_SECOND_VIDEO    = SMACK_CREDITS,
    SMACK_EARTH                      = 37,
    MENU_MOVIE_SMACKER               = 38,
    SMACKER_POL_INTRO                = 0x27,
    FIRST_NETWORK                    = SMACKER_POL_INTRO,
    SMACKER_POL_UPRISING             = 0x28,
    SMACKER_POL_ISLAND_OF_CHAOS      = 0x29,
    SMACKER_POL_ARROWS_FLIGHT        = 0x2a,
    SMACKER_POL_BRANCH_REUNITED      = 0x2b,
    SMACKER_POL_AURORA_BOREALIS      = 0x2c,
    SMACKER_POL_BETRAYALS_END        = 0x2d,
    SMACKER_POL_CORRUPTIONS_HEART    = 0x2e,
    SMACKER_DES_INTRO                = 0x2f,
    SMACKER_DES_CONQUER_AND_UNIFY    = 0x30,
    SMACKER_DES_BORDER_TOWNS         = 0x31,
    SMACKER_DES_FAMILY_REUNITED      = 0x32,
    SMACKER_DES_SOUTHERN_WAR         = 0x33,
    SMACKER_DES_BRANCH_REUNITED      = 0x34,
    SMACKER_DES_EPIC_BATTLE          = 0x35,
    SMACKER_WIZ_INTRO                = 0x36,
    SMACKER_WIZ_SHROUDED_ISLES       = 0x37,
    SMACKER_WIZ_ETERNAL_SCROLLS      = 0x38,
    SMACKER_WIZ_POWERS_END           = 0x39,
    SMACKER_WIZ_FOUNT_OF_WIZARDRY    = 0x3a,
    SMACKER_VOY_INTRO                = 0x3b,
    SMACKER_VOY_STRANDED             = 0x3c,
    SMACKER_VOY_PIRATE_ISLES         = 0x3d,
    SMACKER_VOY_KING_AND_COUNTRY     = 0x3e,
    SMACKER_VOY_BLOOD_IS_THICKER     = 0x3f,
    EXPANSION_DEFEAT_VIDEO           = 64,
    OLD_MAIN_INTRO_SECONDARY_VIDEO   = 65,
    OLD_MAIN_INTRO_PRIMARY_VIDEO     = 66,
    EXPANSION_CAMPAIGN               = 67,
    SMACKER_CAMPAIGN_CHOICE          = EXPANSION_CAMPAIGN,
    EXPANSION_FIRST_MOVIE            = 68,
    SPECIAL_MUSIC                    = 72,
    OLD_MAIN_CREDITS_FIRST_VIDEO     = SPECIAL_MUSIC,
    BUKA_LOGO                        = 73,
    OLD_MAIN_INTRO_PUBLISHER_VIDEO   = BUKA_LOGO,
    BUKA_CREDITS                     = 74,
    OLD_MAIN_CREDITS_THIRD_VIDEO     = BUKA_CREDITS,
    SMACK_OPTION_COUNT               = 75
H2_ENUM_END(SmackVideo)

H2_ENUM_BEGIN(SmackManagerStorageConstant)
    SMACK_OPTION_FILENAME_SIZE = 9,
    SMACK_MILES_RESERVED_SIZE  = 0x4c
H2_ENUM_END(SmackManagerStorageConstant)

#pragma pack(push, 1)
struct tag_rect {
    i16 x;
    i16 y;
    i16 width;
    i16 height;
};

struct SSmackOptions {
    char fileName[SMACK_OPTION_FILENAME_SIZE];
    char companionFileName[SMACK_OPTION_FILENAME_SIZE];
    char slowFileName[SMACK_OPTION_FILENAME_SIZE];
    char slowCompanionFileName[SMACK_OPTION_FILENAME_SIZE];
    b8 fadeIn;
    b8 fadeOut;
    b8 preload;
    b8 waitForInput;
    b8 drawCompanion;
    i16 companionX;
    i16 companionY;
};

struct SmackMilesDigitalDriver {
    char reserved[SMACK_MILES_RESERVED_SIZE];
    void* directSound;
};
#pragma pack(pop)
SIZE(tag_rect, 8);
SIZE(SSmackOptions, 45);
SIZE(SmackMilesDigitalDriver, 0x50);
SIZE(SmackSum, 0x54);

void DoAdvance(Smack* smack, i32 drawFrame, i32 advanceFrame, i32 updatePalette, i32 skipPalette);
void SmackManagerMain(void);
void ShutDownSmacker(void);
i32 PlaySmacker(i32 smackNumber);
ExpansionCampaignId ExpansionCampaignRect(i32 x, i32 y);
i8 PointInRect(i32 x, i32 y, struct tag_rect* rect);
void PrintSummaryInfo(SmackSum* summary);

extern b32 bSmackSound;
extern class icon* brotherIcon;
extern class icon* backImage;
extern struct SSmackOptions SmackOptions[];
extern i32 smackMasterVolumes[];
extern i32 bTesting;
extern Smack* smk1;
extern Smack* smk2;
extern i8 bSmackNum;
extern b32 gbLastFramePlayed;
extern SmackSum smksum;
extern b32 gbPlayedThrough;
extern b8 bMainDone;

#endif
