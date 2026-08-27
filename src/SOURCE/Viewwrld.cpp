#include <Ints.h>
#include <BASE/message.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bs.h>
#include <BASE/Iconf2b.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/soundManager.h>
#include <BASE/bmap2.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/playerData.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Viewwrld.h>
#include <stdio.h>
#include <string.h>

typedef enum ViewWorldConstant {
    WORLD_PALETTE_SIZE         = PALETTE_DATA_SIZE,
    WORLD_WINDOW_X             = 0x1e0,
    WORLD_WINDOW_Y             = 0x10,
    WORLD_ICON_WIDGET          = 3,
    WORLD_POINTER_FRAME        = 0,
    WORLD_TILESET_COUNT        = H2EnumIndex(TILESET_COUNT),
    WORLD_GROUND_SHAPE_MASK    = GROUND_SHAPE_FLIPPED - 1,
    WORLD_TERRAIN_FRAME_STRIDE = 21,
    WORLD_DRAW_SIZE            = 0x1c0,
    WORLD_LEFT                 = 0x10,
    WORLD_TOP                  = 0x10,
    WORLD_RIGHT                = 0x1d0,
    WORLD_BOTTOM               = 0x1d0,
    WORLD_SCREEN_WIDTH         = 0x280,
    WORLD_SCREEN_HEIGHT        = 0x1e0,
    WORLD_BACKGROUND_COLOR     = 0x24,
    WORLD_NO_OWNER_COLOR       = 6,
    WORLD_HIGHLIGHT_BASE       = 0xd7,
    WORLD_ARTIFACT_HIGHLIGHT   = 0xd6,
    WORLD_RESOURCE_HIGHLIGHT   = 0xdd,
    WORLD_NO_SPRITE            = MAPCELL_SPRITE_NONE,
    WORLD_RADAR_WIDGET         = 9,
    WORLD_SCALE_CONTROL        = 2,
    WORLD_RADAR_LEFT           = 0x1e0,
    WORLD_RADAR_RIGHT          = 0x270,
    WORLD_RADAR_TOP            = 0x10,
    WORLD_RADAR_BOTTOM         = 0xa0,
    INITIAL_CENTER_OFFSET      = 7,
    SCALE_OFFSET_CAPACITY      = 4,
    GROUND_HORIZONTAL_FLIP     = 2,
    GROUND_ALTERNATE_SET       = 1,
    GROUND_ALTERNATE_OFFSET    = 9,
    GROUND_RANDOM_X_MULTIPLIER = 2,
    GROUND_RANDOM_FRAME_MASK   = 3,
    GROUND_FRAME_OFFSET        = 3,
    ARTIFACT_ICON_FRAME        = 14,
    HERO_ICON_FRAME_BASE       = 7,
    RESOURCE_ICON_FRAME        = 13,
    RADAR_DRAG_MAP_SCALE       = 2
} ViewWorldConstant;

typedef enum ViewWorldLegendIndex {
    LEGEND_MINES     = 0,
    LEGEND_RESOURCES = 1,
    LEGEND_ARTIFACTS = 2,
    LEGEND_TOWNS     = 3,
    LEGEND_HEROES    = 4,
    LEGEND_ALL       = 5,
    LEGEND_WORLD     = 6,
    LEGEND_COUNT     = 7
} ViewWorldLegendIndex;

typedef enum ViewWorldScaleIndex {
    SCALE_INDEX_FAR    = 0,
    SCALE_INDEX_MIDDLE = 1,
    SCALE_INDEX_NEAR   = 2
} ViewWorldScaleIndex;

typedef enum ViewWorldOffsetKind {
    OFFSET_HERO     = 0,
    OFFSET_RESOURCE = 1,
    OFFSET_ARTIFACT = 2,
    OFFSET_MINE     = 3,
    OFFSET_TOWN     = 4,
    OFFSET_LETTER   = 5
} ViewWorldOffsetKind;

typedef enum ViewWorldGroundFrame {
    GROUND_FRAME_ZERO  = 0,
    GROUND_FRAME_ONE   = 1,
    GROUND_FRAME_TWO   = 2,
    GROUND_FRAME_THREE = 3,
    GROUND_FRAME_FOUR  = 4,
    GROUND_FRAME_FIVE  = 5,
    GROUND_FRAME_SIX   = 6,
    GROUND_FRAME_SEVEN = 7,
    GROUND_FRAME_EIGHT = 8
} ViewWorldGroundFrame;

