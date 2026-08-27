#include <va.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <SOURCE/kbwin.h>
#include <BASE/listBoxWidget.h>
#include <BASE/dropListWidget.h>
#include <BASE/textEntryWidget.h>
#include <BASE/textWidget.h>
#include <BASE/iconWidget.h>
#include <BASE/dimmerWidget.h>
#include <BASE/button.h>
#include <BASE/border.h>
#include <BASE/resourceManager.h>
#include <BASE/bitmap.h>
#include <SOURCE/KB.h>
#include <stdlib.h>
#include <string.h>
H2_ENUM_CLASS_BEGIN(WindowWidgetRecordType)
    WIDGET_RECORD_END                   = 0,
    WIDGET_RECORD_BORDER                = 1,
    WIDGET_RECORD_BUTTON                = 2,
    WIDGET_RECORD_TEXT                  = 8,
    WIDGET_RECORD_ICON                  = 0x10,
    WIDGET_RECORD_DIMMER                = 0x40,
    WIDGET_RECORD_TEXT_ENTRY            = 0x100,
    WIDGET_RECORD_TEXT_ENTRY_RECT       = 0x201,
    WIDGET_RECORD_TEXT_ENTRY_MULTILINE  = 0x202,
    WIDGET_RECORD_DROP_LIST             = 0x203,
    WIDGET_RECORD_TEXT_ENTRY_INSET_FIVE = 0x204,
    WIDGET_RECORD_LIST_BOX              = 0x205,
    WIDGET_RECORD_TEXT_ENTRY_INSET_FOUR = 0x206
H2_ENUM_CLASS_END(WindowWidgetRecordType)

H2_ENUM_BEGIN(WindowConstant)
    SCREEN_WIDTH  = 640,
    SCREEN_HEIGHT = 480,
    OPEN_FAILURE  = 3
H2_ENUM_END(WindowConstant)

VA(0x004ba5c0, 0x99)
heroWindow::heroWindow(void) {
    strcpy(name, "Default Construct");
    m_nextWindow = m_prevWindow = NULL;
    m_zOrder = -1;
    m_posX = m_posY = 0;
    m_winWidth = SCREEN_WIDTH;
    m_winHeight = SCREEN_HEIGHT;
    m_winFlags = WINDOW_FLAG_FIXED_LAYER;
    m_winState = WINDOW_STATE_CLOSED;
    m_widgetListTail = m_widgetListHead = NULL;
    m_savedBackground = NULL;
}

VA(0x004ba660, 0x96)
heroWindow::heroWindow(
    i32 x, i32 y, i32 w, i32 h, H2_ENUM_PARAM(WindowFlag, i32) flags
) {
    strcpy(name, "Dynamic Construct");
    m_nextWindow = m_prevWindow = NULL;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = w;
    m_winHeight = h;
    m_winFlags = flags;
    m_winState = WINDOW_STATE_CLOSED;
    m_widgetListTail = m_widgetListHead = NULL;
    m_savedBackground = NULL;
}

