#include <va.h>
#include <BASE/textEntryWidget.h>
#include <BASE/widgetKind.h>
#include <BASE/resourceManager.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <string.h>

H2_ENUM_BEGIN(TextEntryKeyConstant)
    ACCEPT_KEY         = 10,
    DELETE_KEY         = 0x7f,
    EXTENDED_KEY_SHIFT = 8,
    EXTENDED_KEY_MASK  = 0xff
H2_ENUM_END(TextEntryKeyConstant)

H2_ENUM_BEGIN(TextEntrySourceFileConstant)
    ENTRY_SOURCE_FILE_SLOT_SIZE = 0x2c
H2_ENUM_END(TextEntrySourceFileConstant)

H2_ENUM_BEGIN(TextEntryConstant)
    RESOURCE_NAME_CAPACITY      = RESOURCE_MANAGER_READ13_BYTES,
    COLOR_MASK                  = 0xff,
    HORIZONTAL_INSET_SIDE_COUNT = 2,
    SERIALIZED_HORIZONTAL_INSET = 7,
    INSET_FIVE_VERTICAL_INSET   = 5,
    INSET_FOUR_VERTICAL_INSET   = 4,
    EDIT_BUFFER_CAPACITY        = 600,
    DISPLAY_BUFFER_CAPACITY     = 300,
    CURSOR_BLINK_TICKS          = 360,
    TEXT_ALLOCATION_PADDING     = 5,
    EDIT_ALLOCATION_PADDING     = 6,
    PRESERVE_TEXT_FLAG          = 1
H2_ENUM_END(TextEntryConstant)

H2_ENUM_CLASS_BEGIN(InputManagerExtendedKey)
    EXTENDED_KEY_BASE = 0x100
// extended keys arrive as scan code << 8
H2_ENUM_CLASS_END(InputManagerExtendedKey)

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\Textntry.cpp"
#define TEXT_ENTRY_MAIN_SOURCE_FILES RETAIL_FILE "\0\0\0" RETAIL_FILE

VA(0x004d8740, 0x2d)
textEntryWidget::textEntryWidget(void) : textWidget() {
    m_cursorPosition = 0;
    m_icon = NULL;
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT_ENTRY);
    m_maxLength = 0;
    m_iconFrame = 0;
    m_displayOffset = 0;
}

VA(0x004d87b0, 0x134)
textEntryWidget::textEntryWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 maxLength,
    char* text,
    char* fontName,
    H2_ENUM_PARAM(FontDrawMode, i16) color,
    char* iconName,
    i16 iconFrame,
    i16 id,
    i16 kind,
    i16 layout,
    i32 horizontalInset,
    i32 verticalInset
)
    : textWidget(x, y, width, height, text, fontName, color, id, kind, FONT_ALIGN_CENTER) {
    m_cursorPosition = 0;
    m_maxLength = maxLength;
    icon* loadedIcon = gpResourceManager->GetIcon(iconName);
    i16 rectX = m_x;
    m_displayOffset = 0;
    m_icon = loadedIcon;
    m_iconFrame = iconFrame;
    m_rectX = rectX;
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT_ENTRY);
    m_rectY = m_y;
    m_rectW = m_width;
    m_maxLength = maxLength;
    m_maxLines = 1;
    m_preserveTextOnFocus = 0;
    m_color = FONT_DRAW_DEFAULT;
    m_rectH = m_height;
#line 61 RETAIL_FILE
    m_text = static_cast<char*>(
        H2_ALLOC(static_cast<u16>(maxLength) + TEXT_ALLOCATION_PADDING, 62)
    );
    strcpy(m_text, text);
    if (layout == TEXT_ENTRY_LAYOUT_INSET) {
        m_innerX = horizontalInset + m_x;
        m_preserveTextOnFocus = 1;
        m_innerY = verticalInset + m_y;
        m_innerW = m_width + -HORIZONTAL_INSET_SIDE_COUNT * horizontalInset;
        m_innerH = m_height;
    }
}

VA(0x004d88f0, 0x21)
textEntryWidget::~textEntryWidget() {
    gpResourceManager->Dispose(m_icon);
}

