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

// @early-stop
// Compiler COMDAT-folding artifact: retail carries two strong, byte-identical 0x36
// ??_E/??_G deleting-destructor sections (10 relocations apiece), while retaining the
// exact standalone 0x21 ??1 destructor above makes VC4.2 emit a 0x1f ??_G wrapper and
// a weak ??_E alias with no section. Making the destructor inline produces the folded
// deleting body but removes the separately mapped exact ??1 symbol, so the ABI-correct
// standalone destructor is retained rather than traded for weak aliases.
// VA(0x004dd480, 0x36) ??_E/??_G button deleting-destructor aliases

// @early-stop
// /O2 register-allocation/scheduling wall: base is 0x58d bytes, retail 0x595, and
// all 36 relocation targets agree. Residual spans are the vptr-load/store ordering
// at +0x46..+0x50, flag allocation across keyboard dispatch +0xb7..+0x22a (DX in
// base versus CX in retail), signed hit-test orientation and repeated deselection
// blocks +0x236..+0x4ba, and the release block +0x4c6..+0x54b. The event tree,
// modal input loop, all calls, message writes, and icon replacement agree. Direct
// versus cached flags, signed/unsigned flags, direct versus local event type,
// positive/rejection hit tests, helper calls versus expanded deselection, and
// duplicated/shared widget fallbacks were tried; the shared expanded form is best.
VA(0x004dd6d0, 0x595)
int button::Main(tag_message &msg)
{
    if (field_0x14 == 0x1000 && (m_flags & 1) != 0 && KBTickCount() > gButtonRepeatTime) {
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

    if ((m_flags & 2) == 0) {
        if (msg.type == 0x200)
            return widget::Main(msg);
        return 0;
    }

    int eventType = msg.type;
    switch (eventType) {
    case 1:
        if ((m_flags & 2) != 0 && (m_flags & 4) != 0 && (m_flags & 8) == 0 && m_hotkey != -1 &&
            m_hotkey == msg.field4)
            return Select(msg);
        return 0;

    case 2:
        if ((m_flags & 2) != 0 && (m_flags & 4) != 0 && (m_flags & 8) == 0 && m_hotkey != -1 &&
            m_hotkey == msg.field4) {
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
        return 0;

    case 8:
    case 0x20: {
        if ((m_flags & 4) == 0)
            goto normalEvent;

        short relativeX = static_cast<short>(msg.field4) -
                          static_cast<short>(m_owner->m_posX);
        short relativeY = static_cast<short>(msg.field8) -
                          static_cast<short>(m_owner->m_posY);
        if (eventType == 0x20) {
            if (m_x <= relativeX && m_y <= relativeY &&
                relativeX < m_x + m_width && relativeY < m_y + m_height) {
                msg.field4 = 0xe;
                msg.type = 0x200;
                msg.field8 = m_id;
                msg.fieldC = 0x200;
                return 2;
            }
            return 0;
        }

        if ((m_flags & 8) != 0 || m_x > relativeX || m_y > relativeY ||
            relativeX >= m_x + m_width || relativeY >= m_y + m_height)
            return 0;

        Select(msg);
        while (msg.type != 0x10 && msg.type != 0x40) {
            PollSound();
            gpMouseManager->Main(msg);
            if (msg.type == 4) {
                relativeX = static_cast<short>(msg.field4) -
                            static_cast<short>(m_owner->m_posX);
                relativeY = static_cast<short>(msg.field8) -
                            static_cast<short>(m_owner->m_posY);
                if (m_x > relativeX || m_y > relativeY ||
                    relativeX >= m_x + m_width || relativeY >= m_y + m_height) {
                    if ((m_flags & 1) != 0) {
                        m_flags &= 0xfffe;
                        Draw();
                        gpWindowManager->UpdateScreenRegion(
                            m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
                        msg.field4 = 0xd;
                        msg.type = 0x200;
                        msg.field8 = m_id;
                        msg.fieldC = iLeftRightSave;
                        iLeftRightSave = 0;
                    }
                } else if ((m_flags & 1) == 0) {
                    Select(msg);
                }
            }
            Process1WindowsMessage();
            msg = gpInputManager->GetEvent();
        }
        if ((m_flags & 1) != 0) {
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
        return 1;
    }

    case 0x10:
        if ((m_flags & 4) != 0 && (m_flags & 1) != 0) {
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
        goto normalEvent;

    case 0x200:
        if (msg.field4 == 0x3c) {
            if (msg.field8 == m_iconId) {
                m_iconId = msg.field18;
                gpResourceManager->Dispose(m_icon);
                m_icon = gpResourceManager->GetIcon(msg.field18);
            }
            return 0;
        }
        goto normalEvent;

    default:
        goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

// @early-stop
// /O2 scheduling wall: base is 0x95 bytes, retail 0x96, with all 6 relocation
// targets agreeing. Only coordinate preparation +0x08..+0x2d and message-store
// scheduling +0x50..+0x6f differ; retail retains the owner pointer while base CSEs
// both owner coordinates into AX/CX. The final global-name difference is the same
// single-module gButtonRepeatTime address. Cached/direct owner access, x/y declaration
// order, split compound additions, and both field4/field8 assignment orders were tried.
VA(0x004ddc70, 0x96)
short button::Select(struct tag_message &msg)
{
    heroWindow *window = m_owner;
    short y = m_y;
    short x = static_cast<short>(window->m_posX);
    x += m_x;
    y += static_cast<short>(window->m_posY);
    m_icon->DrawToBuffer(x, y, m_normalFrame, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= 1;
    msg.type = 0x200;
    msg.field4 = 10;
    msg.field8 = m_id;
    if (field_0x28 != 1)
        msg.field4 = 0xc;
    gButtonRepeatTime = KBTickCount() + 0x3c;
    iLeftRightSave = msg.fieldC & 0x300;
    return 2;
}

// @early-stop
// /O2 three-instruction scheduling wall: base and retail are both 0x83 bytes and
// all 4 relocation targets agree. Every instruction matches except base +0x18..+0x20
// (`mov ecx,this; store flags; load vptr`) versus retail +0x18..+0x20 (`load vptr;
// store flags; mov ecx,this`). Direct `&=` and an explicit short flag temporary both
// select the base order; subsequent virtual call and all message bytes are identical.
VA(0x004ddd10, 0x83)
short button::Deselect(struct tag_message &msg)
{
    short flags = m_flags;
    if ((flags & 1) == 0)
        return 0;
    flags &= 0xfffe;
    m_flags = flags;
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
