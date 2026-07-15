// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Viewwrld.obj   from: (directly linked into exe)
// functions: 5   data: 17
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
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

VA(0x004333c0, 0x267)
void advManager::ViewWorld(int whatToDraw, int drawAllObjects, int drawAllTerrains)
{
    int viewIndex;
    signed char savedPalette[VIEW_WORLD_PALETTE_SIZE];
    char *viewNames[7];
    tag_message message;
    heroWindow *viewWindow;

    memcpy(savedPalette, gpBufferPalette->m_data, VIEW_WORLD_PALETTE_SIZE);
    gbInViewWorld = 1;
    iVWWhatToDraw = whatToDraw;
    iVWDrawAllObjs = drawAllObjects;
    iVWDrawAllTerrains = drawAllTerrains;
    gpWindowManager->m_updateFlags = 0;
    giCycleType = 2;
    VWInit(m_mapOriginX + 7, m_mapOriginY + 7);
    VWCompleteDraw();
    gpWindowManager->m_updateFlags = 1;

    sprintf(gText, "viewlgnd.bin");
    viewWindow = new heroWindow(VIEW_WORLD_WINDOW_X, VIEW_WORLD_WINDOW_Y, gText);
    if (viewWindow == 0)
        MemError();

    viewNames[0] = "MINE";
    viewNames[1] = "RSRC";
    viewNames[2] = "RTFX";
    viewNames[3] = "TWNS";
    viewNames[4] = "HROS";
    viewNames[5] = "_ALL";
    viewNames[6] = "WRLD";
    if (whatToDraw == VIEW_WORLD_ALL && drawAllObjects == 0 && drawAllTerrains == 0)
        viewIndex = 6;
    else
        viewIndex = whatToDraw - VIEW_WORLD_MINES;
    sprintf(gText, "view%s.icn", viewNames[viewIndex]);
    message.type = VIEW_WORLD_MESSAGE;
    message.payload.widget.command = VIEW_WORLD_ICON_MESSAGE;
    message.payload.widget.id = VIEW_WORLD_ICON_WIDGET;
    message.payload.widget.data.text = gText;
    viewWindow->BroadcastMessage(message);
    gpWindowManager->DoDialog(viewWindow, ViewWorldDialogHandler, 0);
    delete viewWindow;

    UpdateRadar(1, 0);
    VWCleanup();
    gbInViewWorld = 0;
    gpWindowManager->m_updateFlags = 0;
    RedrawAdvScreen(1, 0);
    giCycleType = 0;
    gpWindowManager->m_updateFlags = 1;
    SetPalette(savedPalette, 1);
}

VA(0x00433627, 0x5a)
void advManager::VWCleanup(void)
{
    gpResourceManager->Dispose(pVWGround);
    gpResourceManager->Dispose(pVWFlags);
    gpResourceManager->Dispose(pVWMisc);
    gpResourceManager->Dispose(pVWLetters);
}

