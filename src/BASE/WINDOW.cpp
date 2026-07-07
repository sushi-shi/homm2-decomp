// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINDOW.OBJ   from: .\basewin.lib
// functions: 15   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
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
heroWindow::heroWindow(int, int, char *) {}

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
void heroWindow::RemoveAndDeleteWidget(int) {}

VA(0x004cf310, 0xaa)
void heroWindow::Close(void)
{
    widget *w, *next;
    if ((field_0x20 & 2) != 0 && (field_0x24 & 1) != 0)
        RestoreBackground();
    next = field_0x3c;
    while ((w = next) != 0) {
        next = w->field_0x8;
        RemoveWidget(w);
        if ((field_0x20 & 0x4000) != 0 && w != 0)
            delete w;
    }
    field_0x24 = 0;
}

VA(0x004cf3c0, 0x13c)
void heroWindow::AddWidget(class widget *, int) {}

VA(0x004cf500, 0x116)
void heroWindow::RemoveWidget(class widget *) {}

VA(0x004cf620, 0x95)
int heroWindow::BroadcastMessage(struct tag_message &) { return 0; }

VA(0x004cf6c0, 0x20)
void heroWindow::DrawWindow(void) { DrawWindow(1); }

VA(0x004cf6e0, 0x2e)
void heroWindow::DrawWindow(int flags) { DrawWindow(flags, -0xffff, 0xffff); }

VA(0x004cf710, 0x116)
void heroWindow::DrawWindow(int, int, int) {}

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

