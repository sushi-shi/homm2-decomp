#include <va.h>
#include <BASE/widget.h>
#include <BASE/widgetKind.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(WidgetConstant)
    DEFAULT_EXTENT   = 16,
    WIDGET_FLAG_MASK = 0xffff
H2_ENUM_END(WidgetConstant)

VA(0x004d4010, 0x8b)
widget::widget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 id,
    H2_ENUM_PARAM(WidgetKind, i16) kind
) {
    m_owner = NULL;
    m_next = NULL;
    m_prev = NULL;
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
    m_id = id;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_kind = kind;
}

VA(0x004d40a0, 0x7d)
widget::widget(void) {
    m_owner = NULL;
    m_next = NULL;
    m_prev = NULL;
    m_id = 0;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_kind = WIDGET_KIND_DEFAULT;
    m_y = 0;
    m_x = 0;
    m_width = DEFAULT_EXTENT;
    m_height = DEFAULT_EXTENT;
}

VA(0x004d4120, 0x14)
widget::~widget() {}

VA(0x004d4140, 0x23)
i32 widget::Open(i32 zOrder, class heroWindow* owner) {
    m_zOrder = zOrder;
    m_owner = owner;
    return 0;
}

VA(0x004d4170, 0xb)
void widget::Close(void) {}

VA(0x004d4180, 0x3b5)
MessageDispatchResult widget::Main(tag_message& message) {
    // A switch arm may not jump past an initialisation, so the hit-test
    // coordinates are declared for the whole function.
    i16 x;
    i16 y;
    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DRAW:
                    if (HAS(m_flags, WIDGET_FLAG_DRAW))
                        Draw();
                    if (HAS(m_flags, WIDGET_FLAG_DIMMED))
                        Dim();
                    break;

                case WIDGET_COMMAND_SET_FLAGS:
                    if (message.payload.widget.id == m_id) {
                        if (message.payload.widget.data.value == IDX(WIDGET_COMMAND_DIMMED)) {
                            m_flags |= WIDGET_FLAG_DIMMED;
                            return MESSAGE_DISPATCH_CONSUME;
                        }
                        m_flags |= static_cast<WidgetFlag>(
                            message.payload.widget.data.value & WIDGET_FLAG_MASK
                        );
                        if (HAS(m_flags, WIDGET_FLAG_DIMMED)) {
                            Draw();
                            Dim();
                        }
                        if (HAS(m_flags, WIDGET_FLAG_UPDATE)) {
                            gpWindowManager->UpdateScreenRegion(
                                m_owner->m_posX + m_x,
                                m_owner->m_posY + m_y,
                                m_width,
                                m_height
                            );
                            m_flags &= ~WIDGET_FLAG_UPDATE;
                        }
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_CLEAR_FLAGS:
                    if (message.payload.widget.id == m_id) {
                        H2_ENUM_STORAGE(WidgetFlag, i16) flags =
                            message.payload.widget.data.value & WIDGET_FLAG_MASK;
                        if (message.payload.widget.data.value == IDX(WIDGET_COMMAND_DIMMED)) {
                            flags = WIDGET_FLAG_DIMMED;
                            m_flags &= ~flags;
                            return MESSAGE_DISPATCH_CONSUME;
                        }
                        m_flags &= ~flags;
                        if (HAS(flags, WIDGET_FLAG_DIMMED))
                            Draw();
                        if (HAS(flags, WIDGET_FLAG_UPDATE))
                            gpWindowManager->UpdateScreenRegion(
                                m_owner->m_posX + m_x,
                                m_owner->m_posY + m_y,
                                m_width,
                                m_height
                            );
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_X:
                    if (message.payload.widget.id == m_id) {
                        m_x = message.payload.widget.data.value;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_Y:
                    if (message.payload.widget.id == m_id) {
                        m_y = message.payload.widget.data.value;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_WIDTH:
                    if (message.payload.widget.id == m_id) {
                        m_width = message.payload.widget.data.value;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            x = message.payload.mouse.x - m_owner->m_posX;
            y = message.payload.mouse.y - m_owner->m_posY;
            if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                message.payload.hover.id = m_id;
                return MESSAGE_DISPATCH_FORWARD;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

VA(0x004d4540, 0x7e)
void widget::Dim(void) {
    if (m_kind == WIDGET_KIND_UNDIMMED || m_kind == WIDGET_KIND_TEXT)
        return;
    i16 x = m_owner->m_posX + m_x;
    i16 y = m_owner->m_posY + m_y;
    DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(widget, 0x004eaa1c)