void advManager::ViewWorld(SpellType whatToDraw, b32 drawAllObjects, b32 drawAllTerrains) {
    heroWindow* window;
    i8 palette[WORLD_PALETTE_SIZE];
    const char* iconNames[LEGEND_COUNT];
    tag_message legendMessage;

    memcpy(palette, gpBufferPalette->m_data, WORLD_PALETTE_SIZE);
    gbInViewWorld = true;
    iVWWhatToDraw = whatToDraw;
    iVWDrawAllObjs = drawAllObjects;
    iVWDrawAllTerrains = drawAllTerrains;
    gpWindowManager->m_updateFlags = 0;
    giCycleType = WINDOW_COLOR_CYCLE_WORLD_VIEW;
    VWInit(m_mapOriginX + INITIAL_CENTER_OFFSET, m_mapOriginY + INITIAL_CENTER_OFFSET);
    VWCompleteDraw();
    gpWindowManager->m_updateFlags = 1;

    sprintf(
        gText,
        "viewlgnd.bin"
    );
    window = new heroWindow(WORLD_WINDOW_X, WORLD_WINDOW_Y, gText);
    if (window == NULL)
        MemError();

    iconNames[LEGEND_MINES] =
        "MINE";
    iconNames[LEGEND_RESOURCES] =
        "RSRC";
    iconNames[LEGEND_ARTIFACTS] =
        "RTFX";
    iconNames[LEGEND_TOWNS] =
        "TWNS";
    iconNames[LEGEND_HEROES] =
        "HROS";
    iconNames[LEGEND_ALL] =
        "_ALL";
    iconNames[LEGEND_WORLD] =
        "WRLD";
    sprintf(
        gText,
        "view%s.icn",
        iconNames[whatToDraw == SPELL_VIEW_ALL && drawAllObjects == 0 && drawAllTerrains == 0
                          ? LEGEND_WORLD
                          : H2EnumIndex(whatToDraw) - H2EnumIndex(SPELL_VIEW_MINES)]
    );
    legendMessage.type = VIEW_WORLD_MESSAGE;
    legendMessage.payload.widget.command = VIEW_WORLD_ICON_MESSAGE;
    legendMessage.payload.widget.id = WORLD_ICON_WIDGET;
    legendMessage.payload.widget.data.text = gText;
    window->BroadcastMessage(legendMessage);
    gpWindowManager->DoDialog(window, ViewWorldDialogHandler, 0);
    delete window;

    UpdateRadar(1, 0);
    VWCleanup();
    gbInViewWorld = false;
    gpWindowManager->m_updateFlags = 0;
    RedrawAdvScreen(1, 0);
    giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
    gpWindowManager->m_updateFlags = 1;
    SetPalette(palette, 1);
}

void advManager::VWCleanup(void) {
    gpResourceManager->Dispose(pVWGround);
    gpResourceManager->Dispose(pVWFlags);
    gpResourceManager->Dispose(pVWMisc);
    gpResourceManager->Dispose(pVWLetters);
}

