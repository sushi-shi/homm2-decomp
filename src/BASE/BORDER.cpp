// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\BORDER.OBJ   from: .\basewin.lib
// functions: 8   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/border.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>
VA(0x004d20a0, 0x32)
border::border(void) : widget(0, 0, 0, 0, 0, 0) {
    m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    m_fillColor = 0;
}

// Generated-alias checkpoint: candidate emits a 0x1f ??_G with 2 relocations that
// calls the exact standalone destructor; each retail ??_E is 0x4d with 7 relocations
// and inlines both resource disposals plus widget::~widget. Inlining border::~border
// recovers that alias body but removes the mapped standalone ??1. This is a real
// emission tradeoff, not a relocation artifact; preserve the exact standalone body.
// VA(0x004d20e0, 0x4d) ??_E/??_G border deleting-destructor aliases

VA(0x004d2130, 0x64)
border::border(i16 x, i16 y, i16 w, i16 h, i16 e, i16 f, i16 fillColor, char* name)
    : widget(x, y, w, h, e, f) {
    if (name != 0)
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
    else
        m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    m_fillColor = fillColor;
}

VA(0x004d21a0, 0x38)
border::~border() {
    if (m_backgroundBitmap != 0)
        gpResourceManager->Dispose(m_backgroundBitmap);
    if (m_backgroundIcon != 0)
        gpResourceManager->Dispose(m_backgroundIcon);
}

VA(0x004d21e0, 0x10e)
void border::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    i16 kind = gpResourceManager->ReadWord();
    m_backgroundBitmap = 0;
    m_backgroundIcon = 0;
    m_kind = kind;
    char name[16];
    if (DecodeWidgetKind(kind) == WIDGET_KIND_BITMAP) {
        gpResourceManager->Read13(reinterpret_cast<i8*>(name));
        gpResourceManager->SavePosition();
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
        gpResourceManager->RestorePosition();
        return;
    }
    if (DecodeWidgetKind(kind) == WIDGET_KIND_ICON) {
        gpResourceManager->Read13(reinterpret_cast<i8*>(name));
        gpResourceManager->SavePosition();
        m_backgroundIcon = gpResourceManager->GetIcon(name);
        gpResourceManager->RestorePosition();
        return;
    }
    m_fillColor = gpResourceManager->ReadWord() & 0xFF;
}

// @semantic
// The shared WidgetKind header invalidates the former raw-exact TU state. Both
// sections remain 0x181 bytes with 9/9 relocations and matching external calls;
// explicit-range comparison differs only at +0x78 and +0x7b, where the equivalent
// signed comparison uses the opposite CMP/branch orientation. Enum tag, include
// position, semantic enumerator-name variants, and a retail-shaped negative hit-test
// rejection did not change these bytes.
VA(0x004d22f0, 0x181)
i32 border::Main(struct tag_message& msg) {
    i16 flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }
    i32 type = msg.type;
    switch (type) {
        default:
            goto normalEvent;
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            goto hoverEvent;
        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            goto leaveEvent;
    }

normalEvent:
    return widget::Main(msg);

hoverEvent: {
    i16 mx = static_cast<i16>(msg.payload.mouse.x);
    heroWindow* window = m_owner;
    mx -= static_cast<i16>(window->m_posX);
    i16 my = static_cast<i16>(msg.payload.mouse.y) - window->m_posY;
    if (m_x <= mx && m_y <= my && mx < m_width + m_x && my < m_height + m_y) {
        if (type == MESSAGE_RIGHT_BUTTON_DOWN) {
            msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
            msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
        } else {
            m_flags = flags | WIDGET_FLAG_SELECTED;
            msg.payload.widget.command = WIDGET_COMMAND_SELECT;
        }
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.id = m_id;
        return 2;
    }
    return 0;
}

leaveEvent:
    if ((flags & WIDGET_FLAG_SELECTED) != 0) {
        m_flags = flags & 0xfffe;
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
        msg.payload.widget.id = m_id;
        return 2;
    }
    return 0;
}

VA(0x004d2480, 0xab)
void border::Draw(void) {
    i16 y = m_y + static_cast<i16>(m_owner->m_posY);
    i16 x = m_x + static_cast<i16>(m_owner->m_posX);
    i32 kind = m_kind;
    switch (kind) {
        case WIDGET_KIND_SOLID:
            FillBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, m_fillColor);
            return;
        case WIDGET_KIND_BITMAP:
            PollSound();
            BlitBitmap(
                m_backgroundBitmap,
                0,
                0,
                m_width,
                m_height,
                gpWindowManager->m_screen,
                x,
                y
            );
            PollSound();
            return;
        case WIDGET_KIND_ICON:
            m_backgroundIcon->DrawToBuffer(x, y, 0, 0);
            return;
        default:
            return;
    }
}

// ===== vtable border : public widget  (3 slots) =====
//  [ 0] VA(0x004d2480, 0xab)  void border::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004d20e0, 0x4d)  void * border::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004d22f0, 0x181)  int border::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(border, 0x004eba60);
