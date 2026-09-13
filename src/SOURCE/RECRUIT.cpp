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
    RESOURCE_COUNT = 6,
    GOLD_RESOURCE = 6,
    WINDOW_X = 0x8f,
    WINDOW_Y = 0x10,
    QUICK_WINDOW_X = 0xa0,
    QUICK_WINDOW_Y = 0x10,
    NAME_SIZE = 40,
    LABEL_SIZE = 40,
    BROADCAST_FLAGS = 0x4008,
    DRAW_DEPTH = 0x7fff,
    VIEW_ARMY_X = 0x77,
    VIEW_ARMY_Y = 0x20,
    NO_ROOM_DIALOG_X = 177,
    NO_ROOM_DIALOG_Y = 100
H2_ENUM_END(RecruitConstant)

H2_ENUM_BEGIN(RecruitControl)
    TITLE_CONTROL = 0x40,
    CREATURE_CONTROL = 0x42,
    AVAILABLE_CONTROL = 0x43,
    QUANTITY_CONTROL = 0x44,
    INCREASE_CONTROL = 0x45,
    DECREASE_CONTROL = 0x46,
    MAXIMUM_CONTROL = 0x47,
    GOLD_ICON_CONTROL = 0x49,
    RESOURCE_ICON_CONTROL = 0x4a,
    RESOURCE_COST_CONTROL = 0x4b,
    GOLD_TOTAL_CONTROL = 0x4d,
    RESOURCE_IMAGE_CONTROL = 0x4e,
    RESOURCE_TOTAL_CONTROL = 0x4f,
    CLOSE_CONTROL = 0x7800,
    CANCEL_CONTROL = 0x7801,
    CONFIRM_CONTROL = 0x7802
H2_ENUM_END(RecruitControl)

VA(0x0048c330, 0x1f2)
void SetupRecruitWin(
    class heroWindow* window,
    H2_ENUM_PARAM(CreatureType, i32) creatureType,
    i32 goldCost,
    ResourceType resourceType,
    i32 resourceCost,
    i32 available
) {
    char ch;
    char monsterName[NAME_SIZE];
    char label[LABEL_SIZE];
    tag_message message;

    strcpy(monsterName, GetMonsterPluralName(creatureType));
    /* CP1251: fold the leading letter to lower case ('A'-'Z', '\xc0'-'\xdf' and '\xa8'). */
    if (static_cast<u8>(monsterName[0]) >= 'A' && static_cast<u8>(monsterName[0]) <= 'Z')
        ch = static_cast<char>(static_cast<u8>(monsterName[0]) + 0x20);
    else if (static_cast<u8>(monsterName[0]) >= 0xc0 && static_cast<u8>(monsterName[0]) <= 0xdf)
        ch = static_cast<char>(static_cast<u8>(monsterName[0]) + 0x20);
    else if (static_cast<u8>(monsterName[0]) == 0xa8)
        ch = static_cast<char>(0xb8);
    else
        ch = monsterName[0];
    monsterName[0] = ch;
    sprintf(label, "%s %s", "\xcd\xe0\xed\xff\xf2\xfc" /* "Нанять" */, monsterName);
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

    sprintf(gText, "%s%d", "\xc4\xee\xf1\xf2\xf3\xef\xed\xee: " /* "Доступно: " */, available);
    message.payload.widget.id = AVAILABLE_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    sprintf(gText, "monh%04d.icn", IDX(creatureType));
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

VA(0x0048c522, 0x26b)
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
    if (m_maximum > *m_available)
        m_maximum = *m_available;
    m_recruited = false;
    m_noRoom = false;
    if (*m_available == 0) {
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_CLEAR_FLAGS,
            CONFIRM_CONTROL,
            IDX(WIDGET_FLAG_ENABLED)
        );
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_SET_FLAGS,
            CONFIRM_CONTROL,
            BROADCAST_FLAGS
        );
    }
    hmnuRecruitSave = hmnuCurrent;
    KBChangeMenu(hmnuDflt);
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "recruitManager");
    return 0;
}

