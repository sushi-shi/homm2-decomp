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
    m_orientation = static_cast<IconDrawOrientation>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & COLOR_INDEX_MASK;
}

inline iconWidget::~iconWidget() {
    gpResourceManager->Dispose(m_icon);
}


#define SET_WIDGET_MESSAGE_TYPE_AND_ID(messageValue, idValue)                                    \
    messageValue.type = MESSAGE_WIDGET;                                                          \
    messageValue.payload.widget.id = idValue

MessageDispatchResult iconWidget::Main(tag_message& msg) {
    i16 flags = m_flags;
    if (!(((flags) & (WIDGET_FLAG_ENABLED)))
        && (msg.type != MESSAGE_WIDGET
            || msg.payload.widget.command != WIDGET_COMMAND_REPLACE_ICON)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    MessageType eventType = msg.type;
    switch (eventType) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 relativeX = static_cast<i16>(msg.payload.mouse.x);
            heroWindow* window = m_owner;
            relativeX -= static_cast<i16>(window->m_posX);
            i16 relativeY = static_cast<i16>(msg.payload.mouse.y);
            relativeY -= static_cast<i16>(window->m_posY);
            if (m_x <= relativeX && m_y <= relativeY && relativeX < m_x + m_width
                && relativeY < m_y + m_height) {
                if (eventType == MESSAGE_RIGHT_BUTTON_DOWN) {
                    msg.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                } else {
                    m_flags = flags | WIDGET_FLAG_SELECTED;
                    msg.payload.widget.command = WIDGET_COMMAND_SELECT;
                }
                SET_WIDGET_MESSAGE_TYPE_AND_ID(msg, m_id);
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((((flags) & (WIDGET_FLAG_SELECTED)))) {
                m_flags = flags & ~WIDGET_FLAG_SELECTED;
                msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
                SET_WIDGET_MESSAGE_TYPE_AND_ID(msg, m_id);
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_SET_FRAME:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    m_frame = msg.payload.widget.data.value;
                    return MESSAGE_DISPATCH_CONSUME;

                case WIDGET_COMMAND_SET_FILL_COLOR:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    m_fillColor = msg.payload.widget.data.value & COLOR_INDEX_MASK;
                    return MESSAGE_DISPATCH_CONSUME;

                case WIDGET_COMMAND_SET_ICON:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    if (m_icon != NULL) {
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.text);
                    }
                    return MESSAGE_DISPATCH_CONSUME;

                case WIDGET_COMMAND_REPLACE_ICON:
                    if (m_iconId == msg.payload.widget.id) {
                        m_iconId = msg.payload.widget.data.value;
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(
                            static_cast<u32l>(msg.payload.widget.data.value)
                        );
                    }
                    return MESSAGE_DISPATCH_CONTINUE;

                default:
                    goto normalEvent;
            }

        default:
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

#undef SET_WIDGET_MESSAGE_TYPE_AND_ID

void iconWidget::Draw(void) {
    heroWindow* window = m_owner;
    i16 y = static_cast<i16>(window->m_posY);
    i16 x = static_cast<i16>(window->m_posX);
    WidgetKind kind = m_kind;
    x += m_x;
    y += m_y;

    switch (kind) {
        case WIDGET_KIND_ICON_DIRECT:
            m_icon->DrawToBuffer(x, y, m_frame, m_orientation);
            return;

        case WIDGET_KIND_ICON_CENTERED: {
            IconEntry* entry = GetIconEntry(m_icon, m_frame);
            i16 widgetWidth;
            i16 iconWidth;
            iconWidth = entry->w;
            y -= entry->y;
            x -= entry->x;
            widgetWidth = m_width;
            if (iconWidth < widgetWidth)
                x += (widgetWidth - iconWidth) >> CENTER_SHIFT;
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
