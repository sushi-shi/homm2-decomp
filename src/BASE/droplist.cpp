#include <va.h>
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
    RESOURCE_NAME_CAPACITY   = 16,
    SCROLL_METRIC_SLOT_COUNT = 2
H2_ENUM_END(DropListStorageConstant)

H2_ENUM_BEGIN(DropListLayoutConstant)
    DIMMED_TEXT_COLOR           = 3,
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

DATA(0x00521078) static SDropListSourceFiles gDropListSourceFiles = {
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

VA(0x004dbf00, 0x3b)
dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, 0) {
    m_items = NULL;
    m_savedBackground = NULL;
    m_itemCount = 0;
    m_selectedIndex = -1;
}

VA(0x004dbf60, 0x7c)
dropListWidget::~dropListWidget() {
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != NULL)
        delete m_savedBackground;
    for (i32 itemIndex = 0; itemIndex < m_itemCount; itemIndex++)
        H2_FREE_AT(m_items[itemIndex], gDropListSourceFiles.itemDestruction.text, 25);
    H2_FREE_AT(m_items, gDropListSourceFiles.listDestruction.text, 27);
}

VA(0x004dbfe0, 0x21d)
void dropListWidget::Read(void) {
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
    m_normalColor = gpResourceManager->ReadWord();
    m_selColor = gpResourceManager->ReadWord();
    m_unusedColor = gpResourceManager->ReadWord();
    m_textMode = gpResourceManager->ReadWord();
    i16 id = gpResourceManager->ReadWord();

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
    m_id = id;
    IconEntry* iconEntries = m_icon->Entries();
    i16 iconX = m_x;
    i16 iconY = m_y;
    IconEntry* closedContentEntry = &iconEntries[IDX(FRAME_CLOSED_CONTENT)];
    m_iconX = iconX;
    m_iconY = iconY;
    m_closedContentWidth = closedContentEntry->w;
    m_closedContentHeight = closedContentEntry->h;
    IconEntry* dropButtonEntry = &iconEntries[IDX(FRAME_DROP_BUTTON)];
    m_dropButtonX = iconX + m_closedContentWidth;
    m_dropButtonY = iconY;
    m_dropButtonWidth = dropButtonEntry->w;
    m_dropButtonHeight = dropButtonEntry->h;
    IconEntry* scrollThumbEntry = &iconEntries[IDX(FRAME_SCROLL_THUMB)];
    m_scrollThumbWidth = scrollThumbEntry->w;
    m_scrollThumbHeight = scrollThumbEntry->h;
}

VA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(i32 index) {
    if (index < m_itemCount) {
        if (m_selectedIndex == index)
            m_selectedIndex = -1;
        if (m_itemCount == 1) {
            H2_FREE_AT(m_items[0], gDropListSourceFiles.finalItemDestruction.text, 111);
            H2_FREE_AT(m_items, gDropListSourceFiles.finalListDestruction.text, 112);
            m_items = NULL;
        } else {
            char** newItems = static_cast<char**>(H2_ALLOC_AT(
                (m_itemCount - 1) * sizeof(*m_items),
                gDropListSourceFiles.resizedListAllocation.text,
                117
            ));
            memcpy(newItems, m_items, (m_itemCount - 1) * sizeof(*m_items));
            if (m_itemCount - index - 1 > 0)
                memcpy(
                    newItems + index,
                    m_items + index + 1,
                    (m_itemCount - index - 1) * sizeof(*m_items)
                );
            if (m_items != NULL)
                H2_FREE_AT(m_items, gDropListSourceFiles.oldListDestruction.text, 123);
            m_items = newItems;
        }
        m_itemCount--;
    }
}

