

#include <H2/Ints.h>
#include <EDITOR/terrainManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/clearManager.h>
#include <EDITOR/EDITOR.h>
#include <BASE/border.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/IconDraw.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <BASE/textWidget.h>
#include <BASE/widgetKind.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

i32 gTerrainBrush = EDIT_BRUSH_DOUBLE;
TerrainButtonPosition gTerrainButtonPositions[(TERRAIN_COUNT)] = {
    {0x1fe, 0xf3},
    {0x21b, 0xf3},
    {0x238, 0xf3},
    {0x1fe, 0x110},
    {0x21b, 0x110},
    {0x238, 0x110},
    {0x1fe, 0x12d},
    {0x21b, 0x12d},
    {0x238, 0x12d}
};
i32 gTerrainChoice;
i32 gTerrainCursorMoves;

terrainManager::terrainManager(void)
    : m_terrain(TERRAIN_WATER), m_lastX(EDIT_NO_CELL), m_lastY(EDIT_NO_CELL) {}

i32 terrainManager::Open(i32 priority) {
    i32 i;
    char* name;

    for (i = 0; i < EDIT_BRUSH_COUNT; i++) {
        m_brushButtons[i] = new iconWidget(
            EDIT_BRUSH_BUTTON_X + i * EDIT_BRUSH_BUTTON_STEP,
            EDIT_BRUSH_BUTTON_Y,
            EDIT_BRUSH_BUTTON_WIDTH,
            EDIT_BRUSH_BUTTON_HEIGHT,
            "editbtns.icn",
            EDIT_BRUSH_FRAME_FIRST + i * EDIT_BUTTON_FRAMES + (i == gTerrainBrush),
            ICON_DRAW_NORMAL,
            EDIT_BRUSH_BUTTON_ID_FIRST + i,
            WIDGET_KIND_ICON_DIRECT,
            1
        );
        gEditManager->m_window->AddWidget(m_brushButtons[i], -1);
    }
    for (i = 0; i < (TERRAIN_COUNT); i++) {
        m_terrainButtons[i] = new border(
            gTerrainButtonPositions[i].x,
            gTerrainButtonPositions[i].y,
            TERRAIN_BUTTON_SIZE,
            TERRAIN_BUTTON_SIZE,
            TERRAIN_BUTTON_ID_FIRST + i,
            WIDGET_KIND_TRANSPARENT,
            0,
            NULL
        );
        gEditManager->m_window->AddWidget(m_terrainButtons[i], -1);
    }
    name = new char[TERRAIN_NAME_TEXT_SIZE];
    strcpy(name, "");
    m_terrainName = new textWidget(
        TERRAIN_NAME_X,
        TERRAIN_NAME_Y,
        TERRAIN_NAME_WIDTH,
        TERRAIN_NAME_HEIGHT,
        name,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        TERRAIN_NAME_ID,
        WIDGET_KIND_UNDIMMED,
        FONT_ALIGN_CENTER
    );
    gEditManager->m_window->AddWidget(m_terrainName, -1);
    m_highlight = new iconWidget(
        gTerrainButtonPositions[(m_terrain)].x - TERRAIN_HIGHLIGHT_INSET,
        gTerrainButtonPositions[(m_terrain)].y - TERRAIN_HIGHLIGHT_INSET,
        0,
        0,
        "terrains.icn",
        TERRAIN_HIGHLIGHT_FRAME,
        ICON_DRAW_NORMAL,
        TERRAIN_HIGHLIGHT_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    gEditManager->m_window->AddWidget(m_highlight, -1);
    gEditManager->m_placedY = EDIT_NO_CELL;
    gEditManager->m_placedX = EDIT_NO_CELL;
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "terrainManager");
    SelectTerrain(gTerrainChoice);
    return 0;
}

void terrainManager::Close(void) {
    i32 i;

    for (i = 0; i < EDIT_BRUSH_COUNT; i++) {
        gEditManager->m_window->RemoveWidget(m_brushButtons[i]);
        delete m_brushButtons[i];
    }
    for (i = 0; i < (TERRAIN_COUNT); i++) {
        gEditManager->m_window->RemoveWidget(m_terrainButtons[i]);
        delete m_terrainButtons[i];
    }
    gEditManager->m_window->RemoveWidget(m_terrainName);
    delete m_terrainName;
    gEditManager->m_window->RemoveWidget(m_highlight);
    delete m_highlight;
    if (gSelectionX != EDIT_NO_CELL) {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
        gEditManager->DrawMap();
    }
    gEditManager->m_window->DrawWindow();
    m_active = false;
}