void advManager::VWInit(i32 centerX, i32 centerY) {
    if (giViewWorldScale == VIEW_WORLD_SCALE_FAR)
        giViewWorldScaleLookup = SCALE_INDEX_FAR;
    else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE)
        giViewWorldScaleLookup = SCALE_INDEX_MIDDLE;
    else
        giViewWorldScaleLookup = SCALE_INDEX_NEAR;

    iVWViewableCells = WORLD_DRAW_SIZE / H2EnumIndex(giViewWorldScale);
    if (iVWViewableCells > MAP_WIDTH)
        iVWViewableCells = MAP_WIDTH;
    iVWCenterOffset = iVWViewableCells >> 1;
    iVWMapOriginY = 0;
    iVWMapOriginX = iVWMapOriginY;
    if (iVWViewableCells != MAP_WIDTH) {
        iVWMapOriginX = centerX - (iVWViewableCells >> 1);
        if (iVWMapOriginX < 0)
            iVWMapOriginX = 0;
        if (iVWMapOriginX + iVWViewableCells >= MAP_WIDTH)
            iVWMapOriginX = MAP_WIDTH - iVWViewableCells;
        iVWMapOriginY = centerY - (iVWViewableCells >> 1);
        if (iVWMapOriginY < 0)
            iVWMapOriginY = 0;
        if (iVWMapOriginY + iVWViewableCells >= MAP_HEIGHT)
            iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
    }

    iVWXPixelOffset = (WORLD_WINDOW_X - H2EnumIndex(iVWViewableCells) * H2EnumIndex(giViewWorldScale)) >> 1;
    iVWYPixelOffset = iVWXPixelOffset;
    gpMouseManager->SetPointer(
        "advmice.mse",
        WORLD_POINTER_FRAME,
        MOUSE_AUTO_CURSOR_TYPE
    );
    sprintf(
        gText,
        "ground%d.icn",
        H2EnumIndex(giViewWorldScale)
    );
    pVWGround = gpResourceManager->GetIcon(gText);
    sprintf(
        gText,
        "vwflag%d.icn",
        H2EnumIndex(giViewWorldScale)
    );
    pVWFlags = gpResourceManager->GetIcon(gText);
    sprintf(
        gText,
        "misc%d.icn",
        H2EnumIndex(giViewWorldScale)
    );
    pVWMisc = gpResourceManager->GetIcon(gText);
    sprintf(
        gText,
        "letter%d.icn",
        H2EnumIndex(giViewWorldScale)
    );
    pVWLetters = gpResourceManager->GetIcon(gText);
    UpdateRadar(1, 0);
}

