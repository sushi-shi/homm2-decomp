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

typedef enum GameDifficultyConstant {
    GAME_DIFFICULTY_HARD = 2,
    GAME_DIFFICULTY_EXPERT = 3,
    GAME_DIFFICULTY_IMPOSSIBLE = 4
} GameDifficultyConstant;

typedef enum GameDailyArtifactConstant {
    ARTIFACT_POWER_RING = 0x43,
    ARTIFACT_ENDLESS_POUCH_SULFUR = 0x47,
    ARTIFACT_ENDLESS_VIAL_MERCURY = 0x48,
    ARTIFACT_ENDLESS_POUCH_GEMS = 0x49,
    ARTIFACT_ENDLESS_CORD_WOOD = 0x4a,
    ARTIFACT_ENDLESS_CART_ORE = 0x4b,
    ARTIFACT_ENDLESS_POUCH_CRYSTAL = 0x4c
} GameDailyArtifactConstant;

typedef enum GameWeeklyConstant {
    WEEKLY_FIRST_DWELLING = 19,
    WEEKLY_LAST_DWELLING = 30,
    WEEKLY_GROWTH_LIMIT = 0x1feb,
    WEEKLY_DRAGON_CITY_LIMIT = 220,
    WEEKLY_MONSTER_LIMIT = 4000,
    WEEKLY_AVAILABLE_HERO = 64,
    WEEKLY_HERO_RESERVED_FLAG = 0x10000,
    WEEKLY_HERO_VISIT_FLAG = 0x800000
} GameWeeklyConstant;

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