void terrainManager::UpdateButtons(void) {
    tag_message message;
    i32 i;

    for (i = 0; i < EDIT_BRUSH_COUNT; i++) {
        message.type = MESSAGE_WIDGET;
        message.payload.widget.command = WIDGET_COMMAND_SET_FRAME;
        message.payload.widget.id = EDIT_BRUSH_BUTTON_ID_FIRST + i;
        message.payload.widget.data.value
            = EDIT_BRUSH_FRAME_FIRST + i * EDIT_BUTTON_FRAMES + (i == gTerrainBrush);
        gEditManager->m_window->BroadcastMessage(message);
    }
    sprintf(gText, gTerrainNames[(m_terrain)]);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = TERRAIN_NAME_ID;
    message.payload.widget.data.text = gText;
    gEditManager->m_window->BroadcastMessage(message);
    gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    gpWindowManager->UpdateScreenRegion(
        EDIT_TOOL_PANEL_X, EDIT_TOOL_PANEL_Y,
        EDIT_TOOL_PANEL_WIDTH, EDIT_TOOL_PANEL_HEIGHT);
}

i32 terrainManager::GetBrushSize(void) {
    if (gTerrainBrush == EDIT_BRUSH_QUADRUPLE)
        return TERRAIN_BRUSH_SIZE_QUADRUPLE;
    else if (gTerrainBrush == EDIT_BRUSH_DOUBLE)
        return TERRAIN_BRUSH_SIZE_DOUBLE;
    else if (gTerrainBrush == EDIT_BRUSH_SINGLE)
        return TERRAIN_BRUSH_SIZE_SINGLE;
    else
        return TERRAIN_BRUSH_SIZE_AREA;
}

void terrainManager::OutlineBrush(i32 size, i32 x, i32 y) {
    if (size == TERRAIN_BRUSH_SIZE_SINGLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_SINGLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_SINGLE_CELLS;
    } else if (size == TERRAIN_BRUSH_SIZE_DOUBLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_DOUBLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_DOUBLE_CELLS;
    } else if (size == TERRAIN_BRUSH_SIZE_QUADRUPLE) {
        gSelectionX = x;
        gSelectionY = y;
        gSelectionWidth = EDIT_BRUSH_QUADRUPLE_CELLS;
        gSelectionHeight = EDIT_BRUSH_QUADRUPLE_CELLS;
    } else {
        gSelectionX = gSelectionY = EDIT_NO_CELL;
    }
}

void terrainManager::TrackCursor(void) {
    i32 x;
    i32 y;
    tag_message nextEvent;

    gpMouseManager->MouseCoords(x, y);
    gEditManager->ScreenToCell(x, y);
    x += gEditManager->m_viewX;
    y += gEditManager->m_viewY;
    if (gEditManager->m_cursorX != x || gEditManager->m_cursorY != y
        || gEditManager->m_cursorSize != gEditManager->m_brushSize || gTerrainCursorMoves) {
        OutlineBrush(gEditManager->m_brushSize, x, y);
        nextEvent = gpInputManager->PeekEvent();
        if (nextEvent.type == MESSAGE_MOUSE_MOVE) {
            gTerrainCursorMoves++;
            if (gTerrainCursorMoves < EDIT_CURSOR_REDRAW_INTERVAL)
                return;
        }
        gTerrainCursorMoves = 0;
        gEditManager->DrawMap();
        gEditManager->UpdateMapView();
        if (gEditManager->m_cursorX != x || gEditManager->m_cursorY != y) {
            gEditManager->m_cursorX = x;
            gEditManager->m_cursorY = y;
            gEditManager->UpdateCursor();
        }
        gEditManager->m_cursorSize = gEditManager->m_brushSize;
    }
}