void advManager::VWCompleteDraw(void) {
    u8* endPixel0;
    u8 drawTilesets0[WORLD_TILESET_COUNT];
    u8* pix0;

    i32 cellX;
    b32 heroHere0;
    mapCellExtra* extraCell;
    i32 cellY;

    u32 frame0;
    i32 drawY1;
    i32 screenX;
    MineType resource0;
    u32 shape;
    i8 iconX1[SCALE_OFFSET_CAPACITY];
    i8 townFlagX0[SCALE_OFFSET_CAPACITY];
    H2EnumStorage<IconDrawOrientation, u32> orientation1;
    i32 color0;
    mapCell* cell;
    i8 letterY1[SCALE_OFFSET_CAPACITY];

    frame0 = 0;
    FillBitmapArea(
        gpWindowManager->m_screen,
        WORLD_LEFT,
        WORLD_TOP,
        WORLD_DRAW_SIZE,
        WORLD_DRAW_SIZE,
        WORLD_BACKGROUND_COLOR
    );
    memset(drawTilesets0, 1, WORLD_TILESET_COUNT);
    drawTilesets0[H2EnumIndex(TILESET_OBJNARTI)] = 0;
    drawTilesets0[H2EnumIndex(TILESET_ART32)] = 0;
    drawTilesets0[H2EnumIndex(TILESET_FLAG32)] = 0;

    for (cellY = iVWMapOriginY; cellY < iVWMapOriginY + iVWViewableCells; cellY++) {
        for (cellX = iVWMapOriginX; cellX < iVWMapOriginX + iVWViewableCells; cellX++) {
            cell = GetCell(cellX, cellY);
            if ((MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit) || iVWDrawAllTerrains
                || (iVWWhatToDraw == SPELL_VIEW_TOWNS
                    && (((cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_CASTLE)
                        || (cell->m_triggerType
                                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                            && gpGame->m_heroRecs[cell->m_objectMetadata].m_locationType
                                   == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))))) {
                orientation1 = ICON_DRAW_NORMAL;
                screenX = iVWXPixelOffset + (cellX - iVWMapOriginX) * H2EnumIndex(giViewWorldScale);
                drawY1 = iVWYPixelOffset + (cellY - iVWMapOriginY) * H2EnumIndex(giViewWorldScale);
                shape =
                    giGroundShape[cell->m_terrainImageIndex] & WORLD_GROUND_SHAPE_MASK;
                switch (shape) {
                    case 0:
                    case 9:
                    case 18:
                    case 19:
                    case 20:
                    case 21:
                        frame0 = GROUND_FRAME_ZERO;
                        break;
                    case 1:
                    case 12:
                    case 15:
                    case 16:
                        frame0 = GROUND_FRAME_ONE;
                        break;
                    case 2:
                    case 10:
                    case 11:
                        frame0 = GROUND_FRAME_TWO;
                        break;
                    case 3:
                    case 13:
                    case 14:
                    case 17:
                        frame0 = GROUND_FRAME_THREE;
                        break;
                    case 4:
                        frame0 = GROUND_FRAME_FOUR;
                        break;
                    case 5:
                        frame0 = GROUND_FRAME_FIVE;
                        break;
                    case 6:
                        frame0 = GROUND_FRAME_SIX;
                        break;
                    case 7:
                        frame0 = GROUND_FRAME_SEVEN;
                        break;
                    case 8:
                        frame0 = GROUND_FRAME_EIGHT;
                        break;
                    default:
                        break;
                }
                if (cell->m_flags & GROUND_HORIZONTAL_FLIP)
                    orientation1 = ICON_DRAW_FLIPPED;
                if (cell->m_flags & GROUND_ALTERNATE_SET)
                    frame0 += GROUND_ALTERNATE_OFFSET;
                if (frame0 == 0)
                    frame0 +=
                        (cellY * GROUND_RANDOM_X_MULTIPLIER + cellX) & GROUND_RANDOM_FRAME_MASK;
                else
                    frame0 += GROUND_FRAME_OFFSET;
                frame0 += H2EnumIndex(giGroundToTerrain[cell->m_terrainImageIndex])
                                 * WORLD_TERRAIN_FRAME_STRIDE;
                if ((MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit) || iVWDrawAllTerrains) {
                    pVWGround->DrawToBuffer(
                        screenX
                            + (orientation1 == ICON_DRAW_FLIPPED ? H2EnumIndex(giViewWorldScale) - 1 : 0),
                        drawY1,
                        frame0,
                        orientation1
                    );
                }

                if (cell->m_objectLayerBit0 && cell->m_objectIndex != WORLD_NO_SPRITE
                    && drawTilesets0[H2EnumIndex(cell->m_objectTileset)]) {
                    IconToBitmapScale(
                        m_objectIcons[H2EnumIndex(cell->m_objectTileset)],
                        gpWindowManager->m_screen,
                        screenX,
                        drawY1,
                        cell->m_objectIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        H2EnumIndex(giViewWorldScale)
                    );
                }
                if (cell->m_extraIndex != 0
                    && m_mapData->Extra(cell->m_extraIndex)->objectIndex != WORLD_NO_SPRITE)
                    extraCell = m_mapData->Extra(cell->m_extraIndex);
                else
                    extraCell = NULL;
                while (extraCell != NULL) {
                    if (extraCell->objectLayerBit0 && extraCell->objectIndex != WORLD_NO_SPRITE
                        && drawTilesets0[H2EnumIndex(extraCell->objectTileset)]) {
                        IconToBitmapScale(
                            m_objectIcons[H2EnumIndex(extraCell->objectTileset)],
                            gpWindowManager->m_screen,
                            screenX,
                            drawY1,
                            extraCell->objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            H2EnumIndex(giViewWorldScale)
                        );
                    }
                    if (extraCell->nextIndex != 0
                        && m_mapData->Extra(extraCell->nextIndex)->objectIndex
                               != WORLD_NO_SPRITE)
                        extraCell = m_mapData->Extra(extraCell->nextIndex);
                    else
                        extraCell = NULL;
                }

                if (!cell->m_objectLayerBit0 && cell->m_objectIndex != WORLD_NO_SPRITE
                    && drawTilesets0[H2EnumIndex(cell->m_objectTileset)]) {
                    IconToBitmapScale(
                        m_objectIcons[H2EnumIndex(cell->m_objectTileset)],
                        gpWindowManager->m_screen,
                        screenX,
                        drawY1,
                        cell->m_objectIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        H2EnumIndex(giViewWorldScale)
                    );
                }
                if (cell->m_extraIndex != 0
                    && m_mapData->Extra(cell->m_extraIndex)->objectIndex != WORLD_NO_SPRITE)
                    extraCell = m_mapData->Extra(cell->m_extraIndex);
                else
                    extraCell = NULL;
                while (extraCell != NULL) {
                    if (!extraCell->objectLayerBit0 && extraCell->objectIndex != WORLD_NO_SPRITE
                        && drawTilesets0[H2EnumIndex(extraCell->objectTileset)]) {
                        IconToBitmapScale(
                            m_objectIcons[H2EnumIndex(extraCell->objectTileset)],
                            gpWindowManager->m_screen,
                            screenX,
                            drawY1,
                            extraCell->objectIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            H2EnumIndex(giViewWorldScale)
                        );
                    }
                    if (extraCell->nextIndex != 0
                        && m_mapData->Extra(extraCell->nextIndex)->objectIndex
                               != WORLD_NO_SPRITE)
                        extraCell = m_mapData->Extra(extraCell->nextIndex);
                    else
                        extraCell = NULL;
                }

                if (cell->m_overlayIndex != WORLD_NO_SPRITE
                    && drawTilesets0[H2EnumIndex(cell->m_overlayTileset)]) {
                    IconToBitmapScale(
                        m_objectIcons[H2EnumIndex(cell->m_overlayTileset)],
                        gpWindowManager->m_screen,
                        screenX,
                        drawY1,
                        cell->m_overlayIndex,
                        ICON_DRAW_NO_CLIP,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        H2EnumIndex(giViewWorldScale)
                    );
                }
                if (cell->m_extraIndex != 0
                    && m_mapData->Extra(cell->m_extraIndex)->overlayIndex != WORLD_NO_SPRITE)
                    extraCell = m_mapData->Extra(cell->m_extraIndex);
                else
                    extraCell = NULL;
                while (extraCell != NULL) {
                    if (drawTilesets0[H2EnumIndex(extraCell->overlayTileset)]) {
                        IconToBitmapScale(
                            m_objectIcons[H2EnumIndex(extraCell->overlayTileset)],
                            gpWindowManager->m_screen,
                            screenX,
                            drawY1,
                            extraCell->overlayIndex,
                            ICON_DRAW_NO_CLIP,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            H2EnumIndex(giViewWorldScale)
                        );
                    }
                    if (extraCell->nextIndex != 0
                        && m_mapData->Extra(extraCell->nextIndex)->overlayIndex
                               != WORLD_NO_SPRITE)
                        extraCell = m_mapData->Extra(extraCell->nextIndex);
                    else
                        extraCell = NULL;
                }
            }
        }
    }

    for (cellY = WORLD_TOP; cellY < WORLD_BOTTOM; cellY++) {
        pix0 =
            gpWindowManager->m_screen->m_pixels + cellY * WORLD_SCREEN_WIDTH + WORLD_LEFT;
        endPixel0 = pix0 + WORLD_DRAW_SIZE;
        for (; pix0 < endPixel0; pix0++)
            *pix0 = gColorTableNoCycle[*pix0];
    }

    for (cellY = iVWMapOriginY; cellY < iVWMapOriginY + iVWViewableCells; cellY++) {
        for (cellX = iVWMapOriginX; cellX < iVWMapOriginX + iVWViewableCells; cellX++) {
            cell = GetCell(cellX, cellY);
            screenX = iVWXPixelOffset + (cellX - iVWMapOriginX) * H2EnumIndex(giViewWorldScale)
                      + (H2EnumIndex(giViewWorldScale) >> 1);
            drawY1 = iVWYPixelOffset + (cellY - iVWMapOriginY) * H2EnumIndex(giViewWorldScale)
                      + (H2EnumIndex(giViewWorldScale) >> 1);

            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT)
                && (iVWDrawAllObjs || (MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit)
                    || iVWWhatToDraw == SPELL_VIEW_ARTIFACTS)) {
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_ARTIFACT][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_ARTIFACT][H2EnumIndex(COORDINATE_AXIS_Y)],
                    ARTIFACT_ICON_FRAME,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == SPELL_VIEW_ARTIFACTS ? WORLD_ARTIFACT_HIGHLIGHT : 0
                );
            }

            if ((cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
                 || (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                     && gpGame->m_heroRecs[cell->m_objectMetadata].m_locationType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)))
                && (iVWDrawAllObjs || (MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit)
                    || iVWWhatToDraw == SPELL_VIEW_TOWNS)) {
                if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                    color0 = gpGame->m_castleOwners[cell->m_objectMetadata];
                else
                    color0 =
                        gpGame->m_castleOwners[gpGame->m_heroRecs[cell->m_objectMetadata]
                                                   .m_occupiedTown];
                if (color0 < 0)
                    color0 = WORLD_NO_OWNER_COLOR;
                else
                    color0 = gpGame->m_players[color0].m_color;
                townFlagX0[SCALE_INDEX_FAR] = -2;
                townFlagX0[SCALE_INDEX_MIDDLE] = -3;
                townFlagX0[SCALE_INDEX_NEAR] = -6;
                iconX1[SCALE_INDEX_FAR] = 5;
                iconX1[SCALE_INDEX_MIDDLE] = 7;
                iconX1[SCALE_INDEX_NEAR] = 10;
                FlipIconToBitmap(
                    pVWFlags,
                    gpWindowManager->m_screen,
                    screenX + townFlagX0[giViewWorldScaleLookup]
                        - iVWHalf[giViewWorldScaleLookup][OFFSET_TOWN][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_TOWN][H2EnumIndex(COORDINATE_AXIS_Y)],
                    color0,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == SPELL_VIEW_TOWNS ? color0 + WORLD_HIGHLIGHT_BASE : 0
                );
                IconToBitmap(
                    pVWFlags,
                    gpWindowManager->m_screen,
                    screenX + iconX1[giViewWorldScaleLookup]
                        - iVWHalf[giViewWorldScaleLookup][OFFSET_TOWN][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_TOWN][H2EnumIndex(COORDINATE_AXIS_Y)],
                    color0,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == SPELL_VIEW_TOWNS ? color0 + WORLD_HIGHLIGHT_BASE : 0
                );
            }

            if (gpCurPlayer->m_currentHero != -1
                && gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_x == cellX
                && gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_y == cellY)
                heroHere0 = true;
            else
                heroHere0 = false;
            if ((cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                 || heroHere0)
                && (iVWDrawAllObjs || (MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit)
                    || iVWWhatToDraw == SPELL_VIEW_HEROES)) {
                if (heroHere0)
                    color0 = gpGame->m_availableHeroes[gpCurPlayer->m_currentHero];
                else
                    color0 = gpGame->m_availableHeroes[cell->m_objectMetadata];
                if (color0 >= 0) {
                    frame0 = gpGame->m_players[color0].m_color;
                    IconToBitmap(
                        pVWMisc,
                        gpWindowManager->m_screen,
                        screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_HERO][H2EnumIndex(COORDINATE_AXIS_X)],
                        drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_HERO][H2EnumIndex(COORDINATE_AXIS_Y)],
                        frame0 + HERO_ICON_FRAME_BASE,
                        ICON_DRAW_CLIP,
                        WORLD_LEFT,
                        WORLD_TOP,
                        WORLD_DRAW_SIZE,
                        WORLD_DRAW_SIZE,
                        iVWWhatToDraw == SPELL_VIEW_HEROES ? frame0 + WORLD_HIGHLIGHT_BASE
                                                           : 0
                    );
                }
            }

            if (cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE)
                && (iVWDrawAllObjs || (MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit)
                    || iVWWhatToDraw == SPELL_VIEW_RESOURCES)) {
                frame0 = cell->m_objectIndex / 2;
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_RESOURCE][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_RESOURCE][H2EnumIndex(COORDINATE_AXIS_Y)],
                    RESOURCE_ICON_FRAME,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == SPELL_VIEW_RESOURCES ? WORLD_RESOURCE_HIGHLIGHT : 0
                );
                IconToBitmap(
                    pVWLetters,
                    gpWindowManager->m_screen,
                    screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_LETTER][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_LETTER][H2EnumIndex(COORDINATE_AXIS_Y)],
                    frame0,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    0
                );
            }

            if ((cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)
                 || cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL)
                 || cell->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB))
                && (iVWDrawAllObjs || (MAP_EXTRA_AT_WFIRST(cellX, cellY) & giCurPlayerBit)
                    || iVWWhatToDraw == SPELL_VIEW_MINES)) {
                color0 = gpGame->m_mineOwners[cell->m_objectMetadata];
                frame0 = color0 < 0 ? H2EnumIndex(WORLD_NO_OWNER_COLOR)
                                                  : gpGame->m_players[color0].m_color;
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_MINE][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 - iVWHalf[giViewWorldScaleLookup][OFFSET_MINE][H2EnumIndex(COORDINATE_AXIS_Y)],
                    frame0,
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == SPELL_VIEW_MINES ? frame0 + WORLD_HIGHLIGHT_BASE : 0
                );
                letterY1[SCALE_INDEX_FAR] = 0;
                letterY1[SCALE_INDEX_MIDDLE] = 0;
                letterY1[SCALE_INDEX_NEAR] = 2;
                resource0 = gpGame->m_mines[cell->m_objectMetadata].resourceType;
                IconToBitmap(
                    pVWLetters,
                    gpWindowManager->m_screen,
                    screenX - iVWHalf[giViewWorldScaleLookup][OFFSET_LETTER][H2EnumIndex(COORDINATE_AXIS_X)],
                    drawY1 + letterY1[giViewWorldScaleLookup]
                        - iVWHalf[giViewWorldScaleLookup][OFFSET_LETTER][H2EnumIndex(COORDINATE_AXIS_Y)],
                    H2EnumIndex(resource0),
                    ICON_DRAW_CLIP,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    0
                );
            }
        }
    }
    gpWindowManager->UpdateScreenRegion(
        WORLD_LEFT,
        WORLD_TOP,
        WORLD_DRAW_SIZE,
        WORLD_DRAW_SIZE
    );
}

