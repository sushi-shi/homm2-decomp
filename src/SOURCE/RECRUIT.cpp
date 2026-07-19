#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <va.h>
#include <BASE/executive.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/mouseManager.h>
#include <BASE/widget.h>
#include <SOURCE/KB.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/RECRUIT.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>

H2_ENUM_BEGIN(RecruitConstant)
    RESOURCE_COUNT   = 6,
    GOLD_RESOURCE    = 6,
    SOURCE_EVENT     = -1,
    SOURCE_TOWN      = 0x23,
    WINDOW_X         = 0x8f,
    WINDOW_Y         = 0x10,
    QUICK_WINDOW_X   = 0xa0,
    QUICK_WINDOW_Y   = 0x10,
    NAME_SIZE        = 20,
    LABEL_SIZE       = 40,
    BROADCAST_FLAGS  = 0x4008,
    DRAW_DEPTH       = 0x7fff,
    VIEW_ARMY_X      = 0x77,
    VIEW_ARMY_Y      = 0x20,
    NO_ROOM_DIALOG_X = 177,
    NO_ROOM_DIALOG_Y = 100
H2_ENUM_END(RecruitConstant)

H2_ENUM_BEGIN(RecruitControl)
    TITLE_CONTROL          = 0x40,
    CREATURE_CONTROL       = 0x42,
    AVAILABLE_CONTROL      = 0x43,
    QUANTITY_CONTROL       = 0x44,
    INCREASE_CONTROL       = 0x45,
    DECREASE_CONTROL       = 0x46,
    MAXIMUM_CONTROL        = 0x47,
    GOLD_ICON_CONTROL      = 0x49,
    RESOURCE_ICON_CONTROL  = 0x4a,
    RESOURCE_COST_CONTROL  = 0x4b,
    GOLD_TOTAL_CONTROL     = 0x4d,
    RESOURCE_IMAGE_CONTROL = 0x4e,
    RESOURCE_TOTAL_CONTROL = 0x4f,
    CLOSE_CONTROL          = 0x7800,
    CANCEL_CONTROL         = 0x7801,
    CONFIRM_CONTROL        = 0x7802
H2_ENUM_END(RecruitControl)

VA(0x0048b310, 0x18c)
void SetupRecruitWin(
    class heroWindow* window,
    i32 creatureType,
    i32 goldCost,
    ResourceType resourceType,
    i32 resourceCost,
    i32 available
) {
    char creatureName[NAME_SIZE];
    char label[LABEL_SIZE];
    tag_message message;

    strcpy(creatureName, GetMonsterName(creatureType));
    creatureName[0] -= 'a' - 'A';
    sprintf(label, "%s %s", "Recruit", creatureName);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TITLE_CONTROL;
    message.payload.widget.data.text = label;
    window->BroadcastMessage(message);

    sprintf(label, "%d", goldCost);
    message.payload.widget.id = GOLD_ICON_CONTROL;
    window->BroadcastMessage(message);
    if (resourceType != RECRUIT_NO_RESOURCE) {
        sprintf(label, "%d", resourceCost);
        message.payload.widget.id = RESOURCE_COST_CONTROL;
        window->BroadcastMessage(message);
    }

    sprintf(gText, "%s%d", "Available: ", available);
    message.payload.widget.id = AVAILABLE_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    sprintf(gText, "monh%04d.icn", creatureType);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = CREATURE_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    if (resourceType != RECRUIT_NO_RESOURCE) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = RESOURCE_ICON_CONTROL;
        message.payload.widget.data.value = IDX(resourceType);
        window->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = RESOURCE_IMAGE_CONTROL;
        window->BroadcastMessage(message);
    }
}

