#include <Ints.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/widget.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <PLATFORM/Runtime.h>
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
enum class WindowWidgetRecordType : i32 {
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
};
using enum WindowWidgetRecordType;

constexpr WindowWidgetRecordType WindowWidgetRecordTypeFromCode(i16 value) {
    return static_cast<WindowWidgetRecordType>(value); // H2_ENUM_CODE_BOUNDARY
}

typedef enum WindowConstant {
    SCREEN_WIDTH  = 640,
    SCREEN_HEIGHT = 480,
    OPEN_FAILURE  = 3
} WindowConstant;

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

heroWindow::heroWindow(
    i32 x, i32 y, i32 width, i32 height, WindowFlag flags
) {
    strcpy(name, "Dynamic Construct");
    m_nextWindow = m_prevWindow = NULL;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = width;
    m_winHeight = height;
    m_winFlags = flags;
    m_winState = WINDOW_STATE_CLOSED;
    m_widgetListTail = m_widgetListHead = NULL;
    m_savedBackground = NULL;
}

heroWindow::heroWindow(i32 x, i32 y, const char* resourceName) {
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
    i32 finishedReading;
    u32l resourceId;
    strcpy(name, resourceName);
    resourceId = gpResourceManager->MakeId(resourceName, 1);
    gpResourceManager->PointToFile(resourceId);
    m_savedBackground = NULL;
    m_nextWindow = m_prevWindow = NULL;
    m_winState = WINDOW_STATE_CLOSED;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = gpResourceManager->ReadWord();
    m_winHeight = gpResourceManager->ReadWord();
    m_winFlags = WindowFlagFromCode(gpResourceManager->ReadWord());
    m_winFlags |= WINDOW_FLAG_OWNS_WIDGETS;
    m_widgetListTail = m_widgetListHead = NULL;
    finishedReading = 0;
    while (finishedReading == 0) {
        PollSound();
        type = WindowWidgetRecordTypeFromCode(gpResourceManager->ReadWord());
        pWidget = NULL;
        switch (type) {
            case WIDGET_RECORD_END:
                finishedReading++;
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
        if (finishedReading == 0 && pWidget != NULL)
            AddWidget(pWidget, -1);
    }
}

i32 heroWindow::Open(i32 x, i32 flags) {
    if ((H2EnumIndex((m_winState) & (WINDOW_STATE_OPEN))) != 0)
        return OPEN_FAILURE;
    if ((H2EnumIndex((m_winFlags) & (WINDOW_FLAG_SAVE_BACKGROUND))) != 0 && SaveBackground() != 0)
        return OPEN_FAILURE;
    m_zOrder = x;
    DrawWindow(flags);
    m_winState |= WINDOW_STATE_OPEN;
    return 0;
}

void heroWindow::RemoveAndDeleteWidget(i32 id) {
    widget *currentWidget, *next;
    currentWidget = m_widgetListHead;
    while (currentWidget != NULL) {
        next = currentWidget->m_next;
        if (currentWidget->m_id == id) {
            RemoveWidget(currentWidget);
            if ((H2EnumIndex((m_winFlags) & (WINDOW_FLAG_OWNS_WIDGETS))) != 0)
                delete currentWidget;
        }
        currentWidget = next;
    }
}

void heroWindow::Close(void) {
    widget *currentWidget, *next;
    if ((H2EnumIndex((m_winFlags) & (WINDOW_FLAG_SAVE_BACKGROUND))) != 0
        && (H2EnumIndex((m_winState) & (WINDOW_STATE_OPEN))) != 0)
        RestoreBackground();
    currentWidget = m_widgetListHead;
    while (currentWidget != NULL) {
        next = currentWidget->m_next;
        RemoveWidget(currentWidget);
        if ((H2EnumIndex((m_winFlags) & (WINDOW_FLAG_OWNS_WIDGETS))) != 0) {
            delete currentWidget;
        }
        currentWidget = next;
    }
    m_winState = WINDOW_STATE_CLOSED;
}

void heroWindow::AddWidget(class widget* newWidget, i32 zOrder) {
    widget* currentWidget = m_widgetListHead;
    if (zOrder == -1) {
        if (currentWidget == NULL)
            zOrder = 0;
        else
            zOrder = currentWidget->m_zOrder + 1;
    }
    if (newWidget->Open(zOrder, this) != 0)
        return;
    while (currentWidget != NULL && currentWidget->m_zOrder > zOrder) {
        currentWidget = currentWidget->m_next;
    }
    if (currentWidget == NULL) {
        newWidget->m_prev = m_widgetListTail;
        newWidget->m_next = NULL;
        m_widgetListTail = newWidget;
        if (m_widgetListHead == NULL)
            m_widgetListHead = newWidget;
    } else if (currentWidget->m_prev == NULL) {
        newWidget->m_next = m_widgetListHead;
        newWidget->m_prev = NULL;
        m_widgetListHead->m_prev = newWidget;
        m_widgetListHead = newWidget;
    } else {
        newWidget->m_next = currentWidget;
        newWidget->m_prev = currentWidget->m_prev;
        currentWidget->m_prev->m_next = newWidget;
        currentWidget->m_prev = newWidget;
    }
}

void heroWindow::RemoveWidget(class widget* currentWidget) {
    if (currentWidget == NULL)
        return;
    currentWidget->Close();
    if (currentWidget == m_widgetListTail) {
        m_widgetListTail = currentWidget->m_prev;
        if (m_widgetListTail == NULL)
            m_widgetListHead = NULL;
        else
            m_widgetListTail->m_next = NULL;
    } else if (currentWidget == m_widgetListHead) {
        m_widgetListHead = currentWidget->m_next;
        m_widgetListHead->m_prev = NULL;
    } else {
        currentWidget->m_next->m_prev = currentWidget->m_prev;
        currentWidget->m_prev->m_next = currentWidget->m_next;
    }
    widget* nextWidget = currentWidget->m_next;
    if (nextWidget == NULL) {
        m_widgetListTail = m_widgetListHead = NULL;
    } else {
        nextWidget->m_prev = currentWidget->m_prev;
        if (nextWidget->m_prev != NULL)
            nextWidget->m_prev->m_next = nextWidget;
    }
}

MessageDispatchResult heroWindow::BroadcastMessage(struct tag_message& message) {
    MessageDispatchResult dispatchResult = MESSAGE_DISPATCH_CONTINUE;
    widget* currentWidget = m_widgetListHead;
    while (currentWidget != NULL) {
        switch (dispatchResult = currentWidget->Main(message)) {
            case MESSAGE_DISPATCH_CONTINUE:
                break;
            case MESSAGE_DISPATCH_CONSUME:
            case MESSAGE_DISPATCH_FORWARD:
                return dispatchResult;
        }
        currentWidget = currentWidget->m_next;
    }
    return dispatchResult;
}

void heroWindow::DrawWindow(void) {
    DrawWindow(1);
}

void heroWindow::DrawWindow(i32 flags) {
    DrawWindow(flags, WINDOW_ALL_WIDGETS_LOW, WINDOW_ALL_WIDGETS_HIGH);
}

void heroWindow::DrawWindow(i32 update, i32 firstId, i32 lastId) {
    tag_message message;
    widget* currentWidget;
    gpMouseManager->m_cursorReady = 0;
    currentWidget = m_widgetListTail;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_DRAW;
    while (currentWidget != NULL) {
        PollSound();
        if (firstId != WINDOW_ALL_WIDGETS_LOW || lastId != WINDOW_ALL_WIDGETS_HIGH) {
            if (currentWidget->m_id >= firstId && currentWidget->m_id <= lastId)
                currentWidget->Main(message);
        } else
            currentWidget->Main(message);
        currentWidget = currentWidget->m_prev;
    }
    PollSound();
    if (update != 0
        && (m_winFlags & WINDOW_UPDATE_SUPPRESS_MASK) != WINDOW_FLAG_FIXED_LAYER) {
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
        PollSound();
    }
    gpMouseManager->m_cursorReady = 1;
}

i32 heroWindow::SaveBackground(void) {
    m_savedBackground = new bitmap(BITMAP_TYPE_MEMORY, m_winWidth, m_winHeight);
    PollSound();
    m_savedBackground->GrabScreen(m_posX, m_posY);
    PollSound();
    return 0;
}

void heroWindow::RestoreBackground(void) {
    if (gbDrawWindowBackground) {
        m_savedBackground->DrawToBuffer(m_posX, m_posY);
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
    }
    delete m_savedBackground;
    m_savedBackground = NULL;
}

void heroWindow::MoveWindow(i32 dx, i32 dy) {
    i32 x = m_posX;
    i32 yPrev = m_posY;
    i32 oldWidth = m_winWidth;
    i32 oldHgt = m_winHeight;
    i32 destinationX = m_posX + dx;
    i32 destinationY = m_posY + dy;
    if (destinationX < 0)
        destinationX = 0;
    if (destinationY < 0)
        destinationY = 0;
    if (SCREEN_WIDTH < destinationX + m_winWidth)
        destinationX = SCREEN_WIDTH - m_winWidth;
    if (SCREEN_HEIGHT < destinationY + m_winHeight)
        destinationY = SCREEN_HEIGHT - m_winHeight;
    m_savedBackground->DrawToBuffer(m_posX, m_posY);
    m_posX = destinationX;
    m_posY = destinationY;
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
