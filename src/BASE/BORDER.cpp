#include <Ints.h>
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
    i16 w,
    i16 h,
    i16 e,
    WidgetKind kind,
    i16 fillColor,
    char* name
)
    : widget(x, y, w, h, e, kind) {
    if (name != NULL)
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
    else
        m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    m_fillColor = fillColor;
}

inline border::~border() {
    if (m_backgroundBitmap != NULL)
        gpResourceManager->Dispose(m_backgroundBitmap);
    if (m_backgroundIcon != NULL)
        gpResourceManager->Dispose(m_backgroundIcon);
}

void border::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    i16 kind = gpResourceManager->ReadWord();
    m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    m_kind = kind;
    char resourceName[RESOURCE_NAME_CAPACITY];
    if (kind == WIDGET_KIND_BITMAP) {
        gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
        gpResourceManager->SavePosition();
        m_backgroundBitmap = gpResourceManager->GetBitmap(resourceName);
        gpResourceManager->RestorePosition();
        return;
    }
    if (kind == WIDGET_KIND_ICON) {
        gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
        gpResourceManager->SavePosition();
        m_backgroundIcon = gpResourceManager->GetIcon(resourceName);
        gpResourceManager->RestorePosition();
        return;
    }
    m_fillColor = gpResourceManager->ReadWord() & COLOR_INDEX_MASK;
}

MessageDispatchResult border::Main(struct tag_message& msg) {
    i16 flags = m_flags;
    if (!(((flags) & (WIDGET_FLAG_ENABLED)))) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }
    MessageType type = msg.type;
    switch (type) {
        default:
            goto normalEvent;
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            goto hoverEvent;
        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            goto leaveEvent;
    }

normalEvent:
    return widget::Main(msg);

hoverEvent: {
    i16 mx = static_cast<i16>(msg.payload.mouse.x);
    heroWindow* window = m_owner;
    mx -= static_cast<i16>(window->m_posX);
    i16 my = static_cast<i16>(msg.payload.mouse.y) - window->m_posY;
    if (m_x <= mx && m_y <= my && mx < m_width + m_x && my < m_height + m_y) {
        if (type == MESSAGE_RIGHT_BUTTON_DOWN) {
            msg.payload.widget.parameter = (MESSAGE_MODIFIER_RIGHT_BUTTON);
            msg.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
        } else {
            m_flags = flags | WIDGET_FLAG_SELECTED;
            msg.payload.widget.command = WIDGET_COMMAND_SELECT;
        }
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.id = m_id;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

leaveEvent:
    if ((((flags) & (WIDGET_FLAG_SELECTED)))) {
        m_flags = flags & ~WIDGET_FLAG_SELECTED;
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
        msg.payload.widget.id = m_id;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

void border::Draw(void) {
    heroWindow* window = m_owner;
    i16 y = m_y + static_cast<i16>(window->m_posY);
    i16 x = m_x + static_cast<i16>(window->m_posX);
    WidgetKind kind = m_kind;
    switch (kind) {
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
        case WIDGET_KIND_ICON:
            m_backgroundIcon->DrawToBuffer(x, y, 0, ICON_DRAW_NORMAL);
            return;
        default:
            return;
    }
}
