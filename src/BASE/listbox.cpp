#include <Ints.h>
#include <BASE/listBoxWidget.h>
#include <BASE/bitmap.h>
#include <BASE/resourceManager.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <string.h>
#include <SOURCE/X_GLOBAL.h>

typedef enum ListBoxSourceFileConstant {
    SOURCE_FILE_SLOT_SIZE = 0x2c
} ListBoxSourceFileConstant;

typedef enum ListBoxTiming {
    DOUBLE_CLICK_TICKS = 0x190
} ListBoxTiming;

typedef enum ListBoxDestructorSourceFileOffset {
    DESTRUCTOR_ITEM_SOURCE_FILE_OFFSET = 0,
    DESTRUCTOR_LIST_SOURCE_FILE_OFFSET = SOURCE_FILE_SLOT_SIZE
} ListBoxDestructorSourceFileOffset;

typedef enum ListBoxDeleteSourceFileOffset {
    DELETE_ITEM_SOURCE_FILE_OFFSET       = 0,
    DELETE_LIST_SOURCE_FILE_OFFSET       = SOURCE_FILE_SLOT_SIZE,
    DELETE_ALLOCATION_SOURCE_FILE_OFFSET = 2 * SOURCE_FILE_SLOT_SIZE,
    DELETE_OLD_LIST_SOURCE_FILE_OFFSET   = 3 * SOURCE_FILE_SLOT_SIZE
} ListBoxDeleteSourceFileOffset;

typedef enum ListBoxMainSourceFileOffset {
    REPLACE_ITEM_SOURCE_FILE_OFFSET           = 0,
    REPLACE_ALLOCATION_SOURCE_FILE_OFFSET     = SOURCE_FILE_SLOT_SIZE,
    APPEND_LIST_ALLOCATION_SOURCE_FILE_OFFSET = 2 * SOURCE_FILE_SLOT_SIZE,
    APPEND_ITEM_ALLOCATION_SOURCE_FILE_OFFSET = 3 * SOURCE_FILE_SLOT_SIZE,
    APPEND_OLD_LIST_SOURCE_FILE_OFFSET        = 4 * SOURCE_FILE_SLOT_SIZE
} ListBoxMainSourceFileOffset;

typedef enum ListBoxFrame {
    FRAME_FIRST_ROW           = 0,
    FRAME_MIDDLE_ROW          = 1,
    FRAME_LAST_ROW            = 2,
    FRAME_SCROLL_UP           = 3,
    FRAME_SCROLL_UP_PRESSED   = 4,
    FRAME_SCROLL_DOWN         = 5,
    FRAME_SCROLL_DOWN_PRESSED = 6,
    FRAME_SCROLL_TRACK_FIRST  = 7,
    FRAME_SCROLL_TRACK_MIDDLE = 8,
    FRAME_SCROLL_TRACK_LAST   = 9,
    FRAME_SCROLL_THUMB        = 10
} ListBoxFrame;

typedef enum ListBoxStorageConstant {
    RESOURCE_NAME_CAPACITY = 16
} ListBoxStorageConstant;

typedef enum ListBoxLayoutConstant {
    LIST_EDGE_ROW_COUNT         = 2,
    TEXT_LEFT_INSET             = 5,
    TEXT_HORIZONTAL_INSET_COUNT = 2,
    FIRST_ROW_TEXT_TOP_INSET    = 4,
    ROW_TEXT_TOP_INSET          = 2,
    SCROLL_TRACK_EDGE_ROW_COUNT = 2,
    SCROLL_THUMB_X_INSET        = 5,
    SCROLL_THUMB_Y_INSET        = 3,
    SCROLL_THUMB_TRAVEL_PADDING = 7,
    SCROLL_THUMB_CENTER_DIVISOR = 2,
    SCROLL_DRAG_Y_ADJUSTMENT    = 4
} ListBoxLayoutConstant;

typedef enum ListBoxSelectionClickCount {
    SELECTION_SINGLE_CLICK = 1,
    SELECTION_DOUBLE_CLICK = 2
} ListBoxSelectionClickCount;

listBoxWidget::listBoxWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {
    m_itemCount = 0;
    m_items = NULL;
    m_selectedIndex = -1;
    m_lastSelectedIndex = -1;
    m_scrollbar = NULL;
}

listBoxWidget::~listBoxWidget() {
    i32 i;
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_scrollbar != NULL)
        delete m_scrollbar;
    for (i = 0; i < m_itemCount; i++)
        H2_FREE(m_items[i]);
    H2_FREE(m_items);
    gbSendMouseMoveMessages = false;
}

