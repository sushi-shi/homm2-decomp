

#include <H2/Ints.h>
#include <EDITOR/line.h>
#include <EDITOR/lineManager.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/OVERLAY.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/icon.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <string.h>

typedef enum LineToolConstant {

    LINE_CURSOR_COLOR     = 0xb5,

    LINE_MAP_MARGIN       = 4,
    LINE_DRAW_MARGIN      = 2,


    LINE_ROAD_MAP_MARGIN    = 3,
    LINE_ROAD_DRAW_MARGIN   = 2,
    LINE_STREAM_MAP_MARGIN  = 2,
    LINE_STREAM_DRAW_MARGIN = 1,

    LINE_VARIANT_PERCENT  = 50
} LineToolConstant;

typedef enum RoadTile {


    ROAD_TILE_PLAIN          = 0,
    ROAD_TILE_FIRST_VARIANT  = 1,
    ROAD_TILE_SECOND_VARIANT = 2,
    ROAD_TILE_FORK_LEFT      = 9,
    ROAD_TILE_FORK_RIGHT     = 0xc,
    ROAD_TILE_TURN_RIGHT     = 0x11,
    ROAD_TILE_TURN_LEFT      = 0x12,
    ROAD_TILE_PLAIN_ALT      = 0x1a,
    ROAD_TILE_FIRST_ALT      = 0x1b,
    ROAD_TILE_SECOND_ALT     = 0x1c,
    ROAD_TILE_TURN_RIGHT_ALT = 0x1d,
    ROAD_TILE_TURN_LEFT_ALT  = 0x1e,

    ROAD_TILE_GATE           = 0x1f
} RoadTile;

typedef enum StreamNeighbour {

    STREAM_RIGHT = 1,
    STREAM_DOWN  = 2,
    STREAM_LEFT  = 4,
    STREAM_UP    = 8
} StreamNeighbour;

typedef enum StreamTile {
    STREAM_TILE_BEND         = 2,
    STREAM_TILE_STRAIGHT     = 3,
    STREAM_TILE_BEND_ALT     = 5,
    STREAM_TILE_STRAIGHT_ALT = 0xc
} StreamTile;

TilesetId gLineTileset = TILESET_ROAD;
i32 gLineOverlayFirst = LINE_ROAD_OVERLAY_FIRST;
u8* gLineMap;
i32 gLineType;

lineManager::lineManager(void) {
    m_lastY = EDIT_NO_CELL;
    m_lastX = EDIT_NO_CELL;
}

i32 lineManager::Open(i32 priority) {
    m_cursorIcon = gpResourceManager->GetIcon("overlay.icn");
    gEditManager->m_window->DrawWindow();
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "lineManager");
    return 0;
}

void lineManager::Close(void) {
    gEditManager->m_window->DrawWindow();
    gpResourceManager->Dispose(m_cursorIcon);
    m_active = false;
}

