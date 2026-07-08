// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\droplist.obj   from: .\basewin.lib
// functions: 12   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/dropListWidget.h>
#include <BASE/bitmap.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
#include <string.h>
VA(0x004dbf00, 0x3b)
dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_items = 0;
    m_savedBackground = 0;
    field_0x3c = 0;
    field_0x3e = -1;
}

VA(0x004dbf60, 0x7c)
dropListWidget::~dropListWidget()
{
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    for (int i = 0; i < field_0x3c; i++)
        BaseFree(m_items[i], __FILE__, __LINE__);
    BaseFree(m_items, __FILE__, __LINE__);
}

VA(0x004dbfe0, 0x21d)
void dropListWidget::Read(void)
{
    int iVar1;
    short sVar2;
    int *piVar5;
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(local_10);
    gpResourceManager->RestorePosition();
    field_0x28 = m_x + gpResourceManager->ReadWord();
    field_0x2a = m_y + gpResourceManager->ReadWord();
    field_0x2c = gpResourceManager->ReadWord();
    field_0x2e = gpResourceManager->ReadWord();
    field_0x30 = gpResourceManager->ReadWord();
    field_0x34 = gpResourceManager->ReadWord();
    field_0x36 = gpResourceManager->ReadWord();
    field_0x38 = gpResourceManager->ReadWord();
    field_0x3a = gpResourceManager->ReadWord();
    sVar2 = gpResourceManager->ReadWord();
    field_0x48 = 0;
    field_0x4a = 1;
    field_0x4c = 2;
    field_0x4e = 3;
    field_0x50 = 4;
    field_0x52 = 5;
    field_0x54 = 6;
    field_0x56 = 7;
    field_0x58 = 8;
    field_0x5a = 9;
    field_0x5c = 10;
    m_id = sVar2;
    piVar5 = reinterpret_cast<int *>(&m_icon->m_data);
    field_0x5e = 0xb;
    field_0x60 = 0xc;
    field_0x62 = 0xd;
    iVar1 = *piVar5;
    field_0x6c = m_x;
    field_0x6e = m_y;
    sVar2 = *reinterpret_cast<short *>(iVar1 + 4);
    field_0x70 = sVar2;
    field_0x72 = *reinterpret_cast<short *>(iVar1 + 6);
    iVar1 = *piVar5;
    field_0x64 = sVar2 + m_x;
    field_0x66 = m_y;
    field_0x68 = *reinterpret_cast<short *>(iVar1 + 0x11);
    field_0x6a = *reinterpret_cast<short *>(iVar1 + 0x13);
    iVar1 = *piVar5;
    field_0xa6 = *reinterpret_cast<short *>(iVar1 + 0xad);
    field_0xa8 = *reinterpret_cast<short *>(iVar1 + 0xaf);
}

VA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(int param_1)
{
    short sVar1;
    char **puVar2, **puVar4, **puVar5;
    unsigned int uVar3;
    sVar1 = field_0x3c;
    if (param_1 < sVar1) {
        if (field_0x3e == param_1)
            field_0x3e = -1;
        if (sVar1 == 1) {
            BaseFree(m_items[0], __FILE__, __LINE__);
            BaseFree(m_items, __FILE__, __LINE__);
            m_items = 0;
        } else {
            puVar2 = static_cast<char **>(BaseAlloc(sVar1 * 4 - 4, __FILE__, __LINE__));
            puVar4 = m_items;
            puVar5 = puVar2;
            for (uVar3 = (field_0x3c * 4 - 4U) >> 2; uVar3 != 0; uVar3--) {
                *puVar5 = *puVar4;
                puVar4 = puVar4 + 1;
                puVar5 = puVar5 + 1;
            }
            uVar3 = (field_0x3c - param_1) - 1;
            if (0 < static_cast<int>(uVar3)) {
                puVar4 = m_items + param_1 + 1;
                puVar5 = puVar2 + param_1;
                for (uVar3 = uVar3 & 0x3fffffff; uVar3 != 0; uVar3--) {
                    *puVar5 = *puVar4;
                    puVar4 = puVar4 + 1;
                    puVar5 = puVar5 + 1;
                }
            }
            if (m_items != 0)
                BaseFree(m_items, __FILE__, __LINE__);
            m_items = puVar2;
        }
        field_0x3c = field_0x3c - 1;
    }
}

