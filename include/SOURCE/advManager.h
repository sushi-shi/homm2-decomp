#ifndef HOMM2_SOURCE_ADVMANAGER_H
#define HOMM2_SOURCE_ADVMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 139 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
#include <SOURCE/ADVMGR.h>
#include <SOURCE/Viewwrld.h>
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

#define ADVMGR_MONSTER_ANIMATION_TABLE_SIZE 16

struct adventureSoundCell {
    i32 soundId;
    i32 volume;
};

#pragma pack(push, 1) // recovered layout is byte-packed
class advManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    AdventureCommand m_selectedCell; // +0x36
    union {
        i32 m_heroLocatorState[12]; // +0x3a
        class widget* m_bottomViewPrimaryWidgets[12];
        struct {
            class iconWidget* m_bottomViewBackground;          // +0x3a
            class iconWidget* m_bottomViewHourglassBackground; // +0x3e
            class iconWidget* m_bottomViewIcons[5];            // +0x42
            char m_bottomViewIconPadding[0x14];                // +0x56
        };
    };
    union {
        i32 m_townLocatorState[12]; // +0x6a
        class widget* m_bottomViewSecondaryWidgets[12];
        class textWidget* m_bottomViewAllTexts[12];
        struct {
            i32 m_bottomViewTextReserved;           // +0x6a
            class textWidget* m_bottomViewTexts[5]; // +0x6e
            char m_bottomViewTextPadding[0x18];     // +0x82
        };
    };
    class heroWindow* m_adventureWindow; // +0x9a
    u16* m_visibilityMap;                // +0x9e, packed route display cells
    i32 m_visibilityMapValid;            // +0xa2
    i32 m_currentTerrain;                // +0xa6, index into giTerrainToMusicTrack
    char _pad_0xaa[0x4];
    class fullMap* m_mapData;              // +0xae
    class iconWidget* m_scrollLeftButton;  // +0xb2
    class iconWidget* m_scrollRightButton; // +0xb6
    u8* m_adventureBorder;                 // +0xba
    char _pad_0xbe[0x4];
    class tileset* m_groundTiles;   // +0xc2
    class tileset* m_cloudTiles;    // +0xc6
    class tileset* m_stoneTiles;    // +0xca
    class icon* m_objectIcons[64];  // +0xce
    class icon* m_puzzleIcon;       // +0x1ce
    class icon* m_cloudOverlayIcon; // +0x1d2
    i32 m_mapOriginX;               // +0x1d6
    i32 m_mapOriginY;               // +0x1da
    i32 m_previousOriginX;          // +0x1de
    i32 m_previousOriginY;          // +0x1e2
    i32 m_lastHoverCell;            // +0x1e6
    i32 m_hoverCellY;               // +0x1ea
    i32 m_commandTargetX;           // +0x1ee
    i32 m_commandTargetY;           // +0x1f2
    i32 m_updateMinX;               // +0x1f6
    i32 m_updateMinY;               // +0x1fa
    i32 m_updateMaxX;               // +0x1fe
    i32 m_updateMaxY;               // +0x202
    i32 m_updatePending;            // +0x206
    i32 m_viewBounds[4];            // +0x20a
    class icon* m_heroIcons[8];     // +0x21a
    class icon* m_shadowIcon;       // +0x23a
    class icon* m_boatShadowIcon;   // +0x23e
    class icon* m_flagIcons[6];     // +0x242
    class icon* m_boatFlagIcons[6]; // +0x25a
    i32 m_cursorActive;             // +0x272
    i32 m_drawHeroShadows;          // +0x276
    i32 m_cursorType;               // +0x27a
    i32 m_cursorDirection;          // +0x27e  !union: conflicting widths
    i32 m_cursorFrame;              // +0x282
    i32 m_cursorFrameCount;         // +0x286
    i32 m_cursorCycle;              // +0x28a
    i32 m_cursorTurning;            // +0x28e
    i32 m_cursorMapX;               // +0x292
    i32 m_previousCursorMapX;       // +0x296
    i32 m_cursorMapY;               // +0x29a
    i32 m_previousCursorMapY;       // +0x29e
    i32 m_comboHeroDrawn;           // +0x2a2
    i32 m_heroContextLocked;        // +0x2a6
    i32 m_townContextLocked;        // +0x2aa
    i32 m_forceCompleteDraw;        // +0x2ae
    i32 m_lastQuickViewX;           // +0x2b2
    i32 m_lastQuickViewY;           // +0x2b6
    i32 m_mineGuardianFacingLeft; // +0x2ba  set when the hero attacks a mine guardian from its left; picks the mirrored guardian frame
    i32 m_activeSoundMask;        // +0x2be
    adventureSoundCell m_activeSounds[4]; // +0x2c2
    class sample* m_loopingSamples[28];   // +0x2e2
    class sample* m_cursorSamples[9];     // +0x352
    i32 m_identifyHeroActive;             // +0x376
    i32 m_openState;                      // +0x37a
    // --- constructors ---
    advManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void StartCursor(i32);
    void StopCursor(i32);
    void DrawCursor(void);
    void DrawCursorShadow(void);
    i32 GetCursorBaseFrame(i32);
    void TurnTo(i32);
    i32 GetMoveShowIt(class hero*, i32);
    class mapCell* MoveHero(i32, i32, i32*, i32*, i32*, i32, i32*, i32);
    void CheckAdjacentMon(i32*);
    i32 ValidMoveWithEvent(class hero*, i32);
    i32 ValidMove(i32, i32);
    void MoveOrigin(i32, i32);
    void ProcessMapChange(struct SMapChange);
    void ProcessIncomingSingleMapChange(struct SMapChange*);
    void ProcessIncomingGroupMapChange(char*);
    void PurgeMapChangeQueue(void);
    void UnwindMapChangeQueue(i32, i32);
    void ViewWorld(ViewWorldMode, b32, b32);
    void VWCleanup(void);
    void VWInit(i32, i32);
    void VWCompleteDraw(void);
    void GetCursorSampleSet(i32);
    class mapCell* DoAdvCommand(void);
    i32 GetCommandTargetX(void) {
        return m_commandTargetX;
    }
    i32 GetCommandTargetY(void) {
        return m_commandTargetY;
    }
    void CheckSetEvilInterface(i32, i32);
    void Reseed(i32, i32);
    i32 ProcessSelect(struct tag_message*, class mapCell**);
    i32 ProcessDeSelect(struct tag_message*, i32*, class mapCell**);
    i32 ProcessSearch(i32, i32);
    i32 ProcessHover(i32, i32);
    void UpdateScreen(i32, i32);
    void CompleteDraw(i32, i32, i32, i32);
    void CompleteDraw(i32);
    i32 GetCloudLookup(i32, i32);
    void DrawCell(i32, i32, i32, i32, i32, i32);
    class mapCell* GetCell(i32, i32);
    void UpdateRadar(i32, i32);
    void QuickInfo(i32, i32);
    void UpdateHeroLocator(i32, i32, i32);
    void UpdateHeroLocators(i32, i32);
    void UpdateTownLocators(i32, i32);
    void UpdBottomView(i32, i32, i32);
    void ClearBottomView(void);
    i32 UpdBottomViewEnemyTurn(void);
    i32 UpdBottomViewNewTurn(void);
    i32 UpdBottomViewResMsg(void);
    i32 UpdBottomViewKingdom(void);
    i32 UpdBottomViewHero(void);
    void HeroQuickView(i32, i32, i32, i32);
    char* GetArmySizeName(i32, i32);
    void TownQuickView(i32, i32, i32, i32);
    void RedrawAdvScreen(i32, i32);
    void DeactivateCurrTown(void);
    void DeactivateCurrHero(void);
    void MobilizeCurrHero(i32);
    void DemobilizeCurrHero(void);
    void SetTownContext(i32);
    void SetHeroContext(i32, i32);
    void DoHeroKnob(void);
    void DoTownKnob(void);
    void CastSpell(i32);
    void CheckCastSpell(void);
    i32 ComboDraw(i32, i32, i32);
    i32 ComboDraw(i32);
    void SetEnvironmentOrigin(i32, i32, i32);
    void CheckLoadSample(i32);
    AdventureEnvironmentSoundId GetSoundId(i32, i32);
    void InsertSound(i32, i32, i32, i32);
    void TeleportTo(class hero*, i32, i32, i32, i32);
    void DimensionDoor(void);
    void TownGate(i32);
    void SummonBoat(void);
    void ShowRoute(i32, i32, i32);
    void HideRoute(i32, i32, i32);
    void CheckDimHero(void);
    void CheckDimNextHeroBut(void);
    void SeedTo(i32, i32);
    void ForceNewHover(void);
    void ScreenScroll(i32, i32);
    void CheckScreenScroll(void);
    i32 MouseInScrollZone(void);
    void SetInitialMapOrigin(void);
    void LoadRemote(void);
    char* CheckHandleNet(void);
    i32 CheckHandleNetPlayerWait(struct tag_message&, i32);
    void TrimLoopingSounds(i32);
    void DisableButtons(void);
    void EnableButtons(void);
    void SaveAdventureBorder(void);
    void DrawAdventureBorder(void);
    i32 FindAdjacentMonster(i32, i32, i32*, i32*, i32, i32);
    void ViewPuzzle(void);
    void PuzzleDraw(i32, i32, i32, i32);
    void AdvPanel(void);
    i32 ControlPanel(void);
    void SystemOptions(void);
    i32 DoVisions(class hero*);
    i32 IsCrystalBallInEffect(i32, i32, i32);
    void DoEvent(class mapCell*, i32, i32);
    void EraseObj(class mapCell*, i32, i32);
    void HeroSwap(class hero*, class hero*);
    i32 BarrierEvent(class mapCell*, class hero*);
    void PasswordEvent(class mapCell*, class hero*);
    void GenericSiteEvent(class mapCell*, class hero*);
    void RecruitSiteEvent(class mapCell*, class hero*);
    void ExpansionRecruitEvent(class hero*, i32, i16*);
    void JailEvent(class mapCell*, class hero*, i32, i32);
    void TownEvent(class mapCell*, i32, i32);
    void EventSound(i32, i32, struct SAMPLE2*);
    void EventWindow(i32, i32, char*, i32, i32, i32, i32, i32);
    i32 GiveRandomArtifact(class hero*);
    i32 GiveExperience(class hero*, i32, i32);
    void GiveResource(class hero*, i32, i32);
    void RecruitEvent(class hero*, i32, class mapCell*);
    i32 SkeletonEvent(class hero*, class mapCell*, char*, i32, i32);
    i32 ZombieEvent(class hero*, class mapCell*, char*, i32, i32);
    i32 GhostEvent(class hero*, class mapCell*, char*, i32, i32);
    void HouseEvent(class hero*, class mapCell*);
    i32 CombatMonsterEvent(
        class hero*,
        CreatureType,
        i32,
        class mapCell*,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void TransferArtifacts(class hero*, class hero*);
    void HeroLoses(class hero*);
    void DoWhirlpool(class hero*);
    void FizzleCenter(i32);
    void DoAIEvent(class mapCell*, class hero*, i32, i32);
    i32 BarrierAIEvent(class mapCell*, class hero*);
    void PasswordAIEvent(class mapCell*, class hero*);
    void GenericSiteAIEvent(class mapCell*, class hero*);
    void RecruitSiteAIEvent(class mapCell*, class hero*);
    void JailAIEvent(class mapCell*, class hero*, i32, i32);
    void PlayerMonsterInteract(
        class mapCell*,
        class mapCell*,
        class hero*,
        i32*,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void ComputerMonsterInteract(class mapCell*, class hero*, i32*);
    i32 DoNetCombat(char*);
    i32 DoCombat(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32
    );
    void SendHeroTownData(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32,
        i32
    );
    void ReceiveHeroTownData(
        char*,
        i32*,
        i32*,
        i32*,
        class hero**,
        class armyGroup**,
        class town**,
        class hero**,
        class armyGroup**,
        i32*,
        i32*,
        i32*,
        i8*,
        i8*,
        i8*
    );
    i32 AutoResolveCombat(
        i32,
        i32,
        class hero*,
        class armyGroup*,
        class town*,
        class hero*,
        class armyGroup*,
        i32,
        i32,
        i32,
        i32
    );
};
#pragma pack(pop)
SIZE(advManager, 0x37e);
// ---- globals (declarations, RVA order) ----
extern i32 bMoveSoundMade;
extern i32 giPixelsPerStep[6];
extern i32 giStepDelay[5];
extern struct _SAMPLE* hOldWalkSample;
extern struct _SAMPLE* hNewWalkSample;
extern i32 EveryOther;
extern i32 startVals[3];
extern i8 iVWHalf[3][6][2];
extern ViewWorldScale giViewWorldScale;
extern i32 giViewWorldScaleLookup;
extern b32 gbInViewWorld;
extern i32 giLimitUpdMinX;
extern i32 iLastScrollTime;
extern i32 iSandAnim;
extern i32 giLastHourGlassUpdateTime;
extern i32 TrigX;
extern i32 TrigY;
extern i32 iCurBottomView;
extern i32 iCurBottomViewEnemy;
extern i32 iCurHourGlassPhase;
extern i32 iLastHourGlassPhase;
extern b32 gbForceUpdate;
extern i32 giCheatSeq;
extern i32 iQWE;
extern u8 monAnimDrawFrame[ADVMGR_MONSTER_ANIMATION_TABLE_SIZE];
extern i32 iLastSandAnimTime;
extern i32 iLastNewSandAnimTime;
extern i32 giFrameCount;
extern b32 gbNoShowCombat;
extern i32 S1cursorCycle;
extern i32 S1cursorFrameCount;
extern i32 S1cursorTurning;
extern i32 S1cursorBaseFrame;
extern i32 S1cursorDirection;
extern class icon* pVWMisc;
extern class icon* pVWLetters;
extern i32 iVWYPixelOffset;
extern class icon* pVWGround;
extern i32 iVWViewableCells;
extern class icon* pVWFlags;
extern i32 iVWDrawAllTerrains;
extern ViewWorldMode iVWWhatToDraw;
extern i32 iVWDrawAllObjs;
extern i32 iVWMapOriginX;
extern i32 iVWMapOriginY;
extern i32 iVWCenterOffset;
extern i32 iVWXPixelOffset;
extern class heroWindow* cPanel;
extern struct tag_message USMsg;
extern i32 iThisMaxY;
extern i32 giTownPortalChoice;
extern i32 iThisMinY;
extern class heroWindow* townPortalWin;
extern i32 giFrameStep;
extern char cArmySizeName[12];
extern i32 giLimitUpdMaxX;
extern i32 giLimitUpdMaxY;
extern i32 bPrefsChanged;
extern i32 giLimitUpdMinY;
extern struct tag_message CDMsg;
extern i8 bComboDraw[18][18];
extern i32 iLastAnimFrame;

#endif // HOMM2_SOURCE_ADVMANAGER_H
