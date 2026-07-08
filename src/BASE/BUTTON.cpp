// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BUTTON.OBJ   from: .\basewin.lib
// functions: 11   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/button.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/KB.h>
#include <_globals_model.h>
VA(0x004dd440, 0x34)
button::button(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x24 = 0;
    field_0x26 = 0;
    field_0x2c = 0;
    field_0x28 = 0;
    field_0x2a = -1;
    field_0x20 = 0;
}

VA(0x004dd4c0, 0x6e)
button::button(short int x, short int y, short int w, short int h, unsigned long int iconId, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    field_0x2c = iconId;
    field_0x20 = gpResourceManager->GetIcon(iconId);
    field_0x24 = p6;
    field_0x26 = p7;
    field_0x28 = p8;
    field_0x2a = p9;
}

VA(0x004dd530, 0x7c)
button::button(short int x, short int y, short int w, short int h, char *name, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    unsigned long id = gpResourceManager->MakeId(name, 1);
    field_0x2c = id;
    field_0x20 = gpResourceManager->GetIcon(id);
    field_0x24 = p6;
    field_0x26 = p7;
    field_0x28 = p8;
    field_0x2a = p9;
}

VA(0x004dd5b0, 0xeb)
void button::Read(void)
{
    char local_10[16];
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x2c = gpResourceManager->MakeId(local_10, 1);
    field_0x20 = gpResourceManager->GetIcon(field_0x2c);
    gpResourceManager->RestorePosition();
    field_0x24 = gpResourceManager->ReadWord();
    field_0x26 = gpResourceManager->ReadWord();
    field_0x28 = gpResourceManager->ReadWord();
    field_0x2a = gpResourceManager->ReadWord();
    field_0x10 = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
}

VA(0x004dd6a0, 0x21)
button::~button()
{
    gpResourceManager->Dispose(field_0x20);
}

VA(0x004dd6d0, 0x595)
int button::Main(struct tag_message &) { return 0; }

VA(0x004ddc70, 0x96)
short button::Select(struct tag_message &msg)
{
    short x = field_0x4->field_0x28 + field_0x18;
    short y = field_0x1a + field_0x4->field_0x2c;
    field_0x20->DrawToBuffer(x, y, field_0x26, 0);
    gpWindowManager->UpdateScreenRegion(x, y, field_0x1c, field_0x1e);
    field_0x16 |= 1;
    msg.type = 0x200;
    msg.field8 = field_0x10;
    msg.field4 = 10;
    if (field_0x28 != 1)
        msg.field4 = 0xc;
    gButtonRepeatTime = KBTickCount() + 0x3c;
    iLeftRightSave = msg.fieldC & 0x300;
    return 2;
}

VA(0x004ddd10, 0x83)
short button::Deselect(struct tag_message &msg)
{
    if ((field_0x16 & 1) == 0)
        return 0;
    field_0x16 &= 0xfffe;
    Draw();
    gpWindowManager->UpdateScreenRegion(field_0x18 + field_0x4->field_0x28,
                                        field_0x1a + field_0x4->field_0x2c, field_0x1c, field_0x1e);
    msg.field4 = 0xd;
    msg.type = 0x200;
    msg.field8 = field_0x10;
    msg.fieldC = iLeftRightSave;
    iLeftRightSave = 0;
    return 2;
}

VA(0x004ddda0, 0x55)
void button::Draw(void)
{
    heroWindow *win = field_0x4;
    if ((field_0x16 & 1) != 0) {
        field_0x20->DrawToBuffer(field_0x18 + win->field_0x28, field_0x1a + win->field_0x2c,
                                 field_0x26, 0);
        return;
    }
    field_0x20->DrawToBuffer(field_0x18 + win->field_0x28, field_0x1a + win->field_0x2c,
                             field_0x24, 0);
}


// ===== vtable button : public widget  (3 slots) =====
//  [ 0] VA(0x004ddda0, 0x55)  void button::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd480, 0x36)  void * button::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd6d0, 0x595)  int button::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(button, 0x004ebaf0);

// ---- globals (definitions, RVA order) ----
int iLeftRightSave;
