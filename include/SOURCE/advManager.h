#ifndef HOMM2_SOURCE_ADVMANAGER_H
#define HOMM2_SOURCE_ADVMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 139 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class armyGroup;
class hero;
class mapCell;
class fullMap;
class sample;
class town;
class heroWindow;
class icon;
struct SAMPLE2;
struct SMapChange;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class advManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    int    field_0x3e;  // +0x3e
    int    field_0x42;  // +0x42
    int    field_0x46;  // +0x46
    int    field_0x4a;  // +0x4a
    char _pad_0x4e[0x1c];
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    char _pad_0x72[0x28];
    class heroWindow *adventureWindow;  // +0x9a
    int    field_0x9e;  // +0x9e
    int    field_0xa2;  // +0xa2
    int    field_0xa6;  // +0xa6
    char _pad_0xaa[0x4];
    class fullMap *mapData;  // +0xae
    int    field_0xb2;  // +0xb2
    int    field_0xb6;  // +0xb6
    unsigned char *adventureBorder;  // +0xba
    char _pad_0xbe[0x4];
    int    field_0xc2;  // +0xc2
    int    field_0xc6;  // +0xc6
    int    field_0xca;  // +0xca
    char _pad_0xce[0x28];
    int    field_0xf6;  // +0xf6
    char _pad_0xfa[0x18];
    class icon *puzzleIcon;  // +0x112
    char _pad_0x116[0x8];
    int    field_0x11e;  // +0x11e
    char _pad_0x122[0x48];
    int    field_0x16a;  // +0x16a
    char _pad_0x16e[0x60];
    int    field_0x1ce;  // +0x1ce
    int    field_0x1d2;  // +0x1d2
    int    mapOriginX;  // +0x1d6
    int    mapOriginY;  // +0x1da
    int    field_0x1de;  // +0x1de
    int    field_0x1e2;  // +0x1e2
    int    lastHoverCell;  // +0x1e6
    int    field_0x1ea;  // +0x1ea
    int    field_0x1ee;  // +0x1ee
    int    field_0x1f2;  // +0x1f2
    int    field_0x1f6;  // +0x1f6
    int    field_0x1fa;  // +0x1fa
    int    field_0x1fe;  // +0x1fe
    int    field_0x202;  // +0x202
    int    field_0x206;  // +0x206
    int    field_0x20a;  // +0x20a
    int    field_0x20e;  // +0x20e
    int    field_0x212;  // +0x212
    int    field_0x216;  // +0x216
    int    field_0x21a;  // +0x21a
    int    field_0x21e;  // +0x21e
    int    field_0x222;  // +0x222
    int    field_0x226;  // +0x226
    int    field_0x22a;  // +0x22a
    int    field_0x22e;  // +0x22e
    int    field_0x232;  // +0x232
    int    field_0x236;  // +0x236
    int    field_0x23a;  // +0x23a
    int    field_0x23e;  // +0x23e
    int    field_0x242;  // +0x242
    int    field_0x246;  // +0x246
    int    field_0x24a;  // +0x24a
    int    field_0x24e;  // +0x24e
    int    field_0x252;  // +0x252
    int    field_0x256;  // +0x256
    int    field_0x25a;  // +0x25a
    int    field_0x25e;  // +0x25e
    int    field_0x262;  // +0x262
    int    field_0x266;  // +0x266
    int    field_0x26a;  // +0x26a
    int    field_0x26e;  // +0x26e
    int    field_0x272;  // +0x272
    int    field_0x276;  // +0x276
    int    field_0x27a;  // +0x27a
    int    field_0x27e;  // +0x27e  !union: conflicting widths
    int    field_0x282;  // +0x282
    int    field_0x286;  // +0x286
    int    field_0x28a;  // +0x28a
    int    field_0x28e;  // +0x28e
    int    field_0x292;  // +0x292
    int    field_0x296;  // +0x296
    int    field_0x29a;  // +0x29a
    int    field_0x29e;  // +0x29e
    int    field_0x2a2;  // +0x2a2
    int    heroContextLocked;  // +0x2a6
    int    field_0x2aa;  // +0x2aa
    int    field_0x2ae;  // +0x2ae
    int    field_0x2b2;  // +0x2b2
    int    field_0x2b6;  // +0x2b6
    int    field_0x2ba;  // +0x2ba
    int    field_0x2be;  // +0x2be
    char _pad_0x2c2[0x20];
    class sample *loopingSamples[5];  // +0x2e2
    char _pad_0x2f6[0x80];
    int    field_0x376;  // +0x376
    int    field_0x37a;  // +0x37a
    // --- constructors ---
    advManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void StartCursor(int);
    void StopCursor(int);
    void DrawCursor(void);
    void DrawCursorShadow(void);
    int GetCursorBaseFrame(int);
    void TurnTo(int);
    int GetMoveShowIt(class hero *, int);
    class mapCell * MoveHero(int, int, int *, int *, int *, int, int *, int);
    void CheckAdjacentMon(int *);
    int ValidMoveWithEvent(class hero *, int);
    int ValidMove(int, int);
    void MoveOrigin(int, int);
    void ProcessMapChange(struct SMapChange);
    void ProcessIncomingSingleMapChange(struct SMapChange *);
    void ProcessIncomingGroupMapChange(char *);
    void PurgeMapChangeQueue(void);
    void UnwindMapChangeQueue(int, int);
    void ViewWorld(int, int, int);
    void VWCleanup(void);
    void VWInit(int, int);
    void VWCompleteDraw(void);
    void GetCursorSampleSet(int);
    class mapCell * DoAdvCommand(void);
    void CheckSetEvilInterface(int, int);
    void Reseed(int, int);
    int ProcessSelect(struct tag_message *, class mapCell * *);
    int ProcessDeSelect(struct tag_message *, int *, class mapCell * *);
    int ProcessSearch(int, int);
    int ProcessHover(int, int);
    void UpdateScreen(int, int);
    void CompleteDraw(int, int, int, int);
    void CompleteDraw(int);
    int GetCloudLookup(int, int);
    void DrawCell(int, int, int, int, int, int);
    class mapCell * GetCell(int, int);
    void UpdateRadar(int, int);
    void QuickInfo(int, int);
    void UpdateHeroLocator(int, int, int);
    void UpdateHeroLocators(int, int);
    void UpdateTownLocators(int, int);
    void UpdBottomView(int, int, int);
    void ClearBottomView(void);
    int UpdBottomViewEnemyTurn(void);
    int UpdBottomViewNewTurn(void);
    int UpdBottomViewResMsg(void);
    int UpdBottomViewKingdom(void);
    int UpdBottomViewHero(void);
    void HeroQuickView(int, int, int, int);
    char * GetArmySizeName(int, int);
    void TownQuickView(int, int, int, int);
    void RedrawAdvScreen(int, int);
    void DeactivateCurrTown(void);
    void DeactivateCurrHero(void);
    void MobilizeCurrHero(int);
    void DemobilizeCurrHero(void);
    void SetTownContext(int);
    void SetHeroContext(int, int);
    void DoHeroKnob(void);
    void DoTownKnob(void);
    void CastSpell(int);
    void CheckCastSpell(void);
    int ComboDraw(int, int, int);
    int ComboDraw(int);
    void SetEnvironmentOrigin(int, int, int);
    void CheckLoadSample(int);
    int GetSoundId(int, int);
    void InsertSound(int, int, int, int);
    void TeleportTo(class hero *, int, int, int, int);
    void DimensionDoor(void);
    void TownGate(int);
    void SummonBoat(void);
    void ShowRoute(int, int, int);
    void HideRoute(int, int, int);
    void CheckDimHero(void);
    void CheckDimNextHeroBut(void);
    void SeedTo(int, int);
    void ForceNewHover(void);
    void ScreenScroll(int, int);
    void CheckScreenScroll(void);
    int MouseInScrollZone(void);
    void SetInitialMapOrigin(void);
    void LoadRemote(void);
    char * CheckHandleNet(void);
    int CheckHandleNetPlayerWait(struct tag_message &, int);
    void TrimLoopingSounds(int);
    void DisableButtons(void);
    void EnableButtons(void);
    void SaveAdventureBorder(void);
    void DrawAdventureBorder(void);
    int FindAdjacentMonster(int, int, int *, int *, int, int);
    void ViewPuzzle(void);
    void PuzzleDraw(int, int, int, int);
    void AdvPanel(void);
    int ControlPanel(void);
    void SystemOptions(void);
    int DoVisions(class hero *);
    int IsCrystalBallInEffect(int, int, int);
    void DoEvent(class mapCell *, int, int);
    void EraseObj(class mapCell *, int, int);
    void HeroSwap(class hero *, class hero *);
    int BarrierEvent(class mapCell *, class hero *);
    void PasswordEvent(class mapCell *, class hero *);
    void GenericSiteEvent(class mapCell *, class hero *);
    void RecruitSiteEvent(class mapCell *, class hero *);
    void ExpansionRecruitEvent(class hero *, int, short int *);
    void JailEvent(class mapCell *, class hero *, int, int);
    void TownEvent(class mapCell *, int, int);
    void EventSound(int, int, struct SAMPLE2 *);
    void EventWindow(int, int, char *, int, int, int, int, int);
    int GiveRandomArtifact(class hero *);
    int GiveExperience(class hero *, int, int);
    void GiveResource(class hero *, int, int);
    void RecruitEvent(class hero *, int, class mapCell *);
    int SkeletonEvent(class hero *, class mapCell *, char *, int, int);
    int ZombieEvent(class hero *, class mapCell *, char *, int, int);
    int GhostEvent(class hero *, class mapCell *, char *, int, int);
    void HouseEvent(class hero *, class mapCell *);
    int CombatMonsterEvent(class hero *, int, int, class mapCell *, int, int, int, int, int, int, int, int, int, int, int);
    void TransferArtifacts(class hero *, class hero *);
    void HeroLoses(class hero *);
    void DoWhirlpool(class hero *);
    void FizzleCenter(int);
    void DoAIEvent(class mapCell *, class hero *, int, int);
    int BarrierAIEvent(class mapCell *, class hero *);
    void PasswordAIEvent(class mapCell *, class hero *);
    void GenericSiteAIEvent(class mapCell *, class hero *);
    void RecruitSiteAIEvent(class mapCell *, class hero *);
    void JailAIEvent(class mapCell *, class hero *, int, int);
    void PlayerMonsterInteract(class mapCell *, class mapCell *, class hero *, int *, int, int, int, int, int);
    void ComputerMonsterInteract(class mapCell *, class hero *, int *);
    int DoNetCombat(char *);
    int DoCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int);
    void SendHeroTownData(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int, int, int, int);
    void ReceiveHeroTownData(char *, int *, int *, int *, class hero * *, class armyGroup * *, class town * *, class hero * *, class armyGroup * *, int *, int *, int *, signed char *, signed char *, signed char *);
    int AutoResolveCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int);
};
#pragma pack(pop)
SIZE(advManager, 0x37e);
// ---- globals (declarations, RVA order) ----
extern int bMoveSoundMade;
extern int *giPixelsPerStep;
extern int *giStepDelay;
extern struct _SAMPLE *hOldWalkSample;
extern struct _SAMPLE *hNewWalkSample;
extern int EveryOther;
extern int *startVals;
extern signed char iVWHalf[3][6][2];
extern int giViewWorldScale;
extern int giViewWorldScaleLookup;
extern int gbInViewWorld;
extern int giLimitUpdMinX;
extern int iLastScrollTime;
extern int iSandAnim;
extern int giLastHourGlassUpdateTime;
extern int TrigX;
extern int TrigY;
extern int iCurBottomView;
extern int iCurBottomViewEnemy;
extern int iLastHourGlassPhase;
extern int gbForceUpdate;
extern int giCheatSeq;
extern int iQWE;
extern unsigned char *monAnimDrawFrame;
extern int iLastSandAnimTime;
extern int iLastNewSandAnimTime;
extern int giFrameCount;
extern int gbNoShowCombat;
extern int S1cursorCycle;
extern int S1cursorFrameCount;
extern int S1cursorTurning;
extern int S1cursorBaseFrame;
extern int S1cursorDirection;
extern struct SMapChange *sMapChangeLastFew;
extern class icon *pVWMisc;
extern class icon *pVWLetters;
extern int iVWYPixelOffset;
extern class icon *pVWGround;
extern int iVWViewableCells;
extern class icon *pVWFlags;
extern int iVWDrawAllTerrains;
extern int iVWWhatToDraw;
extern int iVWDrawAllObjs;
extern int iVWMapOriginX;
extern int iVWMapOriginY;
extern int iVWCenterOffset;
extern int iVWXPixelOffset;
extern class heroWindow *cPanel;
extern int iThisMaxY;
extern int giTownPortalChoice;
extern int iThisMinY;
extern class heroWindow *townPortalWin;
extern int giFrameStep;
extern char *cArmySizeName;
extern int giLimitUpdMaxX;
extern int giLimitUpdMaxY;
extern int bPrefsChanged;
extern int giLimitUpdMinY;
DATA(0x00527f50) extern signed char (*bComboDraw)[18];
extern int iLastAnimFrame;

#endif // HOMM2_SOURCE_ADVMANAGER_H