VA(0x0048b49c, 0x24b)
i32 recruitUnit::Open(i32 priority) {
    i32 goldMaximum;
    i32 resourceMaximum;

    m_window = new heroWindow(
        WINDOW_X,
        WINDOW_Y,
        const_cast<char*>(m_resourceType == RECRUIT_NO_RESOURCE ? "recruit0.bin" : "recruit1.bin")
    );
    if (m_window == NULL)
        MemError();
    m_quantity = 0;
    m_goldTotal = 0;
    m_resourceTotal = 0;
    SetupRecruitWin(
        m_window,
        m_creatureType,
        m_goldCost,
        m_resourceType,
        m_resourceCost,
        *m_available
    );
    gpMouseManager->SetPointer("advmice.mse", 0, MOUSE_AUTO_CURSOR_TYPE);
    Update();
    gpWindowManager->BroadcastMessage(
        MESSAGE_WIDGET,
        WIDGET_COMMAND_SET_FLAGS,
        IDX(CLOSE_CONTROL),
        BROADCAST_FLAGS
    );
    gpWindowManager->AddWindow(m_window, -1, 1);

    goldMaximum = gpCurPlayer->m_resources[GOLD_RESOURCE] / m_goldCost;
    if (m_resourceType != RECRUIT_NO_RESOURCE) {
        resourceMaximum = gpCurPlayer->m_resources[IDX(m_resourceType)] / m_resourceCost;
        m_maximum = goldMaximum < resourceMaximum ? goldMaximum : resourceMaximum;
    } else
        m_maximum = goldMaximum;
    if (*m_available < m_maximum)
        m_maximum = *m_available;
    m_recruited = 0;
    m_noRoom = 0;
    if (*m_available == 0) {
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            IDX(CONFIRM_CONTROL),
            WIDGET_FLAG_ENABLED
        );
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_SET_FLAGS,
            IDX(CONFIRM_CONTROL),
            BROADCAST_FLAGS
        );
    }
    hmnuRecruitSave = hmnuCurrent;
    KBChangeMenu(hmnuDflt);
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = BaseManagerPriority(priority);
    m_active = true;
    strcpy(m_name, "recruitManager");
    return 0;
}

VA(0x0048b6e7, 0xe7)
void recruitUnit::Close(void) {
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    if (m_noRoom != 0) {
        NormalDialog(
            "There is no room in the garrison for this army.",
            NORMAL_DIALOG_INFO,
            NO_ROOM_DIALOG_X,
            NO_ROOM_DIALOG_Y,
            NORMAL_DIALOG_NO_RESOURCE,
            0,
            NORMAL_DIALOG_NO_RESOURCE,
            0,
            NORMAL_DIALOG_NO_RESOURCE,
            0
        );
    }
    gpWindowManager->BroadcastMessage(
        MESSAGE_WIDGET,
        WIDGET_COMMAND_CLEAR_FLAGS,
        IDX(CLOSE_CONTROL),
        BROADCAST_FLAGS
    );
    if (m_sourceType == SOURCE_TOWN && m_recruited != 0 && m_refreshTown != 0) {
        gpTownManager->ResetStrips();
        gpTownManager->m_bankBox->Update(1);
    }
    m_active = false;
    KBChangeMenu(hmnuRecruitSave);
}

VA(0x0048b7ce, 0x122)
void recruitUnit::Update(void) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;

    sprintf(gText, "%s%d", "Available: ", *m_available);
    message.payload.widget.id = AVAILABLE_CONTROL;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);
    sprintf(gText, "%d", m_quantity);
    message.payload.widget.id = QUANTITY_CONTROL;
    m_window->BroadcastMessage(message);
    m_goldTotal = m_quantity * m_goldCost;
    sprintf(gText, "%d", m_goldTotal);
    message.payload.widget.id = GOLD_TOTAL_CONTROL;
    m_window->BroadcastMessage(message);
    if (m_resourceType != RECRUIT_NO_RESOURCE) {
        m_resourceTotal = m_quantity * m_resourceCost;
        sprintf(gText, "%d", m_resourceTotal);
        message.payload.widget.id = RESOURCE_TOTAL_CONTROL;
        m_window->BroadcastMessage(message);
    }
}

