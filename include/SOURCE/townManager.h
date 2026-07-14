#ifndef HOMM2_SOURCE_TOWNMANAGER_H
#define HOMM2_SOURCE_TOWNMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 26 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class heroWindow;
class icon;
class town;
class townObject;
class strip;
class armyGroup;
class bankBox;
class hero;
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class townManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    town *m_town;  // +0x36
    icon *m_backgroundIcon;  // +0x3a
    townObject *m_townObjects[32];  // +0x3e
    int m_townObjectCount;  // +0xbe
    int m_lastTownType;  // +0xc2
    int m_unknownC6;  // +0xc6
    heroWindow *m_townWindow;  // +0xca
    strip *m_garrisonStrip;  // +0xce
    strip *m_heroStrip;  // +0xd2
    strip *m_selectedStrip;  // +0xd6
    int m_selectedArmySlot;  // +0xda
    strip *m_swapStrip;  // +0xde
    int m_swapArmySlot;  // +0xe2
    strip *m_pendingStrip;  // +0xe6
    int m_pendingArmySlot;  // +0xea
    bankBox *m_bankBox;  // +0xee
    char m_statusText[0x50];  // +0xf2
    int m_lastHoverId;  // +0x142
    int m_lastHoverSubId;  // +0x146
    int m_command;  // +0x14a
    int m_recruitResult;  // +0x14e
    unsigned long m_affordableBuildings;  // +0x152
    unsigned long m_buildableBuildings;  // +0x156
    int m_castleDialogActive;  // +0x15a
    int m_selectedBuilding;  // +0x15e
    heroWindow *m_heroWindow0;  // +0x162  hero portrait window (redrawn on recruit)
    heroWindow *m_heroWindow1;  // +0x166  hero portrait window (redrawn on recruit)
    int m_splitAmount;  // +0x16a
    int m_splitMaximum;  // +0x16e
    int    m_recruitState;  // +0x172  recruit slot / selection state
    hero *m_recruitHero;  // +0x176
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
#endif // HOMM2_SOURCE_TOWNMANAGER_H
