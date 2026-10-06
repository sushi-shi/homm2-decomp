// The scenario editor's main manager (EDITMGR, PoL editor editmgr.cpp): the
// map view and its rulers, the radar and scroll knobs, the tool panel, the
// map file and the edits the tool managers ask for.
// Descriptive names: DrawRulers, DrawView, InitializeMap, FillInOverlayTiles.

#include <va.h>
#include <EDITOR/editManager.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/setup.h>
#include <EDITOR/townedit.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/CONFIG_TYPES.h>
#include <BASE/bitmap.h>
#include <BASE/font.h>
#include <BASE/bmap2.h>
#include <BASE/icon2bs.h>
#include <BASE/IconEntry.h>
#include <BASE/Misc.h>
#include <BASE/TILE.h>
#include <BASE/icon.h>
#include <BASE/tileset.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/IconDraw.h>
#include <BASE/iconWidget.h>
#include <BASE/mouseManager.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <BASE/resourceManager.h>
#include <BASE/widgetKind.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(EditViewGeometry)
    // The map view's 448x448 square at (16, 16).
    EDIT_VIEW_LEFT   = 0x10,
    EDIT_VIEW_TOP    = 0x10,
    EDIT_VIEW_PIXELS = 0x1c0,
    // The scroll tracks and knobs (escroll.icn frames 0..3).
    EDIT_TRACK_LONG   = 0x1a0,
    EDIT_TRACK_SHORT  = 0x10,
    EDIT_TRACK_START  = 0x20,
    EDIT_TRACK_EDGE   = 0x1d0,
    EDIT_KNOB_START   = 0x23,
    EDIT_KNOB_EDGE    = 0x1d4,
    EDIT_KNOB_LONG    = 0x11,
    EDIT_KNOB_SHORT   = 8,
    EDIT_SCROLL_HORIZONTAL_TRACK = 0,
    EDIT_SCROLL_VERTICAL_TRACK   = 1,
    EDIT_SCROLL_HORIZONTAL_KNOB  = 2,
    EDIT_SCROLL_VERTICAL_KNOB    = 3,
    EDIT_CONTROL_HORIZONTAL_TRACK = 0xa,
    EDIT_CONTROL_VERTICAL_TRACK   = 0xb,
    EDIT_CONTROL_HORIZONTAL_KNOB  = 0xc,
    EDIT_CONTROL_VERTICAL_KNOB    = 0xd,
    // A map the editor closes to (72x72, a medium map).
    EDIT_DEFAULT_MAP_SIZE = 0x48
H2_ENUM_END(EditViewGeometry)

H2_ENUM_BEGIN(EditRulerGeometry)
    // A ruler numbers every view cell, every second 16-pixel slot at the
    // normal zoom, its number centred half a slot in.
    EDIT_RULER_SLOTS            = 0x1c,
    EDIT_RULER_SLOT_PIXELS      = 0x10,
    EDIT_RULER_CELL_TEXT_OFFSET = 8,
    EDIT_TOP_RULER_TEXT_X       = 0x13,
    EDIT_TOP_RULER_TEXT_Y       = 5,
    EDIT_LEFT_RULER_TEXT_X      = 3,
    EDIT_LEFT_RULER_TEXT_Y      = 0x15,
    // editbtns.icn frames: the ruler cells at each zoom.
    EDIT_FRAME_ZOOMED_RULER_CELL = 0x20,
    EDIT_FRAME_LEFT_RULER_CELL   = 0x21,
    EDIT_FRAME_TOP_RULER_CELL    = 0x22,
    // The zoom levels; the farthest one numbers every second cell.
    EDIT_ZOOM_NORMAL = 0,
    EDIT_ZOOM_HALF   = 1,
    EDIT_ZOOM_FAR    = 2
H2_ENUM_END(EditRulerGeometry)

H2_ENUM_BEGIN(EditCellLayer)
    // DrawCell's passes: the ground and its overlays, the objects, and the
    // objects that hang over the cell above.
    EDIT_DRAW_GROUND      = 1,
    EDIT_DRAW_OBJECTS     = 2,
    EDIT_DRAW_OVERHANGING = 4,
    // The animation frame wraps here.
    EDIT_ANIMATION_FRAMES = 6,
    // The drag selection's outline colour.
    EDIT_SELECTION_COLOR  = 0xb5
H2_ENUM_END(EditCellLayer)

H2_ENUM_BEGIN(EditRadarGeometry)
    // The radar at (480, 16) of the 640-pixel screen: a 144-pixel square a
    // small map fills with 4x4 dots, a medium one with 2x2, a large one with
    // a 2-1-1 pattern of rows and columns, an extra large one with 1x1.
    EDIT_RADAR_LEFT   = 0x1e0,
    EDIT_RADAR_TOP    = 0x10,
    EDIT_RADAR_OFFSET = 0x29e0,
    EDIT_SCREEN_PITCH = 0x280,
    EDIT_MAP_SMALL    = 0x24,
    EDIT_MAP_MEDIUM   = 0x48,
    EDIT_MAP_LARGE    = 0x6c,
    EDIT_MAP_XLARGE   = 0x90,
    EDIT_RADAR_PHASES = 3,
    // radar.icn: the view outline per zoom level and map size; the
    // obstacle, town and unseen colours.
    EDIT_RADAR_OBSTACLE_SHADE = 3,
    EDIT_RADAR_TOWN_COLOR     = 0x11,
    EDIT_RADAR_UNSEEN_COLOR   = 0,
    EDIT_RADAR_VIEW_COLOR     = 0xb5
H2_ENUM_END(EditRadarGeometry)

H2_ENUM_BEGIN(EditCellDrawing)
    // Monsters stand 5 pixels and heroes 14 above their cell (at the normal
    // zoom); both draw clipped to the 480-pixel view square, everything else
    // to the screen.
    EDIT_MONSTER_LIFT   = 5,
    EDIT_HERO_LIFT      = 0xe,
    EDIT_CLIP_VIEW      = 0x1e0,
    EDIT_CLIP_SCREEN_W  = 0x280,
    EDIT_CLIP_SCREEN_H  = 0x1e0,
    // An animated part's frame count is its icon entry's flags; the trigger
    // 0xdf draws one frame fewer.
    EDIT_TRIGGER_ONE_FRAME_LESS = 0xdf,
    // The layer passes: the high layer, the middle one, the ground layer.
    EDIT_LAYER_HIGH  = 2,
    EDIT_LAYER_MID   = 1,
    EDIT_LAYER_LOW   = 0,
    // Clouds hide the map while the generator works: one of four tiles.
    EDIT_CLOUD_TILE_MASK = 3,
    EDIT_TILE_FLAG_SHIFT = 14
H2_ENUM_END(EditCellDrawing)

H2_ENUM_BEGIN(EditKnobGeometry)
    // A scroll knob travels 393 pixels from 35; at the end of a map too
    // small to scroll it parks at 231.
    EDIT_KNOB_TRAVEL      = 393,
    EDIT_KNOB_FIRST       = 35,
    EDIT_KNOB_PARKED      = 231,
    EDIT_KNOB_SCALE_SPAN  = 402,
    EDIT_CONTROL_SCROLL_LAST = 0x15,
    // ToggleZoom's view shifts keep the view centred.
    EDIT_ZOOM_SHIFT_SMALL = 7,
    EDIT_ZOOM_SHIFT_LARGE = 21,
    EDIT_ZOOM_SHIFT_BACK  = 14,
    // A ground cell's overlay-extra and hero-cursor flags.
    EDIT_CELL_GROUND_KEEP = 0x9f
H2_ENUM_END(EditKnobGeometry)

H2_ENUM_BEGIN(EditRadarDrag)
    // The radar's 144-pixel square; a knob's drag measures from its centre.
    EDIT_RADAR_SIZE      = 0x90,
    EDIT_KNOB_DRAG_ORIGIN = 0x27
H2_ENUM_END(EditRadarDrag)

H2_ENUM_BEGIN(EditManagerSetting)
    // Every editor manager's Main accepts these messages.
    EDIT_MANAGER_DISPATCH_MASK = 0x4000,
    EDIT_POINTER_DEFAULT = 0
H2_ENUM_END(EditManagerSetting)