VA(0x004dc2e0, 0x350)
i32 dropListWidget::Main(tag_message& message) {
    if ((m_flags & WIDGET_FLAG_ENABLED) == 0) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return 0;
    }

    switch (message.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
        case MESSAGE_RIGHT_BUTTON_DOWN:
            if (m_flags & WIDGET_FLAG_DRAW) {
                i16 x =
                    static_cast<i16>(message.payload.mouse.x) - static_cast<i16>(m_owner->m_posX);
                i16 y =
                    static_cast<i16>(message.payload.mouse.y) - static_cast<i16>(m_owner->m_posY);
                if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                    if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                        message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.id = m_id;
                        message.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                        return WIDGET_DISPATCH_FORWARD;
                    }
                    return 0;
                } else {
                    if (x >= m_dropButtonX && y >= m_dropButtonY
                        && x < m_dropButtonX + m_dropButtonWidth
                        && y < m_dropButtonY + m_dropButtonHeight) {
                        ProcessSelectDialog();
                        message.payload.widget.command = WIDGET_COMMAND_SELECT;
                        message.type = MESSAGE_WIDGET;
                        message.payload.widget.id = m_id;
                        return WIDGET_DISPATCH_FORWARD;
                    }
                    return 0;
                }
            }
            break;
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_COMMAND_SET_SELECTION:
                    if (m_id == message.payload.widget.id) {
                        m_selectedIndex = static_cast<i16>(message.payload.widget.data.value);
                        return 1;
                    }
                    break;
                case WIDGET_COMMAND_GET_SELECTION:
                    if (m_id == message.payload.widget.id) {
                        message.payload.widget.data.value = m_selectedIndex;
                        return 1;
                    }
                    break;
                case WIDGET_COMMAND_APPEND_ITEM:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
                        char** newItems = static_cast<char**>(H2_ALLOC_AT(
                            (m_itemCount + 1) * sizeof(*m_items),
                            gDropListSourceFiles.appendedListAllocation.text,
                            184
                        ));
                        if (m_itemCount != 0)
                            memcpy(newItems, m_items, m_itemCount * sizeof(*m_items));
                        newItems[m_itemCount] = static_cast<char*>(H2_ALLOC_AT(
                            strlen(text) + 1,
                            gDropListSourceFiles.appendedTextAllocation.text,
                            187
                        ));
                        strcpy(newItems[m_itemCount], text);
                        m_itemCount++;
                        if (m_items != NULL)
                            H2_FREE_AT(
                                m_items,
                                gDropListSourceFiles.appendedOldListDestruction.text,
                                191
                            );
                        m_items = newItems;
                    }
                    break;
                case WIDGET_COMMAND_REPLACE_ITEM:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
                        if (message.payload.widget.parameter < m_itemCount) {
                            H2_FREE_AT(
                                m_items[message.payload.widget.parameter],
                                gDropListSourceFiles.replacedItemDestruction.text,
                                173
                            );
                            m_items[message.payload.widget.parameter] =
                                static_cast<char*>(H2_ALLOC_AT(
                                    strlen(text) + 1,
                                    gDropListSourceFiles.replacementItemAllocation.text,
                                    174
                                ));
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

VA(0x004dc630, 0xaf)
void dropListWidget::Draw(void) {
    m_icon->DrawToBuffer(
        m_iconX + m_owner->m_posX,
        m_iconY + m_owner->m_posY,
        m_closedContentFrame,
        0
    );
    m_icon->DrawToBuffer(
        m_dropButtonX + m_owner->m_posX,
        m_dropButtonY + m_owner->m_posY,
        m_dropButtonFrame,
        0
    );
    if (m_itemCount > 0 && m_selectedIndex >= 0) {
        i32 color = DIMMED_TEXT_COLOR;
        if ((m_flags & WIDGET_FLAG_DIMMED) == 0)
            color = m_normalColor;
        m_font->DrawBoundedString(
            m_items[m_selectedIndex],
            m_contentX + m_owner->m_posX,
            m_contentY + m_owner->m_posY,
            m_contentWidth,
            m_contentHeight,
            color,
            m_textMode
        );
    }
}

VA(0x004dc6e0, 0x378)
void dropListWidget::DrawDropStuff(void) {
    i32 y = m_owner->m_posY + m_listY;
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_firstRowFrame, 0);
    i32 color = m_selectedIndex == m_topIndex ? m_selColor : m_normalColor;
    m_font->DrawBoundedString(
        m_items[m_topIndex],
        m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
        y + FIRST_ROW_TEXT_TOP_INSET,
        m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
        m_font->m_height + 1,
        color,
        m_textMode
    );
    i32 i = 1;
    y += m_firstRowHeight;
    while (i < m_visibleItemCount - 1 && m_topIndex + i < m_itemCount) {
        m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_middleRowFrame, 0);
        i32 item = m_topIndex + i;
        color = m_selectedIndex == item ? m_selColor : m_normalColor;
        m_font->DrawBoundedString(
            m_items[item],
            m_owner->m_posX + m_listX + TEXT_LEFT_INSET,
            y + ROW_TEXT_TOP_INSET,
            m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
            m_font->m_height + 1,
            color,
            m_textMode
        );
        i++;
        y += m_middleRowHeight;
    }
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_lastRowFrame, 0);
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
            m_textMode
        );
    }
    if (m_scrollRange > 0) {
        i32 frame;
        if (m_scrollUpPressed != 0)
            frame = m_scrollUpPressedFrame;
        else
            frame = m_scrollUpFrame;
        m_icon
            ->DrawToBuffer(m_owner->m_posX + m_scrollUpX, m_owner->m_posY + m_scrollUpY, frame, 0);
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY,
            m_scrollTrackFirstFrame,
            0
        );
        i = SCROLL_TRACK_EDGE_ROW_COUNT;
        while (i < m_visibleItemCount - SCROLL_TRACK_EDGE_ROW_COUNT) {
            m_icon->DrawToBuffer(
                m_owner->m_posX + m_scrollTrackX,
                m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
                m_scrollTrackMiddleFrame,
                0
            );
            i++;
        }
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1),
            m_scrollTrackLastFrame,
            0
        );
        if (m_scrollDownPressed != 0)
            frame = m_scrollDownPressedFrame;
        else
            frame = m_scrollDownFrame;
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollDownX,
            m_owner->m_posY + m_scrollDownY,
            frame,
            0
        );
        m_scrollThumbX =
            static_cast<i16>(m_owner->m_posX) + m_scrollTrackX + SCROLL_THUMB_X_INSET;
        m_scrollThumbY = static_cast<i16>(m_owner->m_posY)
                         + (m_scrollThumbTravel * m_topIndex) / m_scrollRange + m_scrollTrackY
                         + SCROLL_THUMB_Y_INSET;
        m_icon->DrawToBuffer(m_scrollThumbX, m_scrollThumbY, m_scrollThumbFrame, 0);
    }
    gpWindowManager->UpdateScreenRegion(m_x, m_y, m_width, m_savedBackgroundHeight + m_height);
}