VA(0x0048c78d, 0xc8)
void recruitUnit::Close(void) {
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    if (m_noRoom != 0) {
        NormalDialog(
            "\xc4\xeb\xff \xfd\xf2\xee\xe9 \xe0\xf0\xec\xe8\xe8 \xed\xe5\xf2 \xec\xe5\xf1\xf2\xe0 "
            "\xe2 \xe3\xe0\xf0\xed\xe8\xe7\xee\xed\xe5." /* "Для этой армии нет места в гарнизоне." */
            ,
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
    if (m_sourceType == RECRUIT_SOURCE_TOWN && m_recruited != 0 && m_refreshTown != 0) {
        gpTownManager->ResetStrips();
        gpTownManager->m_bankBox->Update(1);
    }
    m_active = false;
    KBChangeMenu(hmnuRecruitSave);
}

VA(0x0048c855, 0x115)
void recruitUnit::Update(void) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;

    sprintf(gText, "%s%d", "\xc4\xee\xf1\xf2\xf3\xef\xed\xee: " /* "Доступно: " */, *m_available);
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

VA(0x0048c96a, 0x39d)
MessageDispatchResult recruitUnit::Main(struct tag_message& message) {
    b32 done = false;
    b32 quickView;
    i32 H2_UNUSED(cost);

    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
        quickView = true;
    else
        quickView = false;
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                switch (message.payload.widget.id) {
                    case QUANTITY_CONTROL:
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
                    case CREATURE_CONTROL:
                        gpGame->ViewArmy(
                            VIEW_ARMY_X,
                            VIEW_ARMY_Y,
                            m_creatureType,
                            0,
                            NULL,
                            1,
                            ARMY_FACING_RIGHT,
                            quickView,
                            NULL,
                            NULL,
                            NULL,
                            0
                        );
                        break;
                    default:
                        break;
                }
                Update();
                m_window->DrawWindow(1, 0, DRAW_DEPTH);
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case INCREASE_CONTROL:
                        if (quickView != 0)
                            break;
                        ++m_quantity;
                        if (m_quantity > m_maximum)
                            m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case DECREASE_CONTROL:
                        if (quickView != 0)
                            break;
                        --m_quantity;
                        if (m_quantity < 0)
                            m_quantity = 0;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case MAXIMUM_CONTROL:
                        if (quickView != 0)
                            break;
                        m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, DRAW_DEPTH);
                        break;
                    case CANCEL_CONTROL:
                        if (quickView != 0)
                            break;
                        m_quantity = 0;
                        done = true;
                        break;
                    case CONFIRM_CONTROL:
                        if (quickView != 0)
                            break;
                        if (m_quantity == 0) {
                            done = true;
                            goto checkClose;
                        }
                        if (m_army->CanJoin(m_creatureType) != 0) {
                            m_army->Add(m_creatureType, m_quantity, ARMY_GROUP_EMPTY_SLOT);
                        } else {
                            done = true;
                            m_noRoom = true;
                            goto checkClose;
                        }
                        gpCurPlayer->m_resources[GOLD_RESOURCE] -= m_quantity * m_goldCost;
                        if (m_resourceType != RECRUIT_NO_RESOURCE) {
                            gpCurPlayer->m_resources[IDX(m_resourceType)] -=
                                m_quantity * m_resourceCost;
                        }
                        *m_available -= m_quantity;
                        m_recruited = true;
                        done = true;
                        break;
                }
                break;
            default:
                break;
        }

    checkClose:
        if (done == 1) {
            message.type = MESSAGE_EXECUTIVE;
            message.payload.executive.command = EXECUTIVE_COMMAND_RETURN_RESULT;
            return MESSAGE_DISPATCH_FORWARD;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0048cd07, 0xc4)
recruitUnit::recruitUnit(class armyGroup* army, CreatureType creatureType, i16* available) {
    i32 unitCosts[RESOURCE_COUNT + 1];
    i32 resourceIndex;

    m_sourceType = RECRUIT_SOURCE_EVENT;
    m_refreshTown = 0;
    m_army = army;
    m_creatureType = creatureType;
    m_available = available;
    GetMonsterCost(m_creatureType, unitCosts);
    m_goldCost = unitCosts[GOLD_RESOURCE];
    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; ++resourceIndex) {
        if (unitCosts[resourceIndex] != 0)
            break;
    }
    if (resourceIndex < RESOURCE_COUNT) {
        m_resourceType = ResourceType(resourceIndex);
        m_resourceCost = unitCosts[IDX(m_resourceType)];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

VA(0x0048cdcb, 0xdf)
recruitUnit::recruitUnit(class town* townData, i32 dwelling, i32 refreshTown) {
    i32 unitCosts[RESOURCE_COUNT + 1];
    i32 resourceIndex;

    m_refreshTown = refreshTown;
    m_sourceType = RECRUIT_SOURCE_TOWN;
    m_army = &townData->m_army;
    m_creatureType = gDwellingType[IDX(townData->m_type)][dwelling];
    m_available = &townData->m_garrison[dwelling];
    GetMonsterCost(m_creatureType, unitCosts);
    m_goldCost = unitCosts[GOLD_RESOURCE];
    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; ++resourceIndex) {
        if (unitCosts[resourceIndex] != 0)
            break;
    }
    if (resourceIndex < RESOURCE_COUNT) {
        m_resourceType = ResourceType(resourceIndex);
        m_resourceCost = unitCosts[IDX(m_resourceType)];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

VA(0x0048ceaa, 0x161)
void QuickViewRecruit(class town* townData, i32 dwelling) {
    CreatureType monsterType;
    ResourceType resourceType;
    heroWindow* recruitWindow;
    i32 unitCosts[RESOURCE_COUNT + 1];
    i32 resourceCost;
    i32 goldCost;
    i32 resourceIndex;
    i32 avail;

    monsterType = gDwellingType[IDX(townData->m_type)][dwelling];
    avail = townData->m_garrison[dwelling];
    GetMonsterCost(monsterType, unitCosts);
    goldCost = unitCosts[GOLD_RESOURCE];
    for (resourceIndex = 0; resourceIndex < RESOURCE_COUNT; ++resourceIndex) {
        if (unitCosts[resourceIndex] != 0)
            break;
    }
    if (resourceIndex < RESOURCE_COUNT) {
        resourceType = ResourceType(resourceIndex);
        resourceCost = unitCosts[IDX(resourceType)];
    } else {
        resourceType = RECRUIT_NO_RESOURCE;
        resourceCost = 0;
    }

    recruitWindow = new heroWindow(
        QUICK_WINDOW_X,
        QUICK_WINDOW_Y,
        const_cast<char*>(resourceType == RECRUIT_NO_RESOURCE ? "recruiq0.bin" : "recruiq1.bin")
    );
    if (recruitWindow == NULL)
        MemError();
    SetupRecruitWin(recruitWindow, monsterType, goldCost, resourceType, resourceCost, avail);
    gpWindowManager->AddWindow(recruitWindow, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(recruitWindow);
}

DATA(0x00533204) HMENU hmnuRecruitSave;

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(recruitUnit, 0x004ea7f4)
