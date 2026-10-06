// The object tool (overlayManager) and the placement of objects on the map.
// The unit name comes from the Price of Loyalty editor's assertion path
// (overlay.cpp); Open stores the class name "overlayManager".

#include <va.h>
#include <EDITOR/OVERLAY.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/clearManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/heroedit.h>
#include <EDITOR/townedit.h>
#include <BASE/border.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/icon2bs.h>
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
#include <SOURCE/CONFIG_TYPES.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/hero.h>
#include <SOURCE/X_GLOBAL.h>
#include <stdio.h>
#include <string.h>

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
DATA(0x004a5740) static i32 gPickerRows;
DATA(0x004a5744) static i32 gPickerFirst;
DATA(0x004a5748) static i32 gPickedOverlay;

VA(0x00418dc0, 0x67)
i32 OverlayGridHas(u32* grid, i32 x, i32 y) {
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

    m_previewDrawn = 0;
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
    ShowClass(0);
    gEditManager->m_placedY = EDIT_NO_CELL;
    gEditManager->m_placedX = EDIT_NO_CELL;
    LoadClass(gObjectClass);
    DrawSelectedOverlay();
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = 1;
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
    gEditManager->m_window->DrawWindow(0);
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
    m_active = 0;
}

VA(0x004192a2, 0xcc)
void overlayManager::ShowClass(i32 update) {
    tag_message message;
    i32 i;

    sprintf(gText, gObjectClassNames[gObjectClass]);
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.id = OVERLAY_CLASS_NAME_ID;
    message.payload.widget.data.text = gText;
    gEditManager->m_window->BroadcastMessage(message);
    m_highlight->m_x = gClassButtonPositions[gObjectClass].x - OVERLAY_HIGHLIGHT_INSET;
    m_highlight->m_y = gClassButtonPositions[gObjectClass].y - OVERLAY_HIGHLIGHT_INSET;
    gEditManager->m_window->DrawWindow(0);
    if (update)
        gpWindowManager->UpdateScreenRegion(
            OVERLAY_PANEL_REGION_X,
            OVERLAY_PANEL_REGION_Y,
            OVERLAY_PANEL_REGION_WIDTH,
            OVERLAY_PANEL_REGION_HEIGHT
        );
}

VA(0x0041936e, 0x49a)
MessageDispatchResult overlayManager::Main(tag_message& message) {
    i32 result;
    b32 finished;
    i32 helpItem;
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
                                   < OVERLAY_CLASS_BUTTON_ID_FIRST + OVERLAY_CLASS_COUNT)
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
                            result = PlaceOverlay(
                                &m_types[gSelectedOverlay],
                                gEditManager->m_cursorX - OVERLAY_ANCHOR_X,
                                gEditManager->m_cursorY - OVERLAY_ANCHOR_Y,
                                1
                            );
                            if (result) {
                                gEditManager->m_placedX = gEditManager->m_cursorX;
                                gEditManager->m_placedY =
                                    gEditManager->m_cursorY + (OVERLAY_GRID_HEIGHT - m_height);
                                gEditManager->m_placedState = 0;
                                gEditManager->DrawMap();
                                gEditManager->UpdateMapView();
                                gEditManager->DrawRadar(1);
                                gEditManager->m_mapChanged = 1;
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
                               < OVERLAY_CLASS_BUTTON_ID_FIRST + OVERLAY_CLASS_COUNT) {
                        gObjectClass = message.payload.widget.id - OVERLAY_CLASS_BUTTON_ID_FIRST;
                    pick:
                        gSelectedOverlay = PickOverlay(gObjectClass);
                        DrawSelectedOverlay();
                        ShowClass(1);
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
                        mapX = mapX * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_ORIGIN;
                        mapY = mapY * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_ORIGIN;
                        if (gSelectedOverlay != OVERLAY_NONE) {
                            DrawFootprint(
                                mapX,
                                mapY,
                                OVERLAY_GRID_WIDTH,
                                OVERLAY_GRID_HEIGHT,
                                &m_types[gSelectedOverlay],
                                1
                            );
                            DrawOverlay(
                                &m_types[gSelectedOverlay],
                                mapX,
                                mapY,
                                1,
                                OVERLAY_GRID_WIDTH,
                                OVERLAY_GRID_HEIGHT,
                                1,
                                gEditManager->m_cursorX,
                                gEditManager->m_cursorY
                            );
                            m_previewDrawn = 1;
                        }
                        gEditManager->UpdateMapView();
                        gEditManager->UpdateCursor();
                    }
                    gEditManager->UpdateCursor();
                }
            } else if (m_previewDrawn) {
                m_previewDrawn = 0;
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

VA(0x00419808, 0x1dd)
void overlayManager::DrawFootprint(
    i32 left,
    i32 top,
    i32 width,
    i32 height,
    overlayType* type,
    i32 clip
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
                    || (left + (x - startX) * cellSize >= EDIT_VIEW_ORIGIN
                        && left + (x - startX + 1) * cellSize <= EDIT_VIEW_END
                        && top + (y - startY) * cellSize >= EDIT_VIEW_ORIGIN
                        && top + (y - startY + 1) * cellSize <= EDIT_VIEW_END))
                    MonoIconToBitmap(
                        m_cellIcon,
                        gpWindowManager->m_screen,
                        left + (x - startX) * cellSize,
                        top + (y - startY) * cellSize,
                        gEditManager->m_zoomLevel,
                        color,
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE
                    );
            }
}