H2_ENUM_BEGIN(EditMapCheck)
    // The three kinds of travel gate (stone liths) take three frames each.
    EDIT_TRAVEL_GATE_KINDS       = 3,
    EDIT_TRAVEL_GATE_KIND_FRAMES = 3,
    EDIT_TRAVEL_GATE_FRAMES      = 9,
    // A whirlpool's second trigger cell, from its first.
    EDIT_WHIRLPOOL_SECOND_CELL_X = 2,
    EDIT_WHIRLPOOL_SECOND_CELL_Y = 1,
    // A barrier's and a tent's colour: the low bits of the cell's metadata.
    EDIT_BARRIER_COLOR_MASK      = 7,
    EDIT_MAP_OBELISK_LIMIT       = 48
H2_ENUM_END(EditMapCheck)

H2_ENUM_BEGIN(EditMapFile)
    // The map file's town and capturable-site tables: a fixed number of
    // three-byte records, the unused ones empty.
    EDIT_MAP_TOWN_RECORDS = 72,
    EDIT_MAP_MINE_RECORDS = 144,
    EDIT_MAP_NO_RECORD    = 0xff,
    EDIT_MAP_CASTLE_FLAG  = 0x80,
    // A site record's type: the resource of a mine (ore and later from the
    // mine's overlay frame), or one of the special sites.
    EDIT_MINE_TYPE_WOOD           = 0,
    EDIT_MINE_TYPE_MERCURY        = 1,
    EDIT_MINE_TYPE_FIRST_ORE      = 2,
    EDIT_MINE_TYPE_LIGHTHOUSE     = 100,
    EDIT_MINE_TYPE_DRAGON_CITY    = 101,
    EDIT_MINE_TYPE_EYE_OF_MAGI    = 102,
    EDIT_MINE_TYPE_ABANDONED_MINE = 103
H2_ENUM_END(EditMapFile)

// The drag selection's outline colour and the tick the view last animated.
DATA(0x0049f5f0) i32 gSelectionColor;
DATA(0x0049f940) i32 gLastAnimationTick;
// DrawCell's working state: the map cell, its view position, the ground
// tile, the layer pass, the extra record and an animation's frame count.
DATA(0x0049f7ac) mapCell* gDrawCell;
DATA(0x0049f5f4) i32 gDrawX;
DATA(0x0049f79c) i32 gDrawY;
DATA(0x0049f7a0) u32 gDrawTile;
DATA(0x0049f5d8) i32 gDrawLayer;
DATA(0x0049f5e8) mapCellExtra* gDrawExtra;
DATA(0x0049f944) i32 gDrawFrames;

VA(0x00401a40, 0x139)
editManager::editManager(void) {
    m_viewX = 0;
    m_viewY = 0;
    m_cursorX = 0;
    m_cursorY = 0;
    m_window = NULL;
    gNextObjectLink = 1;
    m_zoomLevel = 0;
    gMap.Init(MAP_HEIGHT, MAP_WIDTH);
    gUndoMap.Init(MAP_HEIGHT, MAP_WIDTH);
    ResetArea(0, 0, MAP_WIDTH, MAP_HEIGHT);
    SaveUndo();
    m_animationFrame = 0;
    m_animationCounter = 0;
    m_tool = -1;
    m_toolManager = NULL;
    m_mapChanged = 0;
    m_placedX = m_placedY = EDIT_NO_CELL;
    m_extraCount = 1;
    m_placedState = -1;
}

