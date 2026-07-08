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
    field_0x8 = 0;
    field_0x4 = field_0x8;
    field_0x0 = -1;
    field_0x2c = 0;
    field_0x28 = field_0x2c;
    field_0x30 = 640;
    field_0x34 = 480;
    field_0x20 = 1;
    field_0x24 = 0;
    field_0x3c = 0;
    field_0x38 = field_0x3c;
    field_0x40 = 0;
}

VA(0x004cec20, 0xa5)
heroWindow::heroWindow(int x, int y, int w, int h, int flags)
{
    strcpy(name, "Dynamic Construct");
    field_0x8 = 0;
    field_0x4 = field_0x8;
    field_0x0 = -1;
    field_0x28 = x;
    field_0x2c = y;
    field_0x30 = w;
    field_0x34 = h;
    field_0x20 = flags;
    field_0x24 = 0;
    field_0x3c = 0;
    field_0x38 = field_0x3c;
    field_0x40 = 0;
}

VA(0x004cecd0, 0x521)
heroWindow::heroWindow(int param_1, int param_2, char *param_3)
{
    int bVar1;
    int iVar4;
    widget *local_28;
    strcpy(name, param_3);
    unsigned long uVar3 = gpResourceManager->MakeId(param_3, 1);
    gpResourceManager->PointToFile(uVar3);
    field_0x40 = 0;
    field_0x8 = 0;
    field_0x4 = field_0x8;
    field_0x24 = 0;
    field_0x0 = -1;
    field_0x28 = param_1;
    field_0x2c = param_2;
    field_0x30 = gpResourceManager->ReadWord();
    field_0x34 = gpResourceManager->ReadWord();
    field_0x20 = gpResourceManager->ReadWord();
    field_0x20 = field_0x20 | 0x4000;
    field_0x3c = 0;
    field_0x38 = field_0x3c;
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
    if (field_0x24 & 1)
        return 3;
    if ((field_0x20 & 2) != 0 && SaveBackground() != 0)
        return 3;
    field_0x0 = x;
    DrawWindow(flags);
    field_0x24 |= 1;
    return 0;
}

