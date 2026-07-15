// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\droplist.obj   from: .\basewin.lib
// functions: 12   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
VA(0x004dbf00, 0x3b)
dropListWidget::dropListWidget(void) : widget(0, 0, 0, 0, 0, 0)
{
    m_items = 0;
    m_savedBackground = 0;
    m_itemCount = 0;
    m_selectedIndex = -1;
}

VA(0x004dbf60, 0x7c)
dropListWidget::~dropListWidget()
{
    gpResourceManager->Dispose(m_font);
    gpResourceManager->Dispose(m_icon);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    for (int itemIndex = 0; itemIndex < m_itemCount; itemIndex++)
        H2_FREE(m_items[itemIndex], "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 25);
    H2_FREE(m_items, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 27);
}

VA(0x004dbfe0, 0x21d)
void dropListWidget::Read(void)
{
    signed char name[16];
    char **entries;

    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    gpResourceManager->Read13(name);
    gpResourceManager->SavePosition();
    m_font = gpResourceManager->GetFont(reinterpret_cast<char *>(name));
    gpResourceManager->RestorePosition();
    gpResourceManager->Read13(name);
    gpResourceManager->SavePosition();
    m_icon = gpResourceManager->GetIcon(reinterpret_cast<char *>(name));
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
    short id = gpResourceManager->ReadWord();

    m_closedContentFrame = 0;
    m_dropButtonFrame = 1;
    m_dropButtonPressedFrame = 2;
    m_firstRowFrame = 3;
    m_middleRowFrame = 4;
    m_lastRowFrame = 5;
    m_scrollUpFrame = 6;
    m_scrollUpPressedFrame = 7;
    m_scrollDownFrame = 8;
    m_scrollDownPressedFrame = 9;
    m_scrollTrackFirstFrame = 10;
    m_scrollTrackMiddleFrame = 11;
    m_scrollTrackLastFrame = 12;
    m_scrollThumbFrame = 13;
    m_id = id;
    entries = &m_icon->m_data;
    short iconX = m_x;
    short iconY = m_y;
    IconEntry *topEntry = reinterpret_cast<IconEntry *>(*entries);
    m_iconX = iconX;
    m_iconY = iconY;
    m_closedContentWidth = topEntry->w;
    m_closedContentHeight = topEntry->h;
    IconEntry *middleEntry = reinterpret_cast<IconEntry *>(*entries) + 1;
    m_dropButtonX = iconX + m_closedContentWidth;
    m_dropButtonY = iconY;
    m_dropButtonWidth = middleEntry->w;
    m_dropButtonHeight = middleEntry->h;
    IconEntry *bottomEntry = reinterpret_cast<IconEntry *>(*entries) + 13;
    m_scrollThumbWidth = bottomEntry->w;
    m_scrollThumbHeight = bottomEntry->h;
}

VA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(int index)
{
    if (index < m_itemCount) {
        if (m_selectedIndex == index)
            m_selectedIndex = -1;
        if (m_itemCount == 1) {
            H2_FREE(m_items[0], "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 111);
            H2_FREE(m_items, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 112);
            m_items = 0;
        } else {
            char **newItems = static_cast<char **>(H2_ALLOC(m_itemCount * 4 - 4, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 117));
            memcpy(newItems, m_items, m_itemCount * 4 - 4);
            if (m_itemCount - index - 1 > 0)
                memcpy(newItems + index, m_items + index + 1, (m_itemCount - index - 1) * 4);
            if (m_items != 0)
                H2_FREE(m_items, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 123);
            m_items = newItems;
        }
        m_itemCount--;
    }
}

// @early-stop
// The complete 0x350-byte range is raw-exact after relocation-union masking, proving
// frame/slots and CFG. All 15 external relocation targets/addends agree. The other seven
// of 22 ordered sites are the dispatch at +0x15b and six-word table at +0x338: MSVC
// emits $L locals, while the delinker rewrites them as Main plus the same local offsets.
VA(0x004dc2e0, 0x350)
int dropListWidget::Main(tag_message &message)
{
    if ((m_flags & 2) == 0) {
        if (message.type == 0x200)
            return widget::Main(message);
        return 0;
    }

    switch (message.type) {
    case 8:
    case 0x20:
        if (m_flags & 4) {
            short x = static_cast<short>(message.payload.mouse.x) - static_cast<short>(m_owner->m_posX);
            short y = static_cast<short>(message.payload.mouse.y) - static_cast<short>(m_owner->m_posY);
            if (message.type == 0x20) {
                if (x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height) {
                    message.payload.widget.command = 14;
                    message.type = 0x200;
                    message.payload.widget.id = m_id;
                    message.payload.widget.parameter = 0x200;
                    return 2;
                }
                return 0;
            } else {
                if (x >= m_dropButtonX && y >= m_dropButtonY &&
                    x < m_dropButtonX + m_dropButtonWidth && y < m_dropButtonY + m_dropButtonHeight) {
                    ProcessSelectDialog();
                    message.payload.widget.command = 12;
                    message.type = 0x200;
                    message.payload.widget.id = m_id;
                    return 2;
                }
                return 0;
            }
        }
        break;
    case 0x200:
        switch (message.payload.widget.command) {
        case 0x36:
            if (m_id == message.payload.widget.id) {
                m_selectedIndex = static_cast<short>(message.payload.widget.data.value);
                return 1;
            }
            break;
        case 0x37:
            if (m_id == message.payload.widget.id) {
                message.payload.widget.data.value = m_selectedIndex;
                return 1;
            }
            break;
        case 0x38:
            if (m_id == message.payload.widget.id) {
                char *text = message.payload.widget.data.text;
                char **newItems = static_cast<char **>(H2_ALLOC(m_itemCount * 4 + 4, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 184));
                if (m_itemCount != 0)
                    memcpy(newItems, m_items, m_itemCount * 4);
                newItems[m_itemCount] = static_cast<char *>(H2_ALLOC(strlen(text) + 1, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 187));
                strcpy(newItems[m_itemCount], text);
                m_itemCount++;
                if (m_items != 0)
                    H2_FREE(m_items, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 191);
                m_items = newItems;
            }
            break;
        case 0x39:
            if (m_id == message.payload.widget.id) {
                char *text = message.payload.widget.data.text;
                if (message.payload.widget.parameter < m_itemCount) {
                    H2_FREE(m_items[message.payload.widget.parameter], "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 173);
                    m_items[message.payload.widget.parameter] = static_cast<char *>(H2_ALLOC(strlen(text) + 1, "I:\\Projects\\Heroes\\Prog\\BASE\\droplist.cpp", 174));
                    strcpy(m_items[message.payload.widget.parameter], text);
                }
            }
            break;
        case 0x3a:
            if (m_id == message.payload.widget.id)
                DeleteItem(message.payload.widget.data.value);
            break;
        case 0x3b:
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
void dropListWidget::Draw(void)
{
    m_icon->DrawToBuffer(m_iconX + m_owner->m_posX, m_iconY + m_owner->m_posY,
                             m_closedContentFrame, 0);
    m_icon->DrawToBuffer(m_dropButtonX + m_owner->m_posX, m_dropButtonY + m_owner->m_posY,
                             m_dropButtonFrame, 0);
    if (m_itemCount > 0 && m_selectedIndex >= 0) {
        int color = 3;
        if ((m_flags & 8) == 0)
            color = m_normalColor;
        m_font->DrawBoundedString(m_items[m_selectedIndex],
                                      m_contentX + m_owner->m_posX,
                                      m_contentY + m_owner->m_posY,
                                      m_contentWidth, m_contentHeight, color, m_textMode);
    }
}

VA(0x004dc6e0, 0x378)
void dropListWidget::DrawDropStuff(void)
{
    int y = m_owner->m_posY + m_listY;
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_firstRowFrame, 0);
    int color = m_selectedIndex == m_topIndex ? m_selColor : m_normalColor;
    m_font->DrawBoundedString(m_items[m_topIndex], m_owner->m_posX + m_listX + 5,
        y + 4, m_listWidth - 10, m_font->m_height + 1, color, m_textMode);
    int i = 1;
    y += m_firstRowHeight;
    while (i < m_visibleItemCount - 1 && m_topIndex + i < m_itemCount) {
        m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_middleRowFrame, 0);
        int item = m_topIndex + i;
        color = m_selectedIndex == item ? m_selColor : m_normalColor;
        m_font->DrawBoundedString(m_items[item], m_owner->m_posX + m_listX + 5,
            y + 2, m_listWidth - 10, m_font->m_height + 1, color, m_textMode);
        i++;
        y += m_middleRowHeight;
    }
    m_icon->DrawToBuffer(m_owner->m_posX + m_listX, y, m_lastRowFrame, 0);
    int item = m_topIndex + i;
    if (item < m_itemCount) {
        color = m_selectedIndex == item ? m_selColor : m_normalColor;
        m_font->DrawBoundedString(m_items[item], m_owner->m_posX + m_listX + 5,
            y + 2, m_listWidth - 10, m_font->m_height + 1, color, m_textMode);
    }
    if (m_scrollRange > 0) {
        int frame;
        if (m_scrollUpPressed != 0)
            frame = m_scrollUpPressedFrame;
        else
            frame = m_scrollUpFrame;
        m_icon->DrawToBuffer(m_owner->m_posX + m_scrollUpX, m_owner->m_posY + m_scrollUpY, frame, 0);
        m_icon->DrawToBuffer(m_owner->m_posX + m_scrollTrackX, m_owner->m_posY + m_scrollTrackY, m_scrollTrackFirstFrame, 0);
        i = 2;
        while (i < m_visibleItemCount - 2) {
            m_icon->DrawToBuffer(m_owner->m_posX + m_scrollTrackX,
                m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1), m_scrollTrackMiddleFrame, 0);
            i++;
        }
        m_icon->DrawToBuffer(m_owner->m_posX + m_scrollTrackX,
            m_owner->m_posY + m_scrollTrackY + m_middleRowHeight * (i - 1), m_scrollTrackLastFrame, 0);
        if (m_scrollDownPressed != 0)
            frame = m_scrollDownPressedFrame;
        else
            frame = m_scrollDownFrame;
        m_icon->DrawToBuffer(m_owner->m_posX + m_scrollDownX, m_owner->m_posY + m_scrollDownY, frame, 0);
        m_scrollThumbX = static_cast<short>(m_owner->m_posX) + m_scrollTrackX + 5;
        m_scrollThumbY = static_cast<short>(m_owner->m_posY) +
            (m_scrollThumbTravel * m_topIndex) / m_scrollRange + m_scrollTrackY + 3;
        m_icon->DrawToBuffer(m_scrollThumbX, m_scrollThumbY, m_scrollThumbFrame, 0);
    }
    gpWindowManager->UpdateScreenRegion(m_x, m_y, m_width, m_savedBackgroundHeight + m_height);
}

VA(0x004dca60, 0x50)
void dropListWidget::SaveDropBackground(void)
{
    m_savedBackground = new bitmap(0, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);
}

VA(0x004dcab0, 0x56)
void dropListWidget::RestoreDropBackground(void)
{
    m_savedBackground->DrawToBuffer(m_savedBackgroundX, m_savedBackgroundY);
    gpWindowManager->UpdateScreenRegion(m_savedBackgroundX, m_savedBackgroundY, m_savedBackgroundWidth, m_savedBackgroundHeight);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    m_savedBackground = 0;
}

// @semantic: complete shared-header/TU-state checkpoint (live 95.256714%;
// retained 99.3675%). Explicit ranges are base 0x827 versus retail 0x81f.
// Retail has a 0x48 frame; base has 0x4c, with every real stack object shifted
// uniformly by four bytes rather than an extra live variable. With the embedded
// tables excluded, both have 76 branches and 17 calls. All 36 relocation targets
// remain in the same order, but base's 24 external sites are shifted +7 and its
// twelve local-table sites +8. The historical 0x81f/0x48 retained state had only
// three scheduling residuals; current tagged-message/shared-header state changed
// the prologue before any event access. Retried after that trigger: plausible
// include orders, exact Save assignment/call, semantic local names, and owner
// scope were byte-identical; narrowing the message scope fell to 95.19%.
// Do not run AST permutations until the retail frame/size state is recovered.
VA(0x004dcb10, 0x81f)
void dropListWidget::ProcessSelectDialog(void)
{
    IconEntry *iconEntry;
    // Retail reserves a four-byte stack object for each 16-bit dimension temporary.
    short scrollWidth[2];
    short scrollTopHeight[2];
    short scrollBottomWidth[2];
    short scrollBottomHeight;
    tag_message message;
    int firstRelease = 1;
    int ownerX;
    int ownerY;
    m_scrollUpPressed = 0;
    m_scrollDownPressed = 0;
    m_itemSelectionTracking = 0;
    m_scrollThumbDragging = 0;
    m_topIndex = 0;
    m_scrollRange = 0;

    short numItems = m_itemCount;
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
        if (numItems <= 3)
            numItems = 3;
        m_visibleItemCount = numItems;
    }

    short topHeight = reinterpret_cast<IconEntry *>(m_icon->m_data)[m_firstRowFrame].h;
    m_firstRowHeight = topHeight;
    short middleHeight = reinterpret_cast<IconEntry *>(m_icon->m_data)[m_middleRowFrame].h;
    m_middleRowHeight = middleHeight;
    short bottomHeight = reinterpret_cast<IconEntry *>(m_icon->m_data)[m_lastRowFrame].h;
    m_lastRowHeight = bottomHeight;
    m_listX = m_iconX;
    m_listY = m_iconY + m_closedContentHeight;
    m_listWidth = reinterpret_cast<IconEntry *>(m_icon->m_data)[m_firstRowFrame].w;
    m_listHeight = (m_visibleItemCount - 2) * m_middleRowHeight + m_firstRowHeight + m_lastRowHeight;
    iconEntry = reinterpret_cast<IconEntry *>(m_icon->m_data) + m_scrollUpFrame;
    scrollWidth[0] = iconEntry->w;
    m_scrollUpWidth = scrollWidth[0];
    scrollTopHeight[0] = iconEntry->h;
    m_scrollUpHeight = scrollTopHeight[0];
    iconEntry = reinterpret_cast<IconEntry *>(m_icon->m_data) + m_scrollDownFrame;
    scrollBottomWidth[0] = iconEntry->w;
    m_scrollDownWidth = scrollBottomWidth[0];
    scrollBottomHeight = iconEntry->h;
    m_scrollDownHeight = scrollBottomHeight;
    m_savedBackgroundX = m_iconX;
    m_savedBackgroundY = m_listY;
    m_savedBackgroundWidth = m_scrollRange > 0 ? scrollWidth[0] + m_listWidth : m_listWidth;
    m_savedBackgroundHeight = m_listHeight;

    if (m_scrollRange > 0) {
        short scrollX = m_x + m_width - scrollWidth[0];
        m_scrollUpX = scrollX;
        m_scrollUpY = m_listY;
        m_scrollDownX = scrollX;
        short bottomY = m_listY - scrollBottomHeight + m_listHeight;
        m_scrollDownY = bottomY;
        m_scrollTrackX = scrollX;
        short topY = m_listY + scrollTopHeight[0];
        m_scrollTrackY = topY;
        m_scrollTrackWidth = scrollBottomWidth[0];
        bottomY -= topY;
        m_scrollTrackHeight = bottomY;
        m_scrollThumbTravel = bottomY - m_scrollThumbHeight - 7;
    }

    m_icon->DrawToBuffer(m_owner->m_posX + m_dropButtonX, m_owner->m_posY + m_dropButtonY,
        m_dropButtonPressedFrame, 0);
    gpWindowManager->UpdateScreenRegion(m_owner->m_posX + m_dropButtonX,
        m_owner->m_posY + m_dropButtonY, m_dropButtonWidth, m_dropButtonHeight);
    m_savedBackground = new bitmap(0, m_savedBackgroundWidth, m_savedBackgroundHeight);
    m_savedBackground->GrabScreen(m_savedBackgroundX, m_savedBackgroundY);

    DrawDropStuff();
    for (;;) {
        PollSound();
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
        gpMouseManager->Main(message);
        ownerX = m_owner->m_posX;
        ownerY = m_owner->m_posY;
        int mouseX = message.payload.mouse.screenX - ownerX;
        int mouseY = message.payload.mouse.screenY - ownerY;

        switch (message.type) {
        case 1:
            switch (message.payload.keyboard.keyCode) {
            case 0x47:
                m_topIndex = 0;
                m_selectedIndex = 0;
                DrawDropStuff();
                continue;
            case 0x48:
                if (m_selectedIndex > 0) {
                    m_selectedIndex--;
                }
                if (m_selectedIndex > 0 && m_selectedIndex < m_topIndex)
                    m_topIndex = m_selectedIndex;
                DrawDropStuff();
                continue;
            case 0x49:
                m_topIndex = m_topIndex - m_maxVisibleItems + 1;
                if (m_topIndex < 0)
                    m_topIndex = 0;
                m_selectedIndex = m_selectedIndex - m_maxVisibleItems + 1;
                if (m_selectedIndex < 0)
                    m_selectedIndex = 0;
                DrawDropStuff();
                continue;
            case 0x4f:
                m_topIndex = m_scrollRange;
                m_selectedIndex = m_itemCount - 1;
                DrawDropStuff();
                continue;
            case 0x50:
                if (m_selectedIndex < m_itemCount - 1)
                    m_selectedIndex++;
                if (m_topIndex + m_maxVisibleItems - 1 < m_selectedIndex)
                    m_topIndex = m_selectedIndex - m_maxVisibleItems + 1;
                DrawDropStuff();
                continue;
            case 0x51:
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

        case 4:
            if (m_itemSelectionTracking != 0) {
                int item;
                if (m_firstRowHeight < mouseY - m_listY)
                    item = (mouseY - m_listY - m_firstRowHeight) / m_middleRowHeight + 1;
                else
                    item = 0;
                if (item < 0)
                    item = 0;
                if (item >= m_visibleItemCount)
                    item = m_visibleItemCount - 1;
                int selected = m_topIndex + item;
                if (selected < m_itemCount && m_selectedIndex != selected) {
                    m_selectedIndex = static_cast<short>(item) + m_topIndex;
                    DrawDropStuff();
                    continue;
                }
            } else if (m_scrollThumbDragging != 0) {
                int scrollRange = m_scrollRange;
                int top = ((mouseY - m_scrollThumbHeight / 2 - m_scrollTrackY - 4) *
                    (scrollRange + 1)) / m_scrollThumbTravel;
                if (top < 0)
                    top = 0;
                if (top > scrollRange)
                    top = scrollRange;
                if (m_topIndex != top) {
                    m_topIndex = static_cast<short>(top);
                    DrawDropStuff();
                    continue;
                }
            }
            break;

        case 8:
            if (mouseX < m_savedBackgroundX || mouseY < m_savedBackgroundY ||
                mouseX >= m_savedBackgroundX + m_savedBackgroundWidth || mouseY >= m_savedBackgroundY + m_savedBackgroundHeight)
                goto done;
            if (mouseX >= m_listX && mouseY >= m_listY &&
                mouseX < m_listX + m_listWidth && mouseY < m_listY + m_listHeight) {
                int item;
                if (m_firstRowHeight < mouseY - m_listY)
                    item = m_topIndex + 1 + (mouseY - m_listY - m_firstRowHeight) / m_middleRowHeight;
                else
                    item = m_topIndex;
                m_itemSelectionTracking = 1;
                if (item < m_itemCount && m_selectedIndex != item) {
                    m_selectedIndex = static_cast<short>(item);
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
                    if (mouseY >= m_scrollThumbY && mouseY < m_scrollThumbY + m_scrollThumbHeight)
                        m_scrollThumbDragging = 1;
                    short scrollRange = m_scrollRange;
                    m_topIndex = static_cast<short>(((mouseY - m_scrollThumbHeight / 2 - m_scrollTrackY - 4) *
                        (scrollRange + 1)) / m_scrollThumbTravel);
                    if (m_topIndex < 0)
                        m_topIndex = 0;
                    if (scrollRange < m_topIndex)
                        m_topIndex = scrollRange;
                }
                DrawDropStuff();
                continue;
            }
            break;

        case 0x10:
            if (firstRelease) {
                firstRelease = 0;
                m_icon->DrawToBuffer(m_dropButtonX + ownerX, m_dropButtonY + ownerY,
                    m_dropButtonFrame, 0);
                gpWindowManager->UpdateScreenRegion(m_owner->m_posX + m_dropButtonX,
                    m_owner->m_posY + m_dropButtonY, m_dropButtonWidth, m_dropButtonHeight);
            } else {
                if (m_itemSelectionTracking != 0)
                    goto done;
                if (m_scrollUpPressed != 0 || m_scrollDownPressed != 0 || m_scrollThumbDragging != 0) {
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
    gpWindowManager->UpdateScreenRegion(m_savedBackgroundX, m_savedBackgroundY,
                                        m_savedBackgroundWidth, m_savedBackgroundHeight);
    if (m_savedBackground != 0)
        delete m_savedBackground;
    m_savedBackground = 0;
    Draw();
    gpWindowManager->UpdateScreenRegion(m_x + m_owner->m_posX, m_y + m_owner->m_posY,
        m_width, m_height);
}


// ===== vtable dropListWidget : public widget  (3 slots) =====
//  [ 0] VA(0x004dc630, 0xaf)  void dropListWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] VA(0x004dbf40, 0x1f)  void * dropListWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] VA(0x004dc2e0, 0x350)  int dropListWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(dropListWidget, 0x004ebad0);
