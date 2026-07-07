#ifndef HOMM2_SOURCE_ARMY_H
#define HOMM2_SOURCE_ARMY_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 57 methods, 0 own-virtual, 0 static data.
#include <va.h>

#pragma pack(push, 1)  // recovered layout is byte-packed
class army {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char   field_0x0;  // +0x00
    char _pad_0x1[0x1];
    char   field_0x2;  // +0x02
    char _pad_0x3[0x3];
    int    field_0x6;  // +0x06
    char _pad_0xa[0x44];
    int    field_0x4e;  // +0x4e
    int    field_0x52;  // +0x52
    int    field_0x56;  // +0x56
    int    field_0x5a;  // +0x5a
    int    field_0x5e;  // +0x5e
    int    field_0x62;  // +0x62
    int    field_0x66;  // +0x66
    int    field_0x6a;  // +0x6a
    int    field_0x6e;  // +0x6e
    int    field_0x72;  // +0x72
    int    field_0x76;  // +0x76
    int    field_0x7a;  // +0x7a
    int    field_0x7e;  // +0x7e
    int    field_0x82;  // +0x82
    int    field_0x86;  // +0x86
    int    field_0x8a;  // +0x8a
    int    field_0x8e;  // +0x8e
    int    field_0x92;  // +0x92
    int    field_0x96;  // +0x96
    int    field_0x9a;  // +0x9a
    int    field_0x9e;  // +0x9e
    int    field_0xa2;  // +0xa2
    int    field_0xa6;  // +0xa6
    int    field_0xaa;  // +0xaa
    int    field_0xae;  // +0xae  !union: conflicting widths
    int    field_0xb2;  // +0xb2
    int    field_0xb6;  // +0xb6
    short  field_0xba;  // +0xba
    char _pad_0xbc[0x6];
    short  field_0xc2;  // +0xc2
    char _pad_0xc4[0x1];
    char   field_0xc5;  // +0xc5
    char   field_0xc6;  // +0xc6
    char   field_0xc7;  // +0xc7
    char   field_0xc8;  // +0xc8
    char   field_0xc9;  // +0xc9
    char   field_0xca;  // +0xca
    char _pad_0xcb[0x5];
    int    field_0xd0;  // +0xd0  !union: conflicting widths
    short  field_0xd4;  // +0xd4
    int    field_0xd6;  // +0xd6
    int    field_0xda;  // +0xda
    int    field_0xde;  // +0xde
    int    field_0xe2;  // +0xe2
    int    field_0xe6;  // +0xe6
    int    field_0xea;  // +0xea  !union: conflicting widths
    int    field_0xee;  // +0xee
    int    field_0xf2;  // +0xf2
    int    field_0xf6;  // +0xf6
    int    field_0xfa;  // +0xfa
    int    field_0xfe;  // +0xfe
    int    field_0x102;  // +0x102
    int    field_0x106;  // +0x106
    char   field_0x10a;  // +0x10a
    char _pad_0x10b[0x1];
    char   field_0x10c;  // +0x10c
    char   field_0x10d;  // +0x10d
    char _pad_0x10e[0x1];
    char   field_0x10f;  // +0x10f
    char   field_0x110;  // +0x110
    char   field_0x111;  // +0x111
    char   field_0x112;  // +0x112
    char   field_0x113;  // +0x113
    char   field_0x114;  // +0x114
    char   field_0x115;  // +0x115
    char   field_0x116;  // +0x116
    char   field_0x117;  // +0x117
    char   field_0x118;  // +0x118
    char _pad_0x119[0x4];
    int    field_0x11d;  // +0x11d
    char _pad_0x121[0x4];
    int    field_0x125;  // +0x125
    char _pad_0x129[0x1];
    short  field_0x12a;  // +0x12a
    short  field_0x12c;  // +0x12c
    char _pad_0x12e[0x9d];
    int    field_0x1cb;  // +0x1cb
    char _pad_0x1cf[0x4];
    int    field_0x1d3;  // +0x1d3
    char _pad_0x1d7[0x4];
    short  field_0x1db;  // +0x1db
    short  field_0x1dd;  // +0x1dd
    char _pad_0x1df[0x4];
    char   field_0x1e3;  // +0x1e3
    char _pad_0x1e4[0x38];
    char   field_0x21c;  // +0x21c
    char _pad_0x21d[0x1];
    char   field_0x21e;  // +0x21e
    char   field_0x21f;  // +0x21f
    char _pad_0x220[0x2];
    char   field_0x222;  // +0x222
    char _pad_0x223[0x7];
    char   field_0x22a;  // +0x22a
    char   field_0x22b;  // +0x22b
    char _pad_0x22c[0x82];
    char   field_0x2ae;  // +0x2ae
    char _pad_0x2af[0x1af];
    int    field_0x45e;  // +0x45e
    int    field_0x462;  // +0x462
    int    field_0x466;  // +0x466
    int    field_0x46a;  // +0x46a
    int    field_0x46e;  // +0x46e
    int    field_0x472;  // +0x472
    int    field_0x476;  // +0x476
    int    field_0x47a;  // +0x47a
    int    field_0x47e;  // +0x47e
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
#pragma pack(pop)
SIZE(army, 0x482);
#endif // HOMM2_SOURCE_ARMY_H