VA(0x004d8920, 0x26c)
void textEntryWidget::Read(i32 type) {
    char resourceName[RESOURCE_NAME_CAPACITY];
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_maxLength = gpResourceManager->ReadWord();
#line 99
    m_text = static_cast<char*>(
        H2_ALLOC_AT(m_maxLength + TEXT_ALLOCATION_PADDING, RETAIL_FILE "\0", 99)
    );
    gpResourceManager->ReadBlock(reinterpret_cast<i8*>(m_text), m_maxLength);
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(resourceName);
    gpResourceManager->RestorePosition();
    m_color = static_cast<FontDrawMode>(gpResourceManager->ReadWord() & COLOR_MASK);
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord());
    gpResourceManager->Read13(reinterpret_cast<i8*>(resourceName));
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(resourceName);
    gpResourceManager->RestorePosition();
    m_entryType = static_cast<i16>(type);
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
        i32 preserveText;
        m_rectH = m_height;
        if (type == TEXT_ENTRY_READ_MULTILINE)
            preserveText = 1;
        else
            preserveText = 1;
        m_maxLines = preserveText;
        m_preserveTextOnFocus = preserveText;
        if (type != TEXT_ENTRY_READ_MULTILINE)
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
    gpResourceManager->ReadWord();
    m_kind = EncodeWidgetKind(WIDGET_KIND_TEXT_ENTRY);
}

