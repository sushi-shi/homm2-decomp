// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINDOW.OBJ   from: .\basewin.lib
// functions: 15   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <_carcass_types.h>
#include <BASE/mouseManager.h>
#include <SOURCE/kbwin.h>
#include <BASE/listBoxWidget.h>
#include <BASE/dropListWidget.h>
#include <BASE/textEntryWidget.h>
#include <BASE/textWidget.h>
#include <BASE/iconWidget.h>
#include <BASE/dimmerWidget.h>
#include <BASE/button.h>
#include <BASE/border.h>
#include <BASE/resourceManager.h>
#include <BASE/bitmap.h>
#include <SOURCE/KB.h>
#include <stdlib.h>
#include <string.h>
VA(0x004ceb70, 0xaa)
heroWindow::heroWindow(void)
{
    strcpy(name, "Default Construct");
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_zOrder = -1;
    m_posY = 0;
    m_posX = m_posY;
    m_winWidth = 640;
    m_winHeight = 480;
    m_winFlags = 1;
    m_winState = 0;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    m_savedBackground = 0;
}

VA(0x004cec20, 0xa5)
heroWindow::heroWindow(int x, int y, int w, int h, int flags)
{
    strcpy(name, "Dynamic Construct");
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = w;
    m_winHeight = h;
    m_winFlags = flags;
    m_winState = 0;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    m_savedBackground = 0;
}

// @early-stop 85% — the widget-factory dispatch is structurally correct, but retail keeps
// nine distinct function-scope widget-type pointer locals (local_10..local_34, one per
// widget class) rather than the block-scoped `w` here, so the /Od name-hash assigns
// different frame slots — a full od_slots.py frame solve for ~9 locals. Deferred wall.
VA(0x004cecd0, 0x521)
heroWindow::heroWindow(int param_1, int param_2, char *param_3)
{
    int bVar1;
    int iVar4;
    widget *local_28;
    strcpy(name, param_3);
    unsigned long uVar3 = gpResourceManager->MakeId(param_3, 1);
    gpResourceManager->PointToFile(uVar3);
    m_savedBackground = 0;
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_winState = 0;
    m_zOrder = -1;
    m_posX = param_1;
    m_posY = param_2;
    m_winWidth = gpResourceManager->ReadWord();
    m_winHeight = gpResourceManager->ReadWord();
    m_winFlags = gpResourceManager->ReadWord();
    m_winFlags = m_winFlags | 0x4000;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    bVar1 = 0;
    while (!bVar1) {
        PollSound();
        iVar4 = gpResourceManager->ReadWord();
        local_28 = 0;
        if (iVar4 < 9) {
            if (iVar4 == 8) {
                textWidget *w = new textWidget();
                w->Read();
                local_28 = w;
            } else if (iVar4 == 0) {
                bVar1 = 1;
            } else if (iVar4 == 1) {
                border *w = new border();
                w->Read();
                local_28 = w;
            } else if (iVar4 == 2) {
                button *w = new button();
                w->Read();
                local_28 = w;
            }
        } else if (iVar4 < 0x41) {
            if (iVar4 == 0x40) {
                dimmerWidget *w = new dimmerWidget();
                w->Read();
                local_28 = w;
            } else if (iVar4 == 0x10) {
                iconWidget *w = new iconWidget();
                w->Read();
                local_28 = w;
            }
        } else if (iVar4 < 0x202) {
            if (iVar4 == 0x201) {
                textEntryWidget *w = new textEntryWidget();
                w->Read(2);
                local_28 = w;
            } else if (iVar4 == 0x100) {
                textEntryWidget *w = new textEntryWidget();
                w->Read(1);
                local_28 = w;
            }
        } else {
            switch (iVar4) {
            case 0x202: {
                textEntryWidget *w = new textEntryWidget();
                w->Read(3);
                local_28 = w;
                break;
            }
            case 0x203: {
                dropListWidget *w = new dropListWidget();
                w->Read();
                local_28 = w;
                break;
            }
            case 0x204: {
                textEntryWidget *w = new textEntryWidget();
                w->Read(4);
                local_28 = w;
                break;
            }
            case 0x205: {
                listBoxWidget *w = new listBoxWidget();
                w->Read();
                local_28 = w;
                break;
            }
            case 0x206: {
                textEntryWidget *w = new textEntryWidget();
                w->Read(5);
                local_28 = w;
                break;
            }
            }
        }
        if (!bVar1 && local_28 != 0) {
            AddWidget(local_28, -1);
        }
    }
}

