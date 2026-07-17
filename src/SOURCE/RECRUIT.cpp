// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\RECRUIT.OBJ   from: (directly linked into exe)
// functions: 8   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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

// @early-stop
// The complete 0x60 frame, seven broadcast sites, optional resource arm, and
// relocation-masked instruction stream agree. Manual object review confirms
// all 25/25 relocation occurrences and targets; the reported 99.71% residual
// is delinked pooled-string identity (the helper also mislabels two gText uses).
VA(0x0048b310, 0x18c)
void SetupRecruitWin(
    class heroWindow* window,
    i32 creatureType,
    i32 goldCost,
    i32 resourceType,
    i32 resourceCost,
    i32 available
) {
    char creatureName[RECRUIT_NAME_SIZE];
    char label[RECRUIT_LABEL_SIZE];
    tag_message message;

    strcpy(creatureName, GetMonsterName(creatureType));
    creatureName[0] -= 'a' - 'A';
    sprintf(label, "%s %s", "Recruit", creatureName);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = RECRUIT_TITLE_CONTROL;
    message.payload.widget.data.text = label;
    window->BroadcastMessage(message);

    sprintf(label, "%d", goldCost);
    message.payload.widget.id = RECRUIT_GOLD_ICON_CONTROL;
    window->BroadcastMessage(message);
    if (resourceType != RECRUIT_NO_RESOURCE) {
        sprintf(label, "%d", resourceCost);
        message.payload.widget.id = RECRUIT_RESOURCE_COST_CONTROL;
        window->BroadcastMessage(message);
    }

    sprintf(gText, "%s%d", "Available: ", available);
    message.payload.widget.id = RECRUIT_AVAILABLE_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);

    sprintf(gText, "monh%04d.icn", creatureType);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_ICON;
    message.payload.widget.id = RECRUIT_CREATURE_CONTROL;
    message.payload.widget.data.text = gText;
    window->BroadcastMessage(message);
    if (resourceType != RECRUIT_NO_RESOURCE) {
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = RECRUIT_RESOURCE_ICON_CONTROL;
        message.payload.widget.data.value = resourceType;
        window->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = RECRUIT_RESOURCE_IMAGE_CONTROL;
        window->BroadcastMessage(message);
    }
}

// @early-stop
// The recovered 0x10 frame, affordability locals/minimum expression, CFG, and
// all non-relocation instructions are exact. All 26/26 relocation occurrences
// resolve to the same owners/addends; only delinked pooled-string identities differ.
VA(0x0048b49c, 0x24b)
i32 recruitUnit::Open(i32 priority) {
    i32 goldMaximum;
    i32 resourceMaximum;

    m_window = new heroWindow(
        RECRUIT_WINDOW_X,
        RECRUIT_WINDOW_Y,
        const_cast<char*>(m_resourceType == RECRUIT_NO_RESOURCE ? "recruit0.bin" : "recruit1.bin")
    );
    if (m_window == 0)
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
        RECRUIT_CLOSE_CONTROL,
        RECRUIT_BROADCAST_FLAGS
    );
    gpWindowManager->AddWindow(m_window, -1, 1);

    goldMaximum = gpCurPlayer->m_resources[RECRUIT_GOLD_RESOURCE] / m_goldCost;
    if (m_resourceType != RECRUIT_NO_RESOURCE) {
        resourceMaximum = gpCurPlayer->m_resources[m_resourceType] / m_resourceCost;
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
            RECRUIT_CONFIRM_CONTROL,
            2
        );
        gpWindowManager->BroadcastMessage(
            MESSAGE_WIDGET,
            WIDGET_COMMAND_SET_FLAGS,
            RECRUIT_CONFIRM_CONTROL,
            RECRUIT_BROADCAST_FLAGS
        );
    }
    hmnuRecruitSave = hmnuCurrent;
    KBChangeMenu(hmnuDflt);
    m_messageMask = RECRUIT_MANAGER_MASK;
    m_priority = priority;
    m_active = 1;
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
            177,
            100,
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
        RECRUIT_CLOSE_CONTROL,
        RECRUIT_BROADCAST_FLAGS
    );
    if (m_sourceType == RECRUIT_SOURCE_TOWN && m_recruited != 0 && m_refreshTown != 0) {
        gpTownManager->ResetStrips();
        gpTownManager->m_bankBox->Update(1);
    }
    m_active = 0;
    KBChangeMenu(hmnuRecruitSave);
}