VA(0x0048b8f0, 0x41b)
i32 recruitUnit::Main(struct tag_message& message) {
    i32 close = 0;
    i32 quickView = (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0;
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                switch (message.payload.widget.id) {
                    case IDX(QUANTITY_CONTROL):
                        if (quickView != 0)
                            break;
                        message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                        m_window->BroadcastMessage(message);
                        m_quantity = atoi(message.payload.widget.data.text);
                        if (m_quantity < 0)
                            m_quantity = 0;
                        if (m_quantity > m_maximum)
                            m_quantity = m_maximum;
                        break;
                    case IDX(CREATURE_CONTROL):
                        gpGame->ViewArmy(
                            VIEW_ARMY_X,
                            VIEW_ARMY_Y,
                            m_creatureType,
                            0,
                            NULL,
                            1,
                            1,
                            quickView,
                            NULL,
                            NULL,
                            NULL,
                            0
                        );
                        break;
                }
                Update();
                m_window->DrawWindow(1, 0, DRAW_DEPTH);
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case IDX(INCREASE_CONTROL):
                        if (quickView != 0)
                            break;
                        ++m_quantity;
                        if (m_quantity > m_maximum)
                            m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case IDX(DECREASE_CONTROL):
                        if (quickView != 0)
                            break;
                        --m_quantity;
                        if (m_quantity < 0)
                            m_quantity = 0;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case IDX(MAXIMUM_CONTROL):
                        if (quickView != 0)
                            break;
                        m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case IDX(CANCEL_CONTROL):
                        if (quickView != 0)
                            break;
                        m_quantity = 0;
                        close = 1;
                        break;
                    case IDX(CONFIRM_CONTROL):
                        if (quickView != 0)
                            break;
                        if (m_quantity == 0) {
                            close = 1;
                            break;
                        }
                        if (m_army->CanJoin(m_creatureType) != 0) {
                            m_army->Add(m_creatureType, m_quantity, ARMY_GROUP_EMPTY_SLOT);
                        } else {
                            close = 1;
                            m_noRoom = 1;
                            break;
                        }
                        gpCurPlayer->m_resources[GOLD_RESOURCE] -= m_quantity * m_goldCost;
                        if (m_resourceType != RECRUIT_NO_RESOURCE) {
                            gpCurPlayer->m_resources[IDX(m_resourceType)] -=
                                m_quantity * m_resourceCost;
                        }
                        *m_available -= m_quantity;
                        m_recruited = 1;
                        close = 1;
                        break;
                }
                break;
        }

        if (close == 1) {
            message.type = MESSAGE_EXECUTIVE;
            message.payload.executive.command = EXECUTIVE_COMMAND_RETURN_RESULT;
            return WIDGET_DISPATCH_FORWARD;
        }
    }
    return WIDGET_DISPATCH_CONSUME;
}

VA(0x0048bd0b, 0xdf)
recruitUnit::recruitUnit(class armyGroup* army, i32 creatureType, i16* available) {
    i32 costs[RESOURCE_COUNT + 1];
    i32 resource;

    m_sourceType = SOURCE_EVENT;
    m_refreshTown = 0;
    m_army = army;
    m_creatureType = creatureType;
    m_available = available;
    GetMonsterCost(m_creatureType, costs);
    m_goldCost = costs[GOLD_RESOURCE];
    for (resource = 0; resource < RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RESOURCE_COUNT) {
        m_resourceType = ResourceType(resource);
        m_resourceCost = costs[IDX(m_resourceType)];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

VA(0x0048bdea, 0xfb)
recruitUnit::recruitUnit(class town* townData, i32 dwelling, i32 refreshTown) {
    i32 costs[RESOURCE_COUNT + 1];
    i32 resource;

    m_refreshTown = refreshTown;
    m_sourceType = SOURCE_TOWN;
    m_army = &townData->m_army;
    m_creatureType = gDwellingType[townData->m_type][dwelling];
    m_available = &townData->m_garrison[dwelling];
    GetMonsterCost(m_creatureType, costs);
    m_goldCost = costs[GOLD_RESOURCE];
    for (resource = 0; resource < RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RESOURCE_COUNT) {
        m_resourceType = ResourceType(resource);
        m_resourceCost = costs[IDX(m_resourceType)];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

VA(0x0048bee5, 0x14f)
void QuickViewRecruit(class town* townData, i32 dwelling) {
    i32 costs[RESOURCE_COUNT + 1];
    i32 goldCost;
    ResourceType resourceType;
    i32 creatureType;
    i32 resourceCost;
    heroWindow* window;
    i32 available;
    i32 resource;

    creatureType = gDwellingType[townData->m_type][dwelling];
    available = townData->m_garrison[dwelling];
    GetMonsterCost(creatureType, costs);
    goldCost = costs[GOLD_RESOURCE];
    for (resource = 0; resource < RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RESOURCE_COUNT) {
        resourceType = ResourceType(resource);
        resourceCost = costs[resource];
    } else {
        resourceType = RECRUIT_NO_RESOURCE;
        resourceCost = 0;
    }

    window = new heroWindow(
        QUICK_WINDOW_X,
        QUICK_WINDOW_Y,
        const_cast<char*>(resourceType == RECRUIT_NO_RESOURCE ? "recruiq0.bin" : "recruiq1.bin")
    );
    if (window == NULL)
        MemError();
    SetupRecruitWin(window, creatureType, goldCost, resourceType, resourceCost, available);
    gpWindowManager->AddWindow(window, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(window);
}


VTBL(recruitUnit, 0x004eb848);

DATA(0x00528578) HMENU hmnuRecruitSave;