VA(0x004dc2e0, 0x350)
int dropListWidget::Main(tag_message &param_1)
{
    char cVar1;
    unsigned int uVar6, uVar7;
    short sVar5, sVar8;
    char *pcVar10, *pcVar12;
    char **puVar3, **puVar9, **puVar11;
    if ((m_flags & 2) == 0) {
        if (param_1.type == 0x200)
            return widget::Main(param_1);
        return 0;
    } else if (param_1.type == 8 || param_1.type == 0x20) {
        if ((m_flags & 4) != 0) {
            sVar8 = static_cast<short>(param_1.field4) - m_owner->m_posX;
            sVar5 = static_cast<short>(param_1.field8) - m_owner->m_posY;
            if (param_1.type != 0x20) {
                if (field_0x64 <= sVar8 && field_0x66 <= sVar5 &&
                    sVar8 < field_0x68 + field_0x64 && sVar5 < field_0x6a + field_0x66) {
                    ProcessSelectDialog();
                    param_1.field4 = 0xc;
                    param_1.type = 0x200;
                    param_1.field8 = m_id;
                    return 2;
                }
                return 0;
            }
            if (m_x <= sVar8 && m_y <= sVar5 && sVar8 < m_width + m_x &&
                sVar5 < m_height + m_y) {
                param_1.field4 = 0xe;
                param_1.type = 0x200;
                param_1.fieldC = 0x200;
                param_1.field8 = m_id;
                return 2;
            }
            return 0;
        }
    } else if (param_1.type == 0x200) {
        switch (param_1.field4) {
        case 0x36:
            if (m_id == param_1.field8) {
                field_0x3e = static_cast<short>(reinterpret_cast<int>(param_1.text));
                return 1;
            }
            break;
        case 0x37:
            if (m_id == param_1.field8) {
                param_1.text = reinterpret_cast<char *>(static_cast<int>(field_0x3e));
                return 1;
            }
            break;
        case 0x38:
            if (m_id == param_1.field8) {
                pcVar10 = param_1.text;
                puVar3 = static_cast<char **>(BaseAlloc(field_0x3c * 4 + 4, __FILE__, __LINE__));
                if (field_0x3c != 0) {
                    puVar9 = m_items;
                    puVar11 = puVar3;
                    for (uVar6 = field_0x3c & 0x3fffffff; uVar6 != 0; uVar6--) {
                        *puVar11 = *puVar9;
                        puVar9 = puVar9 + 1;
                        puVar11 = puVar11 + 1;
                    }
                }
                puVar3[field_0x3c] = static_cast<char *>(BaseAlloc(strlen(pcVar10) + 1, __FILE__, __LINE__));
                strcpy(puVar3[field_0x3c], pcVar10);
                field_0x3c = field_0x3c + 1;
                if (m_items != 0)
                    BaseFree(m_items, __FILE__, __LINE__);
                m_items = puVar3;
            }
            break;
        case 0x39:
            if (m_id == param_1.field8) {
                pcVar10 = param_1.text;
                if (param_1.fieldC < field_0x3c) {
                    BaseFree(m_items[param_1.fieldC], __FILE__, __LINE__);
                    m_items[param_1.fieldC] = static_cast<char *>(BaseAlloc(strlen(pcVar10) + 1, __FILE__, __LINE__));
                    strcpy(m_items[param_1.fieldC], pcVar10);
                }
            }
            break;
        case 0x3a:
            if (m_id == param_1.field8)
                DeleteItem(param_1.fieldC);
            break;
        case 0x3b:
            if (m_id == param_1.field8) {
                sVar5 = field_0x3c;
                while (sVar5 != 0) {
                    DeleteItem(0);
                    sVar5 = field_0x3c;
                }
            }
        }
    }
    return widget::Main(param_1);
}

VA(0x004dc630, 0xaf)
void dropListWidget::Draw(void)
{
    m_icon->DrawToBuffer(field_0x6c + m_owner->m_posX, field_0x6e + m_owner->m_posY,
                             field_0x48, 0);
    m_icon->DrawToBuffer(field_0x64 + m_owner->m_posX, field_0x66 + m_owner->m_posY,
                             field_0x4a, 0);
    if (field_0x3c > 0 && field_0x3e >= 0) {
        int color = 3;
        if ((m_flags & 8) == 0)
            color = field_0x34;
        m_font->DrawBoundedString(m_items[field_0x3e],
                                      field_0x28 + m_owner->m_posX,
                                      field_0x2a + m_owner->m_posY,
                                      field_0x2c, field_0x2e, color, field_0x3a);
    }
}