VA(0x004d8b90, 0x874)
i32 textEntryWidget::Main(struct tag_message& message) {
    if ((m_flags & WIDGET_FLAG_ENABLED) == 0) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return 0;
    }
    switch (message.type) {
        default:
            break;
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            m_cursorBlink = 1;
            i16 windowX = static_cast<i16>(m_owner->m_posX);
            i16 mouseX = static_cast<i16>(message.payload.mouse.x - windowX);
            i16 windowY = static_cast<i16>(m_owner->m_posY);
            i16 mouseY = static_cast<i16>(message.payload.mouse.y - windowY);
            if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (mouseX < m_x || mouseY < m_y || mouseX >= m_x + m_width
                    || mouseY >= m_y + m_height)
                    return 0;
                message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                message.type = MESSAGE_WIDGET;
                message.payload.widget.id = m_id;
                message.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                return WIDGET_DISPATCH_FORWARD;
            }
            if (mouseX >= m_x && mouseY >= m_y && mouseX < m_x + m_width
                && mouseY < m_y + m_height) {
                mouseX = m_x + windowX;
                char original[EDIT_BUFFER_CAPACITY];
                char edit[EDIT_BUFFER_CAPACITY];
                char scratch[EDIT_BUFFER_CAPACITY];
                char backup[EDIT_BUFFER_CAPACITY];
                mouseY = m_y + windowY;
                strcpy(original, m_text);
                if ((m_preserveTextOnFocus & PRESERVE_TEXT_FLAG) != 0) {
                    m_cursorPosition = static_cast<u16>(strlen(m_text));
                } else {
                    m_cursorPosition = 0;
                    m_text[0] = 0;
                }
                strcpy(edit, m_text);
                SetupDisplayString(edit, m_cursorPosition);
                Draw();
                gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                i16 done = 0;
                glTimers[0] = KBTickCount() + CURSOR_BLINK_TICKS;
                gpMouseManager->ReallyHidePointer();
                tag_message event;
                do {
                    if (KBTickCount() > glTimers[0]) {
                        SetupDisplayString(edit, m_cursorPosition);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                    }
                    Process1WindowsMessage();
                    event = gpInputManager->GetEvent();
                    if (event.type == MESSAGE_KEY_DOWN) {
                        switch (event.payload.keyboard.keyCode) {
                            case INPUT_SCAN_ESCAPE:
                                if (gbAllowTextEntryEscape != 0) {
                                    strcpy(edit, original);
                                    done++;
                                    gbTextEntryEscaped = true;
                                }
                                break;
                            case INPUT_SCAN_NUMPAD_4:
                                if (m_cursorPosition != 0) {
                                    m_cursorPosition--;
                                    if (m_cursorPosition < m_displayOffset)
                                        m_displayOffset = m_cursorPosition;
                                }
                                break;
                            case INPUT_SCAN_NUMPAD_6:
                                if (m_cursorPosition < strlen(edit))
                                    m_cursorPosition++;
                                break;
                            case INPUT_SCAN_NUMPAD_DELETE:
                                if (m_cursorPosition < strlen(edit)) {
                                    strcpy(scratch, edit + m_cursorPosition + 1);
                                    strcpy(edit + m_cursorPosition, scratch);
                                }
                                break;
                            default:
                                gpInputManager->AsciiConvert(event);
                                if (event.payload.keyboard.keyCode == ACCEPT_KEY) {
                                    gbTextEntryEscaped = false;
                                    done++;
                                } else if (event.payload.keyboard.keyCode
                                           == DELETE_KEY) {
                                    if (m_cursorPosition != 0) {
                                        strcpy(scratch, edit + m_cursorPosition);
                                        strcpy(edit + m_cursorPosition - 1, scratch);
                                        m_cursorPosition--;
                                        if (m_cursorPosition < m_displayOffset)
                                            m_displayOffset = m_cursorPosition;
                                    }
                                } else if (strlen(edit) + 1 < m_maxLength
                                           && event.payload.keyboard.keyCode != 0) {
                                    strcpy(backup, edit);
                                    char typed = 0;
                                    if (event.payload.keyboard.keyCode >= IDX(EXTENDED_KEY_BASE)) {
                                        switch ((event.payload.keyboard.keyCode
                                                 >> EXTENDED_KEY_SHIFT)
                                                & EXTENDED_KEY_MASK) {
                                            case INPUT_SCAN_NUMPAD_7:
                                                typed = '7';
                                                break;
                                            case INPUT_SCAN_NUMPAD_8:
                                                typed = '8';
                                                break;
                                            case INPUT_SCAN_NUMPAD_9:
                                                typed = '9';
                                                break;
                                            case INPUT_SCAN_NUMPAD_4:
                                                typed = '4';
                                                break;
                                            case INPUT_SCAN_NUMPAD_5:
                                                typed = '5';
                                                break;
                                            case INPUT_SCAN_NUMPAD_6:
                                                typed = '6';
                                                break;
                                            case INPUT_SCAN_NUMPAD_1:
                                                typed = '1';
                                                break;
                                            case INPUT_SCAN_NUMPAD_2:
                                                typed = '2';
                                                break;
                                            case INPUT_SCAN_NUMPAD_3:
                                                typed = '3';
                                                break;
                                            case INPUT_SCAN_NUMPAD_0:
                                                typed = '0';
                                                break;
                                        }
                                    } else {
                                        typed = static_cast<char>(event.payload.keyboard.keyCode);
                                        if (typed == '{' || typed == '}')
                                            typed = 0;
                                    }
                                    if (typed != 0) {
                                        strcpy(scratch, m_text);
#line 388
                                        H2_FREE_AT(m_text, TEXT_ENTRY_MAIN_SOURCE_FILES, 0x184);
#line 389
                                        m_text = static_cast<char*>(H2_ALLOC_AT(
                                            strlen(edit) + EDIT_ALLOCATION_PADDING,
                                            TEXT_ENTRY_MAIN_SOURCE_FILES
                                                + ENTRY_SOURCE_FILE_SLOT_SIZE,
                                            389
                                        ));
                                        strcpy(scratch, edit);
                                        scratch[m_cursorPosition] = typed;
                                        scratch[m_cursorPosition + 1] = 0;
                                        strcat(scratch, edit + m_cursorPosition);
                                        strcpy(edit, scratch);
                                        m_cursorPosition++;
                                        SetupDisplayString(edit, m_cursorPosition);
                                        if (m_entryType != TEXT_ENTRY_READ_MULTILINE) {
                                            i32 lineLength = m_font->LineLength(m_text, m_innerW);
                                            if (m_maxLines >= lineLength) {
                                            } else {
                                                strcpy(edit, backup);
                                                m_cursorPosition--;
                                            }
                                        }
                                    }
                                }
                                break;
                        }
                        SetupDisplayString(edit, m_cursorPosition);
                        Draw();
                        gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                    }
                } while (done == 0);
                gpMouseManager->ReallyShowPointer();
                strcpy(m_text, edit);
                m_displayOffset = 0;
                Draw();
                gpWindowManager->UpdateScreenRegion(mouseX, mouseY, m_width, m_height);
                message.payload.widget.command = WIDGET_COMMAND_SELECT;
                message.type = MESSAGE_WIDGET;
                message.payload.widget.id = m_id;
                return WIDGET_DISPATCH_FORWARD;
            }
            return 0;
        }
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SET_TEXT:
                    if (message.payload.widget.id == m_id) {
                        SetText(message.payload.widget.data.text);
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_GET_TEXT:
                    if (message.payload.widget.id == m_id) {
                        message.payload.widget.data.text = m_text;
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_SET_MAX_LENGTH:
                    if (message.payload.widget.id == m_id) {
                        m_maxLength = static_cast<u16>(message.payload.widget.data.value);
                        return WIDGET_DISPATCH_CONSUME;
                    }
                    break;
            }
            break;
    }
    return widget::Main(message);
}

