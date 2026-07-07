#ifndef HOMM2_GAME_H
#define HOMM2_GAME_H
// Declarations of the free functions DEFINED in GAME.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct SCreatureInfo;
struct configStruct;
struct tag_message;

// GAME-private types (configStruct / SCreatureInfo are shared, in _types.h).
struct EventExtra;
struct SThievesData { char pad[0x4e3]; signed char list[8][283]; };

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

#endif // HOMM2_GAME_H
