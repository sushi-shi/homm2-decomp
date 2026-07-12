#ifndef HOMM2_SOURCE_TOWNMANAGER_H
#define HOMM2_SOURCE_TOWNMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 26 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class heroWindow;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class townManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    int    field_0x36;  // +0x36
    int    field_0x3a;  // +0x3a
    char _pad_0x3e[0x80];
    int    field_0xbe;  // +0xbe
    int    field_0xc2;  // +0xc2
    int    field_0xc6;  // +0xc6
    int    field_0xca;  // +0xca
    int    field_0xce;  // +0xce
    int    field_0xd2;  // +0xd2
    int    field_0xd6;  // +0xd6
    int    field_0xda;  // +0xda
    int    field_0xde;  // +0xde
    int    field_0xe2;  // +0xe2
    int    field_0xe6;  // +0xe6
    int    field_0xea;  // +0xea
    int    field_0xee;  // +0xee
    char _pad_0xf2[0x50];
    int    field_0x142;  // +0x142
    int    field_0x146;  // +0x146
    int    field_0x14a;  // +0x14a
    int    field_0x14e;  // +0x14e
    int    field_0x152;  // +0x152
    int    field_0x156;  // +0x156
    int    field_0x15a;  // +0x15a
    int    field_0x15e;  // +0x15e
    heroWindow *m_heroWindow0;  // +0x162  hero portrait window (redrawn on recruit)
    heroWindow *m_heroWindow1;  // +0x166  hero portrait window (redrawn on recruit)
    int    field_0x16a;  // +0x16a
    int    field_0x16e;  // +0x16e
    int    m_recruitState;  // +0x172  recruit slot / selection state
    char  *m_recruitHeroRec;  // +0x176  selected hero record (byte [2] = hero id)
    // --- constructors ---
    townManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void SetupExtraStuff(void);
    void ChangeTown(void);
    void SetupTown(void);
    void UnloadTown(void);
    void SetArmyCommand(int);
    void SetCommandAndText(struct tag_message &);
    void ShowText(char *);
    void DoCommand(int);
    void RedrawTownScreen(void);
    void SplitArmy(void);
    void ShiftQualChange(void);
    void ResetStrips(void);
    void Toggle(int);
    void DrawTown(int, int);
    int BuyBuild(int, int, int);
    void BuildObj(int);
    void SetupMage(class heroWindow *);
    int RecruitHero(int, int);
    void DoTavern(void);
    void SetupWell(class heroWindow *);
    void SetupThievesGuild(class heroWindow *, int);
    void SetupCastle(class heroWindow *, int);
};
#pragma pack(pop)
SIZE(townManager, 0x17a);
// ---- globals (declarations, RVA order) ----
extern unsigned char *castleSlotsBase;
extern class heroWindow *casWin;
extern unsigned char *castleSlotsUse;

#endif // HOMM2_SOURCE_TOWNMANAGER_H
