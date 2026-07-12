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
// void listBoxWidget::Read(void);

VA(0x004db3d0, 0x142)
// void listBoxWidget::DeleteItem(int);

VA(0x004db520, 0x368)
// int listBoxWidget::Main(tag_message &);

VA(0x004db890, 0x8)
void listBoxWidget::Draw(void)
{
    DrawLBStuff(0);
}

VA(0x004db8a0, 0x334)
// void listBoxWidget::DrawLBStuff(int);

VA(0x004dbbe0, 0x312)
// int listBoxWidget::ProcessMouseMessage(tag_message &);


// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(listBoxWidget, 0x004ebac0);
