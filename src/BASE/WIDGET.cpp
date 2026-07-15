// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WIDGET.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/widget.h>
#include <BASE/widgetKind.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>

VA(0x004dde00, 0x5a)
widget::widget(short int x, short int y, short int width, short int height, short int id, short int kind)
{
    m_owner = 0;
    m_next = 0;
    m_prev = 0;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_id = id;
    m_kind = kind;
}

VA(0x004dde60, 0x3f)
widget::widget(void)
{
    m_id = 0;
    m_owner = 0;
    m_next = 0;
    m_prev = 0;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_kind = EncodeWidgetKind(WIDGET_KIND_DEFAULT);
    m_y = 0;
    m_x = 0;
    m_width = 0x10;
    m_height = 0x10;
}

VA(0x004ddea0, 0x7)
widget::~widget() {}

VA(0x004ddeb0, 0x14)
int widget::Open(int zOrder, class heroWindow *owner)
{
    m_zOrder = zOrder;
    m_owner = owner;
    return 0;
}

// @early-stop
// Delinker symbol artifact: base emits the named 1-byte Close section and retail
// maps the same lone C3 byte as `empty_stub`; neither side has relocations.
VA(0x004dded0, 0x1)
void widget::Close(void) {}

// @semantic
// Both sections are 0x2f4 with the same frame/CFG and all 17/17 ordered
// relocations. Explicit relocation-masked comparison differs only at
// +0x23,+0x38,+0x46,+0x4a,+0x4f,+0x51 in the mouse hit test: ours retains the
// left edge in BX/EBP while retail uses BP/EBX. The dispatcher from +0x77 onward
// is raw-identical. Declaration/lifetime orders, cached bounds/owner, positive and
// rejection CFG spellings, 38 current-state AST variants, and 36 guarded TU-state
// trials did not close it. Revisit after a real predecessor/header state change.
VA(0x004ddee0, 0x2f4)
int widget::Main(tag_message &message)
{
    switch (message.type) {
    case MESSAGE_MOUSE_MOVE: {
        short x = static_cast<short>(message.payload.mouse.x);
        short y = static_cast<short>(message.payload.mouse.y);
        short left = m_x;
        heroWindow *window = m_owner;
        x -= static_cast<short>(window->m_posX);
        y -= static_cast<short>(window->m_posY);
        if (left > x || m_y > y || left + m_width <= x || m_y + m_height <= y)
            break;
        message.payload.hover.id = m_id;
        return 2;
    }

    case MESSAGE_WIDGET:
        switch (message.payload.widget.command) {
        case WIDGET_COMMAND_DRAW:
            if ((m_flags & WIDGET_FLAG_DRAW) != 0)
                Draw();
            if ((m_flags & WIDGET_FLAG_DIMMED) != 0 &&
                DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED &&
                DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
                short x = m_x + static_cast<short>(m_owner->m_posX);
                short y = m_y + static_cast<short>(m_owner->m_posY);
                DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
                return 0;
            }
            break;

        case WIDGET_COMMAND_SET_FLAGS:
            if (m_id == message.payload.widget.id) {
                if (message.payload.widget.data.value == WIDGET_COMMAND_DIMMED) {
                    m_flags |= WIDGET_FLAG_DIMMED;
                    return 1;
                }
                unsigned short flags = m_flags | static_cast<unsigned short>(message.payload.widget.data.value);
                m_flags = flags;
                if ((flags & WIDGET_FLAG_DIMMED) != 0) {
                    Draw();
                    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED &&
                        DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
                        short x = m_x + static_cast<short>(m_owner->m_posX);
                        short y = m_y + static_cast<short>(m_owner->m_posY);
                        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
                    }
                }
                if ((m_flags & WIDGET_FLAG_UPDATE) != 0) {
                    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                        m_y + m_owner->m_posY,
                                                        m_width, m_height);
                    m_flags &= ~WIDGET_FLAG_UPDATE;
                }
                return 1;
            }
            break;

        case WIDGET_COMMAND_CLEAR_FLAGS:
            if (m_id == message.payload.widget.id) {
                unsigned int flags = message.payload.widget.data.value;
                if (flags == WIDGET_COMMAND_DIMMED) {
                    m_flags &= ~WIDGET_FLAG_DIMMED;
                    return 1;
                }
                m_flags &= ~static_cast<unsigned short>(flags);
                if ((static_cast<unsigned short>(flags) & WIDGET_FLAG_DIMMED) != 0)
                    Draw();
                if ((static_cast<unsigned short>(flags) & WIDGET_FLAG_UPDATE) != 0)
                    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX,
                                                        m_y + m_owner->m_posY,
                                                        m_width, m_height);
                return 1;
            }
            break;

        case WIDGET_COMMAND_SET_X:
            if (m_id == message.payload.widget.id) {
                m_x = static_cast<short>(message.payload.widget.data.value);
                return 1;
            }
            break;

        case WIDGET_COMMAND_SET_Y:
            if (m_id == message.payload.widget.id) {
                m_y = static_cast<short>(message.payload.widget.data.value);
                return 1;
            }
            break;

        case WIDGET_COMMAND_SET_WIDTH:
            if (m_id == message.payload.widget.id) {
                m_width = static_cast<short>(message.payload.widget.data.value);
                return 1;
            }
            break;
        }
        break;
    }
    return 0;
}

// @semantic
// The emitted body is 0x46 bytes against the 0x47 CodeView span, with the same CFG,
// call operands, and 2/2 ordered relocations. From +0x0b, VC4.2 schedules the same
// coordinate loads/extensions and argument pushes differently. Direct, staged,
// cached-owner, operand-order, widened-lifetime, and const spellings emit the same
// live body. Several guarded declaration-state probes scored 100%, but none was an
// auditable exact closure: generated state is disposable and the 0x46/0x47 size
// mismatch remained. Revisit after a real header/TU-state change.
VA(0x004de1e0, 0x47)
void widget::Dim(void)
{
    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED &&
        DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
        short x = m_owner->m_posX + m_x;
        short y = m_y + m_owner->m_posY;
        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
    }
}


// ===== vtable widget (root)  (3 slots) =====
//  [ 0] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 1] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)
//  [ 2] VA(0x004e1560, 0xb)  __purecall   <- = 0 (pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(widget, 0x004ebb00);
