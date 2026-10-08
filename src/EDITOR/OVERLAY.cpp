// The object tool (overlayManager) and the placement of objects on the map.
// The unit name comes from the Price of Loyalty editor's assertion path
// (overlay.cpp); Open stores the class name "overlayManager".

#include <match.h>
#include <EDITOR/OVERLAY.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/heroedit.h>
#include <EDITOR/townedit.h>
#include <BASE/border.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/icon2bs.h>
#include <BASE/icon2bsd.h>
#include <BASE/Iconm2b.h>
#include <BASE/IconDraw.h>
#include <BASE/IconEntry.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/textWidget.h>
#include <BASE/widgetKind.h>
#include <SOURCE/configTypes.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/gameTypes.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/hero.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(OverlayPlacementConstant)
    // A town's entrance and the grid offset its ground is read at; its
    // flags go 3 and 1 cells left and a row up. A dragged town reads its
    // ground two cells left of the pointer.
    OVERLAY_TOWN_ENTRANCE_X   = 5,
    OVERLAY_TOWN_ENTRANCE_Y   = 5,
    OVERLAY_LEFT_FLAG_X       = -3,
    OVERLAY_RIGHT_FLAG_X      = -1,
    OVERLAY_FLAG_Y            = -1,
    OVERLAY_TOWN_GROUND_COLUMN = 2,
    // A random town's faction in its map record.
    OVERLAY_RANDOM_TOWN_FACTION = 6,
    // An alchemist's lab may reach at most 3 rows above the map.
    OVERLAY_ALCHEMIST_LAB_TOP = -3,
    // The ultimate artifact stays this many cells inside the map.
    OVERLAY_ULTIMATE_MARGIN   = 9,
    // The map's limits on events (towns and mines take the game's).
    OVERLAY_EVENT_LIMIT       = 50,
    // CanPlaceOverlay's lists of the placement links an object would cover
    // and keep.
    OVERLAY_LINK_LIST_CAPACITY = 400
H2_ENUM_END(OverlayPlacementConstant)

H2_ENUM_BEGIN(OverlayManagerLayout)
    // gClearHelp's right-click help of the first class.
    OVERLAY_CLASS_HELP_FIRST      = 5,
    // The class buttons: transparent borders over the panel's swatches.
    OVERLAY_CLASS_BUTTON_SIZE     = 0x1b,
    OVERLAY_CLASS_BUTTON_ID_FIRST = 500,
    // The class name under the buttons.
    OVERLAY_CLASS_NAME_X          = 0x1ed,
    OVERLAY_CLASS_NAME_Y          = 0x16f,
    OVERLAY_CLASS_NAME_WIDTH      = 0x75,
    OVERLAY_CLASS_NAME_HEIGHT     = 10,
    OVERLAY_CLASS_NAME_ID         = 0x578,
    OVERLAY_CLASS_NAME_TEXT_SIZE  = 2,
    // The frame (terrains.icn) that outlines the selected class button.
    OVERLAY_HIGHLIGHT_SIZE        = 0x1f,
    OVERLAY_HIGHLIGHT_FRAME       = 9,
    OVERLAY_HIGHLIGHT_ID          = 0x19,
    OVERLAY_HIGHLIGHT_INSET       = 2,
    // objpalet.icn's panel backdrop and its place.
    OVERLAY_PANEL_X               = 0x1fd,
    OVERLAY_PANEL_Y               = 0xe3,
    OVERLAY_PANEL_FRAME           = 0,
    // The region the selected object's preview is drawn in.
    OVERLAY_PREVIEW_REGION_X      = 0x1fd,
    OVERLAY_PREVIEW_REGION_Y      = 0xc2,
    OVERLAY_PREVIEW_REGION_WIDTH  = 0x56,
    OVERLAY_PREVIEW_REGION_HEIGHT = 0x46,
    // A footprint cell's colour: an overlay part, a shadow, an entrance,
    // any other part.
    OVERLAY_COLOR_OVERLAY         = 0x65,
    OVERLAY_COLOR_SHADOW          = 0x24,
    OVERLAY_COLOR_ENTRANCE        = 0xcf,
    OVERLAY_COLOR_OBJECT          = 0xc4,
    // A town's flags on its grid cells (4, 4) and (6, 4).
    OVERLAY_TOWN_FLAG_COLUMN      = 4,
    OVERLAY_TOWN_RIGHT_FLAG_COLUMN = 6,
    OVERLAY_TOWN_FLAG_ROW         = 4
H2_ENUM_END(OverlayManagerLayout)

H2_ENUM_BEGIN(OverlayPickerLayout)
    // The picker (editpalt.bin): a 9 x 9 page of 69 x 53 boxes, an object
    // drawn 2 pixels in, in a 4 x 3 cell view.
    OVERLAY_PICKER_COLUMNS      = 9,
    OVERLAY_PICKER_ROWS         = 9,
    OVERLAY_PICKER_PAGE         = OVERLAY_PICKER_COLUMNS * OVERLAY_PICKER_ROWS,
    OVERLAY_PICKER_BOX_WIDTH    = 0x45,
    OVERLAY_PICKER_BOX_HEIGHT   = 0x35,
    OVERLAY_PICKER_OBJECT_INSET = 2,
    OVERLAY_PICKER_VIEW_WIDTH   = 4,
    OVERLAY_PICKER_VIEW_HEIGHT  = 3,
    // objpalet.icn's box frames: a plain box, then a colour's.
    OVERLAY_PICKER_PLAIN_BOX    = 1,
    OVERLAY_PICKER_COLOR_BOX    = 2,
    // A random hero's portrait frames: seven per colour.
    OVERLAY_HERO_FRAMES_PER_COLOR = 7,
    // The scroll track and knob (escroll.icn).
    OVERLAY_PICKER_TRACK_X      = 0x26e,
    OVERLAY_PICKER_TRACK_Y      = 0x10,
    OVERLAY_PICKER_TRACK_WIDTH  = 0x10,
    OVERLAY_PICKER_TRACK_HEIGHT = 0x1a0,
    OVERLAY_PICKER_TRACK_FRAME  = 1,
    OVERLAY_PICKER_TRACK_ID     = 0xb,
    OVERLAY_PICKER_KNOB_X       = 0x272,
    OVERLAY_PICKER_KNOB_Y       = 0x13,
    OVERLAY_PICKER_KNOB_WIDTH   = 8,
    OVERLAY_PICKER_KNOB_HEIGHT  = 0x11,
    OVERLAY_PICKER_KNOB_FRAME   = 3,
    OVERLAY_PICKER_KNOB_ID      = 0xd,
    // The knob's place without a scrollable page, and the track a knob
    // drag measures from. The picker draws at the middle zoom.
    OVERLAY_PICKER_KNOB_PARKED  = 0xd7,
    // A scrolling page's knob travels 393 pixels below OVERLAY_PICKER_KNOB_Y;
    // a drag scales the track's 402 pixels to the page's rows.
    OVERLAY_PICKER_KNOB_TRAVEL  = 393,
    OVERLAY_PICKER_KNOB_SCALE_SPAN = 402,
    OVERLAY_PICKER_ZOOM         = 1,
    OVERLAY_PICKER_DRAG_ORIGIN  = 0x27,
    // The picker's controls: the row arrows, the class arrows, the boxes.
    OVERLAY_PICKER_ROW_UP       = 100,
    OVERLAY_PICKER_ROW_DOWN     = 101,
    OVERLAY_PICKER_CLASS_PREVIOUS = 102,
    OVERLAY_PICKER_CLASS_NEXT   = 103,
    OVERLAY_PICKER_BOXES        = 0x6e
H2_ENUM_END(OverlayPickerLayout)

// The class buttons on the tool panel, water to beach then the other
// classes.
DATA(0x00498ca4) static ClassButtonPosition gClassButtonPositions[OVERLAY_CLASS_COUNT] = {
    {0x1ef, 0xf4},
    {0x20c, 0xf4},
    {0x229, 0xf4},
    {0x246, 0xf4},
    {0x1ef, 0x110},
    {0x20c, 0x110},
    {0x229, 0x110},
    {0x246, 0x110},
    {0x1ef, 0x12c},
    {0x20c, 0x12c},
    {0x229, 0x12c},
    {0x246, 0x12c},
    {0x1ef, 0x148},
    {0x246, 0x148}
};

DATA(0x00498cdc) i32 gSelectedOverlay = OVERLAY_NONE;

