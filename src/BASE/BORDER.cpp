#include <va.h>
#include <BASE/border.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/bmap2.h>
#include <BASE/bitmap.h>
#include <BASE/icon.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/KB.h>

H2_ENUM_BEGIN(BorderConstant)
    RESOURCE_NAME_CAPACITY = 16,
    COLOR_INDEX_MASK       = 0xff
H2_ENUM_END(BorderConstant)

VA(0x004cb0a0, 0x48)
border::border(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_fillColor = 0;
    m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
}


VA(0x004cb120, 0xa8)
border::border(
    i16 x,
    i16 y,
    i16 w,
    i16 h,
    i16 e,
    H2_ENUM_PARAM(WidgetKind, i16) kind,
    i16 fillColor,
    H2_CONST char* name
)
    : widget(x, y, w, h, e, kind) {
    if (name != NULL)
        m_backgroundBitmap = gpResourceManager->GetBitmap(name);
    else
        m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    m_fillColor = fillColor;
}

VA(0x004cb1d0, 0x7f)
H2_RETAIL_INLINE border::~border() {
    if (m_backgroundBitmap != NULL)
        gpResourceManager->Dispose(m_backgroundBitmap);
    if (m_backgroundIcon != NULL)
        gpResourceManager->Dispose(m_backgroundIcon);
}

VA(0x004cb250, 0x13c)
void border::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
    m_backgroundBitmap = NULL;
    m_backgroundIcon = NULL;
    if (m_kind == WIDGET_KIND_BITMAP) {
        char bitmapName[RESOURCE_NAME_CAPACITY];
        gpResourceManager->Read13(reinterpret_cast<i8*>(bitmapName));
        gpResourceManager->SavePosition();
        m_backgroundBitmap = gpResourceManager->GetBitmap(bitmapName);
        gpResourceManager->RestorePosition();
        return;
    }
    if (m_kind == WIDGET_KIND_ICON) {
        char iconName[RESOURCE_NAME_CAPACITY];
        gpResourceManager->Read13(reinterpret_cast<i8*>(iconName));
        gpResourceManager->SavePosition();
        m_backgroundIcon = gpResourceManager->GetIcon(iconName);
        gpResourceManager->RestorePosition();
        return;
    }
    m_fillColor = gpResourceManager->ReadWord() & COLOR_INDEX_MASK;
}

VA(0x004cb390, 0x1de)
MessageDispatchResult border::Main(struct tag_message& msg) {
    if (!HAS(m_flags, WIDGET_FLAG_ENABLED)) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (msg.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 x = msg.payload.mouse.x - m_owner->m_posX;
            i16 y = msg.payload.mouse.y - m_owner->m_posY;
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
        }

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if (HAS(m_flags, WIDGET_FLAG_SELECTED)) {
                m_flags &= ~WIDGET_FLAG_SELECTED;
                msg.type = MESSAGE_WIDGET;
                msg.payload.widget.command = WIDGET_COMMAND_DESELECT;
                msg.payload.widget.id = m_id;
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
    }

    return widget::Main(msg);
}

VA(0x004cb570, 0xec)
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

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(border, 0x004ea9c4)
