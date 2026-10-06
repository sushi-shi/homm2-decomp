

#include <Ints.h>
#include <EDITOR/clearManager.h>
#include <EDITOR/editManager.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/IconDraw.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <BASE/widgetKind.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <stdlib.h>
#include <string.h>

i32 gClearBrush = CLEAR_BRUSH_DOUBLE;

clearManager::clearManager(void) {
    m_lastY = EDIT_NO_CELL;
    m_lastX = EDIT_NO_CELL;
}

i32 clearManager::Open(i32 priority) {
    i32 brush;

    for (brush = 0; brush < CLEAR_BRUSH_COUNT; brush++) {
        m_brushButtons[brush] = new iconWidget(
            CLEAR_BRUSH_BUTTON_X + brush * CLEAR_BRUSH_BUTTON_STEP,
            CLEAR_BRUSH_BUTTON_Y,
            CLEAR_BRUSH_BUTTON_WIDTH,
            CLEAR_BRUSH_BUTTON_HEIGHT,
            "editbtns.icn",
            CLEAR_BRUSH_FRAME_FIRST + brush * 2 + (brush == gClearBrush),
            ICON_DRAW_NORMAL,
            CLEAR_BRUSH_BUTTON_ID_FIRST + brush,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        gEditManager->m_window->AddWidget(m_brushButtons[brush], -1);
    }
    gEditManager->m_window->DrawWindow();
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = 1;
    strcpy(m_name, "clearManager");
    return 0;
}

void clearManager::Close(void) {
    i32 brush;

    for (brush = 0; brush < CLEAR_BRUSH_COUNT; brush++) {
        gEditManager->m_window->RemoveWidget(m_brushButtons[brush]);
        delete m_brushButtons[brush];
    }
    if (gSelectionX != EDIT_NO_CELL) {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
        gEditManager->DrawMap();
    }
    gEditManager->m_window->DrawWindow();
    m_active = 0;
}

void clearManager::UpdateBrushButtons(void) {
    i32 brush;
    tag_message message;

    for (brush = 0; brush < CLEAR_BRUSH_COUNT; brush++) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = CLEAR_BRUSH_BUTTON_ID_FIRST + brush;
        message.payload.widget.data.value
            = CLEAR_BRUSH_FRAME_FIRST + brush * 2 + (brush == gClearBrush);
        gEditManager->m_window->BroadcastMessage(message);
    }
    gEditManager->m_window->DrawWindow(0);
    gpWindowManager->UpdateScreenRegion(
        CLEAR_PANEL_REGION_X, CLEAR_PANEL_REGION_Y,
        CLEAR_PANEL_REGION_WIDTH, CLEAR_PANEL_REGION_HEIGHT);
}