VA(0x004ba700, 0x71f)
heroWindow::heroWindow(i32 x, i32 y, H2_CONST char* resourceName) {
    widget* pWidget;
    textWidget* pText;
    textEntryWidget* pTextEnt;
    iconWidget* pIcon;
    button* pButton;
    dropListWidget* pDrop;
    border* pBorder;
    WindowWidgetRecordType type;
    dimmerWidget* pDimmer;
    listBoxWidget* pListBox;
    i32 idx;
    u32l jb;
    strcpy(name, resourceName);
    jb = gpResourceManager->MakeId(resourceName, 1);
    gpResourceManager->PointToFile(jb);
    m_savedBackground = NULL;
    m_nextWindow = m_prevWindow = NULL;
    m_winState = WINDOW_STATE_CLOSED;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = gpResourceManager->ReadWord();
    m_winHeight = gpResourceManager->ReadWord();
    m_winFlags = static_cast<WindowFlag>(gpResourceManager->ReadWord());
    m_winFlags |= WINDOW_FLAG_OWNS_WIDGETS;
    m_widgetListTail = m_widgetListHead = NULL;
    idx = 0;
    while (idx == 0) {
        PollSound();
        type = static_cast<WindowWidgetRecordType>(gpResourceManager->ReadWord());
        pWidget = NULL;
        switch (type) {
            case WIDGET_RECORD_END:
                idx++;
                break;
            case WIDGET_RECORD_BORDER:
                pBorder = new border();
                pBorder->Read();
                pWidget = pBorder;
                break;
            case WIDGET_RECORD_BUTTON:
                pButton = new button();
                pButton->Read();
                pWidget = pButton;
                break;
            case WIDGET_RECORD_ICON:
                pIcon = new iconWidget();
                pIcon->Read();
                pWidget = pIcon;
                break;
            case WIDGET_RECORD_DIMMER:
                pDimmer = new dimmerWidget();
                pDimmer->Read();
                pWidget = pDimmer;
                break;
            case WIDGET_RECORD_TEXT:
                pText = new textWidget();
                pText->Read();
                pWidget = pText;
                break;
            case WIDGET_RECORD_TEXT_ENTRY:
                pTextEnt = new textEntryWidget();
                pTextEnt->Read(TEXT_ENTRY_READ_DEFAULT);
                pWidget = pTextEnt;
                break;
            case WIDGET_RECORD_TEXT_ENTRY_RECT:
                pTextEnt = new textEntryWidget();
                pTextEnt->Read(TEXT_ENTRY_READ_RECT);
                pWidget = pTextEnt;
                break;
            case WIDGET_RECORD_TEXT_ENTRY_MULTILINE:
                pTextEnt = new textEntryWidget();
                pTextEnt->Read(TEXT_ENTRY_READ_MULTILINE);
                pWidget = pTextEnt;
                break;
            case WIDGET_RECORD_TEXT_ENTRY_INSET_FIVE:
                pTextEnt = new textEntryWidget();
                pTextEnt->Read(TEXT_ENTRY_READ_INSET_FIVE);
                pWidget = pTextEnt;
                break;
            case WIDGET_RECORD_TEXT_ENTRY_INSET_FOUR:
                pTextEnt = new textEntryWidget();
                pTextEnt->Read(TEXT_ENTRY_READ_INSET_FOUR);
                pWidget = pTextEnt;
                break;
            case WIDGET_RECORD_DROP_LIST:
                pDrop = new dropListWidget();
                pDrop->Read();
                pWidget = pDrop;
                break;
            case WIDGET_RECORD_LIST_BOX:
                pListBox = new listBoxWidget();
                pListBox->Read();
                pWidget = pListBox;
                break;
        }
        if (idx == 0 && pWidget != NULL)
            AddWidget(pWidget, -1);
    }
}

VA(0x004bae20, 0x66)
i32 heroWindow::Open(i32 x, i32 flags) {
    if (HAS(m_winState, WINDOW_STATE_OPEN) != 0)
        return OPEN_FAILURE;
    if (HAS(m_winFlags, WINDOW_FLAG_SAVE_BACKGROUND) != 0 && SaveBackground() != 0)
        return OPEN_FAILURE;
    m_zOrder = x;
    DrawWindow(flags);
    m_winState |= WINDOW_STATE_OPEN;
    return 0;
}

VA(0x004bae90, 0x82)
void heroWindow::RemoveAndDeleteWidget(i32 id) {
    widget *w, *next;
    w = m_widgetListHead;
    while (w != NULL) {
        next = w->m_next;
        if (w->m_id == id) {
            RemoveWidget(w);
            if (HAS(m_winFlags, WINDOW_FLAG_OWNS_WIDGETS) != 0)
                delete w;
        }
        w = next;
    }
}

VA(0x004baf20, 0xa0)
void heroWindow::Close(void) {
    widget *w, *next;
    if (HAS(m_winFlags, WINDOW_FLAG_SAVE_BACKGROUND) != 0
        && HAS(m_winState, WINDOW_STATE_OPEN) != 0)
        RestoreBackground();
    w = m_widgetListHead;
    while (w != NULL) {
        next = w->m_next;
        RemoveWidget(w);
        if (HAS(m_winFlags, WINDOW_FLAG_OWNS_WIDGETS) != 0) {
            delete w;
        }
        w = next;
    }
    m_winState = WINDOW_STATE_CLOSED;
}

VA(0x004bafc0, 0x109)
void heroWindow::AddWidget(class widget* newWidget, i32 zOrder) {
    widget* local_8 = m_widgetListHead;
    if (zOrder == -1) {
        if (local_8 == NULL)
            zOrder = 0;
        else
            zOrder = local_8->m_zOrder + 1;
    }
    if (newWidget->Open(zOrder, this) != 0)
        return;
    while (local_8 != NULL && local_8->m_zOrder > zOrder) {
        local_8 = local_8->m_next;
    }
    if (local_8 == NULL) {
        newWidget->m_prev = m_widgetListTail;
        newWidget->m_next = NULL;
        m_widgetListTail = newWidget;
        if (m_widgetListHead == NULL)
            m_widgetListHead = newWidget;
    } else if (local_8->m_prev == NULL) {
        newWidget->m_next = m_widgetListHead;
        newWidget->m_prev = NULL;
        m_widgetListHead->m_prev = newWidget;
        m_widgetListHead = newWidget;
    } else {
        newWidget->m_next = local_8;
        newWidget->m_prev = local_8->m_prev;
        local_8->m_prev->m_next = newWidget;
        local_8->m_prev = newWidget;
    }
}

