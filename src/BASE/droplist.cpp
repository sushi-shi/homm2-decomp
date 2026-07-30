#include <Ints.h>
#include <BASE/DROPLIST_TYPES.h>
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
#include <PLATFORM/Runtime.h>
#include <string.h>

enum class DropListFrame : i32 {
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
};
using enum DropListFrame;

typedef enum DropListStorageConstant {
    RESOURCE_NAME_CAPACITY   = 16,
    SCROLL_METRIC_SLOT_COUNT = 2
} DropListStorageConstant;

typedef enum DropListLayoutConstant {
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
} DropListLayoutConstant;

static SDropListSourceFiles gDropListSourceFiles = {
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE},
    {DROPLIST_SOURCE_FILE}
};

dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_items = NULL;
    m_savedBackground = NULL;
    m_itemCount = 0;
    m_selectedIndex = -1;
}

dropListWidget::~dropListWidget() {
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != NULL)
        delete m_savedBackground;
    for (i32 itemIndex = 0; itemIndex < m_itemCount; itemIndex++)
        H2_FREE(m_items[itemIndex]);
    H2_FREE(m_items);
}

void dropListWidget::Read(void) {
    i8 name[RESOURCE_NAME_CAPACITY];
    u8** entries;

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
    i16 id = gpResourceManager->ReadWord();

    m_closedContentFrame = H2EnumIndex(FRAME_CLOSED_CONTENT);
    m_dropButtonFrame = H2EnumIndex(FRAME_DROP_BUTTON);
    m_dropButtonPressedFrame = H2EnumIndex(FRAME_DROP_BUTTON_PRESSED);
    m_firstRowFrame = H2EnumIndex(FRAME_FIRST_ROW);
    m_middleRowFrame = H2EnumIndex(FRAME_MIDDLE_ROW);
    m_lastRowFrame = H2EnumIndex(FRAME_LAST_ROW);
    m_scrollUpFrame = H2EnumIndex(FRAME_SCROLL_UP);
    m_scrollUpPressedFrame = H2EnumIndex(FRAME_SCROLL_UP_PRESSED);
    m_scrollDownFrame = H2EnumIndex(FRAME_SCROLL_DOWN);
    m_scrollDownPressedFrame = H2EnumIndex(FRAME_SCROLL_DOWN_PRESSED);
    m_scrollTrackFirstFrame = H2EnumIndex(FRAME_SCROLL_TRACK_FIRST);
    m_scrollTrackMiddleFrame = H2EnumIndex(FRAME_SCROLL_TRACK_MIDDLE);
    m_scrollTrackLastFrame = H2EnumIndex(FRAME_SCROLL_TRACK_LAST);
    m_scrollThumbFrame = H2EnumIndex(FRAME_SCROLL_THUMB);
    m_id = id;
    entries = &m_icon->m_data;
    i16 iconX = m_x;
    i16 iconY = m_y;
    IconEntry* topEntry = reinterpret_cast<IconEntry*>(*entries);
    m_iconX = iconX;
    m_iconY = iconY;
    m_closedContentWidth = topEntry->w;
    m_closedContentHeight = topEntry->h;
    IconEntry* middleEntry = reinterpret_cast<IconEntry*>(*entries) + H2EnumIndex(FRAME_DROP_BUTTON);
    m_dropButtonX = iconX + m_closedContentWidth;
    m_dropButtonY = iconY;
    m_dropButtonWidth = middleEntry->w;
    m_dropButtonHeight = middleEntry->h;
    IconEntry* bottomEntry = reinterpret_cast<IconEntry*>(*entries) + H2EnumIndex(FRAME_SCROLL_THUMB);
    m_scrollThumbWidth = bottomEntry->w;
    m_scrollThumbHeight = bottomEntry->h;
}

