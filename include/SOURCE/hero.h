#ifndef HOMM2_SOURCE_HERO_H
#define HOMM2_SOURCE_HERO_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 34 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class town;

#pragma pack(push, 1)  // recovered layout is byte-packed
class hero {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    short  field_0x0;  // +0x00
    char   field_0x2;  // +0x02
    char   field_0x3;  // +0x03
    short  field_0x4;  // +0x04
    char _pad_0x6[0x1];
    short  field_0x7;  // +0x07
    char _pad_0x9[0x1];
    char   field_0xa;  // +0x0a
    char _pad_0xb[0xc];
    char   field_0x17;  // +0x17
    char   field_0x18;  // +0x18
    int    field_0x19;  // +0x19
    int    field_0x1d;  // +0x1d
    int    field_0x21;  // +0x21
    int    field_0x25;  // +0x25
    char _pad_0x29[0x4];
    short  field_0x2d;  // +0x2d
    short  field_0x2f;  // +0x2f
    char _pad_0x31[0x8];
    int    field_0x39;  // +0x39
    short  field_0x3d;  // +0x3d
    char _pad_0x3f[0x2];
    char   field_0x41;  // +0x41
    char _pad_0x42[0x2];
    char   field_0x44;  // +0x44
    char   field_0x45;  // +0x45
    char _pad_0x46[0x1d];
    char   field_0x63;  // +0x63
    char   field_0x64;  // +0x64
    char _pad_0x65[0x16];
    char   field_0x7b;  // +0x7b
    char _pad_0x7c[0x14];
    int    field_0x90;  // +0x90
    char _pad_0x94[0x4f];
    int    field_0xe3;  // +0xe3  !union: conflicting widths
    // size >= 0xe7 (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    hero(void);
    // --- methods ---
    void Read(int, signed char);
    void Write(int, signed char);
    void GetArmyStrengths(unsigned long int * const);
    int HasArtifact(int);
    int CalcMobility(void);
    int HasSpell(int);
    int GetNthSpell(int, int);
    int GetNumSpells(int);
    void UseSpell(int);
    void AddSpell(int, int);
    void HeroScreenUpdate(void);
    void UpdateArmies(void);
    void ViewStat(int, int);
    void ViewArtifact(int, int, int);
    int Dismiss(void);
    void Deallocate(int);
    int GetExperience(int);
    int GetLevel(int);
    void ApplyBattleWinTemps(void);
    void ApplyBattleLossTemps(void);
    void CheckLevel(void);
    int NumArtifacts(void);
    void SetSS(int, int);
    int TakeSS(int, int);
    int GiveSS(int, int);
    int CreatureTypeCount(int);
    void UpgradeCreatures(int, int);
    int GetNthSS(int);
    class town * GetOccupiedTown(void);
    signed char Stats(int);
    signed char GetSSLevel(int);
    void DoSSLevelDialog(int, int);
    void CheckAnduranPieces(int);
};
#pragma pack(pop)
// ---- globals (declarations, RVA order) ----
extern class hero *gpHVHero;
extern class heroWindow *gheroWin;
extern short *gMinExpForLevel;
extern int iOrigHeroViewID;
extern int gbNoDismiss;

#endif // HOMM2_SOURCE_HERO_H
