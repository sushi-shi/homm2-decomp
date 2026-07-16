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
class iconWidget;
class textWidget;
class widget;
class tileset;
struct SAMPLE2;
struct SMapChange;
struct tag_message;

#define ADVMGR_MONSTER_ANIMATION_FRAME_COUNT 16

struct adventureSoundCell {
    int soundId;
    int volume;
};

#pragma pack(push, 1)  // recovered layout is byte-packed
class advManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    m_selectedCell;  // +0x36
    union {
        int m_heroLocatorState[12];  // +0x3a
        class widget *m_bottomViewPrimaryWidgets[12];
        struct {
            class iconWidget *m_bottomViewBackground;  // +0x3a
            class iconWidget *m_bottomViewHourglassBackground;  // +0x3e
            class iconWidget *m_bottomViewIcons[5];  // +0x42
            char m_bottomViewIconPadding[0x14];  // +0x56
        };
    };
    union {
        int m_townLocatorState[12];  // +0x6a
        class widget *m_bottomViewSecondaryWidgets[12];
        class textWidget *m_bottomViewAllTexts[12];
        struct {
            int m_bottomViewTextReserved;  // +0x6a
            class textWidget *m_bottomViewTexts[5];  // +0x6e
            char m_bottomViewTextPadding[0x18];  // +0x82
        };
    };
    class heroWindow *m_adventureWindow;  // +0x9a
    unsigned short *m_visibilityMap;  // +0x9e, packed route display cells
    int    m_visibilityMapValid;  // +0xa2
    int m_currentTerrain;  // +0xa6, index into giTerrainToMusicTrack
    char _pad_0xaa[0x4];
    class fullMap *m_mapData;  // +0xae
    class iconWidget *m_scrollLeftButton;  // +0xb2
    class iconWidget *m_scrollRightButton;  // +0xb6
    unsigned char *m_adventureBorder;  // +0xba
    char _pad_0xbe[0x4];
    class tileset *m_groundTiles;  // +0xc2
    class tileset *m_cloudTiles;  // +0xc6
    class tileset *m_stoneTiles;  // +0xca
    class icon *m_objectIcons[64];  // +0xce
    class icon *m_puzzleIcon;  // +0x1ce
    class icon *m_cloudOverlayIcon;  // +0x1d2
    int    m_mapOriginX;  // +0x1d6
    int    m_mapOriginY;  // +0x1da
    int    m_previousOriginX;  // +0x1de
    int    m_previousOriginY;  // +0x1e2
    int    m_lastHoverCell;  // +0x1e6
    int    m_hoverCellY;  // +0x1ea
    int    m_commandTargetX;  // +0x1ee
    int    m_commandTargetY;  // +0x1f2
    int    m_updateMinX;  // +0x1f6
    int    m_updateMinY;  // +0x1fa
    int    m_updateMaxX;  // +0x1fe
    int    m_updateMaxY;  // +0x202
    int    m_updatePending;  // +0x206
    int    m_viewBounds[4];  // +0x20a
    class icon *m_heroIcons[8];  // +0x21a
    class icon *m_shadowIcon;  // +0x23a
    class icon *m_boatShadowIcon;  // +0x23e
    class icon *m_flagIcons[6];  // +0x242
    class icon *m_boatFlagIcons[6];  // +0x25a
    int    m_cursorActive;  // +0x272
    int    m_drawHeroShadows;  // +0x276
    int    m_cursorType;  // +0x27a
    int    m_cursorDirection;  // +0x27e  !union: conflicting widths
    int    m_cursorFrame;  // +0x282
    int    m_cursorFrameCount;  // +0x286
    int    m_cursorCycle;  // +0x28a
    int    m_cursorTurning;  // +0x28e
    int    m_cursorMapX;  // +0x292
    int    m_previousCursorMapX;  // +0x296
    int    m_cursorMapY;  // +0x29a
    int    m_previousCursorMapY;  // +0x29e
    int    m_comboHeroDrawn;  // +0x2a2
    int    m_heroContextLocked;  // +0x2a6
    int    m_townContextLocked;  // +0x2aa
    int    m_forceCompleteDraw;  // +0x2ae
    int    m_lastQuickViewX;  // +0x2b2
    int    m_lastQuickViewY;  // +0x2b6
    int    m_field_0x2ba;  // +0x2ba
    int    m_activeSoundMask;  // +0x2be
    adventureSoundCell m_activeSounds[4];  // +0x2c2
    class sample *m_loopingSamples[28];  // +0x2e2
    class sample *m_cursorSamples[9];  // +0x352
    int m_identifyHeroActive;  // +0x376
    int    m_openState;  // +0x37a
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
    int GetCommandTargetX(void) { return m_commandTargetX; }
    int GetCommandTargetY(void) { return m_commandTargetY; }
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
extern int giPixelsPerStep[6];
extern int giStepDelay[5];
extern struct _SAMPLE *hOldWalkSample;
extern struct _SAMPLE *hNewWalkSample;
extern int EveryOther;
extern int startVals[3];
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
extern int iCurHourGlassPhase;
extern int iLastHourGlassPhase;
extern int gbForceUpdate;
extern int giCheatSeq;
extern int iQWE;
extern unsigned char monAnimDrawFrame[ADVMGR_MONSTER_ANIMATION_FRAME_COUNT];
extern int iLastSandAnimTime;
extern int iLastNewSandAnimTime;
extern int giFrameCount;
extern int gbNoShowCombat;
extern int S1cursorCycle;
extern int S1cursorFrameCount;
extern int S1cursorTurning;
extern int S1cursorBaseFrame;
extern int S1cursorDirection;
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
extern struct tag_message USMsg;
extern int iThisMaxY;
extern int giTownPortalChoice;
extern int iThisMinY;
extern class heroWindow *townPortalWin;
extern int giFrameStep;
extern char cArmySizeName[12];
extern int giLimitUpdMaxX;
extern int giLimitUpdMaxY;
extern int bPrefsChanged;
extern int giLimitUpdMinY;
extern struct tag_message CDMsg;
extern signed char bComboDraw[18][18];
extern int iLastAnimFrame;

#endif // HOMM2_SOURCE_ADVMANAGER_H
