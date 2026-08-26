#include <va.h>
#include <BASE/dropListWidget.h>
#include <BASE/bitmap.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <string.h>

H2_ENUM_CLASS_BEGIN(DropListFrame)
    FRAME_CLOSED_CONTENT      = 0,
    FRAME_DROP_BUTTON         = 1,
    FRAME_DROP_BUTTON_PRESSED = 2,
    FRAME_FIRST_ROW           = 3,
    FRAME_MIDDLE_ROW          = 4,
    FRAME_LAST_ROW            = 5,
    FRAME_SCROLL_UP           = 6,
    FRAME_SCROLL_UP_PRESSED   = 7,
    FRAME_SCROLL_DOWN         = 8,
    FRAME_SCROLL_DOWN_PRESSED = 9,
    FRAME_SCROLL_TRACK_FIRST  = 10,
    FRAME_SCROLL_TRACK_MIDDLE = 11,
    FRAME_SCROLL_TRACK_LAST   = 12,
    FRAME_SCROLL_THUMB        = 13
H2_ENUM_CLASS_END(DropListFrame)

H2_ENUM_BEGIN(DropListStorageConstant)
    RESOURCE_NAME_CAPACITY = 16
H2_ENUM_END(DropListStorageConstant)

H2_ENUM_BEGIN(DropListLayoutConstant)
    MIN_VISIBLE_ITEM_COUNT      = 3,
    TEXT_LEFT_INSET             = 5,
    TEXT_HORIZONTAL_INSET_COUNT = 2,
    FIRST_ROW_TEXT_TOP_INSET    = 4,
    ROW_TEXT_TOP_INSET          = 2,
    LIST_EDGE_ROW_COUNT         = 2,
    SCROLL_TRACK_EDGE_ROW_COUNT = 2,
    SCROLL_THUMB_X_INSET        = 5,
    SCROLL_THUMB_Y_INSET        = 3,
    SCROLL_THUMB_TRAVEL_PADDING = 7,
    SCROLL_THUMB_CENTER_DIVISOR = 2,
    SCROLL_DRAG_Y_ADJUSTMENT    = 4
H2_ENUM_END(DropListLayoutConstant)

VA(0x004cfdb0, 0x54)
dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_itemCount = 0;
    m_items = NULL;
    m_selectedIndex = -1;
    m_savedBackground = NULL;
}

VA(0x004cfe40, 0xfb)
dropListWidget::~dropListWidget() {
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != NULL)
        delete m_savedBackground;
    for (i32 itemIndex = 0; itemIndex < m_itemCount; itemIndex++)
        H2_FREE(m_items[itemIndex]);
    H2_FREE(m_items);
}

VA(0x004cff40, 0x2f8)
void dropListWidget::Read(void) {
    IconEntry* entry;
    i8 name[RESOURCE_NAME_CAPACITY];

    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(name);
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(reinterpret_cast<char*>(name));
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(name);
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(reinterpret_cast<char*>(name));
    gpResourceManager->RestorePosition();
    m_contentX = m_x + gpResourceManager->ReadWord();
    m_contentY = m_y + gpResourceManager->ReadWord();
    m_contentWidth = gpResourceManager->ReadWord();
    m_contentHeight = gpResourceManager->ReadWord();
    m_maxVisibleItems = gpResourceManager->ReadWord();
    m_normalColor = static_cast<FontDrawMode>(gpResourceManager->ReadWord());
    m_selColor = static_cast<FontDrawMode>(gpResourceManager->ReadWord());
    m_unusedColor = static_cast<FontDrawMode>(gpResourceManager->ReadWord());
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();

    m_closedContentFrame = IDX(FRAME_CLOSED_CONTENT);
    m_dropButtonFrame = IDX(FRAME_DROP_BUTTON);
    m_dropButtonPressedFrame = IDX(FRAME_DROP_BUTTON_PRESSED);
    m_firstRowFrame = IDX(FRAME_FIRST_ROW);
    m_middleRowFrame = IDX(FRAME_MIDDLE_ROW);
    m_lastRowFrame = IDX(FRAME_LAST_ROW);
    m_scrollUpFrame = IDX(FRAME_SCROLL_UP);
    m_scrollUpPressedFrame = IDX(FRAME_SCROLL_UP_PRESSED);
    m_scrollDownFrame = IDX(FRAME_SCROLL_DOWN);
    m_scrollDownPressedFrame = IDX(FRAME_SCROLL_DOWN_PRESSED);
    m_scrollTrackFirstFrame = IDX(FRAME_SCROLL_TRACK_FIRST);
    m_scrollTrackMiddleFrame = IDX(FRAME_SCROLL_TRACK_MIDDLE);
    m_scrollTrackLastFrame = IDX(FRAME_SCROLL_TRACK_LAST);
    m_scrollThumbFrame = IDX(FRAME_SCROLL_THUMB);

    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_closedContentFrame;
    m_iconX = m_x;
    m_iconY = m_y;
    m_closedContentWidth = entry->w;
    m_closedContentHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_dropButtonFrame;
    m_dropButtonX = m_iconX + m_closedContentWidth;
    m_dropButtonY = m_y;
    m_dropButtonWidth = entry->w;
    m_dropButtonHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollThumbFrame;
    m_scrollThumbWidth = entry->w;
    m_scrollThumbHeight = entry->h;
}

