// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINDOW.OBJ   from: .\basewin.lib
// functions: 15   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

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
VA(0x004ceb70, 0xaa)
heroWindow::heroWindow(void) {
    strcpy(name, "Default Construct");
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_zOrder = -1;
    m_posY = 0;
    m_posX = m_posY;
    m_winWidth = 640;
    m_winHeight = 480;
    m_winFlags = 1;
    m_winState = 0;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    m_savedBackground = 0;
}

VA(0x004cec20, 0xa5)
heroWindow::heroWindow(i32 x, i32 y, i32 w, i32 h, i32 flags) {
    strcpy(name, "Dynamic Construct");
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_zOrder = -1;
    m_posX = x;
    m_posY = y;
    m_winWidth = w;
    m_winHeight = h;
    m_winFlags = flags;
    m_winState = 0;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    m_savedBackground = 0;
}

VA(0x004cecd0, 0x521)
heroWindow::heroWindow(i32 param_1, i32 param_2, char* param_3) {
    u32l jb;
    i32 idx;
    textEntryWidget* pte;
    textWidget* ptw;
    i32 type;
    button* pbtn;
    dropListWidget* pdl;
    border* pbd;
    widget* pwdg;
    iconWidget* picn;
    listBoxWidget* plist;
    dimmerWidget* pdim;
    strcpy(name, param_3);
    jb = gpResourceManager->MakeId(param_3, 1);
    gpResourceManager->PointToFile(jb);
    m_savedBackground = 0;
    m_prevWindow = 0;
    m_nextWindow = m_prevWindow;
    m_winState = 0;
    m_zOrder = -1;
    m_posX = param_1;
    m_posY = param_2;
    m_winWidth = gpResourceManager->ReadWord();
    m_winHeight = gpResourceManager->ReadWord();
    m_winFlags = gpResourceManager->ReadWord();
    m_winFlags |= 0x4000;
    m_widgetListHead = 0;
    m_widgetListTail = m_widgetListHead;
    idx = 0;
    while (idx == 0) {
        PollSound();
        type = gpResourceManager->ReadWord();
        pwdg = 0;
        switch (type) {
            case WINDOW_WIDGET_RECORD_END:
                idx++;
                break;
            case WINDOW_WIDGET_RECORD_BORDER:
                pbd = new border();
                pbd->Read();
                pwdg = pbd;
                break;
            case WINDOW_WIDGET_RECORD_BUTTON:
                pbtn = new button();
                pbtn->Read();
                pwdg = pbtn;
                break;
            case WINDOW_WIDGET_RECORD_ICON:
                picn = new iconWidget();
                picn->Read();
                pwdg = picn;
                break;
            case WINDOW_WIDGET_RECORD_DIMMER:
                pdim = new dimmerWidget();
                pdim->Read();
                pwdg = pdim;
                break;
            case WINDOW_WIDGET_RECORD_TEXT:
                ptw = new textWidget();
                ptw->Read();
                pwdg = ptw;
                break;
            case WINDOW_WIDGET_RECORD_TEXT_ENTRY:
                pte = new textEntryWidget();
                pte->Read(1);
                pwdg = pte;
                break;
            case WINDOW_WIDGET_RECORD_TEXT_ENTRY_RECT:
                pte = new textEntryWidget();
                pte->Read(2);
                pwdg = pte;
                break;
            case WINDOW_WIDGET_RECORD_TEXT_ENTRY_MULTILINE:
                pte = new textEntryWidget();
                pte->Read(3);
                pwdg = pte;
                break;
            case WINDOW_WIDGET_RECORD_TEXT_ENTRY_INSET_FIVE:
                pte = new textEntryWidget();
                pte->Read(4);
                pwdg = pte;
                break;
            case WINDOW_WIDGET_RECORD_TEXT_ENTRY_INSET_FOUR:
                pte = new textEntryWidget();
                pte->Read(5);
                pwdg = pte;
                break;
            case WINDOW_WIDGET_RECORD_DROP_LIST:
                pdl = new dropListWidget();
                pdl->Read();
                pwdg = pdl;
                break;
            case WINDOW_WIDGET_RECORD_LIST_BOX:
                plist = new listBoxWidget();
                plist->Read();
                pwdg = plist;
                break;
        }
        if (idx == 0 && pwdg != 0)
            AddWidget(pwdg, -1);
    }
}

VA(0x004cf200, 0x73)
i32 heroWindow::Open(i32 x, i32 flags) {
    if (m_winState & 1)
        return 3;
    if ((m_winFlags & 2) != 0 && SaveBackground() != 0)
        return 3;
    m_zOrder = x;
    DrawWindow(flags);
    m_winState |= 1;
    return 0;
}

VA(0x004cf280, 0x90)
void heroWindow::RemoveAndDeleteWidget(i32 id) {
    widget *w, *next;
    w = m_widgetListHead;
    while (w != 0) {
        next = w->m_next;
        if (w->m_id == id) {
            RemoveWidget(w);
            if ((m_winFlags & WINDOW_FLAG_OWNS_WIDGETS) != 0)
                delete w;
        }
        w = next;
    }
}

VA(0x004cf310, 0xaa)
void heroWindow::Close(void) {
    widget *w, *next;
    if ((m_winFlags & 2) != 0 && (m_winState & 1) != 0)
        RestoreBackground();
    w = m_widgetListHead;
    while (w != 0) {
        next = w->m_next;
        RemoveWidget(w);
        if ((m_winFlags & WINDOW_FLAG_OWNS_WIDGETS) != 0) {
            delete w;
        }
        w = next;
    }
    m_winState = 0;
}

