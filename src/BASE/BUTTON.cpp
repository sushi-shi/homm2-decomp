// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BUTTON.OBJ   from: .\basewin.lib
// functions: 11   data: 2
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/button.h>
#include <BASE/widgetKind.h>
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
    m_selectMode = 0;
    m_hotkey = BUTTON_NO_HOTKEY;
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
    m_selectMode = p8;
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
    m_selectMode = p8;
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
    m_selectMode = gpResourceManager->ReadWord();
    m_hotkey = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VA(0x004dd6a0, 0x21)
inline button::~button()
{
    gpResourceManager->Dispose(m_icon);
}

// @early-stop
// Delinker symbol artifact: inline+dllexport emits one strong 0x36 ??_G body and a
// weak sectionless ??_E alias while retail exposes two byte-identical ??_E copies.
// The candidate is relocation-masked raw exact against each copy at 5/5 relocations;
// the separately mapped 0x21 ??1 destructor remains raw exact at 4/4 relocations.
// VA(0x004dd480, 0x36) ??_E/??_G button deleting-destructor aliases

// @match-note
// First executable divergence is the +0x47 vptr-load/store/this scheduling order.
// Base is 0x58d, retail 0x595, with the complete event CFG, modal loop, calls,
// message writes, icon replacement, stackless FPO shape, and 36/36 relocations.
// Later residuals are the flags register (DX versus CX), signed hit-test register
// assignment, repeated deselection blocks, and release-block scheduling. Direct and
// cached flags, signed/unsigned flags, direct/local event type, positive/rejection
// hit tests, helper/expanded deselection, and duplicated/shared widget fallbacks
// were tried; the shared expanded form is retained. Revisit after a real TU/header
// state change or in the >=95% last-mile phase, not by replaying these families.
VA(0x004dd6d0, 0x595)
int button::Main(tag_message &msg)
{
    if (DecodeWidgetKind(m_kind) == WIDGET_KIND_AUTO_REPEAT &&
        (m_flags & WIDGET_FLAG_SELECTED) != 0 && KBTickCount() > gButtonRepeatTime) {
        if ((m_flags & WIDGET_FLAG_SELECTED) == 0)
            return 0;
        m_flags &= ~WIDGET_FLAG_SELECTED;
        Draw();
        gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                            m_y + m_owner->m_posY, m_width, m_height);
        msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.id = m_id;
        msg.payload.widget.parameter = iLeftRightSave;
        iLeftRightSave = 0;
        return 2;
    }

    if ((m_flags & WIDGET_FLAG_ENABLED) == 0) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }

    int eventType = msg.type;
    switch (eventType) {
    case MESSAGE_KEY_DOWN:
        if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0 &&
            (m_flags & WIDGET_FLAG_DIMMED) == 0 && m_hotkey != BUTTON_NO_HOTKEY &&
            m_hotkey == msg.payload.keyboard.keyCode)
            return Select(msg);
        return 0;

    case MESSAGE_KEY_UP:
        if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0 &&
            (m_flags & WIDGET_FLAG_DIMMED) == 0 && m_hotkey != BUTTON_NO_HOTKEY &&
            m_hotkey == msg.payload.keyboard.keyCode) {
            if ((m_flags & WIDGET_FLAG_SELECTED) == 0)
                return 0;
            m_flags &= ~WIDGET_FLAG_SELECTED;
            Draw();
            gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                m_y + m_owner->m_posY, m_width, m_height);
            msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.id = m_id;
            msg.payload.widget.parameter = iLeftRightSave;
            iLeftRightSave = 0;
            return 2;
        }
        return 0;

    case MESSAGE_LEFT_BUTTON_DOWN:
    case MESSAGE_RIGHT_BUTTON_DOWN: {
        if ((m_flags & WIDGET_FLAG_DRAW) == 0)
            goto normalEvent;

        short relativeX = static_cast<short>(msg.payload.mouse.x) -
                          static_cast<short>(m_owner->m_posX);
        short relativeY = static_cast<short>(msg.payload.mouse.y) -
                          static_cast<short>(m_owner->m_posY);
        if (eventType == MESSAGE_RIGHT_BUTTON_DOWN) {
            if (m_x <= relativeX && m_y <= relativeY &&
                relativeX < m_x + m_width && relativeY < m_y + m_height) {
                msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.id = m_id;
                msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                return 2;
            }
            return 0;
        }

        if ((m_flags & WIDGET_FLAG_DIMMED) != 0 || m_x > relativeX || m_y > relativeY ||
            relativeX >= m_x + m_width || relativeY >= m_y + m_height)
            return 0;

        Select(msg);
        while (msg.type != MESSAGE_LEFT_BUTTON_UP && msg.type != MESSAGE_RIGHT_BUTTON_UP) {
            PollSound();
            gpMouseManager->Main(msg);
            if (msg.type == MESSAGE_MOUSE_MOVE) {
                relativeX = static_cast<short>(msg.payload.mouse.x) -
                            static_cast<short>(m_owner->m_posX);
                relativeY = static_cast<short>(msg.payload.mouse.y) -
                            static_cast<short>(m_owner->m_posY);
                if (m_x > relativeX || m_y > relativeY ||
                    relativeX >= m_x + m_width || relativeY >= m_y + m_height) {
                    if ((m_flags & WIDGET_FLAG_SELECTED) != 0) {
                        m_flags &= ~WIDGET_FLAG_SELECTED;
                        Draw();
                        gpWindowManager->UpdateScreenRegion(
                            m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
                        msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
                        msg.type = MESSAGE_WIDGET;
                        msg.payload.widget.id = m_id;
                        msg.payload.widget.parameter = iLeftRightSave;
                        iLeftRightSave = 0;
                    }
                } else if ((m_flags & WIDGET_FLAG_SELECTED) == 0) {
                    Select(msg);
                }
            }
            Process1WindowsMessage();
            msg = gpInputManager->GetEvent();
        }
        if ((m_flags & WIDGET_FLAG_SELECTED) != 0) {
            m_flags &= ~WIDGET_FLAG_SELECTED;
            Draw();
            gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                m_y + m_owner->m_posY, m_width, m_height);
            msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.id = m_id;
            msg.payload.widget.parameter = iLeftRightSave;
            iLeftRightSave = 0;
            return 2;
        }
        return 1;
    }

    case MESSAGE_LEFT_BUTTON_UP:
        if ((m_flags & WIDGET_FLAG_DRAW) != 0 && (m_flags & WIDGET_FLAG_SELECTED) != 0) {
            m_flags &= ~WIDGET_FLAG_SELECTED;
            Draw();
            gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                m_y + m_owner->m_posY, m_width, m_height);
            msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
            msg.type = MESSAGE_WIDGET;
            msg.payload.widget.id = m_id;
            msg.payload.widget.parameter = iLeftRightSave;
            iLeftRightSave = 0;
            return 2;
        }
        goto normalEvent;

    case MESSAGE_WIDGET:
        if (msg.payload.widget.command == BUTTON_COMMAND_REPLACE_ICON) {
            if (msg.payload.widget.id == m_iconId) {
                m_iconId = msg.payload.widget.data.value;
                gpResourceManager->Dispose(m_icon);
                m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.value);
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

// @match-note
// In the exact-alias TU state the message tail is exact; the remaining divergence is
// coordinate register allocation. Cached/direct owner access, X/Y declaration order,
// and split/combined additions were tried. Direct X-then-Y sums plus the positive
// m_selectMode arm establish the retained 94.72% source-hash maximum. Revisit only
// after a real TU/header-state change or in the >=95% last-mile phase.
VA(0x004ddc70, 0x96)
short button::Select(struct tag_message &msg)
{
    short x = static_cast<short>(m_owner->m_posX + m_x);
    short y = static_cast<short>(m_owner->m_posY + m_y);
    m_icon->DrawToBuffer(x, y, m_normalFrame, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= WIDGET_FLAG_SELECTED;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.id = m_id;
    if (m_selectMode == BUTTON_SELECT_DIALOG_RESULT) {
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    } else {
        msg.payload.widget.command = WIDGET_COMMAND_SELECT;
    }
    gButtonRepeatTime = KBTickCount() + BUTTON_REPEAT_DELAY_TICKS;
    iLeftRightSave = msg.payload.widget.parameter & MESSAGE_MODIFIER_BUTTON_MASK;
    return 2;
}

// @match-note
// Base and retail are 0x83 with 4/4 relocations and two relocation-masked byte
// differences. Only the vptr/store/this three-instruction schedule differs; the
// virtual call and every later message byte are exact. Revisit in the >=95% phase.
VA(0x004ddd10, 0x83)
short button::Deselect(struct tag_message &msg)
{
    short flags = m_flags;
    if ((flags & WIDGET_FLAG_SELECTED) == 0)
        return 0;
    flags &= ~WIDGET_FLAG_SELECTED;
    m_flags = flags;
    Draw();
    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                        m_y + m_owner->m_posY, m_width, m_height);
    msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.id = m_id;
    msg.payload.widget.parameter = iLeftRightSave;
    iLeftRightSave = 0;
    return 2;
}

VA(0x004ddda0, 0x55)
void button::Draw(void)
{
    heroWindow *win = m_owner;
    if ((m_flags & WIDGET_FLAG_SELECTED) != 0) {
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
