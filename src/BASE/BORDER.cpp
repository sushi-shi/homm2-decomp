#include <Ints.h>
#include <BASE/widget.h>
#include <BASE/message.h>
#include <BASE/border.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>

typedef enum BorderConstant {
    RESOURCE_NAME_CAPACITY = 16,
    COLOR_INDEX_MASK       = 0xff
} BorderConstant;

border::border(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_fillColor = 0;
    m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
}

border::border(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 widgetId,
    WidgetKind kind,
    i16 fillColor,
    const char* name
)
    : widget(x, y, width, height, widgetId, kind) {
    if (name != NULL)
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
    else
        m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    m_fillColor = fillColor;
}

border::~border() {
    if (m_backgroundBitmap != NULL)
        gpResourceManager->Dispose(m_backgroundBitmap);
    if (m_backgroundIcon != NULL)
        gpResourceManager->Dispose(m_backgroundIcon);
}

void border::Read(void) {
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    if (m_kind == WIDGET_KIND_BITMAP) {
        char bitmapName[RESOURCE_NAME_CAPACITY];
        gpResourceManager->Read13(bitmapName);
        gpResourceManager->SavePosition();
        m_backgroundBitmap = gpResourceManager->GetBitmap(bitmapName);
        gpResourceManager->RestorePosition();
        return;
    }
    if (m_kind == WIDGET_KIND_ICON) {
        char iconName[RESOURCE_NAME_CAPACITY];
        gpResourceManager->Read13(iconName);
        gpResourceManager->SavePosition();
        m_backgroundIcon = gpResourceManager->GetIcon(iconName);
        gpResourceManager->RestorePosition();
        return;
    }
    m_fillColor = gpResourceManager->ReadWord() & COLOR_INDEX_MASK;
}

MessageDispatchResult border::Main(struct tag_message& message) {
    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (message.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 x = message.payload.mouse.x - m_owner->m_posX;
            i16 y = message.payload.mouse.y - m_owner->m_posY;
            if (WIDGET_CONTAINS_LOCAL_POINT(*this, x, y)) {
                if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                    message.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                } else {
                    m_flags |= WIDGET_FLAG_SELECTED;
                    message.payload.widget.command = WIDGET_COMMAND_SELECT;
                }
                message.type = MESSAGE_WIDGET;
                message.payload.widget.id = m_id;
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                m_flags &= ~WIDGET_FLAG_SELECTED;
                SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_DESELECT, m_id);
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
    }

    return widget::Main(message);
}

void border::Draw(void) {
    i16 x = m_owner->m_posX + m_x;
    i16 y = m_owner->m_posY + m_y;
    switch (m_kind) {
        case WIDGET_KIND_ICON:
            m_backgroundIcon->DrawToBuffer(x, y, 0, ICON_DRAW_NORMAL);
            return;
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
        default:
            return;
    }
}
