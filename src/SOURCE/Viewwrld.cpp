#include <va.h>
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
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Viewwrld.h>
#include <stdio.h>
#include <string.h>

H2_ENUM_BEGIN(ViewWorldConstant)
    WORLD_PALETTE_SIZE         = 0x300,
    WORLD_WINDOW_X             = 0x1e0,
    WORLD_WINDOW_Y             = 0x10,
    WORLD_ICON_WIDGET          = 3,
    WORLD_POINTER_FRAME        = 0,
    WORLD_TILESET_COUNT        = 64,
    WORLD_GROUND_SHAPE_MASK    = 0x7f,
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
    WORLD_NO_SPRITE            = 0xFF,
    WORLD_RADAR_WIDGET         = 9,
    WORLD_SCALE_CONTROL        = 2,
    WORLD_DIALOG_FIRST         = 0x7800,
    WORLD_DIALOG_EXCLUDED      = 0x7804,
    WORLD_DIALOG_LAST          = 0x7806,
    WORLD_DIALOG_CLOSE         = 10,
    WORLD_RADAR_LEFT           = 0x1e0,
    WORLD_RADAR_RIGHT          = 0x270,
    WORLD_RADAR_TOP            = 0x10,
    WORLD_RADAR_BOTTOM         = 0xa0
H2_ENUM_END(ViewWorldConstant)

VA(0x004333c0, 0x267)
void advManager::ViewWorld(ViewWorldMode whatToDraw, b32 drawAllObjects, b32 drawAllTerrains) {
    i8 paletteData[WORLD_PALETTE_SIZE];
    char* viewIconNames[7];
    tag_message dialogMessage;
    heroWindow* viewWorldWindow;

    memcpy(paletteData, gpBufferPalette->m_data, WORLD_PALETTE_SIZE);
    gbInViewWorld = true;
    iVWWhatToDraw = whatToDraw;
    iVWDrawAllObjs = drawAllObjects;
    iVWDrawAllTerrains = drawAllTerrains;
    gpWindowManager->m_updateFlags = 0;
    giCycleType = WINDOW_COLOR_CYCLE_WORLD_VIEW;
    VWInit(m_mapOriginX + 7, m_mapOriginY + 7);
    VWCompleteDraw();
    gpWindowManager->m_updateFlags = 1;

    sprintf(gText, "viewlgnd.bin");
    viewWorldWindow = new heroWindow(WORLD_WINDOW_X, WORLD_WINDOW_Y, gText);
    if (viewWorldWindow == NULL)
        MemError();

    i32 legendMode;
    viewIconNames[0] = "MINE";
    viewIconNames[1] = "RSRC";
    viewIconNames[2] = "RTFX";
    viewIconNames[3] = "TWNS";
    viewIconNames[4] = "HROS";
    viewIconNames[5] = "_ALL";
    viewIconNames[6] = "WRLD";
    if (whatToDraw == VIEW_WORLD_ALL && drawAllObjects == 0 && drawAllTerrains == 0)
        legendMode = 6;
    else
        legendMode = IDX(whatToDraw) - IDX(VIEW_WORLD_MINES);
    sprintf(gText, "view%s.icn", viewIconNames[legendMode]);
    dialogMessage.type = VIEW_WORLD_MESSAGE;
    dialogMessage.payload.widget.command = VIEW_WORLD_ICON_MESSAGE;
    dialogMessage.payload.widget.id = WORLD_ICON_WIDGET;
    dialogMessage.payload.widget.data.text = gText;
    viewWorldWindow->BroadcastMessage(dialogMessage);
    gpWindowManager->DoDialog(viewWorldWindow, ViewWorldDialogHandler, 0);
    delete viewWorldWindow;

    UpdateRadar(1, 0);
    VWCleanup();
    gbInViewWorld = false;
    gpWindowManager->m_updateFlags = 0;
    RedrawAdvScreen(1, 0);
    giCycleType = WINDOW_COLOR_CYCLE_DEFAULT;
    gpWindowManager->m_updateFlags = 1;
    SetPalette(paletteData, 1);
}

VA(0x00433627, 0x5a)
void advManager::VWCleanup(void) {
    gpResourceManager->Dispose(pVWGround);
    gpResourceManager->Dispose(pVWFlags);
    gpResourceManager->Dispose(pVWMisc);
    gpResourceManager->Dispose(pVWLetters);
}

