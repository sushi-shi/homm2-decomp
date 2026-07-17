// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\listbox.obj   from: .\basewin.lib
// functions: 10   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <BASE/listBoxWidget.h>
#include <BASE/LISTBOX_TYPES.h>
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

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\listbox.cpp"
#define LISTBOX_SOURCE_FILE_SEPARATOR "\0\0\0\0"
#define LISTBOX_DESTRUCTOR_SOURCE_FILES                                                            \
    RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE
#define LISTBOX_DELETE_SOURCE_FILES                                                                \
    RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE                          \
        LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR            \
            RETAIL_FILE
#define LISTBOX_MAIN_SOURCE_FILES                                                                  \
    RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE                          \
        LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR            \
            RETAIL_FILE LISTBOX_SOURCE_FILE_SEPARATOR RETAIL_FILE

// @data-layout-note Retail's initialized listbox contribution is
// RVA 0x120e94..0x121078 (0x1e4), with eleven identical source-path views at
// 0x2c-byte intervals. Destructor item/list frees, DeleteItem item/list frees,
// allocation/old-list free, and Main replace free/allocation plus append
// list-allocation/item-allocation/old-list free use those addresses in that
// order. The only rdata owner is the reviewed 0xc listBoxWidget vtable at
// RVA 0x0ebac0; its 0x10 retail contribution includes four bytes of natural
// tail alignment. This TU has no loader-zero contribution. The former source
// pooled all eleven uses into one 0x29 COMDAT. A typed 0x1e4 aggregate
// reproduced every byte and addend but emitted an align-eight writable section
// at the align-four-only retail start, so it cannot represent the retail object.
// The retained align-four COMDAT banks contain 2, 4, and 5 views and have
// logical sizes 0x55, 0xad, and 0xd9. Their natural three-byte alignment tails
// exactly cover the retail interval. Do not pool the paths or add padding
// symbols, aliases, fake owners, section pragmas, or per-TU flag exceptions.

VA(0x004db060, 0x42)
listBoxWidget::listBoxWidget(void) : widget(0, 0, 0, 0, 0, 0) {
    m_items = 0;
    m_scrollbar = 0;
    m_selectedIndex = -1;
    m_itemCount = 0;
    m_lastSelectedIndex = -1;
}

// @semantic
// Complete 0x86-byte destructor with all 11 relocation sites and owner/addends aligned.
// The only raw-code residual is the item loop: retail uses EBX for the four-byte item
// offset and EDI for the item index, while candidate assigns those equivalent induction
// values to EDI and EBX. Semantic item naming, a source-level item temporary, and an
// explicit preincrement loop did not improve the retained form; the latter two worsened
// scheduling. Revisit only after a genuine declaration or combined-TU state change.
VA(0x004db0d0, 0x86)
listBoxWidget::~listBoxWidget() {
    i32 i;
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_scrollbar != 0)
        delete m_scrollbar;
    for (i = 0; i < m_itemCount; i++)
#line 25
        H2_FREE_AT(m_items[i], LISTBOX_DESTRUCTOR_SOURCE_FILES, 25);
#line 27
    H2_FREE_AT(m_items, LISTBOX_DESTRUCTOR_SOURCE_FILES + LISTBOX_DESTRUCTOR_LIST_SOURCE_FILE_OFFSET, 27);
    gbSendMouseMoveMessages = 0;
}

