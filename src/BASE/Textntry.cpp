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
    field_0x2b = 0;
    field_0x14 = 0x4000;
    field_0x33 = 0;
    field_0x2f = 0;
    field_0x4b = 0;
}

VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(short p1, short p2, short p3, short p4, short p5, char *p6,
                                 char *p7, short p8, char *p9, short p10, short p11, short p12,
                                 short p13, int p14, int p15)
    : textWidget(p1, p2, p3, p4, p6, p7, p8, p11, p12, 1)
{
    field_0x33 = p5;
    field_0x31 = 0;
    field_0x2b = gpResourceManager->GetIcon(p9);
    field_0x4b = 0;
    field_0x14 = 0x4000;
    field_0x2f = p10;
    field_0x45 = 1;
    field_0x47 = 0;
    field_0x28 = 1;
    field_0x35 = m_x;
    field_0x37 = m_y;
    field_0x39 = m_width;
    field_0x33 = p5;
    field_0x3b = m_height;
    field_0x20 = static_cast<char *>(BaseAlloc(p5 + 5, __FILE__, __LINE__));
    strcpy(field_0x20, p6);
    if (p13 == 4) {
        field_0x47 = 1;
        field_0x41 = m_x + p14;
        field_0x43 = m_y + p15;
        field_0x3d = m_width + p14 * -2;
        field_0x3f = m_height;
    }
}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget()
{
    gpResourceManager->Dispose(field_0x2b);
}

VA(0x004d8920, 0x26c)
void textEntryWidget::Read(int param_1)
{
    short sVar1;
    unsigned short uVar2;
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    uVar2 = gpResourceManager->ReadWord();
    field_0x33 = uVar2;
    field_0x20 = static_cast<char *>(BaseAlloc(uVar2 + 5, __FILE__, __LINE__));
    gpResourceManager->ReadBlock(reinterpret_cast<signed char *>(field_0x20), field_0x33);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x24 = gpResourceManager->GetFont(local_10);
    gpResourceManager->RestorePosition();
    uVar2 = gpResourceManager->ReadWord();
    field_0x28 = uVar2 & 0xff;
    sVar1 = gpResourceManager->ReadWord();
    field_0x2a = static_cast<char>(sVar1);
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    field_0x2b = gpResourceManager->GetIcon(local_10);
    gpResourceManager->RestorePosition();
    field_0x49 = static_cast<short>(param_1);
    if (param_1 == 2) {
        field_0x35 = gpResourceManager->ReadWord();
        field_0x37 = gpResourceManager->ReadWord();
        field_0x39 = gpResourceManager->ReadWord();
        field_0x3b = gpResourceManager->ReadWord();
        field_0x45 = gpResourceManager->ReadWord();
        field_0x47 = gpResourceManager->ReadWord();
    } else {
        field_0x35 = m_x;
        field_0x37 = m_y;
        field_0x39 = m_width;
        field_0x3b = m_height;
        field_0x45 = 1;
        field_0x47 = 1;
        if (param_1 != 3)
            field_0x47 = 0;
    }
    if (param_1 == 4) {
        field_0x41 = m_x + 7;
        field_0x43 = m_y + 5;
        field_0x3d = m_width - 0xe;
        field_0x3f = m_height;
    } else if (param_1 == 5) {
        field_0x41 = m_x + 7;
        field_0x43 = m_y + 4;
        field_0x3d = m_width - 0xe;
        field_0x3f = m_height;
    } else {
        field_0x41 = m_x;
        field_0x43 = m_y;
        field_0x3d = m_width;
        field_0x3f = m_height;
    }
    field_0x2f = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    field_0x14 = 0x4000;
}

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
