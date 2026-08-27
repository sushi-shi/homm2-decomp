#include <Ints.h>
#include <BASE/textEntryWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/Utf8.h>
#include <SOURCE/KB.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Runtime.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <string.h>

#include <algorithm>
#include <vector>

typedef enum TextEntrySourceFileConstant {
    ENTRY_SOURCE_FILE_SLOT_SIZE = 0x2c
} TextEntrySourceFileConstant;

typedef enum TextEntryConstant {
    RESOURCE_NAME_CAPACITY      = RESOURCE_MANAGER_READ13_BYTES,
    COLOR_MASK                  = 0xff,
    HORIZONTAL_INSET_SIDE_COUNT = 2,
    SERIALIZED_HORIZONTAL_INSET = 7,
    INSET_FIVE_VERTICAL_INSET   = 5,
    INSET_FOUR_VERTICAL_INSET   = 4,
    TEXT_BUFFER_CAPACITY        = 1000,
    CURSOR_BLINK_TICKS          = 360,
    TEXT_ALLOCATION_PADDING     = 5,
    EDIT_ALLOCATION_PADDING     = 6,
    PRESERVE_TEXT_FLAG          = 1
} TextEntryConstant;

textEntryWidget::textEntryWidget(void) : textWidget() {
    m_cursorPosition = 0;
    m_maxLength = 0;
    m_icon = NULL;
    m_iconFrame = 0;
    m_displayOffset = 0;
    m_kind = WIDGET_KIND_TEXT_ENTRY;
}

textEntryWidget::textEntryWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 maxLength,
    const char* text,
    const char* fontName,
    FontDrawMode color,
    const char* iconName,
    i16 iconFrame,
    i16 id,
    WidgetKind kind,
    TextEntryLayout layout,
    i32 horizontalInset,
    i32 verticalInset
)
    : textWidget(x, y, width, height, text, fontName, color, id, kind, FONT_ALIGN_CENTER) {
    m_cursorPosition = 0;
    m_maxLength = maxLength;
    m_icon = gpResourceManager->GetIcon(iconName);
    m_iconFrame = iconFrame;
    m_displayOffset = 0;
    m_kind = WIDGET_KIND_TEXT_ENTRY;
    m_rectX = m_x;
    m_rectY = m_y;
    m_rectW = m_width;
    m_rectH = m_height;
    m_maxLines = 1;
    m_preserveTextOnFocus = 0;
    m_color = FONT_DRAW_DEFAULT;
    m_maxLength = maxLength;
    const std::size_t allocation = std::max<std::size_t>(
        static_cast<std::size_t>(m_maxLength) + TEXT_ALLOCATION_PADDING,
        strlen(text) + TEXT_ALLOCATION_PADDING
    );
    H2_FREE(m_text);
    m_text = static_cast<char*>(H2_ALLOC(allocation));
    strcpy(m_text, text);
    if (layout == TEXT_ENTRY_LAYOUT_INSET) {
        m_preserveTextOnFocus = 1;
        m_innerX = m_x + horizontalInset;
        m_innerY = m_y + verticalInset;
        m_innerW = m_width - HORIZONTAL_INSET_SIDE_COUNT * horizontalInset;
        m_innerH = m_height;
    }
}

inline textEntryWidget::~textEntryWidget() {
    gpResourceManager->Dispose(m_icon);
}