VA(0x00401b79, 0x43e)
i32 editManager::Open(i32 priority) {
    i32 i;

    FillInOverlayTiles();
    InitializeMap(false, gNewMapSize, gNewMapSize);
    gpResourceManager->GetBackdrop("bordedit.icn", gpWindowManager->m_screen, 1);
    m_window = new heroWindow(0, 0, "editwind.bin");
    m_horizontalTrack = new iconWidget(
        EDIT_TRACK_START,
        EDIT_TRACK_EDGE,
        EDIT_TRACK_LONG,
        EDIT_TRACK_SHORT,
        "escroll.icn",
        EDIT_SCROLL_HORIZONTAL_TRACK,
        ICON_DRAW_NORMAL,
        EDIT_CONTROL_HORIZONTAL_TRACK,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_verticalTrack = new iconWidget(
        EDIT_TRACK_EDGE,
        EDIT_TRACK_START,
        EDIT_TRACK_SHORT,
        EDIT_TRACK_LONG,
        "escroll.icn",
        EDIT_SCROLL_VERTICAL_TRACK,
        ICON_DRAW_NORMAL,
        EDIT_CONTROL_VERTICAL_TRACK,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_horizontalKnob = new iconWidget(
        EDIT_KNOB_START,
        EDIT_KNOB_EDGE,
        EDIT_KNOB_LONG,
        EDIT_KNOB_SHORT,
        "escroll.icn",
        EDIT_SCROLL_HORIZONTAL_KNOB,
        ICON_DRAW_NORMAL,
        EDIT_CONTROL_HORIZONTAL_KNOB,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_verticalKnob = new iconWidget(
        EDIT_KNOB_EDGE,
        EDIT_KNOB_START,
        EDIT_KNOB_SHORT,
        EDIT_KNOB_LONG,
        "escroll.icn",
        EDIT_SCROLL_VERTICAL_KNOB,
        ICON_DRAW_NORMAL,
        EDIT_CONTROL_VERTICAL_KNOB,
        WIDGET_KIND_ICON_DIRECT,
        1
    );
    m_window->AddWidget(m_horizontalTrack, -1);
    m_window->AddWidget(m_verticalTrack, -1);
    m_window->AddWidget(m_horizontalKnob, -1);
    m_window->AddWidget(m_verticalKnob, -1);
    gpWindowManager->AddWindow(m_window, -1, 1);
    m_groundTiles[0] = gpResourceManager->GetTileset("ground32.til");
    m_cloudTiles[0] = gpResourceManager->GetTileset("clof32.til");
    for (i = 0; i < EDIT_MANAGER_TILESET_COUNT; i++)
        m_objectIcons[i][0] = m_objectIcons[i][1] = NULL;
    for (i = 0; i < EDIT_MANAGER_TILESET_COUNT; i++) {
        if (strlen(gTilesetFiles[i]) > 1)
            m_objectIcons[i][0] = gpResourceManager->GetIcon(gTilesetFiles[i]);
    }
    m_buttons = gpResourceManager->GetIcon("editbtns.icn");
    m_radarIcons = gpResourceManager->GetIcon("radar.icn");
    m_window->DrawWindow(0);
    DrawRadar(true);
    DrawView(m_viewX, m_viewY);
    UpdateMapView();
    gpMouseManager->SetPointer("editor.mse", EDIT_POINTER_DEFAULT, MOUSE_AUTO_CURSOR_TYPE);
    gpMouseManager->ShowColorPointer();
    m_messageMask = EDIT_MANAGER_DISPATCH_MASK;
    m_priority = priority;
    m_active = 1;
    strcpy(m_name, "editManager");
    SelectTool(0);
    return 0;
}

VA(0x00401fb7, 0xf6)
void editManager::Close(void) {
    i32 i;

    InitializeMap(false, EDIT_DEFAULT_MAP_SIZE, EDIT_DEFAULT_MAP_SIZE);
    ClearErrors();
    SelectTool(-1);
    gpWindowManager->RemoveWindow(m_window);
    delete m_window;
    m_window = NULL;
    gpResourceManager->Dispose(m_groundTiles[0]);
    gpResourceManager->Dispose(m_cloudTiles[0]);
    for (i = 0; i < EDIT_MANAGER_TILESET_COUNT; i++)
        gpResourceManager->Dispose(m_objectIcons[i][0]);
    gpResourceManager->Dispose(m_radarIcons);
    gpResourceManager->Dispose(m_buttons);
    gpMouseManager->SetPointer(-1);
    m_active = 0;
}

VA(0x00403629, 0x1a)
void editManager::SaveUndo(void) {
    gUndoMap.Copy(gMap);
}

VA(0x00403643, 0x24)
void editManager::UpdateMapView(void) {
    gpWindowManager->UpdateScreenRegion(EDIT_VIEW_LEFT, EDIT_VIEW_TOP, EDIT_VIEW_PIXELS, EDIT_VIEW_PIXELS);
}

VA(0x00403667, 0x3b)
void editManager::UpdateCursor(void) {
    DrawRulers(m_viewX, m_viewY, m_cursorX, m_cursorY);
}

VA(0x004036a2, 0x2b8)
void editManager::DrawRulers(i32 viewX, i32 viewY, i32 cursorX, i32 cursorY) {
    i32 mouseX;
    i32 mouseY;
    u8 color;
    char text[8];
    i32 i;
    i32 cell;

    gpMouseManager->MouseCoords(mouseX, mouseY);
    cursorX -= m_viewX;
    cursorY -= m_viewY;
    if (m_zoomLevel == EDIT_ZOOM_FAR) {
        cursorX &= 0xfffe;
        cursorY &= 0xfffe;
    }
    if (mouseX < EDIT_VIEW_LEFT || mouseX >= EDIT_VIEW_LEFT + EDIT_VIEW_PIXELS
        || mouseY < EDIT_VIEW_TOP || mouseY > EDIT_VIEW_TOP + EDIT_VIEW_PIXELS) {
        cursorX = EDIT_NO_CELL;
        cursorY = EDIT_NO_CELL;
    }
    for (i = 0; i < EDIT_RULER_SLOTS; i++) {
        if (m_zoomLevel == EDIT_ZOOM_NORMAL && (i & 1))
            continue;
        if (m_zoomLevel == EDIT_ZOOM_NORMAL)
            m_buttons->DrawToBuffer(
                i * EDIT_RULER_SLOT_PIXELS + EDIT_VIEW_LEFT, 0, EDIT_FRAME_TOP_RULER_CELL, 0
            );
        else
            m_buttons->DrawToBuffer(
                i * EDIT_RULER_SLOT_PIXELS + EDIT_VIEW_LEFT, 0, EDIT_FRAME_ZOOMED_RULER_CELL, 0
            );
        if (m_zoomLevel == EDIT_ZOOM_NORMAL)
            cell = i / 2;
        else if (m_zoomLevel == EDIT_ZOOM_HALF)
            cell = i;
        else
            cell = i * 2;
        sprintf(text, "%02d", viewX + cell);
        if (cell == cursorX)
            sprintf(text, "%02d", viewX + cell);
        else
            sprintf(text, "{%02d}", viewX + cell);
        color = 1;
        smallFont->DrawString(
            text,
            i * EDIT_RULER_SLOT_PIXELS + (m_zoomLevel ? 0 : EDIT_RULER_CELL_TEXT_OFFSET)
                + EDIT_TOP_RULER_TEXT_X,
            EDIT_TOP_RULER_TEXT_Y,
            static_cast<FontDrawMode>(color)
        );
        if (m_zoomLevel == EDIT_ZOOM_NORMAL)
            m_buttons->DrawToBuffer(
                0, i * EDIT_RULER_SLOT_PIXELS + EDIT_VIEW_TOP, EDIT_FRAME_LEFT_RULER_CELL, 0
            );
        else
            m_buttons->DrawToBuffer(
                0, i * EDIT_RULER_SLOT_PIXELS + EDIT_VIEW_TOP, EDIT_FRAME_ZOOMED_RULER_CELL, 0
            );
        if (cell == cursorY)
            sprintf(text, "%02d", viewY + cell);
        else
            sprintf(text, "{%02d}", viewY + cell);
        color = 1;
        smallFont->DrawString(
            text,
            EDIT_LEFT_RULER_TEXT_X,
            i * EDIT_RULER_SLOT_PIXELS + (m_zoomLevel ? 0 : EDIT_RULER_CELL_TEXT_OFFSET)
                + EDIT_LEFT_RULER_TEXT_Y,
            static_cast<FontDrawMode>(color)
        );
    }
    gpWindowManager->UpdateScreenRegion(EDIT_VIEW_LEFT, 0, EDIT_VIEW_PIXELS, EDIT_VIEW_TOP);
    gpWindowManager->UpdateScreenRegion(0, EDIT_VIEW_TOP, EDIT_VIEW_LEFT, EDIT_VIEW_PIXELS);
}

VA(0x0040395a, 0xe3)
void editManager::ScreenToCell(i32& x, i32& y) {
    x -= EDIT_VIEW_LEFT;
    y -= EDIT_VIEW_TOP;
    x /= gZoomTileSize[m_zoomLevel];
    if (x < 0)
        x = 0;
    if (x > gZoomViewCells[m_zoomLevel] - 1)
        x = gZoomViewCells[m_zoomLevel] - 1;
    y /= gZoomTileSize[m_zoomLevel];
    if (y < 0)
        y = 0;
    if (y > gZoomViewCells[m_zoomLevel] - 1)
        y = gZoomViewCells[m_zoomLevel] - 1;
}

#if H2_RETAIL_COMPILER
#define radarLeft viewX4
#define unusedA spare3
#define unusedB tmp2
#endif
VA(0x00403def, 0x79b)
void editManager::DrawRadar(b32) {
    u8* line;
    i32 rowPhase;
    u8 color;
    i32 xPhase;
    i32 radarLeft;
    i32 x;
    i32 viewY;
    u8* dst;
    i32 frame;
    i32 H2_UNUSED(unusedA);
    i32 H2_UNUSED(unusedB);
    i32 mapY;
    mapCell* cell;
    i32 tile;

    line = gpWindowManager->m_screen->m_pixels + EDIT_RADAR_OFFSET;
    dst = line;
    xPhase = rowPhase = 0;
    for (mapY = 0; mapY < MAP_HEIGHT; mapY++) {
        dst = line;
        switch (MAP_HEIGHT) {
            case EDIT_MAP_SMALL:
                line += 4 * EDIT_SCREEN_PITCH;
                break;
            case EDIT_MAP_MEDIUM:
                line += 2 * EDIT_SCREEN_PITCH;
                break;
            case EDIT_MAP_LARGE:
                rowPhase++;
                if (rowPhase > EDIT_RADAR_PHASES - 1)
                    rowPhase = 0;
                if (rowPhase)
                    line += EDIT_SCREEN_PITCH;
                else
                    line += 2 * EDIT_SCREEN_PITCH;
                break;
            case EDIT_MAP_XLARGE:
                line += EDIT_SCREEN_PITCH;
                break;
        }
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = &gMap.Column(x)[mapY * gMap.width];
            tile = -1;
            if (cell->m_objectIndex != MAPCELL_SPRITE_NONE)
                tile = cell->m_objectTileset;
            else if (cell->m_overlayIndex != MAPCELL_SPRITE_NONE)
                tile = cell->m_overlayTileset;
            switch (tile) {
                case IDX(TILESET_MTNSNOW):
                case IDX(TILESET_MTNSWMP):
                case IDX(TILESET_MTNLAVA):
                case IDX(TILESET_MTNDSRT):
                case IDX(TILESET_MTNDIRT):
                case IDX(TILESET_MTNMULT):
                case IDX(TILESET_MTNCRCK):
                case IDX(TILESET_MTNGRAS):
                case IDX(TILESET_TREJNGL):
                case IDX(TILESET_TREEVIL):
                case IDX(TILESET_TRESNOW):
                case IDX(TILESET_TREFIR):
                case IDX(TILESET_TREFALL):
                case IDX(TILESET_TREDECI):
                    switch (cell->m_triggerType) {
                        case IDX(MAP_OBJECT_ALCHEMIST_LAB):
                        case IDX(MAP_OBJECT_MINE):
                        case IDX(MAP_OBJECT_SAWMILL):
                        case MAP_TRIGGER_ACTION_FLAG | IDX(MAP_OBJECT_ALCHEMIST_LAB):
                        case MAP_TRIGGER_ACTION_FLAG | IDX(MAP_OBJECT_MINE):
                        case MAP_TRIGGER_ACTION_FLAG | IDX(MAP_OBJECT_SAWMILL):
                            color = EDIT_RADAR_TOWN_COLOR;
                            break;
                        default:
                            color = gMapColors[giGroundToTerrain[gMap.Column(x)[mapY * gMap.width].m_terrainImageIndex]]
                                    + EDIT_RADAR_OBSTACLE_SHADE;
                            break;
                    }
                    break;
                case IDX(TILESET_OBJNTOWN):
                case IDX(TILESET_OBJNTWBA):
                    color = EDIT_RADAR_TOWN_COLOR;
                    break;
                default:
                    color = gMapColors[giGroundToTerrain[gMap.Column(x)[mapY * gMap.width].m_terrainImageIndex]];
                    break;
            }
            if (gGeneratingMap)
                color = EDIT_RADAR_UNSEEN_COLOR;
            switch (MAP_HEIGHT) {
                case EDIT_MAP_SMALL:
                    memset(dst, color, 4);
                    memset(dst + EDIT_SCREEN_PITCH, color, 4);
                    memset(dst + 2 * EDIT_SCREEN_PITCH, color, 4);
                    memset(dst + 3 * EDIT_SCREEN_PITCH, color, 4);
                    dst += 4;
                    break;
                case EDIT_MAP_MEDIUM:
                    memset(dst, color, 2);
                    memset(dst + EDIT_SCREEN_PITCH, color, 2);
                    dst += 2;
                    break;
                case EDIT_MAP_LARGE:
                    if (xPhase) {
                        if (rowPhase) {
                            *dst = color;
                            dst++;
                        } else {
                            *dst = color;
                            dst[EDIT_SCREEN_PITCH] = color;
                            dst++;
                        }
                    } else if (rowPhase) {
                        dst[0] = color;
                        dst[1] = color;
                        dst += 2;
                    } else {
                        dst[0] = color;
                        dst[1] = color;
                        dst[EDIT_SCREEN_PITCH] = color;
                        dst[EDIT_SCREEN_PITCH + 1] = color;
                        dst += 2;
                    }
                    xPhase++;
                    if (xPhase > EDIT_RADAR_PHASES - 1)
                        xPhase = 0;
                    break;
                case EDIT_MAP_XLARGE:
                    *dst = color;
                    dst++;
                    break;
            }
        }
    }
    frame = -1;
    switch (MAP_HEIGHT) {
        case EDIT_MAP_SMALL:
            switch (m_zoomLevel) {
                case 1:
                    frame = 7;
                    break;
                case 0:
                    frame = 5;
                    break;
            }
            radarLeft = m_viewX * 4;
            viewY = m_viewY * 4;
            break;
        case EDIT_MAP_MEDIUM:
            switch (m_zoomLevel) {
                case 2:
                    frame = 7;
                    break;
                case 1:
                    frame = 5;
                    break;
                case 0:
                    frame = 3;
                    break;
            }
            radarLeft = m_viewX * 2;
            viewY = m_viewY * 2;
            break;
        case EDIT_MAP_LARGE:
            switch (m_zoomLevel) {
                case 2:
                    frame = 6;
                    break;
                case 1:
                    frame = 4;
                    break;
                case 0:
                    frame = 2;
                    break;
            }
            radarLeft = m_viewX * 1.3333;
            viewY = m_viewY * 1.3333;
            break;
        default:
            switch (m_zoomLevel) {
                case 2:
                    frame = 5;
                    break;
                case 1:
                    frame = 3;
                    break;
                case 0:
                    frame = 1;
                    break;
            }
            radarLeft = m_viewX;
            viewY = m_viewY;
            break;
    }
    m_radarIcons->FillToBuffer(
        radarLeft + EDIT_RADAR_LEFT, viewY + EDIT_RADAR_TOP, frame, EDIT_RADAR_VIEW_COLOR, 0, NULL
    );
    UpdateKnobs(1);
    UpdateCursor();
}
#if H2_RETAIL_COMPILER
#undef radarLeft
#undef unusedA
#undef unusedB
#endif

VA(0x0040458a, 0xb70)
void editManager::DrawCell(i32 x, i32 y, i32 column, i32 row, i32 layers) {
    gDrawCell = &gMap.Column(x)[y * gMap.width];
    gDrawX = column * gZoomTileSize[m_zoomLevel] + EDIT_VIEW_LEFT;
    gDrawY = row * gZoomTileSize[m_zoomLevel] + EDIT_VIEW_TOP;
    if (gGeneratingMap) {
        if (layers & EDIT_DRAW_OVERHANGING)
            TileToBitmapScale(
                m_cloudTiles[0],
                (x + y) & EDIT_CLOUD_TILE_MASK,
                gpWindowManager->m_screen,
                gDrawX,
                gDrawY,
                gZoomScale[m_zoomLevel]
            );
        return;
    }
    if (layers & EDIT_DRAW_GROUND) {
        gDrawTile = gDrawCell->m_flags;
        gDrawTile <<= EDIT_TILE_FLAG_SHIFT;
        gDrawTile |= gDrawCell->m_terrainImageIndex;
        TileToBitmapScale(
            m_groundTiles[0], gDrawTile, gpWindowManager->m_screen, gDrawX, gDrawY,
            gZoomScale[m_zoomLevel]
        );
    }
    if (layers & EDIT_DRAW_OBJECTS) {
        for (gDrawLayer = EDIT_LAYER_HIGH; gDrawLayer >= EDIT_LAYER_LOW; gDrawLayer--) {
            if (gDrawCell->m_objectIndex != MAPCELL_SPRITE_NONE
                && ((gDrawLayer == EDIT_LAYER_HIGH && gDrawCell->m_objectLayerBit0)
                    || (gDrawLayer == EDIT_LAYER_MID && gDrawCell->m_objectLayerBit1)
                    || (gDrawLayer == EDIT_LAYER_LOW && !gDrawCell->m_objectLayerBit0
                        && !gDrawCell->m_objectLayerBit1))) {
                if (gDrawCell->m_objectTileset == TILESET_MONS32)
                    IconToBitmapScale(
                        m_objectIcons[gDrawCell->m_objectTileset][0],
                        gpWindowManager->m_screen,
                        gDrawX,
                        gDrawY - EDIT_MONSTER_LIFT / gZoomScale[m_zoomLevel],
                        gDrawCell->m_objectIndex,
                        1,
                        0,
                        0,
                        EDIT_CLIP_VIEW,
                        EDIT_CLIP_VIEW,
                        gZoomCellSize[m_zoomLevel]
                    );
                else if (gDrawCell->m_objectTileset == TILESET_MINIHERO)
                    IconToBitmapScaleShadow(
                        m_objectIcons[gDrawCell->m_objectTileset][0],
                        gpWindowManager->m_screen,
                        gDrawX,
                        gDrawY - EDIT_HERO_LIFT / gZoomScale[m_zoomLevel],
                        gDrawCell->m_objectIndex,
                        1,
                        0,
                        0,
                        EDIT_CLIP_VIEW,
                        EDIT_CLIP_VIEW,
                        gZoomCellSize[m_zoomLevel]
                    );
                else
                    IconToBitmapScale(
                        m_objectIcons[gDrawCell->m_objectTileset][0],
                        gpWindowManager->m_screen,
                        gDrawX,
                        gDrawY,
                        gDrawCell->m_objectIndex,
                        0,
                        0,
                        0,
                        EDIT_CLIP_SCREEN_W,
                        EDIT_CLIP_SCREEN_H,
                        gZoomCellSize[m_zoomLevel]
                    );
                if (gDrawCell->m_animatedObject) {
                    gDrawFrames = GetIconEntry(
                                      m_objectIcons[gDrawCell->m_objectTileset][0],
                                      gDrawCell->m_objectIndex
                                  )->flags;
                    if (gDrawCell->m_triggerType == EDIT_TRIGGER_ONE_FRAME_LESS)
                        gDrawFrames--;
                    IconToBitmapScale(
                        m_objectIcons[gDrawCell->m_objectTileset][0],
                        gpWindowManager->m_screen,
                        gDrawX,
                        gDrawY,
                        gDrawCell->m_objectIndex + 1 + m_animationCounter % gDrawFrames,
                        0,
                        0,
                        0,
                        EDIT_CLIP_SCREEN_W,
                        EDIT_CLIP_SCREEN_H,
                        gZoomCellSize[m_zoomLevel]
                    );
                }
            }
            if (gDrawCell->m_extraIndex
                && gMap.extras[gDrawCell->m_extraIndex].objectIndex != MAPCELL_SPRITE_NONE)
                gDrawExtra = &gMap.extras[gDrawCell->m_extraIndex];
            else
                gDrawExtra = NULL;
            while (gDrawExtra) {
                if ((gDrawLayer == EDIT_LAYER_HIGH && gDrawExtra->objectLayerBit0)
                    || (gDrawLayer == EDIT_LAYER_MID && gDrawExtra->objectLayerBit1)
                    || (gDrawLayer == EDIT_LAYER_LOW && !gDrawExtra->objectLayerBit0
                        && !gDrawExtra->objectLayerBit1)) {
                    if (gDrawExtra->objectTileset == TILESET_MONS32)
                        IconToBitmapScale(
                            m_objectIcons[gDrawExtra->objectTileset][0],
                            gpWindowManager->m_screen,
                            gDrawX,
                            gDrawY - EDIT_MONSTER_LIFT / gZoomScale[m_zoomLevel],
                            gDrawExtra->objectIndex,
                            1,
                            0,
                            0,
                            EDIT_CLIP_VIEW,
                            EDIT_CLIP_VIEW,
                            gZoomCellSize[m_zoomLevel]
                        );
                    else if (gDrawExtra->objectTileset == TILESET_MINIHERO)
                        IconToBitmapScaleShadow(
                            m_objectIcons[gDrawExtra->objectTileset][0],
                            gpWindowManager->m_screen,
                            gDrawX,
                            gDrawY - EDIT_HERO_LIFT / gZoomScale[m_zoomLevel],
                            gDrawExtra->objectIndex,
                            1,
                            0,
                            0,
                            EDIT_CLIP_VIEW,
                            EDIT_CLIP_VIEW,
                            gZoomCellSize[m_zoomLevel]
                        );
                    else
                        IconToBitmapScale(
                            m_objectIcons[gDrawExtra->objectTileset][0],
                            gpWindowManager->m_screen,
                            gDrawX,
                            gDrawY,
                            gDrawExtra->objectIndex,
                            0,
                            0,
                            0,
                            EDIT_CLIP_SCREEN_W,
                            EDIT_CLIP_SCREEN_H,
                            gZoomCellSize[m_zoomLevel]
                        );
                    if (gDrawExtra->animatedObject) {
                        gDrawFrames = GetIconEntry(
                                          m_objectIcons[gDrawExtra->objectTileset][0],
                                          gDrawExtra->objectIndex
                                      )->flags;
                        IconToBitmapScale(
                            m_objectIcons[gDrawExtra->objectTileset][0],
                            gpWindowManager->m_screen,
                            gDrawX,
                            gDrawY,
                            gDrawExtra->objectIndex + 1 + m_animationCounter % gDrawFrames,
                            0,
                            0,
                            0,
                            EDIT_CLIP_SCREEN_W,
                            EDIT_CLIP_SCREEN_H,
                            gZoomCellSize[m_zoomLevel]
                        );
                    }
                }
                if (gDrawExtra->nextIndex
                    && gMap.extras[gDrawExtra->nextIndex].objectIndex != MAPCELL_SPRITE_NONE)
                    gDrawExtra = &gMap.extras[gDrawExtra->nextIndex];
                else
                    gDrawExtra = NULL;
            }
        }
    }
    if (layers & EDIT_DRAW_OVERHANGING) {
        if (gDrawCell->m_overlayIndex != MAPCELL_SPRITE_NONE) {
            IconToBitmapScale(
                m_objectIcons[gDrawCell->m_overlayTileset][0],
                gpWindowManager->m_screen,
                gDrawX,
                gDrawY,
                gDrawCell->m_overlayIndex,
                0,
                0,
                0,
                EDIT_CLIP_SCREEN_W,
                EDIT_CLIP_SCREEN_H,
                gZoomCellSize[m_zoomLevel]
            );
            if (gDrawCell->m_animatedOverlay) {
                gDrawFrames = GetIconEntry(
                                  m_objectIcons[gDrawCell->m_overlayTileset][0],
                                  gDrawCell->m_overlayIndex
                              )->flags;
                IconToBitmapScale(
                    m_objectIcons[gDrawCell->m_overlayTileset][0],
                    gpWindowManager->m_screen,
                    gDrawX,
                    gDrawY,
                    gDrawCell->m_overlayIndex + 1 + m_animationCounter % gDrawFrames,
                    0,
                    0,
                    0,
                    EDIT_CLIP_SCREEN_W,
                    EDIT_CLIP_SCREEN_H,
                    gZoomCellSize[m_zoomLevel]
                );
            }
        }
        if (gDrawCell->m_extraIndex
            && gMap.extras[gDrawCell->m_extraIndex].overlayIndex != MAPCELL_SPRITE_NONE)
            gDrawExtra = &gMap.extras[gDrawCell->m_extraIndex];
        else
            gDrawExtra = NULL;
        while (gDrawExtra) {
            IconToBitmapScale(
                m_objectIcons[gDrawExtra->overlayTileset][0],
                gpWindowManager->m_screen,
                gDrawX,
                gDrawY,
                gDrawExtra->overlayIndex,
                0,
                0,
                0,
                EDIT_CLIP_SCREEN_W,
                EDIT_CLIP_SCREEN_H,
                gZoomCellSize[m_zoomLevel]
            );
            if (gDrawExtra->animatedOverlay) {
                gDrawFrames = GetIconEntry(
                                  m_objectIcons[gDrawExtra->overlayTileset][0],
                                  gDrawExtra->overlayIndex
                              )->flags;
                IconToBitmapScale(
                    m_objectIcons[gDrawExtra->overlayTileset][0],
                    gpWindowManager->m_screen,
                    gDrawX,
                    gDrawY,
                    gDrawExtra->overlayIndex + 1 + m_animationCounter % gDrawFrames,
                    0,
                    0,
                    0,
                    EDIT_CLIP_SCREEN_W,
                    EDIT_CLIP_SCREEN_H,
                    gZoomCellSize[m_zoomLevel]
                );
            }
            if (gDrawExtra->nextIndex
                && gMap.extras[gDrawExtra->nextIndex].overlayIndex != MAPCELL_SPRITE_NONE)
                gDrawExtra = &gMap.extras[gDrawExtra->nextIndex];
            else
                gDrawExtra = NULL;
        }
    }
}

VA(0x00403a3d, 0x27)
void editManager::DrawMap(void) {
    DrawView(m_viewX, m_viewY);
}

VA(0x00403a64, 0x38b)
void editManager::DrawView(i32 viewX, i32 viewY) {
    i32 col;
    i32 outline;
    i32 tileSize;
    i32 row;
    i32 cells;
    i32 oldZoom;

    oldZoom = m_zoomLevel;
    if (gGeneratingMap)
        m_zoomLevel = EDIT_ZOOM_NORMAL;
    cells = gZoomViewCells[m_zoomLevel];
    for (row = 0; row < cells; row++)
        for (col = 0; col < cells; col++)
            DrawCell(viewX + col, viewY + row, col, row, EDIT_DRAW_GROUND);
    for (row = 0; row < cells; row++) {
        for (col = 0; col < cells; col++)
            DrawCell(viewX + col, viewY + row, col, row, EDIT_DRAW_OBJECTS);
        if (row > 0) {
            for (col = 0; col < cells; col++)
                DrawCell(viewX + col, viewY + row - 1, col, row - 1, EDIT_DRAW_OVERHANGING);
        }
    }
    for (col = 0; col < cells; col++)
        DrawCell(viewX + col, viewY + cells - 1, col, cells - 1, EDIT_DRAW_OVERHANGING);
    gLastAnimationTick = KBTickCount();
    if (!gConfig.editorScreenAnimation) {
        m_animationCounter++;
        m_animationFrame++;
        m_animationFrame %= EDIT_ANIMATION_FRAMES;
    }
    tileSize = gZoomTileSize[m_zoomLevel];
    outline = m_zoomLevel ? 1 : 2;
    if (gSelectionX >= 0) {
        gSelectionColor = EDIT_SELECTION_COLOR;
        FillBitmapAreaClip(
            gpWindowManager->m_screen,
            (gSelectionX - viewX) * tileSize + EDIT_VIEW_LEFT,
            (gSelectionY - viewY) * tileSize + EDIT_VIEW_TOP,
            outline,
            gSelectionHeight * tileSize - 1,
            gSelectionColor,
            EDIT_VIEW_LEFT,
            EDIT_VIEW_TOP,
            EDIT_VIEW_PIXELS,
            EDIT_VIEW_PIXELS
        );
        FillBitmapAreaClip(
            gpWindowManager->m_screen,
            (gSelectionX - viewX) * tileSize + EDIT_VIEW_LEFT,
            (gSelectionY - viewY) * tileSize + EDIT_VIEW_TOP,
            gSelectionWidth * tileSize - 1,
            outline,
            gSelectionColor,
            EDIT_VIEW_LEFT,
            EDIT_VIEW_TOP,
            EDIT_VIEW_PIXELS,
            EDIT_VIEW_PIXELS
        );
        FillBitmapAreaClip(
            gpWindowManager->m_screen,
            (gSelectionX - viewX + gSelectionWidth) * tileSize + EDIT_VIEW_LEFT - outline,
            (gSelectionY - viewY) * tileSize + EDIT_VIEW_TOP,
            outline,
            gSelectionHeight * tileSize - 1,
            gSelectionColor,
            EDIT_VIEW_LEFT,
            EDIT_VIEW_TOP,
            EDIT_VIEW_PIXELS,
            EDIT_VIEW_PIXELS
        );
        FillBitmapAreaClip(
            gpWindowManager->m_screen,
            (gSelectionX - viewX) * tileSize + EDIT_VIEW_LEFT,
            (gSelectionY - viewY + gSelectionHeight) * tileSize + EDIT_VIEW_TOP - outline,
            gSelectionWidth * tileSize - 1,
            outline,
            gSelectionColor,
            EDIT_VIEW_LEFT,
            EDIT_VIEW_TOP,
            EDIT_VIEW_PIXELS,
            EDIT_VIEW_PIXELS
        );
    }
    m_zoomLevel = oldZoom;
}
VA(0x004050fa, 0xc4)
void editManager::ToggleZoom(void) {
    if (m_zoomLevel == EDIT_ZOOM_NORMAL) {
        if (MAP_HEIGHT == EDIT_MAP_SMALL) {
            m_zoomLevel = EDIT_ZOOM_HALF;
            Scroll(-EDIT_ZOOM_SHIFT_SMALL, -EDIT_ZOOM_SHIFT_SMALL);
        } else {
            m_zoomLevel = EDIT_ZOOM_FAR;
            Scroll(-EDIT_ZOOM_SHIFT_LARGE, -EDIT_ZOOM_SHIFT_LARGE);
        }
    } else if (m_zoomLevel == EDIT_ZOOM_FAR) {
        m_zoomLevel = EDIT_ZOOM_HALF;
        Scroll(EDIT_ZOOM_SHIFT_BACK, EDIT_ZOOM_SHIFT_BACK);
    } else {
        m_zoomLevel = EDIT_ZOOM_NORMAL;
        Scroll(EDIT_ZOOM_SHIFT_SMALL, EDIT_ZOOM_SHIFT_SMALL);
    }
    DrawView(m_viewX, m_viewY);
    DrawRadar(true);
    UpdateMapView();
}

VA(0x00405540, 0x116)
void editManager::Scroll(i32 dx, i32 dy) {
    m_viewX += dx;
    if (m_viewX < 0)
        m_viewX = 0;
    if (m_viewX > MAP_WIDTH - gZoomViewCells[m_zoomLevel])
        m_viewX = MAP_WIDTH - gZoomViewCells[m_zoomLevel];
    m_viewY += dy;
    if (m_viewY < 0)
        m_viewY = 0;
    if (m_viewY > MAP_HEIGHT - gZoomViewCells[m_zoomLevel])
        m_viewY = MAP_HEIGHT - gZoomViewCells[m_zoomLevel];
    DrawView(m_viewX, m_viewY);
    UpdateMapView();
    DrawRadar(true);
}

VA(0x00405656, 0x189)
void editManager::UpdateKnobs(i32 update) {
    double scaleX;
    double scaleY;
    i32 H2_UNUSED(xPos);
    i32 H2_UNUSED(yPos);

    scaleX = 402.0 / (MAP_WIDTH - gZoomViewCells[m_zoomLevel] + 1);
    scaleY = 402.0 / (MAP_HEIGHT - gZoomViewCells[m_zoomLevel] + 1);
    xPos = m_viewX * scaleX;
    yPos = m_viewY * scaleY;
    if (MAP_WIDTH > gZoomViewCells[m_zoomLevel])
        m_horizontalKnob->m_x
            = m_viewX * (393.0 / ((MAP_WIDTH + 1 - gZoomViewCells[m_zoomLevel]) - 1.0)) + 35.0;
    else
        m_horizontalKnob->m_x = EDIT_KNOB_PARKED;
    if (MAP_HEIGHT > gZoomViewCells[m_zoomLevel])
        m_verticalKnob->m_y
            = m_viewY * (393.0 / ((MAP_HEIGHT + 1 - gZoomViewCells[m_zoomLevel]) - 1.0)) + 35.0;
    else
        m_verticalKnob->m_y = EDIT_KNOB_PARKED;
    m_window->DrawWindow(update, EDIT_CONTROL_HORIZONTAL_TRACK, EDIT_CONTROL_SCROLL_LAST);
}

VA(0x004057df, 0xd6)
void editManager::PaintGround(i32 column, i32 row, i32 width, i32 height, i32 terrain) {
    i32 startY;
    i32 startX;
    i32 j;
    i32 H2_UNUSED(cellPixels);
    i32 H2_UNUSED(redrawTop);
    i32 i;
    i32 H2_UNUSED(redrawLeft);
    i32 H2_UNUSED(redrawWidth);

    startX = m_viewX + column;
    startY = m_viewY + row;
    gEditManager->ClearArea(startX, startY, width, height, EDIT_CLEAR_ALL, 0, 0);
    for (i = 0; i < width; i++)
        for (j = 0; j < height; j++)
            gMap.CellAt(startX + i, startY + j)->m_terrainImageIndex
                = ChooseGroundTile(terrain, 0, 1, startX + i, startY + j, 0, 1.0f);
}

VA(0x004058b5, 0xe9)
void editManager::FillGround(i32 x, i32 y, i32 width, i32 height, i32 terrain) {
    i32 i;
    i32 j;

    gEditManager->ClearArea(x, y, width, height, EDIT_CLEAR_ALL, 0, 0);
    for (i = x; i < x + width; i++) {
        for (j = y; j < y + height; j++) {
            gMap.CellAt(i, j)->m_terrainImageIndex
                = ChooseGroundTile(terrain, 0, 1, i, j, 0, 1.0f);
            gMap.Column(i)[j * gMap.width].m_flags &= EDIT_CELL_GROUND_KEEP;
        }
    }
}

// Gives the cell a new tile of the terrain and shape unless it already has one.
VA(0x0040599e, 0x88)
void SetCellGround(i32 x, i32 y, i32 terrain, i32 shape) {
    mapCell* cell;

    cell = &gMap.Column(x)[y * gMap.width];
    if (giGroundToTerrain[cell->m_terrainImageIndex] == terrain
        && (giGroundShape[cell->m_terrainImageIndex] & 0x7f) == (shape & 0x7f))
        return;
    cell->m_terrainImageIndex = ChooseGroundTile(terrain, shape, 1, x, y, 0, 1.0f);
}

// Centres the view on the radar cell under the pointer while the button is
// held.
VA(0x00405a26, 0x44c)
void editManager::DoRadar(void) {
    tag_message input;
    i32 x;
    tag_message mouseMove;
    i32 y;
    float scale;

    gpMouseManager->MouseCoords(x, y);
    if (x < EDIT_RADAR_LEFT || x > EDIT_RADAR_LEFT + EDIT_RADAR_SIZE || y < EDIT_RADAR_TOP
        || y > EDIT_RADAR_TOP + EDIT_RADAR_SIZE)
        return;
    switch (MAP_HEIGHT) {
        case EDIT_MAP_SMALL:
            scale = 4.0f;
            break;
        case EDIT_MAP_MEDIUM:
            scale = 2.0f;
            break;
        case EDIT_MAP_LARGE:
            scale = 1.3333f;
            break;
        default:
            scale = 1.0f;
            break;
    }
    x = (x - EDIT_RADAR_LEFT) / scale;
    y = (y - EDIT_RADAR_TOP) / scale;
    m_viewX = x - gZoomViewCells[m_zoomLevel] / 2;
    if (m_viewX + gZoomViewCells[m_zoomLevel] > MAP_WIDTH)
        m_viewX = MAP_WIDTH - gZoomViewCells[m_zoomLevel];
    m_viewY = y - gZoomViewCells[m_zoomLevel] / 2;
    if (m_viewY + gZoomViewCells[m_zoomLevel] > MAP_HEIGHT)
        m_viewY = MAP_HEIGHT - gZoomViewCells[m_zoomLevel];
    if (m_viewX < 0)
        m_viewX = 0;
    if (m_viewY < 0)
        m_viewY = 0;
    DrawMap();
    DrawRadar(true);
    UpdateMapView();
    input = gpInputManager->GetEvent();
    while (input.type != MESSAGE_LEFT_BUTTON_UP && input.type != MESSAGE_RIGHT_BUTTON_UP) {
        Process1WindowsMessage();
        if (input.type == MESSAGE_MOUSE_MOVE) {
            while (input.type == MESSAGE_MOUSE_MOVE) {
                mouseMove = input;
                input = gpInputManager->GetEvent();
            }
            gpMouseManager->Main(mouseMove);
            x = (mouseMove.payload.mouse.x - EDIT_RADAR_LEFT) / scale;
            y = (mouseMove.payload.mouse.y - EDIT_RADAR_TOP) / scale;
            m_viewX = x - gZoomViewCells[m_zoomLevel] / 2;
            if (m_viewX + gZoomViewCells[m_zoomLevel] > MAP_WIDTH)
                m_viewX = MAP_WIDTH - gZoomViewCells[m_zoomLevel];
            m_viewY = y - gZoomViewCells[m_zoomLevel] / 2;
            if (m_viewY + gZoomViewCells[m_zoomLevel] > MAP_HEIGHT)
                m_viewY = MAP_HEIGHT - gZoomViewCells[m_zoomLevel];
            if (m_viewX < 0)
                m_viewX = 0;
            if (m_viewY < 0)
                m_viewY = 0;
            DrawMap();
            DrawRadar(true);
            UpdateMapView();
        } else {
            input = gpInputManager->GetEvent();
        }
    }
}

VA(0x00405e72, 0x227)
void editManager::DoHorizontalKnob(void) {
    double scale;
    tag_message latest;
    i32 x;
    tag_message message;
    i32 y;
    i32 newX;

    scale = 402.0 / (MAP_WIDTH - gZoomViewCells[m_zoomLevel] + 1);
    gpMouseManager->MouseCoords(x, y);
    gpInputManager->Flush();
    message.type = MESSAGE_MOUSE_MOVE;
    message.payload.mouse.x = x;
    message.payload.mouse.y = y;
    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        Process1WindowsMessage();
        if (message.type == MESSAGE_MOUSE_MOVE) {
            latest = message;
            while (message.type == MESSAGE_MOUSE_MOVE) {
                latest = message;
                message = gpInputManager->GetEvent();
            }
            gpMouseManager->Main(latest);
            m_horizontalKnob->m_x = latest.payload.mouse.x;
            newX = latest.payload.mouse.x;
            newX = (newX - EDIT_KNOB_DRAG_ORIGIN) / scale;
            newX = newX + 0.5;
            if (m_viewX != newX) {
                if (newX > MAP_WIDTH - gZoomViewCells[m_zoomLevel])
                    newX = MAP_WIDTH - gZoomViewCells[m_zoomLevel];
                if (newX < 0)
                    newX = 0;
                m_viewX = newX;
                DrawMap();
                UpdateMapView();
                DrawRadar(true);
            }
        } else {
            message = gpInputManager->GetEvent();
        }
    }
    m_horizontalKnob->m_flags &= ~WIDGET_FLAG_SELECTED;
    m_horizontalTrack->m_flags &= ~WIDGET_FLAG_SELECTED;
}

VA(0x00406099, 0x227)
void editManager::DoVerticalKnob(void) {
    double scale;
    tag_message latest;
    i32 x;
    tag_message message;
    i32 y;
    i32 newY;

    scale = 402.0 / (MAP_HEIGHT - gZoomViewCells[m_zoomLevel] + 1);
    gpMouseManager->MouseCoords(x, y);
    gpInputManager->Flush();
    message.type = MESSAGE_MOUSE_MOVE;
    message.payload.mouse.x = x;
    message.payload.mouse.y = y;
    while (message.type != MESSAGE_LEFT_BUTTON_UP && message.type != MESSAGE_RIGHT_BUTTON_UP) {
        Process1WindowsMessage();
        if (message.type == MESSAGE_MOUSE_MOVE) {
            latest = message;
            while (message.type == MESSAGE_MOUSE_MOVE) {
                latest = message;
                message = gpInputManager->GetEvent();
            }
            gpMouseManager->Main(latest);
            m_verticalKnob->m_y = latest.payload.mouse.y;
            newY = latest.payload.mouse.y;
            newY = (newY - EDIT_KNOB_DRAG_ORIGIN) / scale;
            newY = newY + 0.5;
            if (m_viewY != newY) {
                if (newY > MAP_HEIGHT - gZoomViewCells[m_zoomLevel])
                    newY = MAP_HEIGHT - gZoomViewCells[m_zoomLevel];
                if (newY < 0)
                    newY = 0;
                m_viewY = newY;
                DrawMap();
                UpdateMapView();
                DrawRadar(true);
            }
        } else {
            message = gpInputManager->GetEvent();
        }
    }
    m_verticalKnob->m_flags &= ~WIDGET_FLAG_SELECTED;
    m_verticalTrack->m_flags &= ~WIDGET_FLAG_SELECTED;
}

VTBL(editManager, 0x0045b368)

// The save checks' messages: AddError keeps a copy of each, ShowErrors
// shows them one by one and ClearErrors frees them.
DATA(0x0049f7b0) char* gEditErrors[EDIT_MANAGER_ERROR_CAPACITY];
DATA(0x004a3a48) i32 gEditErrorCount;

// Reports a travel gate kind with a single gate, a lone whirlpool and a
// barrier whose tent is missing.
VA(0x004062c0, 0x2de)
void editManager::CheckObjects(void) {
    char barrierColors[KB_BARRIER_COLOR_NAME_COUNT];
    i32 gates[EDIT_TRAVEL_GATE_KINDS];
    char tentColors[KB_BARRIER_COLOR_NAME_COUNT];
    i32 x;
    i32 whirlpools;
    i32 y;
    i32 i;

    whirlpools = 0;
    for (i = 0; i < KB_BARRIER_COLOR_NAME_COUNT; i++) {
        barrierColors[i] = 0;
        tentColors[i] = 0;
    }
    gates[0] = 0;
    gates[1] = 0;
    gates[2] = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            if (gMap.CellAt(x, y)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BARRIER))
                barrierColors[gMap.CellAt(x, y)->m_objectMetadata & EDIT_BARRIER_COLOR_MASK] = 1;
            if (gMap.CellAt(x, y)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_TRAVELER_TENT))
                tentColors[gMap.CellAt(x, y)->m_objectMetadata & EDIT_BARRIER_COLOR_MASK] = 1;
            if (gMap.CellAt(x, y)->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_STONE_LITHS))
                gates[gMap.CellAt(x, y)->m_objectIndex % EDIT_TRAVEL_GATE_FRAMES
                      / EDIT_TRAVEL_GATE_KIND_FRAMES]++;
            if (x < MAP_WIDTH - EDIT_WHIRLPOOL_SECOND_CELL_X
                && y < MAP_HEIGHT - EDIT_WHIRLPOOL_SECOND_CELL_Y
                && MAP_TRIGGER_OBJECT(gMap.CellAt(x, y)->m_triggerType) == MAP_OBJECT_WHIRLPOOL
                && MAP_TRIGGER_OBJECT(gMap.CellAt(x + EDIT_WHIRLPOOL_SECOND_CELL_X,
                                                  y + EDIT_WHIRLPOOL_SECOND_CELL_Y)
                                          ->m_triggerType)
                       == MAP_OBJECT_WHIRLPOOL)
                whirlpools++;
        }
    }
    if (gates[0] == 1 || gates[1] == 1 || gates[2] == 1)
        AddError(localization::Tr("editor.check.travel_gate.single"));
    if (whirlpools == 1)
        AddError(localization::Tr("editor.check.whirlpool.single"));
    for (i = 0; i < KB_BARRIER_COLOR_NAME_COUNT; i++) {
        if (barrierColors[i] && !tentColors[i]) {
            sprintf(gText, localization::Tr("editor.check.barrier.no_tent"), xBarrierColor[i],
                    xBarrierColor[i]);
            AddError(gText);
        }
    }
}

