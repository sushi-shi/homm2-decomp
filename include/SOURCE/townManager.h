#ifndef HOMM2_SOURCE_TOWNMANAGER_H
#define HOMM2_SOURCE_TOWNMANAGER_H
#include <va.h>
#include "../BASE/baseManager.h"
class heroWindow;
class icon;
class town;
class townObject;
class strip;
class armyGroup;
class bankBox;
class hero;
struct tag_message;

#pragma pack(push, 1)
class townManager : public baseManager {
public:
    town* m_town;
    icon* m_backgroundIcon;
    townObject* m_townObjects[32];
    i32 m_townObjectCount;
    i32 m_lastTownType;
    i32 m_unknownC6;
    heroWindow* m_townWindow;
    strip* m_garrisonStrip;
    strip* m_heroStrip;
    strip* m_selectedStrip;
    i32 m_selectedArmySlot;
    strip* m_swapStrip;
    i32 m_swapArmySlot;
    strip* m_pendingStrip;
    i32 m_pendingArmySlot;
    bankBox* m_bankBox;
    char m_statusText[0x50];
    i32 m_lastHoverId;
    i32 m_lastHoverSubId;
    i32 m_command;
    i32 m_recruitResult;
    u32l m_affordableBuildings;
    u32l m_buildableBuildings;
    i32 m_castleDialogActive;
    i32 m_selectedBuilding;
    heroWindow* m_heroWindow0;
    heroWindow* m_heroWindow1;
    i32 m_splitAmount;
    i32 m_splitMaximum;
    i32 m_recruitState;
    hero* m_recruitHero;
    townManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
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
#endif