void textEntryWidget::Read(TextEntryReadMode type) {
    char resourceName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_maxLength = gpResourceManager->ReadWord();
    std::vector<char> legacyText(static_cast<std::size_t>(m_maxLength) + 1, 0);
    gpResourceManager->ReadBlock(
        reinterpret_cast<i8*>(legacyText.data()), m_maxLength
    );
    const std::string decodedText = localization::DecodeResourceText(legacyText.data());
    const std::size_t allocation = std::max<std::size_t>(
        static_cast<std::size_t>(m_maxLength) + TEXT_ALLOCATION_PADDING,
        decodedText.size() + TEXT_ALLOCATION_PADDING
    );
    m_text = static_cast<char*>(H2_ALLOC(allocation));
    memcpy(m_text, decodedText.c_str(), decodedText.size() + 1);
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(resourceName);
    gpResourceManager->RestorePosition();
    m_color = static_cast<FontDrawMode>(gpResourceManager->ReadWord() & COLOR_MASK);
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord() & COLOR_MASK);
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(resourceName);
    gpResourceManager->RestorePosition();
    m_entryType = type;
    if (type == TEXT_ENTRY_READ_RECT) {
        m_rectX = gpResourceManager->ReadWord();
        m_rectY = gpResourceManager->ReadWord();
        m_rectW = gpResourceManager->ReadWord();
        m_rectH = gpResourceManager->ReadWord();
        m_maxLines = gpResourceManager->ReadWord();
        m_preserveTextOnFocus = gpResourceManager->ReadWord();
    } else {
        m_rectX = m_x;
        m_rectY = m_y;
        m_rectW = m_width;
        m_rectH = m_height;
        m_maxLines = 1;
        if (type == TEXT_ENTRY_READ_MULTILINE)
            m_preserveTextOnFocus = 1;
        else
            m_preserveTextOnFocus = 0;
    }
    if (type == TEXT_ENTRY_READ_INSET_FIVE) {
        m_innerX = m_x + SERIALIZED_HORIZONTAL_INSET;
        m_innerY = m_y + INSET_FIVE_VERTICAL_INSET;
        m_innerW = m_width - SERIALIZED_HORIZONTAL_INSET * HORIZONTAL_INSET_SIDE_COUNT;
        m_innerH = m_height;
    } else if (type == TEXT_ENTRY_READ_INSET_FOUR) {
        m_innerX = m_x + SERIALIZED_HORIZONTAL_INSET;
        m_innerY = m_y + INSET_FOUR_VERTICAL_INSET;
        m_innerW = m_width - SERIALIZED_HORIZONTAL_INSET * HORIZONTAL_INSET_SIDE_COUNT;
        m_innerH = m_height;
    } else {
        m_innerX = m_x;
        m_innerY = m_y;
        m_innerW = m_width;
        m_innerH = m_height;
    }
    m_iconFrame = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = static_cast<WidgetKind>(gpResourceManager->ReadWord());
    m_kind = WIDGET_KIND_TEXT_ENTRY;
}

