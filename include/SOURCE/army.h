#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 57 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"

class army {
public:
    // --- constructors ---
    army(void);
    // --- methods ---
    void WaitSample(int);
    void InitClean(void);
    void Init(int, int, int, int, int, int);
    void LoadResources(void);
    void FreeResources(void);
    void DrawToBuffer(int, int, int);
    void Wince(void);
    void Walk(int, int, int);
    void SpecialAttack(void);
    void DirDoAttack(int);
    void DoHydraAttack(int);
    void DoAttack(int);
    void ResetPath(void);
    int WalkTo(void);
    int WalkTo(int);
    int AttackTo(void);
    int AttackTo(int);
    void CheckLuck(void);
    void DamageEnemy(class army *, int *, int *, int, int);
    int Damage(long int, int);
    void PowEffect(int, int, int, int);
    unsigned long int Strength(void);
    int LeaveNoBody(void);
    void ProcessDeath(int);
    void SpellEffect(int, int, int);
    void CancelSpellType(int);
    void CancelIndividualSpell(int);
    int SetSpellInfluence(int, int);
    void DecrementSpellRounds(void);
    void GoBerserk(void);
    void MoveAttack(int, int);
    float SpellCastWorkChance(int);
    int SpellCastWorks(int);
    void DispelGood(void);
    void Cure(int);
    int MidX(void);
    int MidY(void);
    int TopY(void);
    int RightX(void);
    int LeftX(void);
    int OtherArmyAdjacent(int, int);
    int GetPowBaseY(void);
    int CanFit(int, int, int *);
    int ValidFlight(int, int);
    int FlyTo(void);
    int FlyTo(int);
    int FindPath(int, int, int, int, int);
    int ValidPath(int, int);
    int GetMoveMask(int);
    int GetAttackMask(int, int, int);
    int ValidMove(int);
    int ValidMove(int, int);
    int ValidAttack(int, int, int, int, int *);
    int GetAdjacentCellIndex(int, int);
    int ValidRange(int);
    int GetBestDirection(int, int, int);
};
