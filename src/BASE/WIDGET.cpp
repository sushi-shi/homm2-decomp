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

// @match-note
// Both sections are 0x5a with the vtable relocation matching at +0x13. The
// relocation-masked residual is 10 bytes at +0x02,+0x0c,+0x0f,+0x19,+0x1b,
// +0x1d,+0x24,+0x2c,+0x3c,+0x3e; the first divergence is our `mov ax,[esp+4]`
// versus retail `mov dx,[esp+4]`. Member values, frame, store CFG, and return are
// complete. Moving the width store before `m_prev` reduced the prior 18-byte
// residual and raised 96.8261% to 97.9565%. Separate/chained null stores, cached
// height, full and coordinate-only initializer lists, and a 200-iteration audited
// libclang AST search found no exact form. A later retail-lifetime pass also ruled
// out ABI-neutral dimension typedef/qualifier, width-conversion/accessor, explicit
// X/height snapshot, and adjacent width/height initialization models; the bounded
// results are in docs/matching-matrices/widget-whole-tu-214bd52.tsv. Retry after a
// predecessor/include/header TU-state change or new evidence for the original
// initialization lifetime graph.
VA(0x004dde00, 0x5a)
widget::widget(short int x, short int y, short int width, short int height, short int id, short int kind)
{
    m_x = x;
    m_owner = 0;
    m_y = y;
    m_next = 0;
    m_width = width;
    m_prev = 0;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_height = height;
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
    m_kind = WIDGET_KIND_DEFAULT;
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

// @match-note
// Both sections are 0x2f4 with 17/17 explicit-range relocations and matching
// external targets. The helper truncates the candidate at a delinked local label,
// so the explicit object range is authoritative. The remaining 8 non-relocation
// bytes are +0x23,+0x38,+0x41,+0x42,+0x46,+0x4a,+0x4f,+0x51, all in the
// mouse-hit-test register coloring; first is our `mov bx,[esi+18h]` versus retail
// `mov bp,[esi+18h]`. The dispatcher from +0x77 onward is byte-identical. Declaring
// mouse X/Y before the left snapshot reduced the prior 12-byte residual and raised
// 99.2% to 99.4833% while preserving exact Dim. A retail-timed top snapshot reached
// 99.7167% in Main but changed exact Dim by 25 raw bytes through TU state. Typed
// payload references emitted an extra `lea`; width/top snapshots, direct/staged
// coordinate assignments, positive/rejection CFGs, explicit right/bottom bounds,
// owner pointer/reference spellings, and audited commutative/relational AST forms
// did not close the residual without losing an exact sibling. Retry after an
// exact-preserving predecessor/TU-state change or new lifetime evidence. The
// bounded declaration-context retest for this combined checkpoint is recorded in
// docs/matching-matrices/widget-whole-tu-214bd52.tsv.
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
            if ((m_flags & WIDGET_FLAG_DIMMED) != 0 && m_kind != WIDGET_KIND_UNDIMMED &&
                m_kind != WIDGET_KIND_TEXT) {
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
                    if (m_kind != WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT) {
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
    if (m_kind != WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT) {
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
