// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINDOW.OBJ   from: .\basewin.lib
// functions: 15   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <SOURCE/KB.h>
VA(0x004ceb70, 0xaa)
heroWindow::heroWindow(void) {}

VA(0x004cec20, 0xa5)
heroWindow::heroWindow(int, int, int, int, int) {}

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
void heroWindow::Close(void) {}

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
void heroWindow::MoveWindow(int, int) {}