VA(0x004199e5, 0x631)
i32 CanPlaceOverlay(overlayType* type, i32 left, i32 top, i32 overObjects) {
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
        return 0;
    for (y = 0; y < OVERLAY_GRID_HEIGHT; y++)
        for (x = 0; x < OVERLAY_GRID_WIDTH; x++) {
            if (OverlayGridHas(shape->occupiedRows, x, y)
                && !OverlayGridHas(shape->shadowRows, x, y)) {
                if (left + x < 0 || left + x > MAP_WIDTH - 1 || top + y < 0
                    || top + y > MAP_HEIGHT - 1)
                    return 0;
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
                    if (!(shape->groundMask & 1 << CELL_TERRAIN(gMap.CellAt(left + x, top + y))))
                        return 0;
                    if (OverlayGridHas(shape->entranceRows, x, y)) {
                        if (cell->m_objectIndex != MAPCELL_SPRITE_NONE) {
                            blocked = 0;
                            if (!cell->m_objectLayerBit1)
                                blocked = 1;
                            if (cell->m_extraIndex
                                && gMap.extras[cell->m_extraIndex].objectIndex
                                       != MAPCELL_SPRITE_NONE)
                                part = &gMap.extras[cell->m_extraIndex];
                            else
                                part = NULL;
                            while (part) {
                                if (!part->objectLayerBit1)
                                    blocked = 1;
                                if (part->nextIndex
                                    && gMap.extras[part->nextIndex].objectIndex
                                           != MAPCELL_SPRITE_NONE)
                                    part = &gMap.extras[part->nextIndex];
                                else
                                    part = NULL;
                            }
                            if (blocked && shape->trigger != IDX(MAP_OBJECT_MINE))
                                return 0;
                        }
                    } else if (HAS(cell->m_triggerType, MAP_TRIGGER_ACTION_FLAG)
                               && !OverlayGridHas(shape->shadowRows, x, y))
                        return 0;
                    if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE
                        || (cell->m_objectIndex != MAPCELL_SPRITE_NONE && !cell->m_objectLayerBit0
                            && shape->highLayer))
                        gKeptLinks[kept++] = cell->m_overlayLink;
                    if (cell->m_objectIndex != MAPCELL_SPRITE_NONE && !overObjects)
                        return 0;
                    if (cell->m_objectIndex != MAPCELL_SPRITE_NONE
                        && (!shape->highLayer || cell->m_objectLayerBit0))
                        gCoveredLinks[covered++] = cell->m_objectLink;
                    if (cell->m_extraIndex) {
                        part = &gMap.extras[cell->m_extraIndex];
                        while (part) {
                            if (part->overlayIndex != MAPCELL_SPRITE_NONE
                                || (part->objectIndex != MAPCELL_SPRITE_NONE
                                    && !part->objectLayerBit0 && shape->highLayer))
                                gKeptLinks[kept++] = part->overlayLink;
                            if (part->objectIndex != MAPCELL_SPRITE_NONE
                                && (!shape->highLayer || part->objectLayerBit0))
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
                return 0;
    return 1;
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
                    && cell->m_overlayTileset == type->tileset)
                    gEditManager->RemoveLinkedObject(cell->m_overlayLink);
                if (cell->m_objectIndex == type->frames[i]
                    && cell->m_objectTileset == type->tileset)
                    gEditManager->RemoveLinkedObject(cell->m_objectLink);
                if (cell->m_extraIndex) {
                    extra = &gMap.extras[cell->m_extraIndex];
                    while (extra) {
                        if (extra->overlayIndex == type->frames[i]
                            && extra->overlayTileset == type->tileset)
                            gEditManager->RemoveLinkedObject(extra->overlayLink);
                        if (extra->objectIndex == type->frames[i]
                            && extra->objectTileset == type->tileset)
                            gEditManager->RemoveLinkedObject(extra->objectLink);
                        extra = extra->nextIndex ? &gMap.extras[extra->nextIndex] : NULL;
                    }
                }
            }
        }
}