MessageDispatchResult lineManager::Main(tag_message& message) {
    mapCell* targetCell [[maybe_unused]];
    i32 newY;
    i32 newX;
    tag_message event;
    b32 redraw;
    i32 y;
    i32 x;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                    if ((H2EnumIndex((message.payload.widget.modifiers) & (MESSAGE_MODIFIER_RIGHT_BUTTON))))
                        break;
                    switch (message.payload.widget.id) {
                        case EDIT_CONTROL_MAP:
                            redraw = true;
                            event = message;
                            gEditManager->SaveUndo();
                            while (event.type != MESSAGE_LEFT_BUTTON_UP
                                   && event.type != MESSAGE_RIGHT_BUTTON_UP) {
                                Process1WindowsMessage();
                                gpMouseManager->Main(event);
                                if (event.type == MESSAGE_MOUSE_MOVE || redraw) {
                                    redraw = false;
                                    x = event.payload.mouse.screenX;
                                    y = event.payload.mouse.screenY;
                                    gEditManager->ScreenToCell(x, y);
                                    x += gEditManager->m_viewX;
                                    y += gEditManager->m_viewY;
                                    if (x != m_lastX || y != m_lastY) {
                                        m_lastX = x;
                                        m_lastY = y;
                                        AddLineCell(x, y);
                                        gSelectionX = x;
                                        gSelectionY = y;
                                        gSelectionWidth = EDIT_BRUSH_SINGLE_CELLS;
                                        gSelectionHeight = EDIT_BRUSH_SINGLE_CELLS;
                                        gEditManager->DrawMap();
                                        gEditManager->UpdateMapView();
                                        gEditManager->DrawRadar(true);
                                    }
                                }
                                event = gpInputManager->GetEvent();
                            }
                            gSelectionX = EDIT_NO_CELL;
                            gEditManager->DrawMap();
                            gEditManager->UpdateMapView();
                            m_lastY = EDIT_NO_CELL;
                            m_lastX = EDIT_NO_CELL;
                            gEditManager->m_mapChanged = true;
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                newX = message.payload.mouse.screenX;
                newY = message.payload.mouse.screenY;
                gEditManager->ScreenToCell(newX, newY);
                newX += gEditManager->m_viewX;
                newY += gEditManager->m_viewY;
                if (gEditManager->m_cursorX != newX || gEditManager->m_cursorY != newY) {
                    gEditManager->m_cursorX = newX;
                    gEditManager->m_cursorY = newY;
                    targetCell = gMap.GetCell(newX, newY);
                    newX -= gEditManager->m_viewX;
                    newY -= gEditManager->m_viewY;
                    newX = newX * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_LEFT;
                    newY = newY * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_TOP;
                    gEditManager->DrawMap();
                    m_cursorIcon->FillToBuffer(newX, newY, gEditManager->m_zoomLevel,
                                               LINE_CURSOR_COLOR, ICON_DRAW_NORMAL, NULL);
                    gEditManager->UpdateMapView();
                    gEditManager->UpdateCursor();
                }
            }
            return MESSAGE_DISPATCH_CONSUME;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

void SetLineType(i32 type) {
    switch (type) {
        case LINE_ROAD:
            gLineType = LINE_ROAD;
            gLineTileset = TILESET_ROAD;
            gLineOverlayFirst = LINE_ROAD_OVERLAY_FIRST;
            break;
        case LINE_STREAM:
            gLineType = LINE_STREAM;
            gLineTileset = TILESET_STREAM;
            gLineOverlayFirst = LINE_STREAM_OVERLAY_FIRST;
            break;
    }
}

void AddLineCell(i32 x, i32 y) {
    if (x < 0 || x > MAP_WIDTH - 1 || y < 0 || y > MAP_HEIGHT - 1
        || CELL_TERRAIN(gMap.GetCell(x, y)) == TERRAIN_WATER
        || (y > 0 && CELL_TERRAIN(gMap.GetCell(x, y - 1)) == TERRAIN_WATER))
        return;
    BuildLineMap(x - LINE_MAP_MARGIN, y - LINE_MAP_MARGIN, x + LINE_MAP_MARGIN, y + LINE_MAP_MARGIN,
                 false);
    LINE_MAP_AT(x, y)++;
    DrawLines(x - LINE_DRAW_MARGIN, y - LINE_DRAW_MARGIN, x + LINE_DRAW_MARGIN, y + LINE_DRAW_MARGIN);
}

b32 IsLineTile(TilesetId tileset, i32 index, b32 alternate) {
    switch (gLineType) {
        case LINE_ROAD:
            return tileset == TILESET_ROAD
                   && ((alternate && gRoadTileJoinsAlt[index]) || (!alternate && gRoadTileJoins[index]));
        case LINE_STREAM:
            return tileset == TILESET_STREAM;
    }
    return false;
}

void BuildLineMap(i32 fromX, i32 fromY, i32 toX, i32 toY, b32 alternate) {
    mapCell* cell;
    i32 y;
    i32 unusedIndex [[maybe_unused]];
    i32 x;
    mapCellExtra* extra;

    if (gLineMap)
        delete[] gLineMap;
    gLineMap = NULL;
    gLineMap = new u8[MAP_WIDTH * MAP_HEIGHT];
    memset(gLineMap, 0, MAP_WIDTH * MAP_HEIGHT);
    if (fromX < 0)
        fromX = 0;
    if (fromY < 0)
        fromY = 0;
    if (toX > MAP_WIDTH - 1)
        toX = MAP_WIDTH - 1;
    if (toY > MAP_HEIGHT - 1)
        toY = MAP_HEIGHT - 1;
    for (x = fromX; x <= toX; x++) {
        for (y = fromY; y <= toY; y++) {
            cell = gMap.GetCell(x, y);
            if (cell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                if (IsLineTile(cell->m_objectTileset, cell->m_objectIndex, alternate)) {
                    LINE_MAP_AT(x, y)++;
                    goto nextCell;
                }
                if (cell->m_extraIndex && gMap.extras[cell->m_extraIndex].objectIndex != MAPCELL_SPRITE_NONE)
                    extra = &gMap.extras[cell->m_extraIndex];
                else
                    extra = NULL;
                while (extra) {
                    if (IsLineTile(extra->objectTileset, extra->objectIndex, false)) {
                        LINE_MAP_AT(x, y)++;
                        goto nextCell;
                    }
                    if (extra->nextIndex && gMap.extras[extra->nextIndex].objectIndex != MAPCELL_SPRITE_NONE)
                        extra = &gMap.extras[extra->nextIndex];
                    else
                        extra = NULL;
                }
            }
        nextCell:;
        }
    }
}


void DrawRoads(i32 fromX, i32 fromY, i32 toX, i32 toY) {
    i32 unusedA [[maybe_unused]];
    const i32 up = 0x80;
    const i32 down = 0x40;
    const i32 left = 0x20;
    const i32 right = 0x10;
    const i32 upLeft = 8;
    const i32 downLeft = 4;
    const i32 downRight = 2;
    i32 x;
    i32 unused1 [[maybe_unused]];
    const i32 upRight = 1;
    i32 y;
    i32 unusedIndex [[maybe_unused]];
    i32 mask;
    mapCell* cell;

    if (fromX < 0)
        fromX = 0;
    if (fromY < 0)
        fromY = 0;
    if (toX > MAP_WIDTH - 1)
        toX = MAP_WIDTH - 1;
    if (toY > MAP_HEIGHT - 1)
        toY = MAP_HEIGHT - 1;
    for (x = fromX; x <= toX; x++) {
        for (y = fromY; y <= toY; y++) {
            cell = gMap.GetCell(x, y);
            if (CELL_TERRAIN(cell) == TERRAIN_WATER)
                continue;
            mask = 0;
            if (y > 0 && LINE_MAP_AT(x, y - 1))
                mask |= up;
            if (y < MAP_HEIGHT - 1 && LINE_MAP_AT(x, y + 1))
                mask |= down;
            if (x > 0 && LINE_MAP_AT(x - 1, y))
                mask |= left;
            if (x < MAP_WIDTH - 1 && LINE_MAP_AT(x + 1, y))
                mask |= right;
            if (y > 0 && x > 0 && LINE_MAP_AT(x - 1, y - 1))
                mask |= upLeft;
            if (y < MAP_HEIGHT - 1 && x > 0 && LINE_MAP_AT(x - 1, y + 1))
                mask |= downLeft;
            if (y < MAP_HEIGHT - 1 && x < MAP_WIDTH - 1 && LINE_MAP_AT(x + 1, y + 1))
                mask |= downRight;
            if (y > 0 && x < MAP_WIDTH - 1 && LINE_MAP_AT(x + 1, y - 1))
                mask |= upRight;
            if (LINE_MAP_AT(x, y)) {
                if ((mask & up) && (mask & upRight) && !(mask & down) && !(mask & left)
                    && !(mask & right) && y > 1 && !LINE_MAP_AT(x, y - 2))
                    SetLineTile(x, y, gLineTileset, ROAD_TILE_TURN_LEFT, LINE_NO_VARIANT);
                else if ((mask & up) && (mask & upLeft) && !(mask & down) && !(mask & left)
                         && !(mask & right) && y > 1 && !LINE_MAP_AT(x, y - 2))
                    SetLineTile(x, y, gLineTileset, ROAD_TILE_TURN_RIGHT, LINE_NO_VARIANT);
                else if ((mask & up) && (mask & down) && (mask & upRight) && !(mask & upLeft)
                         && !(mask & left) && !(mask & right) && y > 1
                         && !LINE_MAP_AT(x, y - 2))
                    SetLineTile(x, y, gLineTileset, ROAD_TILE_FORK_LEFT, LINE_NO_VARIANT);
                else if ((mask & up) && (mask & down) && (mask & upLeft) && !(mask & upRight)
                         && !(mask & left) && !(mask & right) && y > 1
                         && !LINE_MAP_AT(x, y - 2))
                    SetLineTile(x, y, gLineTileset, ROAD_TILE_FORK_RIGHT, LINE_NO_VARIANT);
                else
                    SetLineTile(x, y, gLineTileset, gLineEdgeTiles[mask], LINE_NO_VARIANT);
            } else {
                SetLineTile(x, y, gLineTileset, gLineTiles[mask], LINE_NO_VARIANT);
            }
        }
    }
}

void DrawStreams(i32 fromX, i32 fromY, i32 toX, i32 toY) {
    i32 unusedX [[maybe_unused]];
    i32 mask;
    i32 y;
    i32 unusedIndex [[maybe_unused]];
    i32 x;
    i32 unused1 [[maybe_unused]];
    i32 unused [[maybe_unused]];
    i32 variant;

    if (fromX < 0)
        fromX = 0;
    if (fromY < 0)
        fromY = 0;
    if (toX > MAP_WIDTH - 1)
        toX = MAP_WIDTH - 1;
    if (toY > MAP_HEIGHT - 1)
        toY = MAP_HEIGHT - 1;
    for (x = fromX; x <= toX; x++) {
        for (y = fromY; y <= toY; y++) {
            if (LINE_MAP_AT(x, y)) {
                mask = 0;
                if (y > 0 && LINE_MAP_AT(x, y - 1))
                    mask |= STREAM_UP;
                if (y < MAP_HEIGHT - 1 && LINE_MAP_AT(x, y + 1))
                    mask |= STREAM_DOWN;
                if (x > 0 && LINE_MAP_AT(x - 1, y))
                    mask |= STREAM_LEFT;
                if (x < MAP_WIDTH - 1 && LINE_MAP_AT(x + 1, y))
                    mask |= STREAM_RIGHT;
                variant = LINE_NO_VARIANT;
                if (gLineEndTiles[mask] == STREAM_TILE_STRAIGHT)
                    variant = STREAM_TILE_STRAIGHT_ALT;
                if (gLineEndTiles[mask] == STREAM_TILE_BEND)
                    variant = STREAM_TILE_BEND_ALT;
                SetLineTile(x, y, gLineTileset, gLineEndTiles[mask], variant);
            } else {
                SetLineTile(x, y, gLineTileset, LINE_NO_TILE, LINE_NO_VARIANT);
            }
        }
    }
}

void DrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY) {
    switch (gLineType) {
        case LINE_ROAD:
            DrawRoads(fromX, fromY, toX, toY);
            break;
        case LINE_STREAM:
            DrawStreams(fromX, fromY, toX, toY);
            break;
    }
}