// CanPlaceOverlay's placement links: those the object would cover, and
// those whose parts would stay over or under it.
DATA(0x004a5420) static u16 gCoveredLinks[OVERLAY_LINK_LIST_CAPACITY];
DATA(0x004a5100) static u16 gKeptLinks[OVERLAY_LINK_LIST_CAPACITY];
// The picker's rows, its first shown entry and the entry it picked.
#define gPickerRows gPickerRowsRecord // spelling fixes .bss order
DATA(0x004a5740) static i32 gPickerRows;
#define gPickerFirst gPickerFirstValue // spelling fixes .bss order
DATA(0x004a5744) static i32 gPickerFirst;
#define gPickedOverlay gPickedOverlayCopy // spelling fixes .bss order
DATA(0x004a5748) static i32 gPickedOverlay;

VA(0x00418dc0, 0x67)
b32 OverlayGridHas(u32* grid, i32 x, i32 y) {
    i32 bit;

    bit = (OVERLAY_GRID_WIDTH - x - 1) + (OVERLAY_GRID_HEIGHT - y - 1) * OVERLAY_GRID_WIDTH;
    if (bit < OVERLAY_GRID_WORD_BITS)
        return (grid[OVERLAY_GRID_BOTTOM] & 1 << bit) != 0;
    return (grid[OVERLAY_GRID_TOP] & 1 << (bit - OVERLAY_GRID_WORD_BITS)) != 0;
}

VA(0x00418e27, 0x1f)
overlayManager::overlayManager(void) {}

VA(0x00418e46, 0x2f3)
i32 overlayManager::Open(i32 priority) {
    i32 i;
    char* name;

    m_previewDrawn = false;
    m_cellIcon = gpResourceManager->GetIcon("overlay.icn");
    m_paletteIcon = gpResourceManager->GetIcon("objpalet.icn");
    for (i = 0; i < OVERLAY_CLASS_COUNT; i++) {
        m_classButtons[i] = new border(
            gClassButtonPositions[i].x,
            gClassButtonPositions[i].y,
            OVERLAY_CLASS_BUTTON_SIZE,
            OVERLAY_CLASS_BUTTON_SIZE,
            OVERLAY_CLASS_BUTTON_ID_FIRST + i,
            WIDGET_KIND_TRANSPARENT,
            0,
            NULL
        );
        gEditManager->m_window->AddWidget(m_classButtons[i], -1);
    }
    name = new char[OVERLAY_CLASS_NAME_TEXT_SIZE];
    strcpy(name, "");
    m_className = new textWidget(
        OVERLAY_CLASS_NAME_X,
        OVERLAY_CLASS_NAME_Y,
        OVERLAY_CLASS_NAME_WIDTH,
        OVERLAY_CLASS_NAME_HEIGHT,
        name,
        "smalfont.fnt",
        FONT_DRAW_DEFAULT,
        OVERLAY_CLASS_NAME_ID,
        WIDGET_KIND_UNDIMMED,
        FONT_ALIGN_CENTER
    );
    gEditManager->m_window->AddWidget(m_className, -1);
    m_highlight = new iconWidget(
        gClassButtonPositions[0].x - OVERLAY_HIGHLIGHT_INSET,
        gClassButtonPositions[0].y - OVERLAY_HIGHLIGHT_INSET,
        OVERLAY_HIGHLIGHT_SIZE,
        OVERLAY_HIGHLIGHT_SIZE,
        "terrains.icn",
        OVERLAY_HIGHLIGHT_FRAME,
        ICON_DRAW_NORMAL,
        OVERLAY_HIGHLIGHT_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    gEditManager->m_window->AddWidget(m_highlight, -1);
    gEditManager->m_window->DrawWindow();
    m_paletteIcon
        ->DrawToBuffer(OVERLAY_PANEL_X, OVERLAY_PANEL_Y, OVERLAY_PANEL_FRAME, ICON_DRAW_NORMAL);
    ShowClass(false);
    gEditManager->m_placedY = EDIT_NO_CELL;
    gEditManager->m_placedX = EDIT_NO_CELL;
    LoadClass(gObjectClass);
    DrawSelectedOverlay();
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "overlayManager");
    return 0;
}

VA(0x00419139, 0x169)
void overlayManager::Close(void) {
    i32 i;

    gpResourceManager->Dispose(m_cellIcon);
    gpResourceManager->Dispose(m_paletteIcon);
    for (i = 0; i < OVERLAY_CLASS_COUNT; i++) {
        gEditManager->m_window->RemoveWidget(m_classButtons[i]);
        delete m_classButtons[i];
    }
    gEditManager->m_window->RemoveWidget(m_className);
    delete m_className;
    gEditManager->m_window->RemoveWidget(m_highlight);
    delete m_highlight;
    gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    m_active = false;
}

VA(0x004192a2, 0xcc)
void overlayManager::ShowClass(b32 update) {
    tag_message message;
    i32 H2_UNUSED(i);

    sprintf(gText, gObjectClassNames[gObjectClass]);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = OVERLAY_CLASS_NAME_ID;
    message.payload.widget.data.text = gText;
    gEditManager->m_window->BroadcastMessage(message);
    m_highlight->m_x = gClassButtonPositions[gObjectClass].x - OVERLAY_HIGHLIGHT_INSET;
    m_highlight->m_y = gClassButtonPositions[gObjectClass].y - OVERLAY_HIGHLIGHT_INSET;
    gEditManager->m_window->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    if (update)
        gpWindowManager->UpdateScreenRegion(
            EDIT_TOOL_PANEL_X,
            EDIT_TOOL_PANEL_Y,
            EDIT_TOOL_PANEL_WIDTH,
            EDIT_TOOL_PANEL_HEIGHT
        );
}

#define placed result // frame-slot spelling
VA(0x0041936e, 0x49a)
MessageDispatchResult overlayManager::Main(tag_message& message) {
    b32 placed;
    b32 finished;
    i32 H2_UNUSED(helpItem);
    tag_message peek;
    i32 mapX;
    i32 mapY;

    finished = false;
    helpItem = -1;
    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                case WIDGET_NOTIFY_RIGHT_CLICK:
                    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON)) {
                        if (message.payload.widget.id >= OVERLAY_CLASS_BUTTON_ID_FIRST
                            && message.payload.widget.id
                                   < OVERLAY_CLASS_BUTTON_ID_FIRST + IDX(OVERLAY_CLASS_COUNT))
                            NormalDialog(
                                gClearHelp
                                    [OVERLAY_CLASS_HELP_FIRST + message.payload.widget.id
                                     - OVERLAY_CLASS_BUTTON_ID_FIRST],
                                NORMAL_DIALOG_QUICK_VIEW
                            );
                        break;
                    }
                    switch (message.payload.widget.id) {
                        case EDIT_CONTROL_MAP:
                            if (gSelectedOverlay < 0)
                                break;
                            ClearStatusText();
                            gEditManager->SaveUndo();
                            placed = PlaceOverlay(
                                &m_types[gSelectedOverlay],
                                gEditManager->m_cursorX - OVERLAY_ANCHOR_X,
                                gEditManager->m_cursorY - OVERLAY_ANCHOR_Y,
                                true
                            );
                            if (placed) {
                                gEditManager->m_placedX = gEditManager->m_cursorX;
                                gEditManager->m_placedY =
                                    gEditManager->m_cursorY + (OVERLAY_GRID_HEIGHT - m_height);
                                gEditManager->m_placedState = 0;
                                gEditManager->DrawMap();
                                gEditManager->UpdateMapView();
                                gEditManager->DrawRadar(true);
                                gEditManager->m_mapChanged = true;
                            }
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_DESELECT:
                    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
                        break;
                    if (message.payload.widget.id == OVERLAY_HIGHLIGHT_ID)
                        goto pick;
                    if (message.payload.widget.id >= OVERLAY_CLASS_BUTTON_ID_FIRST
                        && message.payload.widget.id
                               < OVERLAY_CLASS_BUTTON_ID_FIRST + IDX(OVERLAY_CLASS_COUNT)) {
                        gObjectClass = message.payload.widget.id - OVERLAY_CLASS_BUTTON_ID_FIRST;
                    pick:
                        gSelectedOverlay = PickOverlay(gObjectClass);
                        DrawSelectedOverlay();
                        ShowClass(true);
                        gpInputManager->Flush();
                    }
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                peek = gpInputManager->PeekEvent();
                if (peek.type == MESSAGE_MOUSE_MOVE)
                    return MESSAGE_DISPATCH_CONSUME;
                mapX = message.payload.mouse.screenX;
                mapY = message.payload.mouse.screenY;
                gEditManager->ScreenToCell(mapX, mapY);
                mapX += gEditManager->m_viewX;
                mapY += gEditManager->m_viewY;
                if (gEditManager->m_cursorX != mapX || gEditManager->m_cursorY != mapY) {
                    gEditManager->m_cursorX = mapX;
                    gEditManager->m_cursorY = mapY;
                    mapX -= gEditManager->m_viewX;
                    mapY -= gEditManager->m_viewY;
                    if (gSelectedOverlay != OVERLAY_NONE) {
                        gEditManager->DrawMap();
                        mapX -= OVERLAY_ANCHOR_X;
                        mapY -= OVERLAY_ANCHOR_Y;
                        mapX = mapX * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_LEFT;
                        mapY = mapY * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_TOP;
                        if (gSelectedOverlay != OVERLAY_NONE) {
                            DrawFootprint(
                                mapX,
                                mapY,
                                OVERLAY_GRID_WIDTH,
                                OVERLAY_GRID_HEIGHT,
                                &m_types[gSelectedOverlay],
                                true
                            );
                            DrawOverlay(
                                &m_types[gSelectedOverlay],
                                mapX,
                                mapY,
                                true,
                                OVERLAY_GRID_WIDTH,
                                OVERLAY_GRID_HEIGHT,
                                true,
                                gEditManager->m_cursorX,
                                gEditManager->m_cursorY
                            );
                            m_previewDrawn = true;
                        }
                        gEditManager->UpdateMapView();
                        gEditManager->UpdateCursor();
                    }
                    gEditManager->UpdateCursor();
                }
            } else if (m_previewDrawn) {
                m_previewDrawn = false;
                gEditManager->DrawMap();
                gEditManager->UpdateMapView();
                gEditManager->UpdateCursor();
            }
            return MESSAGE_DISPATCH_CONSUME;
    }
    if (finished == true) {
        message.type = MESSAGE_EXECUTIVE;
        message.payload.executive.command = EXECUTIVE_COMMAND_TERMINATE_LOOP;
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}
#undef placed