// Asks a yes/no question; true when answered yes.
VA(0x00408682, 0x41)
b32 editManager::Confirm(char* question) {
    NormalDialog(question, NORMAL_DIALOG_CONFIRM);
    if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_YES)
        return true;
    return false;
}

// Whether any map cell carries the trigger.
VA(0x004086c3, 0x7b)
b32 editManager::HasObject(i32 trigger) {
    i32 x;
    i32 y;

    for (y = 0; y < MAP_HEIGHT; y++)
        for (x = 0; x < MAP_WIDTH; x++)
            if (gMap.CellAt(x, y)->m_triggerType == trigger)
                return true;
    return false;
}

// The artifacts on the map (placed or random), its map events, its towns
// and castles, and the sites a player can capture.
VA(0x0040873e, 0xb2)
i32 editManager::CountArtifacts(void) {
    i32 count;
    i32 x;
    i32 y;

    count = 0;
    for (y = 0; y < MAP_HEIGHT; y++)
        for (x = 0; x < MAP_WIDTH; x++)
            if (MAP_TRIGGER_OBJECT(gMap.CellAt(x, y)->m_triggerType) == MAP_OBJECT_ARTIFACT
                || MAP_TRIGGER_OBJECT(gMap.CellAt(x, y)->m_triggerType)
                       == MAP_OBJECT_RANDOM_ARTIFACT)
                count++;
    return count;
}