MessageDispatchResult textEntryWidget::Main(struct tag_message& message) {
    i16 done;
    i16 x;
    i16 y;
    tag_message event;

    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return MESSAGE_DISPATCH_CONTINUE;
    }
    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SET_MAX_LENGTH:
                    if (message.payload.widget.id == m_id) {
                        m_maxLength = message.payload.widget.data.value;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_SET_TEXT:
                    if (message.payload.widget.id == m_id) {
                        SetText(message.payload.widget.data.text);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_GET_TEXT:
                    if (message.payload.widget.id == m_id) {
                        message.payload.widget.data.text = m_text;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            m_cursorBlink = 1;
            x = message.payload.mouse.x - m_owner->m_posX;
            y = message.payload.mouse.y - m_owner->m_posY;
            if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                    message.payload.widget.id = m_id;
                    message.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            }
            if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                char original[TEXT_BUFFER_CAPACITY];
                char edit[TEXT_BUFFER_CAPACITY];
                char copy[TEXT_BUFFER_CAPACITY];
                char swap[TEXT_BUFFER_CAPACITY];

                x = m_owner->m_posX + m_x;
                y = m_owner->m_posY + m_y;
                strcpy(original, m_text);
                if ((m_preserveTextOnFocus & PRESERVE_TEXT_FLAG) != 0) {
                    m_cursorPosition = strlen(m_text);
                } else {
                    m_cursorPosition = 0;
                    m_text[0] = 0;
                }
                strcpy(edit, m_text);
                SetupDisplayString(edit, m_cursorPosition);
                Draw();
                gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
                done = 0;
                glTimers[0] = platform::Ticks() + CURSOR_BLINK_TICKS;
                gpMouseManager->ReallyHidePointer();
                platform::StartTextInput();
                const auto insertCodePoint = [&](std::uint32_t codePoint) {
                    if (codePoint < ' ' || codePoint == '{' || codePoint == '}')
                        return;

                    char encoded[4];
                    const std::size_t encodedLength = utf8::Encode(codePoint, encoded);
                    const std::size_t editLength = strlen(edit);
                    if (encodedLength == 0
                        || editLength + encodedLength >= static_cast<std::size_t>(m_maxLength)
                        || editLength + encodedLength >= TEXT_BUFFER_CAPACITY)
                        return;

                    strcpy(copy, edit);
                    H2_FREE(m_text);
                    m_text = static_cast<char*>(
                        H2_ALLOC(editLength + encodedLength + EDIT_ALLOCATION_PADDING)
                    );
                    memmove(
                        edit + m_cursorPosition + encodedLength,
                        edit + m_cursorPosition,
                        editLength - m_cursorPosition + 1
                    );
                    memcpy(edit + m_cursorPosition, encoded, encodedLength);
                    const u16 oldCursor = m_cursorPosition;
                    m_cursorPosition += static_cast<u16>(encodedLength);
                    SetupDisplayString(edit, m_cursorPosition);
                    if (m_entryType != TEXT_ENTRY_READ_MULTILINE
                        && m_font->LineLength(m_text, m_innerW) > m_maxLines) {
                        strcpy(edit, copy);
                        m_cursorPosition = oldCursor;
                    }
                };
                while (done == 0) {
                    if (platform::Ticks() > glTimers[0]) {
                        SetupDisplayString(edit, m_cursorPosition);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
                    }
                    platform::PumpEvents();
                    event = gpInputManager->GetEvent();
                    bool redraw = false;
                    if (event.type == MESSAGE_KEY_DOWN) {
                        redraw = true;
                        switch (event.payload.keyboard.keyCode) {
                            case INPUT_SCAN_ESCAPE:
                                if (gbAllowTextEntryEscape == 0)
                                    break;
                                strcpy(edit, original);
                                done++;
                                gbTextEntryEscaped = true;
                                break;
                            case INPUT_SCAN_NUMPAD_DELETE:
                                if (m_cursorPosition < strlen(edit)) {
                                    const std::size_t next = utf8::Next(edit, m_cursorPosition);
                                    strcpy(swap, edit + next);
                                    strcpy(edit + m_cursorPosition, swap);
                                }
                                break;
                            case INPUT_SCAN_NUMPAD_4:
                                if (m_cursorPosition > 0) {
                                    m_cursorPosition = static_cast<u16>(
                                        utf8::Previous(edit, m_cursorPosition)
                                    );
                                    if (m_cursorPosition < m_displayOffset)
                                        m_displayOffset = m_cursorPosition;
                                }
                                break;
                            case INPUT_SCAN_NUMPAD_6:
                                if (m_cursorPosition < strlen(edit)) {
                                    m_cursorPosition = static_cast<u16>(
                                        utf8::Next(edit, m_cursorPosition)
                                    );
                                }
                                break;
                            case INPUT_SCAN_ENTER:
                                gbTextEntryEscaped = false;
                                done++;
                                break;
                            case INPUT_SCAN_BACKSPACE:
                                if (m_cursorPosition > 0) {
                                    const std::size_t previous =
                                        utf8::Previous(edit, m_cursorPosition);
                                    strcpy(swap, edit + m_cursorPosition);
                                    strcpy(edit + previous, swap);
                                    m_cursorPosition = static_cast<u16>(previous);
                                    if (m_cursorPosition < m_displayOffset)
                                        m_displayOffset = m_cursorPosition;
                                }
                                break;
                            default:
                                break;
                        }
                    } else if (event.type == MESSAGE_TEXT_INPUT) {
                        redraw = true;
                        insertCodePoint(
                            static_cast<std::uint32_t>(event.payload.keyboard.keyCode)
                        );
                    }
                    if (redraw) {
                        SetupDisplayString(edit, m_cursorPosition);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
                    }
                }
                platform::StopTextInput();
                gpMouseManager->ReallyShowPointer();
                strcpy(m_text, edit);
                m_displayOffset = 0;
                Draw();
                gpWindowManager->UpdateScreenRegion(x, y, m_width, m_height);
                message.type = MESSAGE_WIDGET;
                message.payload.widget.command = WIDGET_COMMAND_SELECT;
                message.payload.widget.id = m_id;
                return MESSAGE_DISPATCH_FORWARD;
            }
            return MESSAGE_DISPATCH_CONTINUE;
    }
    return widget::Main(message);
}

