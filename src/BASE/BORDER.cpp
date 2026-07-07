// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BORDER.OBJ   from: .\basewin.lib
// functions: 8   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/border.h>
#include <BASE/resourceManager.h>
#include <BASE/bmap2.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
VA(0x004d20a0, 0x32)
border::border(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x20 = 0;
    field_0x24 = 0;
    field_0x28 = 0;
}

VA(0x004d2130, 0x64)
border::border(short int x, short int y, short int w, short int h, short int e, short int f, short int p7, char *name)
    : widget(x, y, w, h, e, f)
{
    if (name == 0)
        field_0x20 = 0;
    else
        field_0x20 = gpResourceManager->GetBitmap(name);
    field_0x24 = 0;
    field_0x28 = p7;
}

VA(0x004d21a0, 0x38)
border::~border() {}

VA(0x004d21e0, 0x10e)
void border::Read(void)
{
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    field_0x10 = gpResourceManager->ReadWord();
    short kind = gpResourceManager->ReadWord();
    field_0x20 = 0;
    field_0x24 = 0;
    field_0x14 = kind;
    char name[16];
    if (kind == 0x800) {
        gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
        gpResourceManager->SavePosition();
        field_0x20 = gpResourceManager->GetBitmap(name);
        gpResourceManager->RestorePosition();
        return;
    }
    if (kind == 0x801) {
        gpResourceManager->Read13(reinterpret_cast<signed char *>(name));
        gpResourceManager->SavePosition();
        field_0x24 = gpResourceManager->GetIcon(name);
        gpResourceManager->RestorePosition();
        return;
    }
    field_0x28 = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d22f0, 0x181)
int border::Main(struct tag_message &msg)
{
    unsigned short flags = field_0x16;
    int type = msg.type;
    if ((flags & 2) == 0) {
        if (type == 0x200)
            return widget::Main(msg);
        return 0;
    }
    switch (type) {
    case 8:
    case 0x20:
        break;
    default:
        return widget::Main(msg);
    case 0x10:
    case 0x40:
        if ((flags & 1) != 0) {
            field_0x16 = flags & 0xfffe;
            msg.type = 0x200;
            msg.field4 = 0xd;
            msg.field8 = field_0x10;
            return 2;
        }
        return 0;
    }
    short mx = static_cast<short>(msg.field4) - field_0x4->field_0x28;
    short my = static_cast<short>(msg.field8) - field_0x4->field_0x2c;
    if (field_0x18 <= mx && field_0x1a <= my &&
        mx < field_0x1c + field_0x18 && my < field_0x1e + field_0x1a) {
        if (type == 0x20) {
            msg.fieldC = 0x200;
            msg.field4 = 0xe;
        } else {
            field_0x16 = flags | 1;
            msg.field4 = 0xc;
        }
        msg.type = 0x200;
        msg.field8 = field_0x10;
        return 2;
    }
    return 0;
}

VA(0x004d2480, 0xab)
void border::Draw(void)
{
    short y = field_0x1a + field_0x4->field_0x2c;
    short x = field_0x18 + field_0x4->field_0x28;
    short kind = field_0x14;
    if (kind == 0x400) {
        FillBitmapArea(gpWindowManager->field_0x46, x, y, field_0x1c, field_0x1e, field_0x28);
        return;
    }
    if (kind != 0x800) {
        if (kind != 0x801)
            return;
        field_0x24->DrawToBuffer(x, y, 0, 0);
        return;
    }
    PollSound();
    BlitBitmap(field_0x20, 0, 0, field_0x1c, field_0x1e, gpWindowManager->field_0x46, x, y);
    PollSound();
}


// ===== vtable border : public widget  (3 slots) =====
//  [ 0] VA(0x004d2480, 0xab)  void border::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d20e0, 0x4d)  void * border::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d22f0, 0x181)  int border::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(border, 0x004eba60);