VA(0x004db160, 0x26e)
void listBoxWidget::Read(void) {
    i16 frameHeight[2];
    i8 iconName[16];
    IconEntry* iconEntry;
    i16 bottomY;
    i16 topY;
    i16 rightX;
    i16 firstRowHeight;
    i16 listX;
    i16 rowHeight;
    i16 listY;
    i16 lastRowHeight;
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(iconName);
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(reinterpret_cast<char*>(iconName));
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(iconName);
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(reinterpret_cast<char*>(iconName));
    gpResourceManager->RestorePosition();
    m_maxVisibleItems = gpResourceManager->ReadWord();
    m_normalColor = gpResourceManager->ReadWord();
    m_selectedColor = gpResourceManager->ReadWord();
    m_textMode = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_firstRowFrame = LISTBOX_FRAME_FIRST_ROW;
    m_middleRowFrame = LISTBOX_FRAME_MIDDLE_ROW;
    m_lastRowFrame = LISTBOX_FRAME_LAST_ROW;
    m_scrollUpFrame = LISTBOX_FRAME_SCROLL_UP;
    m_scrollUpPressedFrame = LISTBOX_FRAME_SCROLL_UP_PRESSED;
    m_scrollDownFrame = LISTBOX_FRAME_SCROLL_DOWN;
    m_scrollDownPressedFrame = LISTBOX_FRAME_SCROLL_DOWN_PRESSED;
    m_scrollTrackFirstFrame = LISTBOX_FRAME_SCROLL_TRACK_FIRST;
    m_scrollTrackMiddleFrame = LISTBOX_FRAME_SCROLL_TRACK_MIDDLE;
    m_scrollTrackLastFrame = LISTBOX_FRAME_SCROLL_TRACK_LAST;
    m_scrollThumbFrame = LISTBOX_FRAME_SCROLL_THUMB;
    iconEntry = &m_icon->Entries()[LISTBOX_FRAME_SCROLL_THUMB];
    m_scrollThumbWidth = iconEntry->w;
    frameHeight[0] = iconEntry->h;
    m_scrollThumbHeight = frameHeight[0];
    firstRowHeight = m_icon->Entries()[LISTBOX_FRAME_FIRST_ROW].h;
    m_firstRowHeight = firstRowHeight;
    listX = m_x;
    rowHeight = m_icon->Entries()[LISTBOX_FRAME_MIDDLE_ROW].h;
    listY = m_y;
    m_rowHeight = rowHeight;
    lastRowHeight = m_icon->Entries()[LISTBOX_FRAME_LAST_ROW].h;
    m_lastRowHeight = lastRowHeight;
    m_listX = listX;
    m_listY = listY;
    m_listWidth = m_icon->Entries()[LISTBOX_FRAME_FIRST_ROW].w;
    m_listHeight = (m_maxVisibleItems - 2) * m_rowHeight + m_firstRowHeight + m_lastRowHeight;
    iconEntry = &m_icon->Entries()[LISTBOX_FRAME_SCROLL_UP];
    m_scrollUpWidth = iconEntry->w;
    m_scrollUpHeight = iconEntry->h;
    iconEntry = &m_icon->Entries()[LISTBOX_FRAME_SCROLL_DOWN];
    m_scrollDownWidth = iconEntry->w;
    m_scrollDownHeight = iconEntry->h;
    rightX = m_width - m_scrollUpWidth + m_x;
    m_scrollUpX = rightX;
    m_scrollUpY = m_y;
    m_scrollDownX = rightX;
    bottomY = m_y + m_height - m_scrollDownHeight;
    m_scrollDownY = bottomY;
    m_scrollTrackX = rightX;
    topY = m_y + m_scrollUpHeight;
    bottomY -= topY;
    m_scrollTrackY = topY;
    m_scrollTrackWidth = m_scrollDownWidth;
    m_scrollTrackHeight = bottomY;
    m_scrollThumbTravel = m_scrollTrackHeight - frameHeight[0] - 7;
    m_scrollUpPressed = 0;
    m_scrollDownPressed = 0;
    m_itemSelectionTracking = 0;
    m_scrollThumbDragging = 0;
    m_topIndex = 0;
    m_scrollRange = 0;
    m_visibleItemCount = 0;
}

VA(0x004db3d0, 0x142)
void listBoxWidget::DeleteItem(i32 index) {
    if (index >= m_itemCount)
        return;
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
#line 156
        H2_FREE_AT(m_items[0], LISTBOX_DELETE_SOURCE_FILES, 0x9c);
        H2_FREE_AT(m_items, LISTBOX_DELETE_SOURCE_FILES + LISTBOX_DELETE_LIST_SOURCE_FILE_OFFSET, 0x9d);
        m_items = 0;
    } else {
#line 162
        char** newItems = static_cast<char**>(H2_ALLOC_AT((m_itemCount - 1) * 4, LISTBOX_DELETE_SOURCE_FILES + LISTBOX_DELETE_ALLOCATION_SOURCE_FILE_OFFSET, 0xa2));
        memcpy(newItems, m_items, (m_itemCount - 1) * 4);
        if (m_itemCount - index - 1 > 0)
            memcpy(&newItems[index], &m_items[index + 1], (m_itemCount - index - 1) * 4);
        if (m_items != 0)
#line 169
            H2_FREE_AT(m_items, LISTBOX_DELETE_SOURCE_FILES + LISTBOX_DELETE_OLD_LIST_SOURCE_FILE_OFFSET, 0xa9);
        m_items = newItems;
    }
    m_itemCount--;
    if (m_scrollRange > 0)
        m_visibleItemCount = m_maxVisibleItems;
    else
        m_visibleItemCount = m_itemCount;
}

