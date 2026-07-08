// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x24 = 0;
    field_0x27 = 0;
    field_0x20 = 0;
    field_0x26 = 0;
    field_0x29 = 0;
}

// @early-stop ~16% — prologue through GetIcon is byte-identical; only the final 5 field
// stores differ, purely /O2 register allocation (retail loads param_6→cx first for an
// ascending-offset store order; ours loads param_10→cx first, descending). Not
// source-controllable — reordering the assignments produces byte-identical output.
VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       unsigned long int param_5, short int param_6, signed char param_7,
                       short int param_8, short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    field_0x29 = param_5;
    field_0x20 = gpResourceManager->GetIcon(param_5);
    field_0x24 = param_6;
    field_0x26 = param_7;
    field_0x27 = param_10;
    field_0x14 = param_9;
}

// @early-stop ~13% — same /O2 register-scheduling wall as the sibling ctor above; body
// is semantically exact (MakeId+GetIcon then the same field stores).
VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       char *param_5, short int param_6, signed char param_7, short int param_8,
                       short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    unsigned long int uVar1 = gpResourceManager->MakeId(param_5, 1);
    field_0x29 = uVar1;
    field_0x20 = gpResourceManager->GetIcon(uVar1);
    field_0x24 = param_6;
    field_0x26 = param_7;
    field_0x27 = param_10;
    field_0x14 = param_9;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void)
{
    char local_10[16];
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    unsigned long id = gpResourceManager->MakeId(local_10, 1);
    field_0x20 = gpResourceManager->GetIcon(id);
    gpResourceManager->RestorePosition();
    field_0x24 = gpResourceManager->ReadWord();
    field_0x26 = static_cast<char>(gpResourceManager->ReadWord());
    field_0x10 = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
    field_0x27 = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget()
{
    gpResourceManager->Dispose(field_0x20);
}

VA(0x004d0cd0, 0x291)
int iconWidget::Main(tag_message &param_1)
{
    unsigned short uVar1;
    short sVar2, sVar5;
    int iVar3;
    uVar1 = field_0x16;
    if ((uVar1 & 2) == 0) {
        if (param_1.type != 0x200)
            return 0;
        if (param_1.field4 != 0x3c)
            return widget::Main(param_1);
    }
    iVar3 = param_1.type;
    if (iVar3 < 0x11) {
        if (iVar3 == 0x10) {
LAB_004d0df0:
            if ((uVar1 & 1) == 0)
                return 0;
            field_0x16 = uVar1 & 0xfffe;
            param_1.field4 = 0xd;
            param_1.type = 0x200;
            param_1.field8 = field_0x10;
            return 2;
        }
        if (iVar3 != 8) {
LAB_widgetmain:
            return widget::Main(param_1);
        }
    } else if (iVar3 != 0x20) {
        if (iVar3 != 0x40) {
            if (iVar3 == 0x200) {
                switch (param_1.field4) {
                case 4:
                    if (field_0x10 == param_1.field8) {
                        field_0x24 = static_cast<short>(reinterpret_cast<int>(param_1.text));
                        return 1;
                    }
                    break;
                case 8:
                    if (field_0x10 == param_1.field8) {
                        field_0x27 = static_cast<unsigned short>(reinterpret_cast<int>(param_1.text)) & 0xff;
                        return 1;
                    }
                    break;
                case 9:
                    if (field_0x10 == param_1.field8) {
                        if (field_0x20 != 0) {
                            gpResourceManager->Dispose(field_0x20);
                            field_0x20 = gpResourceManager->GetIcon(param_1.text);
                        }
                        return 1;
                    }
                    break;
                case 0x3c:
                    if (param_1.field8 == field_0x29) {
                        field_0x29 = reinterpret_cast<int>(param_1.text);
                        gpResourceManager->Dispose(field_0x20);
                        field_0x20 = gpResourceManager->GetIcon(reinterpret_cast<unsigned long>(param_1.text));
                    }
                    return 0;
                }
            }
            goto LAB_widgetmain;
        }
        goto LAB_004d0df0;
    }
    sVar2 = static_cast<short>(param_1.field4) - field_0x4->field_0x28;
    sVar5 = static_cast<short>(param_1.field8) - field_0x4->field_0x2c;
    if (field_0x18 <= sVar2 && field_0x1a <= sVar5 && sVar2 < field_0x1c + field_0x18 &&
        sVar5 < field_0x1e + field_0x1a) {
        if (iVar3 == 0x20) {
            param_1.fieldC = 0x200;
            param_1.field4 = 0xe;
        } else {
            field_0x16 = uVar1 | 1;
            param_1.field4 = 0xc;
        }
        param_1.type = 0x200;
        param_1.field8 = field_0x10;
        return 2;
    }
    return 0;
}

VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void)
{
    short type = field_0x14;
    short x = field_0x4->field_0x28 + field_0x18;
    short y = field_0x4->field_0x2c + field_0x1a;
    if (type == 0x10) {
        field_0x20->DrawToBuffer(x, y, field_0x24, field_0x26);
        return;
    }
    if (type != 0x11) {
        if (type != 0x80)
            return;
        field_0x20->FillToBuffer(x, y, field_0x24, field_0x27, field_0x26, 0);
        return;
    }
    short *entry = reinterpret_cast<short *>(GetIconEntry(field_0x20, field_0x24));
    y = y - entry[1];
    x = x - entry[0];
    if (entry[2] < field_0x1c)
        x = x + (short)((field_0x1c - entry[2]) >> 1);
    if (entry[3] + 2 < field_0x1e)
        y = y + (field_0x1e - entry[3]) - 2;
    field_0x20->DrawToBuffer(x, y, field_0x24, field_0x26);
}


// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(iconWidget, 0x004eba40);