VA(0x004087f0, 0x8d)
i32 editManager::CountEvents(void) {
    i32 count;
    i32 x;
    i32 y;
    mapCell* cell;

    count = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT))
                count++;
        }
    }
    return count;
}

VA(0x0040887d, 0xaf)
i32 editManager::CountTowns(void) {
    i32 count;
    i32 x;
    i32 y;
    mapCell* cell;

    count = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE))
                count++;
        }
    }
    return count;
}

VA(0x0040892c, 0xf9)
i32 editManager::CountMines(void) {
    i32 y;
    i32 x;
    i32 mineTotal;
    mapCell* cell;

    mineTotal = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ABANDONED_MINE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MINE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_EYE_OF_MAGI)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_DRAGON_CITY)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_LIGHTHOUSE))
                mineTotal++;
        }
    }
    return mineTotal;
}

// The map file's town table: x, y and faction per town (castles set bit 7),
// padded with empty records.
#define cell spot           // frame-slot spelling
#define townExtra townData // frame-slot spelling
VA(0x00408a25, 0x190)
void editManager::WriteTowns(i32 file) {
    i32 count;
    i32 setTowns;
    i32 type;
    i32 setCastles;
    i32 x;
    mapCell* cell;
    i32 y;
    i32 notUsed;
    EditMapRecord empty;
    i32 castleCount;
    i32 index;
    TownExtra* townExtra;

    count = 0;
    castleCount = 0;
    setCastles = 0;
    setTowns = 0;
    cell = NULL;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)) {
                write(file, &x, 1);
                write(file, &y, 1);
                townExtra = static_cast<TownExtra*>(gEditManager->m_extras[cell->m_objectMetadata]);
                type = townExtra->faction;
                if (townExtra->isCastle)
                    type |= EDIT_MAP_CASTLE_FLAG;
                write(file, &type, 1);
                count++;
            }
        }
    }
    empty.x = EDIT_MAP_NO_RECORD;
    empty.y = EDIT_MAP_NO_RECORD;
    empty.type = 0;
    for (x = 0; x < EDIT_MAP_TOWN_RECORDS - count; x++)
        WRITE_FILE_VALUE(file, empty);
}
#undef cell
#undef townExtra

