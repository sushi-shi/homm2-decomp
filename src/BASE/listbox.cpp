// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\listbox.obj   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/listBoxWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <string.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004db060, 0x42)
listBoxWidget::listBoxWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_items = 0;
    m_scrollbar = 0;
    m_itemCount = 0;
    m_selectedIndex = -1;
    field_0x36 = -1;
}

VA(0x004db0d0, 0x86)
listBoxWidget::~listBoxWidget()
{
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_scrollbar != 0)
        delete m_scrollbar;
    for (int i = 0; i < m_itemCount; i++)
        BaseFree(m_items[i], __FILE__, __LINE__);
    BaseFree(m_items, __FILE__, __LINE__);
    gbSendMouseMoveMessages = 0;
}

VA(0x004db160, 0x26e)
void listBoxWidget::Read(void)
{
    unsigned short uVar1;
    int iVar2;
    short sVar3, sVar4, sVar7, sVar8;
    int *piVar9;
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
    field_0x28 = gpResourceManager->ReadWord();
    field_0x2c = gpResourceManager->ReadWord();
    field_0x2e = gpResourceManager->ReadWord();
    field_0x30 = gpResourceManager->ReadWord();
    sVar3 = gpResourceManager->ReadWord();
    field_0x44 = 0;
    m_id = sVar3;
    field_0x46 = 1;
    field_0x48 = 2;
    field_0x4a = 3;
    field_0x4c = 4;
    field_0x4e = 5;
    field_0x50 = 6;
    field_0x52 = 7;
    field_0x54 = 8;
    field_0x56 = 9;
    field_0x58 = 10;
    iVar2 = *reinterpret_cast<int *>(&m_icon->m_data);
    piVar9 = reinterpret_cast<int *>(&m_icon->m_data);
    field_0x84 = *reinterpret_cast<short *>(iVar2 + 0x86);
    sVar3 = *reinterpret_cast<short *>(iVar2 + 0x88);
    field_0x86 = sVar3;
    sVar4 = *reinterpret_cast<short *>(*piVar9 + 6);
    field_0x5a = sVar4;
    sVar8 = *reinterpret_cast<short *>(*piVar9 + 0x13);
    field_0x5c = sVar8;
    sVar7 = *reinterpret_cast<short *>(*piVar9 + 0x20);
    field_0x5e = sVar7;
    m_listX = m_x;
    m_listY = m_y;
    field_0x64 = *reinterpret_cast<short *>(*piVar9 + 4);
    field_0x66 = (field_0x28 - 2) * sVar8 + sVar4 + sVar7;
    iVar2 = *piVar9;
    sVar4 = *reinterpret_cast<short *>(iVar2 + 0x2b);
    field_0x6c = sVar4;
    sVar8 = *reinterpret_cast<short *>(iVar2 + 0x2d);
    field_0x6e = sVar8;
    iVar2 = *piVar9;
    uVar1 = *reinterpret_cast<unsigned short *>(iVar2 + 0x45);
    field_0x7c = uVar1;
    sVar7 = (m_width - sVar4) + m_x;
    sVar4 = *reinterpret_cast<short *>(iVar2 + 0x47);
    field_0x7e = sVar4;
    field_0x68 = sVar7;
    field_0x6a = m_y;
    field_0x78 = sVar7;
    sVar4 = m_y + (m_height - sVar4);
    field_0x7a = sVar4;
    field_0x70 = sVar7;
    sVar8 = m_y + sVar8;
    sVar4 = sVar4 - sVar8;
    field_0x8a = 0;
    field_0x8b = 0;
    field_0x8d = 0;
    field_0x8c = 0;
    m_topIndex = 0;
    field_0x42 = 0;
    field_0x2a = 0;
    field_0x72 = sVar8;
    field_0x74 = uVar1;
    field_0x76 = sVar4;
    field_0x88 = (sVar4 - sVar3) + -7;
}