VA(0x004d9410, 0x160)
void textEntryWidget::Draw(void) {
    if (m_entryType == TEXT_ENTRY_READ_MULTILINE) {
        char display[EDIT_BUFFER_CAPACITY];
        strcpy(display, m_text + m_displayOffset);
        u32 length = strlen(display);
        if (m_font->LineWidth(display) > m_innerW) {
            do {
                display[length - 1] = 0;
                length--;
            } while (m_font->LineWidth(display) > m_innerW);
        }
        m_icon->DrawToBuffer(m_rectX + m_owner->m_posX, m_rectY + m_owner->m_posY, m_iconFrame, 0);
        m_font->DrawBoundedString(
            display,
            m_innerX + m_owner->m_posX,
            m_innerY + m_owner->m_posY,
            m_innerW,
            m_innerH,
            m_color,
            m_alignment
        );
    } else {
        m_icon->DrawToBuffer(m_rectX + m_owner->m_posX, m_rectY + m_owner->m_posY, m_iconFrame, 0);
        FontDrawMode color = FONT_DRAW_DIMMED;
        if ((m_flags & WIDGET_FLAG_DIMMED) == 0)
            color = m_color;
        m_font->DrawBoundedString(
            m_text,
            m_innerX + m_owner->m_posX,
            m_innerY + m_owner->m_posY,
            m_innerW,
            m_innerH,
            color,
            m_alignment
        );
    }
}

VA(0x004d9570, 0x1be)
void textEntryWidget::SetupDisplayString(char* source, u16 cursor) {
    if (KBTickCount() > glTimers[0]) {
        m_cursorBlink = 1 - m_cursorBlink;
        glTimers[0] = KBTickCount() + CURSOR_BLINK_TICKS;
    }
    if (cursor != 0)
        strncpy(m_text, source, cursor);
    if (m_cursorBlink != 0)
        m_text[cursor] = FONT_SPACER_CHAR;
    else
        m_text[cursor] = '_';
    if (cursor < strlen(source))
        strcpy(m_text + cursor + 1, source + cursor);
    else
        m_text[cursor + 1] = 0;

    if (m_entryType == TEXT_ENTRY_READ_MULTILINE) {
        i32 shifted;
        char display[DISPLAY_BUFFER_CAPACITY];
        do {
            shifted = 0;
            strcpy(display, m_text + m_displayOffset);
            if (m_font->LineWidth(display) > m_innerW) {
                display[cursor - m_displayOffset + 1] = 0;
                if (m_font->LineWidth(display) > m_innerW) {
                    shifted = 1;
                    m_displayOffset++;
                }
            }
        } while (shifted);
        if (m_displayOffset > 0) {
            do {
                shifted = 0;
                strcpy(display, m_text + m_displayOffset - 1);
                if (m_font->LineWidth(display) <= m_innerW)
                    m_displayOffset--;
                else
                    shifted = 0;
                if (m_displayOffset == 0)
                    shifted = 0;
            } while (shifted != 0);
        }
    }
}


VTBL(textEntryWidget, 0x004ebaa0);

#undef RETAIL_FILE