void SetLineTile(i32 x, i32 y, TilesetId tileset, i32 index, i32 variant) {
    mapCellExtra* extra;
    mapCell* cell;

    cell = gMap.GetCell(x, y);
    if (tileset == TILESET_ROAD) {
        variant = LINE_NO_VARIANT;
        if (index == ROAD_TILE_PLAIN && y >= 2
            && (gMap.GetCell(x, y - 1)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)
                || gMap.GetCell(x, y - 1)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                || gMap.GetCell(x, y - 1)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE)))
            index = ROAD_TILE_GATE;
        if (index == ROAD_TILE_PLAIN)
            variant = ROAD_TILE_PLAIN_ALT;
        if (index == ROAD_TILE_FIRST_VARIANT)
            variant = ROAD_TILE_FIRST_ALT;
        if (index == ROAD_TILE_SECOND_VARIANT)
            variant = ROAD_TILE_SECOND_ALT;
        if (index == ROAD_TILE_TURN_RIGHT)
            variant = ROAD_TILE_TURN_RIGHT_ALT;
        if (index == ROAD_TILE_TURN_LEFT)
            variant = ROAD_TILE_TURN_LEFT_ALT;
    }
    if (cell->m_objectTileset == gLineTileset && cell->m_objectIndex != MAPCELL_SPRITE_NONE) {
        if (index == LINE_NO_TILE) {
            gEditManager->RemoveLinkedObject(cell->m_objectLink);
            return;
        }
        if (cell->m_objectIndex == index || cell->m_objectIndex == variant)
            return;
        gEditManager->RemoveLinkedObject(cell->m_objectLink);
    }
    if (cell->m_extraIndex && gMap.extras[cell->m_extraIndex].objectIndex != MAPCELL_SPRITE_NONE)
        extra = &gMap.extras[cell->m_extraIndex];
    else
        extra = NULL;
    while (extra) {
        if (extra->objectTileset == gLineTileset && extra->objectIndex != MAPCELL_SPRITE_NONE) {
            if (index == LINE_NO_TILE) {
                gEditManager->RemoveLinkedObject(extra->objectLink);
                return;
            }
            if (extra->objectIndex == index)
                return;
            gEditManager->RemoveLinkedObject(extra->objectLink);
        }
        if (extra->nextIndex && gMap.extras[extra->nextIndex].objectIndex != MAPCELL_SPRITE_NONE)
            extra = &gMap.extras[extra->nextIndex];
        else
            extra = NULL;
    }
    if (variant != LINE_NO_VARIANT && Random(0, 100) < LINE_VARIANT_PERCENT)
        index = variant;
    if (index != LINE_NO_TILE)
        PlaceOverlay(&gOverlayTypes[gLineOverlayFirst + index], x - OVERLAY_ANCHOR_X,
                     y - OVERLAY_ANCHOR_Y, true);
}