VA(0x004cf280, 0x90)
void heroWindow::RemoveAndDeleteWidget(int param_1)
{
    widget *pwVar1, *local_8;
    local_8 = field_0x3c;
    while (local_8 != 0) {
        pwVar1 = local_8->field_0x8;
        if (local_8->field_0x10 == param_1) {
            RemoveWidget(local_8);
            if ((field_0x20 & 0x4000) != 0) {
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
    if ((field_0x20 & 2) != 0 && (field_0x24 & 1) != 0)
        RestoreBackground();
    w = field_0x3c;
    while (w != 0) {
        next = w->field_0x8;
        RemoveWidget(w);
        if ((field_0x20 & 0x4000) != 0) {
            delete w;
        }
        w = next;
    }
    field_0x24 = 0;
}

VA(0x004cf3c0, 0x13c)
void heroWindow::AddWidget(class widget *param_1, int param_2)
{
    int iVar1;
    widget *local_8 = field_0x3c;
    if (param_2 == -1) {
        if (local_8 == 0)
            param_2 = 0;
        else
            param_2 = local_8->field_0x12 + 1;
    }
    iVar1 = param_1->Open(param_2, this);
    if (iVar1 == 0) {
        for (; local_8 != 0 && param_2 < local_8->field_0x12; local_8 = local_8->field_0x8) {
        }
        if (local_8 == 0) {
            param_1->field_0xc = field_0x38;
            param_1->field_0x8 = 0;
            field_0x38 = param_1;
            if (field_0x3c == 0)
                field_0x3c = param_1;
        } else {
            param_1->field_0x8 = local_8;
            param_1->field_0xc = local_8->field_0xc;
            local_8->field_0xc->field_0x8 = param_1;
            local_8->field_0xc = param_1;
        }
    }
}

VA(0x004cf500, 0x116)
void heroWindow::RemoveWidget(class widget *param_1)
{
    widget *iVar1;
    if (param_1 != 0) {
        param_1->Close();
        if (field_0x38 == param_1) {
            field_0x38 = param_1->field_0xc;
            if (field_0x38 == 0)
                field_0x3c = 0;
            else
                field_0x38->field_0x8 = 0;
        } else if (field_0x3c == param_1) {
            field_0x3c = param_1->field_0x8;
            field_0x3c->field_0xc = 0;
        } else {
            param_1->field_0x8->field_0xc = param_1->field_0xc;
            param_1->field_0xc->field_0x8 = param_1->field_0x8;
        }
        iVar1 = param_1->field_0x8;
        if (iVar1 == 0) {
            field_0x3c = 0;
            field_0x38 = field_0x3c;
        } else {
            iVar1->field_0xc = param_1->field_0xc;
            if (iVar1->field_0xc != 0)
                iVar1->field_0xc->field_0x8 = iVar1;
        }
    }
}

VA(0x004cf620, 0x95)
int heroWindow::BroadcastMessage(struct tag_message &param_1)
{
    int local_8 = 0;
    widget *local_c = field_0x3c;
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
    local_8 = field_0x38;
    local_24.type = 0x200;
    local_24.field4 = 2;
    for (; local_8 != 0; local_8 = local_8->field_0xc) {
        PollSound();
        if (param_2 == -0xffff && param_3 == 0xffff) {
            local_8->Main(local_24);
        } else if (param_2 <= local_8->field_0x10 && local_8->field_0x10 <= param_3) {
            local_8->Main(local_24);
        }
    }
    PollSound();
    if (param_1 != 0 && (field_0x20 & 0x7fff) != 1) {
        gpWindowManager->UpdateScreenRegion(field_0x28, field_0x2c, field_0x30, field_0x34);
        PollSound();
    }
    gpMouseManager->field_0x82 = 1;
}

VA(0x004cf830, 0x7f)
int heroWindow::SaveBackground(void)
{
    field_0x40 = new bitmap(0x21, field_0x30, field_0x34);
    PollSound();
    field_0x40->GrabScreen(field_0x28, field_0x2c);
    PollSound();
    return 0;
}

VA(0x004cf8b0, 0x97)
void heroWindow::RestoreBackground(void)
{
    if (gbDrawWindowBackground) {
        field_0x40->DrawToBuffer(field_0x28, field_0x2c);
        gpWindowManager->UpdateScreenRegion(field_0x28, field_0x2c, field_0x30, field_0x34);
    }
    delete field_0x40;
    field_0x40 = 0;
}

VA(0x004cf950, 0x186)
void heroWindow::MoveWindow(int dx, int dy)
{
    int oldX = field_0x28;
    int oldY = field_0x2c;
    int oldW = field_0x30;
    int oldH = field_0x34;
    int newX = field_0x28 + dx;
    int newY = field_0x2c + dy;
    if (newX < 0)
        newX = 0;
    if (newY < 0)
        newY = 0;
    if (640 < field_0x30 + newX)
        newX = 640 - field_0x30;
    if (480 < field_0x34 + newY)
        newY = 480 - field_0x34;
    field_0x40->DrawToBuffer(field_0x28, field_0x2c);
    field_0x28 = newX;
    field_0x2c = newY;
    field_0x40->GrabBitmap(gpWindowManager->field_0x46, field_0x28, field_0x2c);
    DrawWindow(0);
    int movedX = abs(field_0x28 - oldX);
    int movedY = abs(field_0x2c - oldY);
    if (field_0x28 < oldX)
        oldX = field_0x28;
    if (field_0x2c < oldY)
        oldY = field_0x2c;
    gpWindowManager->UpdateScreenRegion(oldX, oldY, oldW + movedX, oldH + movedY);
}

