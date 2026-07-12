// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\Textntry.obj   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/textEntryWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <string.h>
VA(0x004d8740, 0x2d)
textEntryWidget::textEntryWidget(void) : textWidget()
{
    field_0x31 = 0;
    m_icon = 0;
    field_0x14 = 0x4000;
    m_maxLength = 0;
    field_0x2f = 0;
    field_0x4b = 0;
}

VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(short p1, short p2, short p3, short p4, short p5, char *p6,
                                 char *p7, short p8, char *p9, short p10, short p11, short p12,
                                 short p13, int p14, int p15)
    : textWidget(p1, p2, p3, p4, p6, p7, p8, p11, p12, 1)
{
    m_maxLength = p5;
    field_0x31 = 0;
    m_icon = gpResourceManager->GetIcon(p9);
    field_0x4b = 0;
    field_0x14 = 0x4000;
    field_0x2f = p10;
    field_0x45 = 1;
    m_hasInset = 0;
    m_color = 1;
    m_rectX = m_x;
    m_rectY = m_y;
    m_rectW = m_width;
    m_maxLength = p5;
    m_rectH = m_height;
    m_text = static_cast<char *>(BaseAlloc(p5 + 5, __FILE__, __LINE__));
    strcpy(m_text, p6);
    if (p13 == 4) {
        m_hasInset = 1;
        m_innerX = m_x + p14;
        m_innerY = m_y + p15;
        m_innerW = m_width + p14 * -2;
        m_innerH = m_height;
    }
}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget()
{
    gpResourceManager->Dispose(m_icon);
}

VA(0x004d8920, 0x26c)
// void textEntryWidget::Read(int);

VA(0x004d8b90, 0x874)
int textEntryWidget::Main(struct tag_message &) { return 0; }

VA(0x004d9410, 0x160)
void textEntryWidget::Draw(void) {}

VA(0x004d9570, 0x1be)
void textEntryWidget::SetupDisplayString(char *, unsigned short int) {}


// ===== vtable textEntryWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d9410, 0x160)  void textEntryWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d8770, 0x36)  void * textEntryWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d8b90, 0x874)  int textEntryWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(textEntryWidget, 0x004ebaa0);
