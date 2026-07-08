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
    m_font = 0;
    m_text = 0;
    field_0x14 = 0x200;
}

VA(0x004d10f0, 0x64)
textWidget::textWidget(short p1, short p2, short p3, short p4, char *p5, char *p6, short p7,
                       short p8, short p9, short p10)
    : widget(p1, p2, p3, p4, p8, p9)
{
    m_font = gpResourceManager->GetFont(p6);
    field_0x28 = p7;
    field_0x2a = static_cast<char>(p10);
    field_0x14 = 0x200;
    m_text = p5;
}

VA(0x004d1160, 0xef)
void textWidget::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    short len = gpResourceManager->ReadWord();
    m_text = static_cast<char *>(BaseAlloc(len, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(m_text), len);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    field_0x28 = gpResourceManager->ReadWord() & 0xff;
    field_0x2a = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    field_0x14 = 0x200;
}

VA(0x004d1250, 0x30)
textWidget::~textWidget()
{
    gpResourceManager->Dispose(m_font);
    BaseFree(m_text, __FILE__, __LINE__);
}

VA(0x004d1280, 0x210)
int textWidget::Main(tag_message &param_1)
{
    unsigned short uVar2;
    int iVar3;
    unsigned int uVar5, uVar6;
    short sVar7, sVar9;
    char *pcVar8;
    uVar2 = m_flags;
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
            m_flags = uVar2 & 0xfffe;
            if (param_1.type == 0x40)
                param_1.fieldC = 0x200;
            param_1.type = 0x200;
            param_1.field4 = 0xd;
            param_1.field8 = m_id;
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
                    if (m_id == param_1.field8) {
                        pcVar8 = param_1.text;
                        if (field_0x14 != 0x200 && field_0x14 != 0x4000) {
                            m_text = pcVar8;
                            return 1;
                        }
                        uVar5 = strlen(pcVar8) & 0xffff;
                        uVar6 = strlen(m_text);
                        if (uVar6 < uVar5) {
                            BaseFree(m_text, __FILE__, __LINE__);
                            m_text = static_cast<char *>(BaseAlloc(uVar5 + 5, __FILE__, __LINE__));
                        }
                        strcpy(m_text, pcVar8);
                        return 1;
                    }
                } else if (param_1.field4 == 8 && m_id == param_1.field8) {
                    field_0x28 = static_cast<short>(reinterpret_cast<int>(param_1.text));
                    return 1;
                }
            }
            goto LAB_004d12c4;
        }
        goto LAB_004d136b;
    }
    sVar9 = static_cast<short>(param_1.field4) - m_owner->m_posX;
    sVar7 = static_cast<short>(param_1.field8) - m_owner->m_posY;
    if (m_x <= sVar9 && m_y <= sVar7 && sVar9 < m_width + m_x &&
        sVar7 < m_height + m_y) {
        m_flags = uVar2 | 1;
        if (param_1.type == 0x20)
            param_1.fieldC = 0x200;
        param_1.type = 0x200;
        param_1.field4 = 0xc;
        param_1.field8 = m_id;
        return 2;
    }
    return 0;
}

VA(0x004d1490, 0x49)
void textWidget::Draw(void)
{
    int color = 3;
    if ((m_flags & 8) == 0)
        color = field_0x28;
    m_font->DrawBoundedString(m_text, m_x + m_owner->m_posX,
                                  m_y + m_owner->m_posY, m_width, m_height,
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
        m_text = param_1;
        return;
    }
    unsigned short newLen = strlen(param_1);
    if (strlen(m_text) < newLen) {
        BaseFree(m_text, __FILE__, __LINE__);
        m_text = static_cast<char *>(BaseAlloc(newLen + 5, __FILE__, __LINE__));
    }
    strcpy(m_text, param_1);
}


// ===== vtable textWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d1490, 0x49)  void textWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d10a0, 0x45)  void * textWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d1280, 0x210)  int textWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textWidget, 0x004eba50);