// @semantic
// Semantics, types, 0x8 frame, CFG, and the 0x368 CodeView span are complete. All 23
// ordered relocation sites/types and semantic targets align: 16 external identities/addends
// are exact, while the dispatch at +0x12a and six tail-table entries at +0x350..+0x364
// differ only by compiler-local label versus delinked containing-function identity.
// Relocation-masked raw comparison leaves 23 bytes, all in +0x8a..+0xb7: retail and
// candidate assign owner-relative mouse X/Y and list left/top to different registers and use
// equivalent signed compare polarity for the same half-open bounds. Mouse and widget-message
// dispatch, ID gates, set/get/replace/append/delete/clear behavior, and fallback Main call agree.
// A bounded 512-trial exact-only TU-state diagnostic found no closure; no probe was retained.
VA(0x004db520, 0x368)
i32 listBoxWidget::Main(tag_message& message) {
    if (!(m_flags & WIDGET_FLAG_ENABLED)) {
        if (message.type == MESSAGE_WIDGET)
            return widget::Main(message);
        return 0;
    }
    switch (message.type) {
        case MESSAGE_MOUSE_MOVE:
        case MESSAGE_LEFT_BUTTON_UP:
            if (m_flags & WIDGET_FLAG_DRAW)
                return ProcessMouseMessage(message);
            break;
        case MESSAGE_LEFT_BUTTON_DOWN:
        case 0x20: {
            if (!(m_flags & WIDGET_FLAG_DRAW))
                break;
            i16 mx = message.payload.mouse.x - m_owner->m_posX;
            i16 my = message.payload.mouse.y - m_owner->m_posY;
            i16 left = m_x;
            if (left > mx || m_y > my || left + m_width <= mx || m_y + m_height <= my)
                return 0;
            if (message.type == MESSAGE_RIGHT_BUTTON_DOWN) {
                message.payload.widget.command = WIDGET_COMMAND_ALTERNATE_SELECT;
                message.type = MESSAGE_WIDGET;
                message.payload.widget.id = m_id;
                message.payload.widget.parameter = MESSAGE_MODIFIER_RIGHT_BUTTON;
                return 2;
            }
            return ProcessMouseMessage(message);
        }
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case 0x36:
                    if (m_id == message.payload.widget.id) {
                        m_selectedIndex = message.payload.widget.data.value;
                        return 1;
                    }
                    break;
                case 0x37:
                    if (m_id == message.payload.widget.id) {
                        message.payload.widget.data.value = m_selectedIndex;
                        return 1;
                    }
                    break;
                case 0x39:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
                        if (m_itemCount <= message.payload.widget.parameter)
                            break;
#line 222
                        H2_FREE_AT(m_items[message.payload.widget.parameter], LISTBOX_MAIN_SOURCE_FILES, 0xde);
                        m_items[message.payload.widget.parameter] = static_cast<char*>(H2_ALLOC_AT(strlen(text) + 1, LISTBOX_MAIN_SOURCE_FILES
                                + LISTBOX_REPLACE_ALLOCATION_SOURCE_FILE_OFFSET, 0xdf));
                        strcpy(m_items[message.payload.widget.parameter], text);
                    }
                    break;
                case 0x38:
                    if (m_id == message.payload.widget.id) {
                        char* text = message.payload.widget.data.text;
#line 233
                        char** newItems = static_cast<char**>(H2_ALLOC_AT((m_itemCount + 1) * 4, LISTBOX_MAIN_SOURCE_FILES
                                + LISTBOX_APPEND_LIST_ALLOCATION_SOURCE_FILE_OFFSET, 0xe9));
                        if (m_itemCount != 0)
                            memcpy(newItems, m_items, m_itemCount * 4);
#line 236
                        newItems[m_itemCount] = static_cast<char*>(H2_ALLOC_AT(strlen(text) + 1, LISTBOX_MAIN_SOURCE_FILES
                                + LISTBOX_APPEND_ITEM_ALLOCATION_SOURCE_FILE_OFFSET, 0xec));
                        strcpy(newItems[m_itemCount], text);
                        m_itemCount++;
                        if (m_items != 0)
#line 240
                            H2_FREE_AT(m_items, LISTBOX_MAIN_SOURCE_FILES
                                    + LISTBOX_APPEND_OLD_LIST_SOURCE_FILE_OFFSET, 0xf0);
                        m_items = newItems;
                        if (m_maxVisibleItems < m_itemCount) {
                            m_scrollRange = m_itemCount - m_maxVisibleItems;
                            m_topIndex = m_selectedIndex;
                            if (m_topIndex < 0)
                                m_topIndex = 0;
                            if (m_topIndex > m_scrollRange)
                                m_topIndex = m_scrollRange;
                        }
                        if (m_scrollRange > 0)
                            m_visibleItemCount = m_maxVisibleItems;
                        else
                            m_visibleItemCount = m_itemCount;
                    }
                    break;
                case 0x3a:
                    if (m_id == message.payload.widget.id)
                        DeleteItem(message.payload.widget.data.value);
                    break;
                case 0x3b:
                    if (m_id == message.payload.widget.id)
                        while (m_itemCount != 0)
                            DeleteItem(0);
                    break;
            }
            break;
    }
    return widget::Main(message);
}

