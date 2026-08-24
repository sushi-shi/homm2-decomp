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

VA(0x004dde60, 0x3f)
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

static inline void DimWidgetArea(widget* target) {
    i16 x = target->m_x + static_cast<i16>(target->m_owner->m_posX);
    i16 y = target->m_y + static_cast<i16>(target->m_owner->m_posY);
    DimBitmapArea(
        gpWindowManager->m_screen,
        x,
        y,
        target->m_width,
        target->m_height,
        0
    );
}

static inline i32 IsInsideWidget(widget* target, i16 x, i16 y) {
    i16 left = target->m_x;
    return !(left > x || target->m_y > y || left + target->m_width <= x
             || target->m_y + target->m_height <= y);
}

VA(0x004ddee0, 0x2f4)
MessageDispatchResult widget::Main(tag_message& message) {
    switch (message.type) {
        case MESSAGE_MOUSE_MOVE: {
            i16 x = static_cast<i16>(message.payload.mouse.x);
            i16 y = static_cast<i16>(message.payload.mouse.y);
            heroWindow* window = m_owner;
            x -= static_cast<i16>(window->m_posX);
            y -= static_cast<i16>(window->m_posY);
            if (!IsInsideWidget(this, x, y))
                break;
            message.payload.hover.id = m_id;
            return MESSAGE_DISPATCH_FORWARD;
        }

        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_DRAW:
                    if (HAS(m_flags, WIDGET_FLAG_DRAW))
                        Draw();
                    if (HAS(m_flags, WIDGET_FLAG_DIMMED)
                        && m_kind != WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT) {
                        DimWidgetArea(this);
                        return MESSAGE_DISPATCH_CONTINUE;
                    }
                    break;

                case WIDGET_COMMAND_SET_FLAGS:
                    if (m_id == message.payload.widget.id) {
                        if (message.payload.widget.data.value == IDX(WIDGET_COMMAND_DIMMED)) {
                            m_flags |= WIDGET_FLAG_DIMMED;
                            return MESSAGE_DISPATCH_CONSUME;
                        }
                        H2_ENUM_STORAGE(WidgetFlag, i16) flags =
                            m_flags
                            | static_cast<H2_ENUM_STORAGE(WidgetFlag, i16)>(
                                message.payload.widget.data.value
                            );
                        m_flags = flags;
                        if (HAS(flags, WIDGET_FLAG_DIMMED)) {
                            Draw();
                            if (m_kind != WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT) {
                                DimWidgetArea(this);
                            }
                        }
                        if (HAS(m_flags, WIDGET_FLAG_UPDATE)) {
                            gpWindowManager->UpdateScreenRegion(
                                m_x + m_owner->m_posX,
                                m_y + m_owner->m_posY,
                                m_width,
                                m_height
                            );
                            m_flags &= ~WIDGET_FLAG_UPDATE;
                        }
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_CLEAR_FLAGS:
                    if (m_id == message.payload.widget.id) {
                        i32 rawFlags = message.payload.widget.data.value;
                        if (rawFlags == IDX(WIDGET_COMMAND_DIMMED)) {
                            m_flags &= ~WIDGET_FLAG_DIMMED;
                            return MESSAGE_DISPATCH_CONSUME;
                        }
                        H2_ENUM_STORAGE(WidgetFlag, i16) flags =
                            static_cast<H2_ENUM_STORAGE(WidgetFlag, i16)>(rawFlags);
                        m_flags &= ~flags;
                        if (HAS(flags, WIDGET_FLAG_DIMMED))
                            Draw();
                        if (HAS(flags, WIDGET_FLAG_UPDATE))
                            gpWindowManager->UpdateScreenRegion(
                                m_x + m_owner->m_posX,
                                m_y + m_owner->m_posY,
                                m_width,
                                m_height
                            );
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_X:
                    if (m_id == message.payload.widget.id) {
                        m_x = static_cast<i16>(message.payload.widget.data.value);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_Y:
                    if (m_id == message.payload.widget.id) {
                        m_y = static_cast<i16>(message.payload.widget.data.value);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_WIDTH:
                    if (m_id == message.payload.widget.id) {
                        m_width = static_cast<i16>(message.payload.widget.data.value);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

VA(0x004de1e0, 0x47)
void widget::Dim(void) {
    if (m_kind != WIDGET_KIND_UNDIMMED && m_kind != WIDGET_KIND_TEXT) {
        i16 x = m_owner->m_posX + m_x;
        i16 y = m_y + m_owner->m_posY;
        DimBitmapArea(gpWindowManager->m_screen, x, y, m_width, m_height, 0);
    }
}


VTBL(widget, 0x004ebb00);