void listBoxWidget::Read(void) {
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
    m_maxVisibleItems = gpResourceManager->ReadWord();
    m_normalColor = static_cast<FontDrawMode>(gpResourceManager->ReadWord());
    m_selectedColor = static_cast<FontDrawMode>(gpResourceManager->ReadWord());
    m_alignment = static_cast<FontAlignment>(gpResourceManager->ReadWord());
    m_id = gpResourceManager->ReadWord();
    m_firstRowFrame = FRAME_FIRST_ROW;
    m_middleRowFrame = FRAME_MIDDLE_ROW;
    m_lastRowFrame = FRAME_LAST_ROW;
    m_scrollUpFrame = FRAME_SCROLL_UP;
    m_scrollUpPressedFrame = FRAME_SCROLL_UP_PRESSED;
    m_scrollDownFrame = FRAME_SCROLL_DOWN;
    m_scrollDownPressedFrame = FRAME_SCROLL_DOWN_PRESSED;
    m_scrollTrackFirstFrame = FRAME_SCROLL_TRACK_FIRST;
    m_scrollTrackMiddleFrame = FRAME_SCROLL_TRACK_MIDDLE;
    m_scrollTrackLastFrame = FRAME_SCROLL_TRACK_LAST;
    m_scrollThumbFrame = FRAME_SCROLL_THUMB;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollThumbFrame;
    m_scrollThumbWidth = entry->w;
    m_scrollThumbHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_firstRowFrame;
    m_firstRowHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_middleRowFrame;
    m_rowHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_lastRowFrame;
    m_lastRowHeight = entry->h;
    m_listX = m_x;
    m_listY = m_y;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_firstRowFrame;
    m_listWidth = entry->w;
    m_listHeight = (m_maxVisibleItems - LIST_EDGE_ROW_COUNT) * m_rowHeight + m_firstRowHeight
                   + m_lastRowHeight;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollUpFrame;
    m_scrollUpWidth = entry->w;
    m_scrollUpHeight = entry->h;
    entry = reinterpret_cast<IconEntry*>(m_icon->m_data) + m_scrollDownFrame;
    m_scrollDownWidth = entry->w;
    m_scrollDownHeight = entry->h;
    m_scrollUpX = m_x + m_width - 1 - (m_scrollUpWidth - 1);
    m_scrollUpY = m_y;
    m_scrollDownX = m_scrollUpX;
    m_scrollDownY = m_y + m_height - 1 - m_scrollDownHeight + 1;
    m_scrollTrackX = m_scrollUpX;
    m_scrollTrackY = m_scrollUpY + m_scrollUpHeight;
    m_scrollTrackWidth = m_scrollDownWidth;
    m_scrollTrackHeight = m_scrollDownY - m_scrollTrackY;
    m_scrollThumbTravel = m_scrollTrackHeight - m_scrollThumbHeight - SCROLL_THUMB_TRAVEL_PADDING;
    m_scrollUpPressed = 0;
    m_scrollDownPressed = 0;
    m_itemSelectionTracking = 0;
    m_scrollThumbDragging = 0;
    m_topIndex = 0;
    m_scrollRange = 0;
    m_visibleItemCount = 0;
}

void listBoxWidget::DeleteItem(i32 index) {
    if (m_itemCount > index) {
        if (m_selectedIndex == index)
            m_selectedIndex = -1;
        if (m_topIndex == index && m_scrollRange <= m_topIndex)
            m_topIndex--;
        if (--m_scrollRange < 0)
            m_scrollRange = 0;
        if (m_topIndex < 0)
            m_topIndex = 0;
        if (m_topIndex > m_scrollRange)
            m_topIndex = m_scrollRange;
        if (m_itemCount == 1) {
            H2_FREE(m_items[0]);
            H2_FREE(m_items);
            m_items = NULL;
        } else {
            char** newItems = static_cast<char**>(H2_ALLOC((m_itemCount - 1) * sizeof(*m_items)));
            memcpy(newItems, m_items, (m_itemCount - 1) * sizeof(*m_items));
            if (m_itemCount - index - 1 > 0)
                memcpy(
                    &newItems[index],
                    &m_items[index + 1],
                    (m_itemCount - index - 1) * sizeof(*m_items)
                );
            if (m_items != NULL)
                H2_FREE(m_items);
            m_items = newItems;
        }
        m_itemCount--;
        m_visibleItemCount = m_scrollRange > 0 ? m_maxVisibleItems : m_itemCount;
    }
}

