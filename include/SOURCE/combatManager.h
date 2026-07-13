#ifndef HOMM2_SOURCE_COMBATMANAGER_H
#define HOMM2_SOURCE_COMBATMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 149 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
#include "../BASE/icon.h"
#include "army.h"
#include "hexcell.h"
// forward declarations:
class armyGroup;
class hero;
class heroWindow;
class icon;
class town;
struct SBolt;
struct tag_message;

typedef enum CombatDrawLayer {
    COMBAT_DRAW_LAYER_COUNT = 9,
    COMBAT_DRAW_RIGHT_HERO_LAYER = 1,
    COMBAT_DRAW_LEFT_HERO_LAYER = 2,
    COMBAT_DRAW_WALL_TOP_LAYER = 4,
    COMBAT_DRAW_WALL_MIDDLE_LAYER = 5,
    COMBAT_DRAW_GATE_LAYER = 6,
    COMBAT_DRAW_CATAPULT_LAYER = 7
} CombatDrawLayer;

typedef enum CombatCastleHex {
    COMBAT_CASTLE_HEX_TOP_TOWER = 9,
    COMBAT_CASTLE_HEX_TOP_WALL = 22,
    COMBAT_CASTLE_HEX_SECOND_TOWER = 34,
    COMBAT_CASTLE_HEX_SECOND_WALL = 47,
    COMBAT_CASTLE_HEX_GATE = 59,
    COMBAT_CASTLE_HEX_THIRD_WALL = 73,
    COMBAT_CASTLE_HEX_THIRD_TOWER = 86,
    COMBAT_CASTLE_HEX_BOTTOM_WALL = 100,
    COMBAT_CASTLE_HEX_BOTTOM_TOWER = 113,
    COMBAT_CASTLE_SPECIAL_HEX_FIRST = 114,
    COMBAT_CASTLE_SPECIAL_HEX_SECOND = 115
} CombatCastleHex;

typedef enum CombatSmallViewText {
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_PLURAL = 0,
    COMBAT_SMALL_VIEW_TEXT_QUANTITY_SINGULAR = 1,
    COMBAT_SMALL_VIEW_TEXT_ATTACK = 2,
    COMBAT_SMALL_VIEW_TEXT_DEFENSE = 3,
    COMBAT_SMALL_VIEW_TEXT_HIT_POINTS = 4,
    COMBAT_SMALL_VIEW_TEXT_DAMAGE = 5,
    COMBAT_SMALL_VIEW_TEXT_MORALE = 6,
    COMBAT_SMALL_VIEW_TEXT_LUCK = 7,
    COMBAT_SMALL_VIEW_TEXT_SHOTS = 8
} CombatSmallViewText;

typedef enum CombatSmallViewModifierFrame {
    COMBAT_SMALL_VIEW_BAD_LUCK_FRAME = 0,
    COMBAT_SMALL_VIEW_GOOD_LUCK_FRAME = 1,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_FRAME = 2,
    COMBAT_SMALL_VIEW_BAD_MORALE_FRAME = 3,
    COMBAT_SMALL_VIEW_GOOD_MORALE_FRAME = 4,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_FRAME = 5
} CombatSmallViewModifierFrame;