void dropListWidget::DeleteItem(i32 index) {
    if (index < m_itemCount) {
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

MessageDispatchResult dropListWidget::Main(tag_message& message) {
    if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_ENABLED)))) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return MESSAGE_DISPATCH_CONTINUE;
    }

    switch (message.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            if ((H2EnumIndex((m_flags) & (WIDGET_FLAG_DRAW)))) {
                i16 x =
                    static_cast<i16>(message.payload.mouse.x) - static_cast<i16>(m_owner->m_posX);
                i16 y =
                    static_cast<i16>(message.payload.mouse.y) - static_cast<i16>(m_owner->m_posY);
                if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                    if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                        message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                        message.type = MESSAGE_WIDGET;
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
                        message.payload.widget.command = WIDGET_COMMAND_SELECT;
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.id = m_id;
                        return MESSAGE_DISPATCH_FORWARD;
                    }
                    return MESSAGE_DISPATCH_CONTINUE;
                }
            }
            break;
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SET_SELECTION:
                    if (m_id == message.payload.widget.id) {
                        m_selectedIndex = static_cast<i16>(message.payload.widget.data.value);
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_GET_SELECTION:
                    if (m_id == message.payload.widget.id) {
                        message.payload.widget.data.value = m_selectedIndex;
                        return MESSAGE_DISPATCH_CONSUME;
                    }
                    break;
                case WIDGET_COMMAND_APPEND_ITEM:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
                        char** newItems = static_cast<char**>(H2_ALLOC((m_itemCount + 1) * sizeof(*m_items)));
                        if (m_itemCount != 0)
                            memcpy(newItems, m_items, m_itemCount * sizeof(*m_items));
                        newItems[m_itemCount] = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                        strcpy(newItems[m_itemCount], text);
                        m_itemCount++;
                        if (m_items != NULL)
                            H2_FREE(m_items);
                        m_items = newItems;
                    }
                    break;
                case WIDGET_COMMAND_REPLACE_ITEM:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
                        if (message.payload.widget.parameter < m_itemCount) {
                            H2_FREE(m_items[message.payload.widget.parameter]);
                            m_items[message.payload.widget.parameter] =
                                static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                            strcpy(m_items[message.payload.widget.parameter], text);
                        }
                    }
                    break;
                case WIDGET_COMMAND_DELETE_ITEM:
                    if (m_id == message.payload.widget.id)
                        DeleteItem(message.payload.widget.data.value);
                    break;
                case WIDGET_COMMAND_CLEAR_ITEMS:
                    if (m_id == message.payload.widget.id) {
                        while (m_itemCount != 0)
                            DeleteItem(0);
                    }
                    break;
            }
            break;
    }
    return widget::Main(message);
}

void dropListWidget::Draw(void) {
    m_icon->DrawToBuffer(
        m_iconX + m_owner->m_posX,
        m_iconY + m_owner->m_posY,
        m_closedContentFrame,
        ICON_DRAW_NORMAL
    );
    m_icon->DrawToBuffer(
        m_dropButtonX + m_owner->m_posX,
        m_dropButtonY + m_owner->m_posY,
        m_dropButtonFrame,
        ICON_DRAW_NORMAL
    );
    if (m_itemCount > 0 && m_selectedIndex >= 0) {
        FontDrawMode color = FONT_DRAW_DIMMED;
        if (!(H2EnumIndex((m_flags) & (WIDGET_FLAG_DIMMED))))
            color = m_normalColor;
        m_font->DrawBoundedString(
            m_items[m_selectedIndex],
            m_contentX + m_owner->m_posX,
            m_contentY + m_owner->m_posY,
            m_contentWidth,
            m_contentHeight,
            color,
            m_alignment
        );
    }
}