VA(0x00419808, 0x1dd)
void overlayManager::DrawFootprint(
    i32 left,
    i32 top,
    i32 width,
    i32 height,
    overlayType* type,
    b32 clip
) {
    i32 cellSize;
    u8 color;
    i32 x;
    i32 y;
    i32 startX;
    i32 startY;
    overlayType* shape;

    if (!gConfig.showObjectBoxes)
        return;
    if (type->category == OVERLAY_CATEGORY_TOWN)
        shape = &gOverlayTypes[OVERLAY_TOWN_OUTLINE];
    else
        shape = type;
    startY = OVERLAY_GRID_HEIGHT - height;
    startX = OVERLAY_GRID_WIDTH - width;
    cellSize = gZoomTileSize[gEditManager->m_zoomLevel];
    for (y = startY; y < OVERLAY_GRID_HEIGHT; y++)
        for (x = startX; x < OVERLAY_GRID_WIDTH; x++)
            if (OverlayGridHas(shape->occupiedRows, x, y)) {
                if (OverlayGridHas(shape->overlayRows, x, y))
                    color = OVERLAY_COLOR_OVERLAY;
                else if (OverlayGridHas(shape->shadowRows, x, y))
                    color = OVERLAY_COLOR_SHADOW;
                else if (OverlayGridHas(shape->entranceRows, x, y))
                    color = OVERLAY_COLOR_ENTRANCE;
                else
                    color = OVERLAY_COLOR_OBJECT;
                if (!clip
                    || (left + (x - startX) * cellSize >= EDIT_VIEW_LEFT
                        && left + (x - startX + 1) * cellSize <= EDIT_VIEW_RIGHT
                        && top + (y - startY) * cellSize >= EDIT_VIEW_TOP
                        && top + (y - startY + 1) * cellSize <= EDIT_VIEW_BOTTOM))
                    MonoIconToBitmap(
                        m_cellIcon,
                        gpWindowManager->m_screen,
                        left + (x - startX) * cellSize,
                        top + (y - startY) * cellSize,
                        gEditManager->m_zoomLevel,
                        color,
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS
                    );
            }
}

VA(0x004199e5, 0x631)
b32 CanPlaceOverlay(overlayType* type, i32 left, i32 top, b32 overObjects) {
    i32 covered;
    i32 kept;
    mapCellExtra* part;
    i32 x;
    i32 y;
    i32 i;
    i32 j;
    overlayType* shape;
    b32 blocked;
    mapCell* cell;

    if (type->category == OVERLAY_CATEGORY_TOWN)
        shape = &gOverlayTypes[OVERLAY_TOWN_OUTLINE];
    else
        shape = type;
    covered = 0;
    kept = 0;
    if (type->trigger == IDX(MAP_OBJECT_ALCHEMIST_LAB) && top < OVERLAY_ALCHEMIST_LAB_TOP)
        return false;
    for (y = 0; y < OVERLAY_GRID_HEIGHT; y++)
        for (x = 0; x < OVERLAY_GRID_WIDTH; x++) {
            if (OverlayGridHas(shape->occupiedRows, x, y)
                && !OverlayGridHas(shape->shadowRows, x, y)) {
                if (left + x < 0 || left + x > MAP_WIDTH - 1 || top + y < 0
                    || top + y > MAP_HEIGHT - 1)
                    return false;
                cell = gMap.CellAt(left + x, top + y);
                if (OverlayGridHas(shape->overlayRows, x, y)) {
                    if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE)
                        gCoveredLinks[covered++] = cell->m_overlayLink;
                    if (cell->m_objectIndex != MAPCELL_SPRITE_NONE)
                        gCoveredLinks[covered++] = cell->m_objectLink;
                    if (cell->m_extraIndex) {
                        part = &gMap.extras[cell->m_extraIndex];
                        while (part) {
                            if (part->overlayIndex != MAPCELL_SPRITE_NONE)
                                gCoveredLinks[covered++] = part->overlayLink;
                            if (part->objectIndex != MAPCELL_SPRITE_NONE)
                                gCoveredLinks[covered++] = part->objectLink;
                            part = part->nextIndex ? &gMap.extras[part->nextIndex] : NULL;
                        }
                    }
                } else {
                    if (!(shape->groundMask & BIT(CELL_TERRAIN(gMap.CellAt(left + x, top + y)))))
                        return false;
                    if (OverlayGridHas(shape->entranceRows, x, y)) {
                        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                            blocked = false;
                            if (!cell->m_objectShadow)
                                blocked = true;
                            if (cell->m_extraIndex
                                && gMap.extras[cell->m_extraIndex].objectIndex
                                       != MAPCELL_SPRITE_NONE)
                                part = &gMap.extras[cell->m_extraIndex];
                            else
                                part = NULL;
                            while (part) {
                                if (!part->objectShadow)
                                    blocked = true;
                                if (part->nextIndex
                                    && gMap.extras[part->nextIndex].objectIndex
                                           != MAPCELL_SPRITE_NONE)
                                    part = &gMap.extras[part->nextIndex];
                                else
                                    part = NULL;
                            }
                            if (blocked && shape->trigger != IDX(MAP_OBJECT_MINE))
                                return false;
                        }
                    } else if (HAS(cell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
                               && !OverlayGridHas(shape->shadowRows, x, y))
                        return false;
                    if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE
                        || (cell->m_objectIndex != MAPCELL_SPRITE_NONE && !cell->m_objectHighLayer
                            && shape->highLayer))
                        gKeptLinks[kept++] = cell->m_overlayLink;
                    if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && !overObjects)
                        return false;
                    if (cell->m_objectIndex != MAPCELL_SPRITE_NONE
                        && (!shape->highLayer || cell->m_objectHighLayer))
                        gCoveredLinks[covered++] = cell->m_objectLink;
                    if (cell->m_extraIndex) {
                        part = &gMap.extras[cell->m_extraIndex];
                        while (part) {
                            if (part->overlayIndex != MAPCELL_SPRITE_NONE
                                || (part->objectIndex != MAPCELL_SPRITE_NONE
                                    && !part->objectHighLayer && shape->highLayer))
                                gKeptLinks[kept++] = part->overlayLink;
                            if (part->objectIndex != MAPCELL_SPRITE_NONE
                                && (!shape->highLayer || part->objectHighLayer))
                                gCoveredLinks[covered++] = part->objectLink;
                            part = part->nextIndex ? &gMap.extras[part->nextIndex] : NULL;
                        }
                    }
                }
            }
        }
    for (i = 0; i < covered; i++)
        for (j = 0; j < kept; j++)
            if (gCoveredLinks[i] == gKeptLinks[j])
                return false;
    return true;
}

