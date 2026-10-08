

#include <H2/Ints.h>
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

i32 gClearBrush = EDIT_BRUSH_DOUBLE;
i32 gClearCursorMoves;

clearManager::clearManager(void) {
    m_lastY = EDIT_NO_CELL;
    m_lastX = EDIT_NO_CELL;
}

i32 clearManager::Open(i32 priority) {
    i32 brush;

    for (brush = 0; brush < EDIT_BRUSH_COUNT; brush++) {
        m_brushButtons[brush] = new iconWidget(
            EDIT_BRUSH_BUTTON_X + brush * EDIT_BRUSH_BUTTON_STEP,
            EDIT_BRUSH_BUTTON_Y,
            EDIT_BRUSH_BUTTON_WIDTH,
            EDIT_BRUSH_BUTTON_HEIGHT,
            "editbtns.icn",
            EDIT_BRUSH_FRAME_FIRST + brush * EDIT_BUTTON_FRAMES + (brush == gClearBrush),
            ICON_DRAW_NORMAL,
            EDIT_BRUSH_BUTTON_ID_FIRST + brush,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        gEditManager->m_window->AddWidget(m_brushButtons[brush], -1);
    }
    gEditManager->m_window->DrawWindow();
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "clearManager");
    return 0;
}

void clearManager::Close(void) {
    i32 brush;

    for (brush = 0; brush < EDIT_BRUSH_COUNT; brush++) {
        gEditManager->m_window->RemoveWidget(m_brushButtons[brush]);
        delete m_brushButtons[brush];
    }
    if (gSelectionX != EDIT_NO_CELL) {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
        gEditManager->DrawMap();
    }
    gEditManager->m_window->DrawWindow();
    m_active = false;
}

void clearManager::UpdateBrushButtons(void) {
    tag_message msg;
    i32 brush;

    for (brush = 0; brush < EDIT_BRUSH_COUNT; brush++) {
        msg.type = MESSAGE_WIDGET;
        msg.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        msg.payload.widget.id = EDIT_BRUSH_BUTTON_ID_FIRST + brush;
        msg.payload.widget.data.value
            = EDIT_BRUSH_FRAME_FIRST + brush * EDIT_BUTTON_FRAMES + (brush == gClearBrush);
        gEditManager->m_window->BroadcastMessage(msg);
    }
    gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    gpWindowManager->UpdateScreenRegion(
        EDIT_TOOL_PANEL_X, EDIT_TOOL_PANEL_Y,
        EDIT_TOOL_PANEL_WIDTH, EDIT_TOOL_PANEL_HEIGHT);
}