void dropListWidget::DrawDropStuff(void) {
    i32 y = m_owner->m_posY + m_listY;
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_firstRowFrame, ICON_DRAW_NORMAL);
    FontDrawMode color = m_selectedIndex == m_topIndex ? m_selColor : m_normalColor;
    m_font->DrawBoundedString(
        m_items[m_topIndex],
        m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
        y + FIRST_ROW_TEXT_TOP_INSET,
        m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
        m_font->m_height + 1,
        color,
        m_alignment
    );
    i32 i = 1;
    y += m_firstRowHeight;
    while (i < m_visibleItemCount - 1 && m_topIndex + i < m_itemCount) {
        m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_middleRowFrame, ICON_DRAW_NORMAL);
        i32 item = m_topIndex + i;
        color = m_selectedIndex == item ? m_selColor : m_normalColor;
        m_font->DrawBoundedString(
            m_items[item],
            m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
            y + ROW_TEXT_TOP_INSET,
            m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
            m_font->m_height + 1,
            color,
            m_alignment
        );
        i++;
        y += m_middleRowHeight;
    }
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_lastRowFrame, ICON_DRAW_NORMAL);
    i32 item = m_topIndex + i;
    if (item < m_itemCount) {
        color = m_selectedIndex == item ? m_selColor : m_normalColor;
        m_font->DrawBoundedString(
            m_items[item],
            m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
            y + ROW_TEXT_TOP_INSET,
            m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
            m_font->m_height + 1,
            color,
            m_alignment
        );
    }
    if (m_scrollRange > 0) {
        i32 frame;
        if (m_scrollUpPressed != 0)
            frame = m_scrollUpPressedFrame;
        else
            frame = m_scrollUpFrame;
        m_icon
            ->DrawToBuffer(m_owner->m_posX + m_scrollUpX, m_owner->m_posY + m_scrollUpY, frame, ICON_DRAW_NORMAL);
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY,
            m_scrollTrackFirstFrame,
            ICON_DRAW_NORMAL
        );
        i = SCROLL_TRACK_EDGE_ROW_COUNT;
        while (i < m_visibleItemCount - SCROLL_TRACK_EDGE_ROW_COUNT) {
            m_icon->DrawToBuffer(
                m_owner->m_posX + m_scrollTrackX,
                m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
                m_scrollTrackMiddleFrame,
                ICON_DRAW_NORMAL
            );
            i++;
        }
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
            m_scrollTrackLastFrame,
            ICON_DRAW_NORMAL
        );
        if (m_scrollDownPressed != 0)
            frame = m_scrollDownPressedFrame;
        else
            frame = m_scrollDownFrame;
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollDownX,
            m_owner->m_posY + m_scrollDownY,
            frame,
            ICON_DRAW_NORMAL
        );
        m_scrollThumbX =
            static_cast<i16>(m_owner->m_posX) + m_scrollTrackX + SCROLL_THUMB_X_INSET;
        m_scrollThumbY = static_cast<i16>(m_owner->m_posY)
                         + (m_scrollThumbTravel * m_topIndex) / m_scrollRange + m_scrollTrackY
                         + SCROLL_THUMB_Y_INSET;
        m_icon->DrawToBuffer(m_scrollThumbX, m_scrollThumbY, m_scrollThumbFrame, ICON_DRAW_NORMAL);
    }
    gpWindowManager->UpdateScreenRegion(m_x, m_y, m_width, m_savedBackgroundHeight + m_height);
}

void dropListWidget::SaveDropBackground(void) {
    m_savedBackground =
        new bitmap(BITMAP_TYPE_NONE, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);
}

void dropListWidget::RestoreDropBackground(void) {
    m_savedBackground->DrawToBuffer(m_savedBackgroundX, m_savedBackgroundY);
    gpWindowManager->UpdateScreenRegion(
        m_savedBackgroundX,
        m_savedBackgroundY,
        m_savedBackgroundWidth,
        m_savedBackgroundHeight
    );
    if (m_savedBackground != NULL)
        delete m_savedBackground;
    m_savedBackground = NULL;
}