MessageDispatchResult terrainManager::Main(tag_message& message) {
    tag_message peek;
    i32 help;
    i32 anchorY;
    b32 first;
    i32 x;
    i32 y;
    i32 terrain;
    tag_message event;
    i32 oldSize;
    i32 height;
    i32 width;
    i32 anchorX;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
                        break;
                    switch (message.payload.widget.id) {
                        case EDIT_BRUSH_BUTTON_ID_FIRST:
                        case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_DOUBLE):
                        case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_QUADRUPLE):
                        case EDIT_BRUSH_BUTTON_ID_LAST:
                            gTerrainBrush
                                = message.payload.widget.id - EDIT_BRUSH_BUTTON_ID_FIRST;
                            gEditManager->m_brushSize = GetBrushSize();
                            UpdateButtons();
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK:
                    if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON)))) {
                        help = TERRAIN_HELP_NONE;
                        switch (message.payload.widget.id) {
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_WATER):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_WATER);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_GRASS):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_GRASS);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_SNOW):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_SNOW);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_SWAMP):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_SWAMP);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_LAVA):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_LAVA);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_DESERT):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_DESERT);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_DIRT):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_DIRT);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_WASTELAND):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_WASTELAND);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_BEACH):
                                help = TERRAIN_HELP_FIRST_TERRAIN + (TERRAIN_BEACH);
                                break;
                            case EDIT_BRUSH_BUTTON_ID_FIRST:
                                help = TERRAIN_HELP_FIRST_BRUSH + (EDIT_BRUSH_SINGLE);
                                break;
                            case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_DOUBLE):
                                help = TERRAIN_HELP_FIRST_BRUSH + (EDIT_BRUSH_DOUBLE);
                                break;
                            case EDIT_BRUSH_BUTTON_ID_FIRST + (EDIT_BRUSH_QUADRUPLE):
                                help = TERRAIN_HELP_FIRST_BRUSH + (EDIT_BRUSH_QUADRUPLE);
                                break;
                            case EDIT_BRUSH_BUTTON_ID_LAST:
                                help = TERRAIN_HELP_FIRST_BRUSH + (EDIT_BRUSH_AREA);
                                break;
                        }
                        if (help >= 0)
                            NormalDialog(gTerrainHelp[help], NORMAL_DIALOG_QUICK_VIEW);
                    } else {
                        switch (message.payload.widget.id) {
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_WATER):
                                SelectTerrain(TERRAIN_WATER);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_GRASS):
                                SelectTerrain(TERRAIN_GRASS);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_SNOW):
                                SelectTerrain(TERRAIN_SNOW);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_SWAMP):
                                SelectTerrain(TERRAIN_SWAMP);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_LAVA):
                                SelectTerrain(TERRAIN_LAVA);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_DESERT):
                                SelectTerrain(TERRAIN_DESERT);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_DIRT):
                                SelectTerrain(TERRAIN_DIRT);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_WASTELAND):
                                SelectTerrain(TERRAIN_WASTELAND);
                                break;
                            case TERRAIN_BUTTON_ID_FIRST + (TERRAIN_BEACH):
                                SelectTerrain(TERRAIN_BEACH);
                                break;
                            case EDIT_CONTROL_MAP:
                                if ((((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
                                    terrain = (TERRAIN_WATER);
                                else
                                    terrain = (m_terrain);
                                gSelectionX = EDIT_NO_CELL;
                                gEditManager->m_brushSize = GetBrushSize();
                                anchorX = message.payload.widget.screenX;
                                anchorY = message.payload.widget.screenY;
                                gEditManager->ScreenToCell(anchorX, anchorY);
                                anchorX += gEditManager->m_viewX;
                                anchorY += gEditManager->m_viewY;
                                gEditManager->SaveUndo();
                                first = true;
                                gTerrainCursorMoves = 0;
                                event = gpInputManager->GetEvent();
                                while (event.type != MESSAGE_LEFT_BUTTON_UP
                                       && event.type != MESSAGE_RIGHT_BUTTON_UP) {
                                    Process1WindowsMessage();
                                    gpMouseManager->Main(event);
                                    if (event.type == MESSAGE_MOUSE_MOVE || first) {
                                        first = false;
                                        if (event.type == MESSAGE_MOUSE_MOVE) {
                                            x = event.payload.mouse.screenX;
                                            y = event.payload.mouse.screenY;
                                        } else {
                                            x = message.payload.widget.screenX;
                                            y = message.payload.widget.screenY;
                                        }
                                        gEditManager->ScreenToCell(x, y);
                                        x += gEditManager->m_viewX;
                                        y += gEditManager->m_viewY;
                                        if (x != m_lastX || y != m_lastY) {
                                            m_lastX = x;
                                            m_lastY = y;
                                            switch (gEditManager->m_brushSize) {
                                                case TERRAIN_BRUSH_SIZE_SINGLE:
                                                    OutlineBrush(gEditManager->m_brushSize, x, y);
                                                    gEditManager->PaintGround(
                                                        x - gEditManager->m_viewX,
                                                        y - gEditManager->m_viewY,
                                                        EDIT_BRUSH_SINGLE_CELLS, EDIT_BRUSH_SINGLE_CELLS,
                                                        terrain);
                                                    break;
                                                case TERRAIN_BRUSH_SIZE_DOUBLE:
                                                    OutlineBrush(gEditManager->m_brushSize, x, y);
                                                    width = x < MAP_WIDTH - 1 ? EDIT_BRUSH_DOUBLE_CELLS : 1;
                                                    height = y < MAP_HEIGHT - 1 ? EDIT_BRUSH_DOUBLE_CELLS : 1;
                                                    gEditManager->PaintGround(
                                                        x - gEditManager->m_viewX,
                                                        y - gEditManager->m_viewY,
                                                        width, height, terrain);
                                                    break;
                                                case TERRAIN_BRUSH_SIZE_QUADRUPLE:
                                                    OutlineBrush(gEditManager->m_brushSize, x, y);
                                                    width = x < MAP_WIDTH - (EDIT_BRUSH_QUADRUPLE_CELLS - 1)
                                                                ? EDIT_BRUSH_QUADRUPLE_CELLS
                                                                : MAP_WIDTH - x;
                                                    height = y < MAP_HEIGHT - (EDIT_BRUSH_QUADRUPLE_CELLS - 1)
                                                                 ? EDIT_BRUSH_QUADRUPLE_CELLS
                                                                 : MAP_HEIGHT - y;
                                                    gEditManager->PaintGround(
                                                        x - gEditManager->m_viewX,
                                                        y - gEditManager->m_viewY,
                                                        width, height, terrain);
                                                    break;
                                                case TERRAIN_BRUSH_SIZE_AREA:
                                                    gSelectionX = x < anchorX ? x : anchorX;
                                                    gSelectionY = y < anchorY ? y : anchorY;
                                                    gSelectionWidth = abs(x - anchorX) + 1;
                                                    gSelectionHeight = abs(y - anchorY) + 1;
                                                    break;
                                            }
                                            gTerrainCursorMoves++;
                                            peek = gpInputManager->PeekEvent();
                                            while (peek.type == MESSAGE_KEY_DOWN) {
                                                peek = gpInputManager->GetEvent();
                                                peek = gpInputManager->PeekEvent();
                                            }
                                            if (peek.type == MESSAGE_MOUSE_MOVE
                                                && gTerrainCursorMoves
                                                       < TERRAIN_PAINT_REDRAW_INTERVAL)
                                                goto nextEvent;
                                            if (gTerrainCursorMoves) {
                                                gTerrainCursorMoves = 0;
                                                gEditManager->DrawMap();
                                                gEditManager->UpdateMapView();
                                            }
                                        }
                                    }
                                nextEvent:
                                    event = gpInputManager->GetEvent();
                                }
                                if (gEditManager->m_brushSize == TERRAIN_BRUSH_SIZE_AREA
                                    && gSelectionX >= 0)
                                    gEditManager->FillGround(
                                        gSelectionX, gSelectionY,
                                        gSelectionWidth, gSelectionHeight, terrain);
                                gSelectionX = gSelectionY = EDIT_NO_CELL;
                                gEditManager->BlendTerrain((m_terrain), false, true, false, false);
                                gEditManager->DrawMap();
                                gEditManager->UpdateMapView();
                                gEditManager->DrawRadar(true);
                                m_lastY = EDIT_NO_CELL;
                                m_lastX = EDIT_NO_CELL;
                                gEditManager->m_mapChanged = true;
                                break;
                        }
                    }
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                gEditManager->m_brushSize = GetBrushSize();
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
                oldSize = gEditManager->m_brushSize;
                gTerrainBrush = message.payload.keyboard.keyCode - INPUT_SCAN_1;
                gEditManager->m_brushSize = GetBrushSize();
                if (oldSize != gEditManager->m_brushSize)
                    TrackCursor();
                UpdateButtons();
                return MESSAGE_DISPATCH_CONSUME;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}


void terrainManager::Idle(void) {}

void terrainManager::SelectTerrain(TerrainType terrain) {
    m_terrain = terrain;
    m_highlight->m_x = gTerrainButtonPositions[(m_terrain)].x - TERRAIN_HIGHLIGHT_INSET;
    m_highlight->m_y = gTerrainButtonPositions[(m_terrain)].y - TERRAIN_HIGHLIGHT_INSET;
    gTerrainChoice = terrain;
    UpdateButtons();
}