VA(0x004d0240, 0x139)
void dropListWidget::DeleteItem(i32 index) {
    if (m_itemCount > index) {
        if (m_selectedIndex == index)
            m_selectedIndex = -1;
        if (m_itemCount == 1) {
            H2_FREE(m_items[0]);
            H2_FREE(m_items);
            m_items = NULL;
        } else {
            char** newItems = static_cast<char**>(H2_ALLOC((m_itemCount - 1) * sizeof(*m_items)));
            memcpy(newItems, m_items, (m_itemCount - 1) * sizeof(*m_items));
            if (m_itemCount - index - 1 > 0)
                memcpy(
                    newItems + index,
                    m_items + index + 1,
                    (m_itemCount - index - 1) * sizeof(*m_items)
                );
            if (m_items != NULL)
                H2_FREE(m_items);
            m_items = newItems;
        }
        m_itemCount--;
    }
}

VA(0x004d0380, 0x42f)
MessageDispatchResult dropListWidget::Main(tag_message& message) {
    char* text;
    char** newItems;

    if (!HAS(m_flags, WIDGET_FLAG_ENABLED)) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_CLEAR_ITEMS:
                    if (message.payload.widget.id != m_id)
                        break;
                    while (m_itemCount != 0)
                        DeleteItem(0);
                    break;

                case WIDGET_COMMAND_DELETE_ITEM:
                    if (message.payload.widget.id != m_id)
                        break;
                    DeleteItem(message.payload.widget.data.value);
                    break;

                case WIDGET_COMMAND_REPLACE_ITEM:
                    if (message.payload.widget.id != m_id)
                        break;
                    text = message.payload.widget.data.text;
                    if (m_itemCount > message.payload.widget.parameter) {
                        H2_FREE(m_items[message.payload.widget.parameter]);
                        m_items[message.payload.widget.parameter] =
                            static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                        strcpy(m_items[message.payload.widget.parameter], text);
                    }
                    break;

                case WIDGET_COMMAND_APPEND_ITEM:
                    if (message.payload.widget.id != m_id)
                        break;
                    text = message.payload.widget.data.text;
                    newItems =
                        static_cast<char**>(H2_ALLOC((m_itemCount + 1) * sizeof(*m_items)));
                    if (m_itemCount != 0)
                        memcpy(newItems, m_items, m_itemCount * sizeof(*m_items));
                    newItems[m_itemCount] = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                    strcpy(newItems[m_itemCount], text);
                    m_itemCount++;
                    if (m_items != NULL)
                        H2_FREE(m_items);
                    m_items = newItems;
                    break;

                case WIDGET_COMMAND_SET_SELECTION:
                    if (message.payload.widget.id != m_id)
                        break;
                    m_selectedIndex = message.payload.widget.data.value;
                    return MESSAGE_DISPATCH_CONSUME;

                case WIDGET_COMMAND_GET_SELECTION:
                    if (message.payload.widget.id != m_id)
                        break;
                    message.payload.widget.data.value = m_selectedIndex;
                    return MESSAGE_DISPATCH_CONSUME;
            }
            break;

        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN: {
            if (!HAS(m_flags, WIDGET_FLAG_DRAW))
                break;
            i16 x = message.payload.mouse.x - m_owner->m_posX;
            i16 y = message.payload.mouse.y - m_owner->m_posY;
            if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                    message.payload.widget.id = m_id;
                    message.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            } else {
                if (x >= m_dropButtonX && y >= m_dropButtonY
                    && x < m_dropButtonX + m_dropButtonWidth
                    && y < m_dropButtonY + m_dropButtonHeight) {
                    ProcessSelectDialog();
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.command = WIDGET_COMMAND_SELECT;
                    message.payload.widget.id = m_id;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            }
        }
    }
    return widget::Main(message);
}

