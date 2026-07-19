#include <va.h>
#include <BASE/widget.h>
#include <BASE/widgetKind.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bitmap.h>
#include <BASE/bmap2.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(WidgetConstant)
    DEFAULT_EXTENT = 16
H2_ENUM_END(WidgetConstant)

VA(0x004dde00, 0x5a)
widget::widget(i16 x, i16 y, i16 width, i16 height, i16 id, i16 kind) {
    m_owner = NULL;
    m_next = NULL;
    m_prev = NULL;
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
widget::widget(void) {
    m_id = 0;
    m_owner = NULL;
    m_next = NULL;
    m_prev = NULL;
    m_flags = WIDGET_FLAG_ENABLED | WIDGET_FLAG_DRAW;
    m_zOrder = -1;
    m_kind = EncodeWidgetKind(WIDGET_KIND_DEFAULT);
    m_y = 0;
    m_x = 0;
    m_width = DEFAULT_EXTENT;
    m_height = DEFAULT_EXTENT;
}

VA(0x004ddea0, 0x7)
widget::~widget() {}

VA(0x004ddeb0, 0x14)
i32 widget::Open(i32 zOrder, class heroWindow* owner) {
    m_zOrder = zOrder;
    m_owner = owner;
    return 0;
}

VA(0x004dded0, 0x1)
void widget::Close(void) {}

VA(0x004ddee0, 0x2f4)
i32 widget::Main(tag_message& message) {
    switch (message.type) {
        case MESSAGE_MOUSE_MOVE: {
            i16 x = static_cast<i16>(message.payload.mouse.x);
            i16 y = static_cast<i16>(message.payload.mouse.y);
            i16 left = m_x;
            heroWindow* window = m_owner;
            x -= static_cast<i16>(window->m_posX);
            y -= static_cast<i16>(window->m_posY);
            if (left > x || m_y > y || left + m_width <= x || m_y + m_height <= y)
                break;
            message.payload.hover.id = m_id;
            return WIDGET_DISPATCH_FORWARD;
        }

        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DRAW:
                    if ((m_flags & WIDGET_FLAG_DRAW) != 0)
                        Draw();
                    if ((m_flags & WIDGET_FLAG_DIMMED) != 0
                        && DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED
                        && DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
                        i16 x = m_x + static_cast<i16>(m_owner->m_posX);
                        i16 y = m_y + static_cast<i16>(m_owner->m_posY);
                        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
                        return WIDGET_DISPATCH_CONTINUE;
                    }
                    break;

                case WIDGET_COMMAND_SET_FLAGS:
                    if (m_id == message.payload.widget.id) {
                        if (message.payload.widget.data.value == WIDGET_COMMAND_DIMMED) {
                            m_flags |= WIDGET_FLAG_DIMMED;
                            return WIDGET_DISPATCH_CONSUME;
                        }
                        u16 flags = m_flags | static_cast<u16>(message.payload.widget.data.value);
                        m_flags = flags;
                        if ((flags & WIDGET_FLAG_DIMMED) != 0) {
                            Draw();
                            if (DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED
                                && DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
                                i16 x = m_x + static_cast<i16>(m_owner->m_posX);
                                i16 y = m_y + static_cast<i16>(m_owner->m_posY);
                                DimBitmapArea(
                                    gpWindowManager->m_screen,
                                    x,
                                    y,
                                    m_width,
                                    m_height,
                                    0
                                );
                            }
                        }
                        if ((m_flags & WIDGET_FLAG_UPDATE) != 0) {
                            gpWindowManager->UpdateScreenRegion(
                                m_x + m_owner->m_posX,
                                m_y + m_owner->m_posY,
                                m_width,
                                m_height
                            );
                            m_flags &= ~WIDGET_FLAG_UPDATE;
                        }
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_CLEAR_FLAGS:
                    if (m_id == message.payload.widget.id) {
                        u32 flags = message.payload.widget.data.value;
                        if (flags == IDX(WIDGET_COMMAND_DIMMED)) {
                            m_flags &= ~WIDGET_FLAG_DIMMED;
                            return WIDGET_DISPATCH_CONSUME;
                        }
                        m_flags &= ~static_cast<u16>(flags);
                        if ((static_cast<u16>(flags) & WIDGET_FLAG_DIMMED) != 0)
                            Draw();
                        if ((static_cast<u16>(flags) & WIDGET_FLAG_UPDATE) != 0)
                            gpWindowManager->UpdateScreenRegion(
                                m_x + m_owner->m_posX,
                                m_y + m_owner->m_posY,
                                m_width,
                                m_height
                            );
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_X:
                    if (m_id == message.payload.widget.id) {
                        m_x = static_cast<i16>(message.payload.widget.data.value);
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_Y:
                    if (m_id == message.payload.widget.id) {
                        m_y = static_cast<i16>(message.payload.widget.data.value);
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_WIDTH:
                    if (m_id == message.payload.widget.id) {
                        m_width = static_cast<i16>(message.payload.widget.data.value);
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;
    }
    return WIDGET_DISPATCH_CONTINUE;
}

VA(0x004de1e0, 0x47)
void widget::Dim(void) {
    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_UNDIMMED
        && DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT) {
        i16 x = m_owner->m_posX + m_x;
        i16 y = m_y + m_owner->m_posY;
        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
    }
}


VTBL(widget, 0x004ebb00);