VA(0x004cf3c0, 0x13c)
void heroWindow::AddWidget(class widget* param_1, i32 param_2) {
    widget* local_8 = m_widgetListHead;
    if (param_2 == -1) {
        if (local_8 == 0)
            param_2 = 0;
        else
            param_2 = local_8->m_zOrder + 1;
    }
    if (param_1->Open(param_2, this) != 0)
        return;
    while (local_8 != 0 && param_2 < local_8->m_zOrder) {
        local_8 = local_8->m_next;
    }
    if (local_8 == 0) {
        param_1->m_prev = m_widgetListTail;
        param_1->m_next = 0;
        m_widgetListTail = param_1;
        if (m_widgetListHead == 0)
            m_widgetListHead = param_1;
    } else if (local_8->m_prev == 0) {
        param_1->m_next = m_widgetListHead;
        param_1->m_prev = 0;
        m_widgetListHead->m_prev = param_1;
        m_widgetListHead = param_1;
    } else {
        param_1->m_next = local_8;
        param_1->m_prev = local_8->m_prev;
        local_8->m_prev->m_next = param_1;
        local_8->m_prev = param_1;
    }
}

VA(0x004cf500, 0x116)
void heroWindow::RemoveWidget(class widget* w) {
    if (w == 0)
        return;
    // Retail call VA 0x004cf51e resolves to CodeView widget::Close at 0x004dded0;
    // both retail and candidate Close bodies are the same lone C3.
    w->Close();
    if (m_widgetListTail == w) {
        m_widgetListTail = w->m_prev;
        if (m_widgetListTail == 0)
            m_widgetListHead = 0;
        else
            m_widgetListTail->m_next = 0;
    } else if (m_widgetListHead == w) {
        m_widgetListHead = w->m_next;
        m_widgetListHead->m_prev = 0;
    } else {
        w->m_next->m_prev = w->m_prev;
        w->m_prev->m_next = w->m_next;
    }
    widget* nextWidget = w->m_next;
    if (nextWidget == 0) {
        m_widgetListHead = 0;
        m_widgetListTail = m_widgetListHead;
    } else {
        nextWidget->m_prev = w->m_prev;
        if (nextWidget->m_prev != 0)
            nextWidget->m_prev->m_next = nextWidget;
    }
}

VA(0x004cf620, 0x95)
i32 heroWindow::BroadcastMessage(struct tag_message& param_1) {
    i32 local_8 = 0;
    widget* local_c = m_widgetListHead;
    while (local_c != 0) {
        switch (local_8 = local_c->Main(param_1)) {
            case 0:
                break;
            case 1:
            case 2:
                return local_8;
        }
        local_c = local_c->m_next;
    }
    return local_8;
}

VA(0x004cf6c0, 0x20)
void heroWindow::DrawWindow(void) {
    DrawWindow(1);
}

VA(0x004cf6e0, 0x2e)
void heroWindow::DrawWindow(i32 flags) {
    DrawWindow(flags, -0xffff, 0xffff);
}

VA(0x004cf710, 0x116)
void heroWindow::DrawWindow(i32 param_1, i32 param_2, i32 param_3) {
    tag_message local_24;
    widget* local_8;
    gpMouseManager->m_cursorReady = 0;
    local_8 = m_widgetListTail;
    local_24.type = MESSAGE_WIDGET;
    local_24.payload.widget.command = WIDGET_COMMAND_DRAW;
    while (local_8 != 0) {
        PollSound();
        if (param_2 != WINDOW_ALL_WIDGETS_LOW || param_3 != WINDOW_ALL_WIDGETS_HIGH) {
            if (param_2 <= local_8->m_id && local_8->m_id <= param_3)
                local_8->Main(local_24);
        } else
            local_8->Main(local_24);
        local_8 = local_8->m_prev;
    }
    PollSound();
    if (param_1 != 0 && (m_winFlags & WINDOW_UPDATE_SUPPRESS_MASK) != 1) {
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
        PollSound();
    }
    gpMouseManager->m_cursorReady = 1;
}

VA(0x004cf830, 0x7f)
i32 heroWindow::SaveBackground(void) {
    m_savedBackground = new bitmap(BITMAP_TYPE_MEMORY, m_winWidth, m_winHeight);
    PollSound();
    m_savedBackground->GrabScreen(m_posX, m_posY);
    PollSound();
    return 0;
}

VA(0x004cf8b0, 0x97)
void heroWindow::RestoreBackground(void) {
    if (gbDrawWindowBackground) {
        m_savedBackground->DrawToBuffer(m_posX, m_posY);
        gpWindowManager->UpdateScreenRegion(m_posX, m_posY, m_winWidth, m_winHeight);
    }
    delete m_savedBackground;
    m_savedBackground = 0;
}

VA(0x004cf950, 0x186)
void heroWindow::MoveWindow(i32 dx, i32 dy) {
    i32 oldX = m_posX;
    i32 oldY = m_posY;
    i32 oldW = m_winWidth;
    i32 oldH = m_winHeight;
    i32 newX = m_posX + dx;
    i32 newY = m_posY + dy;
    if (newX < 0)
        newX = 0;
    if (newY < 0)
        newY = 0;
    if (640 < m_winWidth + newX)
        newX = 640 - m_winWidth;
    if (480 < m_winHeight + newY)
        newY = 480 - m_winHeight;
    m_savedBackground->DrawToBuffer(m_posX, m_posY);
    m_posX = newX;
    m_posY = newY;
    m_savedBackground->GrabBitmap(gpWindowManager->m_screen, m_posX, m_posY);
    DrawWindow(0);
    oldW = oldW + abs(m_posX - oldX);
    oldH = oldH + abs(m_posY - oldY);
    if (m_posX < oldX)
        oldX = m_posX;
    if (m_posY < oldY)
        oldY = m_posY;
    gpWindowManager->UpdateScreenRegion(oldX, oldY, oldW, oldH);
}
