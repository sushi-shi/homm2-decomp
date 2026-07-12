// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\ICONWDGT.OBJ   from: .\basewin.lib
// functions: 9   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_frame = 0;
    field_0x27 = 0;
    m_icon = 0;
    m_flip = 0;
    m_iconId = 0;
}

// @early-stop ~16% — prologue through GetIcon is byte-identical; only the final 5 field
// stores differ, purely /O2 register allocation (retail loads param_6→cx first for an
// ascending-offset store order; ours loads param_10→cx first, descending). Not
// source-controllable — reordering the assignments produces byte-identical output.
VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       unsigned long int param_5, short int param_6, signed char param_7,
                       short int param_8, short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    m_iconId = param_5;
    m_icon = gpResourceManager->GetIcon(param_5);
    m_frame = param_6;
    m_flip = param_7;
    field_0x27 = param_10;
    field_0x14 = param_9;
}

// @early-stop ~13% — same /O2 register-scheduling wall as the sibling ctor above; body
// is semantically exact (MakeId+GetIcon then the same field stores).
VA(0x004d0b40, 0x78)
iconWidget::iconWidget(short int param_1, short int param_2, short int param_3, short int param_4,
                       char *param_5, short int param_6, signed char param_7, short int param_8,
                       short int param_9, short int param_10)
    : widget(param_1, param_2, param_3, param_4, param_8, param_9)
{
    unsigned long int uVar1 = gpResourceManager->MakeId(param_5, 1);
    m_iconId = uVar1;
    m_icon = gpResourceManager->GetIcon(uVar1);
    m_frame = param_6;
    m_flip = param_7;
    field_0x27 = param_10;
    field_0x14 = param_9;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    unsigned long id = gpResourceManager->MakeId(local_10, 1);
    m_icon = gpResourceManager->GetIcon(id);
    gpResourceManager->RestorePosition();
    m_frame = gpResourceManager->ReadWord();
    m_flip = static_cast<char>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
    field_0x27 = gpResourceManager->ReadWord() & 0xff;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget()
{
    gpResourceManager->Dispose(m_icon);
}

VA(0x004d0cd0, 0x291)
// int iconWidget::Main(tag_message &);

VA(0x004d0f70, 0xe5)
// void iconWidget::Draw(void);


// ===== vtable iconWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004d0f70, 0xe5)  void iconWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d0a90, 0x36)  void * iconWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d0cd0, 0x291)  int iconWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(iconWidget, 0x004eba40);