MessageDispatchResult listBoxWidget::Main(tag_message& message) {
    i16 x;
    i16 y;
    const char* text;
    char** newItems;

    if (!(((m_flags) & (WIDGET_FLAG_ENABLED)))) {
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
                    newItems = static_cast<char**>(H2_ALLOC((m_itemCount + 1) * sizeof(*m_items)));
                    if (m_itemCount != 0)
                        memcpy(newItems, m_items, m_itemCount * sizeof(*m_items));
                    newItems[m_itemCount] = static_cast<char*>(H2_ALLOC(strlen(text) + 1));
                    strcpy(newItems[m_itemCount], text);
                    m_itemCount++;
                    if (m_items != NULL)
                        H2_FREE(m_items);
                    m_items = newItems;
                    if (m_itemCount > m_maxVisibleItems) {
                        m_scrollRange = m_itemCount - m_maxVisibleItems;
                        m_topIndex = m_selectedIndex;
                        if (m_topIndex < 0)
                            m_topIndex = 0;
                        if (m_topIndex > m_scrollRange)
                            m_topIndex = m_scrollRange;
                    }
                    m_visibleItemCount = m_scrollRange > 0 ? m_maxVisibleItems : m_itemCount;
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
        case MESSAGE_RIGHT_BUTTON_DOWN:
            if (!(((m_flags) & (WIDGET_FLAG_DRAW))))
                break;
            x = message.payload.mouse.x - m_owner->m_posX;
            y = message.payload.mouse.y - m_owner->m_posY;
            if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                    message.payload.widget.id = m_id;
                    message.payload.widget.modifiers = MESSAGE_MODIFIER_RIGHT_BUTTON;
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return ProcessMouseMessage(message);
            }
            return MESSAGE_DISPATCH_CONTINUE;

        case MESSAGE_MOUSE_MOVE:
        case MESSAGE_LEFT_BUTTON_UP:
            if (!(((m_flags) & (WIDGET_FLAG_DRAW))))
                break;
            return ProcessMouseMessage(message);
    }
    return widget::Main(message);
}

void listBoxWidget::Draw(void) {
    DrawLBStuff(0);
}