enum {
    COMBAT_GRID_ROW_COUNT = 9,
    COMBAT_GRID_ROW_LENGTH = 13,
    COMBAT_GRID_FIRST_COLUMN = 1,
    COMBAT_GRID_COLUMN_END = 12,
    COMBAT_GRID_REVERSE_FIRST_COLUMN = 11,
    COMBAT_GRID_REVERSE_COLUMN_END = 0,
    COMBAT_DRAW_PHASE_COUNT = 4,
    COMBAT_DRAW_ALL_OCCUPANTS = 100,
    COMBAT_CASTLE_REVERSE_ROW = 5,
    COMBAT_CASTLE_GATE_ROW = 4,
    COMBAT_CASTLE_GATE_OPEN = 4,
    COMBAT_CASTLE_GATE_HIDDEN = 3,
    COMBAT_SIDE_COUNT_DRAWING = 2,
    COMBAT_ARMY_SLOT_COUNT_DRAWING = 20,
    COMBAT_HERO_LEFT_X = 30,
    COMBAT_HERO_LEFT_Y = 183,
    COMBAT_HERO_RIGHT_X = 610,
    COMBAT_HERO_RIGHT_Y = 148,
    COMBAT_HERO_RIGHT_ALT_X = 615,
    COMBAT_HERO_RIGHT_ALT_Y = 135,
    COMBAT_CATAPULT_X = 22,
    COMBAT_CATAPULT_Y = 390,
    COMBAT_CASTLE_TOP_LAYER_FRAME = 0x19,
    COMBAT_CASTLE_GATE_FRAME = 0x1a,
    COMBAT_CASTLE_GATE_CLOSED_FRAME = 0x14,
    COMBAT_SCREEN_WIDTH = 0x280,
    COMBAT_AREA_HEIGHT = 0x1bb,
    COMBAT_MAX_EXTENT_X = 0x27f,
    COMBAT_MAX_EXTENT_Y = 0x1ba,
    COMBAT_SMALL_VIEW_FULL_INFO = 2,
    COMBAT_SMALL_VIEW_LEFT_X = 5,
    COMBAT_SMALL_VIEW_RIGHT_X = 555,
    COMBAT_SMALL_VIEW_FULL_Y = 299,
    COMBAT_SMALL_VIEW_FULL_RIGHT_Y = 154,
    COMBAT_SMALL_VIEW_COMPACT_Y = 366,
    COMBAT_SMALL_VIEW_COMPACT_RIGHT_Y = 288,
    COMBAT_SMALL_VIEW_WIDTH = 83,
    COMBAT_SMALL_VIEW_FULL_HEIGHT = 141,
    COMBAT_SMALL_VIEW_COMPACT_HEIGHT = 74,
    COMBAT_SMALL_VIEW_ICON_SIZE = 19,
    COMBAT_SMALL_VIEW_MAX_SPELLS = 6,
    COMBAT_SMALL_VIEW_UNUSED_POSITION = 255,
    COMBAT_SMALL_VIEW_SPELL_X_FIRST = 1,
    COMBAT_SMALL_VIEW_SPELL_X_SECOND = 11,
    COMBAT_SMALL_VIEW_SPELL_X_THIRD = 22,
    COMBAT_SMALL_VIEW_SPELL_X_FOURTH = 32,
    COMBAT_SMALL_VIEW_SPELL_X_FIFTH = 43,
    COMBAT_SMALL_VIEW_SPELL_Y_FIRST = 0,
    COMBAT_SMALL_VIEW_SPELL_Y_SECOND = 10,
    COMBAT_SMALL_VIEW_SPELL_Y_THIRD = 21,
    COMBAT_SMALL_VIEW_INSET_X = 6,
    COMBAT_SMALL_VIEW_TEXT_X = 8,
    COMBAT_SMALL_VIEW_TEXT_WIDTH = 57,
    COMBAT_SMALL_VIEW_TEXT_HEIGHT = 12,
    COMBAT_SMALL_VIEW_QUANTITY_Y = 7,
    COMBAT_SMALL_VIEW_FIRST_STAT_Y = 20,
    COMBAT_SMALL_VIEW_STAT_ROW_HEIGHT = 9,
    COMBAT_SMALL_VIEW_SHOTS_Y = 74,
    COMBAT_SMALL_VIEW_FULL_SPELL_Y = 86,
    COMBAT_SMALL_VIEW_COMPACT_SPELL_Y = 19,
    COMBAT_SMALL_VIEW_MODIFIER_STEP = 10,
    COMBAT_SMALL_VIEW_MODIFIER_RIGHT_X = 57,
    COMBAT_SMALL_VIEW_NEUTRAL_MORALE_X = 45,
    COMBAT_SMALL_VIEW_NEUTRAL_LUCK_X = 50,
    COMBAT_ARMY_FLAG_SHOOTER = 4
};

