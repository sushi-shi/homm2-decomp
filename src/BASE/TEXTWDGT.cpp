#include <Ints.h>
#include <BASE/textWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <SOURCE/KB.h>
#include <string.h>

typedef enum TextWidgetConstant {
    RESOURCE_NAME_CAPACITY = 16,
    DRAW_MODE_MASK         = 0xff,
    TEXT_BUFFER_GROWTH     = 5
} TextWidgetConstant;

textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_font = NULL;
    m_text = NULL;
    m_color = FONT_DRAW_DEFAULT;
    m_alignment = FONT_ALIGN_CENTER;
    m_kind = WIDGET_KIND_TEXT;
}

textWidget::textWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    char* text,
    const char* fontName,
    FontDrawMode color,
    i16 id,
    WidgetKind kind,
    FontAlignment alignment
)
    : widget(x, y, width, height, id, kind) {
    m_font = gpResourceManager->GetFont(fontName);
    m_text = text;
    m_color = color;
    m_alignment = alignment;
    m_kind = WIDGET_KIND_TEXT;
}

void textWidget::Read(void) {
    char resourceName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    i16 len = gpResourceManager->ReadWord();
    m_text = static_cast<char*>(H2_ALLOC(len));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_text), len);
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(resourceName);
    gpResourceManager->RestorePosition();
    m_color = static_cast<FontDrawMode>(gpResourceManager->ReadWord() & DRAW_MODE_MASK);
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord() & DRAW_MODE_MASK);
    m_id = gpResourceManager->ReadWord();
    m_kind = static_cast<WidgetKind>(gpResourceManager->ReadWord());
    m_kind = WIDGET_KIND_TEXT;
}

 textWidget::~textWidget() {
    gpResourceManager->Dispose(m_font);
    H2_FREE(m_text);
}

#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                  \
    messageValue.type = MESSAGE_WIDGET;                                                          \
    messageValue.payload.widget.command = commandValue;                                          \
    messageValue.payload.widget.id = idValue

MessageDispatchResult textWidget::Main(tag_message& msg) {
    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (msg.type) {
        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_SET_TEXT:
                    if (msg.payload.widget.id == m_id) {
                        SetText(msg.payload.widget.data.text);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;

                case WIDGET_COMMAND_SET_FILL_COLOR:
                    if (msg.payload.widget.id == m_id) {
                        SetColorIndex(
                            static_cast<FontDrawMode>(msg.payload.widget.data.value)
                        );
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 relativeX = msg.payload.mouse.x - m_owner->m_posX;
            i16 relativeY = msg.payload.mouse.y - m_owner->m_posY;
            if (relativeX >= m_x && relativeY >= m_y && relativeX < m_x + m_width
                && relativeY < m_y + m_height) {
                m_flags |= WIDGET_FLAG_SELECTED;
                if (msg.type == MESSAGE_RIGHT_BUTTON_DOWN)
                    msg.payload.widget.parameter = H2EnumIndex(MESSAGE_MODIFIER_RIGHT_BUTTON);
                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_SELECT, m_id);
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
        }

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_SELECTED)))) {
                m_flags &= ~WIDGET_FLAG_SELECTED;
                if (msg.type == MESSAGE_RIGHT_BUTTON_UP)
                    msg.payload.widget.parameter = H2EnumIndex(MESSAGE_MODIFIER_RIGHT_BUTTON);
                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
    }

    return widget::Main(msg);
}

#undef SET_WIDGET_MESSAGE

void textWidget::Draw(void) {
    m_font->DrawBoundedString(
        m_text,
        m_owner->m_posX + m_x,
        m_owner->m_posY + m_y,
        m_width,
        m_height,
        (H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED)))
            ? FONT_DRAW_DIMMED
            : static_cast<FontDrawMode>(m_color),
        m_alignment
    );
}

void textWidget::SetColorIndex(FontDrawMode color) {
    m_color = color;
}

void textWidget::SetText(const char* text) {
    if (m_kind == WIDGET_KIND_TEXT || m_kind == WIDGET_KIND_TEXT_ENTRY) {
        u16 newLen = strlen(text);
        if (newLen > strlen(m_text)) {
            H2_FREE(m_text);
            m_text = static_cast<char*>(H2_ALLOC(newLen + TEXT_BUFFER_GROWTH));
        }
        strcpy(m_text, text);
    } else {
        m_text = const_cast<char*>(text);
    }
}