VA(0x004cf200, 0x73)
int heroWindow::Open(int x, int flags)
{
    if (m_winState & 1)
        return 3;
    if ((m_winFlags & 2) != 0 && SaveBackground() != 0)
        return 3;
    m_zOrder = x;
    DrawWindow(flags);
    m_winState |= 1;
    return 0;
}

VA(0x004cf280, 0x90)
void heroWindow::RemoveAndDeleteWidget(int param_1)
{
    widget *pwVar1, *local_8;
    local_8 = m_widgetListHead;
    while (local_8 != 0) {
        pwVar1 = local_8->field_0x8;
        if (local_8->field_0x10 == param_1) {
            RemoveWidget(local_8);
            if ((m_winFlags & 0x4000) != 0) {
                delete local_8;
            }
        }
        local_8 = pwVar1;
    }
}

VA(0x004cf310, 0xaa)
void heroWindow::Close(void)
{
    widget *w, *next;
    if ((m_winFlags & 2) != 0 && (m_winState & 1) != 0)
        RestoreBackground();
    w = m_widgetListHead;
    while (w != 0) {
        next = w->field_0x8;
        RemoveWidget(w);
        if ((m_winFlags & 0x4000) != 0) {
            delete w;
        }
        w = next;
    }
    m_winState = 0;
}

VA(0x004cf3c0, 0x13c)
void heroWindow::AddWidget(class widget *param_1, int param_2)
{
    widget *local_8 = m_widgetListHead;
    if (param_2 == -1) {
        if (local_8 == 0)
            param_2 = 0;
        else
            param_2 = local_8->field_0x12 + 1;
    }
    if (param_1->Open(param_2, this) != 0)
        return;
    while (local_8 != 0 && param_2 < local_8->field_0x12) {
        local_8 = local_8->field_0x8;
    }
    if (local_8 == 0) {
        param_1->field_0xc = m_widgetListTail;
        param_1->field_0x8 = 0;
        m_widgetListTail = param_1;
        if (m_widgetListHead == 0)
            m_widgetListHead = param_1;
    } else if (local_8->field_0xc == 0) {
        param_1->field_0x8 = m_widgetListHead;
        param_1->field_0xc = 0;
        m_widgetListHead->field_0xc = param_1;
        m_widgetListHead = param_1;
    } else {
        param_1->field_0x8 = local_8;
        param_1->field_0xc = local_8->field_0xc;
        local_8->field_0xc->field_0x8 = param_1;
        local_8->field_0xc = param_1;
    }
}

VA(0x004cf500, 0x116)
void heroWindow::RemoveWidget(class widget *param_1)
{
    widget *iVar1;
    if (param_1 == 0)
        return;
    param_1->Close();
    if (m_widgetListTail == param_1) {
        m_widgetListTail = param_1->field_0xc;
        if (m_widgetListTail == 0)
            m_widgetListHead = 0;
        else
            m_widgetListTail->field_0x8 = 0;
    } else if (m_widgetListHead == param_1) {
        m_widgetListHead = param_1->field_0x8;
        m_widgetListHead->field_0xc = 0;
    } else {
        param_1->field_0x8->field_0xc = param_1->field_0xc;
        param_1->field_0xc->field_0x8 = param_1->field_0x8;
    }
    iVar1 = param_1->field_0x8;
    if (iVar1 == 0) {
        m_widgetListHead = 0;
        m_widgetListTail = m_widgetListHead;
    } else {
        iVar1->field_0xc = param_1->field_0xc;
        if (iVar1->field_0xc != 0)
            iVar1->field_0xc->field_0x8 = iVar1;
    }
}

