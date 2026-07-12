#ifndef HOMM2_SOURCE_TOWN_H
#define HOMM2_SOURCE_TOWN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class hero;

#pragma pack(push, 1)  // recovered layout is byte-packed
class town {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    char   field_0x0;  // +0x00
    char   field_0x1;  // +0x01
    char _pad_0x2[0x1];
    char   field_0x3;  // +0x03
    char   field_0x4;  // +0x04
    char   field_0x5;  // +0x05
    char   field_0x6;  // +0x06
    char _pad_0x7[0x10];
    char   field_0x17;  // +0x17
    int    field_0x18;  // +0x18  !union: conflicting widths
    char   field_0x1c;  // +0x1c
    char   field_0x1d;  // +0x1d
    char _pad_0x1e[0x14];
    short  field_0x32;  // +0x32
    short  field_0x34;  // +0x34
    // size >= 0x36 (accessed lower bound; no ctor/stride oracle)
    // --- constructors ---
    town(void);
    // --- methods ---
    int HasGarrison(void);
    void GiveSpells(class hero *);
    void XformToCastle(void);
    void View(int);
    void Deallocate(void);
    void BuildBuilding(int);
    int CanBuildDock(void);
    void CalcNumLevelArchers(int *, int *);
};
#pragma pack(pop)
// ---- globals (declarations, RVA order) ----
extern int bEnteringTown;

#endif // HOMM2_SOURCE_TOWN_H
