#ifndef HOMM2_GAME_H
#define HOMM2_GAME_H
#include <va.h>
#include <_types.h>
// Declarations of the free functions DEFINED in GAME.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

// GAME-private types (configStruct / SCreatureInfo are shared, in _types.h).
struct EventExtra;
struct SThievesData { char pad[0x4e3]; signed char list[8][283]; };

typedef enum GameSerializationConstant {
    GAME_SOURCE_LINE = 660,
    GAME_LOAD_SOURCE_LINE = 1103,
    GAME_SAVE_BUFFER_SIZE = 50000,
    GAME_FILE_MARKER = 1234,
    GAME_UNUSED_FILE_MARKER = 9999,
    GAME_PLAYER_COUNT = 6,
    GAME_HERO_COUNT = 54,
    GAME_TOWN_COUNT = 72,
    GAME_MINE_COUNT = 144,
    GAME_BOAT_COUNT = 48
} GameSerializationConstant;

int GetNumObelisks(int color);
void ComputeUALoc(int);
void GenerateStandardFileName(char *, char *);
int ViewSpellsHandler(struct tag_message &);
int ViewSpecialHandler(struct tag_message &);
int ViewArmyHandler(struct tag_message &);
int IsCursedItem(int item);
int CalcBaseScore(int);
void WriteDiffHeaderInfo(unsigned char cmd, int len, unsigned char *buf, int *pos);
int GetSkipCopyLen(unsigned char *buf, int *pos);
void CreateDiffFile(char *, char *, char *, int, int);
void CreateJoinFile(char *, char *, char *);
EventExtra *GetMapEvent(int x, int y);
void CheckValidAvailableHeroes(void);
int CalcFileCRC(char *filename);
void CompressTest2(void);
void CompressTest(void);
void CompressTest3(void);


// --- globals owned by this TU (moved from _globals.h; CodeView-attributed) ---
extern char bMapInitialized;

#endif // HOMM2_GAME_H