VA(0x004db3d0, 0x142)
void listBoxWidget::DeleteItem(int param_1)
{
    short sVar2, sVar3;
    void **puVar4, **puVar6, **puVar7;
    unsigned int uVar5;
    sVar2 = m_itemCount;
    if (param_1 < sVar2) {
        if (m_selectedIndex == param_1)
            m_selectedIndex = -1;
        sVar3 = m_topIndex;
        if (sVar3 == param_1 && field_0x42 <= sVar3)
            m_topIndex = sVar3 - 1;
        field_0x42 = field_0x42 - 1;
        if (field_0x42 < 0)
            field_0x42 = 0;
        if (m_topIndex < 0)
            m_topIndex = 0;
        if (field_0x42 < m_topIndex)
            m_topIndex = field_0x42;
        if (sVar2 == 1) {
            BaseFree(m_items[0], __FILE__, __LINE__);
            BaseFree(m_items, __FILE__, __LINE__);
            m_items = 0;
        } else {
            puVar4 = static_cast<void **>(BaseAlloc(sVar2 * 4 - 4, __FILE__, __LINE__));
            puVar6 = m_items;
            puVar7 = puVar4;
            for (uVar5 = (m_itemCount * 4 - 4U) >> 2; uVar5 != 0; uVar5--) {
                *puVar7 = *puVar6;
                puVar6 = puVar6 + 1;
                puVar7 = puVar7 + 1;
            }
            uVar5 = (m_itemCount - param_1) - 1;
            if (0 < static_cast<int>(uVar5)) {
                puVar6 = m_items + param_1 + 1;
                puVar7 = puVar4 + param_1;
                for (uVar5 = uVar5 & 0x3fffffff; uVar5 != 0; uVar5--) {
                    *puVar7 = *puVar6;
                    puVar6 = puVar6 + 1;
                    puVar7 = puVar7 + 1;
                }
            }
            if (m_items != 0)
                BaseFree(m_items, __FILE__, __LINE__);
            m_items = puVar4;
        }
        m_itemCount = m_itemCount - 1;
    }
}

