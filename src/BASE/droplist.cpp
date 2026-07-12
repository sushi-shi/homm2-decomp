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
    m_itemCount = 0;
    m_selectedIndex = -1;
}

VA(0x004dbf60, 0x7c)
dropListWidget::~dropListWidget()
{
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    for (int i = 0; i < m_itemCount; i++)
        BaseFree(m_items[i], __FILE__, __LINE__);
    BaseFree(m_items, __FILE__, __LINE__);
}

VA(0x004dbfe0, 0x21d)
// void dropListWidget::Read(void);

VA(0x004dc200, 0xd5)
// void dropListWidget::DeleteItem(int);

VA(0x004dc2e0, 0x350)
// int dropListWidget::Main(tag_message &);

VA(0x004dc630, 0xaf)
void dropListWidget::Draw(void)
{
    m_icon->DrawToBuffer(m_iconX + m_owner->m_posX, m_iconY + m_owner->m_posY,
                             field_0x48, 0);
    m_icon->DrawToBuffer(field_0x64 + m_owner->m_posX, field_0x66 + m_owner->m_posY,
                             field_0x4a, 0);
    if (m_itemCount > 0 && m_selectedIndex >= 0) {
        int color = 3;
        if ((m_flags & 8) == 0)
            color = m_normalColor;
        m_font->DrawBoundedString(m_items[m_selectedIndex],
                                      m_contentX + m_owner->m_posX,
                                      m_contentY + m_owner->m_posY,
                                      field_0x2c, field_0x2e, color, m_textMode);
    }
}

VA(0x004dc6e0, 0x378)
// void dropListWidget::DrawDropStuff(void);

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