VA(0x004d07b0, 0x11c)
void dropListWidget::Draw(void) {
    m_icon->DrawToBuffer(
        m_owner->m_posX + m_iconX,
        m_owner->m_posY + m_iconY,
        m_closedContentFrame,
        ICON_DRAW_NORMAL
    );
    m_icon->DrawToBuffer(
        m_owner->m_posX + m_dropButtonX,
        m_owner->m_posY + m_dropButtonY,
        m_dropButtonFrame,
        ICON_DRAW_NORMAL
    );
    if (m_itemCount > 0 && m_selectedIndex >= 0)
        m_font->DrawBoundedString(
            m_items[m_selectedIndex],
            m_owner->m_posX + m_contentX,
            m_owner->m_posY + m_contentY,
            m_contentWidth,
            m_contentHeight,
            HAS(m_flags, WIDGET_FLAG_DIMMED) ? FONT_DRAW_DIMMED : m_normalColor,
            m_alignment
        );
}

VA(0x004d08d0, 0x58f)
void dropListWidget::DrawDropStuff(void) {
    i32 y = m_owner->m_posY + m_listY;
    i32 i;

    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_firstRowFrame, ICON_DRAW_NORMAL);
    m_font->DrawBoundedString(
        m_items[m_topIndex],
        m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
        y + FIRST_ROW_TEXT_TOP_INSET,
        m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
        m_font->m_height + 1,
        m_topIndex == m_selectedIndex ? m_selColor : m_normalColor,
        m_alignment
    );
    y += m_firstRowHeight;
    for (i = 1; i < m_visibleItemCount - 1; i++) {
        if (m_topIndex + i >= m_itemCount)
            break;
        m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_middleRowFrame, ICON_DRAW_NORMAL);
        m_font->DrawBoundedString(
            *(m_items + m_topIndex + i),
            m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
            y + ROW_TEXT_TOP_INSET,
            m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
            m_font->m_height + 1,
            m_topIndex + i == m_selectedIndex ? m_selColor : m_normalColor,
            m_alignment
        );
        y += m_middleRowHeight;
    }
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_lastRowFrame, ICON_DRAW_NORMAL);
    if (m_topIndex + i < m_itemCount)
        m_font->DrawBoundedString(
            *(m_items + m_topIndex + i),
            m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
            y + ROW_TEXT_TOP_INSET,
            m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
            m_font->m_height + 1,
            m_topIndex + i == m_selectedIndex ? m_selColor : m_normalColor,
            m_alignment
        );
    if (m_scrollRange > 0) {
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollUpX,
            m_owner->m_posY + m_scrollUpY,
            m_scrollUpPressed ? m_scrollUpPressedFrame : m_scrollUpFrame,
            ICON_DRAW_NORMAL
        );
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY,
            m_scrollTrackFirstFrame,
            ICON_DRAW_NORMAL
        );
        for (i = SCROLL_TRACK_EDGE_ROW_COUNT;
             i < m_visibleItemCount - SCROLL_TRACK_EDGE_ROW_COUNT;
             i++)
            m_icon->DrawToBuffer(
                m_owner->m_posX + m_scrollTrackX,
                m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
                m_scrollTrackMiddleFrame,
                ICON_DRAW_NORMAL
            );
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
            m_scrollTrackLastFrame,
            ICON_DRAW_NORMAL
        );
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollDownX,
            m_owner->m_posY + m_scrollDownY,
            m_scrollDownPressed ? m_scrollDownPressedFrame : m_scrollDownFrame,
            ICON_DRAW_NORMAL
        );
        m_scrollThumbX = m_owner->m_posX + m_scrollTrackX + SCROLL_THUMB_X_INSET;
        m_scrollThumbY = m_owner->m_posY + m_scrollTrackY
                         + m_scrollThumbTravel * m_topIndex / m_scrollRange
                         + SCROLL_THUMB_Y_INSET;
        m_icon->DrawToBuffer(m_scrollThumbX, m_scrollThumbY, m_scrollThumbFrame, ICON_DRAW_NORMAL);
    }
    gpWindowManager->UpdateScreenRegion(m_x, m_y, m_width, m_height + m_savedBackgroundHeight);
}