VA(0x00433681, 0x253)
void advManager::VWInit(i32 centerX, i32 centerY) {
    if (giViewWorldScale == VIEW_WORLD_SCALE_FAR)
        giViewWorldScaleLookup = 0;
    else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE)
        giViewWorldScaleLookup = 1;
    else
        giViewWorldScaleLookup = 2;

    iVWViewableCells = WORLD_DRAW_SIZE / IDX(giViewWorldScale);
    if (MAP_WIDTH < iVWViewableCells)
        iVWViewableCells = MAP_WIDTH;
    iVWCenterOffset = iVWViewableCells >> 1;
    iVWMapOriginY = 0;
    iVWMapOriginX = iVWMapOriginY;
    if (MAP_WIDTH != iVWViewableCells) {
        iVWMapOriginX = centerX - (iVWViewableCells >> 1);
        if (iVWMapOriginX < 0)
            iVWMapOriginX = 0;
        if (MAP_WIDTH <= iVWMapOriginX + iVWViewableCells)
            iVWMapOriginX = MAP_WIDTH - iVWViewableCells;
        iVWMapOriginY = centerY - (iVWViewableCells >> 1);
        if (iVWMapOriginY < 0)
            iVWMapOriginY = 0;
        if (MAP_HEIGHT <= iVWMapOriginY + iVWViewableCells)
            iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
    }

    iVWXPixelOffset = (WORLD_WINDOW_X - IDX(giViewWorldScale) * IDX(iVWViewableCells)) >> 1;
    iVWYPixelOffset = iVWXPixelOffset;
    gpMouseManager->SetPointer("advmice.mse", WORLD_POINTER_FRAME, MOUSE_AUTO_CURSOR_TYPE);
    sprintf(gText, "ground%d.icn", giViewWorldScale);
    pVWGround = gpResourceManager->GetIcon(gText);
    sprintf(gText, "vwflag%d.icn", giViewWorldScale);
    pVWFlags = gpResourceManager->GetIcon(gText);
    sprintf(gText, "misc%d.icn", giViewWorldScale);
    pVWMisc = gpResourceManager->GetIcon(gText);
    sprintf(gText, "letter%d.icn", giViewWorldScale);
    pVWLetters = gpResourceManager->GetIcon(gText);
    UpdateRadar(1, 0);
}