VA(0x0041a26c, 0x14d6)
i32 PlaceOverlay(overlayType* type, i32 x, i32 y, i32 newLink) {
    i32 idx;
    i32 col;
    mapCellExtra* node;
    i32 row;
    mapCell* dest;
    i32 spotX;
    mapCell* landing;
    i32 groundNum;
    i32 spotY;
    i32 shadowNum;
    i32 done;
    TownExtra* newCastle;
    signEventExtra* bottle;
    EventExtra* pEvent;
    HeroExtra* jail;
    mapEventExtra* sphinx;
    i32 extraIndex;
    i32 kind;

    if (newLink)
        gNextObjectLink++;
    if (!CanPlaceOverlay(type, x, y, 1)) {
        LogStr("Invalid Placement");
        ShowStatusWarning(localization::Tr("editor.overlay.invalid_placement"));
        return 0;
    }
    RemoveReplacedObjects(type, x, y);
    if (type->category == OVERLAY_CATEGORY_TOWN && gEditManager->CountTowns() >= GAME_TOWN_COUNT) {
        sprintf(gText, localization::Tr("editor.overlay.limit.towns"), GAME_TOWN_COUNT);
        ShowStatusWarning(gText);
        return 0;
    }
    if (type->trigger == IDX(MAP_OBJECT_MAP_EVENT)
        && gEditManager->CountEvents() >= OVERLAY_EVENT_LIMIT) {
        sprintf(gText, localization::Tr("editor.overlay.limit.events"), OVERLAY_EVENT_LIMIT);
        ShowStatusWarning(gText);
        return 0;
    }
    if ((type->trigger & MAP_TRIGGER_TYPE_MASK) == IDX(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT)) {
        for (col = 0; col < MAP_WIDTH; col++)
            for (row = 0; row < MAP_WIDTH; row++)
                if ((gMap.CellAt(col, row)->m_triggerType & MAP_TRIGGER_TYPE_MASK)
                    == IDX(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT)) {
                    sprintf(gText, localization::Tr("editor.overlay.ultimate.placed"));
                    ShowStatusWarning(gText);
                    return 0;
                }
        spotX = x + OVERLAY_ANCHOR_X;
        spotY = y + OVERLAY_ANCHOR_Y;
        if (spotX < OVERLAY_ULTIMATE_MARGIN || spotX > MAP_WIDTH - OVERLAY_ULTIMATE_MARGIN - 1
            || spotY < OVERLAY_ULTIMATE_MARGIN
            || spotY > MAP_HEIGHT - OVERLAY_ULTIMATE_MARGIN - 1) {
            sprintf(gText, localization::Tr("editor.overlay.ultimate.edge"));
            ShowStatusWarning(gText);
            return 0;
        }
        landing = gMap.CellAt(x + OVERLAY_ANCHOR_X, y + OVERLAY_ANCHOR_Y);
        if (CELL_TERRAIN(landing) == TERRAIN_WATER) {
            sprintf(gText, localization::Tr("editor.overlay.ultimate.land"));
            ShowStatusWarning(gText);
            return 0;
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
        return 0;
    }
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        shadowNum = -1;
        groundNum = -1;
        if (type->id >= OVERLAY_TOWN_FIRST && type->id <= OVERLAY_TOWN_LAST)
            shadowNum =
                (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS + OVERLAY_TOWN_SHADOWS;
        else
            shadowNum = (type->id - OVERLAY_RANDOM_TOWN_FIRST) % 2 + OVERLAY_RANDOM_TOWN_SHADOWS;
        groundNum =
            CELL_TERRAIN(gMap.CellAt(x + OVERLAY_TOWN_ENTRANCE_X, y + OVERLAY_TOWN_ENTRANCE_Y))
            + OVERLAY_TOWN_GROUNDS;
        done = PlaceOverlay(&gOverlayTypes[shadowNum], x, y, 0);
        done = PlaceOverlay(&gOverlayTypes[groundNum], x, y, 0);
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
                            node->overlayTileset = type->tileset;
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
                            dest->m_overlayTileset = type->tileset;
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
                            node->objectTileset = type->tileset;
                            if (OverlayGridHas(type->shadowRows, col, row))
                                node->objectLayerBit1 = 1;
                            else
                                node->objectLayerBit1 = 0;
                            if (row < OVERLAY_GRID_HEIGHT - 1 && !type->highLayer
                                && !OverlayGridHas(type->shadowRows, col, row)
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                node->objectDrawnAsOverlay = 1;
                            else
                                node->objectDrawnAsOverlay = 0;
                            if (type->highLayer)
                                node->objectLayerBit0 = 1;
                            else
                                node->objectLayerBit0 = 0;
                            if (OverlayGridHas(type->animatedRows, col, row))
                                node->animatedObject = 1;
                            else
                                node->animatedObject = 0;
                        } else {
                            if (giGroundShape[dest->m_terrainImageIndex]
                                & GROUND_SHAPE_VARIANT_FLAG)
                                dest->m_terrainImageIndex = ChooseGroundTile(
                                    CELL_TERRAIN(dest),
                                    giGroundShape[dest->m_terrainImageIndex]
                                        - GROUND_SHAPE_VARIANT_FLAG,
                                    0,
                                    0,
                                    0,
                                    0,
                                    1.0f
                                );
                            dest->m_objectLink = gNextObjectLink;
                            dest->m_objectIndex = type->frames[idx];
                            dest->m_objectTileset = type->tileset;
                            if (OverlayGridHas(type->shadowRows, col, row))
                                dest->m_triggerType = MAP_OBJECT_NONE;
                            else
                                dest->m_triggerType = type->trigger;
                            if (OverlayGridHas(type->shadowRows, col, row))
                                dest->m_objectLayerBit1 = 1;
                            else
                                dest->m_objectLayerBit1 = 0;
                            if (row < OVERLAY_GRID_HEIGHT - 1 && !type->highLayer
                                && !OverlayGridHas(type->shadowRows, col, row)
                                && OverlayGridHas(type->occupiedRows, col, row + 1)
                                && !OverlayGridHas(type->shadowRows, col, row + 1))
                                dest->m_objectDrawnAsOverlay = 1;
                            else
                                dest->m_objectDrawnAsOverlay = 0;
                            if (type->highLayer)
                                dest->m_objectLayerBit0 = 1;
                            else
                                dest->m_objectLayerBit0 = 0;
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
                        newCastle = new TownExtra;
                        memset(newCastle, 0, sizeof(TownExtra));
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = newCastle;
                        gEditMapHeader.townNameIndex = (gEditMapHeader.townNameIndex + 1) % EDITOR_TOWN_NAME_COUNT;
                        strcpy(newCastle->name, gTownNames[gEditMapHeader.townNameIndex]);
                        newCastle->owner = type->color == OVERLAY_NO_COLOR ? -1 : type->color;
                        if (type->id >= OVERLAY_RANDOM_TOWN_FIRST
                            && type->id <= OVERLAY_RANDOM_TOWN_LAST) {
                            newCastle->faction = OVERLAY_RANDOM_TOWN_FACTION;
                            newCastle->isCastle = 1 - (type->id - OVERLAY_RANDOM_TOWN_FIRST) % 2;
                        } else {
                            newCastle->faction =
                                (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS / 2;
                            newCastle->isCastle = 1 - (type->id - OVERLAY_TOWN_FIRST) % 2;
                        }
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(TownExtra);
                        gEditManager->m_extraCount++;
                    }
                    if ((dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE))
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        bottle = new signEventExtra;
                        memset(bottle, 0, sizeof(signEventExtra));
                        bottle->pad[0] = MAP_EVENT_DATA_AVAILABLE;
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = bottle;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] =
                            sizeof(signEventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        pEvent = new EventExtra;
                        memset(pEvent, 0, sizeof(EventExtra));
                        pEvent->isMapEvent = 1;
                        pEvent->artifact = -1;
                        pEvent->cancelAfterVisit = 1;
                        for (idx = 0; idx < GAME_PLAYER_COUNT; idx++)
                            pEvent->players[idx] = 1;
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = pEvent;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(EventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX)
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        sphinx = new mapEventExtra;
                        memset(sphinx, 0, sizeof(mapEventExtra));
                        sphinx->artifact = -1;
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = sphinx;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] =
                            sizeof(mapEventExtra);
                        gEditManager->m_extraCount++;
                    }
                    if ((dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_HERO)
                         || dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))
                        && OverlayGridHas(type->entranceRows, col, row)) {
                        jail = new HeroExtra;
                        memset(jail, 0, sizeof(HeroExtra));
                        for (idx = 0; idx < ARMY_GROUP_SLOT_COUNT; idx++)
                            jail->troopTypes[idx] = -1;
                        for (idx = 0; idx < EVENT_RECORD_HERO_ARTIFACT_COUNT; idx++)
                            jail->artifacts[idx] = -1;
                        for (idx = 0; idx < HERO_SECONDARY_SKILL_CAPACITY; idx++)
                            jail->skillTypes[idx] = -1;
                        if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))
                            kind = IDX(FACTION_KNIGHT);
                        else
                            kind = dest->m_objectIndex % OVERLAY_HERO_FRAMES_PER_COLOR;
                        if (kind == IDX(FACTION_KNIGHT)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_LEADERSHIP);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_BASIC;
                            jail->skillTypes[1] = IDX(HERO_SKILL_BALLISTICS);
                            jail->skillLevels[1] = HERO_SKILL_LEVEL_BASIC;
                        }
                        if (kind == IDX(FACTION_SORCERESS)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_NAVIGATION);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_ADVANCED;
                            jail->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            jail->skillLevels[1] = HERO_SKILL_LEVEL_BASIC;
                        }
                        if (kind == IDX(FACTION_BARBARIAN)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_PATHFINDING);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_ADVANCED;
                        }
                        if (kind == IDX(FACTION_WARLOCK)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_SCOUTING);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_ADVANCED;
                            jail->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            jail->skillLevels[1] = HERO_SKILL_LEVEL_BASIC;
                        }
                        if (kind == IDX(FACTION_WIZARD)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_WISDOM);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_ADVANCED;
                        }
                        if (kind == IDX(FACTION_NECROMANCER)) {
                            jail->skillTypes[0] = IDX(HERO_SKILL_NECROMANCY);
                            jail->skillLevels[0] = HERO_SKILL_LEVEL_BASIC;
                            jail->skillTypes[1] = IDX(HERO_SKILL_WISDOM);
                            jail->skillLevels[1] = HERO_SKILL_LEVEL_BASIC;
                        }
                        dest->m_objectMetadata = gEditManager->m_extraCount;
                        gEditManager->m_extras[gEditManager->m_extraCount] = jail;
                        gEditManager->m_extraSizes[gEditManager->m_extraCount] = sizeof(HeroExtra);
                        gEditManager->m_extraCount++;
                    }
                    if (dest->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT)
                        && dest->m_objectIndex / 2 == IDX(ARTIFACT_SPELL_SCROLL))
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
                            0
                        );
                }
            }
        }
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        PlaceOverlay(
            &gOverlayTypes[OVERLAY_TOWN_FLAGS + type->color * 2],
            x + OVERLAY_LEFT_FLAG_X,
            y + OVERLAY_FLAG_Y,
            0
        );
        PlaceOverlay(
            &gOverlayTypes[OVERLAY_TOWN_FLAGS + 1 + type->color * 2],
            x + OVERLAY_RIGHT_FLAG_X,
            y + OVERLAY_FLAG_Y,
            0
        );
    }
    return 1;
}

