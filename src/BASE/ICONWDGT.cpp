#include <va.h>
#include <BASE/iconWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/Misc.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d0a60, 0x2d)
iconWidget::iconWidget(void) : widget(0, 0, 0, 0, 0, 0) {
    m_frame = 0;
    m_fillColor = 0;
    m_icon = 0;
    m_flip = 0;
    m_iconId = 0;
}

// @early-stop: inline continuation artifact.
VA(0x004d0ad0, 0x6a)
iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    u32l iconId,
    i16 frame,
    i8 flip,
    i16 id,
    i16 kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = iconId;
    m_icon = gpResourceManager->GetIcon(iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0b40, 0x78)
iconWidget::iconWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    char* iconName,
    i16 frame,
    i8 flip,
    i16 id,
    i16 kind,
    i16 fillColor
)
    : widget(x, y, width, height, id, kind) {
    m_iconId = gpResourceManager->MakeId(iconName, 1);
    m_icon = gpResourceManager->GetIcon(m_iconId);
    m_frame = frame;
    m_fillColor = fillColor;
    m_flip = flip;
    m_kind = kind;
}

VA(0x004d0bc0, 0xdf)
void iconWidget::Read(void) {
    char iconName[16];
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
    m_flip = static_cast<i8>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_fillColor = gpResourceManager->ReadWord() & 0xFF;
}

VA(0x004d0ca0, 0x21)
iconWidget::~iconWidget() {
    gpResourceManager->Dispose(m_icon);
}

// @semantic: jump-table placement residual.
VA(0x004d0cd0, 0x291)
i32 iconWidget::Main(tag_message& msg) {
    u16 flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0
        && (msg.type != MESSAGE_WIDGET
            || msg.payload.widget.command != WIDGET_COMMAND_REPLACE_ICON)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return 0;
    }

    i32 eventType = msg.type;
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

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((flags & WIDGET_FLAG_SELECTED) != 0) {
                m_flags = flags & ~WIDGET_FLAG_SELECTED;
                msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.id = m_id;
                return 2;
            }
            return 0;

        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_SET_FRAME:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    m_frame = msg.payload.widget.data.value;
                    return 1;

                case WIDGET_COMMAND_SET_FILL_COLOR:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    m_fillColor = msg.payload.widget.data.value & 0xFF;
                    return 1;

                case WIDGET_COMMAND_SET_ICON:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    if (m_icon != 0) {
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(msg.payload.widget.data.text);
                    }
                    return 1;

                case WIDGET_COMMAND_REPLACE_ICON:
                    if (m_iconId == msg.payload.widget.id) {
                        m_iconId = msg.payload.widget.data.value;
                        gpResourceManager->Dispose(m_icon);
                        m_icon = gpResourceManager->GetIcon(
                            static_cast<u32l>(msg.payload.widget.data.value)
                        );
                    }
                    return 0;

                default:
                    goto normalEvent;
            }

        default:
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

// @semantic: compiler-shape residual.
VA(0x004d0f70, 0xe5)
void iconWidget::Draw(void) {
    heroWindow* window = m_owner;
    i16 y = static_cast<i16>(window->m_posY);
    i16 x = static_cast<i16>(window->m_posX);
    i16 kind = m_kind;
    x += m_x;
    y += m_y;

    switch (DecodeWidgetKind(kind)) {
        case WIDGET_KIND_ICON_DIRECT:
            m_icon->DrawToBuffer(x, y, m_frame, m_flip);
            return;

        case WIDGET_KIND_ICON_CENTERED: {
            IconEntry* entry = GetIconEntry(m_icon, m_frame);
            i16 widgetWidth;
            i16 iconWidth;
            iconWidth = entry->w;
            y -= entry->y;
            widgetWidth = m_width;
            x -= entry->x;
            if (iconWidth < widgetWidth)
                x += (widgetWidth - iconWidth) >> 1;
            if (entry->h + 2 < m_height)
                y += m_height - entry->h - 2;
            m_icon->DrawToBuffer(x, y, m_frame, m_flip);
            return;
        }

        case WIDGET_KIND_ICON_FILL:
            m_icon->FillToBuffer(x, y, m_frame, m_fillColor, m_flip, 0);
            return;
    }
}


VTBL(iconWidget, 0x004eba40);
