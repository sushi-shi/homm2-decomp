// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\STRIP.OBJ   from: (directly linked into exe)
// functions: 8   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <BASE/border.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/bankBox.h>
#include <SOURCE/playerData.h>
#include <SOURCE/strip.h>

VA(0x00432230, 0x27e)
strip::strip(int x, int y, int stripType, unsigned long portraitIconId,
             int portraitFrame, class armyGroup *army, int firstBorderId,
             int drawWindow, int flagFrame)
{
    m_selectedSlot = ARMY_GROUP_EMPTY_SLOT;
    m_x = x;
    m_y = y;
    m_stripType = stripType;
    m_portraitIcon = gpResourceManager->GetIcon(portraitIconId);
    m_portraitFrame = portraitFrame;
    m_army = army;
    m_stripIcon = gpResourceManager->GetIcon("strip.icn");

    for (int slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
        m_creatureIcons[slot] = 0;
        m_cachedCreatureTypes[slot] = ARMY_GROUP_EMPTY_SLOT;
    }

    m_flagIcon = gpResourceManager->GetIcon("portcflg.icn");
    m_flagFrame = flagFrame;
    m_window = new heroWindow(m_x, m_y, STRIP_WINDOW_WIDTH,
                              STRIP_WINDOW_HEIGHT, STRIP_WINDOW_FLAGS);
    if (m_window == 0)
        MemError();

    if (m_army != 0) {
        m_borders[0] = new border(
            STRIP_PORTRAIT_X, STRIP_CONTENT_Y, STRIP_PORTRAIT_BORDER_WIDTH,
            STRIP_BORDER_HEIGHT, firstBorderId,
            STRIP_BORDER_FILL_COLOR, 0, 0);
        if (m_borders[0] == 0)
            MemError();
        m_window->AddWidget(m_borders[0], -1);

        for (int slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
            m_borders[slot + 1] = new border(
                slot * STRIP_ARMY_X_STEP + STRIP_ARMY_FIRST_X,
                STRIP_CONTENT_Y, STRIP_ARMY_BORDER_WIDTH,
                STRIP_BORDER_HEIGHT, 0[&slot] + firstBorderId + 1,
                STRIP_BORDER_FILL_COLOR, 0, 0);
            if (m_borders[slot + 1] == 0)
                MemError();
            m_window->AddWidget(m_borders[slot + 1], -1);
        }
    }

    DrawIcons(drawWindow);
    gpWindowManager->AddWindow(m_window, -1, drawWindow);
}

VA(0x004324ae, 0x144)
strip::~strip()
{
    int slot;

    gpWindowManager->RemoveWindow(m_window);
    if (m_army != 0) {
        for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++)
            delete m_borders[slot];
        delete m_borders[STRIP_ARMY_SLOT_COUNT];
    }
    delete m_window;
    gpResourceManager->Dispose(m_stripIcon);
    for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
        if (m_creatureIcons[slot] != 0)
            gpResourceManager->Dispose(m_creatureIcons[slot]);
    }
    gpResourceManager->Dispose(m_portraitIcon);
    gpResourceManager->Dispose(m_flagIcon);
}

VA(0x004325f2, 0x40)
void strip::Draw(void)
{
    DrawIcons(1);
    gpWindowManager->UpdateScreenRegion(m_x, m_y, STRIP_WINDOW_WIDTH,
                                        STRIP_WINDOW_HEIGHT);
}

