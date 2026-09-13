#ifndef HOMM2_SMACKMGR_H
#define HOMM2_SMACKMGR_H

#include <Ints.h>
#include <smack.h>
#include <mss.h>

void ConvertSmackerPalette(u8* paletteData);
class icon;
enum class ExpansionCampaignId : i32;

typedef enum SmackManagerStorageConstant {
    SMACK_OPTION_FILENAME_SIZE = 9,
    SMACK_MILES_RESERVED_SIZE  = 0x4c
} SmackManagerStorageConstant;

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