VA(0x0041b742, 0xf1)
b32 PlaceResourceMarker(overlayType* type, i32 x, i32 y, i32 requireMine) {
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

VA(0x0041b833, 0x627)
void overlayManager::DrawOverlay(
    overlayType* type,
    i32 x,
    i32 y,
    i32 clip,
    i32 width,
    i32 height,
    i32 update,
    i32 cellX,
    i32 cellY
) {
    i32 gridX;
    i32 tileSize;
    i32 gridY;
    i32 fromX;
    i32 fromY;
    i32 cellTerrain;
    i32 count;
    i32 groundType;
    i32 shadow;

    fromY = OVERLAY_GRID_HEIGHT - height;
    fromX = OVERLAY_GRID_WIDTH - width;
    if (type->category == OVERLAY_CATEGORY_TOWN) {
        shadow = -1;
        groundType = -1;
        if (type->id >= OVERLAY_TOWN_FIRST && type->id <= OVERLAY_TOWN_LAST)
            shadow = (type->id - OVERLAY_TOWN_FIRST) % OVERLAY_TOWN_VARIANTS + OVERLAY_TOWN_SHADOWS;
        else
            shadow = (type->id - OVERLAY_RANDOM_TOWN_FIRST) % 2 + OVERLAY_RANDOM_TOWN_SHADOWS;
        groundType = OVERLAY_TOWN_GRASS_GROUND;
        if (cellX != EDIT_NO_CELL) {
            if (cellX < OVERLAY_TOWN_GROUND_COLUMN)
                cellX = OVERLAY_TOWN_GROUND_COLUMN;
            if (cellY < 0)
                cellY = 0;
            if (cellX > MAP_WIDTH - 1)
                cellX = MAP_WIDTH - 1;
            if (cellY > MAP_HEIGHT - 1)
                cellY = MAP_HEIGHT - 1;
            cellTerrain = CELL_TERRAIN(gMap.CellAt(cellX - OVERLAY_TOWN_GROUND_COLUMN, cellY));
            if (cellTerrain != IDX(TERRAIN_WATER))
                groundType = cellTerrain + OVERLAY_TOWN_GROUNDS;
        }
        DrawOverlay(
            &gOverlayTypes[shadow],
            x,
            y,
            clip,
            width,
            height,
            0,
            EDIT_NO_CELL,
            EDIT_NO_CELL
        );
        DrawOverlay(
            &gOverlayTypes[groundType],
            x,
            y,
            clip,
            width,
            height,
            0,
            EDIT_NO_CELL,
            EDIT_NO_CELL
        );
    }
    tileSize = gZoomTileSize[gEditManager->m_zoomLevel];
    for (gridY = fromY; gridY < OVERLAY_GRID_HEIGHT; gridY++)
        for (gridX = fromX; gridX < OVERLAY_GRID_WIDTH; gridX++)
            if (type->frames[gridX + gridY * OVERLAY_GRID_WIDTH] != OVERLAY_NO_FRAME
                && (!clip
                    || (x + (gridX - fromX) * tileSize >= EDIT_VIEW_ORIGIN
                        && x + (gridX - fromX + 1) * tileSize <= EDIT_VIEW_END
                        && y + (gridY - fromY) * tileSize >= EDIT_VIEW_ORIGIN
                        && y + (gridY - fromY + 1) * tileSize <= EDIT_VIEW_END))) {
                if (type->tileset == TILESET_MINIHERO)
                    IconToBitmapScaleShadow(
                        gEditManager->m_objectIcons[type->tileset][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize
                            - OVERLAY_HERO_LIFT / gZoomScale[gEditManager->m_zoomLevel],
                        type->frames[gridX + gridY * OVERLAY_GRID_WIDTH],
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                else
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[type->tileset][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->frames[gridX + gridY * OVERLAY_GRID_WIDTH],
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (OverlayGridHas(type->animatedRows, gridX, gridY)) {
                    count = GetIconEntry(
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
                            + gEditManager->m_animationCounter % count,
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                }
                if (type->flags & OVERLAY_FLAG_SHOWS_RESOURCE
                    && OverlayGridHas(type->resourceRows, gridX, gridY))
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[TILESET_EXTRAOVR][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color,
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (type->category == OVERLAY_CATEGORY_TOWN && gridX == OVERLAY_TOWN_FLAG_COLUMN
                    && gridY == OVERLAY_TOWN_FLAG_ROW)
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[TILESET_FLAG32][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color * 2,
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
                        gZoomCellSize[gEditManager->m_zoomLevel]
                    );
                if (type->category == OVERLAY_CATEGORY_TOWN
                    && gridX == OVERLAY_TOWN_RIGHT_FLAG_COLUMN && gridY == OVERLAY_TOWN_FLAG_ROW)
                    IconToBitmapScale(
                        gEditManager->m_objectIcons[TILESET_FLAG32][0],
                        gpWindowManager->m_screen,
                        x + (gridX - fromX) * tileSize,
                        y + (gridY - fromY) * tileSize,
                        type->color * 2 + 1,
                        clip,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_ORIGIN,
                        EDIT_VIEW_SIZE,
                        EDIT_VIEW_SIZE,
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

VA(0x0041be5a, 0x24c)
i32 overlayManager::LoadClass(i32 objectClass) {
    i32 unusedCount;
    i32 changed;
    i32 unusedFlag;
    overlayType tmp;
    i32 unusedIndex;
    i32 i;
    i32 j;

    m_typeCount = 0;
    unusedCount = 0;
    for (i = 0; i < OVERLAY_TYPE_COUNT; i++)
        if (gOverlayTypes[i].category == gObjectClassCategories[objectClass]
            && gOverlayTypes[i].terrainMask & gObjectClassTerrains[objectClass])
            m_types[m_typeCount++] = gOverlayTypes[i];
    for (i = 0; i < m_typeCount; i++) {
        changed = 0;
        for (j = m_typeCount - 1; j > 0; j--)
            if (m_types[j].ordinal < m_types[j - 1].ordinal) {
                tmp = m_types[j];
                m_types[j] = m_types[j - 1];
                m_types[j - 1] = tmp;
                changed = 1;
            }
        if (!changed)
            break;
    }
    if (m_typeCount)
        return 1;
    return 0;
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

VA(0x0041c16f, 0x120)
i32 overlayManager::SelectOverlay(i32 index) {
    i32 objectClass;
    i32 slot;
    i32 was;
    i32 x;
    tag_message message;
    i32 y;

    was = gObjectClass;
    for (objectClass = OVERLAY_CLASS_COUNT - 1; objectClass >= 0; objectClass--) {
        gObjectClass = objectClass;
        LoadClass(gObjectClass);
        for (slot = 0; slot < m_typeCount; slot++)
            if (m_types[slot].id == gOverlayTypes[index].id) {
                gSelectedOverlay = slot;
                DrawSelectedOverlay();
                ShowClass(1);
                gEditManager->m_cursorX = EDIT_NO_CELL;
                message.type = MESSAGE_MOUSE_MOVE;
                gpMouseManager->MouseCoords(x, y);
                message.payload.mouse.screenX = message.payload.mouse.x = x;
                message.payload.mouse.screenY = message.payload.mouse.y = y;
                Main(message);
                return 1;
            }
    }
    gSelectedOverlay = OVERLAY_NONE;
    gObjectClass = was;
    return 0;
}

VA(0x0041c28f, 0x2ba)
void overlayManager::DrawPicker(i32 update) {
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
                                * (393.0 / (gPickerRows - (OVERLAY_PICKER_ROWS - 1) - 1.0))
                            + 19.0;
    else
        m_pickerKnob->m_y = OVERLAY_PICKER_KNOB_PARKED;
    m_picker->DrawWindow(0);
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
        DrawFootprint(col, row, OVERLAY_PICKER_VIEW_WIDTH, OVERLAY_PICKER_VIEW_HEIGHT, shown, 0);
        DrawOverlay(
            shown,
            col,
            row,
            0,
            OVERLAY_PICKER_VIEW_WIDTH,
            OVERLAY_PICKER_VIEW_HEIGHT,
            0,
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
    i32 needDraw;
    i32 done;
    tag_message message;
    i32 zoom;

    zoom = gEditManager->m_zoomLevel;
    done = 0;
    needDraw = 1;
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
    DrawPicker(1);
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
                                1,
                                message.payload.widget.screenX,
                                message.payload.widget.screenY
                            );
                            break;
                        case OVERLAY_PICKER_KNOB_ID:
                            DragPickerKnob(0, EDIT_NO_CELL, EDIT_NO_CELL);
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
                case OVERLAY_KEY_PAGE_UP:
                    gPickerFirst -= OVERLAY_PICKER_PAGE;
                    if (gPickerFirst < 0)
                        gPickerFirst = 0;
                    needDraw = true;
                    break;
                case OVERLAY_KEY_PAGE_DOWN:
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
                case OVERLAY_KEY_ESCAPE:
                    gPickedOverlay = OVERLAY_NONE;
                    done = true;
                    break;
            }
            break;
    }
    if (needDraw)
        DrawPicker(1);
    if (done) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        FINISH_EDIT_DIALOG(message);
        return MESSAGE_DISPATCH_FORWARD;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0041cb93, 0x197)
void overlayManager::DragPickerKnob(i32 trackClick, i32 mouseX, i32 mouseY) {
    double rowHeight;
    tag_message latest;
    tag_message message;
    i32 top;

    if (gPickerRows <= OVERLAY_PICKER_ROWS)
        return;
    rowHeight = 402.0 / (gPickerRows - (OVERLAY_PICKER_ROWS - 1));
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
                DrawPicker(1);
            }
            if (trackClick)
                return;
        } else
            message = gpInputManager->GetEvent();
    }
}

// Compiler-emitted vtable; the marker is a census claim, not a definition.
VTBL(overlayManager, 0x0045b3f4)