MessageDispatchResult ViewWorldDialogHandler(struct tag_message& message) {
    float radarScale;
    tag_message oldMessage;
    tag_message eventMessage;
    i32 mapX;
    i32 mapY;

    if (!gpSoundManager->MusicPlaying())
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[H2EnumIndex(gpAdvManager->m_currentTerrain)]
        );

    if (message.type == VIEW_WORLD_MESSAGE) {
        switch (message.payload.widget.command) {
            case VIEW_WORLD_SELECT:
                if (message.payload.widget.id == WORLD_RADAR_WIDGET) {
                    if ((giViewWorldScale == VIEW_WORLD_SCALE_NEAR
                         && MAP_WIDTH <= MAP_DIMENSION_SMALL)
                        || (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE
                            && MAP_WIDTH <= MAP_DIMENSION_MEDIUM)
                        || (giViewWorldScale == VIEW_WORLD_SCALE_FAR
                            && MAP_WIDTH <= MAP_DIMENSION_LARGE))
                        break;
                    switch (MAP_HEIGHT) {
                        case MAP_DIMENSION_SMALL:
                            radarScale = 4.0f;
                            break;
                        case MAP_DIMENSION_MEDIUM:
                            radarScale = 2.0f;
                            break;
                        case MAP_DIMENSION_LARGE:
                            radarScale = 1.3333f;
                            break;
                        default:
                            radarScale = 1.0f;
                            break;
                    }

                    mapX = message.payload.mouse.screenX;
                    mapY = message.payload.mouse.screenY;
                    mapX = static_cast<i32>((mapX - WORLD_RADAR_LEFT) / radarScale);
                    mapY = static_cast<i32>((mapY - WORLD_RADAR_TOP) / radarScale);
                    iVWMapOriginX = mapX - iVWCenterOffset;
                    iVWMapOriginY = mapY - iVWCenterOffset;
                    if (iVWMapOriginX < 0)
                        iVWMapOriginX = 0;
                    if (iVWMapOriginY < 0)
                        iVWMapOriginY = 0;
                    if (iVWMapOriginX + iVWViewableCells >= MAP_WIDTH)
                        iVWMapOriginY = MAP_WIDTH - iVWViewableCells;
                    if (iVWMapOriginY + iVWViewableCells >= MAP_HEIGHT)
                        iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
                    gpAdvManager->UpdateRadar(1, 0);
                    gpAdvManager->VWCompleteDraw();

                    eventMessage.type = MESSAGE_NONE;
                    while (eventMessage.type != VIEW_WORLD_MOUSE_UP) {
                        platform::PumpEvents();
                        eventMessage = gpInputManager->GetEvent();
                        oldMessage = eventMessage;
                        while (eventMessage.type != VIEW_WORLD_MOUSE_UP
                               && eventMessage.type != MESSAGE_NONE) {
                            if (eventMessage.type == VIEW_WORLD_MOUSE_MOVE)
                                oldMessage = eventMessage;
                            platform::PumpEvents();
                            eventMessage = gpInputManager->GetEvent();
                        }
                        if (oldMessage.type == VIEW_WORLD_MOUSE_MOVE) {
                            if (oldMessage.payload.mouse.x < WORLD_RADAR_LEFT)
                                oldMessage.payload.mouse.x = WORLD_RADAR_LEFT;
                            if (oldMessage.payload.mouse.x >= WORLD_RADAR_RIGHT)
                                oldMessage.payload.mouse.x =
                                    MAP_WIDTH * RADAR_DRAG_MAP_SCALE + WORLD_RADAR_LEFT - 1;
                            if (oldMessage.payload.mouse.y < WORLD_RADAR_TOP)
                                oldMessage.payload.mouse.y = WORLD_RADAR_TOP;
                            if (oldMessage.payload.mouse.y >= WORLD_RADAR_BOTTOM)
                                oldMessage.payload.mouse.y =
                                    MAP_HEIGHT * RADAR_DRAG_MAP_SCALE + WORLD_RADAR_TOP - 1;
                            gpMouseManager->Main(oldMessage);
                            mapX = static_cast<i32>(
                                (oldMessage.payload.mouse.x - WORLD_RADAR_LEFT) / radarScale
                            );
                            mapY = static_cast<i32>(
                                (oldMessage.payload.mouse.y - WORLD_RADAR_TOP) / radarScale
                            );
                            iVWMapOriginX = mapX - iVWCenterOffset;
                            iVWMapOriginY = mapY - iVWCenterOffset;
                            if (iVWMapOriginX < 0)
                                iVWMapOriginX = 0;
                            if (iVWMapOriginY < 0)
                                iVWMapOriginY = 0;
                            if (iVWMapOriginX + iVWViewableCells >= MAP_WIDTH)
                                iVWMapOriginX = MAP_WIDTH - iVWViewableCells;
                            if (iVWMapOriginY + iVWViewableCells >= MAP_HEIGHT)
                                iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
                            gpAdvManager->UpdateRadar(1, 0);
                            gpAdvManager->VWCompleteDraw();
                            oldMessage.type = MESSAGE_NONE;
                        }
                    }
                }
                break;
            case VIEW_WORLD_RELEASE:
                switch (message.payload.widget.id) {
                    case WORLD_SCALE_CONTROL:
                        gpAdvManager->VWCleanup();
                        if (giViewWorldScale == VIEW_WORLD_SCALE_NEAR)
                            giViewWorldScale = VIEW_WORLD_SCALE_MIDDLE;
                        else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE)
                            giViewWorldScale = VIEW_WORLD_SCALE_FAR;
                        else
                            giViewWorldScale = VIEW_WORLD_SCALE_NEAR;
                        gpAdvManager->VWInit(
                            iVWMapOriginX + iVWCenterOffset,
                            iVWMapOriginY + iVWCenterOffset
                        );
                        gpAdvManager->VWCompleteDraw();
                        break;
                    case EVENT_WINDOW_FIRST_BUTTON:
                    case EVENT_WINDOW_SECOND_BUTTON:
                    case EVENT_WINDOW_THIRD_BUTTON:
                    case EVENT_WINDOW_FOURTH_BUTTON:
                    case EVENT_WINDOW_FIFTH_BUTTON:
                    case EVENT_WINDOW_SIXTH_BUTTON:
                        gpWindowManager->m_dialogResult = message.payload.widget.id;
                        message.payload.widget.id = EVENT_WINDOW_CLOSE_COMMAND;
                        message.payload.widget.command =
                            BaseWidgetCommand(EVENT_WINDOW_CLOSE_COMMAND);
                        return MESSAGE_DISPATCH_FORWARD;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return MESSAGE_DISPATCH_CONSUME;
}

i8 iVWHalf[ADVMGR_VIEW_WORLD_SCALE_COUNT][ADVMGR_VIEW_WORLD_OFFSET_KIND_COUNT]
            [H2EnumIndex(COORDINATE_AXIS_COUNT)] = {3,  3, 5, 5, 6, 6, 8, 5, 2, 3, 2,  2,
                                              4,  5, 6, 6, 8, 8, 11, 7, 3, 4, 3, 3,
                                              7,  7, 8, 8, 10, 10, 14, 8, 3, 6, 4, 4};
ViewWorldScale giViewWorldScale = VIEW_WORLD_SCALE_MIDDLE;
i32 giViewWorldScaleLookup = SCALE_INDEX_MIDDLE;
b32 gbInViewWorld = false;
class icon* pVWMisc;
class icon* pVWLetters;
i32 iVWYPixelOffset;
class icon* pVWGround;
i32 iVWViewableCells;
class icon* pVWFlags;
b32 iVWDrawAllTerrains;
H2EnumStorage<SpellType, i32> iVWWhatToDraw;
b32 iVWDrawAllObjs;
i32 iVWMapOriginX;
i32 iVWMapOriginY;
i32 iVWCenterOffset;
i32 iVWXPixelOffset;
