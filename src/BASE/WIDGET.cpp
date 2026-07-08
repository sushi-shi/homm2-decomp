// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WIDGET.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/widget.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>
VA(0x004dde00, 0x5a)
widget::widget(short int x, short int y, short int w, short int h, short int p5, short int kind)
{
    m_x = x;
    m_owner = 0;
    m_y = y;
    m_next = 0;
    m_prev = 0;
    m_flags = 6;
    m_zOrder = -1;
    m_width = w;
    m_height = h;
    m_id = p5;
    field_0x14 = kind;
}

VA(0x004dde60, 0x3f)
widget::widget(void)
{
    m_id = 0;
    m_owner = 0;
    m_next = 0;
    m_prev = 0;
    m_flags = 6;
    m_zOrder = -1;
    field_0x14 = 2;
    m_y = 0;
    m_x = 0;
    m_width = 0x10;
    m_height = 0x10;
}

VA(0x004ddea0, 0x7)
widget::~widget() {}

VA(0x004ddeb0, 0x14)
int widget::Open(int id, class heroWindow *win)
{
    m_zOrder = id;
    m_owner = win;
    return 0;
}

VA(0x004dded0, 0x1)
void widget::Close(void) {}

VA(0x004ddee0, 0x2f4)
int widget::Main(tag_message &param_1)
{
    unsigned int uVar1;
    unsigned short uVar2;
    short sVar3, sVar4;
    unsigned char *pFlagsHi = reinterpret_cast<unsigned char *>(&m_flags) + 1;
    if (param_1.type == 4) {
        sVar3 = static_cast<short>(param_1.field4) - m_owner->m_posX;
        sVar4 = static_cast<short>(param_1.field8) - m_owner->m_posY;
        if (m_x <= sVar3 && m_y <= sVar4 && sVar3 < m_width + m_x &&
            sVar4 < m_height + m_y) {
            param_1.field8 = m_id;
            return 2;
        }
    } else if (param_1.type == 0x200) {
        switch (param_1.field4) {
        case 2:
            if ((m_flags & 4) != 0)
                Draw();
            if ((m_flags & 8) != 0 && field_0x14 != 8 && field_0x14 != 0x200) {
                short x = m_x + m_owner->m_posX;
                short y = m_y + m_owner->m_posY;
                DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
                return 0;
            }
            break;
        case 5:
            if (m_id == param_1.field8) {
                if (reinterpret_cast<int>(param_1.text) == 0x1000) {
                    m_flags = m_flags | 8;
                    return 1;
                }
                uVar2 = m_flags | static_cast<unsigned short>(reinterpret_cast<int>(param_1.text));
                m_flags = uVar2;
                if ((uVar2 & 8) != 0) {
                    Draw();
                    if (field_0x14 != 8 && field_0x14 != 0x200) {
                        short x = m_x + m_owner->m_posX;
                        short y = m_y + m_owner->m_posY;
                        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
                    }
                }
                if ((*pFlagsHi & 0x40) != 0) {
                    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                        m_y + m_owner->m_posY,
                                                        m_width, m_height);
                    *pFlagsHi = *pFlagsHi & 0xbf;
                }
                return 1;
            }
            break;
        case 6:
            if (m_id == param_1.field8) {
                uVar1 = reinterpret_cast<int>(param_1.text);
                if (uVar1 == 0x1000) {
                    m_flags = m_flags & 0xf7;
                    return 1;
                }
                m_flags = m_flags & ~static_cast<unsigned short>(uVar1);
                if ((uVar1 & 8) != 0)
                    Draw();
                if ((uVar1 & 0x4000) != 0)
                    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                        m_y + m_owner->m_posY,
                                                        m_width, m_height);
                return 1;
            }
            break;
        case 0x34:
            if (m_id == param_1.field8) {
                m_x = static_cast<short>(reinterpret_cast<int>(param_1.text));
                return 1;
            }
            break;
        case 0x35:
            if (m_id == param_1.field8) {
                m_y = static_cast<short>(reinterpret_cast<int>(param_1.text));
                return 1;
            }
            break;
        case 0x3d:
            if (m_id == param_1.field8) {
                m_width = static_cast<short>(reinterpret_cast<int>(param_1.text));
                return 1;
            }
        }
    }
    return 0;
}

VA(0x004de1e0, 0x47)
void widget::Dim(void)
{
    if (field_0x14 != 8 && field_0x14 != 0x200) {
        short x = m_owner->m_posX + m_x;
        short y = m_y + m_owner->m_posY;
        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
    }
}


// ===== vtable widget (root)  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(widget, 0x004ebb00);