MessageDispatchResult clearManager::Main(tag_message& message) {
    i32 help;
    i32 anchorY;
    i32 x;
    i32 y;
    tag_message event;
    i32 anchorX;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK:
                    if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                        help = -1;
                        switch (message.payload.widget.id) {
                            case CLEAR_BRUSH_BUTTON_ID_FIRST:
                                help = CLEAR_BRUSH_SINGLE;
                                break;
                            case CLEAR_BRUSH_BUTTON_ID_FIRST + 1:
                                help = CLEAR_BRUSH_DOUBLE;
                                break;
                            case CLEAR_BRUSH_BUTTON_ID_FIRST + 2:
                                help = CLEAR_BRUSH_QUADRUPLE;
                                break;
                            case CLEAR_BRUSH_BUTTON_ID_FIRST + 3:
                                help = CLEAR_BRUSH_AREA;
                                break;
                        }
                        if (help >= 0)
                            NormalDialog(gClearHelp[help], NORMAL_DIALOG_QUICK_VIEW);
                    } else {
                        switch (message.payload.widget.id) {
                            case EDIT_CONTROL_MAP:
                                anchorX = message.payload.widget.screenX;
                                anchorY = message.payload.widget.screenY;
                                gEditManager->ScreenToCell(anchorX, anchorY);
                                gSelectionX = EDIT_NO_CELL;
                                anchorX += gEditManager->m_viewX;
                                anchorY += gEditManager->m_viewY;
                                gEditManager->SaveUndo();
                                event = gpInputManager->GetEvent();
                                while (event.type != MESSAGE_LEFT_BUTTON_UP
                                       && event.type != MESSAGE_RIGHT_BUTTON_UP) {
                                    Process1WindowsMessage();
                                    gpMouseManager->Main(event);
                                    if (event.type == MESSAGE_MOUSE_MOVE) {
                                        x = event.payload.mouse.screenX;
                                        y = event.payload.mouse.screenY;
                                        gEditManager->ScreenToCell(x, y);
                                        x += gEditManager->m_viewX;
                                        y += gEditManager->m_viewY;
                                        if (x != m_lastX || y != m_lastY) {
                                            m_lastX = x;
                                            m_lastY = y;
                                            if (gClearBrush <= CLEAR_BRUSH_QUADRUPLE) {
                                                gEditManager->ClearArea(
                                                    x, y, gClearBrush + 1, gClearBrush + 1,
                                                    0, 0, 0);
                                                SelectBrush(gClearBrush, x, y);
                                                gEditManager->DrawMap();
                                                gEditManager->UpdateMapView();
                                                gEditManager->DrawRadar(1);
                                            } else {
                                                gSelectionX = x < anchorX ? x : anchorX;
                                                gSelectionY = y < anchorY ? y : anchorY;
                                                gSelectionWidth = abs(x - anchorX) + 1;
                                                gSelectionHeight = abs(y - anchorY) + 1;
                                                gEditManager->DrawMap();
                                                gEditManager->UpdateMapView();
                                                gEditManager->DrawRadar(1);
                                            }
                                        }
                                    }
                                    event = gpInputManager->GetEvent();
                                }
                                if (gClearBrush <= CLEAR_BRUSH_QUADRUPLE)
                                    gEditManager->ClearArea(
                                        anchorX, anchorY, gClearBrush + 1, gClearBrush + 1,
                                        0, 0, 0);
                                else
                                    gEditManager->ClearArea(
                                        gSelectionX, gSelectionY,
                                        gSelectionWidth, gSelectionHeight, 0, 0, 0);
                                gSelectionX = gSelectionY = EDIT_NO_CELL;
                                gEditManager->DrawMap();
                                gEditManager->UpdateMapView();
                                gEditManager->DrawRadar(1);
                                m_lastY = EDIT_NO_CELL;
                                m_lastX = EDIT_NO_CELL;
                                gEditManager->m_mapChanged = 1;
                                break;
                        }
                    }
                    break;
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case CLEAR_BRUSH_BUTTON_ID_FIRST:
                        case CLEAR_BRUSH_BUTTON_ID_FIRST + 1:
                        case CLEAR_BRUSH_BUTTON_ID_FIRST + 2:
                        case CLEAR_BRUSH_BUTTON_ID_FIRST + 3:
                            gClearBrush = message.payload.widget.id - CLEAR_BRUSH_BUTTON_ID_FIRST;
                            UpdateBrushButtons();
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                TrackCursor();
            } else if (gSelectionX != EDIT_NO_CELL) {
                gSelectionX = gSelectionY = EDIT_NO_CELL;
                gEditManager->DrawMap();
                gEditManager->UpdateMapView();
            }
            return MESSAGE_DISPATCH_CONSUME;
        case MESSAGE_KEY_DOWN:
            if ((message.payload.keyboard.keyCode == CLEAR_BRUSH_KEY_FIRST
                 || message.payload.keyboard.keyCode == CLEAR_BRUSH_KEY_FIRST + 1
                 || message.payload.keyboard.keyCode == CLEAR_BRUSH_KEY_FIRST + 2
                 || message.payload.keyboard.keyCode == CLEAR_BRUSH_KEY_LAST)
                && !(((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                gClearBrush = message.payload.keyboard.keyCode - CLEAR_BRUSH_KEY_FIRST;
                TrackCursor();
                UpdateBrushButtons();
                return MESSAGE_DISPATCH_CONSUME;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

void clearManager::TrackCursor(void) {
    i32 x;
    i32 y;
    tag_message event;

    gpMouseManager->MouseCoords(x, y);
    gEditManager->ScreenToCell(x, y);
    x += gEditManager->m_viewX;
    y += gEditManager->m_viewY;
    if (gEditManager->m_cursorX == x && gEditManager->m_cursorY == y
        && gEditManager->m_cursorSize == gClearBrush && !gClearCursorMoves)
        return;
    SelectBrush(gClearBrush, x, y);
    event = gpInputManager->PeekEvent();
    if (event.type == MESSAGE_MOUSE_MOVE) {
        gClearCursorMoves++;
        if (gClearCursorMoves < CLEAR_CURSOR_REDRAW_INTERVAL)
            return;
    }
    gClearCursorMoves = 0;
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    if (gEditManager->m_cursorX != x || gEditManager->m_cursorY != y) {
        gEditManager->m_cursorX = x;
        gEditManager->m_cursorY = y;
        gEditManager->UpdateCursor();
    }
    gEditManager->m_cursorSize = gClearBrush;
}

void clearManager::SelectBrush(i32 brush, i32 x, i32 y) {
    if (brush == CLEAR_BRUSH_SINGLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = 1;
        gSelectionHeight = 1;
    } else if (brush == CLEAR_BRUSH_DOUBLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = 2;
        gSelectionHeight = 2;
    } else if (brush == CLEAR_BRUSH_QUADRUPLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = 4;
        gSelectionHeight = 4;
    } else {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
    }
}