VA(0x0041a016, 0x256)
void RemoveReplacedObjects(overlayType* type, i32 left, i32 top) {
    mapCellExtra* extra;
    i32 x;
    i32 y;
    i32 i;
    mapCell* cell;

    for (y = 0; y < OVERLAY_GRID_HEIGHT; y++)
        for (x = 0; x < OVERLAY_GRID_WIDTH; x++) {
            if (left + x < 0 || left + x > MAP_WIDTH - 1 || top + y < 0 || top + y > MAP_HEIGHT - 1)
                continue;
            i = x + y * OVERLAY_GRID_WIDTH;
            if (OverlayGridHas(type->occupiedRows, x, y)) {
                cell = gMap.CellAt(left + x, top + y);
                if (cell->m_overlayIndex == type->frames[i]
                    && IDX(cell->m_overlayTileset) == type->tileset)
                    gEditManager->RemoveLinkedObject(cell->m_overlayLink);
                if (cell->m_objectIndex == type->frames[i]
                    && IDX(cell->m_objectTileset) == type->tileset)
                    gEditManager->RemoveLinkedObject(cell->m_objectLink);
                if (cell->m_extraIndex) {
                    extra = &gMap.extras[cell->m_extraIndex];
                    while (extra) {
                        if (extra->overlayIndex == type->frames[i]
                            && IDX(extra->overlayTileset) == type->tileset)
                            gEditManager->RemoveLinkedObject(extra->overlayLink);
                        if (extra->objectIndex == type->frames[i]
                            && IDX(extra->objectTileset) == type->tileset)
                            gEditManager->RemoveLinkedObject(extra->objectLink);
                        extra = extra->nextIndex ? &gMap.extras[extra->nextIndex] : NULL;
                    }
                }
            }
        }
}