VA(0x004dca60, 0x50)
void dropListWidget::SaveDropBackground(void) {
    m_savedBackground =
        new bitmap(BITMAP_TYPE_NONE, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);
}

VA(0x004dcab0, 0x56)
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

VA(0x004dcb10, 0x81f)
void dropListWidget::ProcessSelectDialog(void) {
    IconEntry* iconEntry;
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

    i16 topHeight = m_icon->Entries()[m_firstRowFrame].h;
    m_firstRowHeight = topHeight;
    i16 middleHeight = m_icon->Entries()[m_middleRowFrame].h;
    m_middleRowHeight = middleHeight;
    i16 bottomHeight = m_icon->Entries()[m_lastRowFrame].h;
    m_lastRowHeight = bottomHeight;
    m_listX = m_iconX;
    m_listY = m_iconY + m_closedContentHeight;
    m_listWidth = m_icon->Entries()[m_firstRowFrame].w;
    m_listHeight =
        (m_visibleItemCount - LIST_EDGE_ROW_COUNT) * m_middleRowHeight + m_firstRowHeight
        + m_lastRowHeight;
    iconEntry = &m_icon->Entries()[m_scrollUpFrame];
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
    m_savedBackgroundWidth = m_scrollRange > 0 ? scrollWidth[0] + m_listWidth : m_listWidth;
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
        0
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
        Process1WindowsMessage();
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
                        0
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


VTBL(dropListWidget, 0x004ebad0);