// The map file's capturable-site table: every mine (its resource from the
// overlay of its extra chain), sawmill, alchemist lab, lighthouse, dragon
// city and abandoned mine, padded with empty records.
#define cell spot                   // frame-slot spelling
#define lighthouseX beaconX         // frame-slot spelling
#define lighthouseY beaconY         // frame-slot spelling
#define lighthouseCount beaconCount // frame-slot spelling
#define cityCount numCities         // frame-slot spelling
#define markerCell neighbour        // frame-slot spelling
#define mineSlot mineNumber         // frame-slot spelling
#define link extraIndex             // frame-slot spelling
VA(0x00408bb5, 0x313)
void editManager::WriteMines(i32 file) {
    u8 type;
    i32 lighthouseCount;
    u8 cityX;
    u8 cityY;
    u8 x;
    mapCell* cell;
    u8 y;
    EditMapRecord empty;
    u8 lighthouseX;
    i32 mineSlot;
    u8 lighthouseY;
    i32 cityCount;
    mapCell* markerCell;
    i32 link;

    cityX = EDIT_MAP_NO_RECORD;
    cityY = EDIT_MAP_NO_RECORD;
    lighthouseX = EDIT_MAP_NO_RECORD;
    lighthouseY = EDIT_MAP_NO_RECORD;
    lighthouseCount = 0;
    cityCount = 0;
    cell = NULL;
    empty.x = EDIT_MAP_NO_RECORD;
    empty.y = EDIT_MAP_NO_RECORD;
    empty.type = EDIT_MAP_NO_RECORD;
    mineSlot = 0;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_DRAGON_CITY)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_LIGHTHOUSE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MINE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ALCHEMIST_LAB)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ABANDONED_MINE)
                || cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_EYE_OF_MAGI)) {
                if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_LIGHTHOUSE)) {
                    type = EDIT_MINE_TYPE_LIGHTHOUSE;
                } else if (cell->m_triggerType
                           == MAP_ACTION_TRIGGER(MAP_OBJECT_ABANDONED_MINE)) {
                    type = EDIT_MINE_TYPE_ABANDONED_MINE;
                } else if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_DRAGON_CITY)) {
                    type = EDIT_MINE_TYPE_DRAGON_CITY;
                } else if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_EYE_OF_MAGI)) {
                    type = EDIT_MINE_TYPE_EYE_OF_MAGI;
                } else {
                    markerCell = gMap.CellAt(x, y);
                    link = markerCell->m_extraIndex;
                    while (link) {
                        if (gMap.Extra(link)->objectIndex != MAPCELL_SPRITE_NONE
                            && gMap.Extra(link)->objectTileset == TILESET_EXTRAOVR) {
                            type = gMap.Extra(link)->objectIndex + EDIT_MINE_TYPE_FIRST_ORE;
                            goto write;
                        }
                        link = gMap.Extra(link)->nextIndex;
                    }
                    if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SAWMILL))
                        type = EDIT_MINE_TYPE_WOOD;
                    else
                        type = EDIT_MINE_TYPE_MERCURY;
                }
            write:
                WRITE_FILE_VALUE(file, x);
                WRITE_FILE_VALUE(file, y);
                WRITE_FILE_VALUE(file, type);
                sprintf(gText, "Mine %02d: (%02d,%02d) type: %02d\n", mineSlot, x, y, type);
                LogStr(gText);
                mineSlot++;
            }
        }
    }
    for (x = 0; x < EDIT_MAP_MINE_RECORDS - mineSlot; x++)
        WRITE_FILE_VALUE(file, empty);
}
#undef cell
#undef lighthouseX
#undef lighthouseY
#undef lighthouseCount
#undef cityCount
#undef markerCell
#undef mineSlot
#undef link