// @early-stop
// The complete 0x38 frame, totals/update CFG, and relocation-masked instruction
// stream agree. Manual object review confirms all 18/18 relocation occurrences
// and targets; 99.67% is pooled-string identity (the helper mislabels one gText).
VA(0x0048b7ce, 0x122)
void recruitUnit::Update(void) {
    tag_message message;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;

    sprintf(gText, "%s%d", "Available: ", *m_available);
    message.payload.widget.id = RECRUIT_AVAILABLE_CONTROL;
    message.payload.widget.data.text = gText;
    m_window->BroadcastMessage(message);
    sprintf(gText, "%d", m_quantity);
    message.payload.widget.id = RECRUIT_QUANTITY_CONTROL;
    m_window->BroadcastMessage(message);
    m_goldTotal = m_quantity * m_goldCost;
    sprintf(gText, "%d", m_goldTotal);
    message.payload.widget.id = RECRUIT_GOLD_TOTAL_CONTROL;
    m_window->BroadcastMessage(message);
    if (m_resourceType != RECRUIT_NO_RESOURCE) {
        m_resourceTotal = m_quantity * m_resourceCost;
        sprintf(gText, "%d", m_resourceTotal);
        message.payload.widget.id = RECRUIT_RESOURCE_TOTAL_CONTROL;
        m_window->BroadcastMessage(message);
    }
}

// @early-stop
// Jump-only proof: the 0x3fd-byte candidate versus the 0x41b-byte retail span
// differs by exactly six five-byte continuation JMPs (0x1e bytes). Every
// non-jump opcode/operand and all 16/16 ordered external relocations match; this
// function has no embedded jump table. Guard-clause cases, confirmation shared
// tails, right-button quick view, frame/slots, and final close==1 CFG are recovered.
VA(0x0048b8f0, 0x41b)
i32 recruitUnit::Main(struct tag_message& message) {
    i32 close = 0;
    i32 quickView = (message.payload.widget.parameter & MESSAGE_MODIFIER_RIGHT_BUTTON) != 0;
    if (message.type == MESSAGE_WIDGET) {
        switch (message.payload.widget.command) {
            case WIDGET_COMMAND_SELECT:
            case WIDGET_COMMAND_ALTERNATE_SELECT:
                switch (message.payload.widget.id) {
                    case RECRUIT_QUANTITY_CONTROL:
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
                    case RECRUIT_CREATURE_CONTROL:
                        gpGame->ViewArmy(
                            RECRUIT_VIEW_ARMY_X,
                            RECRUIT_VIEW_ARMY_Y,
                            m_creatureType,
                            0,
                            0,
                            1,
                            1,
                            quickView,
                            0,
                            0,
                            0,
                            0
                        );
                        break;
                }
                Update();
                m_window->DrawWindow(1, 0, RECRUIT_DRAW_DEPTH);
                break;
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                    case RECRUIT_INCREASE_CONTROL:
                        if (quickView != 0)
                            break;
                        ++m_quantity;
                        if (m_quantity > m_maximum)
                            m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, RECRUIT_DRAW_DEPTH);
                        break;
                    case RECRUIT_DECREASE_CONTROL:
                        if (quickView != 0)
                            break;
                        --m_quantity;
                        if (m_quantity < 0)
                            m_quantity = 0;
                        Update();
                        m_window->DrawWindow(1, 0, RECRUIT_DRAW_DEPTH);
                        break;
                    case RECRUIT_MAXIMUM_CONTROL:
                        if (quickView != 0)
                            break;
                        m_quantity = m_maximum;
                        Update();
                        m_window->DrawWindow(1, 0, RECRUIT_DRAW_DEPTH);
                        break;
                    case RECRUIT_CANCEL_CONTROL:
                        if (quickView != 0)
                            break;
                        m_quantity = 0;
                        close = 1;
                        break;
                    case RECRUIT_CONFIRM_CONTROL:
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
                        gpCurPlayer->m_resources[RECRUIT_GOLD_RESOURCE] -= m_quantity * m_goldCost;
                        if (m_resourceType != RECRUIT_NO_RESOURCE) {
                            gpCurPlayer->m_resources[m_resourceType] -= m_quantity * m_resourceCost;
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
            return 2;
        }
    }
    return 1;
}

