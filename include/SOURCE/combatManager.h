#ifndef HOMM2_SOURCE_COMBATMANAGER_H
#define HOMM2_SOURCE_COMBATMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 149 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class army;
class armyGroup;
class hero;
class heroWindow;
class icon;
class town;
struct SBolt;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class combatManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    char _pad_0x3a[0x85a];
    char   field_0x894;  // +0x894
    char _pad_0x895[0xc5];
    char   field_0x95a;  // +0x95a
    char _pad_0x95b[0x125];
    char   field_0xa80;  // +0xa80
    char _pad_0xa81[0x30d];
    char   field_0xd8e;  // +0xd8e
    char _pad_0xd8f[0xc5];
    char   field_0xe54;  // +0xe54
    char _pad_0xe55[0x125];
    char   field_0xf7a;  // +0xf7a
    char _pad_0xf7b[0xe8fc];  // tail pad to sizeof
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
DATA(0x004ed25c) extern int bGridWasShowing;
DATA(0x004ed290) extern int gbInDrawSmallView;
DATA(0x004eddf4) extern int iViewGeneralWhichSide;
DATA(0x004f00b0) extern int castX;
DATA(0x004f00b4) extern int castY;
DATA(0x004f00b8) extern int bInTeleportGetDest;
DATA(0x004f0278) extern int indexToCastOn;
DATA(0x004f04c0) extern unsigned char *uRedBeam;
DATA(0x004f04c8) extern unsigned char *uRainbow;
DATA(0x004f04e0) extern signed char *gyModify;
DATA(0x004f80b8) extern float *gfDurationMods;
DATA(0x004f80e8) extern float *gfCancelDurationMods;
DATA(0x004f8900) extern int bInHighMoraleBonus;
DATA(0x004f8904) extern int giSeed;
DATA(0x004f8c1c) extern unsigned char *wallHex;
DATA(0x005250b8) extern int gbThisNetHasControl;
DATA(0x005250bc) extern int iCurTransferArtifact;
DATA(0x005250c0) extern signed char *iTransferArtifactsInfo;
DATA(0x005250d0) extern int gbWhichAnimationPlaying;
DATA(0x005250d4) extern int iMaxTransferArtifacts;
DATA(0x005250d8) extern int giNextActionExtra;
DATA(0x005250dc) extern int bSkeletonsShown;
DATA(0x005250e0) extern int gbShowingLoseWindow;
DATA(0x005250e4) extern int giWinCmbtFrame;
DATA(0x005250e8) extern int giNextActionGridIndex;
DATA(0x005250ec) extern int giSurrenderCost;
DATA(0x005250f0) extern int giSkeletonsCreated;
DATA(0x005250f8) extern signed char *iTransferArtifacts;
DATA(0x00525108) extern int giNextAction;
DATA(0x0052510c) extern int giNextActionGridIndex2;
DATA(0x005284b4) extern int giCurrSpellGroup;
DATA(0x00528588) extern int bMouseWasVis;
DATA(0x0052858c) extern class heroWindow *CSPanel;
DATA(0x00528590) extern int bCPrefsChanged;

#endif // HOMM2_SOURCE_COMBATMANAGER_H