VA(0x00433681, 0x253)
void advManager::VWInit(int centerX, int centerY)
{
    if (giViewWorldScale == VIEW_WORLD_SCALE_FAR)
        giViewWorldScaleLookup = 0;
    else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE)
        giViewWorldScaleLookup = 1;
    else
        giViewWorldScaleLookup = 2;

    iVWViewableCells = VIEW_WORLD_DRAW_SIZE / giViewWorldScale;
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

    iVWXPixelOffset = (VIEW_WORLD_WINDOW_X - giViewWorldScale * iVWViewableCells) >> 1;
    iVWYPixelOffset = iVWXPixelOffset;
    gpMouseManager->SetPointer("advmice.mse", VIEW_WORLD_POINTER_FRAME,
                               VIEW_WORLD_POINTER_COLOR);
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
void advManager::VWCompleteDraw(void)
{
    int mineHighlight18;
    int heroHighlight8;
    int artifactHighlight2;
    int townFlagHighlight3;
    int townIconHighlight1;
    signed char mineLetterY0[4];
    signed char townIconX2[4];
    signed char townFlagX15[4];
    int currentHeroHere;
    int pixelY2;
    int pixelX6;
    unsigned char *pixel4;
    signed char drawTilesets0[VIEW_WORLD_TILESET_COUNT];
    unsigned char *rowEnd37;
    int mapY3;
    mapCell *cell0;
    int mapX7;
    int playerColor26;
    unsigned int groundShape1;
    unsigned int groundFrame29;
    int resourceType9;
    unsigned int flipped5;
    mapCellExtra *extra15;

    groundFrame29 = 0;
    FillBitmapArea(gpWindowManager->m_screen, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                   VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                   VIEW_WORLD_BACKGROUND_COLOR);
    memset(drawTilesets0, 1, VIEW_WORLD_TILESET_COUNT);
    drawTilesets0[11] = 0;
    drawTilesets0[13] = 0;
    drawTilesets0[14] = 0;

    for (mapY3 = iVWMapOriginY; mapY3 < iVWMapOriginY + iVWViewableCells; mapY3++) {
        for (mapX7 = iVWMapOriginX; mapX7 < iVWMapOriginX + iVWViewableCells; mapX7++) {
            cell0 = GetCell(mapX7, mapY3);
            if ((giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                iVWDrawAllTerrains ||
                (iVWWhatToDraw == VIEW_WORLD_TOWNS &&
                 (((cell0->m_triggerType & VIEW_WORLD_TRIGGER_TYPE_MASK) == MAP_EVENT_CASTLE) ||
                  (cell0->m_triggerType == VIEW_WORLD_TRIGGER_HERO &&
                   gpGame->m_heroRecs[cell0->m_objectMetadata].m_locationType ==
                       VIEW_WORLD_TRIGGER_TOWN)))) {
                flipped5 = 0;
                pixelX6 = (mapX7 - iVWMapOriginX) * giViewWorldScale + iVWXPixelOffset;
                pixelY2 = (mapY3 - iVWMapOriginY) * giViewWorldScale + iVWYPixelOffset;
                groundShape1 =
                    giGroundShape[cell0->m_terrainImageIndex] & VIEW_WORLD_GROUND_SHAPE_MASK;
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
                }
                if (cell0->m_flags & 2)
                    flipped5 = 1;
                if (cell0->m_flags & 1)
                    groundFrame29 += 9;
                if (groundFrame29 == 0)
                    groundFrame29 = (mapY3 * 2 + mapX7) & 3;
                else
                    groundFrame29 += 3;
                groundFrame29 += giGroundToTerrain[cell0->m_terrainImageIndex] *
                                 VIEW_WORLD_TERRAIN_FRAME_STRIDE;
                if ((giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                    iVWDrawAllTerrains) {
                    if (flipped5 == 1)
                        townIconHighlight1 = giViewWorldScale - 1;
                    else
                        townIconHighlight1 = 0;
                    pVWGround->DrawToBuffer(pixelX6 + townIconHighlight1, pixelY2,
                                            groundFrame29, flipped5);
                }

                if ((cell0->m_objectData & 1) && cell0->m_objectIndex != VIEW_WORLD_NO_SPRITE &&
                    drawTilesets0[cell0->m_objectTileset]) {
                    IconToBitmapScale(m_objectIcons[cell0->m_objectTileset], gpWindowManager->m_screen,
                                      pixelX6, pixelY2, cell0->m_objectIndex, 0, 0, 0,
                                      VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                      giViewWorldScale);
                }
                if (cell0->m_extraIndex == 0 ||
                    m_mapData->Extra(cell0->m_extraIndex)->objectIndex == VIEW_WORLD_NO_SPRITE)
                    extra15 = 0;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != 0) {
                    if (extra15->objectLayerBit0 && extra15->objectIndex != VIEW_WORLD_NO_SPRITE &&
                        drawTilesets0[extra15->objectTileset]) {
                        IconToBitmapScale(m_objectIcons[extra15->objectTileset],
                                          gpWindowManager->m_screen, pixelX6, pixelY2,
                                          extra15->objectIndex, 0, 0, 0,
                                          VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                          giViewWorldScale);
                    }
                    if (extra15->nextIndex == 0 ||
                        m_mapData->Extra(extra15->nextIndex)->objectIndex == VIEW_WORLD_NO_SPRITE)
                        extra15 = 0;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }

                if (!(cell0->m_objectData & 1) && cell0->m_objectIndex != VIEW_WORLD_NO_SPRITE &&
                    drawTilesets0[cell0->m_objectTileset]) {
                    IconToBitmapScale(m_objectIcons[cell0->m_objectTileset], gpWindowManager->m_screen,
                                      pixelX6, pixelY2, cell0->m_objectIndex, 0, 0, 0,
                                      VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                      giViewWorldScale);
                }
                if (cell0->m_extraIndex == 0 ||
                    m_mapData->Extra(cell0->m_extraIndex)->objectIndex == VIEW_WORLD_NO_SPRITE)
                    extra15 = 0;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != 0) {
                    if (!extra15->objectLayerBit0 && extra15->objectIndex != VIEW_WORLD_NO_SPRITE &&
                        drawTilesets0[extra15->objectTileset]) {
                        IconToBitmapScale(m_objectIcons[extra15->objectTileset],
                                          gpWindowManager->m_screen, pixelX6, pixelY2,
                                          extra15->objectIndex, 0, 0, 0,
                                          VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                          giViewWorldScale);
                    }
                    if (extra15->nextIndex == 0 ||
                        m_mapData->Extra(extra15->nextIndex)->objectIndex == VIEW_WORLD_NO_SPRITE)
                        extra15 = 0;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }

                if (cell0->m_overlayIndex != VIEW_WORLD_NO_SPRITE &&
                    drawTilesets0[cell0->m_overlayTileset]) {
                    IconToBitmapScale(m_objectIcons[cell0->m_overlayTileset], gpWindowManager->m_screen,
                                      pixelX6, pixelY2, cell0->m_overlayIndex, 0, 0, 0,
                                      VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                      giViewWorldScale);
                }
                if (cell0->m_extraIndex == 0 ||
                    m_mapData->Extra(cell0->m_extraIndex)->overlayIndex == VIEW_WORLD_NO_SPRITE)
                    extra15 = 0;
                else
                    extra15 = m_mapData->Extra(cell0->m_extraIndex);
                while (extra15 != 0) {
                    if (drawTilesets0[extra15->overlayTileset]) {
                        IconToBitmapScale(m_objectIcons[extra15->overlayTileset],
                                          gpWindowManager->m_screen, pixelX6, pixelY2,
                                          extra15->overlayIndex, 0, 0, 0,
                                          VIEW_WORLD_SCREEN_WIDTH, VIEW_WORLD_SCREEN_HEIGHT,
                                          giViewWorldScale);
                    }
                    if (extra15->nextIndex == 0 ||
                        m_mapData->Extra(extra15->nextIndex)->overlayIndex == VIEW_WORLD_NO_SPRITE)
                        extra15 = 0;
                    else
                        extra15 = m_mapData->Extra(extra15->nextIndex);
                }
            }
        }
    }

    for (mapY3 = VIEW_WORLD_TOP; mapY3 < VIEW_WORLD_BOTTOM; mapY3++) {
        pixel4 = gpWindowManager->m_screen->m_pixels + mapY3 * VIEW_WORLD_SCREEN_WIDTH;
        rowEnd37 = pixel4 + VIEW_WORLD_RIGHT;
        for (pixel4 += VIEW_WORLD_LEFT; pixel4 < rowEnd37; pixel4++)
            *pixel4 = gColorTableNoCycle[*pixel4];
    }

    for (mapY3 = iVWMapOriginY; mapY3 < iVWMapOriginY + iVWViewableCells; mapY3++) {
        for (mapX7 = iVWMapOriginX; mapX7 < iVWMapOriginX + iVWViewableCells; mapX7++) {
            cell0 = GetCell(mapX7, mapY3);
            pixelX6 = (mapX7 - iVWMapOriginX) * giViewWorldScale +
                     (giViewWorldScale >> 1) + iVWXPixelOffset;
            pixelY2 = (mapY3 - iVWMapOriginY) * giViewWorldScale +
                     (giViewWorldScale >> 1) + iVWYPixelOffset;

            if (cell0->m_triggerType == VIEW_WORLD_TRIGGER_ARTIFACT &&
                (iVWDrawAllObjs ||
                 (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                 iVWWhatToDraw == VIEW_WORLD_ARTIFACTS)) {
                artifactHighlight2 = iVWWhatToDraw == VIEW_WORLD_ARTIFACTS
                                        ? VIEW_WORLD_ARTIFACT_HIGHLIGHT : 0;
                IconToBitmap(pVWMisc, gpWindowManager->m_screen,
                             pixelX6 - iVWHalf[giViewWorldScaleLookup][2][0],
                             pixelY2 - iVWHalf[giViewWorldScaleLookup][2][1],
                             14, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                             artifactHighlight2);
            }

            if ((cell0->m_triggerType == VIEW_WORLD_TRIGGER_TOWN ||
                 (cell0->m_triggerType == VIEW_WORLD_TRIGGER_HERO &&
                  gpGame->m_heroRecs[cell0->m_objectMetadata].m_locationType ==
                      VIEW_WORLD_TRIGGER_TOWN)) &&
                (iVWDrawAllObjs ||
                 (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                 iVWWhatToDraw == VIEW_WORLD_TOWNS)) {
                if (cell0->m_triggerType == VIEW_WORLD_TRIGGER_TOWN)
                    playerColor26 = gpGame->m_castleOwners[cell0->m_objectMetadata];
                else
                    playerColor26 = gpGame->m_castleOwners[
                        gpGame->m_heroRecs[cell0->m_objectMetadata].m_occupiedTown];
                if (playerColor26 < 0)
                    playerColor26 = VIEW_WORLD_NO_OWNER_COLOR;
                else
                    playerColor26 = gpGame->m_players[playerColor26].m_color;
                townFlagX15[0] = -2;
                townFlagX15[1] = -3;
                townFlagX15[2] = -6;
                townIconX2[0] = 5;
                townIconX2[1] = 7;
                townIconX2[2] = 10;
                if (iVWWhatToDraw == VIEW_WORLD_TOWNS)
                    townFlagHighlight3 = playerColor26 + VIEW_WORLD_HIGHLIGHT_BASE;
                else
                    townFlagHighlight3 = 0;
                FlipIconToBitmap(pVWFlags, gpWindowManager->m_screen,
                                 townFlagX15[giViewWorldScaleLookup] + pixelX6 -
                                     iVWHalf[giViewWorldScaleLookup][4][0],
                                 pixelY2 - iVWHalf[giViewWorldScaleLookup][4][1],
                                 playerColor26, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                                 VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                                 townFlagHighlight3);
                if (iVWWhatToDraw == VIEW_WORLD_TOWNS)
                    townIconHighlight1 = playerColor26 + VIEW_WORLD_HIGHLIGHT_BASE;
                else
                    townIconHighlight1 = 0;
                IconToBitmap(pVWFlags, gpWindowManager->m_screen,
                             townIconX2[giViewWorldScaleLookup] + pixelX6 -
                                 iVWHalf[giViewWorldScaleLookup][4][0],
                             pixelY2 - iVWHalf[giViewWorldScaleLookup][4][1],
                             playerColor26, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                             townIconHighlight1);
            }

            if (gpCurPlayer->m_currentHero == -1 ||
                gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_x != mapX7 ||
                gpGame->m_heroRecs[gpCurPlayer->m_currentHero].m_y != mapY3)
                currentHeroHere = 0;
            else
                currentHeroHere = 1;
            if ((cell0->m_triggerType == VIEW_WORLD_TRIGGER_HERO || currentHeroHere) &&
                (iVWDrawAllObjs ||
                 (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                 iVWWhatToDraw == VIEW_WORLD_HEROES)) {
                if (!currentHeroHere)
                    playerColor26 = gpGame->m_availableHeroes[cell0->m_objectMetadata];
                else
                    playerColor26 = gpGame->m_availableHeroes[gpCurPlayer->m_currentHero];
                if (playerColor26 >= 0) {
                    groundFrame29 = gpGame->m_players[playerColor26].m_color;
                    if (iVWWhatToDraw == VIEW_WORLD_HEROES)
                        heroHighlight8 = groundFrame29 + VIEW_WORLD_HIGHLIGHT_BASE;
                    else
                        heroHighlight8 = 0;
                    IconToBitmap(pVWMisc, gpWindowManager->m_screen,
                                 pixelX6 - iVWHalf[giViewWorldScaleLookup][0][0],
                                 pixelY2 - iVWHalf[giViewWorldScaleLookup][0][1],
                                 groundFrame29 + 7, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                                 VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                                 heroHighlight8);
                }
            }

            if (cell0->m_triggerType == VIEW_WORLD_TRIGGER_RESOURCE &&
                (iVWDrawAllObjs ||
                 (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                 iVWWhatToDraw == VIEW_WORLD_RESOURCES)) {
                groundFrame29 = cell0->m_objectIndex >> 1;
                IconToBitmap(pVWMisc, gpWindowManager->m_screen,
                             pixelX6 - iVWHalf[giViewWorldScaleLookup][1][0],
                             pixelY2 - iVWHalf[giViewWorldScaleLookup][1][1],
                             13, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                             iVWWhatToDraw == VIEW_WORLD_RESOURCES
                                 ? VIEW_WORLD_RESOURCE_HIGHLIGHT : 0);
                IconToBitmap(pVWLetters, gpWindowManager->m_screen,
                             pixelX6 - iVWHalf[giViewWorldScaleLookup][5][0],
                             pixelY2 - iVWHalf[giViewWorldScaleLookup][5][1],
                             groundFrame29, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE, 0);
            }

            if ((cell0->m_triggerType == VIEW_WORLD_TRIGGER_MINE ||
                 cell0->m_triggerType == VIEW_WORLD_TRIGGER_SAWMILL ||
                 cell0->m_triggerType == VIEW_WORLD_TRIGGER_ALCHEMIST_LAB) &&
                (iVWDrawAllObjs ||
                 (giCurPlayerBit & mapExtra[MAP_WIDTH * mapY3 + mapX7]) ||
                 iVWWhatToDraw == VIEW_WORLD_MINES)) {
                playerColor26 = gpGame->m_mineOwners[cell0->m_objectMetadata];
                if (playerColor26 < 0)
                    groundFrame29 = VIEW_WORLD_NO_OWNER_COLOR;
                else
                    groundFrame29 = gpGame->m_players[playerColor26].m_color;
                if (iVWWhatToDraw == VIEW_WORLD_MINES)
                    mineHighlight18 = groundFrame29 + VIEW_WORLD_HIGHLIGHT_BASE;
                else
                    mineHighlight18 = 0;
                IconToBitmap(pVWMisc, gpWindowManager->m_screen,
                             pixelX6 - iVWHalf[giViewWorldScaleLookup][3][0],
                             pixelY2 - iVWHalf[giViewWorldScaleLookup][3][1],
                             groundFrame29, 1, VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE,
                             mineHighlight18);
                mineLetterY0[0] = 0;
                mineLetterY0[1] = 0;
                mineLetterY0[2] = 2;
                resourceType9 = gpGame->m_mines[cell0->m_objectMetadata].resourceType;
                IconToBitmap(pVWLetters, gpWindowManager->m_screen,
                             pixelX6 - iVWHalf[giViewWorldScaleLookup][5][0],
                             mineLetterY0[giViewWorldScaleLookup] + pixelY2 -
                                 iVWHalf[giViewWorldScaleLookup][5][1],
                             resourceType9, 1,
                             VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                             VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE, 0);
            }
        }
    }
    gpWindowManager->UpdateScreenRegion(VIEW_WORLD_LEFT, VIEW_WORLD_TOP,
                                        VIEW_WORLD_DRAW_SIZE, VIEW_WORLD_DRAW_SIZE);
}

VA(0x00434c1a, 0x5e2)
int ViewWorldDialogHandler(struct tag_message &message)
{
    float radarScale;
    int radarY;
    int radarX;
    tag_message savedMessage;
    tag_message currentMessage;

    if (!gpSoundManager->MusicPlaying())
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);

    if (message.type == VIEW_WORLD_MESSAGE) {
        switch (message.payload.widget.command) {
        case VIEW_WORLD_SELECT:
            if (message.payload.widget.id == VIEW_WORLD_RADAR_WIDGET &&
                (giViewWorldScale != VIEW_WORLD_SCALE_NEAR ||
                 MAP_WIDTH > VIEW_WORLD_NEAR_MAX_MAP_SIZE) &&
                (giViewWorldScale != VIEW_WORLD_SCALE_MIDDLE ||
                 MAP_WIDTH > VIEW_WORLD_MIDDLE_MAX_MAP_SIZE)) {
                if (giViewWorldScale == VIEW_WORLD_SCALE_FAR &&
                    MAP_WIDTH <= VIEW_WORLD_FAR_MAX_MAP_SIZE)
                    break;
                switch (MAP_HEIGHT) {
                case VIEW_WORLD_NEAR_MAX_MAP_SIZE:
                    radarScale = 4.0f;
                    break;
                case VIEW_WORLD_MIDDLE_MAX_MAP_SIZE:
                    radarScale = 2.0f;
                    break;
                case VIEW_WORLD_FAR_MAX_MAP_SIZE:
                    radarScale = 1.3333f;
                    break;
                default:
                    radarScale = 1.0f;
                }

                radarX = message.payload.mouse.screenX;
                radarY = message.payload.mouse.screenY;
                radarX = static_cast<int>((radarX - VIEW_WORLD_RADAR_LEFT) / radarScale);
                radarY = static_cast<int>((radarY - VIEW_WORLD_RADAR_TOP) / radarScale);
                iVWMapOriginX = radarX - iVWCenterOffset;
                iVWMapOriginY = radarY - iVWCenterOffset;
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

                currentMessage.type = 0;
                while (currentMessage.type != VIEW_WORLD_MOUSE_UP) {
                    Process1WindowsMessage();
                    currentMessage = gpInputManager->GetEvent();
                    savedMessage = currentMessage;
                    while (currentMessage.type != VIEW_WORLD_MOUSE_UP &&
                           currentMessage.type != 0) {
                        if (currentMessage.type == VIEW_WORLD_MOUSE_MOVE)
                            savedMessage = currentMessage;
                        Process1WindowsMessage();
                        currentMessage = gpInputManager->GetEvent();
                    }
                    if (savedMessage.type == VIEW_WORLD_MOUSE_MOVE) {
                        if (savedMessage.payload.mouse.x < VIEW_WORLD_RADAR_LEFT)
                            savedMessage.payload.mouse.x = VIEW_WORLD_RADAR_LEFT;
                        if (savedMessage.payload.mouse.x >= VIEW_WORLD_RADAR_RIGHT)
                            savedMessage.payload.mouse.x = MAP_WIDTH * 2 + VIEW_WORLD_RADAR_LEFT - 1;
                        if (savedMessage.payload.mouse.y < VIEW_WORLD_RADAR_TOP)
                            savedMessage.payload.mouse.y = VIEW_WORLD_RADAR_TOP;
                        if (savedMessage.payload.mouse.y >= VIEW_WORLD_RADAR_BOTTOM)
                            savedMessage.payload.mouse.y = MAP_HEIGHT * 2 + VIEW_WORLD_RADAR_TOP - 1;
                        gpMouseManager->Main(savedMessage);
                        radarX = static_cast<int>(
                            (savedMessage.payload.mouse.x - VIEW_WORLD_RADAR_LEFT) / radarScale);
                        radarY = static_cast<int>(
                            (savedMessage.payload.mouse.y - VIEW_WORLD_RADAR_TOP) / radarScale);
                        iVWMapOriginX = radarX - iVWCenterOffset;
                        iVWMapOriginY = radarY - iVWCenterOffset;
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
                        savedMessage.type = 0;
                    }
                }
            }
            break;
        case VIEW_WORLD_RELEASE:
            switch (message.payload.widget.id) {
            case VIEW_WORLD_SCALE_CONTROL:
                gpAdvManager->VWCleanup();
                if (giViewWorldScale == VIEW_WORLD_SCALE_NEAR)
                    giViewWorldScale = VIEW_WORLD_SCALE_MIDDLE;
                else if (giViewWorldScale == VIEW_WORLD_SCALE_MIDDLE)
                    giViewWorldScale = VIEW_WORLD_SCALE_FAR;
                else
                    giViewWorldScale = VIEW_WORLD_SCALE_NEAR;
                gpAdvManager->VWInit(iVWCenterOffset + iVWMapOriginX,
                                     iVWCenterOffset + iVWMapOriginY);
                gpAdvManager->VWCompleteDraw();
                break;
            case EVENT_WINDOW_FIRST_BUTTON:
            case EVENT_WINDOW_SECOND_BUTTON:
            case EVENT_WINDOW_THIRD_BUTTON:
            case EVENT_WINDOW_FOURTH_BUTTON:
            case EVENT_WINDOW_FIFTH_BUTTON:
            case EVENT_WINDOW_SIXTH_BUTTON:
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                message.payload.widget.id = VIEW_WORLD_DIALOG_CLOSE;
                message.payload.widget.command = message.payload.widget.id;
                return 2;
            }
            break;
        }
    }
    return 1;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004f11b8) signed char iVWHalf[3][6][2];
DATA(0x004f11dc) int giViewWorldScale = 6;
DATA(0x004f11e0) int giViewWorldScaleLookup = 1;
DATA(0x004f11e4) int gbInViewWorld = 0;
DATA(0x00525110) class icon *pVWMisc;
DATA(0x00525114) class icon *pVWLetters;
DATA(0x00525118) int iVWYPixelOffset;
DATA(0x0052511c) class icon *pVWGround;
DATA(0x00525120) int iVWViewableCells;
DATA(0x00525124) class icon *pVWFlags;
DATA(0x00525128) int iVWDrawAllTerrains;
DATA(0x0052512c) int iVWWhatToDraw;
DATA(0x00525130) int iVWDrawAllObjs;
DATA(0x00525134) int iVWMapOriginX;
DATA(0x00525138) int iVWMapOriginY;
DATA(0x0052513c) int iVWCenterOffset;
DATA(0x00525140) int iVWXPixelOffset;