VA(0x004db890, 0x8)
void listBoxWidget::Draw(void) {
    DrawLBStuff(0);
}

VA(0x004db8a0, 0x334)
void listBoxWidget::DrawLBStuff(i32 doUpdate) {
    i32 y;
    i32 x;
    x = m_listX + m_owner->m_posX;
    y = m_listY + m_owner->m_posY;
    for (i32 i = 0; i < m_maxVisibleItems; i++) {
        if (i == 0) {
            m_icon->DrawToBuffer(x, y, m_firstRowFrame, 0);
            if (i < m_visibleItemCount) {
                i32 color = m_selectedIndex == m_topIndex ? m_selectedColor : m_normalColor;
                m_font->DrawBoundedString(
                    m_items[m_topIndex],
                    x + 5,
                    y + 4,
                    m_listWidth - 10,
                    m_font->m_height + 1,
                    color,
                    m_textMode
                );
            }
            y += m_firstRowHeight;
        } else if (1 == m_maxVisibleItems - i) {
            m_icon->DrawToBuffer(x, y, m_lastRowFrame, 0);
            if (m_visibleItemCount > i) {
                i32 itemIndex = m_topIndex + i;
                i32 color = m_selectedIndex == itemIndex ? m_selectedColor : m_normalColor;
                m_font->DrawBoundedString(
                    m_items[itemIndex],
                    x + 5,
                    y + 2,
                    m_listWidth - 10,
                    m_font->m_height + 1,
                    color,
                    m_textMode
                );
            }
        } else {
            m_icon->DrawToBuffer(x, y, m_middleRowFrame, 0);
            if (i < m_visibleItemCount) {
                i32 itemIndex = m_topIndex + i;
                i32 color = m_selectedIndex == itemIndex ? m_selectedColor : m_normalColor;
                m_font->DrawBoundedString(
                    m_items[itemIndex],
                    x + 5,
                    y + 2,
                    m_listWidth - 10,
                    m_font->m_height + 1,
                    color,
                    m_textMode
                );
            }
            y += m_rowHeight;
        }
    }
    i32 upFrame = m_scrollUpPressed ? m_scrollUpPressedFrame : m_scrollUpFrame;
    m_icon->DrawToBuffer(m_scrollUpX + m_owner->m_posX, m_scrollUpY + m_owner->m_posY, upFrame, 0);
    m_icon->DrawToBuffer(
        m_scrollTrackX + m_owner->m_posX,
        m_scrollTrackY + m_owner->m_posY,
        m_scrollTrackFirstFrame,
        0
    );
    i32 j;
    for (j = 2; j < m_maxVisibleItems - 2; j++)
        m_icon->DrawToBuffer(
            m_scrollTrackX + m_owner->m_posX,
            (j - 1) * m_rowHeight + m_scrollTrackY + m_owner->m_posY,
            m_scrollTrackMiddleFrame,
            0
        );
    m_icon->DrawToBuffer(
        m_scrollTrackX + m_owner->m_posX,
        (j - 1) * m_rowHeight + m_scrollTrackY + m_owner->m_posY,
        m_scrollTrackLastFrame,
        0
    );
    i32 downFrame = m_scrollDownPressed ? m_scrollDownPressedFrame : m_scrollDownFrame;
    m_icon->DrawToBuffer(
        m_scrollDownX + m_owner->m_posX,
        m_scrollDownY + m_owner->m_posY,
        downFrame,
        0
    );
    i16 thumbX = m_owner->m_posX + m_scrollTrackX + 5;
    m_scrollThumbX = thumbX;
    i32 offset;
    if (m_scrollRange > 0)
        offset = m_topIndex * m_scrollThumbTravel / m_scrollRange;
    else
        offset = m_scrollThumbTravel / 2;
    i16 thumbY = offset + m_owner->m_posY + m_scrollTrackY + 3;
    m_scrollThumbY = thumbY;
    m_icon->DrawToBuffer(thumbX, thumbY, m_scrollThumbFrame, 0);
    if (doUpdate)
        gpWindowManager
            ->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY, m_width, m_height);
}