VA(0x004db520, 0x368)
int listBoxWidget::Main(tag_message &param_1)
{
    unsigned short uVar2;
    short sVar3, sVar7, sVar10;
    int iVar4;
    unsigned int uVar8;
    void **puVar5, **puVar11, **puVar13;
    char *pcVar12;
    uVar2 = m_flags;
    if ((uVar2 & 2) == 0) {
        if (param_1.type == 0x200)
            return widget::Main(param_1);
        return 0;
    }
    iVar4 = param_1.type;
    if (iVar4 < 9) {
        if (iVar4 == 8) {
LAB_004db5a2:
            if ((uVar2 & 4) != 0) {
                sVar3 = static_cast<short>(param_1.field4) - m_owner->m_posX;
                sVar10 = static_cast<short>(param_1.field8) - m_owner->m_posY;
                if (m_x <= sVar3 && m_y <= sVar10 && sVar3 < m_width + m_x &&
                    sVar10 < m_height + m_y) {
                    if (iVar4 == 0x20) {
                        param_1.field4 = 0xe;
                        param_1.type = 0x200;
                        param_1.field8 = m_id;
                        param_1.fieldC = 0x200;
                        return 2;
                    }
                    return ProcessMouseMessage(param_1);
                }
                return 0;
            }
            goto switchD_default;
        }
        if (iVar4 != 4)
            goto switchD_default;
    } else if (iVar4 != 0x10) {
        if (iVar4 != 0x20) {
            if (iVar4 == 0x200) {
                switch (param_1.field4) {
                case 0x36:
                    if (m_id == param_1.field8) {
                        m_selectedIndex = static_cast<short>(reinterpret_cast<int>(param_1.text));
                        return 1;
                    }
                    break;
                case 0x37:
                    if (m_id == param_1.field8) {
                        param_1.text = reinterpret_cast<char *>(static_cast<int>(m_selectedIndex));
                        return 1;
                    }
                    break;
                case 0x38:
                    if (m_id == param_1.field8) {
                        pcVar12 = param_1.text;
                        puVar5 = static_cast<void **>(BaseAlloc(m_itemCount * 4 + 4, __FILE__, __LINE__));
                        if (m_itemCount != 0) {
                            puVar11 = m_items;
                            puVar13 = puVar5;
                            for (uVar8 = m_itemCount & 0x3fffffff; uVar8 != 0; uVar8--) {
                                *puVar13 = *puVar11;
                                puVar11 = puVar11 + 1;
                                puVar13 = puVar13 + 1;
                            }
                        }
                        puVar5[m_itemCount] = BaseAlloc(strlen(pcVar12) + 1, __FILE__, __LINE__);
                        strcpy(static_cast<char *>(puVar5[m_itemCount]), pcVar12);
                        m_itemCount = m_itemCount + 1;
                        if (m_items != 0)
                            BaseFree(m_items, __FILE__, __LINE__);
                        sVar3 = field_0x28;
                        m_items = puVar5;
                        sVar10 = m_itemCount;
                        if (sVar3 < sVar10) {
                            sVar7 = sVar10 - sVar3;
                            field_0x42 = sVar7;
                            m_topIndex = m_selectedIndex;
                            if (m_selectedIndex < 0)
                                m_topIndex = 0;
                            if (sVar7 < m_topIndex)
                                m_topIndex = sVar7;
                        }
                        if (field_0x42 < 1)
                            field_0x2a = sVar10;
                        else
                            field_0x2a = sVar3;
                    }
                    break;
                case 0x39:
                    if (m_id == param_1.field8) {
                        pcVar12 = param_1.text;
                        if (param_1.fieldC < m_itemCount) {
                            BaseFree(m_items[param_1.fieldC], __FILE__, __LINE__);
                            m_items[param_1.fieldC] = BaseAlloc(strlen(pcVar12) + 1, __FILE__, __LINE__);
                            strcpy(static_cast<char *>(m_items[param_1.fieldC]), pcVar12);
                        }
                    }
                    break;
                case 0x3a:
                    if (m_id == param_1.field8)
                        DeleteItem(param_1.fieldC);
                    break;
                case 0x3b:
                    if (m_id == param_1.field8) {
                        sVar3 = m_itemCount;
                        while (sVar3 != 0) {
                            DeleteItem(0);
                            sVar3 = m_itemCount;
                        }
                    }
                }
            }
            goto switchD_default;
        }
        goto LAB_004db5a2;
    }
    if ((uVar2 & 4) != 0)
        return ProcessMouseMessage(param_1);
switchD_default:
    return widget::Main(param_1);
}

VA(0x004db890, 0x8)
void listBoxWidget::Draw(void)
{
    DrawLBStuff(0);
}