VA(0x0048bd0b, 0xdf)
recruitUnit::recruitUnit(class armyGroup* army, i32 creatureType, i16* available) {
    i32 costs[RECRUIT_RESOURCE_COUNT + 1];
    i32 resource;

    m_sourceType = RECRUIT_SOURCE_EVENT;
    m_refreshTown = 0;
    m_army = army;
    m_creatureType = creatureType;
    m_available = available;
    GetMonsterCost(m_creatureType, costs);
    m_goldCost = costs[RECRUIT_GOLD_RESOURCE];
    for (resource = 0; resource < RECRUIT_RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RECRUIT_RESOURCE_COUNT) {
        m_resourceType = resource;
        m_resourceCost = costs[m_resourceType];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

VA(0x0048bdea, 0xfb)
recruitUnit::recruitUnit(class town* townData, i32 dwelling, i32 refreshTown) {
    i32 costs[RECRUIT_RESOURCE_COUNT + 1];
    i32 resource;

    m_refreshTown = refreshTown;
    m_sourceType = RECRUIT_SOURCE_TOWN;
    m_army = &townData->m_army;
    m_creatureType = gDwellingType[townData->m_type][dwelling];
    m_available = &townData->m_garrison[dwelling];
    GetMonsterCost(m_creatureType, costs);
    m_goldCost = costs[RECRUIT_GOLD_RESOURCE];
    for (resource = 0; resource < RECRUIT_RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RECRUIT_RESOURCE_COUNT) {
        m_resourceType = resource;
        m_resourceCost = costs[m_resourceType];
    } else {
        m_resourceType = RECRUIT_NO_RESOURCE;
        m_resourceCost = 0;
    }
}

// @early-stop
// The complete 0x3c frame, cost search, window lifecycle, and relocation-masked
// instruction stream agree. All 13/13 relocation occurrences resolve to the
// same targets; the 99.68% residual is the two pooled window-name symbols.
VA(0x0048bee5, 0x14f)
void QuickViewRecruit(class town* townData, i32 dwelling) {
    i32 costs[RECRUIT_RESOURCE_COUNT + 1];
    i32 goldCost;
    i32 resourceType;
    i32 creatureType;
    i32 resourceCost;
    heroWindow* window;
    i32 available;
    i32 resource;

    creatureType = gDwellingType[townData->m_type][dwelling];
    available = townData->m_garrison[dwelling];
    GetMonsterCost(creatureType, costs);
    goldCost = costs[RECRUIT_GOLD_RESOURCE];
    for (resource = 0; resource < RECRUIT_RESOURCE_COUNT; ++resource) {
        if (costs[resource] != 0)
            break;
    }
    if (resource < RECRUIT_RESOURCE_COUNT) {
        resourceType = resource;
        resourceCost = costs[resource];
    } else {
        resourceType = RECRUIT_NO_RESOURCE;
        resourceCost = 0;
    }

    window = new heroWindow(
        RECRUIT_QUICK_WINDOW_X,
        RECRUIT_QUICK_WINDOW_Y,
        const_cast<char*>(resourceType == RECRUIT_NO_RESOURCE ? "recruiq0.bin" : "recruiq1.bin")
    );
    if (window == 0)
        MemError();
    SetupRecruitWin(window, creatureType, goldCost, resourceType, resourceCost, available);
    gpWindowManager->AddWindow(window, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(window);
}

// ===== vtable recruitUnit : public baseManager  (3 slots) =====
//  [ 0] VA(0x0048b49c, 0x24b)  int recruitUnit::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x0048b6e7, 0xe7)  void recruitUnit::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0048b8f0, 0x41b)  int recruitUnit::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(recruitUnit, 0x004eb848);

// ---- globals (definitions, RVA order) ----
DATA(0x00528578) HMENU hmnuRecruitSave;