VA(0x004dbbe0, 0x312)
i32 listBoxWidget::ProcessMouseMessage(tag_message& message) {
    i32 mouseX = message.payload.mouse.screenX - m_owner->m_posX;
    i32 mouseY = message.payload.mouse.screenY - m_owner->m_posY;
    i32 adjY = mouseY - m_listY;
    switch (message.type) {
        case MESSAGE_MOUSE_MOVE:
            if (m_itemSelectionTracking) {
                i32 row;
                i16 firstRowHeight = m_firstRowHeight;
                if (adjY > firstRowHeight)
                    row = (adjY - firstRowHeight) / m_rowHeight + 1;
                else
                    row = 0;
                if (row < 0)
                    row = 0;
                if (row >= m_visibleItemCount)
                    row = m_visibleItemCount - 1;
                if (m_topIndex + row == m_selectedIndex)
                    goto done;
                m_selectedIndex = row + m_topIndex;
            } else if (m_scrollThumbDragging) {
                i32 newTop = (mouseY - m_scrollThumbHeight / 2 - m_scrollTrackY - 4)
                             * (m_scrollRange + 1) / m_scrollThumbTravel;
                if (newTop < 0)
                    newTop = 0;
                if (newTop > m_scrollRange)
                    newTop = m_scrollRange;
                if (m_topIndex == newTop)
                    goto done;
                m_topIndex = newTop;
            } else {
                return 0;
            }
            goto redraw;
        case MESSAGE_LEFT_BUTTON_DOWN:
            if (m_itemCount == 0)
                goto done;
            if (m_listX <= mouseX && m_listY <= mouseY && mouseX < m_listX + m_listWidth
                && mouseY < m_listY + m_listHeight) {
                i32 clickedIndex;
                if (adjY > m_firstRowHeight)
                    clickedIndex = m_topIndex + (adjY - m_firstRowHeight) / m_rowHeight + 1;
                else
                    clickedIndex = m_topIndex;
                if (m_itemCount <= clickedIndex)
                    goto done;
                m_itemSelectionTracking = 1;
                gbSendMouseMoveMessages = 1;
                if (m_selectedIndex == clickedIndex)
                    goto done;
                m_selectedIndex = clickedIndex;
            } else if (mouseY < m_scrollUpY + m_scrollUpHeight) {
                if (m_topIndex > 0)
                    m_topIndex--;
                m_scrollUpPressed = 1;
            } else if (mouseY >= m_scrollDownY) {
                if (m_topIndex < m_scrollRange)
                    m_topIndex++;
                m_scrollDownPressed = 1;
            } else {
                if (m_scrollThumbY <= mouseY && mouseY < m_scrollThumbY + m_scrollThumbHeight) {
                    m_scrollThumbDragging = 1;
                    gbSendMouseMoveMessages = 1;
                }
                m_topIndex = (mouseY - m_scrollThumbHeight / 2 - m_scrollTrackY - 4)
                             * (m_scrollRange + 1) / m_scrollThumbTravel;
                if (m_topIndex < 0)
                    m_topIndex = 0;
                if (m_topIndex > m_scrollRange)
                    m_topIndex = m_scrollRange;
            }
            goto redraw;
        case MESSAGE_LEFT_BUTTON_UP:
            gbSendMouseMoveMessages = 0;
            if (m_scrollUpPressed || m_scrollDownPressed || m_scrollThumbDragging) {
                m_scrollThumbDragging = 0;
                m_scrollDownPressed = 0;
                m_scrollUpPressed = 0;
                goto redraw;
            }
            if (m_itemSelectionTracking) {
                m_itemSelectionTracking = 0;
                message.payload.widget.command = WIDGET_COMMAND_SELECT;
                message.type = MESSAGE_WIDGET;
                message.payload.widget.id = m_id;
                i32 selectedIndex = m_selectedIndex;
                message.payload.widget.parameter = 1;
                message.payload.widget.data.value = selectedIndex;
                if (m_lastSelectedIndex == m_selectedIndex) {
                    i32 lastTick = m_lastClickTime;
                    i32 currentTick = KBTickCount();
                    if (lastTick + 0x190 > currentTick)
                        message.payload.widget.parameter = 2;
                }
                m_lastSelectedIndex = m_selectedIndex;
                m_lastClickTime = KBTickCount();
                return 2;
            }
            return 0;
    }
    goto done;
redraw:
    DrawLBStuff(1);
done:
    return 1;
}

// ===== vtable listBoxWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004db890, 0x8)  void listBoxWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004db0b0, 0x1f)  void * listBoxWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004db520, 0x368)  int listBoxWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(listBoxWidget, 0x004ebac0);

#undef RETAIL_FILE