VA(0x00432632, 0x3d3)
void strip::DrawIcons(int drawWindow)
{
    icon *oldIcons[STRIP_ARMY_SLOT_COUNT];
    int oldCreatureTypes[STRIP_ARMY_SLOT_COUNT];
    int iconsCurrent_8;
    int slot;
    int creatureType;

    m_portraitIcon->DrawToBuffer(m_x + STRIP_PORTRAIT_X,
                                 m_y + STRIP_CONTENT_Y, m_portraitFrame, 0);
    if (m_flagFrame != ARMY_GROUP_EMPTY_SLOT)
        m_flagIcon->DrawToBuffer(m_x + STRIP_PORTRAIT_X,
                                 m_y + STRIP_CONTENT_Y, m_flagFrame, 0);

    if (m_army == 0) {
        m_stripIcon->DrawToBuffer(m_x + STRIP_ARMY_FIRST_X,
                                  m_y + STRIP_CONTENT_Y,
                                  STRIP_NO_ARMY_FRAME, 0);
        m_window->DrawWindow(drawWindow);
        return;
    }
    iconsCurrent_8 = 1;
    for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
        if (m_army->m_creatureTypes[slot] != ARMY_GROUP_EMPTY_SLOT &&
            m_army->m_creatureTypes[slot] !=
                m_cachedCreatureTypes[slot])
            iconsCurrent_8 = 0;
    }

    if (iconsCurrent_8 == 0) {
        for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
            oldIcons[slot] = m_creatureIcons[slot];
            oldCreatureTypes[slot] = m_cachedCreatureTypes[slot];
            if (m_army->m_creatureTypes[slot] == ARMY_GROUP_EMPTY_SLOT) {
                m_creatureIcons[slot] = 0;
                m_cachedCreatureTypes[slot] = ARMY_GROUP_EMPTY_SLOT;
            } else {
                sprintf(gText, "monh%04d.icn",
                        m_army->m_creatureTypes[slot]);
                m_creatureIcons[slot] = gpResourceManager->GetIcon(gText);
                m_cachedCreatureTypes[slot] =
                    m_army->m_creatureTypes[slot];
            }
        }
        for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
            if (oldCreatureTypes[slot] != 0)
                gpResourceManager->Dispose(oldIcons[slot]);
        }
    }

    for (slot = 0; slot < STRIP_ARMY_SLOT_COUNT; slot++) {
        creatureType = m_army->m_creatureTypes[slot];
        if (creatureType != ARMY_GROUP_EMPTY_SLOT) {
            m_stripIcon->DrawToBuffer(
                m_x + slot * STRIP_ARMY_X_STEP + STRIP_ARMY_FIRST_X,
                m_y + STRIP_CONTENT_Y,
                gMonsterDatabase[creatureType].race +
                    STRIP_RACE_FRAME_OFFSET,
                0);
            m_creatureIcons[slot]->DrawToBuffer(
                m_x + slot * STRIP_ARMY_X_STEP + STRIP_ARMY_FIRST_X,
                m_y + STRIP_CONTENT_Y, 0, 0);
            sprintf(gText, "%d", m_army->m_creatureCounts[slot]);
            smallFont->DrawBoundedString(
                gText,
                m_x + slot * STRIP_ARMY_X_STEP + STRIP_ARMY_FIRST_X,
                m_y + STRIP_QUANTITY_Y, STRIP_QUANTITY_WIDTH,
                STRIP_QUANTITY_HEIGHT, STRIP_QUANTITY_COLOR,
                STRIP_QUANTITY_BACKGROUND);
        } else {
            m_stripIcon->DrawToBuffer(
                m_x + slot * STRIP_ARMY_X_STEP + STRIP_ARMY_FIRST_X,
                m_y + STRIP_CONTENT_Y, STRIP_EMPTY_FRAME, 0);
        }
    }
    m_window->DrawWindow(drawWindow);
    if (m_selectedSlot != ARMY_GROUP_EMPTY_SLOT) {
        m_stripIcon->DrawToBuffer(
            m_x + m_selectedSlot * STRIP_ARMY_X_STEP +
                STRIP_ARMY_FIRST_X,
            m_y + STRIP_CONTENT_Y, STRIP_SELECTED_FRAME, 0);
    }
}

VA(0x00432a05, 0x33)
void strip::DrawFrame(void)
{
    m_stripIcon->DrawToBuffer(m_x, m_y, 0, 0);
}

VA(0x00432a38, 0xb2)
bankBox::bankBox(int x, int y, class playerData *player)
{
    m_player = player;
    m_x = x;
    m_y = y;
    m_window = new heroWindow(m_x, m_y, "bankbox.bin");
    if (m_window == 0)
        MemError();
    gpWindowManager->AddWindow(m_window, BANK_BOX_WINDOW_Z_ORDER,
                               BANK_BOX_WINDOW_ACTIVE);
    Update(1);
}

VA(0x00432aea, 0x43)
bankBox::~bankBox()
{
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
}

VA(0x00432b2d, 0xcd)
void bankBox::Update(int drawWindow)
{
    char text[BANK_BOX_TEXT_SIZE];
    tag_message message;
    int resource_1;

    message.type = EVENT_WINDOW_MESSAGE;
    message.payload.widget.command = BANK_BOX_SET_TEXT_COMMAND;
    for (resource_1 = 0; resource_1 < BANK_BOX_NON_GOLD_RESOURCE_COUNT;
         resource_1++) {
        sprintf(text, "%d", m_player->m_resources[resource_1]);
        message.payload.widget.id =
            BANK_BOX_FIRST_RESOURCE_WIDGET + resource_1;
        message.payload.widget.data.text = text;
        m_window->BroadcastMessage(message);
    }
    sprintf(text, "%d", m_player->m_resources[RES_GOLD]);
    message.payload.widget.id = BANK_BOX_GOLD_WIDGET;
    message.payload.widget.data.text = text;
    m_window->BroadcastMessage(message);
    m_window->DrawWindow(drawWindow);
}