#define placed done       // frame-slot spelling
#define newTown newCastle // frame-slot spelling
#define newSign bottle    // frame-slot spelling
#define newEvent pEvent   // frame-slot spelling
#define newSphinx sphinx  // frame-slot spelling
#define newHero jail      // frame-slot spelling
#define heroFaction kind  // frame-slot spelling
#define anchor landing    // frame-slot spelling
#define anchorX spotX     // frame-slot spelling
#define anchorY spotY     // frame-slot spelling
#define ground groundNum  // frame-slot spelling
#define shadow shadowNum  // frame-slot spelling
VA(0x0041a26c, 0x14d6)
b32 PlaceOverlay(overlayType* type, i32 x, i32 y, b32 newLink) {
    i32 idx;
    i32 col;
    mapCellExtra* node;
    i32 row;
    mapCell* dest;
    i32 anchorX;
    mapCell* anchor;
    i32 ground;
    i32 anchorY;
    i32 shadow;
    b32 H2_UNUSED(placed);
    TownExtra* newTown;
    signEventExtra* newSign;
    EventExtra* newEvent;
    HeroExtra* newHero;
    mapEventExtra* newSphinx;
    i32 H2_UNUSED(extraIndex);
    i32 heroFaction;

    if (newLink)
        gNextObjectLink++;
    if (!CanPlaceOverlay(type, x, y, true)) {
        LogStr("Invalid Placement");
        ShowStatusWarning(localization::Tr("editor.overlay.invalid_placement"));
        return false;
    }
    RemoveReplacedObjects(type, x, y);
    if (type->category == OVERLAY_CATEGORY_TOWN && gEditManager->CountTowns() >= GAME_TOWN_COUNT) {
        sprintf(gText, localization::Tr("editor.overlay.limit.towns"), GAME_TOWN_COUNT);
        ShowStatusWarning(gText);
        return false;
    }
    if (type->trigger == IDX(MAP_OBJECT_MAP_EVENT)
        && gEditManager->CountEvents() >= OVERLAY_EVENT_LIMIT) {
        sprintf(gText, localization::Tr("editor.overlay.limit.events"), OVERLAY_EVENT_LIMIT);
        ShowStatusWarning(gText);
        return false;
    }
    if ((type->trigger & MAP_TRIGGER_TYPE_MASK) == IDX(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT)) {
        for (col = 0; col < MAP_WIDTH; col++)
            for (row = 0; row < MAP_WIDTH; row++)
                if ((gMap.CellAt(col, row)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    == MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT) {
                    sprintf(gText, localization::Tr("editor.overlay.ultimate.placed"));
                    ShowStatusWarning(gText);
                    return false;
                }
        anchorX = x + OVERLAY_ANCHOR_X;
        anchorY = y + OVERLAY_ANCHOR_Y;
        if (anchorX < OVERLAY_ULTIMATE_MARGIN || anchorX > MAP_WIDTH - OVERLAY_ULTIMATE_MARGIN - 1
            || anchorY < OVERLAY_ULTIMATE_MARGIN
            || anchorY > MAP_HEIGHT - OVERLAY_ULTIMATE_MARGIN - 1) {
            sprintf(gText, localization::Tr("editor.overlay.ultimate.edge"));
            ShowStatusWarning(gText);
            return false;
        }
        anchor = gMap.CellAt(x + OVERLAY_ANCHOR_X, y + OVERLAY_ANCHOR_Y);
        if (CELL_TERRAIN(anchor) == TERRAIN_WATER) {
            sprintf(gText, localization::Tr("editor.overlay.ultimate.land"));
            ShowStatusWarning(gText);
            return false;
        }
    }
    if ((type->trigger == IDX(MAP_OBJECT_ABANDONED_MINE) || type->trigger == IDX(MAP_OBJECT_MINE)
         || type->trigger == IDX(MAP_OBJECT_SAWMILL)
         || type->trigger == IDX(MAP_OBJECT_ALCHEMIST_LAB)
         || type->trigger == IDX(MAP_OBJECT_EYE_OF_MAGI)
         || type->trigger == IDX(MAP_OBJECT_DRAGON_CITY)
         || type->trigger == IDX(MAP_OBJECT_LIGHTHOUSE))
        && gEditManager->CountMines() >= GAME_MINE_COUNT) {
        sprintf(gText, localization::Tr("editor.overlay.limit.mines"), GAME_MINE_COUNT);
        ShowStatusWarning(gText);
        return false;
    }
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        shadow = -1;
        ground = -1;
        if (type->id >= OVERLAY_TOWN_FIRST && type->id <= OVERLAY_TOWN_LAST)
            shadow =
                (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS + OVERLAY_TOWN_SHADOWS;
        else
            shadow = (type->id - OVERLAY_RANDOM_TOWN_FIRST) % OVERLAY_TOWN_KINDS + OVERLAY_RANDOM_TOWN_SHADOWS;
        ground =
            IDX(CELL_TERRAIN(gMap.CellAt(x + OVERLAY_TOWN_ENTRANCE_X, y + OVERLAY_TOWN_ENTRANCE_Y)))
            + OVERLAY_TOWN_GROUNDS;
        placed = PlaceOverlay(&gOverlayTypes[shadow], x, y, false);
        placed = PlaceOverlay(&gOverlayTypes[ground], x, y, false);
    }
    for (row = 0; row < OVERLAY_GRID_HEIGHT; row++)
        for (col = 0; col < OVERLAY_GRID_WIDTH; col++) {
            if (x + col >= 0 && x + col < MAP_WIDTH && y + row >= 0 && y + row < MAP_HEIGHT) {
                idx = col + row * OVERLAY_GRID_WIDTH;
                dest = gMap.CellAt(x + col, y + row);
                if (type->frames[idx] != OVERLAY_NO_FRAME) {
                    if (OverlayGridHas(type->overlayRows, col, row)) {
                        if (dest->m_overlayIndex != MAPCELL_SPRITE_NONE) {
                            node = gMap.GetNewCellExtraOverlay(x + col, y + row);
                            node->overlayLink = gNextObjectLink;
                            node->overlayIndex = type->frames[idx];
                            node->overlayTileset = static_cast<TilesetId>(type->tileset);
                            if (row < OVERLAY_GRID_HEIGHT - 2
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && (type->trigger != IDX(MAP_OBJECT_ALCHEMIST_TOWER)
                                    || !OverlayGridHas(type->entranceRows, col, row + 1))
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                node->drawOverlayOnTop = 1;
                            else
                                node->drawOverlayOnTop = 0;
                            if (OverlayGridHas(type->animatedRows, col, row))
                                node->animatedOverlay = 1;
                            else
                                node->animatedOverlay = 0;
                        } else {
                            dest->m_overlayLink = gNextObjectLink;
                            dest->m_overlayIndex = type->frames[idx];
                            dest->m_overlayTileset = static_cast<TilesetId>(type->tileset);
                            if (row < OVERLAY_GRID_HEIGHT - 2
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && (type->trigger != IDX(MAP_OBJECT_ALCHEMIST_TOWER)
                                    || !OverlayGridHas(type->entranceRows, col, row + 1))
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                dest->m_drawOverlayOnTop = 1;
                            else
                                dest->m_drawOverlayOnTop = 0;
                            if (OverlayGridHas(type->animatedRows, col, row))
                                dest->m_animatedOverlay = 1;
                            else
                                dest->m_animatedOverlay = 0;
                        }
                    } else {
                        if (dest->m_objectIndex != MAPCELL_SPRITE_NONE
                            && OverlayGridHas(type->entranceRows, col, row)
                            && type->trigger != IDX(MAP_OBJECT_MINE))
                            gMap.PushCellObject(x + col, y + row);
                        if (dest->m_objectIndex != MAPCELL_SPRITE_NONE) {
                            node = gMap.GetNewCellExtraObject(x + col, y + row);
                            node->objectLink = gNextObjectLink;
                            node->objectIndex = type->frames[idx];
                            node->objectTileset = static_cast<TilesetId>(type->tileset);
                            if (OverlayGridHas(type->shadowRows, col, row))
                                node->objectShadow = 1;
                            else
                                node->objectShadow = 0;
                            if (row < OVERLAY_GRID_HEIGHT - 1 && !type->highLayer
                                && !OverlayGridHas(type->shadowRows, col, row)
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                node->objectDrawnAsOverlay = 1;
                            else
                                node->objectDrawnAsOverlay = 0;
                            if (type->highLayer)
                                node->objectHighLayer = 1;
                            else
                                node->objectHighLayer = 0;
                            if (OverlayGridHas(type->animatedRows, col, row))
                                node->animatedObject = 1;
                            else
                                node->animatedObject = 0;
                        } else {
                            if (giGroundShape[dest->m_terrainImageIndex]
                                & GROUND_SHAPE_VARIED)
                                dest->m_terrainImageIndex = ChooseGroundTile(
                                    IDX(CELL_TERRAIN(dest)),
                                    giGroundShape[dest->m_terrainImageIndex]
                                        - GROUND_SHAPE_VARIED,
                                    false,
                                    0,
                                    0,
                                    false,
                                    1.0f
                                );
                            dest->m_objectLink = gNextObjectLink;
                            dest->m_objectIndex = type->frames[idx];
                            dest->m_objectTileset = static_cast<TilesetId>(type->tileset);
                            if (OverlayGridHas(type->shadowRows, col, row))
                                dest->m_triggerType = MAP_OBJECT_NONE;
                            else
                                dest->m_triggerType = type->trigger;
                            if (OverlayGridHas(type->shadowRows, col, row))
                                dest->m_objectShadow = 1;
                            else
                                dest->m_objectShadow = 0;
                            if (row < OVERLAY_GRID_HEIGHT - 1 && !type->highLayer
                                && !OverlayGridHas(type->shadowRows, col, row)
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                dest->m_objectDrawnAsOverlay = 1;
                            else
                                dest->m_objectDrawnAsOverlay = 0;
                            if (type->highLayer)
                                dest->m_objectHighLayer = 1;
                            else
                                dest->m_objectHighLayer = 0;
                            if (OverlayGridHas(type->animatedRows, col, row))
                                dest->m_animatedObject = 1;
                            else
                                dest->m_animatedObject = 0;
                        }
                    }
                    if (OverlayGridHas(type->entranceRows, col, row)
                        && type->trigger == IDX(MAP_OBJECT_MINE))
                        dest->m_triggerType = MAP_OBJECT_MINE;
                    if (OverlayGridHas(type->entranceRows, col, row))
                        dest->m_triggerType |= MAP_TRIGGER_ACTION_FLAG;
                    if ((dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE))
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        newTown = new TownExtra;
                        memset(newTown, 0, sizeof(TownExtra));
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newTown;
                        gEditMapHeader.townNameIndex =
                            (gEditMapHeader.townNameIndex + 1) % EDITOR_TOWN_NAME_COUNT;
                        strcpy(newTown->name, gTownNames[gEditMapHeader.townNameIndex]);
                        newTown->owner =
                            type->color == OVERLAY_NO_COLOR ? TOWN_OWNER_NONE : type->color;
                        if (type->id >= OVERLAY_RANDOM_TOWN_FIRST
                            && type->id <= OVERLAY_RANDOM_TOWN_LAST) {
                            newTown->faction = OVERLAY_RANDOM_TOWN_FACTION;
                            newTown->isCastle = 1 - (type->id - OVERLAY_RANDOM_TOWN_FIRST) % OVERLAY_TOWN_KINDS;
                        } else {
                            newTown->faction =
                                (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS / OVERLAY_TOWN_KINDS;
                            newTown->isCastle = 1 - (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_KINDS;
                        }
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(TownExtra);
                        gEditManager->m_extraCount++;
                    }
                    if ((dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE))
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        newSign = new signEventExtra;
                        memset(newSign, 0, sizeof(signEventExtra));
                        newSign->active = MAP_EVENT_DATA_AVAILABLE;
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newSign;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] =
                            sizeof(signEventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        newEvent = new EventExtra;
                        memset(newEvent, 0, sizeof(EventExtra));
                        newEvent->isMapEvent = true;
                        newEvent->artifact = IDX(ARTIFACT_NONE);
                        newEvent->cancelAfterVisit = true;
                        for (idx = 0; idx < GAME_PLAYER_COUNT; idx++)
                            newEvent->players[idx] = true;
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newEvent;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(EventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        newSphinx = new mapEventExtra;
                        memset(newSphinx, 0, sizeof(mapEventExtra));
                        newSphinx->artifact = IDX(ARTIFACT_NONE);
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newSphinx;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] =
                            sizeof(mapEventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if ((dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_HERO)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        newHero = new HeroExtra;
                        memset(newHero, 0, sizeof(HeroExtra));
                        for (idx = 0; idx < ARMY_GROUP_SLOT_COUNT; idx++)
                            newHero->troopTypes[idx] = CREATURE_NONE;
                        for (idx = 0; idx < EVENT_RECORD_HERO_ARTIFACT_COUNT; idx++)
                            newHero->artifacts[idx] = IDX(ARTIFACT_NONE);
                        for (idx = 0; idx < HERO_SECONDARY_SKILL_CAPACITY; idx++)
                            newHero->skillTypes[idx] = IDX(HERO_SKILL_NONE);
                        if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))
                            heroFaction = IDX(FACTION_KNIGHT);
                        else
                            heroFaction = dest->m_objectIndex % OVERLAY_HERO_FRAMES_PER_COLOR;
                        if (heroFaction == IDX(FACTION_KNIGHT)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_LEADERSHIP);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_BASIC);
                            newHero->skillTypes[1] = IDX(HERO_SKILL_BALLISTICS);
                            newHero->skillLevels[1] = IDX(HERO_SKILL_LEVEL_BASIC);
                        }
                        if (heroFaction == IDX(FACTION_SORCERESS)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_NAVIGATION);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_ADVANCED);
                            newHero->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            newHero->skillLevels[1] = IDX(HERO_SKILL_LEVEL_BASIC);
                        }
                        if (heroFaction == IDX(FACTION_BARBARIAN)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_PATHFINDING);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_ADVANCED);
                        }
                        if (heroFaction == IDX(FACTION_WARLOCK)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_SCOUTING);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_ADVANCED);
                            newHero->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            newHero->skillLevels[1] = IDX(HERO_SKILL_LEVEL_BASIC);
                        }
                        if (heroFaction == IDX(FACTION_WIZARD)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_WISDOM);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_ADVANCED);
                        }
                        if (heroFaction == IDX(FACTION_NECROMANCER)) {
                            newHero->skillTypes[0] = IDX(HERO_SKILL_NECROMANCY);
                            newHero->skillLevels[0] = IDX(HERO_SKILL_LEVEL_BASIC);
                            newHero->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            newHero->skillLevels[1] = IDX(HERO_SKILL_LEVEL_BASIC);
                        }
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newHero;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(HeroExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT)
                        && dest->m_objectIndex / EVENTS_ARTIFACT_SPRITE_FRAMES == IDX(ARTIFACT_SPELL_SCROLL))
                        dest->m_objectMetadata = 0;
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_OBJECT)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        switch (type->id) {
                            case OVERLAY_ALCHEMIST_TOWER_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_ALCHEMIST_TOWER);
                                break;
                            case OVERLAY_ARENA_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_ARENA);
                                break;
                            case OVERLAY_HUT_OF_MAGI_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_HUT_OF_MAGI);
                                break;
                            case OVERLAY_EYE_OF_MAGI_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_EYE_OF_MAGI);
                                break;
                            case OVERLAY_STABLES_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_STABLES);
                                break;
                            case OVERLAY_MERMAID_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_MERMAID);
                                break;
                            case OVERLAY_SIRENS_SITE:
                                dest->m_objectMetadata = IDX(GENERIC_SITE_SIRENS);
                                break;
                        }
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_EXPANSION_DWELLING)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        switch (type->id) {
                            case OVERLAY_BARROW_MOUNDS_SITE:
                                dest->m_objectMetadata = IDX(RECRUITMENT_SITE_BARROW_MOUNDS);
                                break;
                            case OVERLAY_EARTH_ALTAR_SITE:
                                dest->m_objectMetadata = IDX(RECRUITMENT_SITE_EARTH_ALTAR);
                                break;
                            case OVERLAY_AIR_ALTAR_SITE:
                                dest->m_objectMetadata = IDX(RECRUITMENT_SITE_AIR_ALTAR);
                                break;
                            case OVERLAY_FIRE_ALTAR_SITE:
                                dest->m_objectMetadata = IDX(RECRUITMENT_SITE_FIRE_ALTAR);
                                break;
                            case OVERLAY_WATER_ALTAR_SITE:
                                dest->m_objectMetadata = IDX(RECRUITMENT_SITE_WATER_ALTAR);
                                break;
                        }
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BARRIER)
                        && OverlayGridHas(type->entranceRows, col, row))
                        dest->m_objectMetadata = type->id - OVERLAY_BARRIERS;
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_TRAVELER_TENT)
                        && OverlayGridHas(type->entranceRows, col, row))
                        dest->m_objectMetadata = type->id - OVERLAY_TRAVELER_TENTS;
                    if (OverlayGridHas(type->resourceRows, col, row))
                        PlaceResourceMarker(
                            &gOverlayTypes[OVERLAY_RESOURCE_MARKERS] + type->color,
                            x + col,
                            y + row,
                            false
                        );
                }
            }
        }
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        PlaceOverlay(
            &gOverlayTypes[OVERLAY_TOWN_FLAGS + type->color * OVERLAY_TOWN_FLAG_PARTS],
            x + OVERLAY_LEFT_FLAG_X,
            y + OVERLAY_FLAG_Y,
            false
        );
        PlaceOverlay(
            &gOverlayTypes[OVERLAY_TOWN_FLAGS + 1 + type->color * OVERLAY_TOWN_FLAG_PARTS],
            x + OVERLAY_RIGHT_FLAG_X,
            y + OVERLAY_FLAG_Y,
            false
        );
    }
    return true;
}
#undef placed
#undef newTown
#undef newSign
#undef newEvent
#undef newSphinx
#undef newHero
#undef heroFaction
#undef anchor
#undef anchorX
#undef anchorY
#undef ground
#undef shadow

