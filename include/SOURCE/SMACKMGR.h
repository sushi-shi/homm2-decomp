#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H

#include <va.h>
#include <smack.h>
#include <mss.h>

void ConvertSmackerPalette(u8*);
class icon;

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
    i8 fadeIn;
    i8 fadeOut;
    i8 preload;
    i8 waitForInput;
    i8 drawCompanion;
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

void DoAdvance(Smack*, i32, i32, i32, i32);
void SmackManagerMain(void);
void ShutDownSmacker(void);
i32 PlaySmacker(i32);
i32 ExpansionCampaignRect(i32, i32);
i8 PointInRect(i32, i32, struct tag_rect*);
void PrintSummaryInfo(SmackSum*);

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

#endif