void dropListWidget::ProcessSelectDialog(void) {
    i16 scrollWidth[SCROLL_METRIC_SLOT_COUNT];
    i16 scrollTopHeight[SCROLL_METRIC_SLOT_COUNT];
    i16 scrollBottomWidth[SCROLL_METRIC_SLOT_COUNT];
    i16 scrollBottomHeight;
    tag_message message;
    i32 firstRelease = 1;
    i32 ownerX;
    i32 ownerY;
    m_scrollUpPressed = 0;
    m_scrollDownPressed = 0;
    m_itemSelectionTracking = 0;
    m_scrollThumbDragging = 0;
    m_topIndex = 0;
    m_scrollRange = 0;

    i16 numItems = m_itemCount;
    if (numItems > m_maxVisibleItems) {
        m_scrollRange = numItems - m_maxVisibleItems;
        m_topIndex = m_selectedIndex;
        if (m_selectedIndex < 0)
            m_topIndex = 0;
        if (m_scrollRange < m_topIndex)
            m_topIndex = m_scrollRange;
    }
    if (m_scrollRange > 0) {
        m_visibleItemCount = m_maxVisibleItems;
    } else {
        if (numItems <= MIN_VISIBLE_ITEM_COUNT)
            numItems = MIN_VISIBLE_ITEM_COUNT;
        m_visibleItemCount = numItems;
    }

    m_firstRowHeight = m_icon->Entries()[m_firstRowFrame].h;
    m_middleRowHeight = m_icon->Entries()[m_middleRowFrame].h;
    m_lastRowHeight = m_icon->Entries()[m_lastRowFrame].h;
    m_listX = m_iconX;
    m_listY = m_iconY + m_closedContentHeight;
    m_listWidth = m_icon->Entries()[m_firstRowFrame].w;
    m_listHeight =
        (m_visibleItemCount - LIST_EDGE_ROW_COUNT) * m_middleRowHeight + m_firstRowHeight
        + m_lastRowHeight;
    IconEntry* iconEntry = &m_icon->Entries()[m_scrollUpFrame];
    scrollWidth[0] = iconEntry->w;
    m_scrollUpWidth = scrollWidth[0];
    scrollTopHeight[0] = iconEntry->h;
    m_scrollUpHeight = scrollTopHeight[0];
    iconEntry = &m_icon->Entries()[m_scrollDownFrame];
    scrollBottomWidth[0] = iconEntry->w;
    m_scrollDownWidth = scrollBottomWidth[0];
    scrollBottomHeight = iconEntry->h;
    m_scrollDownHeight = scrollBottomHeight;
    m_savedBackgroundX = m_iconX;
    m_savedBackgroundY = m_listY;
    if (m_scrollRange > 0) {
        m_savedBackgroundWidth = scrollWidth[0] + m_listWidth;
    } else {
        m_savedBackgroundWidth = m_listWidth;
    }
    m_savedBackgroundHeight = m_listHeight;

    if (m_scrollRange > 0) {
        i16 scrollX = m_x + m_width - scrollWidth[0];
        m_scrollUpX = scrollX;
        m_scrollUpY = m_listY;
        m_scrollDownX = scrollX;
        i16 bottomY = m_listY - scrollBottomHeight + m_listHeight;
        m_scrollDownY = bottomY;
        m_scrollTrackX = scrollX;
        i16 topY = m_listY + scrollTopHeight[0];
        m_scrollTrackY = topY;
        m_scrollTrackWidth = scrollBottomWidth[0];
        bottomY -= topY;
        m_scrollTrackHeight = bottomY;
        m_scrollThumbTravel = bottomY - m_scrollThumbHeight - SCROLL_THUMB_TRAVEL_PADDING;
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
    m_savedBackground =
        new bitmap(BITMAP_TYPE_NONE, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);

    DrawDropStuff();
    for (;;) {
        PollSound();
        platform::PumpEvents();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        ownerX = m_owner->m_posX;
        ownerY = m_owner->m_posY;
        i32 mouseX = message.payload.mouse.screenX - ownerX;
        i32 mouseY = message.payload.mouse.screenY - ownerY;

        switch (message.type) {
            case MESSAGE_KEY_DOWN:
                switch (message.payload.keyboard.keyCode) {
                    case INPUT_SCAN_NUMPAD_7:
                        m_topIndex = 0;
                        m_selectedIndex = 0;
                        DrawDropStuff();
                        continue;
                    case INPUT_SCAN_NUMPAD_8:
                        if (m_selectedIndex > 0) {
                            m_selectedIndex--;
                        }
                        if (m_selectedIndex > 0 && m_selectedIndex < m_topIndex)
                            m_topIndex = m_selectedIndex;
                        DrawDropStuff();
                        continue;
                    case INPUT_SCAN_NUMPAD_9:
                        m_topIndex = m_topIndex - m_maxVisibleItems + 1;
                        if (m_topIndex < 0)
                            m_topIndex = 0;
                        m_selectedIndex = m_selectedIndex - m_maxVisibleItems + 1;
                        if (m_selectedIndex < 0)
                            m_selectedIndex = 0;
                        DrawDropStuff();
                        continue;
                    case INPUT_SCAN_NUMPAD_1:
                        m_topIndex = m_scrollRange;
                        m_selectedIndex = m_itemCount - 1;
                        DrawDropStuff();
                        continue;
                    case INPUT_SCAN_NUMPAD_2:
                        if (m_selectedIndex < m_itemCount - 1)
                            m_selectedIndex++;
                        if (m_topIndex + m_maxVisibleItems - 1 < m_selectedIndex)
                            m_topIndex = m_selectedIndex - m_maxVisibleItems + 1;
                        DrawDropStuff();
                        continue;
                    case INPUT_SCAN_NUMPAD_3:
                        m_topIndex = m_topIndex - 1 + m_maxVisibleItems;
                        if (m_scrollRange < m_topIndex)
                            m_topIndex = m_scrollRange;
                        m_selectedIndex = m_selectedIndex - 1 + m_maxVisibleItems;
                        if (m_itemCount - 1 < m_selectedIndex)
                            m_selectedIndex = m_itemCount - 1;
                        DrawDropStuff();
                        continue;
                }
                break;

            case MESSAGE_MOUSE_MOVE:
                if (m_itemSelectionTracking != 0) {
                    i32 item;
                    if (m_firstRowHeight < mouseY - m_listY)
                        item = (mouseY - m_listY - m_firstRowHeight) / m_middleRowHeight + 1;
                    else
                        item = 0;
                    if (item < 0)
                        item = 0;
                    if (item >= m_visibleItemCount)
                        item = m_visibleItemCount - 1;
                    i32 selected = m_topIndex + item;
                    if (selected < m_itemCount && m_selectedIndex != selected) {
                        m_selectedIndex = static_cast<i16>(item) + m_topIndex;
                        DrawDropStuff();
                        continue;
                    }
                } else if (m_scrollThumbDragging != 0) {
                    i32 scrollRange = m_scrollRange;
                    i32 top = ((mouseY - m_scrollThumbHeight / SCROLL_THUMB_CENTER_DIVISOR
                                - m_scrollTrackY - SCROLL_DRAG_Y_ADJUSTMENT)
                               * (scrollRange + 1))
                              / m_scrollThumbTravel;
                    if (top < 0)
                        top = 0;
                    if (top > scrollRange)
                        top = scrollRange;
                    if (m_topIndex != top) {
                        m_topIndex = static_cast<i16>(top);
                        DrawDropStuff();
                        continue;
                    }
                }
                break;

            case MESSAGE_LEFT_BUTTON_DOWN:
                if (mouseX < m_savedBackgroundX || mouseY < m_savedBackgroundY
                    || mouseX >= m_savedBackgroundX + m_savedBackgroundWidth
                    || mouseY >= m_savedBackgroundY + m_savedBackgroundHeight)
                    goto done;
                if (mouseX >= m_listX && mouseY >= m_listY && mouseX < m_listX + m_listWidth
                    && mouseY < m_listY + m_listHeight) {
                    i32 item;
                    if (m_firstRowHeight < mouseY - m_listY)
                        item = m_topIndex + 1
                               + (mouseY - m_listY - m_firstRowHeight) / m_middleRowHeight;
                    else
                        item = m_topIndex;
                    m_itemSelectionTracking = 1;
                    if (item < m_itemCount && m_selectedIndex != item) {
                        m_selectedIndex = static_cast<i16>(item);
                        DrawDropStuff();
                        continue;
                    }
                } else {
                    if (mouseY < m_scrollUpY + m_scrollUpHeight) {
                        if (m_topIndex > 0)
                            m_topIndex--;
                        m_scrollUpPressed = 1;
                    } else if (mouseY >= m_scrollDownY) {
                        if (m_topIndex < m_scrollRange)
                            m_topIndex++;
                        m_scrollDownPressed = 1;
                    } else {
                        if (mouseY >= m_scrollThumbY
                            && mouseY < m_scrollThumbY + m_scrollThumbHeight)
                            m_scrollThumbDragging = 1;
                        i16 scrollRange = m_scrollRange;
                        m_topIndex = static_cast<i16>(
                            ((mouseY - m_scrollThumbHeight / SCROLL_THUMB_CENTER_DIVISOR
                              - m_scrollTrackY - SCROLL_DRAG_Y_ADJUSTMENT)
                             * (scrollRange + 1))
                            / m_scrollThumbTravel
                        );
                        if (m_topIndex < 0)
                            m_topIndex = 0;
                        if (scrollRange < m_topIndex)
                            m_topIndex = scrollRange;
                    }
                    DrawDropStuff();
                    continue;
                }
                break;

            case MESSAGE_LEFT_BUTTON_UP:
                if (firstRelease) {
                    firstRelease = 0;
                    m_icon->DrawToBuffer(
                        m_dropButtonX + ownerX,
                        m_dropButtonY + ownerY,
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
                    if (m_itemSelectionTracking != 0)
                        goto done;
                    if (m_scrollUpPressed != 0 || m_scrollDownPressed != 0
                        || m_scrollThumbDragging != 0) {
                        m_scrollThumbDragging = 0;
                        m_scrollDownPressed = 0;
                        m_scrollUpPressed = 0;
                        DrawDropStuff();
                        continue;
                    }
                }
                break;
        }
    }

done:
    m_savedBackground->DrawToBuffer(m_savedBackgroundX, m_savedBackgroundY);
    gpWindowManager->UpdateScreenRegion(
        m_savedBackgroundX,
        m_savedBackgroundY,
        m_savedBackgroundWidth,
        m_savedBackgroundHeight
    );
    if (m_savedBackground != NULL)
        delete m_savedBackground;
    m_savedBackground = NULL;
    Draw();
    gpWindowManager
        ->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
}