MessageDispatchResult clearManager::Main(tag_message& message) {
    i32 anchorY;
    i32 help;
    i32 x;
    i32 y;
    i32 unusedMask [[maybe_unused]];
    tag_message event;
    i32 anchorX;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK:
                    if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                        help = CLEAR_HELP_NONE;
                        switch (message.payload.widget.id) {
                            case EDIT_BRUSH_BUTTON_ID_FIRST:
                                help = EDIT_BRUSH_SINGLE;
                                break;
                            case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_DOUBLE):
                                help = EDIT_BRUSH_DOUBLE;
                                break;
                            case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_QUADRUPLE):
                                help = EDIT_BRUSH_QUADRUPLE;
                                break;
                            case EDIT_BRUSH_BUTTON_ID_LAST:
                                help = EDIT_BRUSH_AREA;
                                break;
                        }
                        if (help >= 0)
                            NormalDialog(gObjectPanelHelp[help], NORMAL_DIALOG_QUICK_VIEW);
                        break;
                    }
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
                                        if (gClearBrush <= EDIT_BRUSH_QUADRUPLE) {


                                            gEditManager->ClearArea(
                                                x, y, gClearBrush + 1, gClearBrush + 1,
                                                0, false, false);
                                            OutlineBrush(gClearBrush, x, y);
                                            gEditManager->DrawMap();
                                            gEditManager->UpdateMapView();
                                            gEditManager->DrawRadar(true);
                                        } else {
                                            gSelectionX = x < anchorX ? x : anchorX;
                                            gSelectionY = y < anchorY ? y : anchorY;
                                            gSelectionWidth = abs(x - anchorX) + 1;
                                            gSelectionHeight = abs(y - anchorY) + 1;
                                            gEditManager->DrawMap();
                                            gEditManager->UpdateMapView();
                                            gEditManager->DrawRadar(true);
                                        }
                                    }
                                }
                                event = gpInputManager->GetEvent();
                            }
                            if (gClearBrush <= EDIT_BRUSH_QUADRUPLE)
                                gEditManager->ClearArea(
                                    anchorX, anchorY, gClearBrush + 1, gClearBrush + 1,
                                    0, false, false);
                            else
                                gEditManager->ClearArea(
                                    gSelectionX, gSelectionY,
                                    gSelectionWidth, gSelectionHeight, 0, false, false);
                            gSelectionX = gSelectionY = EDIT_NO_CELL;
                            gEditManager->DrawMap();
                            gEditManager->UpdateMapView();
                            gEditManager->DrawRadar(true);
                            m_lastY = EDIT_NO_CELL;
                            m_lastX = EDIT_NO_CELL;
                            gEditManager->m_mapChanged = true;
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case EDIT_BRUSH_BUTTON_ID_FIRST:
                        case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_DOUBLE):
                        case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_QUADRUPLE):
                        case EDIT_BRUSH_BUTTON_ID_LAST:
                            gClearBrush = message.payload.widget.id - EDIT_BRUSH_BUTTON_ID_FIRST;
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
            if ((message.payload.keyboard.keyCode == INPUT_SCAN_1
                 || message.payload.keyboard.keyCode == INPUT_SCAN_2
                 || message.payload.keyboard.keyCode == INPUT_SCAN_3
                 || message.payload.keyboard.keyCode == INPUT_SCAN_4)
                && !(((message.payload.keyboard.modifiers) & (MESSAGE_MODIFIER_CONTROL_KEYS)))) {
                gClearBrush = message.payload.keyboard.keyCode - INPUT_SCAN_1;
                TrackCursor();
                UpdateBrushButtons();
                return MESSAGE_DISPATCH_CONSUME;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

void clearManager::TrackCursor(void) {
    i32 mapX;
    tag_message pending;
    i32 mapY;

    gpMouseManager->MouseCoords(mapX, mapY);
    gEditManager->ScreenToCell(mapX, mapY);
    mapX += gEditManager->m_viewX;
    mapY += gEditManager->m_viewY;
    if (gEditManager->m_cursorX != mapX || gEditManager->m_cursorY != mapY
        || gEditManager->m_cursorSize != gClearBrush || gClearCursorMoves) {
        OutlineBrush(gClearBrush, mapX, mapY);
        pending = gpInputManager->PeekEvent();
        if (pending.type == MESSAGE_MOUSE_MOVE) {
            gClearCursorMoves++;
            if (gClearCursorMoves < EDIT_CURSOR_REDRAW_INTERVAL)
                return;
        }
        gClearCursorMoves = 0;
        gEditManager->DrawMap();
        gEditManager->UpdateMapView();
        if (gEditManager->m_cursorX != mapX || gEditManager->m_cursorY != mapY) {
            gEditManager->m_cursorX = mapX;
            gEditManager->m_cursorY = mapY;
            gEditManager->UpdateCursor();
        }
        gEditManager->m_cursorSize = gClearBrush;
    }
}

void clearManager::OutlineBrush(i32 brush, i32 x, i32 y) {
    if (brush == EDIT_BRUSH_SINGLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_SINGLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_SINGLE_CELLS;
    } else if (brush == EDIT_BRUSH_DOUBLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_DOUBLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_DOUBLE_CELLS;
    } else if (brush == EDIT_BRUSH_QUADRUPLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_QUADRUPLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_QUADRUPLE_CELLS;
    } else {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
    }
}
