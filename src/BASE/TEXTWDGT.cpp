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
    m_color = 1;
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
    m_color = p7;
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
    m_color = gpResourceManager->ReadWord() & 0xff;
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
// int textWidget::Main(tag_message &);

VA(0x004d1490, 0x49)
void textWidget::Draw(void)
{
    int color = 3;
    if ((m_flags & 8) == 0)
        color = m_color;
    m_font->DrawBoundedString(m_text, m_x + m_owner->m_posX,
                                  m_y + m_owner->m_posY, m_width, m_height,
                                  color, field_0x2a);
}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(short int param_1)
{
    m_color = param_1;
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