#pragma pack(push, 1)  // recovered layout is byte-packed
class combatManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    m_unknown36;  // +0x36
    char _pad_0x3a[0x4da];
    hexcell m_hexCells[117];  // +0x514
    char _pad_0x31de[0x24];
    class icon *m_catapultIcon;  // +0x3202
    char _pad_0x3206[0x4];
    class icon *m_towerIcon;  // +0x320a
    char _pad_0x320e[0x10];
    class icon *m_smallViewBackgroundIcon;  // +0x321e
    class icon *m_smallViewModifierIcon;  // +0x3222
    class icon *m_smallViewSpellIcon;  // +0x3226
    class icon *m_moatIcon;  // +0x322a
    class icon *m_drawbridgeIcon;  // +0x322e
    class icon *m_obstacleIcons[9];  // +0x3232
    short m_eagleEyeSpell[2];  // +0x3256
    int m_drawbridgeState;  // +0x325a
    int m_drawbridgeBackgroundVisible;  // +0x325e
    unsigned char m_wallStates[9];  // +0x3262
    class bitmap *m_combatBuffer;  // +0x326b
    class bitmap *m_backgroundBuffer;  // +0x326f
    char _pad_0x3273[0x4];
    int m_backgroundDrawn;  // +0x3277
    char _pad_0x327b[0x8];
    class town *m_castle;  // +0x3283
    class hero *m_heroes[2];  // +0x3287
    char _pad_0x328f[0xfa];
    int m_spellPower[2];  // +0x3389
    char _pad_0x3391[0x14];
    int m_heroAnimationState[2];  // +0x33a5
    int m_heroAnimationFrame[2];  // +0x33ad
    int m_heroSpriteIndex[2];  // +0x33b5
    char _pad_0x33bd[0x8];
    class icon *m_heroIcons[2];  // +0x33c5
    class icon *m_heroOverlayIcons[2];  // +0x33cd
    int m_heroOverlayFrame[2];  // +0x33d5
    struct SLimitData m_heroLimits[2];  // +0x33dd
    struct SLimitData m_heroOverlayLimits[2];  // +0x33fd
    struct SLimitData m_moatLimits[9];  // +0x341d
    char _pad_0x34ad[0x92];
    int m_heroCastSpell[2];  // +0x353f
    int m_armyCount[2];  // +0x3547
    class army m_armies[2][21];  // +0x354f
    int m_currentArmySide;  // +0xf2a3
    int m_currentArmyIndex;  // +0xf2a7
    char _pad_0xf2ab[0x4];
    int m_currentSide;  // +0xf2af
    char _pad_0xf2b3[0x4];
    int m_limitCreature;  // +0xf2b7
    int m_limitCreatureHex;  // +0xf2bb
    char _pad_0xf2bf[0x14];
    struct SLimitData m_catapultLimits;  // +0xf2d3
    struct SLimitData m_gateLimits;  // +0xf2e3
    struct SLimitData m_upperWallLimits;  // +0xf2f3
    struct SLimitData m_middleWallLimits;  // +0xf303
    int m_catapultFrame;  // +0xf313
    char _pad_0xf317[0x1c];
    int m_inCastleCombat;  // +0xf333
    char _pad_0xf337[0x20];
    int m_nonVisualCombat;  // +0xf357
    char _pad_0xf35b[0x24];
    int m_limitCreatureCount[2][20];  // +0xf37f
    int m_drawHero[2];  // +0xf41f
    int m_drawHeroOverlay[2];  // +0xf427
    int m_combatWindowOpen;  // +0xf42f
    char _pad_0xf433[0xf0];
    struct SLimitData m_smallViewLimits;  // +0xf523
    char _pad_0xf533[0x10];
    int m_smallViewSide[2];  // +0xf543
    int m_smallViewArmyIndex[2];  // +0xf54b
    int m_smallViewLastX[2];  // +0xf553
    int m_smallViewLastY[2];  // +0xf55b
    int m_smallViewWidth[2];  // +0xf563
    int m_smallViewHeight[2];  // +0xf56b
    char _pad_0xf573[0x304];
    // --- constructors ---
    combatManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void NoShowCombatLog(char *);
    void ClearCombatMessages(int);
    void CheckUpdateCombatMessages(void);
    void CombatMessage(char *, int, int, int);
    void CombatMessage(int);
    void ResetLimitCreature(void);
    void UpdateCombatArea(void);
    void SetupGridForArmy(class army *);
    int UpdateGrid(int, int);
    void DrawBackground(void);
    void UpdateMouseGrid(int, int);
    void DrawFrame(int, int, int, int, int, int, int);
    void DrawSmallView(int, int);
    int ViewGeneral(int, int, int);
    void ViewArmy(class army *, int);
    int HasValidSpellTarget(int);
    int ViewSpells(int);
    int FindResurrectArmyIndex(int, int, int);
    int ValidSpellTarget(int, int);
    void SpellMessage(int, int);
    void CastSpell(int, int, int, int);
    void DefaultSpell(int);
    void Fireball(int, int);
    void MeteorShower(int);
    void ElementalStorm(void);
    void Armageddon(void);
    void TurnToStone(class army *);
    void BloodLustEffect(class army *, int);
    void Ripple(int);
    void Blur(int, int, int);
    void ResetBoltAngle(struct SBolt *);
    void DrawBolt(struct SBolt *, int);
    void AddBolt(struct SBolt *, int, int, int, int, int, int, int, int, int, int, int, int);
    void DoBolt(int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int);
    int GetNextChainLightningTarget(class army *, int);
    void ChainLightning(int, int);
    void VaporizeCreature(int, int);
    void RippleCreature(int, int, int);
    void ShowMassSpell(signed char (* const)[20], int, int);
    void CastMassSpell(int, int);
    void MirrorImage(int);
    void SummonElemental(int, int);
    void DoLuck(int, int);
    void DoBlast(int, int);
    void Resurrect(int, int, int);
    int SpaceForElementalExists(void);
    void ShowSpellCastFailure(class army *, int);
    void ModifyDamageForArtifacts(long int *, int, class hero *, class hero *);
    void Earthquake(void);
    void ShowSpellMessage(int, int, class army *);
    int ValidHexToStandOn(int);
    void SetCombatDirections(int);
    void CheckSetMouseDirection(int, int, int);
    int GetPointer(int, int);
    int ProcessCombatMsg(struct tag_message &);
    int IsNegationSphereInEffect(void);
    void ResetRound(void);
    int CheckWin(struct tag_message *);
    int GetCommand(int);
    int RightClick(int);
    void DoCommand(int);
    void ClearWinLoseBottom(class heroWindow *);
    void ShowWinLoseArtifact(class heroWindow *, int);
    void ShowSkeletons(class heroWindow *);
    void ShowEagleEyeSpell(class heroWindow *);
    void ShowDeadArmies(class heroWindow *);
    void DoVictory(int);
    void DoLoseWindow(void);
    int DoSurrender(void);
    void CheckChangeSelector(void);
    void CheckCastleAttack(void);
    void CheckGetAIMove(void);
    void GetControl(void);
    void ResetMouse(void);
    int ProcessNextAction(struct tag_message &);
    void ResetCyclingCreatures(void);
    void ResetCycleTimers(void);
    void CycleCombatScreen(void);
    void SetCombatViewArmySmallLevel(int);
    void SetCombatGrid(int, int, int);
    void AddArmy(int, int, int, int, int, int);
    void SetupSmallView(void);
    void ViewBallista(int);
    int DoSpellAI(int, int);
    void DetermineEffectOfSpell(int, int *, int *);
    int EffectSpellCreateCreature(int, int);
    int RawEffectSpellInfluence(class army *, int);
    void ClearEffects(void);
    void NextPos(int *);
    int FirstArmy(int, int, int *);
    int FirstResurrectable(int, int *, int);
    void EffectSpellCure(int *, int, int, int);
    void EffectSpellResurrect(int *, int, int);
    void EffectSpellDamage(int *, int, int);
    void CombineGroups(class armyGroup *, class armyGroup *);
    void SetupCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int);
    void InitNonVisualVars(void);
    void SetupAdjacencyArray(void);
    void UpdateArmyGroup(int);
    void GenerateMap(void);
    char * GetBackgroundName(void);
    int MoreTreesNear(void);
    void LoadIcons(void);
    void FreeIcons(void);
    void LoadArmies(void);
    void FreeArmies(void);
    int GetGridIndex(int, int);
    void CheckApplyGoodMorale(int, int);
    int CheckApplyBadMorale(int, int);
    int GetNextArmy(int);
    int IsWinner(int);
    void CatAttack(int);
    void KeepAttack(int);
    int ExperienceValueOfStack(int);
    void ResetHitByCreature(void);
    void SaveCombatBorder(void);
    void DrawCombatBorder(void);
    void SetupAndLoadObstacles(void);
    void MakeCreaturesVanish(void);
    void LowerDoor(void);
    void RaiseDoor(void);
    void TestRaiseDoor(void);
    int InCastle(int);
    int ShotIsThroughWall(int, int, int);
    void ShootMissile(int, int, int, int, float *, class icon *);
    void CombatSystemOptions(void);
    int AICheckRetreat(void);
    void DoCompAI(int);
    float GetModLichDamage(class army *, float);
    void DoLichShot(class army *);
    int GetShooterMask(int);
    int GetMirrorImageMask(int);
    int GetFlyerMask(int);
    int GetAllMask(int);
    int GetWalkerMask(int);
    int GetOutOfItMask(int);
    int GetTraitorMask(int);
    int GetBestArmy(int, int);
    int GetWorstArmy(int, int);
    int GetClosestArmy(class army *, int, int);
    unsigned long int GetStrength(int, int);
    int AttemptAttack(class army *, int, int);
    int AttemptAdjacentAttack(class army *);
    int WalkTowardArmyFront(class army *, int, int);
    int WalkTowardArmy(class army *, int, int);
};
#pragma pack(pop)
SIZE(combatManager, 0xf877);
// ---- globals (declarations, RVA order) ----
extern int bGridWasShowing;
extern int gbInDrawSmallView;
extern int iViewGeneralWhichSide;
extern int castX;
extern int castY;
extern int bInTeleportGetDest;
extern int indexToCastOn;
extern unsigned char *uRedBeam;
extern unsigned char *uRainbow;
extern signed char *gyModify;
extern float *gfDurationMods;
extern float *gfCancelDurationMods;
extern int bInHighMoraleBonus;
extern int giSeed;
extern unsigned char *wallHex;
extern int gbThisNetHasControl;
extern int iCurTransferArtifact;
extern signed char *iTransferArtifactsInfo;
extern int gbWhichAnimationPlaying;
extern int iMaxTransferArtifacts;
extern int giNextActionExtra;
extern int bSkeletonsShown;
extern int gbShowingLoseWindow;
extern int giWinCmbtFrame;
extern int giNextActionGridIndex;
extern int giSurrenderCost;
extern int giSkeletonsCreated;
extern signed char *iTransferArtifacts;
extern int giNextAction;
extern int giNextActionGridIndex2;
extern int giCurrSpellGroup;
extern int bMouseWasVis;
extern class heroWindow *CSPanel;
extern int bCPrefsChanged;

#endif // HOMM2_SOURCE_COMBATMANAGER_H