VA(0x004cf620, 0x95)
int heroWindow::BroadcastMessage(struct tag_message &param_1)
{
    // @early-stop 78% — retail materializes the (result<1 || 2<result) guard into a bool
    // temp before AND-ing with local_c!=0 (a /Od boolean-codegen shape not reachable from
    // the natural compound condition). Same wall as heroWindowManager::Main.
    int local_8 = 0;
    widget *local_c = m_widgetListHead;
    while (local_c != 0 && ((local_8 = local_c->Main(param_1)) < 1 || 2 < local_8)) {
        local_c = local_c->field_0x8;
    }
    return local_8;
}

VA(0x004cf6c0, 0x20)
void heroWindow::DrawWindow(void) { DrawWindow(1); }

VA(0x004cf6e0, 0x2e)
void heroWindow::DrawWindow(int flags) { DrawWindow(flags, -0xffff, 0xffff); }

VA(0x004cf710, 0x116)
void heroWindow::DrawWindow(int param_1, int param_2, int param_3)
{
    tag_message local_24;
    widget *local_8;
    gpMouseManager->field_0x82 = 0;
    local_8 = m_widgetListTail;
    local_24.type = 0x200;
    local_24.field4 = 2;
    // @early-stop 91% — retail duplicates this Main() call across the two guards
    // (if/else-if codegen: the all-widgets body placed after the range body, reached by a
    // forward je); the equivalent || form shares one call and aligns closer. The exact
    // basic-block ordering isn't source-forcible.
    while (local_8 != 0) {
        PollSound();
        if ((param_2 == -0xffff && param_3 == 0xffff) ||
            (param_2 <= local_8->field_0x10 && local_8->field_0x10 <= param_3)) {
            local_8->Main(local_24);
        }
        local_8 = local_8->field_0xc;
    }
    PollSound();
    if (param_1 != 0 && (m_winFlags & 0x7fff) != 1) {
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
        PollSound();
    }
    gpMouseManager->field_0x82 = 1;
}

VA(0x004cf830, 0x7f)
int heroWindow::SaveBackground(void)
{
    m_savedBackground = new bitmap(0x21, m_winWidth, m_winHeight);
    PollSound();
    m_savedBackground->GrabScreen(m_posX, m_posY);
    PollSound();
    return 0;
}

VA(0x004cf8b0, 0x97)
void heroWindow::RestoreBackground(void)
{
    if (gbDrawWindowBackground) {
        m_savedBackground->DrawToBuffer(m_posX, m_posY);
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
    }
    delete m_savedBackground;
    m_savedBackground = 0;
}

VA(0x004cf950, 0x186)
void heroWindow::MoveWindow(int dx, int dy)
{
    int oldX = m_posX;
    int oldY = m_posY;
    int oldW = m_winWidth;
    int oldH = m_winHeight;
    int newX = m_posX + dx;
    int newY = m_posY + dy;
    if (newX < 0)
        newX = 0;
    if (newY < 0)
        newY = 0;
    if (640 < m_winWidth + newX)
        newX = 640 - m_winWidth;
    if (480 < m_winHeight + newY)
        newY = 480 - m_winHeight;
    m_savedBackground->DrawToBuffer(m_posX, m_posY);
    m_posX = newX;
    m_posY = newY;
    m_savedBackground->GrabBitmap(gpWindowManager->field_0x46, m_posX, m_posY);
    DrawWindow(0);
    oldW = oldW + abs(m_posX - oldX);
    oldH = oldH + abs(m_posY - oldY);
    if (m_posX < oldX)
        oldX = m_posX;
    if (m_posY < oldY)
        oldY = m_posY;
    gpWindowManager->UpdateScreenRegion(oldX, oldY, oldW, oldH);
}