VA(0x0041b742, 0xf1)
b32 PlaceResourceMarker(overlayType* type, i32 x, i32 y, b32 requireMine) {
    b32 valid;
    mapCell* dest;

    valid = true;
    if (requireMine && type->flags & OVERLAY_FLAG_MINE_MARKER
        && (gMap.CellAt(x, y)->m_triggerType != MAP_OBJECT_MINE || x < 1
            || gMap.CellAt(x - 1, y)->m_triggerType != MAP_ACTION_TRIGGER(MAP_OBJECT_MINE)))
        valid = false;
    if (!valid) {
        ShowStatusWarning(localization::Tr("editor.overlay.invalid_placement"));
        return false;
    }
    dest = gMap.CellAt(x, y);
    gMap.ChangeTilesetIndex(
        dest,
        x,
        y,
        TILESET_EXTRAOVR,
        type->frames[OVERLAY_GRID_ANCHOR],
        0,
        gNextObjectLink
    );
    return true;
}

#define ground groundType // frame-slot spelling
#define frameCount count  // frame-slot spelling
VA(0x0041b833, 0x627)
void overlayManager::DrawOverlay(
    overlayType* type,
    i32 x,
    i32 y,
    b32 clip,
    i32 width,
    i32 height,
    b32 update,
    i32 cellX,
    i32 cellY
) {
    i32 gridX;
    i32 tileSize;
    i32 gridY;
    i32 fromX;
    i32 fromY;
    i32 cellTerrain;
    i32 frameCount;
    i32 ground;
    i32 shadow;

    fromY = OVERLAY_GRID_HEIGHT - height;
    fromX = OVERLAY_GRID_WIDTH - width;
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        shadow = -1;
        ground = -1;
        if (type->id >= OVERLAY_TOWN_FIRST && type->id <= OVERLAY_TOWN_LAST)
            shadow = (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS + OVERLAY_TOWN_SHADOWS;
        else
            shadow = (type->id - OVERLAY_RANDOM_TOWN_FIRST) % OVERLAY_TOWN_KINDS + OVERLAY_RANDOM_TOWN_SHADOWS;
        ground = OVERLAY_TOWN_GRASS_GROUND;
        if (cellX != EDIT_NO_CELL) {
            if (cellX < OVERLAY_TOWN_GROUND_COLUMN)
                cellX = OVERLAY_TOWN_GROUND_COLUMN;
            if (cellY < 0)
                cellY = 0;
            if (cellX > MAP_WIDTH - 1)
                cellX = MAP_WIDTH - 1;
            if (cellY > MAP_HEIGHT - 1)
                cellY = MAP_HEIGHT - 1;
            cellTerrain = IDX(CELL_TERRAIN(gMap.CellAt(cellX - OVERLAY_TOWN_GROUND_COLUMN, cellY)));
            if (cellTerrain != IDX(TERRAIN_WATER))
                ground = cellTerrain + OVERLAY_TOWN_GROUNDS;
        }
        DrawOverlay(
            &gOverlayTypes[shadow],
            x,
            y,
            clip,
            width,
            height,
            false,
            EDIT_NO_CELL,
            EDIT_NO_CELL
        );
        DrawOverlay(
            &gOverlayTypes[ground],
            x,
            y,
            clip,
            width,
            height,
            false,
            EDIT_NO_CELL,
            EDIT_NO_CELL
        );
    }
    tileSize = gZoomTileSize[gEditManager->m_zoomLevel];
    for (gridY = fromY; gridY < OVERLAY_GRID_HEIGHT; gridY++)
        for (gridX = fromX; gridX < OVERLAY_GRID_WIDTH; gridX++)
            if (type->frames[gridX + gridY * OVERLAY_GRID_WIDTH] != OVERLAY_NO_FRAME
                && (!clip
                    || (x + (gridX - fromX) * tileSize >= EDIT_VIEW_LEFT
                        && x + (gridX - fromX + 1) * tileSize <= EDIT_VIEW_RIGHT
                        && y + (gridY - fromY) * tileSize >= EDIT_VIEW_TOP
                        && y + (gridY - fromY + 1) * tileSize <= EDIT_VIEW_BOTTOM))) {
                if (type->tileset == IDX(TILESET_MINIHERO))
                    IconToBitmapScaleDouble(
                        gEditManager->m_objectIcons[type->tileset][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize
                            - EDIT_HERO_LIFT / gZoomScale[gEditManager->m_zoomLevel],
                        type->frames[gridX + gridY * OVERLAY_GRID_WIDTH],
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                else
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[type->tileset][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->frames[gridX + gridY * OVERLAY_GRID_WIDTH],
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (OverlayGridHas(type->animatedRows, gridX, gridY)) {
                    frameCount = GetIconEntry(
                                gEditManager->m_objectIcons[type->tileset][0],
                                type->frames[gridX + gridY * OVERLAY_GRID_WIDTH]
                    )
                                ->flags;
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[type->tileset][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->frames[gridX + gridY * OVERLAY_GRID_WIDTH] + 1
                            + gEditManager->m_animationCounter % frameCount,
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                }
                if (type->flags & OVERLAY_FLAG_SHOWS_RESOURCE
                    && OverlayGridHas(type->resourceRows, gridX, gridY))
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[IDX(TILESET_EXTRAOVR)][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color,
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (type->category == OVERLAY_CATEGORY_TOWN && gridX == OVERLAY_TOWN_FLAG_COLUMN
                    && gridY == OVERLAY_TOWN_FLAG_ROW)
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[IDX(TILESET_FLAG32)][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color * OVERLAY_TOWN_FLAG_PARTS,
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (type->category == OVERLAY_CATEGORY_TOWN
                    && gridX == OVERLAY_TOWN_RIGHT_FLAG_COLUMN && gridY == OVERLAY_TOWN_FLAG_ROW)
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[IDX(TILESET_FLAG32)][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color * OVERLAY_TOWN_FLAG_PARTS + 1,
                        static_cast<IconDrawClipMode>(clip),
                        EDIT_VIEW_LEFT,
                        EDIT_VIEW_TOP,
                        EDIT_VIEW_PIXELS,
                        EDIT_VIEW_PIXELS,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (update)
                    gpWindowManager->UpdateScreenRegion(
                        OVERLAY_PREVIEW_REGION_X,
                        OVERLAY_PREVIEW_REGION_Y,
                        OVERLAY_PREVIEW_REGION_WIDTH,
                        OVERLAY_PREVIEW_REGION_HEIGHT
                    );
            }
}
#undef ground
#undef frameCount

VA(0x0041be5a, 0x24c)
b32 overlayManager::LoadClass(i32 objectClass) {
    i32 H2_UNUSED(unusedCount);
    b32 changed;
    i32 H2_UNUSED(unusedFlag);
    overlayType tmp;
    i32 H2_UNUSED(unusedIndex);
    i32 i;
    i32 j;

    m_typeCount = 0;
    unusedCount = 0;
    for (i = 0; i < OVERLAY_TYPE_COUNT; i++)
        if (gOverlayTypes[i].category == gObjectClassCategories[objectClass]
            && gOverlayTypes[i].terrainMask & gObjectClassTerrains[objectClass])
            m_types[m_typeCount++] = gOverlayTypes[i];
    for (i = 0; i < m_typeCount; i++) {
        changed = false;
        for (j = m_typeCount - 1; j > 0; j--)
            if (m_types[j].ordinal < m_types[j - 1].ordinal) {
                tmp = m_types[j];
                m_types[j] = m_types[j - 1];
                m_types[j - 1] = tmp;
                changed = true;
            }
        if (!changed)
            break;
    }
    if (m_typeCount)
        return true;
    return false;
}

VA(0x0041c0a6, 0xbe)
void overlayManager::MeasureOverlay(overlayType* type) {
    i32 x;
    i32 y;

    m_width = 1;
    m_height = 1;
    for (y = 0; y < OVERLAY_GRID_HEIGHT; y++)
        for (x = 0; x < OVERLAY_GRID_WIDTH; x++)
            if (OverlayGridHas(type->occupiedRows, x, y)) {
                if (OVERLAY_GRID_WIDTH - x > m_width)
                    m_width = OVERLAY_GRID_WIDTH - x;
                if (OVERLAY_GRID_HEIGHT - y > m_height)
                    m_height = OVERLAY_GRID_HEIGHT - y;
            }
}

// The panel has no preview of the selected object.
VA(0x0041c164, 0xb)
void overlayManager::DrawSelectedOverlay(void) {}

#define previousClass was // frame-slot spelling
VA(0x0041c16f, 0x120)
b32 overlayManager::SelectOverlay(i32 index) {
    i32 objectClass;
    i32 slot;
    i32 previousClass;
    i32 x;
    tag_message message;
    i32 y;

    previousClass = gObjectClass;
    for (objectClass = OVERLAY_CLASS_COUNT - 1; objectClass >= 0; objectClass--) {
        gObjectClass = objectClass;
        LoadClass(gObjectClass);
        for (slot = 0; slot < m_typeCount; slot++)
            if (m_types[slot].id == gOverlayTypes[index].id) {
                gSelectedOverlay = slot;
                DrawSelectedOverlay();
                ShowClass(true);
                gEditManager->m_cursorX = EDIT_NO_CELL;
                message.type = MESSAGE_MOUSE_MOVE;
                gpMouseManager->MouseCoords(x, y);
                message.payload.mouse.screenX = message.payload.mouse.x = x;
                message.payload.mouse.screenY = message.payload.mouse.y = y;
                Main(message);
                return true;
            }
    }
    gSelectedOverlay = OVERLAY_NONE;
    gObjectClass = previousClass;
    return false;
}
#undef previousClass

VA(0x0041c28f, 0x2ba)
void overlayManager::DrawPicker(b32 update) {
    i32 col;
    i32 idx;
    i32 row;
    u8 frames[OVERLAY_PICKER_COLUMNS][OVERLAY_PICKER_ROWS];
    overlayType* shown;
    overlayType* type;

    idx = 0;
    memset(frames, OVERLAY_PICKER_PLAIN_BOX, sizeof(frames));
    idx = 0;
    while (gPickerFirst + idx < m_typeCount && idx < OVERLAY_PICKER_PAGE) {
        type = &m_types[gPickerFirst + idx];
        col = idx % OVERLAY_PICKER_COLUMNS;
        row = idx / OVERLAY_PICKER_COLUMNS;
        if (type->trigger == IDX(MAP_OBJECT_CASTLE) || type->trigger == IDX(MAP_OBJECT_RANDOM_TOWN)
            || type->trigger == IDX(MAP_OBJECT_RANDOM_CASTLE))
            frames[col][row] = type->color == OVERLAY_NO_COLOR
                                   ? OVERLAY_PICKER_PLAIN_BOX
                                   : type->color + OVERLAY_PICKER_COLOR_BOX;
        if (type->trigger == IDX(MAP_OBJECT_HERO))
            frames[col][row] = type->frames[OVERLAY_GRID_ANCHOR] / OVERLAY_HERO_FRAMES_PER_COLOR
                               + OVERLAY_PICKER_COLOR_BOX;
        idx++;
    }
    if (gPickerRows > OVERLAY_PICKER_ROWS)
        m_pickerKnob->m_y = gPickerFirst / OVERLAY_PICKER_COLUMNS
                                * (static_cast<double>(OVERLAY_PICKER_KNOB_TRAVEL)
                                   / (gPickerRows - (OVERLAY_PICKER_ROWS - 1) - 1.0))
                            + static_cast<double>(OVERLAY_PICKER_KNOB_Y);
    else
        m_pickerKnob->m_y = OVERLAY_PICKER_KNOB_PARKED;
    m_picker->DrawWindow(WINDOW_DRAW_BUFFER_ONLY);
    for (col = 0; col < OVERLAY_PICKER_COLUMNS; col++)
        for (row = 0; row < OVERLAY_PICKER_ROWS; row++)
            m_paletteIcon->DrawToBuffer(
                col * OVERLAY_PICKER_BOX_WIDTH,
                row * OVERLAY_PICKER_BOX_HEIGHT,
                frames[col][row],
                ICON_DRAW_NORMAL
            );
    idx = 0;
    while (gPickerFirst + idx < m_typeCount && idx < OVERLAY_PICKER_PAGE) {
        shown = &m_types[gPickerFirst + idx];
        col = idx % OVERLAY_PICKER_COLUMNS * OVERLAY_PICKER_BOX_WIDTH + OVERLAY_PICKER_OBJECT_INSET;
        row =
            idx / OVERLAY_PICKER_COLUMNS * OVERLAY_PICKER_BOX_HEIGHT + OVERLAY_PICKER_OBJECT_INSET;
        DrawFootprint(
            col,
            row,
            OVERLAY_PICKER_VIEW_WIDTH,
            OVERLAY_PICKER_VIEW_HEIGHT,
            shown,
            false
        );
        DrawOverlay(
            shown,
            col,
            row,
            false,
            OVERLAY_PICKER_VIEW_WIDTH,
            OVERLAY_PICKER_VIEW_HEIGHT,
            false,
            EDIT_NO_CELL,
            EDIT_NO_CELL
        );
        idx++;
    }
    if (update)
        gpWindowManager->UpdateScreen();
}

VA(0x0041c549, 0x2d7)
i32 overlayManager::PickOverlay(i32 objectClass) {
    b32 H2_UNUSED(needDraw);
    b32 H2_UNUSED(done);
    tag_message H2_UNUSED(message);
    i32 zoom;

    zoom = gEditManager->m_zoomLevel;
    done = false;
    needDraw = true;
    m_picker = new heroWindow(0, 0, "editpalt.bin");
    m_pickerTrack = new iconWidget(
        OVERLAY_PICKER_TRACK_X,
        OVERLAY_PICKER_TRACK_Y,
        OVERLAY_PICKER_TRACK_WIDTH,
        OVERLAY_PICKER_TRACK_HEIGHT,
        "escroll.icn",
        OVERLAY_PICKER_TRACK_FRAME,
        ICON_DRAW_NORMAL,
        OVERLAY_PICKER_TRACK_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_pickerKnob = new iconWidget(
        OVERLAY_PICKER_KNOB_X,
        OVERLAY_PICKER_KNOB_Y,
        OVERLAY_PICKER_KNOB_WIDTH,
        OVERLAY_PICKER_KNOB_HEIGHT,
        "escroll.icn",
        OVERLAY_PICKER_KNOB_FRAME,
        ICON_DRAW_NORMAL,
        OVERLAY_PICKER_KNOB_ID,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_picker->AddWidget(m_pickerTrack, -1);
    m_picker->AddWidget(m_pickerKnob, -1);
    gEditManager->m_zoomLevel = OVERLAY_PICKER_ZOOM;
    LoadClass(objectClass);
    if (gPickerFirst + OVERLAY_PICKER_PAGE >= m_typeCount)
        gPickerFirst = m_typeCount - OVERLAY_PICKER_PAGE
                       + (m_typeCount % OVERLAY_PICKER_COLUMNS
                              ? OVERLAY_PICKER_COLUMNS - m_typeCount % OVERLAY_PICKER_COLUMNS
                              : 0);
    if (gPickerFirst < 0)
        gPickerFirst = 0;
    gPickerRows = (m_typeCount - 1) / OVERLAY_PICKER_COLUMNS + 1;
    gpInputManager->Flush();
    gpWindowManager->AddWindow(m_picker, -1, 0);
    DrawPicker(true);
    gpWindowManager->DoDialog(m_picker, PickerHandler, 0);
    gpWindowManager->RemoveWindow(m_picker);
    delete m_picker;
    gpInputManager->Flush();
    gEditManager->m_zoomLevel = zoom;
    return gPickedOverlay;
}

VA(0x0041c820, 0x20)
MessageDispatchResult PickerHandler(tag_message& message) {
    return static_cast<overlayManager*>(gEditManager->m_toolManager)->PickerMain(message);
}

VA(0x0041c840, 0x353)
MessageDispatchResult overlayManager::PickerMain(tag_message& message) {
    b32 needDraw;
    b32 done;
    i32 x;
    i32 y;

    needDraw = false;
    done = false;
    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case OVERLAY_PICKER_ROW_UP:
                            gPickerFirst -= OVERLAY_PICKER_COLUMNS;
                            if (gPickerFirst < 0)
                                gPickerFirst = 0;
                            needDraw = true;
                            break;
                        case OVERLAY_PICKER_ROW_DOWN:
                            if (m_typeCount < OVERLAY_PICKER_PAGE)
                                break;
                            if (gPickerFirst + OVERLAY_PICKER_PAGE < m_typeCount)
                                gPickerFirst += OVERLAY_PICKER_COLUMNS;
                            needDraw = true;
                            break;
                        case OVERLAY_PICKER_CLASS_PREVIOUS:
                            gObjectClass =
                                (gObjectClass + OVERLAY_CLASS_COUNT - 1) % OVERLAY_CLASS_COUNT;
                            goto showClass;
                        case OVERLAY_PICKER_CLASS_NEXT:
                            gObjectClass =
                                (gObjectClass + OVERLAY_CLASS_COUNT + 1) % OVERLAY_CLASS_COUNT;
                        showClass:
                            gPickerFirst = 0;
                            gSelectedOverlay = 0;
                            needDraw = true;
                            LoadClass(gObjectClass);
                            gPickerRows = (m_typeCount - 1) / OVERLAY_PICKER_COLUMNS + 1;
                            break;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    switch (message.payload.widget.id) {
                        case OVERLAY_PICKER_TRACK_ID:
                            DragPickerKnob(
                                true,
                                message.payload.widget.screenX,
                                message.payload.widget.screenY
                            );
                            break;
                        case OVERLAY_PICKER_KNOB_ID:
                            DragPickerKnob(false, EDIT_NO_CELL, EDIT_NO_CELL);
                            break;
                        case OVERLAY_PICKER_BOXES:
                            x = message.payload.widget.screenX / OVERLAY_PICKER_BOX_WIDTH;
                            y = message.payload.widget.screenY / OVERLAY_PICKER_BOX_HEIGHT;
                            if (x >= OVERLAY_PICKER_COLUMNS || y >= OVERLAY_PICKER_ROWS)
                                break;
                            if (x < 0 || y < 0)
                                break;
                            gPickedOverlay = gPickerFirst + y * OVERLAY_PICKER_COLUMNS + x;
                            if (gPickedOverlay < m_typeCount)
                                done = true;
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_NUMPAD_9:
                    gPickerFirst -= OVERLAY_PICKER_PAGE;
                    if (gPickerFirst < 0)
                        gPickerFirst = 0;
                    needDraw = true;
                    break;
                case INPUT_SCAN_NUMPAD_3:
                    if (m_typeCount < OVERLAY_PICKER_PAGE)
                        break;
                    gPickerFirst += OVERLAY_PICKER_PAGE;
                    if (gPickerFirst + OVERLAY_PICKER_PAGE >= m_typeCount)
                        gPickerFirst =
                            m_typeCount - OVERLAY_PICKER_PAGE
                            + (OVERLAY_PICKER_COLUMNS - m_typeCount % OVERLAY_PICKER_COLUMNS)
                                  % OVERLAY_PICKER_COLUMNS;
                    needDraw = true;
                    break;
                case INPUT_SCAN_ESCAPE:
                    gPickedOverlay = OVERLAY_NONE;
                    done = true;
                    break;
            }
            break;
    }
    if (needDraw)
        DrawPicker(true);
    if (done) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        FINISH_EDIT_DIALOG(message);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0041cb93, 0x197)
void overlayManager::DragPickerKnob(b32 trackClick, i32 mouseX, i32 mouseY) {
    double rowHeight;
    tag_message latest;
    tag_message message;
    i32 top;

    if (gPickerRows <= OVERLAY_PICKER_ROWS)
        return;
    rowHeight = static_cast<double>(OVERLAY_PICKER_KNOB_SCALE_SPAN) / (gPickerRows - (OVERLAY_PICKER_ROWS - 1));
    if (mouseX == EDIT_NO_CELL)
        gpMouseManager->MouseCoords(mouseX, mouseY);
    gpInputManager->Flush();
    message.type = MESSAGE_MOUSE_MOVE;
    message.payload.mouse.x = mouseX;
    message.payload.mouse.y = mouseY;
    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        Process1WindowsMessage();
        if (message.type == MESSAGE_MOUSE_MOVE) {
            latest = message;
            while (message.type == MESSAGE_MOUSE_MOVE) {
                latest = message;
                message = gpInputManager->GetEvent();
            }
            gpMouseManager->Main(latest);
            top = latest.payload.mouse.y;
            top = (top - OVERLAY_PICKER_DRAG_ORIGIN) / rowHeight;
            top = top + 0.5;
            if (top < 0)
                top = 0;
            if (top > gPickerRows - OVERLAY_PICKER_ROWS)
                top = gPickerRows - OVERLAY_PICKER_ROWS;
            top = top * OVERLAY_PICKER_COLUMNS;
            if (gPickerFirst != top) {
                gPickerFirst = top;
                DrawPicker(true);
            }
            if (trackClick)
                return;
        } else
            message = gpInputManager->GetEvent();
    }
}

// Compiler-emitted vtable; the marker is a census claim, not a definition.
VTBL(overlayManager, 0x0045b3f4)
