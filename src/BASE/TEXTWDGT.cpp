// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\TEXTWDGT.OBJ   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <SOURCE/KB.h>
#include <string.h>
VA(0x004d1060, 0x3e)
textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    field_0x28 = 1;
    field_0x2a = 1;
    field_0x24 = 0;
    field_0x20 = 0;
    field_0x14 = 0x200;
}

VA(0x004d10f0, 0x64)
textWidget::textWidget(short p1, short p2, short p3, short p4, char *p5, char *p6, short p7,
                       short p8, short p9, short p10)
    : widget(p1, p2, p3, p4, p8, p9)
{
    field_0x24 = gpResourceManager->GetFont(p6);
    field_0x28 = p7;
    field_0x2a = static_cast<char>(p10);
    field_0x14 = 0x200;
    field_0x20 = p5;
}

VA(0x004d1160, 0xef)
void textWidget::Read(void)
{
    char local_10[16];
    field_0x18 = gpResourceManager->ReadWord();
    field_0x1a = gpResourceManager->ReadWord();
    field_0x1c = gpResourceManager->ReadWord();
    field_0x1e = gpResourceManager->ReadWord();
    short len = gpResourceManager->ReadWord();
    field_0x20 = static_cast<char *>(BaseAlloc(len, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(field_0x20), len);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x24 = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    field_0x28 = gpResourceManager->ReadWord() & 0xff;
    field_0x2a = static_cast<char>(gpResourceManager->ReadWord());
    field_0x10 = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    field_0x14 = 0x200;
}

VA(0x004d1250, 0x30)
textWidget::~textWidget()
{
    gpResourceManager->Dispose(field_0x24);
    BaseFree(field_0x20, __FILE__, __LINE__);
}

VA(0x004d1280, 0x210)
int textWidget::Main(tag_message &param_1)
{
    unsigned short uVar2;
    int iVar3;
    unsigned int uVar5, uVar6;
    short sVar7, sVar9;
    char *pcVar8;
    uVar2 = field_0x16;
    iVar3 = param_1.type;
    if ((uVar2 & 2) == 0) {
        if (iVar3 != 0x200)
            return 0;
        return widget::Main(param_1);
    }
    if (iVar3 < 0x11) {
        if (iVar3 == 0x10) {
LAB_004d136b:
            if ((uVar2 & 1) == 0)
                return 0;
            field_0x16 = uVar2 & 0xfffe;
            if (param_1.type == 0x40)
                param_1.fieldC = 0x200;
            param_1.type = 0x200;
            param_1.field4 = 0xd;
            param_1.field8 = field_0x10;
            return 2;
        }
        if (iVar3 != 8) {
LAB_004d12c4:
            return widget::Main(param_1);
        }
    } else if (iVar3 != 0x20) {
        if (iVar3 != 0x40) {
            if (iVar3 == 0x200) {
                if (param_1.field4 == 3) {
                    if (field_0x10 == param_1.field8) {
                        pcVar8 = param_1.text;
                        if (field_0x14 != 0x200 && field_0x14 != 0x4000) {
                            field_0x20 = pcVar8;
                            return 1;
                        }
                        uVar5 = strlen(pcVar8) & 0xffff;
                        uVar6 = strlen(field_0x20);
                        if (uVar6 < uVar5) {
                            BaseFree(field_0x20, __FILE__, __LINE__);
                            field_0x20 = static_cast<char *>(BaseAlloc(uVar5 + 5, __FILE__, __LINE__));
                        }
                        strcpy(field_0x20, pcVar8);
                        return 1;
                    }
                } else if (param_1.field4 == 8 && field_0x10 == param_1.field8) {
                    field_0x28 = static_cast<short>(reinterpret_cast<int>(param_1.text));
                    return 1;
                }
            }
            goto LAB_004d12c4;
        }
        goto LAB_004d136b;
    }
    sVar9 = static_cast<short>(param_1.field4) - field_0x4->m_posX;
    sVar7 = static_cast<short>(param_1.field8) - field_0x4->m_posY;
    if (field_0x18 <= sVar9 && field_0x1a <= sVar7 && sVar9 < field_0x1c + field_0x18 &&
        sVar7 < field_0x1e + field_0x1a) {
        field_0x16 = uVar2 | 1;
        if (param_1.type == 0x20)
            param_1.fieldC = 0x200;
        param_1.type = 0x200;
        param_1.field4 = 0xc;
        param_1.field8 = field_0x10;
        return 2;
    }
    return 0;
}

VA(0x004d1490, 0x49)
void textWidget::Draw(void)
{
    int color = 3;
    if ((field_0x16 & 8) == 0)
        color = field_0x28;
    field_0x24->DrawBoundedString(field_0x20, field_0x18 + field_0x4->m_posX,
                                  field_0x1a + field_0x4->m_posY, field_0x1c, field_0x1e,
                                  color, field_0x2a);
}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(short int param_1)
{
    field_0x28 = param_1;
}

VA(0x004d14f0, 0xa2)
void textWidget::SetText(char *param_1)
{
    if (field_0x14 != 0x200 && field_0x14 != 0x4000) {
        field_0x20 = param_1;
        return;
    }
    unsigned short newLen = strlen(param_1);
    if (strlen(field_0x20) < newLen) {
        BaseFree(field_0x20, __FILE__, __LINE__);
        field_0x20 = static_cast<char *>(BaseAlloc(newLen + 5, __FILE__, __LINE__));
    }
    strcpy(field_0x20, param_1);
}


// ===== vtable textWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d1490, 0x49)  void textWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d10a0, 0x45)  void * textWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d1280, 0x210)  int textWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textWidget, 0x004eba50);
