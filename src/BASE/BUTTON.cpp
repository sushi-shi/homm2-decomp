// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BUTTON.OBJ   from: .\basewin.lib
// functions: 11   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/button.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/kbwin.h>
#include <BASE/mouseManager.h>
#include <BASE/inputManager.h>
#include <SOURCE/KB.h>
#include <_globals_model.h>



// ---- module-private synthetic globals (retail xref: single-module) ----
DATA(0x00528cf8) static long gButtonRepeatTime; // button auto-repeat deadline tick (button::Select)

VA(0x004dd440, 0x34)
button::button(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_pressedFrame = 0;
    m_normalFrame = 0;
    m_iconId = 0;
    field_0x28 = 0;
    m_hotkey = -1;
    m_icon = 0;
}

VA(0x004dd4c0, 0x6e)
button::button(short int x, short int y, short int w, short int h, unsigned long int iconId, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_pressedFrame = p6;
    m_normalFrame = p7;
    field_0x28 = p8;
    m_hotkey = p9;
}

VA(0x004dd530, 0x7c)
button::button(short int x, short int y, short int w, short int h, char *name, short int p6, short int p7, short int p8, short int p9, short int p10, short int p11)
    : widget(x, y, w, h, p10, p11)
{
    unsigned long id = gpResourceManager->MakeId(name, 1);
    m_iconId = id;
    m_icon = gpResourceManager->GetIcon(id);
    m_pressedFrame = p6;
    m_normalFrame = p7;
    field_0x28 = p8;
    m_hotkey = p9;
}

VA(0x004dd5b0, 0xeb)
void button::Read(void)
{
    char local_10[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<signed char *>(local_10));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(local_10, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_pressedFrame = gpResourceManager->ReadWord();
    m_normalFrame = gpResourceManager->ReadWord();
    field_0x28 = gpResourceManager->ReadWord();
    m_hotkey = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    field_0x14 = gpResourceManager->ReadWord();
}

VA(0x004dd6a0, 0x21)
button::~button()
{
    gpResourceManager->Dispose(m_icon);
}

VA(0x004dd6d0, 0x595)
// int button::Main(tag_message &);

VA(0x004ddc70, 0x96)
short button::Select(struct tag_message &msg)
{
    short x = m_owner->m_posX + m_x;
    short y = m_y + m_owner->m_posY;
    m_icon->DrawToBuffer(x, y, m_normalFrame, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= 1;
    msg.type = 0x200;
    msg.field8 = m_id;
    msg.field4 = 10;
    if (field_0x28 != 1)
        msg.field4 = 0xc;
    gButtonRepeatTime = KBTickCount() + 0x3c;
    iLeftRightSave = msg.fieldC & 0x300;
    return 2;
}

VA(0x004ddd10, 0x83)
short button::Deselect(struct tag_message &msg)
{
    if ((m_flags & 1) == 0)
        return 0;
    m_flags &= 0xfffe;
    Draw();
    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                        m_y + m_owner->m_posY, m_width, m_height);
    msg.field4 = 0xd;
    msg.type = 0x200;
    msg.field8 = m_id;
    msg.fieldC = iLeftRightSave;
    iLeftRightSave = 0;
    return 2;
}

VA(0x004ddda0, 0x55)
void button::Draw(void)
{
    heroWindow *win = m_owner;
    if ((m_flags & 1) != 0) {
        m_icon->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY,
                                 m_normalFrame, 0);
        return;
    }
    m_icon->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY,
                             m_pressedFrame, 0);
}


// ===== vtable button : public widget  (3 slots) =====
//  [ 0] VA(0x004ddda0, 0x55)  void button::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd480, 0x36)  void * button::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd6d0, 0x595)  int button::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(button, 0x004ebaf0);

// ---- globals (definitions, RVA order) ----
DATA(0x0052125c) int iLeftRightSave;