VA(0x004db8a0, 0x334)
void listBoxWidget::DrawLBStuff(int param_1)
{
    short sVar1, sVar4;
    int iVar2, iVar3, iVar5;
    iVar2 = m_listX + m_owner->m_posX;
    iVar5 = m_listY + m_owner->m_posY;
    iVar3 = 0;
    if (0 < field_0x28) {
        do {
            if (iVar3 == 0) {
                m_icon->DrawToBuffer(iVar2, iVar5, field_0x44, 0);
                if (0 < field_0x2a) {
                    if (m_selectedIndex == m_topIndex)
                        sVar4 = field_0x2e;
                    else
                        sVar4 = field_0x2c;
                    m_font->DrawBoundedString(static_cast<char *>(m_items[m_topIndex]),
                                                  iVar2 + 5, iVar5 + 4, field_0x64 - 10,
                                                  m_font->m_height + 1, sVar4, field_0x30);
                }
                sVar4 = field_0x5a;
LAB_004dba0b:
                iVar5 = iVar5 + sVar4;
            } else {
                if (field_0x28 - iVar3 != 1) {
                    m_icon->DrawToBuffer(iVar2, iVar5, field_0x46, 0);
                    if (iVar3 < field_0x2a) {
                        if (m_selectedIndex == m_topIndex + iVar3)
                            sVar4 = field_0x2e;
                        else
                            sVar4 = field_0x2c;
                        m_font->DrawBoundedString(
                            static_cast<char *>(m_items[m_topIndex + iVar3]), iVar2 + 5, iVar5 + 2,
                            field_0x64 - 10, m_font->m_height + 1, sVar4, field_0x30);
                    }
                    sVar4 = field_0x5c;
                    goto LAB_004dba0b;
                }
                m_icon->DrawToBuffer(iVar2, iVar5, field_0x48, 0);
                if (iVar3 < field_0x2a) {
                    if (m_selectedIndex == m_topIndex + iVar3)
                        sVar4 = field_0x2e;
                    else
                        sVar4 = field_0x2c;
                    m_font->DrawBoundedString(static_cast<char *>(m_items[m_topIndex + iVar3]),
                                                  iVar2 + 5, iVar5 + 2, field_0x64 - 10,
                                                  m_font->m_height + 1, sVar4, field_0x30);
                }
            }
            iVar3 = iVar3 + 1;
        } while (iVar3 < field_0x28);
    }
    if (field_0x8a == 0)
        sVar4 = field_0x4a;
    else
        sVar4 = field_0x4c;
    m_icon->DrawToBuffer(field_0x68 + m_owner->m_posX, field_0x6a + m_owner->m_posY,
                             sVar4, 0);
    iVar2 = 2;
    m_icon->DrawToBuffer(m_owner->m_posX + field_0x70, m_owner->m_posY + field_0x72,
                             field_0x52, 0);
    if (2 < field_0x28 - 2) {
        do {
            iVar3 = iVar2 - 1;
            iVar2 = iVar2 + 1;
            m_icon->DrawToBuffer(m_owner->m_posX + field_0x70,
                                     field_0x5c * iVar3 + m_owner->m_posY + field_0x72,
                                     field_0x54, 0);
        } while (iVar2 < field_0x28 - 2);
    }
    m_icon->DrawToBuffer(m_owner->m_posX + field_0x70,
                             field_0x5c * (iVar2 - 1) + m_owner->m_posY + field_0x72, field_0x56,
                             0);
    if (field_0x8b == 0)
        sVar4 = field_0x4e;
    else
        sVar4 = field_0x50;
    m_icon->DrawToBuffer(field_0x78 + m_owner->m_posX, field_0x7a + m_owner->m_posY,
                             sVar4, 0);
    sVar4 = static_cast<short>(m_owner->m_posX) + 5 + field_0x70;
    field_0x80 = sVar4;
    if (field_0x42 < 1)
        sVar1 = field_0x88 / 2;
    else
        sVar1 = static_cast<short>((m_topIndex * field_0x88) / field_0x42);
    sVar1 = static_cast<short>(m_owner->m_posY) + field_0x72 + 3 + sVar1;
    field_0x82 = sVar1;
    m_icon->DrawToBuffer(sVar4, sVar1, field_0x58, 0);
    if (param_1 != 0)
        gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                            m_y + m_owner->m_posY, m_width, m_height);
}

