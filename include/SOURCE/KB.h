#ifndef HOMM2_KB_H
#define HOMM2_KB_H
// Declarations of the free functions DEFINED in KB.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>
#include <_types.h>
#include <_carcass_types.h>

// KB-private record structs (shared ones — tag_message/SAMPLE2/tag_monsterInfo/SSpellInfo/
// SWinSetup/SNetPlayerInfo — live in _types.h / _carcass_types.h).
struct townSlot { char m_pad[0x64]; };
struct heroRec  { char m_pad[250]; };
struct SPlayerExit { signed char m_0, m_1, m_2, m_3, m_4, m_5, m_6; };
#pragma pack(push, 1)
struct playerRec { char m_pad0[0x8f]; int resources[7]; char m_pad1[0x70]; };
#pragma pack(pop)

extern "C" void PollSound(void);
void ForcePollSound(void);
void InitMainClasses(void);
void DeleteMainClasses(void);
void EarlyShutdown(char *caption, char *text);
void SetupCDRom(void);
int EarlySetup(void);
int oldmain(void);
char toupper(char c);
int InterpretCommandLine(void);
int InitMenuHandler(struct tag_message &);
int NullHandler(struct tag_message &msg);
int RecruitHeroHandler(tag_message &msg);
char *GetBuildingInfo(int race, int building, int mode);
char *GetBuildingName(int race, int building);
void GetBuildingCost(int race, int building, int *const dest, int mageLevel);
char *GetMonsterName(int m);
void GetMonsterCost(int monster, int *const cost);
int CanBuild(town *t, int building);
int CanBuy(town *t, int type);
int GetBuildingBaseResourceValue(int race, int building, int level);
int WaitHandler(tag_message &msg);
int EventWindowHandler(struct tag_message &);
int TrueFalseDialogHandler(struct tag_message &msg);
void PlayerDead(int player);
void CheckEndGame(int, int);
void QuickViewWait(void);
void InitVars(void);
void ClearMapExtra(void);
int GetMonType(int score, int campaign);
int AddScoreToHighScore(int, int, int, int, char *);
void BVResMsg(char *s, int res, int qty);
void GOut(char *str);
int NetPosToGamePos(int netPos);
int WaitForOtherPlayer(void);
void PopNetBox(char *, int);
void AddNetBoxLine(char *str, char color);
void ShutDown(char *msg);
void FileError(char *filename);
void SmackFade(unsigned char *src, unsigned char *dst);
void ShowCongrats(int);
void CongratsWait(void);
SAMPLE2 LoadPlaySample(char *name);
void WaitEndSample(SAMPLE2 s, int waitTime);
void MemError(void);
char *GetTownName(int i);
void LoadSystemwideIcons(void);
void UnloadSystemwideIcons(void);
void EarlyShutDownSystem(void);
int GameUnsaved(void);
int HandleAppSpecificMenuCommands(int);
void UpdateSystemOptionsMenu(void);
void CleanUpMenus(void);
void UpdateAppSpecificMenus(void *hMenu);
void EarlyResizeWindow(int x, int y, int w, int h);
int InMapArea(int x, int y);
void SetupDynamicWindow(int, int, int, int, int, int, int, int *, int *, int *, int *, int *, int *, class heroWindow * *, int);
void TestDynamicWindow(int p1, int p2);
void HandleRemoteDeadPlayerExit(int pos);
void HandleRemoteSuddenExit(void);
void DropDownToOnePlayer(void);
void ReceiveHostReportsPlayerExit(int, struct SPlayerExit, int);
void ReceiveRemotePlayerExit(struct SPlayerExit);
int CheckMem(void);
int GetManaCost(int spell, hero *h);
void SetWinText(heroWindow *j, int id);
void CheckShingleUpdate(void);
void NormalDialog(char *, int, int, int, int, int, int, int, int, int);
void UpdateNormalDialog(char *text);

#endif // HOMM2_KB_H