// The map file keeps no artifact table.
VA(0x00408ec8, 0xd)
void editManager::WriteArtifacts(i32 H2_UNUSED(file)) {
}

VA(0x00408ed5, 0xed)
void editManager::WriteObelisks(i32 file) {
    u8 obeliskCount;
    i32 x;
    i32 y;
    mapCell* cell;

    obeliskCount = 0;
    cell = NULL;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            cell = gMap.CellAt(x, y);
            if (cell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_OBELISK))
                obeliskCount++;
        }
    }
    if (!obeliskCount)
        obeliskCount++;
    WRITE_FILE_VALUE(file, obeliskCount);
    if (obeliskCount > EDIT_MAP_OBELISK_LIMIT) {
        sprintf(gText, localization::Tr("editor.check.obelisks.many"), obeliskCount);
        AddError(gText);
    }
}

VA(0x0040990c, 0x4d)
void editManager::ClearErrors(void) {
    for (; gEditErrorCount > 0; gEditErrorCount--)
        delete gEditErrors[gEditErrorCount - 1];
    gEditErrorCount = 0;
}

// Logs every message, then shows them one at a time until one is answered
// no.
VA(0x00409959, 0xdd)
void editManager::ShowErrors(void) {
    i32 oldDebugLevel;
    i32 i;

    oldDebugLevel = giDebugLevel;
    if (gEditErrorCount > 0) {
        for (i = 0; i < gEditErrorCount; i++) {
            sprintf(gText, localization::Tr("editor.check.log"), gEditErrors[i]);
            LogStr(gText);
        }
        giDebugLevel = oldDebugLevel;
        for (i = 0; i < gEditErrorCount; i++) {
            sprintf(gText, localization::Tr("editor.check.next"), gEditErrors[i]);
            NormalDialog(gText, NORMAL_DIALOG_CONFIRM);
            if (gpWindowManager->m_dialogResult == NORMAL_DIALOG_NO)
                break;
        }
    }
}

VA(0x00409a36, 0x72)
void editManager::AddError(char* text) {
    i32 length;

    if (gEditErrorCount < EDIT_MANAGER_ERROR_CAPACITY) {
        length = strlen(text) + 1;
        gEditErrors[gEditErrorCount] = new char[length];
        strcpy(gEditErrors[gEditErrorCount], text);
        gEditErrorCount++;
    }
}