void RedrawLines(i32 fromX, i32 fromY, i32 toX, i32 toY) {
    i32 savedType;

    savedType = gLineType;
    SetLineType(LINE_ROAD);
    BuildLineMap(fromX - LINE_ROAD_MAP_MARGIN, fromY - LINE_ROAD_MAP_MARGIN,
                 toX + LINE_ROAD_MAP_MARGIN, toY + LINE_ROAD_MAP_MARGIN, false);
    DrawLines(fromX - LINE_ROAD_DRAW_MARGIN, fromY - LINE_ROAD_DRAW_MARGIN,
              toX + LINE_ROAD_DRAW_MARGIN, toY + LINE_ROAD_DRAW_MARGIN);
    SetLineType(LINE_STREAM);
    BuildLineMap(fromX - LINE_STREAM_MAP_MARGIN, fromY - LINE_STREAM_MAP_MARGIN,
                 toX + LINE_STREAM_MAP_MARGIN, toY + LINE_STREAM_MAP_MARGIN, false);
    DrawLines(fromX - LINE_STREAM_DRAW_MARGIN, fromY - LINE_STREAM_DRAW_MARGIN,
              toX + LINE_STREAM_DRAW_MARGIN, toY + LINE_STREAM_DRAW_MARGIN);
    SetLineType(savedType);
}
