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



VA(0x004dd440, 0x34)
button::button(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_normalFrame = 0;
    m_pressedFrame = 0;
    m_iconId = 0;
    m_selectMode = 0;
    m_hotkey = BUTTON_NO_HOTKEY;
    m_icon = 0;
}

VA(0x004dd4c0, 0x6e)
button::button(i16 x, i16 y, i16 width, i16 height,
               u32l iconId, i16 normalFrame, i16 pressedFrame,
               i16 selectMode, i16 hotkey, i16 id, i16 kind)
    : widget(x, y, width, height, id, kind)
{
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

VA(0x004dd530, 0x7c)
button::button(i16 x, i16 y, i16 width, i16 height, char *iconName,
               i16 normalFrame, i16 pressedFrame, i16 selectMode,
               i16 hotkey, i16 id, i16 kind)
    : widget(x, y, width, height, id, kind)
{
    u32l iconId = gpResourceManager->MakeId(iconName, 1);
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_normalFrame = normalFrame;
    m_pressedFrame = pressedFrame;
    m_selectMode = selectMode;
    m_hotkey = hotkey;
}

VA(0x004dd5b0, 0xeb)
void button::Read(void)
{
    char iconName[16];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<i8 *>(iconName));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_normalFrame = gpResourceManager->ReadWord();
    m_pressedFrame = gpResourceManager->ReadWord();
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

// @semantic
// Candidate is 0x585 versus retail 0x595. The first non-relocation byte divergence
// is +0x47: candidate loads the vptr after storing m_flags, while retail loads it
// before the store. The modal loop, message CFG, frame, calls, and all 36 ordered
// relocations are present with matching identities. Later residuals are CX/DX flag
// allocation, hit-test register allocation/polarity, and four deselection schedules;
// relocation offsets accumulate
// retail deltas of +6, +12, then +16 bytes. Negative key-up gating, per-site short
// flag snapshots, cached/global flags, hit-test polarity, and goto/break spellings
// were tried. A completed 40-iteration libclang AST pass and 24 guarded TU-state
// probes found no improvement or exact closure. Revisit after a real predecessor,
// header, or TU-state change; correcting m_iconId to its unsigned resource-ID type
// preserved the 0x585 body and first +0x47 divergence. Removing the unused synthetic
// _globals_model include restored the retained 95.57% schedule without changing any
// exact predecessor. This residual remains unresolved; it is not a byte-proven early
// stop.
VA(0x004dd6d0, 0x595)
i32 button::Main(tag_message &msg)
{
    if (DecodeWidgetKind(m_kind) == WIDGET_KIND_AUTO_REPEAT &&
        (m_flags & WIDGET_FLAG_SELECTED) != 0 &&
        KBTickCount() > glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT]) {
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

    i32 eventType = msg.type;
    switch (eventType) {
    case MESSAGE_KEY_DOWN:
        if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0 &&
            (m_flags & WIDGET_FLAG_DIMMED) == 0) {
            if (m_hotkey != BUTTON_NO_HOTKEY &&
                m_hotkey == msg.payload.keyboard.keyCode)
                return Select(msg);
            return 0;
        }
        break;

    case MESSAGE_KEY_UP:
        if ((m_flags & WIDGET_FLAG_ENABLED) != 0 && (m_flags & WIDGET_FLAG_DRAW) != 0 &&
            (m_flags & WIDGET_FLAG_DIMMED) == 0) {
            if (m_hotkey == BUTTON_NO_HOTKEY ||
                m_hotkey != msg.payload.keyboard.keyCode)
                return 0;
            i16 keyFlags = m_flags;
            if ((keyFlags & WIDGET_FLAG_SELECTED) == 0)
                return 0;
            keyFlags &= ~WIDGET_FLAG_SELECTED;
            m_flags = keyFlags;
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
        break;

    case MESSAGE_LEFT_BUTTON_DOWN:
    case MESSAGE_RIGHT_BUTTON_DOWN: {
        if ((m_flags & WIDGET_FLAG_DRAW) == 0)
            goto normalEvent;

        i16 relativeX = static_cast<i16>(msg.payload.mouse.x) -
                          static_cast<i16>(m_owner->m_posX);
        i16 relativeY = static_cast<i16>(msg.payload.mouse.y) -
                          static_cast<i16>(m_owner->m_posY);
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

        if ((m_flags & WIDGET_FLAG_DIMMED) == 0 && m_x <= relativeX && m_y <= relativeY &&
            relativeX < m_x + m_width && relativeY < m_y + m_height) {
            Select(msg);
            while (msg.type != MESSAGE_LEFT_BUTTON_UP && msg.type != MESSAGE_RIGHT_BUTTON_UP) {
                PollSound();
                gpMouseManager->Main(msg);
                if (msg.type == MESSAGE_MOUSE_MOVE) {
                    relativeX = static_cast<i16>(msg.payload.mouse.x) -
                                static_cast<i16>(m_owner->m_posX);
                    relativeY = static_cast<i16>(msg.payload.mouse.y) -
                                static_cast<i16>(m_owner->m_posY);
                    if (m_x > relativeX || m_y > relativeY ||
                        relativeX >= m_x + m_width || relativeY >= m_y + m_height) {
                        i16 moveFlags = m_flags;
                        if ((moveFlags & WIDGET_FLAG_SELECTED) != 0) {
                            moveFlags &= ~WIDGET_FLAG_SELECTED;
                            m_flags = moveFlags;
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
            i16 releaseFlags = m_flags;
            if ((releaseFlags & WIDGET_FLAG_SELECTED) != 0) {
                releaseFlags &= ~WIDGET_FLAG_SELECTED;
                m_flags = releaseFlags;
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
        return 0;
    }

    case MESSAGE_LEFT_BUTTON_UP: {
        i16 releaseFlags = m_flags;
        if ((releaseFlags & WIDGET_FLAG_DRAW) != 0 &&
            (releaseFlags & WIDGET_FLAG_SELECTED) != 0) {
            releaseFlags &= ~WIDGET_FLAG_SELECTED;
            m_flags = releaseFlags;
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
    }

    case MESSAGE_WIDGET:
        if (msg.payload.widget.command == WIDGET_COMMAND_REPLACE_ICON) {
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

// @semantic
// After removing BUTTON's unused synthetic _globals_model include, candidate is 0x95
// versus retail 0x96 and live match returns to 94.72%. The first non-relocation
// divergence is +0x0a: candidate loads both owner coordinates first; retail starts
// button Y in CX, completes X in DX, then adds owner Y to CX. Everything from
// DrawToBuffer onward is instruction-identical after the one-byte setup shift, and all
// 6 ordered relocations agree in type and target. Cached/direct owner access, X/Y
// declaration order, staged Y,
// split/combined sums, and the matching iconWidget staged-coordinate pattern were
// tried. A separate int X/Y form reached only 90.15% and was reverted. Revisit after a
// real predecessor/header TU-state change. This residual remains unresolved; it is not
// a byte-proven early stop.
VA(0x004ddc70, 0x96)
i16 button::Select(struct tag_message &msg)
{
    heroWindow *window = m_owner;
    i16 x = static_cast<i16>(window->m_posX + m_x);
    i16 y = static_cast<i16>(window->m_posY + m_y);
    m_icon->DrawToBuffer(x, y, m_pressedFrame, 0);
    gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
    m_flags |= WIDGET_FLAG_SELECTED;
    msg.type = MESSAGE_WIDGET;
    msg.payload.widget.id = m_id;
    if (m_selectMode == BUTTON_SELECT_DIALOG_RESULT) {
        msg.payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT;
    } else {
        msg.payload.widget.command = WIDGET_COMMAND_SELECT;
    }
    glTimers[GLOBAL_BUTTON_REPEAT_TIMER_SLOT] = KBTickCount() + BUTTON_REPEAT_DELAY_TICKS;
    iLeftRightSave = msg.payload.widget.parameter & MESSAGE_MODIFIER_BUTTON_MASK;
    return 2;
}

// @semantic
// Semantics, types, CFG, frame, 0x83 CodeView body, virtual Draw dispatch, and all four
// ordered relocations are correct. The only residual is scheduling at +0x19/+0x1f:
// retail loads the vptr, stores m_flags, then sets ECX; candidate stores m_flags, sets
// ECX, then loads the vptr. Exact-preserving ctor/dtor inline, dllexport, definition,
// and declaration-placement variants did not change it; a direct compound flag clear
// also emitted the same current-epoch schedule. Variants that changed COMDAT emission
// broke the destructor/alias shape.
VA(0x004ddd10, 0x83)
i16 button::Deselect(struct tag_message &msg)
{
    i16 flags = m_flags;
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
                                 m_pressedFrame, 0);
        return;
    }
    m_icon->DrawToBuffer(m_x + win->m_posX, m_y + win->m_posY,
                             m_normalFrame, 0);
}


// ===== vtable button : public widget  (3 slots) =====
//  [ 0] VA(0x004ddda0, 0x55)  void button::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dd480, 0x36)  void * button::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dd6d0, 0x595)  int button::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(button, 0x004ebaf0);

// ---- globals (definitions, RVA order) ----
DATA(0x0052125c) i32 iLeftRightSave = 0;
