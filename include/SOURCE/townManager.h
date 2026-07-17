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

#pragma pack(push, 1) // recovered layout is byte-packed
class townManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    town* m_town;                  // +0x36
    icon* m_backgroundIcon;        // +0x3a
    townObject* m_townObjects[32]; // +0x3e
    i32 m_townObjectCount;         // +0xbe
    i32 m_lastTownType;            // +0xc2
    i32 m_unknownC6; // +0xc6 only zeroed (ctor/Open), never read; unnamed and unused in Ironfist (field_C6) too
    heroWindow* m_townWindow;   // +0xca
    strip* m_garrisonStrip;     // +0xce
    strip* m_heroStrip;         // +0xd2
    strip* m_selectedStrip;     // +0xd6
    i32 m_selectedArmySlot;     // +0xda
    strip* m_swapStrip;         // +0xde
    i32 m_swapArmySlot;         // +0xe2
    strip* m_pendingStrip;      // +0xe6
    i32 m_pendingArmySlot;      // +0xea
    bankBox* m_bankBox;         // +0xee
    char m_statusText[0x50];    // +0xf2
    i32 m_lastHoverId;          // +0x142
    i32 m_lastHoverSubId;       // +0x146
    i32 m_command;              // +0x14a
    i32 m_recruitResult;        // +0x14e
    u32l m_affordableBuildings; // +0x152
    u32l m_buildableBuildings;  // +0x156
    i32 m_castleDialogActive;   // +0x15a
    i32 m_selectedBuilding;     // +0x15e
    heroWindow* m_heroWindow0;  // +0x162  hero portrait window (redrawn on recruit)
    heroWindow* m_heroWindow1;  // +0x166  hero portrait window (redrawn on recruit)
    i32 m_splitAmount;          // +0x16a
    i32 m_splitMaximum;         // +0x16e
    i32 m_recruitState;         // +0x172  recruit slot / selection state
    hero* m_recruitHero;        // +0x176
    // --- constructors ---
    townManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void SetupExtraStuff(void);
    void SetTown(class town* townData) {
        m_town = townData;
    }
    void ChangeTown(void);
    void SetupTown(void);
    void UnloadTown(void);
    void SetArmyCommand(i32);
    void SetCommandAndText(struct tag_message&);
    void ShowText(char*);
    void DoCommand(i32);
    void RedrawTownScreen(void);
    void SplitArmy(void);
    void ShiftQualChange(void);
    void ResetStrips(void);
    void Toggle(i32);
    void DrawTown(i32, i32);
    i32 BuyBuild(i32, i32, i32);
    void BuildObj(i32);
    void SetupMage(class heroWindow*);
    i32 RecruitHero(i32, i32);
    void DoTavern(void);
    void SetupWell(class heroWindow*);
    void SetupThievesGuild(class heroWindow*, i32);
    void SetupCastle(class heroWindow*, i32);
};
#pragma pack(pop)
SIZE(townManager, 0x17a);
#endif // HOMM2_SOURCE_TOWNMANAGER_H