void textEntryWidget::Draw(void) {
    if (m_entryType == TEXT_ENTRY_READ_MULTILINE) {
        char display[TEXT_BUFFER_CAPACITY];
        u32 len;

        strcpy(display, m_text + m_displayOffset);
        len = strlen(display);
        while (m_font->LineWidth(display) > m_innerW) {
            len = utf8::Previous(display, len);
            display[len] = 0;
        }
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_rectX,
            m_owner->m_posY + m_rectY,
            m_iconFrame,
            ICON_DRAW_NORMAL
        );
        m_font->DrawBoundedString(
            display,
            m_owner->m_posX + m_innerX,
            m_owner->m_posY + m_innerY,
            m_innerW,
            m_innerH,
            m_color,
            m_alignment
        );
    } else {
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_rectX,
            m_owner->m_posY + m_rectY,
            m_iconFrame,
            ICON_DRAW_NORMAL
        );
        m_font->DrawBoundedString(
            m_text,
            m_owner->m_posX + m_innerX,
            m_owner->m_posY + m_innerY,
            m_innerW,
            m_innerH,
            (H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))) ? FONT_DRAW_DIMMED : static_cast<FontDrawMode>(m_color),
            m_alignment
        );
    }
}

void textEntryWidget::SetupDisplayString(char* source, u16 cursor) {
    b32 changed;
    char display[TEXT_BUFFER_CAPACITY];

    if (platform::Ticks() > glTimers[0]) {
        m_cursorBlink = 1 - m_cursorBlink;
        glTimers[0] = platform::Ticks() + CURSOR_BLINK_TICKS;
    }
    if (cursor > 0)
        strncpy(m_text, source, cursor);
    if (m_cursorBlink != 0)
        m_text[cursor] = FONT_SPACER_CHAR;
    else
        m_text[cursor] = '_';
    if (strlen(source) > cursor)
        strcpy(m_text + cursor + 1, source + cursor);
    else
        m_text[cursor + 1] = 0;
    if (m_entryType == TEXT_ENTRY_READ_MULTILINE) {
        changed = true;
        while (changed) {
            changed = false;
            strcpy(display, m_text + m_displayOffset);
            if (m_font->LineWidth(display) > m_innerW) {
                display[cursor - m_displayOffset + 1] = 0;
                if (m_font->LineWidth(display) > m_innerW) {
                    m_displayOffset = static_cast<i16>(
                        utf8::Next(source, m_displayOffset)
                    );
                    changed = true;
                }
            }
        }
        if (m_displayOffset > 0) {
            changed = true;
            while (changed) {
                changed = false;
                const i16 previous = static_cast<i16>(
                    utf8::Previous(source, m_displayOffset)
                );
                strcpy(display, m_text + previous);
                if (m_font->LineWidth(display) <= m_innerW)
                    m_displayOffset = previous;
                else
                    changed = false;
                if (m_displayOffset == 0)
                    changed = false;
            }
        }
    }
}