void listBoxWidget::DrawLBStuff(i32 doUpdate) {
    i32 row;
    i32 x;
    i32 y;

    x = m_owner->m_posX + m_listX;
    y = m_owner->m_posY + m_listY;
    for (row = 0; row < m_maxVisibleItems; row++) {
        if (row == 0) {
            m_icon->DrawToBuffer(x, y, m_firstRowFrame, ICON_DRAW_NORMAL);
            if (row < m_visibleItemCount)
                m_font->DrawBoundedString(
                    m_items[m_topIndex],
                    x + TEXT_LEFT_INSET,
                    y + FIRST_ROW_TEXT_TOP_INSET,
                    m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
                    m_font->m_height + 1,
                    m_topIndex == m_selectedIndex ? m_selectedColor : m_normalColor,
                    m_alignment
                );
            y += m_firstRowHeight;
        } else if (row == m_maxVisibleItems - 1) {
            m_icon->DrawToBuffer(x, y, m_lastRowFrame, ICON_DRAW_NORMAL);
            if (row < m_visibleItemCount)
                m_font->DrawBoundedString(
                    *(m_items + m_topIndex + row),
                    x + TEXT_LEFT_INSET,
                    y + ROW_TEXT_TOP_INSET,
                    m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
                    m_font->m_height + 1,
                    m_topIndex + row == m_selectedIndex ? m_selectedColor : m_normalColor,
                    m_alignment
                );
        } else {
            m_icon->DrawToBuffer(x, y, m_middleRowFrame, ICON_DRAW_NORMAL);
            if (row < m_visibleItemCount)
                m_font->DrawBoundedString(
                    *(m_items + m_topIndex + row),
                    x + TEXT_LEFT_INSET,
                    y + ROW_TEXT_TOP_INSET,
                    m_listWidth - TEXT_HORIZONTAL_INSET_COUNT * TEXT_LEFT_INSET,
                    m_font->m_height + 1,
                    m_topIndex + row == m_selectedIndex ? m_selectedColor : m_normalColor,
                    m_alignment
                );
            y += m_rowHeight;
        }
    }
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
    for (row = SCROLL_TRACK_EDGE_ROW_COUNT; row < m_maxVisibleItems - SCROLL_TRACK_EDGE_ROW_COUNT;
         row++)
        m_icon->DrawToBuffer(
            m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY + m_rowHeight * (row - 1),
            m_scrollTrackMiddleFrame,
            ICON_DRAW_NORMAL
        );
    m_icon->DrawToBuffer(
        m_owner->m_posX + m_scrollTrackX,
        m_owner->m_posY + m_scrollTrackY + m_rowHeight * (row - 1),
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
                     + (m_scrollRange > 0 ? m_scrollThumbTravel * m_topIndex / m_scrollRange
                                          : m_scrollThumbTravel / SCROLL_THUMB_CENTER_DIVISOR)
                     + SCROLL_THUMB_Y_INSET;
    m_icon->DrawToBuffer(m_scrollThumbX, m_scrollThumbY, m_scrollThumbFrame, ICON_DRAW_NORMAL);
    if (doUpdate)
        gpWindowManager
            ->UpdateScreenRegion(m_owner->m_posX + m_x, m_owner->m_posY + m_y, m_width, m_height);
}

MessageDispatchResult listBoxWidget::ProcessMouseMessage(tag_message& message) {
    i32 offset;
    i32 x;
    i32 itemIndex;
    i32 y;

    x = message.payload.mouse.screenX - m_owner->m_posX;
    y = message.payload.mouse.screenY - m_owner->m_posY;
    offset = y - m_listY;
    switch (message.type) {
        case MESSAGE_LEFT_BUTTON_DOWN:
            if (m_itemCount == 0)
                break;
            if (x >= m_listX && y >= m_listY && x < m_listX + m_listWidth
                && y < m_listY + m_listHeight) {
                if (offset > m_firstRowHeight)
                    itemIndex = m_topIndex + (offset - m_firstRowHeight) / m_rowHeight + 1;
                else
                    itemIndex = m_topIndex;
                if (itemIndex >= m_itemCount)
                    break;
                m_itemSelectionTracking = 1;
                gbSendMouseMoveMessages = true;
                if (m_selectedIndex != itemIndex) {
                    m_selectedIndex = itemIndex;
                    DrawLBStuff(1);
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
                    if (y >= m_scrollThumbY && y < m_scrollThumbY + m_scrollThumbHeight) {
                        m_scrollThumbDragging = 1;
                        gbSendMouseMoveMessages = true;
                    }
                    m_topIndex = (y - m_scrollTrackY - SCROLL_DRAG_Y_ADJUSTMENT
                                  - m_scrollThumbHeight / SCROLL_THUMB_CENTER_DIVISOR)
                                 * (m_scrollRange + 1) / m_scrollThumbTravel;
                    if (m_topIndex < 0)
                        m_topIndex = 0;
                    if (m_topIndex > m_scrollRange)
                        m_topIndex = m_scrollRange;
                }
                DrawLBStuff(1);
            }
            break;

        case MESSAGE_LEFT_BUTTON_UP:
            gbSendMouseMoveMessages = false;
            if (m_scrollUpPressed || m_scrollDownPressed || m_scrollThumbDragging) {
                m_scrollThumbDragging = 0;
                m_scrollDownPressed = 0;
                m_scrollUpPressed = 0;
                DrawLBStuff(1);
            } else {
                if (m_itemSelectionTracking) {
                    m_itemSelectionTracking = 0;
                    message.type = MESSAGE_WIDGET;
                    message.payload.widget.command = WIDGET_COMMAND_SELECT;
                    message.payload.widget.id = m_id;
                    message.payload.widget.data.value = m_selectedIndex;
                    message.payload.widget.parameter = SELECTION_SINGLE_CLICK;
                    if (m_selectedIndex == m_lastSelectedIndex) {
                        if (m_lastClickTime + DOUBLE_CLICK_TICKS > KBTickCount())
                            message.payload.widget.parameter = SELECTION_DOUBLE_CLICK;
                    }
                    m_lastSelectedIndex = m_selectedIndex;
                    m_lastClickTime = KBTickCount();
                    return MESSAGE_DISPATCH_FORWARD;
                }
                return MESSAGE_DISPATCH_CONTINUE;
            }
            break;

        case MESSAGE_MOUSE_MOVE:
            if (m_itemSelectionTracking) {
                if (offset > m_firstRowHeight)
                    itemIndex = (offset - m_firstRowHeight) / m_rowHeight + 1;
                else
                    itemIndex = 0;
                if (itemIndex < 0)
                    itemIndex = 0;
                if (itemIndex >= m_visibleItemCount)
                    itemIndex = m_visibleItemCount - 1;
                if (m_topIndex + itemIndex != m_selectedIndex) {
                    m_selectedIndex = m_topIndex + itemIndex;
                    DrawLBStuff(1);
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
                    DrawLBStuff(1);
                }
            } else {
                return MESSAGE_DISPATCH_CONTINUE;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONSUME;
}
