#ifndef HOMM2_SOURCE_TOWNMANAGER_H
#define HOMM2_SOURCE_TOWNMANAGER_H

#include <Ints.h>
#include <BASE/baseManager.h>

class heroWindow;
class icon;
class town;
class townObject;
class strip;
class armyGroup;
class bankBox;
class hero;
struct tag_message;

typedef enum TownManagerStorageConstant {
    TOWN_MANAGER_OBJECT_COUNT     = 32,
    TOWN_MANAGER_STATUS_TEXT_SIZE = 0x50
} TownManagerStorageConstant;

enum {
    ARMY_COMMAND_NONE      = -1,
    ARMY_COMMAND_SELECT    = 0,
    ARMY_COMMAND_VIEW      = 1,
    ARMY_COMMAND_MERGE     = 2,
    ARMY_COMMAND_SWAP      = 3,
    ARMY_COMMAND_VIEW_HERO = 4,
    ARMY_COMMAND_SPLIT     = 5
};
typedef i32 TownManagerArmyCommand;
#pragma pack(push, 1)
class townManager H2_FINAL : public baseManager {
public:
    town* m_town;
    icon* m_backgroundIcon;
    townObject* m_townObjects[TOWN_MANAGER_OBJECT_COUNT];
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
    char m_statusText[TOWN_MANAGER_STATUS_TEXT_SIZE];
    i32 m_lastHoverId;
    i32 m_lastHoverSubId;
    TownManagerArmyCommand m_command;
    b32 m_recruitResult;
    u32l m_affordableBuildings;
    u32l m_buildableBuildings;
    b32 m_castleDialogActive;
    i32 m_selectedBuilding;
    heroWindow* m_heroWindow0;
    heroWindow* m_heroWindow1;
    i32 m_splitAmount;
    i32 m_splitMaximum;
    i32 m_recruitState;
    hero* m_recruitHero;
    townManager(void);
    virtual i32 Open(i32 id) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;
    void SetupExtraStuff(void);
    void SetTown(class town* townData) {
        m_town = townData;
    }
    void ChangeTown(void);
    void SetupTown(void);
    void UnloadTown(void);
    void SetArmyCommand(i32 qualifier);
    void SetCommandAndText(struct tag_message& message);
    void ShowText(char*);
    void DoCommand(TownManagerArmyCommand command);
    void RedrawTownScreen(void);
    void SplitArmy(void);
    void ShiftQualChange(void);
    void ResetStrips(void);
    void Toggle(BuildingSlotType building);
    void DrawTown(i32 updateScreen, i32 drawFlags);
    i32 BuyBuild(BuildingSlotType building, i32 cannotBuy, i32 quickView);
    void BuildObj(BuildingSlotType building);
    void SetupMage(class heroWindow* window);
    i32 RecruitHero(i32 availableHeroIndex, i32 cannotRecruit);
    void DoTavern(void);
    void SetupWell(class heroWindow* window);
    void SetupThievesGuild(class heroWindow* window, i32 informationLevel);
    void SetupCastle(class heroWindow* window, i32 updateOnly);
};
#pragma pack(pop)
#endif
