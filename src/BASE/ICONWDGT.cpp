#include <Ints.h>
#include <BASE/iconWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>

typedef enum IconWidgetConstant {
    RESOURCE_NAME_CAPACITY = 16,
    COLOR_INDEX_MASK       = 0xff,
    ORIENTATION_MASK       = 0xff,
    FRAME_INDEX_MASK       = 0xffff,
    CENTER_SHIFT           = 1,
    BOTTOM_PADDING         = 2
} IconWidgetConstant;

iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_icon = NULL;
    m_frame = 0;
    m_fillColor = 0;
    m_orientation = ICON_DRAW_NORMAL;
    m_iconId = 0;
}

iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    u32l iconId,
    i16 frame,
    IconDrawOrientation orientation,
    i16 id,
    WidgetKind kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_orientation = orientation;
    m_kind = kind;
}

iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    char* iconName,
    i16 frame,
    IconDrawOrientation orientation,
    i16 id,
    WidgetKind kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_orientation = orientation;
    m_kind = kind;
}

void iconWidget::Read(void) {
    char iconName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(reinterpret_cast<i8*>(iconName));
    gpResourceManager->SavePosition();
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    gpResourceManager->RestorePosition();
    m_frame = gpResourceManager->ReadWord();
    m_orientation =
        static_cast<IconDrawOrientation>(gpResourceManager->ReadWord() & ORIENTATION_MASK);
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & COLOR_INDEX_MASK;
}

iconWidget::~iconWidget() {
    gpResourceManager->Dispose(m_icon);
}

MessageDispatchResult iconWidget::Main(tag_message& msg) {


    i16 x;
    i16 y;
    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))
        && (msg.type != MESSAGE_WIDGET
            || msg.payload.widget.command != WIDGET_COMMAND_REPLACE_ICON)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (msg.type) {
        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_REPLACE_ICON:
                    if (m_iconId == msg.payload.widget.id) {
                        m_iconId = msg.payload.widget.data.value;
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.value);
                    }
                    return MESSAGE_DISPATCH_CONTINUE;

                case WIDGET_COMMAND_SET_ICON:
                    if (msg.payload.widget.id == m_id) {
                        if (m_icon != NULL) {
                            gpResourceManager->Dispose(m_icon);
                            m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.text);
                        }
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_FRAME:
                    if (msg.payload.widget.id == m_id) {
                        i16 frame = static_cast<i16>(
                            msg.payload.widget.data.value & FRAME_INDEX_MASK
                        );
                        m_frame = frame;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_FILL_COLOR:
                    if (msg.payload.widget.id == m_id) {
                        m_fillColor = msg.payload.widget.data.value & COLOR_INDEX_MASK;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            x = msg.payload.mouse.x - m_owner->m_posX;
            y = msg.payload.mouse.y - m_owner->m_posY;
            if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                if (msg.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                    msg.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                } else {
                    m_flags |= WIDGET_FLAG_SELECTED;
                    msg.payload.widget.command = WIDGET_COMMAND_SELECT;
                }
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.id = m_id;
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                m_flags &= ~WIDGET_FLAG_SELECTED;
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
                msg.payload.widget.id = m_id;

                if (msg.type == MESSAGE_RIGHT_BUTTON_UP)
                    msg.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
    }

    return widget::Main(msg);
}

void iconWidget::Draw(void) {
    i16 x = m_owner->m_posX + m_x;
    i16 y = m_owner->m_posY + m_y;

    switch (m_kind) {
        case WIDGET_KIND_ICON_DIRECT:
            m_icon->DrawToBuffer(x, y, m_frame, m_orientation);
            return;

        case WIDGET_KIND_ICON_CENTERED: {
            IconEntry* entry = GetIconEntry(m_icon, m_frame);
            x -= entry->x;
            y -= entry->y;
            if (entry->w < m_width)
                x += (m_width - entry->w) >> CENTER_SHIFT;
            if (entry->h + BOTTOM_PADDING < m_height)
                y += m_height - entry->h - BOTTOM_PADDING;
            m_icon->DrawToBuffer(x, y, m_frame, m_orientation);
            return;
        }

        case WIDGET_KIND_ICON_FILL:
            m_icon->FillToBuffer(x, y, m_frame, m_fillColor, m_orientation, NULL);
            return;
    }
}