VA(0x004dc6e0, 0x378)
void dropListWidget::DrawDropStuff(void)
{
    int iVar1, iVar4, iVar5;
    short sVar2, sVar3;
    iVar5 = field_0x84 + m_owner->m_posY;
    m_icon->DrawToBuffer(m_owner->m_posX + field_0x82, iVar5, field_0x4e, 0);
    if (field_0x3e == field_0x44)
        sVar2 = field_0x36;
    else
        sVar2 = field_0x34;
    iVar4 = 1;
    m_font->DrawBoundedString(m_items[field_0x44], m_owner->m_posX + field_0x82 + 5,
                                  iVar5 + 4, field_0x86 - 10, m_font->field_0x10 + 1, sVar2,
                                  field_0x3a);
    iVar5 = iVar5 + field_0x74;
    if (1 < field_0x32 - 1) {
        do {
            if (field_0x3c <= field_0x44 + iVar4)
                break;
            m_icon->DrawToBuffer(m_owner->m_posX + field_0x82, iVar5, field_0x50, 0);
            iVar1 = field_0x44 + iVar4;
            if (field_0x3e == iVar1)
                sVar2 = field_0x36;
            else
                sVar2 = field_0x34;
            iVar4 = iVar4 + 1;
            m_font->DrawBoundedString(m_items[iVar1], m_owner->m_posX + field_0x82 + 5,
                                          iVar5 + 2, field_0x86 - 10, m_font->field_0x10 + 1,
                                          sVar2, field_0x3a);
            iVar5 = iVar5 + field_0x76;
        } while (iVar4 < field_0x32 - 1);
    }
    m_icon->DrawToBuffer(m_owner->m_posX + field_0x82, iVar5, field_0x52, 0);
    iVar4 = field_0x44 + iVar4;
    if (iVar4 < field_0x3c) {
        if (field_0x3e == iVar4)
            sVar2 = field_0x36;
        else
            sVar2 = field_0x34;
        m_font->DrawBoundedString(m_items[iVar4], m_owner->m_posX + field_0x82 + 5,
                                      iVar5 + 2, field_0x86 - 10, m_font->field_0x10 + 1, sVar2,
                                      field_0x3a);
    }
    if (0 < field_0x46) {
        if (field_0xac == 0)
            sVar2 = field_0x54;
        else
            sVar2 = field_0x56;
        m_icon->DrawToBuffer(field_0x8a + m_owner->m_posX, field_0x8c + m_owner->m_posY,
                                 sVar2, 0);
        iVar5 = 2;
        m_icon->DrawToBuffer(m_owner->m_posX + field_0x92, m_owner->m_posY + field_0x94,
                                 field_0x5c, 0);
        if (2 < field_0x32 - 2) {
            do {
                iVar4 = iVar5 - 1;
                iVar5 = iVar5 + 1;
                m_icon->DrawToBuffer(m_owner->m_posX + field_0x92,
                                         field_0x76 * iVar4 + m_owner->m_posY + field_0x94,
                                         field_0x5e, 0);
            } while (iVar5 < field_0x32 - 2);
        }
        m_icon->DrawToBuffer(m_owner->m_posX + field_0x92,
                                 field_0x76 * (iVar5 - 1) + m_owner->m_posY + field_0x94,
                                 field_0x60, 0);
        if (field_0xad == 0)
            sVar2 = field_0x58;
        else
            sVar2 = field_0x5a;
        m_icon->DrawToBuffer(field_0x9a + m_owner->m_posX, field_0x9c + m_owner->m_posY,
                                 sVar2, 0);
        sVar3 = static_cast<short>(m_owner->m_posX) + 5 + field_0x92;
        field_0xa2 = sVar3;
        sVar2 = static_cast<short>(m_owner->m_posY) +
                static_cast<short>((field_0xaa * field_0x44) / field_0x46) + 3 + field_0x94;
        field_0xa4 = sVar2;
        m_icon->DrawToBuffer(sVar3, sVar2, field_0x62, 0);
    }
    gpWindowManager->UpdateScreenRegion(m_x, m_y, m_width, field_0x80 + m_height);
}

VA(0x004dca60, 0x50)
void dropListWidget::SaveDropBackground(void)
{
    m_savedBackground = new bitmap(0, field_0x7e, field_0x80);
    m_savedBackground->GrabScreen(field_0x7a, field_0x7c);
}

VA(0x004dcab0, 0x56)
void dropListWidget::RestoreDropBackground(void)
{
    m_savedBackground->DrawToBuffer(field_0x7a, field_0x7c);
    gpWindowManager->UpdateScreenRegion(field_0x7a, field_0x7c, field_0x7e, field_0x80);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    m_savedBackground = 0;
}

VA(0x004dcb10, 0x81f)
void dropListWidget::ProcessSelectDialog(void) {}


// ===== vtable dropListWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004dc630, 0xaf)  void dropListWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dbf40, 0x1f)  void * dropListWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dc2e0, 0x350)  int dropListWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(dropListWidget, 0x004ebad0);
