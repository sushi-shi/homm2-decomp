// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WIDGET.OBJ   from: .\basewin.lib
// functions: 7   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/widget.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>

#define WIDGET_FLAG_ENABLED 0x2
#define WIDGET_FLAG_DRAW 0x4
#define WIDGET_FLAG_DIMMED 0x8
#define WIDGET_FLAG_UPDATE 0x4000
#define WIDGET_MESSAGE_MOUSE_MOVE 0x4
#define WIDGET_MESSAGE_COMMAND 0x200
#define WIDGET_COMMAND_DRAW 0x2
#define WIDGET_COMMAND_SET_FLAGS 0x5
#define WIDGET_COMMAND_CLEAR_FLAGS 0x6
#define WIDGET_COMMAND_SET_X 0x34
#define WIDGET_COMMAND_SET_Y 0x35
#define WIDGET_COMMAND_SET_WIDTH 0x3d
#define WIDGET_KIND_UNDIMMED 0x8
#define WIDGET_KIND_TEXT 0x200
#define WIDGET_COMMAND_DIMMED 0x1000

// @early-stop
// /O2 scheduling wall: base and retail are both 0x5a bytes and the sole vtable
// relocation at +0x13 agrees. The residual bytes at +0x02, +0x0c, +0x0f,
// +0x18..+0x22, +0x24, +0x2c, +0x3c, and +0x3e are only AX/DX coloring and
// parameter-load/null-store scheduling; all member values and the return sequence
// agree. Separate and chained null stores, a cached height, full initializer-list,
// and coordinate-only initializer-list forms were tried; this body is the best.
VA(0x004dde00, 0x5a)
widget::widget(short int x, short int y, short int w, short int h, short int p5, short int kind)
{
    m_x = x;
    m_owner = 0;
    m_y = y;
    m_next = 0;
    m_prev = 0;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_width = w;
    m_height = h;
    m_id = p5;
    field_0x14 = kind;
}

VA(0x004dde60, 0x3f)
widget::widget(void)
{
    m_id = 0;
    m_owner = 0;
    m_next = 0;
    m_prev = 0;
    m_flags = 6;
    m_zOrder = -1;
    field_0x14 = 2;
    m_y = 0;
    m_x = 0;
    m_width = 0x10;
    m_height = 0x10;
}

VA(0x004ddea0, 0x7)
widget::~widget() {}

VA(0x004ddeb0, 0x14)
int widget::Open(int id, class heroWindow *win)
{
    m_zOrder = id;
    m_owner = win;
    return 0;
}

// @early-stop
// Delinker symbol artifact: base emits the named 1-byte Close section and retail
// maps the same lone C3 byte as `empty_stub`; neither side has relocations.
VA(0x004dded0, 0x1)
void widget::Close(void) {}

// @early-stop
// /O2 register-coloring wall plus delinker-local identities: both sections are
// 0x2f4 bytes with 17 relocations at identical offsets. All eight external targets
// agree; retail names the two switch-dispatch and seven table-entry relocations as
// Main-relative while base uses $L locals. After masking those relocations, 742/756
// bytes are identical. Every non-relocation residual is confined to +0x26..+0x61,
// where AX and DX are exchanged while computing the mouse hit test; the complete
// command dispatcher from +0x77 onward is otherwise byte-identical. Direct versus
// staged coordinates, x/y declaration order, and a cached owner were tried.
VA(0x004ddee0, 0x2f4)
int widget::Main(tag_message &message)
{
    switch (message.type) {
    case WIDGET_MESSAGE_MOUSE_MOVE: {
        short x = static_cast<short>(message.payload.mouse.x);
        short y = static_cast<short>(message.payload.mouse.y);
        x -= static_cast<short>(m_owner->m_posX);
        y -= static_cast<short>(m_owner->m_posY);
        if (m_x > x || m_y > y || m_x + m_width <= x || m_y + m_height <= y)
            break;
        message.payload.hover.id = m_id;
        return 2;
    }

    case WIDGET_MESSAGE_COMMAND:
        switch (message.payload.widget.command) {
        case WIDGET_COMMAND_DRAW:
            if ((m_flags & WIDGET_FLAG_DRAW) != 0)
                Draw();
            if ((m_flags & WIDGET_FLAG_DIMMED) != 0 && field_0x14 != WIDGET_KIND_UNDIMMED &&
                field_0x14 != WIDGET_KIND_TEXT) {
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
                    if (field_0x14 != WIDGET_KIND_UNDIMMED && field_0x14 != WIDGET_KIND_TEXT) {
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

VA(0x004de1e0, 0x47)
void widget::Dim(void)
{
    if (field_0x14 != 8 && field_0x14 != 0x200) {
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