VA(0x004d0e60, 0xa6)
void dropListWidget::SaveDropBackground(void) {
    m_savedBackground =
        new bitmap(BITMAP_TYPE_NONE, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);
}

VA(0x004d0f10, 0x96)
void dropListWidget::RestoreDropBackground(void) {
    m_savedBackground->DrawToBuffer(m_savedBackgroundX, m_savedBackgroundY);
    gpWindowManager->UpdateScreenRegion(
        m_savedBackgroundX,
        m_savedBackgroundY,
        m_savedBackgroundWidth,
        m_savedBackgroundHeight
    );
    delete m_savedBackground;
    m_savedBackground = NULL;
}

VA(0x004d0fb0, 0xcb2)
void dropListWidget::ProcessSelectDialog(void) {
    i32 offset;
    i32 x;
    tag_message message;
    i32 itemIndex;
    IconEntry* entry;
    i32 y;
    b32 firstRelease = true;

    m_scrollUpPressed = 0;
    m_scrollDownPressed = 0;
    m_itemSelectionTracking = 0;
    m_scrollThumbDragging = 0;
    m_topIndex = 0;
    m_scrollRange = 0;
    if (m_itemCount > m_maxVisibleItems) {
        m_scrollRange = m_itemCount - m_maxVisibleItems;
        m_topIndex = m_selectedIndex;
        if (m_topIndex < 0)
            m_topIndex = 0;
        if (m_topIndex > m_scrollRange)
            m_topIndex = m_scrollRange;
    }
    m_visibleItemCount = m_scrollRange > 0 ? m_maxVisibleItems
                                           : (m_itemCount > MIN_VISIBLE_ITEM_COUNT
                                                  ? m_itemCount
                                                  : MIN_VISIBLE_ITEM_COUNT);
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_firstRowFrame;
    m_firstRowHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_middleRowFrame;
    m_middleRowHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_lastRowFrame;
    m_lastRowHeight = entry->h;
    m_listX = m_iconX;
    m_listY = m_iconY + m_closedContentHeight;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_firstRowFrame;
    m_listWidth = entry->w;
    m_listHeight = (m_visibleItemCount - LIST_EDGE_ROW_COUNT) * m_middleRowHeight
                   + m_firstRowHeight + m_lastRowHeight;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollUpFrame;
    m_scrollUpWidth = entry->w;
    m_scrollUpHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollDownFrame;
    m_scrollDownWidth = entry->w;
    m_scrollDownHeight = entry->h;
    m_savedBackgroundX = m_listX;
    m_savedBackgroundY = m_listY;
    m_savedBackgroundWidth = m_scrollRange > 0 ? m_listWidth + m_scrollUpWidth : m_listWidth;
    m_savedBackgroundHeight = m_listHeight;
    if (m_scrollRange > 0) {
        m_scrollUpX = m_x + m_width - 1 - (m_scrollUpWidth - 1);
        m_scrollUpY = m_savedBackgroundY;
        m_scrollDownX = m_scrollUpX;
        m_scrollDownY = m_savedBackgroundY + m_savedBackgroundHeight - 1 - m_scrollDownHeight + 1;
        m_scrollTrackX = m_scrollUpX;
        m_scrollTrackY = m_scrollUpY + m_scrollUpHeight;
        m_scrollTrackWidth = m_scrollDownWidth;
        m_scrollTrackHeight = m_scrollDownY - m_scrollTrackY;
        m_scrollThumbTravel =
            m_scrollTrackHeight - m_scrollThumbHeight - SCROLL_THUMB_TRAVEL_PADDING;
    }
    m_icon->DrawToBuffer(
        m_owner->m_posX + m_dropButtonX,
        m_owner->m_posY + m_dropButtonY,
        m_dropButtonPressedFrame,
        ICON_DRAW_NORMAL
    );
    gpWindowManager->UpdateScreenRegion(
        m_owner->m_posX + m_dropButtonX,
        m_owner->m_posY + m_dropButtonY,
        m_dropButtonWidth,
        m_dropButtonHeight
    );
    SaveDropBackground();
    DrawDropStuff();
    while (1) {
        PollSound();
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        x = message.payload.mouse.screenX - m_owner->m_posX;
        y = message.payload.mouse.screenY - m_owner->m_posY;
        switch (message.type) {
            case MESSAGE_LEFT_BUTTON_DOWN:
                if (x < m_savedBackgroundX || y < m_savedBackgroundY
                    || x >= m_savedBackgroundX + m_savedBackgroundWidth
                    || y >= m_savedBackgroundY + m_savedBackgroundHeight)
                    goto closeList;
                if (x >= m_listX && y >= m_listY && x < m_listX + m_listWidth
                    && y < m_listY + m_listHeight) {
                    offset = y - m_listY;
                    if (offset > m_firstRowHeight)
                        itemIndex =
                            m_topIndex + (offset - m_firstRowHeight) / m_middleRowHeight + 1;
                    else
                        itemIndex = m_topIndex;
                    m_itemSelectionTracking = 1;
                    if (itemIndex < m_itemCount && m_selectedIndex != itemIndex) {
                        m_selectedIndex = itemIndex;
                        DrawDropStuff();
                    }
                } else {
                    if (y < m_scrollUpY + m_scrollUpHeight) {
                        if (m_topIndex > 0)
                            m_topIndex--;
                        m_scrollUpPressed = 1;
                    } else if (y >= m_scrollDownY) {
                        if (m_topIndex < m_scrollRange)
                            m_topIndex++;
                        m_scrollDownPressed = 1;
                    } else {
                        if (y >= m_scrollThumbY && y < m_scrollThumbY + m_scrollThumbHeight)
                            m_scrollThumbDragging = 1;
                        m_topIndex = (y - m_scrollTrackY - SCROLL_DRAG_Y_ADJUSTMENT
                                      - m_scrollThumbHeight / SCROLL_THUMB_CENTER_DIVISOR)
                                     * (m_scrollRange + 1) / m_scrollThumbTravel;
                        if (m_topIndex < 0)
                            m_topIndex = 0;
                        if (m_topIndex > m_scrollRange)
                            m_topIndex = m_scrollRange;
                    }
                    DrawDropStuff();
                }
                break;

            case MESSAGE_LEFT_BUTTON_UP:
                if (firstRelease != 0) {
                    firstRelease = false;
                    m_icon->DrawToBuffer(
                        m_owner->m_posX + m_dropButtonX,
                        m_owner->m_posY + m_dropButtonY,
                        m_dropButtonFrame,
                        ICON_DRAW_NORMAL
                    );
                    gpWindowManager->UpdateScreenRegion(
                        m_owner->m_posX + m_dropButtonX,
                        m_owner->m_posY + m_dropButtonY,
                        m_dropButtonWidth,
                        m_dropButtonHeight
                    );
                } else {
                    if (m_itemSelectionTracking)
                        goto closeList;
                    if (m_scrollUpPressed || m_scrollDownPressed || m_scrollThumbDragging) {
                        m_scrollThumbDragging = 0;
                        m_scrollDownPressed = 0;
                        m_scrollUpPressed = 0;
                        DrawDropStuff();
                    }
                }
                break;

            case MESSAGE_MOUSE_MOVE:
                if (m_itemSelectionTracking) {
                    offset = y - m_listY;
                    if (offset > m_firstRowHeight)
                        itemIndex = (offset - m_firstRowHeight) / m_middleRowHeight + 1;
                    else
                        itemIndex = 0;
                    if (itemIndex < 0)
                        itemIndex = 0;
                    if (itemIndex >= m_visibleItemCount)
                        itemIndex = m_visibleItemCount - 1;
                    if (m_topIndex + itemIndex < m_itemCount
                        && m_topIndex + itemIndex != m_selectedIndex) {
                        m_selectedIndex = m_topIndex + itemIndex;
                        DrawDropStuff();
                    }
                } else if (m_scrollThumbDragging) {
                    itemIndex = (y - m_scrollTrackY - SCROLL_DRAG_Y_ADJUSTMENT
                                 - m_scrollThumbHeight / SCROLL_THUMB_CENTER_DIVISOR)
                                * (m_scrollRange + 1) / m_scrollThumbTravel;
                    if (itemIndex < 0)
                        itemIndex = 0;
                    if (itemIndex > m_scrollRange)
                        itemIndex = m_scrollRange;
                    if (m_topIndex != itemIndex) {
                        m_topIndex = itemIndex;
                        DrawDropStuff();
                    }
                }
                break;

            case MESSAGE_KEY_DOWN:
                switch (message.payload.keyboard.keyCode) {
                    case INPUT_SCAN_NUMPAD_8:
                        if (m_selectedIndex > 0)
                            m_selectedIndex--;
                        if (m_selectedIndex > 0 && m_selectedIndex < m_topIndex)
                            m_topIndex = m_selectedIndex;
                        DrawDropStuff();
                        break;

                    case INPUT_SCAN_NUMPAD_2:
                        if (m_selectedIndex < m_itemCount - 1)
                            m_selectedIndex++;
                        if (m_selectedIndex > m_topIndex + m_maxVisibleItems - 1)
                            m_topIndex = m_selectedIndex - (m_maxVisibleItems - 1);
                        DrawDropStuff();
                        break;

                    case INPUT_SCAN_NUMPAD_9:
                        m_topIndex -= m_maxVisibleItems - 1;
                        if (m_topIndex < 0)
                            m_topIndex = 0;
                        m_selectedIndex -= m_maxVisibleItems - 1;
                        if (m_selectedIndex < 0)
                            m_selectedIndex = 0;
                        DrawDropStuff();
                        break;

                    case INPUT_SCAN_NUMPAD_3:
                        m_topIndex += m_maxVisibleItems - 1;
                        if (m_topIndex > m_scrollRange)
                            m_topIndex = m_scrollRange;
                        m_selectedIndex += m_maxVisibleItems - 1;
                        if (m_selectedIndex > m_itemCount - 1)
                            m_selectedIndex = m_itemCount - 1;
                        DrawDropStuff();
                        break;

                    case INPUT_SCAN_NUMPAD_7:
                        m_topIndex = 0;
                        m_selectedIndex = 0;
                        DrawDropStuff();
                        break;

                    case INPUT_SCAN_NUMPAD_1:
                        m_topIndex = m_scrollRange;
                        m_selectedIndex = m_itemCount - 1;
                        DrawDropStuff();
                        break;
                }
                break;
        }
    }

closeList:
    RestoreDropBackground();
    Draw();
    gpWindowManager->UpdateScreenRegion(
        m_owner->m_posX + m_x,
        m_owner->m_posY + m_y,
        m_width,
        m_height
    );
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(dropListWidget, 0x004ea9ec)
