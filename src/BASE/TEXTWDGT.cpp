#include <va.h>
#include <BASE/textWidget.h>
#include <BASE/TEXTWDGT_TYPES.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <SOURCE/KB.h>
#include <string.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\TEXTWDGT.CPP"

DATA(0x0051fa70) static STextWidgetSourceFiles gTextWidgetSourceFiles =
    {RETAIL_FILE, RETAIL_FILE, RETAIL_FILE, RETAIL_FILE};

H2_ENUM_BEGIN(TextWidgetConstant)
    RESOURCE_NAME_CAPACITY = 16,
    DRAW_MODE_MASK         = 0xff,
    TEXT_BUFFER_GROWTH     = 5
H2_ENUM_END(TextWidgetConstant)

VA(0x004d1060, 0x3e)
textWidget::textWidget(void) : widget(0, 0, 0, 0, 0, 0) {
    m_color = FONT_DRAW_DEFAULT;
    m_alignment = FONT_ALIGN_CENTER;
    m_font = NULL;
    m_text = NULL;
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d10f0, 0x64)
textWidget::textWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    char* text,
    char* fontName,
    H2_ENUM_PARAM(FontDrawMode, i16) color,
    i16 id,
    i16 kind,
    H2_ENUM_PARAM(FontAlignment, i16) alignment
)
    : widget(x, y, width, height, id, kind) {
    font* loadedFont = gpResourceManager->GetFont(fontName);
    m_color = color;
    m_font = loadedFont;
    m_text = text;
    m_alignment = alignment;
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d1160, 0xef)
void textWidget::Read(void) {
    char resourceName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    i16 len = gpResourceManager->ReadWord();
    m_text = static_cast<char*>(H2_ALLOC_AT(len, gTextWidgetSourceFiles.read, 57));
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_text), len);
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(resourceName);
    gpResourceManager->RestorePosition();
    m_color = static_cast<FontDrawMode>(gpResourceManager->ReadWord() & DRAW_MODE_MASK);
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    gpResourceManager->ReadWord();
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT);
}

VA(0x004d1250, 0x30)
textWidget::~textWidget() {
    gpResourceManager->Dispose(m_font);
    H2_FREE_AT(m_text, gTextWidgetSourceFiles.destruction, 0x55);
}

// Preserve the original statement stream while consolidating repeated message setup.
#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                  \
    messageValue.type = MESSAGE_WIDGET;                                                          \
    messageValue.payload.widget.command = commandValue;                                          \
    messageValue.payload.widget.id = idValue

VA(0x004d1280, 0x210)
i32 textWidget::Main(tag_message& msg) {
    u16 flags = m_flags;
    if ((flags & WIDGET_FLAG_ENABLED) == 0) {
        if (msg.type == MESSAGE_WIDGET)
            return widget::Main(msg);
        return WIDGET_DISPATCH_CONTINUE;
    }

    switch (msg.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            i16 relativeX =
                static_cast<i16>(msg.payload.mouse.x) - static_cast<i16>(m_owner->m_posX);
            i16 relativeY =
                static_cast<i16>(msg.payload.mouse.y) - static_cast<i16>(m_owner->m_posY);
            if (relativeX < m_x || relativeY < m_y || relativeX >= m_x + m_width
                || relativeY >= m_y + m_height)
                return WIDGET_DISPATCH_CONTINUE;
            m_flags = flags | WIDGET_FLAG_SELECTED;
            if (msg.type == MESSAGE_RIGHT_BUTTON_DOWN)
                msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
            SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_SELECT, m_id);
            return WIDGET_DISPATCH_FORWARD;
        }

        case MESSAGE_LEFT_BUTTON_UP:
        case MESSAGE_RIGHT_BUTTON_UP:
            if ((flags & WIDGET_FLAG_SELECTED) != 0) {
                m_flags = flags & ~WIDGET_FLAG_SELECTED;
                if (msg.type == MESSAGE_RIGHT_BUTTON_UP)
                    msg.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                SET_WIDGET_MESSAGE(msg, WIDGET_COMMAND_DESELECT, m_id);
                return WIDGET_DISPATCH_FORWARD;
            }
            return WIDGET_DISPATCH_CONTINUE;

        case MESSAGE_WIDGET:
            switch (msg.payload.widget.command) {
                case WIDGET_COMMAND_SET_TEXT: {
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    char* newText = msg.payload.widget.data.text;
                    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT
                        && DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT_ENTRY) {
                        m_text = newText;
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    u16 newLen = strlen(newText);
                    if (strlen(m_text) < newLen) {
                        H2_FREE_AT(m_text, gTextWidgetSourceFiles.resizeFree, 0xd3);
                        m_text = static_cast<char*>(
                            H2_ALLOC_AT(
                                newLen + TEXT_BUFFER_GROWTH,
                                gTextWidgetSourceFiles.resizeAlloc,
                                212
                            )
                        );
                    }
                    strcpy(m_text, newText);
                    return WIDGET_DISPATCH_CONSUME;
                }

                case WIDGET_COMMAND_SET_FILL_COLOR:
                    if (m_id != msg.payload.widget.id)
                        goto normalEvent;
                    m_color = static_cast<FontDrawMode>(msg.payload.widget.data.value);
                    return WIDGET_DISPATCH_CONSUME;

                default:
                    goto normalEvent;
            }

        default:
            goto normalEvent;
    }

normalEvent:
    return widget::Main(msg);
}

#undef SET_WIDGET_MESSAGE

VA(0x004d1490, 0x49)
void textWidget::Draw(void) {
    FontDrawMode color = FONT_DRAW_DIMMED;
    if ((m_flags & WIDGET_FLAG_DIMMED) == 0)
        color = m_color;
    m_font->DrawBoundedString(
        m_text,
        m_x + m_owner->m_posX,
        m_y + m_owner->m_posY,
        m_width,
        m_height,
        color,
        m_alignment
    );
}

VA(0x004d14e0, 0xc)
void textWidget::SetColorIndex(H2_ENUM_PARAM(FontDrawMode, i16) color) {
    m_color = color;
}

VA(0x004d14f0, 0xa2)
void textWidget::SetText(char* text) {
    if (DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT
        && DecodeWidgetKind(m_kind) != WIDGET_KIND_TEXT_ENTRY) {
        m_text = text;
        return;
    }
    u16 newLen = strlen(text);
    if (strlen(m_text) < newLen) {
        H2_FREE_AT(m_text, gTextWidgetSourceFiles.resizeFree, 0xd3);
        m_text = static_cast<char*>(H2_ALLOC_AT(
            newLen + TEXT_BUFFER_GROWTH,
            gTextWidgetSourceFiles.resizeAlloc,
            212
        ));
    }
    strcpy(m_text, text);
}


VTBL(textWidget, 0x004eba50);

#undef RETAIL_FILE