VA(0x004338d4, 0x1346)
void advManager::VWCompleteDraw(void) {
    i32 mineHighlight18;
    i32 heroHighlight8;
    i32 artifactHighlight2;
    i32 townFlagHighlight3;
    i32 townIconHighlight1;
    i8 mineLetterY0[4];
    i8 townIconX2[4];
    i8 townFlagX15[4];
    i32 currentHeroHere;
    i32 pixelY2;
    i32 pixelX6;
    u8* pixel4;
    i8 drawTilesets0[WORLD_TILESET_COUNT];
    u8* rowEnd37;
    i32 unusedRowState14;
    i32 mapY3;
    mapCell* cell0;
    i32 mapX7;
    i32 playerColor26;
    u32 groundShape1;
    u32 groundFrame29;
    i32 resourceType9;
    u32 flipped5;
    i32 unusedExtraState29;
    mapCellExtra* extra15;

    groundFrame29 = 0;
    FillBitmapArea(
        gpWindowManager->m_screen,
        WORLD_LEFT,
        WORLD_TOP,
        WORLD_DRAW_SIZE,
        WORLD_DRAW_SIZE,
        WORLD_BACKGROUND_COLOR
    );
    memset(drawTilesets0, 1, WORLD_TILESET_COUNT);
    drawTilesets0[11] = 0;
    drawTilesets0[13] = 0;
    drawTilesets0[14] = 0;

    for (mapY3 = iVWMapOriginY; mapY3 < iVWMapOriginY + iVWViewableCells; mapY3++) {
        for (mapX7 = iVWMapOriginX; mapX7 < iVWMapOriginX + iVWViewableCells; mapX7++) {
            cell0 = GetCell(mapX7, mapY3);
            if ((giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) || iVWDrawAllTerrains
                || (iVWWhatToDraw == VIEW_WORLD_TOWNS
                    && (((cell0->m_triggerType & MAP_TRIGGER_TYPE_MASK) == MAP_OBJECT_CASTLE)
                        || (cell0->m_triggerType
                                == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                            && gpGame->m_heroRecs[cell0->m_objectMetadata].m_locationType
                                   == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))))) {
                flipped5 = 0;
                pixelX6 = (mapX7 - iVWMapOriginX) * IDX(giViewWorldScale) + iVWXPixelOffset;
                pixelY2 = (mapY3 - iVWMapOriginY) * IDX(giViewWorldScale) + iVWYPixelOffset;
                groundShape1 =
                    giGroundShape[cell0->m_terrainImageIndex] & WORLD_GROUND_SHAPE_MASK;
                switch (groundShape1) {
                    case 0:
                    case 9:
                    case 18:
                    case 19:
                    case 20:
                    case 21:
                        groundFrame29 = 0;
                        break;
                    case 1:
                    case 12:
                    case 15:
                    case 16:
                        groundFrame29 = 1;
                        break;
                    case 2:
                    case 10:
                    case 11:
                        groundFrame29 = 2;
                        break;
                    case 3:
                    case 13:
                    case 14:
                    case 17:
                        groundFrame29 = 3;
                        break;
                    case 4:
                        groundFrame29 = 4;
                        break;
                    case 5:
                        groundFrame29 = 5;
                        break;
                    case 6:
                        groundFrame29 = 6;
                        break;
                    case 7:
                        groundFrame29 = 7;
                        break;
                    case 8:
                        groundFrame29 = 8;
                        break;
                    default:
                        break;
                }
                if (cell0->m_flags & 2)
                    flipped5 = 1;
                if (cell0->m_flags & 1)
                    groundFrame29 += 9;
                if (groundFrame29 == 0)
                    groundFrame29 = (mapY3 * 2 + mapX7) & 3;
                else
                    groundFrame29 += 3;
                groundFrame29 +=
                    giGroundToTerrain[cell0->m_terrainImageIndex] * WORLD_TERRAIN_FRAME_STRIDE;
                if ((giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) || iVWDrawAllTerrains) {
                    if (flipped5 == 1)
                        townIconHighlight1 = IDX(giViewWorldScale) - 1;
                    else
                        townIconHighlight1 = 0;
                    pVWGround->DrawToBuffer(
                        pixelX6 + townIconHighlight1,
                        pixelY2,
                        groundFrame29,
                        flipped5
                    );
                }

                if ((cell0->m_objectData & 1) && cell0->m_objectIndex != WORLD_NO_SPRITE
                    && drawTilesets0[cell0->m_objectTileset]) {
                    IconToBitmapScale(
                        m_objectIcons[cell0->m_objectTileset],
                        gpWindowManager->m_screen,
                        pixelX6,
                        pixelY2,
                        cell0->m_objectIndex,
                        0,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        IDX(giViewWorldScale)
                    );
                }
                if (cell0->m_extraIndex == 0
                    || m_mapData->Extra(cell0->m_extraIndex)->objectIndex == WORLD_NO_SPRITE)
                    extra15 = NULL;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != NULL) {
                    if (extra15->objectLayerBit0 && extra15->objectIndex != WORLD_NO_SPRITE
                        && drawTilesets0[extra15->objectTileset]) {
                        IconToBitmapScale(
                            m_objectIcons[extra15->objectTileset],
                            gpWindowManager->m_screen,
                            pixelX6,
                            pixelY2,
                            extra15->objectIndex,
                            0,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            IDX(giViewWorldScale)
                        );
                    }
                    if (extra15->nextIndex == 0
                        || m_mapData->Extra(extra15->nextIndex)->objectIndex
                               == WORLD_NO_SPRITE)
                        extra15 = NULL;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }

                if (!(cell0->m_objectData & 1) && cell0->m_objectIndex != WORLD_NO_SPRITE
                    && drawTilesets0[cell0->m_objectTileset]) {
                    IconToBitmapScale(
                        m_objectIcons[cell0->m_objectTileset],
                        gpWindowManager->m_screen,
                        pixelX6,
                        pixelY2,
                        cell0->m_objectIndex,
                        0,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        IDX(giViewWorldScale)
                    );
                }
                if (cell0->m_extraIndex == 0
                    || m_mapData->Extra(cell0->m_extraIndex)->objectIndex == WORLD_NO_SPRITE)
                    extra15 = NULL;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != NULL) {
                    if (!extra15->objectLayerBit0 && extra15->objectIndex != WORLD_NO_SPRITE
                        && drawTilesets0[extra15->objectTileset]) {
                        IconToBitmapScale(
                            m_objectIcons[extra15->objectTileset],
                            gpWindowManager->m_screen,
                            pixelX6,
                            pixelY2,
                            extra15->objectIndex,
                            0,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            IDX(giViewWorldScale)
                        );
                    }
                    if (extra15->nextIndex == 0
                        || m_mapData->Extra(extra15->nextIndex)->objectIndex
                               == WORLD_NO_SPRITE)
                        extra15 = NULL;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }

                if (cell0->m_overlayIndex != WORLD_NO_SPRITE
                    && drawTilesets0[cell0->m_overlayTileset]) {
                    IconToBitmapScale(
                        m_objectIcons[cell0->m_overlayTileset],
                        gpWindowManager->m_screen,
                        pixelX6,
                        pixelY2,
                        cell0->m_overlayIndex,
                        0,
                        0,
                        0,
                        WORLD_SCREEN_WIDTH,
                        WORLD_SCREEN_HEIGHT,
                        IDX(giViewWorldScale)
                    );
                }
                if (cell0->m_extraIndex == 0
                    || m_mapData->Extra(cell0->m_extraIndex)->overlayIndex == WORLD_NO_SPRITE)
                    extra15 = NULL;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != NULL) {
                    if (drawTilesets0[extra15->overlayTileset]) {
                        IconToBitmapScale(
                            m_objectIcons[extra15->overlayTileset],
                            gpWindowManager->m_screen,
                            pixelX6,
                            pixelY2,
                            extra15->overlayIndex,
                            0,
                            0,
                            0,
                            WORLD_SCREEN_WIDTH,
                            WORLD_SCREEN_HEIGHT,
                            IDX(giViewWorldScale)
                        );
                    }
                    if (extra15->nextIndex == 0
                        || m_mapData->Extra(extra15->nextIndex)->overlayIndex
                               == WORLD_NO_SPRITE)
                        extra15 = NULL;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }
            }
        }
    }

    for (mapY3 = WORLD_TOP; mapY3 < WORLD_BOTTOM; mapY3++) {
        pixel4 = gpWindowManager->m_screen->m_pixels + mapY3 * WORLD_SCREEN_WIDTH;
        rowEnd37 = pixel4 + WORLD_RIGHT;
        for (pixel4 += WORLD_LEFT; pixel4 < rowEnd37; pixel4++)
            *pixel4 = gColorTableNoCycle[*pixel4];
    }

    for (mapY3 = iVWMapOriginY; mapY3 < iVWMapOriginY + iVWViewableCells; mapY3++) {
        for (mapX7 = iVWMapOriginX; mapX7 < iVWMapOriginX + iVWViewableCells; mapX7++) {
            cell0 = GetCell(mapX7, mapY3);
            pixelX6 = (mapX7 - iVWMapOriginX) * IDX(giViewWorldScale) + (IDX(giViewWorldScale) >> 1)
                      + iVWXPixelOffset;
            pixelY2 = (mapY3 - iVWMapOriginY) * IDX(giViewWorldScale) + (IDX(giViewWorldScale) >> 1)
                      + iVWYPixelOffset;

            if (cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ARTIFACT)
                && (iVWDrawAllObjs || (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7])
                    || iVWWhatToDraw == VIEW_WORLD_ARTIFACTS)) {
                artifactHighlight2 =
                    iVWWhatToDraw == VIEW_WORLD_ARTIFACTS ? WORLD_ARTIFACT_HIGHLIGHT : 0;
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    pixelX6 - iVWHalf[giViewWorldScaleLookup][2][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][2][1],
                    14,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    artifactHighlight2
                );
            }

            if ((cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)
                 || (cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                     && gpGame->m_heroRecs[cell0->m_objectMetadata].m_locationType
                            == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE)))
                && (iVWDrawAllObjs || (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7])
                    || iVWWhatToDraw == VIEW_WORLD_TOWNS)) {
                if (cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_CASTLE))
                    playerColor26 = gpGame->m_castleOwners[cell0->m_objectMetadata];
                else
                    playerColor26 =
                        gpGame->m_castleOwners[gpGame->m_heroRecs[cell0->m_objectMetadata]
                                                   .m_occupiedTown];
                if (playerColor26 < 0)
                    playerColor26 = WORLD_NO_OWNER_COLOR;
                else
                    playerColor26 = gpGame->m_players[playerColor26].m_color;
                townFlagX15[0] = -2;
                townFlagX15[1] = -3;
                townFlagX15[2] = -6;
                townIconX2[0] = 5;
                townIconX2[1] = 7;
                townIconX2[2] = 10;
                if (iVWWhatToDraw == VIEW_WORLD_TOWNS)
                    townFlagHighlight3 = playerColor26 + WORLD_HIGHLIGHT_BASE;
                else
                    townFlagHighlight3 = 0;
                FlipIconToBitmap(
                    pVWFlags,
                    gpWindowManager->m_screen,
                    townFlagX15[giViewWorldScaleLookup] + pixelX6
                        - iVWHalf[giViewWorldScaleLookup][4][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][4][1],
                    playerColor26,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    townFlagHighlight3
                );
                if (iVWWhatToDraw == VIEW_WORLD_TOWNS)
                    townIconHighlight1 = playerColor26 + WORLD_HIGHLIGHT_BASE;
                else
                    townIconHighlight1 = 0;
                IconToBitmap(
                    pVWFlags,
                    gpWindowManager->m_screen,
                    townIconX2[giViewWorldScaleLookup] + pixelX6
                        - iVWHalf[giViewWorldScaleLookup][4][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][4][1],
                    playerColor26,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    townIconHighlight1
                );
            }

            if (gpCurPlayer->m_currentHero == -1
                || gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_x != mapX7
                || gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_y != mapY3)
                currentHeroHere = 0;
            else
                currentHeroHere = 1;
            if ((cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION)
                 || currentHeroHere)
                && (iVWDrawAllObjs || (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7])
                    || iVWWhatToDraw == VIEW_WORLD_HEROES)) {
                if (!currentHeroHere)
                    playerColor26 = gpGame->m_availableHeroes[cell0->m_objectMetadata];
                else
                    playerColor26 = gpGame->m_availableHeroes[gpCurPlayer->m_currentHero];
                if (playerColor26 >= 0) {
                    groundFrame29 = gpGame->m_players[playerColor26].m_color;
                    if (iVWWhatToDraw == VIEW_WORLD_HEROES)
                        heroHighlight8 = groundFrame29 + WORLD_HIGHLIGHT_BASE;
                    else
                        heroHighlight8 = 0;
                    IconToBitmap(
                        pVWMisc,
                        gpWindowManager->m_screen,
                        pixelX6 - iVWHalf[giViewWorldScaleLookup][0][0],
                        pixelY2 - iVWHalf[giViewWorldScaleLookup][0][1],
                        groundFrame29 + 7,
                        1,
                        WORLD_LEFT,
                        WORLD_TOP,
                        WORLD_DRAW_SIZE,
                        WORLD_DRAW_SIZE,
                        heroHighlight8
                    );
                }
            }

            if (cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_RESOURCE)
                && (iVWDrawAllObjs || (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7])
                    || iVWWhatToDraw == VIEW_WORLD_RESOURCES)) {
                groundFrame29 = cell0->m_objectIndex >> 1;
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    pixelX6 - iVWHalf[giViewWorldScaleLookup][1][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][1][1],
                    13,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    iVWWhatToDraw == VIEW_WORLD_RESOURCES ? WORLD_RESOURCE_HIGHLIGHT : 0
                );
                IconToBitmap(
                    pVWLetters,
                    gpWindowManager->m_screen,
                    pixelX6 - iVWHalf[giViewWorldScaleLookup][5][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][5][1],
                    groundFrame29,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    0
                );
            }

            if ((cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_MINE)
                 || cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_SAWMILL)
                 || cell0->m_triggerType == (MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_ALCHEMIST_LAB))
                && (iVWDrawAllObjs || (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7])
                    || iVWWhatToDraw == VIEW_WORLD_MINES)) {
                playerColor26 = gpGame->m_mineOwners[cell0->m_objectMetadata];
                if (playerColor26 < 0)
                    groundFrame29 = WORLD_NO_OWNER_COLOR;
                else
                    groundFrame29 = gpGame->m_players[playerColor26].m_color;
                if (iVWWhatToDraw == VIEW_WORLD_MINES)
                    mineHighlight18 = groundFrame29 + WORLD_HIGHLIGHT_BASE;
                else
                    mineHighlight18 = 0;
                IconToBitmap(
                    pVWMisc,
                    gpWindowManager->m_screen,
                    pixelX6 - iVWHalf[giViewWorldScaleLookup][3][0],
                    pixelY2 - iVWHalf[giViewWorldScaleLookup][3][1],
                    groundFrame29,
                    1,
                    WORLD_LEFT,
                    WORLD_TOP,
                    WORLD_DRAW_SIZE,
                    WORLD_DRAW_SIZE,
                    mineHighlight18
                );
                mineLetterY0[0] = 0;
                mineLetterY0[1] = 0;
                mineLetterY0[2] = 2;
                resourceType9 = gpGame->m_mines[cell0->m_objectMetadata].resourceType;
                IconToBitmap(
                    pVWLetters,
                    gpWindowManager->m_screen,
                    pixelX6 - iVWHalf[giViewWorldScaleLookup][5][0],
                    mineLetterY0[giViewWorldScaleLookup] + pixelY2
                        - iVWHalf[giViewWorldScaleLookup][5][1],
                    resourceType9,
                    1,
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

VA(0x00434c1a, 0x5e2)
i32 ViewWorldDialogHandler(struct tag_message& message) {
    float radarScale6;
    i32 radarY1;
    i32 radarX0;
    tag_message savedMessage;
    tag_message currentMessage7;

    if (!gpSoundManager->MusicPlaying())
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);

    if (message.type == VIEW_WORLD_MESSAGE) {
        switch (message.payload.widget.command) {
            case VIEW_WORLD_SELECT:
                if (message.payload.widget.id == WORLD_RADAR_WIDGET
                    && (giViewWorldScale != VIEW_WORLD_SCALE_NEAR
                        || MAP_WIDTH > IDX(VIEW_WORLD_NEAR_MAX_MAP_SIZE))
                    && (giViewWorldScale != VIEW_WORLD_SCALE_MIDDLE
                        || MAP_WIDTH > IDX(VIEW_WORLD_MIDDLE_MAX_MAP_SIZE))) {
                    if (giViewWorldScale == VIEW_WORLD_SCALE_FAR
                        && MAP_WIDTH <= IDX(VIEW_WORLD_FAR_MAX_MAP_SIZE))
                        break;
                    switch (MAP_HEIGHT) {
                        case IDX(VIEW_WORLD_NEAR_MAX_MAP_SIZE):
                            radarScale6 = 4.0f;
                            break;
                        case IDX(VIEW_WORLD_MIDDLE_MAX_MAP_SIZE):
                            radarScale6 = 2.0f;
                            break;
                        case IDX(VIEW_WORLD_FAR_MAX_MAP_SIZE):
                            radarScale6 = 1.3333f;
                            break;
                        default:
                            radarScale6 = 1.0f;
                            break;
                    }

                    radarX0 = message.payload.mouse.screenX;
                    radarY1 = message.payload.mouse.screenY;
                    radarX0 = static_cast<i32>((radarX0 - WORLD_RADAR_LEFT) / radarScale6);
                    radarY1 = static_cast<i32>((radarY1 - WORLD_RADAR_TOP) / radarScale6);
                    iVWMapOriginX = radarX0 - iVWCenterOffset;
                    iVWMapOriginY = radarY1 - iVWCenterOffset;
                    if (iVWMapOriginX < 0)
                        iVWMapOriginX = 0;
                    if (iVWMapOriginY < 0)
                        iVWMapOriginY = 0;
                    if (MAP_WIDTH <= iVWMapOriginX + iVWViewableCells)
                        iVWMapOriginY = MAP_WIDTH - iVWViewableCells;
                    if (MAP_HEIGHT <= iVWMapOriginY + iVWViewableCells)
                        iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
                    gpAdvManager->UpdateRadar(1, 0);
                    gpAdvManager->VWCompleteDraw();

                    currentMessage7.type = MESSAGE_NONE;
                    while (currentMessage7.type != VIEW_WORLD_MOUSE_UP) {
                        Process1WindowsMessage();
                        currentMessage7 = gpInputManager->GetEvent();
                        savedMessage = currentMessage7;
                        while (currentMessage7.type != VIEW_WORLD_MOUSE_UP
                               && currentMessage7.type != MESSAGE_NONE) {
                            if (currentMessage7.type == VIEW_WORLD_MOUSE_MOVE)
                                savedMessage = currentMessage7;
                            Process1WindowsMessage();
                            currentMessage7 = gpInputManager->GetEvent();
                        }
                        if (savedMessage.type == VIEW_WORLD_MOUSE_MOVE) {
                            if (savedMessage.payload.mouse.x < WORLD_RADAR_LEFT)
                                savedMessage.payload.mouse.x = WORLD_RADAR_LEFT;
                            if (savedMessage.payload.mouse.x >= WORLD_RADAR_RIGHT)
                                savedMessage.payload.mouse.x =
                                    MAP_WIDTH * 2 + WORLD_RADAR_LEFT - 1;
                            if (savedMessage.payload.mouse.y < WORLD_RADAR_TOP)
                                savedMessage.payload.mouse.y = WORLD_RADAR_TOP;
                            if (savedMessage.payload.mouse.y >= WORLD_RADAR_BOTTOM)
                                savedMessage.payload.mouse.y =
                                    MAP_HEIGHT * 2 + WORLD_RADAR_TOP - 1;
                            gpMouseManager->Main(savedMessage);
                            radarX0 = static_cast<i32>(
                                (savedMessage.payload.mouse.x - WORLD_RADAR_LEFT) / radarScale6
                            );
                            radarY1 = static_cast<i32>(
                                (savedMessage.payload.mouse.y - WORLD_RADAR_TOP) / radarScale6
                            );
                            iVWMapOriginX = radarX0 - iVWCenterOffset;
                            iVWMapOriginY = radarY1 - iVWCenterOffset;
                            if (iVWMapOriginX < 0)
                                iVWMapOriginX = 0;
                            if (iVWMapOriginY < 0)
                                iVWMapOriginY = 0;
                            if (MAP_WIDTH <= iVWMapOriginX + iVWViewableCells)
                                iVWMapOriginX = MAP_WIDTH - iVWViewableCells;
                            if (MAP_HEIGHT <= iVWMapOriginY + iVWViewableCells)
                                iVWMapOriginY = MAP_HEIGHT - iVWViewableCells;
                            gpAdvManager->UpdateRadar(1, 0);
                            gpAdvManager->VWCompleteDraw();
                            savedMessage.type = MESSAGE_NONE;
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
                            iVWCenterOffset + iVWMapOriginX,
                            iVWCenterOffset + iVWMapOriginY
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
                        message.payload.widget.id = WORLD_DIALOG_CLOSE;
                        message.payload.widget.command =
                            BaseWidgetCommand(message.payload.widget.id);
                        return 2;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return 1;
}

DATA(0x004f11b8) i8 iVWHalf[3][6][2] = {3,  3, 5, 5, 6, 6, 8, 5, 2, 3, 2,  2,  4,  5, 6, 6, 8, 8,
                                        11, 7, 3, 4, 3, 3, 7, 7, 8, 8, 10, 10, 14, 8, 3, 6, 4, 4};
DATA(0x004f11dc) ViewWorldScale giViewWorldScale = VIEW_WORLD_SCALE_MIDDLE;
DATA(0x004f11e0) i32 giViewWorldScaleLookup = 1;
DATA(0x004f11e4) b32 gbInViewWorld = false;
DATA(0x00525110) class icon* pVWMisc;
DATA(0x00525114) class icon* pVWLetters;
DATA(0x00525118) i32 iVWYPixelOffset;
DATA(0x0052511c) class icon* pVWGround;
DATA(0x00525120) i32 iVWViewableCells;
DATA(0x00525124) class icon* pVWFlags;
DATA(0x00525128) i32 iVWDrawAllTerrains;
DATA(0x0052512c) ViewWorldMode iVWWhatToDraw;
DATA(0x00525130) i32 iVWDrawAllObjs;
DATA(0x00525134) i32 iVWMapOriginX;
DATA(0x00525138) i32 iVWMapOriginY;
DATA(0x0052513c) i32 iVWCenterOffset;
DATA(0x00525140) i32 iVWXPixelOffset;