VA(0x004bb0d0, 0xe7)
void heroWindow::RemoveWidget(class widget* w) {
    if (w == NULL)
        return;
    w->Close();
    if (w == m_widgetListTail) {
        m_widgetListTail = w->m_prev;
        if (m_widgetListTail == NULL)
            m_widgetListHead = NULL;
        else
            m_widgetListTail->m_next = NULL;
    } else if (w == m_widgetListHead) {
        m_widgetListHead = w->m_next;
        m_widgetListHead->m_prev = NULL;
    } else {
        w->m_next->m_prev = w->m_prev;
        w->m_prev->m_next = w->m_next;
    }
    widget* nextWidget = w->m_next;
    if (nextWidget == NULL) {
        m_widgetListTail = m_widgetListHead = NULL;
    } else {
        nextWidget->m_prev = w->m_prev;
        if (nextWidget->m_prev != NULL)
            nextWidget->m_prev->m_next = nextWidget;
    }
}

VA(0x004bb1c0, 0x5e)
MessageDispatchResult heroWindow::BroadcastMessage(struct tag_message& message) {
    MessageDispatchResult local_8 = MESSAGE_DISPATCH_CONTINUE;
    widget* local_c = m_widgetListHead;
    while (local_c != NULL) {
        switch (local_8 = local_c->Main(message)) {
            case MESSAGE_DISPATCH_CONTINUE:
                break;
            case MESSAGE_DISPATCH_CONSUME:
            case MESSAGE_DISPATCH_FORWARD:
                return local_8;
        }
        local_c = local_c->m_next;
    }
    return local_8;
}

VA(0x004bb220, 0x15)
void heroWindow::DrawWindow(void) {
    DrawWindow(1);
}

VA(0x004bb240, 0x23)
void heroWindow::DrawWindow(i32 flags) {
    DrawWindow(flags, WINDOW_ALL_WIDGETS_LOW, WINDOW_ALL_WIDGETS_HIGH);
}

VA(0x004bb270, 0xed)
void heroWindow::DrawWindow(i32 update, i32 firstId, i32 lastId) {
    tag_message local_24;
    widget* local_8;
    gpMouseManager->m_cursorReady = 0;
    local_8 = m_widgetListTail;
    local_24.type = MESSAGE_WIDGET;
    local_24.payload.widget.command = WIDGET_COMMAND_DRAW;
    while (local_8 != NULL) {
        PollSound();
        if (firstId != WINDOW_ALL_WIDGETS_LOW || lastId != WINDOW_ALL_WIDGETS_HIGH) {
            if (local_8->m_id >= firstId && local_8->m_id <= lastId)
                local_8->Main(local_24);
        } else
            local_8->Main(local_24);
        local_8 = local_8->m_prev;
    }
    PollSound();
    if (update != 0
        && (m_winFlags & WINDOW_UPDATE_SUPPRESS_MASK) != WINDOW_FLAG_FIXED_LAYER) {
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
        PollSound();
    }
    gpMouseManager->m_cursorReady = 1;
}

VA(0x004bb360, 0xa9)
i32 heroWindow::SaveBackground(void) {
    m_savedBackground = new bitmap(BITMAP_TYPE_MEMORY, m_winWidth, m_winHeight);
    PollSound();
    m_savedBackground->GrabScreen(m_posX, m_posY);
    PollSound();
    return 0;
}

VA(0x004bb410, 0x8f)
void heroWindow::RestoreBackground(void) {
    if (gbDrawWindowBackground) {
        m_savedBackground->DrawToBuffer(m_posX, m_posY);
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
    }
    delete m_savedBackground;
    m_savedBackground = NULL;
}

VA(0x004bb4a0, 0x175)
void heroWindow::MoveWindow(i32 dx, i32 dy) {
    i32 x = m_posX;
    i32 yPrev = m_posY;
    i32 oldWidth = m_winWidth;
    i32 oldHgt = m_winHeight;
    i32 toX = m_posX + dx;
    i32 toY = m_posY + dy;
    if (toX < 0)
        toX = 0;
    if (toY < 0)
        toY = 0;
    if (SCREEN_WIDTH < toX + m_winWidth)
        toX = SCREEN_WIDTH - m_winWidth;
    if (SCREEN_HEIGHT < toY + m_winHeight)
        toY = SCREEN_HEIGHT - m_winHeight;
    m_savedBackground->DrawToBuffer(m_posX, m_posY);
    m_posX = toX;
    m_posY = toY;
    m_savedBackground->GrabBitmap(gpWindowManager->m_screen, m_posX, m_posY);
    DrawWindow(0);
    oldWidth = oldWidth + abs(m_posX - x);
    oldHgt = oldHgt + abs(m_posY - yPrev);
    if (m_posX < x)
        x = m_posX;
    if (m_posY < yPrev)
        yPrev = m_posY;
    gpWindowManager->UpdateScreenRegion(x, yPrev, oldWidth, oldHgt);
}