VA(0x004dbbe0, 0x312)
int listBoxWidget::ProcessMouseMessage(tag_message &param_1)
{
    short sVar1, sVar2;
    int iVar3, iVar5, iVar6, iVar7, iVar8;
    long lVar4;
    iVar6 = param_1.type;
    iVar7 = param_1.field10 - m_owner->m_posX;
    iVar5 = param_1.field14 - m_owner->m_posY;
    iVar8 = m_listY;
    iVar3 = iVar5 - iVar8;
    if (iVar6 == 4) {
        if (field_0x8d == 0) {
            if (field_0x8c == 0)
                return 0;
            iVar3 = field_0x42;
            iVar6 = ((((iVar5 - field_0x86 / 2) - field_0x72) - 4) * (iVar3 + 1)) / field_0x88;
            if (iVar6 < 0)
                iVar6 = 0;
            if (iVar3 < iVar6)
                iVar6 = iVar3;
            if (m_topIndex == iVar6)
                return 1;
            m_topIndex = static_cast<short>(iVar6);
        } else {
            if (field_0x5a < iVar3)
                iVar6 = (iVar3 - field_0x5a) / field_0x5c + 1;
            else
                iVar6 = 0;
            if (iVar6 < 0)
                iVar6 = 0;
            if (field_0x2a <= iVar6)
                iVar6 = field_0x2a - 1;
            if (m_topIndex + iVar6 == m_selectedIndex)
                return 1;
            m_selectedIndex = static_cast<short>(iVar6) + m_topIndex;
        }
    } else if (iVar6 == 8) {
        if (m_itemCount == 0)
            return 1;
        if (iVar7 < m_listX || iVar5 < iVar8 || field_0x64 + m_listX <= iVar7 ||
            field_0x66 + iVar8 <= iVar5) {
            if (iVar5 < field_0x6a + field_0x6e) {
                if (0 < m_topIndex)
                    m_topIndex = m_topIndex - 1;
                field_0x8a = 1;
            } else if (iVar5 < field_0x7a) {
                if (field_0x82 <= iVar5 && iVar5 < field_0x86 + field_0x82) {
                    field_0x8c = 1;
                    gbSendMouseMoveMessages = 1;
                }
                sVar1 = field_0x42;
                sVar2 = static_cast<short>(((((iVar5 - field_0x86 / 2) - field_0x72) - 4) * (sVar1 + 1)) /
                                           field_0x88);
                m_topIndex = sVar2;
                if (sVar2 < 0)
                    m_topIndex = 0;
                if (sVar1 < m_topIndex)
                    m_topIndex = sVar1;
            } else {
                if (m_topIndex < field_0x42)
                    m_topIndex = m_topIndex + 1;
                field_0x8b = 1;
            }
        } else {
            if (field_0x5a < iVar3)
                iVar6 = m_topIndex + 1 + (iVar3 - field_0x5a) / field_0x5c;
            else
                iVar6 = m_topIndex;
            if (m_itemCount <= iVar6)
                return 1;
            field_0x8d = 1;
            gbSendMouseMoveMessages = 1;
            if (m_selectedIndex == iVar6) {
                gbSendMouseMoveMessages = 1;
                return 1;
            }
            m_selectedIndex = static_cast<short>(iVar6);
        }
    } else {
        if (iVar6 != 0x10)
            return 1;
        gbSendMouseMoveMessages = 0;
        if (field_0x8a == 0 && field_0x8b == 0 && field_0x8c == 0) {
            if (field_0x8d != 0) {
                field_0x8d = 0;
                param_1.field4 = 0xc;
                param_1.type = 0x200;
                param_1.field8 = m_id;
                sVar1 = m_selectedIndex;
                param_1.fieldC = 1;
                param_1.text = reinterpret_cast<char *>(static_cast<int>(sVar1));
                if (field_0x36 == m_selectedIndex &&
                    (iVar6 = field_0x38, lVar4 = KBTickCount(), lVar4 < iVar6 + 400))
                    param_1.fieldC = 2;
                field_0x36 = m_selectedIndex;
                field_0x38 = KBTickCount();
                return 2;
            }
            return 0;
        }
        field_0x8c = 0;
        field_0x8b = 0;
        field_0x8a = 0;
    }
    DrawLBStuff(1);
    return 1;
}


// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(listBoxWidget, 0x004ebac0);
