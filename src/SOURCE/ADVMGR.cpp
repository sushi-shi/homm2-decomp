// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ADVMGR.OBJ   from: (directly linked into exe)
// functions: 92   data: 33
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
#include <_globals_model.h>
#include <BASE/icon.h>
#include <BASE/font.h>
#include <BASE/bitmap.h>
#include <BASE/executive.h>
#include <BASE/BITS.h>
#include <BASE/iconWidget.h>
#include <BASE/textWidget.h>
#include <BASE/Icon2b.h>
#include <BASE/icon2bc.h>
#include <BASE/Iconf2b.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <BASE/soundManager.h>
#include <BASE/tileset.h>
#include <BASE/TILE.h>
#include <BASE/WINMGR.h>
#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/KB.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/HERO.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/FINDPATH.h>
#include <SOURCE/GAME.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/NOOPT.h>
#include <SOURCE/philAI.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ADVMGR.h>
#include <SOURCE/game.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/searchArray.h>
#include <SOURCE/town.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADVMGR_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\ADVMGR.CPP"
#define ADVMGR_QUICK_VIEW_LINE (*reinterpret_cast<const short *>("\x76\x21"))
#define ADVMGR_TOWN_VIEW_LINE (*reinterpret_cast<const short *>("\x5f\x5e"))
#define ADVMGR_BOTTOM_HERO_LINE (*reinterpret_cast<const short *>("\x5f\x21"))
#define ADVMGR_NEW_TURN_LINE (*reinterpret_cast<const short *>("\xd0\x11"))
#define ADVMGR_RESOURCE_VIEW_LINE (*reinterpret_cast<const short *>("\x2f\x12"))
#define ADVMGR_KINGDOM_VIEW_LINE (*reinterpret_cast<const short *>("\x96\x12"))
#define ADVMGR_BORDER_FREE_LINE (*reinterpret_cast<const short *>("\x24\x01"))
#define ADVMGR_ENVIRONMENT_VOLUME(distance)                                \
    (reinterpret_cast<const int *>("\x40\0\0\0\x39\0\0\0\x28\0\0\0" \
                                   "\x15\0\0\0\x07\0\0\0\x05\0\0\0") \
         [distance])

DATA(0x00527eb8) static unsigned short s_drawGroundTile;
DATA(0x00527ebc) static int s_adjacentMonsterX;
DATA(0x00527ec0) static int s_drawPixelY;
DATA(0x00527ec4) static mapCellExtra *s_drawExtra;
DATA(0x00527ecc) static int s_adjacentMonsterY;
DATA(0x00527ed4) static int s_adjacentMonsterMinX;
DATA(0x00527ed8) static int s_adjacentMonsterMinY;
DATA(0x00527ee4) static int s_drawMonsterFrame;
DATA(0x00527f0c) static int s_drawCloudFrame;
DATA(0x00527f10) static int s_drawStoneTile;
DATA(0x00527f18) static mapCell *s_drawCell;
DATA(0x00527f1c) static mineRecord *s_drawMine;
DATA(0x00527f20) static hero *s_drawHero;
DATA(0x00527f3c) static int s_drawHeroType;
DATA(0x00527f44) static int s_drawCovered;
DATA(0x00527f48) static mapCell *s_drawAdjacentCell;
DATA(0x00528094) static int s_drawHasHero;
DATA(0x00528098) static int s_adjacentMonsterEndY;
DATA(0x0052809c) static int s_drawFlipCloud;
DATA(0x005280a0) static int s_drawHeroFrame;
DATA(0x005280a4) static int s_drawPixelX;
DATA(0x005280a8) static int s_drawPlayerColor;
DATA(0x005280cc) static int s_adjacentMonsterEndX;
DATA(0x005280d0) static int s_drawAnimationLength;
DATA(0x005280d8) static int s_drawHeroYOffset;
VA(0x00456350, 0x30f)
advManager::advManager(void)
{
    m_groundTiles = 0;
    m_puzzleIcon = 0;
    m_mapOriginX = 0;
    m_mapOriginY = 0;
    m_updateMinX = 0;
    m_updateMinY = 0;
    m_updateMaxX = 0;
    m_updateMaxY = 0;
    m_updatePending = 0;
    m_selectedCell = ADVMGR_INVALID_CELL;
    m_cursorActive = 0;
    m_identifyHeroActive = 0;
    m_drawHeroShadows = 1;
    m_adventureBorder = 0;

    int index;
    for (index = 0; index < ADVMGR_OBJECT_ICON_COUNT; ++index)
        m_objectIcons[index] = 0;
    for (index = 0; index < ADVMGR_HERO_ICON_COUNT; ++index)
        m_heroIcons[index] = 0;
    for (index = 0; index < ADVMGR_PLAYER_COLOR_COUNT; ++index) {
        m_flagIcons[index] = 0;
        m_boatFlagIcons[index] = 0;
    }
    for (index = 0; index < ADVMGR_LOOPING_SAMPLE_COUNT; ++index)
        m_loopingSamples[index] = 0;
    for (index = 0; index < ADVMGR_CURSOR_SAMPLE_COUNT; ++index)
        m_cursorSamples[index] = 0;

    m_puzzleIcon = 0;
    m_cloudOverlayIcon = 0;
    m_boatShadowIcon = 0;
    m_shadowIcon = 0;
    m_groundTiles = 0;
    m_cloudTiles = 0;
    m_stoneTiles = 0;
    m_adventureWindow = 0;
    m_visibilityMap = 0;
    m_heroContextLocked = 0;
    m_townContextLocked = 0;
    bShowIt = 1;
    m_lastQuickViewX = ADVMGR_QUICK_VIEW_NONE;
    m_lastQuickViewY = ADVMGR_QUICK_VIEW_NONE;
    m_viewBounds[0] = 0;
    m_viewBounds[1] = 4;
    m_viewBounds[2] = 11;
    m_viewBounds[3] = 13;
    m_mapData = gpGame->GetWorldMapData();
    gMapX = 0;
    gMapY = 0;
    m_cursorFrameCount = 0;
    m_cursorCycle = 0;
    m_cursorTurning = 0;
}

// @early-stop
// Raw instructions and relocation sites are byte-identical; only delinked string/global relocation symbol names differ.
VA(0x0045665f, 0x9c9)
int advManager::Open(int id)
{
    iCurBottomView = 0;
    m_openState = 0;
    bShowIt = 0;
    m_adventureBorder = 0;

    int index;
    for (index = 0; index < 12; ++index) {
        m_heroLocatorState[index] = 0;
        m_townLocatorState[index] = 0;
    }

    if (m_adventureWindow == 0) {
        m_adventureWindow = new heroWindow(0, 0, "adv_wind.bin");
        if (m_adventureWindow == 0)
            MemError();
        m_scrollLeftButton = new iconWidget(ADVMGR_SCROLL_LEFT_X, ADVMGR_SCROLL_Y,
                                            ADVMGR_SCROLL_WIDTH, ADVMGR_SCROLL_HEIGHT,
                                            "scroll.icn", 4, 0, ADVMGR_SCROLL_LEFT_FRAME,
                                            ADVMGR_SCROLL_HOTKEY, 1);
        if (m_scrollLeftButton == 0)
            MemError();
        m_adventureWindow->AddWidget(m_scrollLeftButton, -1);
        m_scrollRightButton = new iconWidget(ADVMGR_SCROLL_RIGHT_X, ADVMGR_SCROLL_Y,
                                             ADVMGR_SCROLL_WIDTH, ADVMGR_SCROLL_HEIGHT,
                                             "scroll.icn", 4, 0, ADVMGR_SCROLL_RIGHT_FRAME,
                                             ADVMGR_SCROLL_HOTKEY, 1);
        if (m_scrollRightButton == 0)
            MemError();
        m_adventureWindow->AddWidget(m_scrollRightButton, -1);
    }

    if (gbThisNetHumanPlayer[giCurPlayer])
        gpMouseManager->SetPointer("advmice.mse", 0, ADVMGR_DEFAULT_POINTER_FRAME);
    else
        gpMouseManager->SetPointer("advmice.mse", 1, ADVMGR_DEFAULT_POINTER_FRAME);

    if (m_visibilityMap == 0) {
        m_visibilityMap = new unsigned short[MAP_WIDTH * MAP_HEIGHT];
        if (m_visibilityMap == 0)
            MemError();
    }
    m_visibilityMapValid = 0;
    gpWindowManager->AddWindow(m_adventureWindow, 0, 1);

    if (m_groundTiles == 0)
        m_groundTiles = gpResourceManager->GetTileset("ground32.til");
    if (m_cloudTiles == 0)
        m_cloudTiles = gpResourceManager->GetTileset("clof32.til");
    if (m_stoneTiles == 0)
        m_stoneTiles = gpResourceManager->GetTileset("ston.til");
    if (m_cloudOverlayIcon == 0)
        m_cloudOverlayIcon = gpResourceManager->GetIcon("clop32.icn");

    for (index = 0; index < ADVMGR_OBJECT_ICON_COUNT; ++index) {
        if (strlen(gTilesetFiles[index]) > 1 && m_objectIcons[index] == 0 &&
            index != ADVMGR_UNUSED_OBJECT_ICON_1 && index != ADVMGR_UNUSED_OBJECT_ICON_2)
            m_objectIcons[index] = gpResourceManager->GetIcon(gTilesetFiles[index]);
    }

    if (m_heroIcons[0] == 0)
        m_heroIcons[0] = gpResourceManager->GetIcon("kngt32.icn");
    if (m_heroIcons[1] == 0)
        m_heroIcons[1] = gpResourceManager->GetIcon("barb32.icn");
    if (m_heroIcons[2] == 0)
        m_heroIcons[2] = gpResourceManager->GetIcon("sorc32.icn");
    if (m_heroIcons[3] == 0)
        m_heroIcons[3] = gpResourceManager->GetIcon("wrlk32.icn");
    if (m_heroIcons[4] == 0)
        m_heroIcons[4] = gpResourceManager->GetIcon("wzrd32.icn");
    if (m_heroIcons[5] == 0)
        m_heroIcons[5] = gpResourceManager->GetIcon("necr32.icn");
    if (m_heroIcons[6] == 0)
        m_heroIcons[6] = gpResourceManager->GetIcon("boat32.icn");
    if (m_heroIcons[7] == 0)
        m_heroIcons[7] = gpResourceManager->GetIcon("froth.icn");

    gbLoadingMonoIcon = 1;
    if (m_shadowIcon == 0)
        m_shadowIcon = gpResourceManager->GetIcon("shadow32.icn");
    if (m_boatShadowIcon == 0)
        m_boatShadowIcon = gpResourceManager->GetIcon("boatshad.icn");
    gbLoadingMonoIcon = 0;

    if (m_flagIcons[0] == 0)
        m_flagIcons[0] = gpResourceManager->GetIcon("b-flag32.icn");
    if (m_flagIcons[1] == 0)
        m_flagIcons[1] = gpResourceManager->GetIcon("g-flag32.icn");
    if (m_flagIcons[2] == 0)
        m_flagIcons[2] = gpResourceManager->GetIcon("r-flag32.icn");
    if (m_flagIcons[3] == 0)
        m_flagIcons[3] = gpResourceManager->GetIcon("y-flag32.icn");
    if (m_flagIcons[4] == 0)
        m_flagIcons[4] = gpResourceManager->GetIcon("o-flag32.icn");
    if (m_flagIcons[5] == 0)
        m_flagIcons[5] = gpResourceManager->GetIcon("p-flag32.icn");
    if (m_boatFlagIcons[0] == 0)
        m_boatFlagIcons[0] = gpResourceManager->GetIcon("b-bflg32.icn");
    if (m_boatFlagIcons[1] == 0)
        m_boatFlagIcons[1] = gpResourceManager->GetIcon("g-bflg32.icn");
    if (m_boatFlagIcons[2] == 0)
        m_boatFlagIcons[2] = gpResourceManager->GetIcon("r-bflg32.icn");
    if (m_boatFlagIcons[3] == 0)
        m_boatFlagIcons[3] = gpResourceManager->GetIcon("y-bflg32.icn");
    if (m_boatFlagIcons[4] == 0)
        m_boatFlagIcons[4] = gpResourceManager->GetIcon("o-bflg32.icn");
    if (m_boatFlagIcons[5] == 0)
        m_boatFlagIcons[5] = gpResourceManager->GetIcon("p-bflg32.icn");

    gbLoadingMonoIcon = 1;
    if (m_puzzleIcon == 0)
        m_puzzleIcon = gpResourceManager->GetIcon("radar.icn");
    gbLoadingMonoIcon = 0;

    for (index = 0; index < ADVMGR_LOOPING_SAMPLE_COUNT; ++index)
        m_loopingSamples[index] = 0;
    for (index = 0; index < ADVMGR_SOUND_CELL_COUNT; ++index) {
        m_activeSounds[index].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
        m_activeSounds[index].volume = ADVMGR_ENVIRONMENT_SOUND_DEFAULT_VOLUME;
        m_activeSoundMask = 0;
    }

    GetCursorSampleSet(gConfig.walkSpeed);
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }

    glTimers[0] = KBTickCount() + ADVMGR_TIMER_DELAY;
    int oldSampleVolume = gConfig.soundVolume;
    if (gConfig.soundVolume != 0)
        gConfig.soundVolume = 10;
    SetInitialMapOrigin();

    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    int oldPlayer = giCurPlayer;
    int oldShowIt = bShowIt;
    giCurPlayer = giCurWatchPlayer;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    bShowIt = 1;
    RedrawAdvScreen(1, 0);
    giCurPlayer = oldPlayer;
    bShowIt = oldShowIt;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    if (!gbThisNetHumanPlayer[giCurPlayer])
        gpGame->ShowComputerScreen();
    KBChangeMenu(hmnuAdv);
    ForceNewHover();
    gpWindowManager->FadeScreen(0, 8, gPalette);
    giBottomViewOverride = 0;
    gConfig.soundVolume = oldSampleVolume;
    gpSoundManager->AdjustSoundVolumes();
    m_messageMask = ADVMGR_MANAGER_MESSAGE;
    m_priority = id;
    m_active = 1;
    strcpy(m_name, "advManager");
    return 0;
}

VA(0x00457028, 0x40a)
void advManager::Close(void)
{
    ClearBottomView();
    gpMouseManager->SetPointer(-1);
    if (!bEnteringTown || gConfig.useOpera || gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
        gpSoundManager->SwitchAmbientMusic(-1);
        gpSoundManager->StopAllSamples(1);
    } else {
        gpSoundManager->StopAllSamples(0);
    }
    if (m_adventureBorder != 0) {
        BaseFree(m_adventureBorder, ADVMGR_SOURCE_FILE,
                 ADVMGR_BORDER_FREE_LINE + ADVMGR_BORDER_FREE_LINE_OFFSET);
        m_adventureBorder = 0;
    }

    int index;
    if (gAdvDisposeLevel <= 1) {
        for (index = 0; index < ADVMGR_OBJECT_ICON_COUNT; ++index) {
            if (m_objectIcons[index] != 0)
                gpResourceManager->Dispose(m_objectIcons[index]);
            m_objectIcons[index] = 0;
        }
    }
    if (gAdvDisposeLevel <= 0) {
        gpResourceManager->Dispose(m_puzzleIcon);
        m_puzzleIcon = 0;
        gpResourceManager->Dispose(m_cloudOverlayIcon);
        m_cloudOverlayIcon = 0;
        for (index = 0; index < ADVMGR_HERO_ICON_COUNT; ++index) {
            gpResourceManager->Dispose(m_heroIcons[index]);
            m_heroIcons[index] = 0;
        }
        gpResourceManager->Dispose(m_shadowIcon);
        m_shadowIcon = 0;
        gpResourceManager->Dispose(m_boatShadowIcon);
        m_boatShadowIcon = 0;
        for (index = 0; index < ADVMGR_PLAYER_COLOR_COUNT; ++index) {
            gpResourceManager->Dispose(m_flagIcons[index]);
            m_flagIcons[index] = 0;
            gpResourceManager->Dispose(m_boatFlagIcons[index]);
            m_boatFlagIcons[index] = 0;
        }
        gpResourceManager->Dispose(m_groundTiles);
        m_groundTiles = 0;
        gpResourceManager->Dispose(m_cloudTiles);
        m_cloudTiles = 0;
        gpResourceManager->Dispose(m_stoneTiles);
        m_stoneTiles = 0;
    }
    for (index = 0; index < ADVMGR_LOOPING_SAMPLE_COUNT; ++index) {
        if (m_loopingSamples[index] != 0)
            gpResourceManager->Dispose(m_loopingSamples[index]);
        m_loopingSamples[index] = 0;
    }
    for (index = 0; index < ADVMGR_CURSOR_SAMPLE_COUNT; ++index) {
        gpResourceManager->Dispose(m_cursorSamples[index]);
        m_cursorSamples[index] = 0;
    }
    gpWindowManager->RemoveWindow(m_adventureWindow);
    delete m_adventureWindow;
    m_adventureWindow = 0;
    if (m_visibilityMap != 0)
        delete m_visibilityMap;
    m_visibilityMap = 0;
    iCurBottomView = 0;
    m_active = 0;
}

VA(0x00457432, 0xe9)
void advManager::GetCursorSampleSet(int sampleSet)
{
    if (sampleSet >= 1)
        sampleSet = 2;
    int sampleSuffix[ADVMGR_CURSOR_SAMPLE_COUNT] = { 0, 3, 5, 3, 4, 5, 6, 3, 3 };
    for (int index = 0; index < ADVMGR_CURSOR_SAMPLE_COUNT; ++index) {
        sprintf(gText, "wsnd%1d%1d.82M", sampleSet, sampleSuffix[index]);
        m_cursorSamples[index] = gpResourceManager->GetSample(gText);
        m_cursorSamples[index]->m_volume = ADVMGR_CURSOR_SAMPLE_VOLUME;
        m_cursorSamples[index]->m_channelType = ADVMGR_CURSOR_SAMPLE_CHANNEL;
    }
}

// @match-note
// Excluding the 0x24 pointer table, all 429 executable instructions, the 0x80
// frame/slots, CFG, and all 87 relocation targets agree. Retail's three
// const_00128d34 references are gConfig + 0x14 (showRoute). The only code
// residuals are the commutative X-coordinate load orders at retail +0x5a6
// and +0x5ef: retail loads m_lastHoverCell before adding m_mapOriginX.
// Both source operand orders and all 31 syntax-aware AST variants were tried.
// Revisit after a material TU-state change or in the global last-mile phase.
VA(0x0045751b, 0x6c0)
class mapCell * advManager::DoAdvCommand(void)
{
    mapCell *eventCellState = 0;
    town *viewTownValue;
    char movementStopped = 0;
    char refreshHover = 0;
    hero *currentHeroState;
    int oldVisibilityStateLocal;
    int pathIndexLocal;
    int movementChangedResult;
    int movementEndedLocal;
    tag_message messageValue;
    if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO)
        currentHeroState = 0;
    else
        currentHeroState = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];

    switch (m_selectedCell) {
    case ADVMGR_COMMAND_MOVE_TO:
        if (currentHeroState == 0)
            break;
        currentHeroState->m_destinationX = m_commandTargetX,
        currentHeroState->m_destinationY = m_commandTargetY;
        goto continue_route;
    case ADVMGR_COMMAND_CONTINUE_ROUTE:
continue_route:
        if (currentHeroState == 0)
            break;
        if (currentHeroState->m_destinationX == ADVMGR_INVALID_CELL ||
            currentHeroState->m_destinationY == ADVMGR_INVALID_CELL)
            break;
        gpSearchArray->BuildPath(currentHeroState->m_x, currentHeroState->m_y,
                                 currentHeroState->m_destinationX,
                                 currentHeroState->m_destinationY, 59999);
        if (gpSearchArray->m_pathLength > 0) {
            oldVisibilityStateLocal = m_visibilityMapValid;
            MobilizeCurrHero(1);
            if (gConfig.showRoute || oldVisibilityStateLocal) {
                ShowRoute(0, 0, 0);
            } else {
                if (m_visibilityMapValid && m_selectedCell != ADVMGR_COMMAND_CONTINUE_ROUTE)
                    HideRoute(1, 0, 1);
            }
            gpMouseManager->HideColorPointer();
            gpInputManager->Flush();

            pathIndexLocal = gpSearchArray->m_pathLength - 1;
            for (; pathIndexLocal >= 0; --pathIndexLocal) {
                eventCellState = MoveHero(static_cast<unsigned char>(
                                              gpSearchArray->m_storage.path.directions[
                                                  pathIndexLocal + 1]),
                                          pathIndexLocal == 0,
                                          &TrigX, &TrigY, &movementChangedResult, 0,
                                          &movementEndedLocal, 0);
                UpdateHeroLocator(-1, 1, 1);
                if (eventCellState != 0)
                    break;
                if (movementChangedResult || movementEndedLocal || gbHitEvent)
                    goto movement_done;
                messageValue = gpInputManager->GetEvent();
                while (messageValue.type != 0) {
                    if (messageValue.type == ADVMGR_INPUT_MOUSE_DOWN ||
                        messageValue.type == ADVMGR_INPUT_MOUSE_UP ||
                        messageValue.type == ADVMGR_INPUT_KEY_DOWN ||
                        messageValue.type == ADVMGR_INPUT_COMMAND) {
                        movementStopped = 1;
                        StopCursor(1);
                        goto movement_done;
                    }
                    Process1WindowsMessage();
                    messageValue = gpInputManager->GetEvent();
                }
            }

movement_done:
            if ((pathIndexLocal <= 0 && currentHeroState->m_x == currentHeroState->m_destinationX &&
                 currentHeroState->m_destinationY == currentHeroState->m_y) ||
                (movementStopped && !gConfig.showRoute) || eventCellState != 0) {
                HideRoute(1, 1, 1);
            } else if (m_selectedCell == ADVMGR_COMMAND_CONTINUE_ROUTE || gConfig.showRoute) {
                ShowRoute(0, 1, 1);
            }
            while (gpMouseManager->m_hideCount != 0)
                gpMouseManager->ShowColorPointer();
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
            UpdBottomView(1, 1, 1);
            if (eventCellState != 0) {
                StopCursor(1);
                DoEvent(eventCellState, TrigX, TrigY);
                eventCellState = 0;
            }
            Reseed(0, 0);
            refreshHover = 1;
            CheckDimHero();
        }
        break;

    case ADVMGR_COMMAND_OCCUPIED_TOWN_VIEW:
        DemobilizeCurrHero();
        gpMouseManager->SetPointer(0);
        viewTownValue = gpGame->GetTown(currentHeroState->m_occupiedTown);
        viewTownValue->View(0);
        eventCellState = 0;
        break;

    case ADVMGR_COMMAND_TOWN_VIEW:
        DemobilizeCurrHero();
        gpMouseManager->SetPointer(0);
        eventCellState = GetCell(
            gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].m_x,
            gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].m_y);
        gpGame->m_castleRecs[gpCurPlayer->CurrentTown()].View(0);
        eventCellState = 0;
        break;

    case ADVMGR_COMMAND_HERO_VIEW:
        if (currentHeroState == 0)
            break;
            gpMouseManager->SetPointer(0);
            if (gbLowMemory)
                SetEnvironmentOrigin(-1, -1, 1);
            TrimLoopingSounds(0);
            HeroView(gpCurPlayer->CurrentHero(), 0, 0);
            if (gbLowMemory)
                SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
            RedrawAdvScreen(1, 0);
            gpWindowManager->FadeScreen(0, 8, 0);
        break;

    case ADVMGR_COMMAND_SELECT_HERO:
        SetHeroContext(GetCell(m_mapOriginX + m_lastHoverCell,
                               m_hoverCellY + m_mapOriginY)->m_objectMetadata, 0);
        break;

    case ADVMGR_COMMAND_SELECT_TOWN:
        SetTownContext(GetCell(m_mapOriginX + m_lastHoverCell,
                               m_hoverCellY + m_mapOriginY)->m_objectMetadata);
        break;

    case ADVMGR_COMMAND_NONE:
        break;

    }

    m_selectedCell = ADVMGR_COMMAND_NONE;
    m_hoverCellY = ADVMGR_INVALID_CELL;
    m_lastHoverCell = m_hoverCellY;
    if (refreshHover)
        ForceNewHover();
    return eventCellState;
}

// @early-stop
// Raw instructions and relocation sites are byte-identical; only the delinked gConfig relocation symbol name differs.
VA(0x00457bdb, 0x191)
void advManager::CheckSetEvilInterface(int redraw, int player)
{
    if (player == -1)
        player = giCurWatchPlayer;

    int shouldChange = 0;
    if (gConfig.evilInterfaceUsage == ADVMGR_INTERFACE_EVIL && !gbUseEvilInterface)
        shouldChange = 1;
    else if (gConfig.evilInterfaceUsage == ADVMGR_INTERFACE_GOOD && gbUseEvilInterface)
        shouldChange = 1;
    else if (gConfig.evilInterfaceUsage == ADVMGR_INTERFACE_AUTO &&
             gpGame->m_players[player].m_evilInterface != gbUseEvilInterface)
        shouldChange = 1;

    if (shouldChange) {
        gbUseEvilInterface = 1 - gbUseEvilInterface;
        if (redraw) {
            tag_message interfaceMessage;
            interfaceMessage.type = ADVMGR_INTERFACE_MESSAGE;
            interfaceMessage.payload.widget.command = ADVMGR_INTERFACE_REPLACE_RESOURCE;
            int translationIndex;
            for (translationIndex = 0;
                 translationIndex < ADVMGR_INTERFACE_TRANSLATION_COUNT; ++translationIndex) {
                interfaceMessage.payload.widget.id = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][1 - gbUseEvilInterface], 0);
                interfaceMessage.payload.widget.data.value = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][gbUseEvilInterface], 0);
                m_adventureWindow->BroadcastMessage(interfaceMessage);
            }
            int savedShowIt = bShowIt;
            bShowIt = 1;
            RedrawAdvScreen(1, 1);
            bShowIt = savedShowIt;
        }
    }
}

// @early-stop
// Excluding the 0x20 and 0x94 pointer tables and the latter's adjacent 0x51
// byte lookup, all 902 executable instructions, the 0x48 frame, and all 232
// relocation targets match. Retail's const_00128d28 is gConfig + 0x08
// (musicVolume); the other displayed addend is the delinked byte table.
// Retail has two trailing alignment NOPs.
VA(0x00457d6c, 0xfda)
int advManager::Main(struct tag_message &message)
{
    if (KBTickCount() > glTimers[0] && ComboDraw(1))
        UpdateScreen(1, 0);
    if (gbGameOver) {
        message.type = 0x4000;
        message.payload.executive.command = 1;
        return 2;
    }

    CheckHandleNet();
    if (!gbThisNetGotAdventureControl)
        return CheckHandleNetPlayerWait(message, 0);
    if (!gbHumanPlayer[giCurPlayer]) {
        gpPhilAI->DoAI(giCurPlayer);
        gpGame->NextPlayer();
        return 1;
    }
    if (giScreenScroll && gbForegroundApp)
        CheckScreenScroll();
    if (!gbNoSound && gConfig.musicVolume && giForceSwitchMusic > 0 &&
        KBTickCount() - giForceSwitchMusic > 6000) {
        giForceSwitchMusic = -1;
        if (gpSoundManager->m_currentTrack == 21)
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
        SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
    }

    int processResult = 1;
    int exitRequestedFlag = 0;
    mapCell *eventCellsResult[3];
    eventCellsResult[0] = 0;
    int moveDirectionState;
    int cheatDigitLocal;
    int nextTownId;
    hero *currentHero;
    if (message.type != 0) {
    switch (message.type) {
    case 0x200:
        switch (message.payload.widget.command) {
        case 13:
            if (!(message.payload.widget.parameter & 0x200))
                processResult = ProcessDeSelect(&message, &exitRequestedFlag, eventCellsResult);
            break;
        case 12:
        case 14: {
            int helpIndexState;
            if (message.payload.widget.parameter & 0x200) {
                helpIndexState = -1;
                switch (message.payload.widget.id) {
                case 1: helpIndexState = 0; break;
                case 2: helpIndexState = 1; break;
                case 3: helpIndexState = 2; break;
                case 4: helpIndexState = 3; break;
                case 5: helpIndexState = 4; break;
                case 6: helpIndexState = 5; break;
                case 7: helpIndexState = 6; break;
                case 8: helpIndexState = 7; break;
                }
                if (helpIndexState >= 0) {
                    NormalDialog(gAdvMenuHelp[helpIndexState], 4, -1, -1, -1,
                                 0, -1, 0, -1, 0);
                    break;
                }
            }
            processResult = ProcessSelect(&message, eventCellsResult);
            break;
        }
        }
        break;
    case 4:
        processResult = ProcessHover(message.payload.mouse.screenX, message.payload.mouse.screenY);
    default:
        break;
    case 1:
        moveDirectionState = -1;
        if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO)
            currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
        else
            currentHero = 0;
        if (giDebugLevel < 1 &&
            (message.payload.keyboard.keyCode == 61 || message.payload.keyboard.keyCode == 62 ||
             message.payload.keyboard.keyCode == 63 || message.payload.keyboard.keyCode == 64 ||
             message.payload.keyboard.keyCode == 65 || message.payload.keyboard.keyCode == 66 ||
             message.payload.keyboard.keyCode == 67 || message.payload.keyboard.keyCode == 68 ||
             message.payload.keyboard.keyCode == 87 || message.payload.keyboard.keyCode == 88))
            break;

    switch (message.payload.keyboard.keyCode) {
    case 60:
        CreateColorTables();
        PopNetBox(0, -1);
        break;
    case 64:
        gpGame->CheckHeroConsistency();
        break;
    case 65:
        gpCurPlayer->m_cheatValue += 12;
        if (currentHero != 0)
            GiveExperience(currentHero, 3000, 1);
        break;
    case 66:
    case 1:
        break;
    case 67:
        for (cheatDigitLocal = 0; cheatDigitLocal < 7; ++cheatDigitLocal) {
            if (cheatDigitLocal == RES_GOLD)
                gpCurPlayer->m_resources[cheatDigitLocal] += 1000;
            else
                gpCurPlayer->m_resources[cheatDigitLocal] += 10;
        }
        break;
    case 11:
        cheatDigitLocal = 0;
        goto process_cheat_digit;
    case 2: cheatDigitLocal = 1; goto process_cheat_digit;
    case 3: cheatDigitLocal = 2; goto process_cheat_digit;
    case 4: cheatDigitLocal = 3; goto process_cheat_digit;
    case 5: cheatDigitLocal = 4; goto process_cheat_digit;
    case 6: cheatDigitLocal = 5; goto process_cheat_digit;
    case 7: cheatDigitLocal = 6; goto process_cheat_digit;
    case 8: cheatDigitLocal = 7; goto process_cheat_digit;
    case 9: cheatDigitLocal = 8; goto process_cheat_digit;
    case 10: cheatDigitLocal = 9;
        goto process_cheat_digit;
process_cheat_digit: {
        hero *cheatHero = 0;
        if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO)
            cheatHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
        giCheatSeq = cheatDigitLocal + (giCheatSeq * 10) % 10000000;
        if (!gbRemoteOn) {
            if (giCheatSeq % 100000 == 0x7da7 && cheatHero != 0) {
                gpGame->m_cheated = 1;
                if (gbInCampaign)
                    gpGame->m_campaignCheated = 1;
                gpGame->GiveArmy(&cheatHero->m_army, 37, 5, -1);
                UpdBottomView(1, 1, 1);
            }
            if (giCheatSeq % 1000 == 0x38f) {
                gpGame->m_cheated = 1;
                if (gbInCampaign)
                    gpGame->m_campaignCheated = 1;
                CheckEndGame(1, 0);
            }
            if (giCheatSeq % 10000 == 0x521) {
                gpGame->m_cheated = 1;
                CheckEndGame(2, 0);
            }
        }
        if (giCheatSeq % 10000000 == 0x845fed) {
            gpGame->m_cheated = 1;
            if (gbInCampaign)
                gpGame->m_campaignCheated = 1;
            gpGame->SetVisibility(30, 30, 0, 180);
            gpGame->SetVisibility(30, 30, 1, 180);
            gpGame->SetVisibility(30, 30, 2, 180);
            gpGame->SetVisibility(30, 30, 3, 180);
            gpGame->SetVisibility(30, 30, 4, 180);
            gpGame->SetVisibility(30, 30, 5, 180);
            Reseed(0, 0);
            UpdateRadar(1, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);
        }
        if (giCheatSeq % 1000 == 0x19b) {
            sprintf(gText, "Coordinates at top left corner of view: %d, %d",
                    m_mapOriginX, m_mapOriginY);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        }
        break;
    }
    case 72:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(0, 0); else moveDirectionState = 0;
        break;
    case 73:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(1, 0); else moveDirectionState = 1;
        break;
    case 77:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(2, 0); else moveDirectionState = 2;
        break;
    case 81:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(3, 0); else moveDirectionState = 3;
        break;
    case 80:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(4, 0); else moveDirectionState = 4;
        break;
    case 79:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(5, 0); else moveDirectionState = 5;
        break;
    case 75:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(6, 0); else moveDirectionState = 6;
        break;
    case 71:
        if (message.payload.keyboard.modifiers & 0xc) ScreenScroll(7, 0); else moveDirectionState = 7;
        break;
    case 46:
        CheckCastSpell();
        break;
    case 32:
        ProcessSearch(-1, -1);
        break;
    case 25:
        ViewPuzzle();
        break;
    case 47:
        ViewWorld(0x35, 0, 0);
        break;
    case 49:
        cheatDigitLocal = 0x65;
        strcpy(gText, "Are you sure you want to restart?  (Your current game will be lost)");
        goto confirm_game_command;
    case 38:
        cheatDigitLocal = 0x66;
        strcpy(gText, "Are you sure you want to load a new game?  (Your current game will be lost)");
        goto confirm_game_command;
    case 16:
        cheatDigitLocal = 0x69;
        strcpy(gText, "Are you sure you want to quit?");
        goto confirm_game_command;
confirm_game_command:
        exitRequestedFlag = 1;
        NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gpWindowManager->m_dialogResult == ADVMGR_DIALOG_OK)
            exitRequestedFlag = 0;
        else
            gGameCommand = cheatDigitLocal;
        break;
    case 31:
        SaveGame();
        break;
    case 23:
        if (gbInCampaign) {
            SetEnvironmentOrigin(-1, -1, 1);
            gpGame->ShowCampaignInfo(1, 0);
            SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
            RedrawAdvScreen(1, 0);
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
        } else if (xIsPlayingExpansionCampaign) {
            SetEnvironmentOrigin(-1, -1, 1);
            xCampaign.ShowInfo(1, 0);
            SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
            RedrawAdvScreen(1, 0);
            gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
        } else {
            gpGame->ShowScenInfo();
        }
        break;
    case 20:
        if (gpCurPlayer->m_townCount >= 0) {
            if (gpCurPlayer->CurrentTown() == ADVMGR_INVALID_CELL) {
                nextTownId = gpCurPlayer->m_townIds[0];
            } else {
                nextTownId = 0;
                for (cheatDigitLocal = 0; cheatDigitLocal < gpCurPlayer->m_townCount; ++cheatDigitLocal) {
                    if (gpCurPlayer->TownId(cheatDigitLocal) == gpCurPlayer->m_currentTown) {
                        if (cheatDigitLocal == gpCurPlayer->m_townCount - 1)
                            nextTownId = gpCurPlayer->m_townIds[0];
                        else
                            nextTownId = gpCurPlayer->m_townIds[cheatDigitLocal + 1];
                    }
                }
            }
            SetTownContext(nextTownId);
        }
        break;
    case 35:
        SetHeroContext(gpCurPlayer->NextHero(0), 0);
        break;
    case 28:
        if (gpCurPlayer->CurrentTown() != ADVMGR_INVALID_CELL) {
            m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
            DoAdvCommand();
        } else {
            if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO) {
                m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                DoAdvCommand();
            }
        }
        break;
    }

    if (gpCurPlayer->m_currentHero != ADVMGR_INVALID_HERO && moveDirectionState >= 0) {
        HideRoute(1, 1, 1);
        gpMouseManager->HideColorPointer();
        int movementChanged;
        int movementEnded;
        eventCellsResult[0] = MoveHero(moveDirectionState, 1, &TrigX, &TrigY,
                                 &movementChanged, 0, &movementEnded, 0);
        UpdateHeroLocator(-1, 1, 1);
        while (gpMouseManager->m_hideCount != 0)
            gpMouseManager->ShowColorPointer();
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
        if (eventCellsResult[0] != 0) {
            StopCursor(1);
            DoEvent(eventCellsResult[0], TrigX, TrigY);
            eventCellsResult[0] = 0;
        }
        Reseed(0, 0);
        ForceNewHover();
        UpdBottomView(1, 1, 1);
        CheckDimHero();
    }
        break;
    }
    }

finish_message:
    if (eventCellsResult[0] != 0)
        DoEvent(eventCellsResult[0], TrigX, TrigY);
    if (gbGameOver || exitRequestedFlag == 1 || giMenuCommand != -1) {
        message.type = 0x4000;
        message.payload.executive.command = 1;
        return 2;
    }
    return processResult;
}

VA(0x00458d46, 0x22)
void advManager::Reseed(int, int)
{
    giSeedingValid = 0;
}

// @match-note
// The retained source-hash object has the complete 0xb8 frame/slots, switch and radar
// CFG, 999 aligned instructions, and all 118 relocation targets. The first code
// residual is commutative load order at +0x73b; later residuals at +0xa94..+0xd21
// have the same cause, followed by one moved /Ob1 continuation and one adjacent-
// block jump at +0xd71. Reversing the commutative expressions, using direct
// members versus CurrentHero(), and both boundary-test polarities were checked.
// Revisit after a material predecessor/TU-state change or in the last-mile phase.
VA(0x00458d68, 0xeb1)
int advManager::ProcessSelect(struct tag_message *message, class mapCell **eventCell)
{
    int mouseX;
    mapCell *currentCell;
    int objectTypeState;
    int objectIdIndex;
    int mouseY;
    int visible;
    int pageState;
    float radarScale;
    tag_message currentMessageLocal;
    tag_message radarMessage;
    int mobileResult;
    hero *currentHero;

    visible = 1;
    mouseX = message->payload.mouse.screenX;
    mouseY = message->payload.mouse.screenY;

    switch (message->payload.widget.id) {
    case 105:
    case 112:
    case 119:
    case 126: {
        pageState = (message->payload.widget.id - 105) / 7;
        if (pageState >= gpCurPlayer->m_heroCount)
            break;
        objectTypeState = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + pageState];
        if (message->payload.widget.parameter & 0x200) {
            HeroQuickView(objectTypeState, pageState, -1, -1);
        } else {
            if (gpCurPlayer->CurrentHero() == objectTypeState) {
                m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                DoAdvCommand();
            } else {
                HideRoute(1, 0, 1);
                SetHeroContext(objectTypeState, 0);
            }
        }
        break;
    }
    case 16:
    case 17:
    case 18:
    case 19: {
        objectTypeState = gpCurPlayer->m_townIds[
            gpCurPlayer->m_townLocatorPage + message->payload.widget.id - 16];
        if (message->payload.widget.parameter & 0x200) {
            TownQuickView(objectTypeState, message->payload.widget.id - 16, -1, -1);
        } else {
            HideRoute(1, 0, 1);
            if (gpCurPlayer->CurrentTown() == objectTypeState) {
                m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                *eventCell = DoAdvCommand();
            } else {
                SetTownContext(objectTypeState);
            }
        }
        break;
    }
    case 26:
        DoHeroKnob();
        break;
    case 22: {
        mouseY -= 194;
        if (gpCurPlayer->m_heroCount > ADVMGR_LOCATOR_PAGE_STEP) {
            pageState = mouseY / (92 / (gpCurPlayer->m_heroCount - 3));
            if (pageState > gpCurPlayer->m_heroCount - 4)
                pageState = gpCurPlayer->m_heroCount - 4;
        } else
            pageState = 0;
        gpCurPlayer->m_heroLocatorPage = pageState;
        UpdateHeroLocators(1, 1);
        break;
    }
    case 27:
        DoTownKnob();
        break;
    case 25: {
        mouseY -= 194;
        if (gpCurPlayer->m_townCount > ADVMGR_LOCATOR_PAGE_STEP) {
            pageState = mouseY / (92 / (gpCurPlayer->m_townCount - 3));
            if (pageState > gpCurPlayer->m_townCount - 4)
                pageState = gpCurPlayer->m_townCount - 4;
        } else
            pageState = 0;
        gpCurPlayer->m_townLocatorPage = pageState;
        UpdateTownLocators(1, 1);
        break;
    }
    case 10: {
        if (!(giCurPlayerBit &
              (mapExtra + (m_hoverCellY + m_mapOriginY) * MAP_WIDTH)
                  [m_lastHoverCell + m_mapOriginX]))
            visible = 0;
        currentCell = GetCell(m_lastHoverCell + m_mapOriginX,
                              m_hoverCellY + m_mapOriginY);
        if (message->payload.widget.parameter & 0x200) {
            if (!visible) {
                QuickInfo(m_lastHoverCell, m_hoverCellY);
            } else {
                if (m_lastHoverCell == 7 && m_hoverCellY == 7 &&
                    gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO && m_heroContextLocked) {
                    objectTypeState = MAP_EVENT_HERO_INTERACTION;
                    objectIdIndex = gpCurPlayer->CurrentHero();
                } else {
                    objectTypeState = currentCell->m_triggerType & 0x7f;
                    objectIdIndex = currentCell->m_objectMetadata;
                }
                switch (objectTypeState) {
                case MAP_EVENT_HERO_INTERACTION:
                    mouseX = m_lastHoverCell * 32 - 73;
                    if (mouseX < 30)
                        mouseX = 30;
                    if (mouseX + 192 > 448)
                        mouseX = 256;
                    mouseY = m_hoverCellY * 32 - 72;
                    if (mouseY < 16)
                        mouseY = 16;
                    if (mouseY + 190 > 464)
                        mouseY = 274;
                    HeroQuickView(objectIdIndex, -1, mouseX, mouseY);
                    break;
                case MAP_EVENT_CASTLE:
                    mouseX = m_lastHoverCell * 32 - 89;
                    if (mouseX < 30)
                        mouseX = 30;
                    if (mouseX + 224 > 448)
                        mouseX = 224;
                    mouseY = m_hoverCellY * 32 - 70;
                    if (mouseY < 16)
                        mouseY = 16;
                    if (mouseY + 186 > 464)
                        mouseY = 278;
                    TownQuickView(objectIdIndex, -1, mouseX, mouseY);
                    break;
                default:
                    if (giCurPlayerBit &
                        (mapExtra + (m_hoverCellY + m_mapOriginY) * MAP_WIDTH)
                            [m_lastHoverCell + m_mapOriginX]) {
                        QuickInfo(m_lastHoverCell, m_hoverCellY);
                    }
                    break;
                }
            }
        } else if (visible) {
            currentHero = 0;
            mobileResult = 0;
            if (gpCurPlayer->m_currentHero != ADVMGR_INVALID_HERO) {
                currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
                mobileResult = gpGame->IsMobile(static_cast<unsigned char>(currentHero->m_id));
            }
            if (currentHero != 0) {
                if (m_lastHoverCell == 7 && m_hoverCellY == 7 &&
                    gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO &&
                    m_heroContextLocked) {
                    m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                    DoAdvCommand();
                } else if ((!mobileResult || (message->payload.widget.parameter & 0xc) ||
                            (gConfig.showRoute &&
                             (m_commandTargetX != currentHero->m_destinationX ||
                              m_commandTargetY != currentHero->m_destinationY))) &&
                           (gpSearchArray->GetCell(
                                m_commandTargetX, m_commandTargetY).flags & 1)) {
                    currentHero->m_destinationX = m_commandTargetX;
                    currentHero->m_destinationY = m_commandTargetY;
                    ShowRoute(1, 1, 1);
                } else {
                    *eventCell = DoAdvCommand();
                }
            } else {
                objectTypeState = currentCell->m_triggerType & 0x7f;
                objectIdIndex = currentCell->m_objectMetadata;
                if (objectTypeState == MAP_EVENT_HERO_INTERACTION) {
                    if (gpCurPlayer->CurrentHero() == objectIdIndex) {
                        m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                        DoAdvCommand();
                    } else if (gpGame->GetHero(objectIdIndex)->m_owner == giCurPlayer) {
                        SetHeroContext(objectIdIndex, 0);
                    }
                }
                if (objectTypeState == MAP_EVENT_CASTLE) {
                    if (gpCurPlayer->CurrentTown() == objectIdIndex) {
                        m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                        *eventCell = DoAdvCommand();
                    } else if (gpGame->GetTown(objectIdIndex)->m_owner == giCurPlayer) {
                        SetTownContext(objectIdIndex);
                    }
                }
            }
        }
        break;
    }
    case 9:
        if (message->payload.widget.parameter & 0x200) {
            NormalDialog("{World Map}",
                         4, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        }
            DemobilizeCurrHero();
            switch (MAP_HEIGHT) {
            case 36:
                radarScale = 4.0f;
                break;
            case 72:
                radarScale = 2.0f;
                break;
            case 108:
                radarScale = 1.3333f;
                break;
            default:
                radarScale = 1.0f;
                break;
            }
            mouseX = static_cast<int>((mouseX - 480) / radarScale);
            mouseY = static_cast<int>((mouseY - 16) / radarScale);
            m_mapOriginX = mouseX - 7;
            m_mapOriginY = mouseY - 7;
            if (m_mapOriginX < -7)
                m_mapOriginX = -7;
            if (m_mapOriginY < -7)
                m_mapOriginY = -7;
            if (!(MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN >= m_mapOriginX))
                m_mapOriginX = MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN;
            if (!(MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN >= m_mapOriginY))
                m_mapOriginY = MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN;
            UpdateRadar(1, 0);
            CompleteDraw(0);
            UpdateScreen(0, 0);

            currentMessageLocal.type = 0;
            while (currentMessageLocal.type != 0x10) {
                Process1WindowsMessage();
                currentMessageLocal = gpInputManager->GetEvent();
                radarMessage = currentMessageLocal;
                while (currentMessageLocal.type != 0x10 && currentMessageLocal.type != 0) {
                    if (currentMessageLocal.type == 4)
                        radarMessage = currentMessageLocal;
                    Process1WindowsMessage();
                    currentMessageLocal = gpInputManager->GetEvent();
                }
                if (radarMessage.type == 4) {
                    if (radarMessage.payload.mouse.x < ADVMGR_RADAR_LEFT)
                        radarMessage.payload.mouse.x = ADVMGR_RADAR_LEFT;
                    if (radarMessage.payload.mouse.x >= ADVMGR_RADAR_RIGHT)
                        radarMessage.payload.mouse.x = ADVMGR_RADAR_RIGHT - 1;
                    if (radarMessage.payload.mouse.y < ADVMGR_RADAR_TOP)
                        radarMessage.payload.mouse.y = ADVMGR_RADAR_TOP;
                    if (radarMessage.payload.mouse.y >= ADVMGR_RADAR_BOTTOM)
                        radarMessage.payload.mouse.y = ADVMGR_RADAR_BOTTOM - 1;
                    gpMouseManager->Main(radarMessage);
                    mouseX = static_cast<int>((radarMessage.payload.mouse.x - 480) / radarScale);
                    mouseY = static_cast<int>((radarMessage.payload.mouse.y - 16) / radarScale);
                    m_mapOriginX = mouseX - 7;
                    m_mapOriginY = mouseY - 7;
                    if (m_mapOriginX < -7)
                        m_mapOriginX = -7;
                    if (m_mapOriginY < -7)
                        m_mapOriginY = -7;
                    if (!(MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN >= m_mapOriginX))
                        m_mapOriginX = MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN;
                    if (!(MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN >= m_mapOriginY))
                        m_mapOriginY = MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN;
                    UpdateRadar(1, 0);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                    radarMessage.type = 0;
                }
            }
        break;
    }

    if ((message->payload.widget.parameter & 0x200) &&
        message->payload.widget.id >= ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE &&
        message->payload.widget.id <= ADVMGR_BOTTOM_VIEW_LAST_MESSAGE) {
        NormalDialog("{Status Window}",
                     4, -1, -1, -1, 0, -1, 0, -1, 0);
    }
    return 1;
}

// @early-stop
// All 0x463 bytes / 401 instructions match with relocation bytes masked; residual is one string symbol, 15 delinked switch-table local-label identities, and the same iCurBottomView target.
VA(0x00459c19, 0x463)
int advManager::ProcessDeSelect(struct tag_message *message, int *result,
                                class mapCell **eventCell)
{
    switch (message->payload.widget.id) {
    case ADVMGR_PANEL_TOWN_PAGE_PREVIOUS:
        if (gpCurPlayer->m_townLocatorPage > 0) {
            --gpCurPlayer->m_townLocatorPage;
            UpdateTownLocators(1, 1);
        }
        break;
    case ADVMGR_PANEL_TOWN_PAGE_NEXT:
        if (gpCurPlayer->m_townLocatorPage + ADVMGR_LOCATOR_PAGE_STEP <
            gpCurPlayer->m_townCount) {
            ++gpCurPlayer->m_townLocatorPage;
            UpdateTownLocators(1, 1);
        }
        break;
    case ADVMGR_PANEL_HERO_PAGE_PREVIOUS:
        if (gpCurPlayer->m_heroLocatorPage > 0) {
            --gpCurPlayer->m_heroLocatorPage;
            UpdateHeroLocators(1, 1);
        }
        break;
    case ADVMGR_PANEL_HERO_PAGE_NEXT:
        if (gpCurPlayer->m_heroLocatorPage + ADVMGR_LOCATOR_PAGE_STEP <
            gpCurPlayer->m_heroCount) {
            ++gpCurPlayer->m_heroLocatorPage;
            UpdateHeroLocators(1, 1);
        }
        break;
    case ADVMGR_PANEL_CONTINUE_ROUTE:
        m_selectedCell = ADVMGR_COMMAND_CONTINUE_ROUTE;
        *eventCell = DoAdvCommand();
        break;
    case ADVMGR_PANEL_ADVENTURE_OPTIONS:
        AdvPanel();
        break;
    case ADVMGR_PANEL_CONTROL_OPTIONS:
        *result = ControlPanel();
        break;
    case ADVMGR_PANEL_END_TURN:
        if (gpCurPlayer->HasMobileHero()) {
            NormalDialog("One or more heroes may still move, are you sure you want to end your turn?",
                         2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == ADVMGR_DIALOG_OK)
                break;
        }
        gpGame->NextPlayer();
        break;
    case ADVMGR_PANEL_NEXT_HERO:
        HideRoute(1, 0, 1);
        SetHeroContext(gpCurPlayer->NextHero(1), 0);
        break;
    case ADVMGR_PANEL_OVERVIEW: {
        if (gbLowMemory)
            SetEnvironmentOrigin(-1, -1, 1);
        TrimLoopingSounds(0);
        gpGame->Overview();
        int fadeAfter = 1;
        if (giOverviewReturnAction == ADVMGR_OVERVIEW_HERO) {
            gpMouseManager->SetPointer(0);
            if (gbLowMemory)
                SetEnvironmentOrigin(-1, -1, 1);
            TrimLoopingSounds(0);
            HeroView(giOverviewReturnActionExtra,
                     reinterpret_cast<int>(
                         gpGame->GetHero(giOverviewReturnAction)->GetOccupiedTown()), 1);
            if (gbLowMemory)
                SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
        } else if (giOverviewReturnAction == ADVMGR_OVERVIEW_TOWN) {
            DemobilizeCurrHero();
            gpMouseManager->SetPointer(0);
            gpGame->GetTown(giOverviewReturnActionExtra)->View(1);
            fadeAfter = 0;
        } else if (gbLowMemory) {
            SetEnvironmentOrigin(m_mapOriginX + 7, m_mapOriginY + 7, 1);
        }
        RedrawAdvScreen(1, 0);
        if (fadeAfter)
            gpWindowManager->FadeScreen(0, 8, 0);
        break;
    }
    case ADVMGR_PANEL_CAST_SPELL:
        CheckCastSpell();
        break;
    case ADVMGR_PANEL_SYSTEM_OPTIONS:
        SystemOptions();
        break;
    }

    if (message->payload.widget.id >= ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE &&
        message->payload.widget.id <= ADVMGR_BOTTOM_VIEW_LAST_MESSAGE) {
        if (giBottomViewOverride == 2)
            giBottomViewOverride = 1;
        else if (giBottomViewOverride != 0)
            giBottomViewOverride = 0;
        else if (iCurBottomView == 2)
            giBottomViewOverride = 1;
        else
            giBottomViewOverride = 2;
        giBottomViewOverrideEndTime = KBTickCount() + ADVMGR_BOTTOM_VIEW_DURATION;
        UpdBottomView(1, 1, 1);
    }
    return 1;
}

// @early-stop
// Both sides are 0x5c8 bytes / 390 instructions with the same 0x44 frame, CFG,
// and 84 relocation targets. The only code difference is GetNullSample's five-
// byte /Ob1 continuation: ours is at +0x0c and retail's is at +0x1d. Declaration
// initialization and post-declaration assignment compile to the same placement.
VA(0x0045a07c, 0x5c8)
int advManager::ProcessSearch(int x, int y)
{
    mapCell *currentCell;
    int playerState;
    tag_message messageValue;
    char specialArtifactValue;
    hero *searchingHeroState;
    SAMPLE2 digSampleState;
    int artifactResultLocal;

    digSampleState = GetNullSample();
    searchingHeroState = &gpGame->m_heroRecs[gpCurPlayer->m_currentHero];

    if (searchingHeroState->m_mobility != searchingHeroState->m_remainingMobility) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Digging for artifacts requires a whole day, try again tomorrow.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }
    if (searchingHeroState->NumArtifacts() == ADVMGR_ARTIFACT_CAPACITY) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Searching for the Ultimate Artifact is fruitless.  Your hero could not carry it even if he found it - all his artifact slots are full.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }

    MobilizeCurrHero(0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (x == -1) {
        x = m_mapOriginX + 7;
        y = m_mapOriginY + 7;
    }
    currentCell = GetCell(x, y);
    if (!((currentCell->m_objectIndex == 0xff ||
           currentCell->m_objectTileset == ADVMGR_CLEAR_GROUND_TILESET) &&
          currentCell->m_overlayIndex == 0xff)) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Try searching on clear ground.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }
    if (!giGroundToTerrain[currentCell->m_terrainImageIndex]) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Try looking on land!!!",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }

            if (gbHumanPlayer[giCurPlayer])
                digSampleState = LoadPlaySample("DIGSOUND.82M");
            if (currentCell->m_objectIndex == 0xff ||
                currentCell->m_objectTileset == ADVMGR_CLEAR_GROUND_TILESET) {
                currentCell->m_objectTileset = ADVMGR_DIG_HOLE_TILESET;
                currentCell->m_objectIndex = ADVMGR_DIG_HOLE_FRAME;
                currentCell->m_objectLayerBit0 = 1;
                currentCell->m_objectLayerBit1 = 1;
                currentCell->m_flags |= 0x80;
            }
            CompleteDraw(0);
            UpdateScreen(0, 0);

            if (gpGame->m_ultimateArtifactX == x &&
                gpGame->m_ultimateArtifactY == y &&
                gpGame->m_ultimateArtifactId != ADVMGR_ULTIMATE_ARTIFACT_NONE) {
                if (searchingHeroState->NumArtifacts() >= ADVMGR_ARTIFACT_CAPACITY) {
                    if (gbHumanPlayer[giCurPlayer])
                        NormalDialog("You have no room to carry another artifact!",
                                     1, -1, -1, -1, 0, -1, 0, -1, 0);
                } else {
                    specialArtifactValue = 0;
                    if (xIsPlayingExpansionCampaign)
                        specialArtifactValue = xCampaign.IsSpecialUA();
                    if (gbHumanPlayer[giCurPlayer]) {
                        gpSoundManager->SwitchAmbientMusic(ADVMGR_ULTIMATE_ARTIFACT_MUSIC);
                        if (specialArtifactValue)
                            sprintf(gText, "%s%s",
                                    "Congratulations! After spending many hours digging here, you have uncovered the ",
                                    gArtifactNames[ADVMGR_SPHERE_OF_NEGATION]);
                        else
                            sprintf(gText, "%s%s",
                                    "Congratulations! After spending many hours digging here, you have uncovered the ",
                                    gArtifactNames[gpGame->m_ultimateArtifactId]);
                        NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                        if (specialArtifactValue)
                            searchingHeroState->ViewArtifact(ADVMGR_SPHERE_OF_NEGATION, 0, -1);
                        else
                            searchingHeroState->ViewArtifact(gpGame->m_ultimateArtifactId, 0, -1);
                        gpSoundManager->SwitchAmbientMusic(
                            giTerrainToMusicTrack[m_currentTerrain]);
                    }
                    if (specialArtifactValue)
                        GiveArtifact(searchingHeroState, ADVMGR_SPHERE_OF_NEGATION, 1, -1);
                    else
                        artifactResultLocal = GiveArtifact(searchingHeroState,
                                                           gpGame->m_ultimateArtifactId, 1, -1);
                    gpGame->m_ultimateArtifactId = ADVMGR_ULTIMATE_ARTIFACT_NONE;
                }
            } else if (gbHumanPlayer[giCurPlayer]) {
                NormalDialog("Nothing here.\nWhere could it be?",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            if (gbHumanPlayer[giCurPlayer])
                WaitEndSample(digSampleState, -1);
            for (playerState = 0; playerState < gpGame->m_playerCount; ++playerState)
                ComputeUALoc(playerState);
            searchingHeroState->m_remainingMobility = 0;
            UpdBottomView(1, 1, 1);
            CheckDimHero();
            Reseed(0, 0);
            CheckEndGame(0, 0);
    return 1;
search_end:
    gpCurPlayer->m_ultimateArtifactHintChance = 0;
    return 1;
}

// @early-stop
// Reloc-masked bytes differ only at +0xc5, +0x2d8, +0x9df, and +0x9fb: four
// equivalent local-return displacements. The 0x2c frame/slots and CFG agree,
// and all 106 relocation targets resolve to the same addresses.
VA(0x0045a644, 0xa50)
int advManager::ProcessHover(int mouseX, int mouseY) {
    int heroXHero;
    int heroYCoordinate;
    int cellXPosition;
    int cellYCurrent;
    town* hoverTownCell;
    mapCell* hoverCellLocal;
    int routeDaysCount;
    hero* hoverHeroLocal;
    int pointerBaseCursor;

    if (InMapArea(mouseX, mouseY)) {
        if (mouseX > ADVMGR_DRAW_CLIP_WIDTH) {
            gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
            return 1;
        }

        cellXPosition = mouseX / ADVMGR_CELL_PIXELS;
        cellYCurrent = mouseY / ADVMGR_CELL_PIXELS;
        if (cellXPosition < 0) {
            cellXPosition = 0;
        }
        if (cellYCurrent < 0) {
            cellYCurrent = 0;
        }
        if (cellXPosition > ADVMGR_HOVER_MAX_CELL) {
            cellXPosition = ADVMGR_HOVER_MAX_CELL;
        }
        if (cellYCurrent > ADVMGR_HOVER_MAX_CELL) {
            cellYCurrent = ADVMGR_HOVER_MAX_CELL;
        }

        if (m_lastHoverCell != cellXPosition || m_hoverCellY != cellYCurrent) {

            m_selectedCell = ADVMGR_COMMAND_NONE;
            m_lastHoverCell = cellXPosition;
            m_hoverCellY = cellYCurrent;
            m_commandTargetX = m_mapOriginX + cellXPosition;
            m_commandTargetY = m_mapOriginY + cellYCurrent;

            if (m_commandTargetX < 0 || m_commandTargetY < 0 || m_commandTargetX > MAP_WIDTH - 1
                || m_commandTargetY > MAP_HEIGHT - 1
                || !(giCurPlayerBit & mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX])) {
                gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                return 1;
            }

            hoverCellLocal = GetCell(m_commandTargetX, m_commandTargetY);
            if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO) {
                if ((hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) == ADVMGR_HOVER_TOWN
                    && gpGame->GetTown(hoverCellLocal->m_objectMetadata)->m_owner == giCurPlayer) {
                    gpMouseManager->SetPointer(ADVMGR_POINTER_TOWN);
                    m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                    return 1;
                } else {
                    if ((hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK)
                            == ADVMGR_HOVER_HERO
                        && gpGame->GetHero(hoverCellLocal->m_objectMetadata)->m_owner == giCurPlayer) {
                        gpMouseManager->SetPointer(ADVMGR_POINTER_HERO);
                        m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                        return 1;
                    } else {
                        gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                        return 1;
                    }
                }
            } else {
                hoverHeroLocal = gpGame->GetHero(gpCurPlayer->m_currentHero);
                heroXHero = hoverHeroLocal->m_x - m_mapOriginX;
                heroYCoordinate = hoverHeroLocal->m_y - m_mapOriginY;
                if (cellXPosition == heroXHero && cellYCurrent == heroYCoordinate) {
                    gpMouseManager->SetPointer(ADVMGR_POINTER_HERO);
                    m_selectedCell = ADVMGR_COMMAND_HERO_VIEW;
                    return 1;
                }

                if (hoverCellLocal->m_flags & ADVMGR_HOVER_OBJECT_BLOCKED) {
                    if ((hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK)
                        == ADVMGR_HOVER_TOWN) {
                        hoverTownCell = gpGame->GetTown(hoverCellLocal->m_objectMetadata);
                        if (hoverTownCell->m_owner == giCurPlayer) {
                            gpMouseManager->SetPointer(ADVMGR_POINTER_TOWN);
                            m_selectedCell = ADVMGR_COMMAND_SELECT_TOWN;
                            return 1;
                        }
                    }
                    gpSearchArray->m_pathLength = 0;
                    gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                    return 1;
                }

                if (!((m_cursorType == ADVMGR_CURSOR_ROUTE
                       || giGroundToTerrain[hoverCellLocal->m_terrainImageIndex]
                       || hoverCellLocal->m_triggerType == ADVMGR_HERO_TRIGGER
                       || hoverCellLocal->m_triggerType == ADVMGR_BOAT_TRIGGER
                       || hoverCellLocal->m_triggerType == ADVMGR_HOVER_SHIPWRECK_TRIGGER)
                      && (m_cursorType != ADVMGR_CURSOR_ROUTE
                          || !giGroundToTerrain[hoverCellLocal->m_terrainImageIndex]
                          || hoverCellLocal->m_triggerType == ADVMGR_HOVER_COAST))) {
                    gpSearchArray->m_pathLength = 0;
                    gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                    return 1;
                }
                SeedTo(m_commandTargetX, m_commandTargetY);
                if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).flags) {
                    if (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                        <= hoverHeroLocal->m_remainingMobility) {
                        routeDaysCount = 0;
                    } else {
                        routeDaysCount =
                            (gpSearchArray->GetCell(m_commandTargetX, m_commandTargetY).previous
                             - hoverHeroLocal->m_remainingMobility)
                                / hoverHeroLocal->m_mobility
                            + 1;
                        if (routeDaysCount > ADVMGR_HOVER_ROUTE_DAY_LIMIT) {
                            routeDaysCount = ADVMGR_HOVER_ROUTE_DAY_LIMIT;
                        }
                    }
                    pointerBaseCursor = routeDaysCount * ADVMGR_HOVER_ROUTE_FRAMES_PER_DAY;

                    switch (hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) {
                        case ADVMGR_HOVER_BOAT:
                            if (m_cursorType != ADVMGR_CURSOR_ROUTE) {
                                gpMouseManager->SetPointer(pointerBaseCursor + ADVMGR_POINTER_SAIL);
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(pointerBaseCursor);
                            }
                            break;
                        case ADVMGR_HOVER_COAST:
                            if (m_cursorType == ADVMGR_CURSOR_ROUTE) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + ADVMGR_POINTER_DISEMBARK
                                );
                            } else if (mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX]
                                       & ADVMGR_HOVER_UNREACHABLE) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + ADVMGR_POINTER_ATTACK
                                );
                            } else {
                                gpMouseManager->SetPointer(pointerBaseCursor + ADVMGR_POINTER_MOVE);
                            }
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case ADVMGR_HOVER_MONSTER:
                            gpMouseManager->SetPointer(pointerBaseCursor + ADVMGR_POINTER_ATTACK);
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                        case ADVMGR_HOVER_HERO:
                            if (gpGame->GetHero(hoverCellLocal->m_objectMetadata)->m_owner != giCurPlayer) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + ADVMGR_POINTER_ATTACK
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            } else {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + ADVMGR_POINTER_SELECT_HERO
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            }
                            break;
                        case ADVMGR_HOVER_TOWN:
                            hoverTownCell = gpGame->GetTown(hoverCellLocal->m_objectMetadata);
                            if ((hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                                && hoverTownCell->m_owner != giCurPlayer
                                && hoverTownCell->HasGarrison()) {
                                gpMouseManager->SetPointer(
                                    pointerBaseCursor + ADVMGR_POINTER_ATTACK
                                );
                                m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                                break;
                            }
                            goto process_default_hover;
                        default:
                        process_default_hover:
                            if (!((mapExtra[m_commandTargetY * MAP_WIDTH + m_commandTargetX]
                                   & ADVMGR_HOVER_UNREACHABLE)
                                  && m_cursorType != ADVMGR_CURSOR_ROUTE
                                  && (!(hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                                      || !StopOnTrigger(hoverCellLocal))
                                  && (gpMouseManager->SetPointer(
                                          pointerBaseCursor + ADVMGR_POINTER_ATTACK
                                      ),
                                      1))) {
                                if (hoverCellLocal->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG) {
                                    if (m_cursorType != ADVMGR_CURSOR_ROUTE) {
                                        if (giGroundToTerrain[hoverCellLocal->m_terrainImageIndex]) {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + ADVMGR_POINTER_ACTION
                                            );
                                        } else if (hoverCellLocal->m_triggerType
                                                   == ADVMGR_HOVER_SHIPWRECK_TRIGGER) {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + ADVMGR_POINTER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + ADVMGR_POINTER_MOVE
                                            );
                                            break;
                                        }
                                    } else {
                                        if (!giGroundToTerrain[hoverCellLocal->m_terrainImageIndex]) {
                                            gpMouseManager->SetPointer(
                                                routeDaysCount + ADVMGR_POINTER_WATER_ACTION
                                            );
                                        } else {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + ADVMGR_POINTER_SAIL
                                            );
                                        }
                                    }
                                } else {
                                    if (m_cursorType == ADVMGR_CURSOR_ROUTE) {
                                        gpMouseManager->SetPointer(
                                            pointerBaseCursor + ADVMGR_POINTER_SAIL
                                        );
                                    } else {
                                        gpMouseManager->SetPointer(
                                            pointerBaseCursor + ADVMGR_POINTER_MOVE
                                        );
                                    }
                                }
                            }
                            m_selectedCell = ADVMGR_COMMAND_MOVE_TO;
                            break;
                    }
                    return 1;
                } else {
                    gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                }
            }
        }
        return 1;
    } else {
        if (!(gpMouseManager->m_cursorFrame >= ADVMGR_HOVER_SCROLL_FRAME_FIRST
              && gpMouseManager->m_cursorFrame < ADVMGR_HOVER_SCROLL_FRAME_END
              && MouseInScrollZone())) {
            gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
        }
        return 1;
    }
}

// @early-stop
// The 0x4 frame/slots, CFG, instructions, and all 31 relocation targets match;
// only +0x42 branches to the adjacent jmp-to-epilogue instead of directly to
// that same epilogue.
VA(0x0045b094, 0x21a)
void advManager::UpdateScreen(int, int forceUpdate)
{
    if (forceUpdate == 0 && bShowIt == 0) {
        if (KBTickCount() > glTimers[0])
            glTimers[0] = KBTickCount() + ADVMGR_TIMER_DELAY;
    } else {
        PollSound();
        giScrollX = m_updateMinX;
        giScrollY = m_updateMinY;
        if (giLimitUpdMinX == ADVMGR_UPDATE_NONE) {
            BlitBitmapToScreen(
                gpWindowManager->m_screen, ADVMGR_UPDATE_VIEWPORT_ORIGIN,
                ADVMGR_UPDATE_VIEWPORT_ORIGIN, ADVMGR_UPDATE_VIEWPORT_SIZE,
                ADVMGR_UPDATE_VIEWPORT_SIZE, ADVMGR_UPDATE_VIEWPORT_ORIGIN,
                ADVMGR_UPDATE_VIEWPORT_ORIGIN);
        } else {
            BlitBitmapToScreen(
                gpWindowManager->m_screen, giLimitUpdMinX, giLimitUpdMinY,
                giLimitUpdMaxX - giLimitUpdMinX,
                giLimitUpdMaxY - giLimitUpdMinY, giLimitUpdMinX,
                giLimitUpdMinY);
        }
        giScrollY = 0;
        giScrollX = giScrollY;
        PollSound();

        if (KBTickCount() > glTimers[0]) {
            ++m_updateMaxY;
            ++m_updateMaxX;
            if (m_updateMaxX >= ADVMGR_UPDATE_ANIMATION_PHASES)
                m_updateMaxX = 0;
            glTimers[0] = KBTickCount() + ADVMGR_TIMER_DELAY;

            if (m_updateMaxX == 1 || m_updateMaxX == 3 ||
                m_updateMaxX == 5) {
                ++m_viewBounds[1];
                m_viewBounds[1] %= ADVMGR_UPDATE_FRAME_CYCLE;
                ++m_viewBounds[3];
                m_viewBounds[3] %= ADVMGR_UPDATE_FRAME_CYCLE;
            } else {
                ++m_viewBounds[0];
                m_viewBounds[0] %= ADVMGR_UPDATE_FRAME_CYCLE;
                ++m_viewBounds[2];
                m_viewBounds[2] %= ADVMGR_UPDATE_FRAME_CYCLE;
            }
        }
        giLimitUpdMinX = ADVMGR_UPDATE_NONE;
        Process1WindowsMessage();
    }
}

// @early-stop
// Exact size and all 23 relocation targets; only twelve commutative X adds
// exchange [ebp-4] drawX and [ebp+8] originX at +0xf9/+0xfc, +0x15c/+0x15f,
// +0x19e/+0x1a1, +0x210/+0x213, +0x254/+0x257, +0x29d/+0x2a0,
// +0x2e1/+0x2e4, +0x323/+0x326, +0x368/+0x36b, +0x3a8/+0x3ab,
// +0x403/+0x406, and +0x463/+0x466.
VA(0x0045b2ae, 0x4eb)
void advManager::CompleteDraw(int originX, int originY, int forceDraw, int updateBottomView) {
    int drawY;
    int drawX;

    PollSound();
    if (forceDraw == 0 && bShowIt == 0) {
        return;
    }

    giLimitUpdMinX = ADVMGR_UPDATE_NONE;
    m_previousOriginX = m_mapOriginX;
    m_previousOriginY = m_mapOriginY;
    if (gbAllBlack != 0) {
        m_mapOriginY = 0;
        m_mapOriginX = m_mapOriginY;
    }

    gpMouseManager->m_cursorReady = 0;
    m_comboHeroDrawn = 0;
    m_forceCompleteDraw = 0;

    for (drawY = 0; drawY < ADVMGR_DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_GROUND, forceDraw);
        }
    }

    for (drawY = 0; drawY < ADVMGR_DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(
                originX + drawX,
                originY + drawY,
                drawX,
                drawY,
                ADVMGR_DRAW_HERO_SHADOW,
                forceDraw
            );
        }
    }

    for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(originX + drawX, originY, drawX, 0, ADVMGR_DRAW_OBJECT, forceDraw);
    }

    for (drawY = 1; drawY < ADVMGR_DRAW_VIEW_CELLS; ++drawY) {
        PollSound();
        if (m_cursorDirection > ADVMGR_DRAW_FORWARD_DIRECTION_MAX) {
            for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_HERO,
                    forceDraw
                );
            }
            for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OVERLAY,
                    forceDraw
                );
            }
        } else {
            for (drawX = ADVMGR_DRAW_LAST_CELL; drawX >= 0; --drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_HERO,
                    forceDraw
                );
            }
            for (drawX = ADVMGR_DRAW_LAST_CELL; drawX >= 0; --drawX) {
                DrawCell(
                    originX + drawX,
                    originY + drawY - 1,
                    drawX,
                    drawY - 1,
                    ADVMGR_DRAW_OVERLAY,
                    forceDraw
                );
            }
        }

        for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_OBJECT, forceDraw);
        }
    }

    for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(
            originX + drawX,
            originY + ADVMGR_DRAW_LAST_CELL,
            drawX,
            ADVMGR_DRAW_LAST_CELL,
            ADVMGR_DRAW_HERO,
            forceDraw
        );
    }
    for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
        DrawCell(
            originX + drawX,
            originY + ADVMGR_DRAW_LAST_CELL,
            drawX,
            ADVMGR_DRAW_LAST_CELL,
            ADVMGR_DRAW_OVERLAY,
            forceDraw
        );
    }

    for (drawY = 0; drawY < ADVMGR_DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(
                originX + drawX,
                originY + drawY,
                drawX,
                drawY,
                ADVMGR_DRAW_OVERLAY_TOP,
                forceDraw
            );
        }
    }
    for (drawY = 0; drawY < ADVMGR_DRAW_VIEW_CELLS; ++drawY) {
        for (drawX = 0; drawX < ADVMGR_DRAW_VIEW_CELLS; ++drawX) {
            DrawCell(originX + drawX, originY + drawY, drawX, drawY, ADVMGR_DRAW_CLOUD, forceDraw);
        }
    }

    DrawAdventureBorder();
    gpMouseManager->m_cursorReady = 1;
    PollSound();
    if (updateBottomView != 0) {
        UpdBottomView(0, 1, 1);
    }

    if (gbAllBlack != 0) {
        m_mapOriginX = m_previousOriginX;
        m_mapOriginY = m_previousOriginY;
    }
}

VA(0x0045b799, 0x3a)
void advManager::CompleteDraw(int update)
{
    CompleteDraw(m_mapOriginX, m_mapOriginY, update, 1);
}

// @match-note
// Complete 0x8 frame, mask/edge CFG, and all 51 relocation targets agree.
// The four residual index blocks are the east and west lookups in each path:
// retail loads MAP_WIDTH before y for east and forms x+mapExtra before the row
// term for west. Reassociating the west pointer terms recovered both diagonals;
// both multiplication orders and all 99 legal single-step AST variants were
// checked without steering the remaining /Od evaluation order.
VA(0x0045b7d3, 0x3a9)
int advManager::GetCloudLookup(int x, int y)
{
    unsigned int cloudMask = 0;

    if (x < 1)
        cloudMask |= ADVMGR_CLOUD_LEFT_EDGE;
    else if (x >= MAP_WIDTH - 1)
        cloudMask |= ADVMGR_CLOUD_RIGHT_EDGE;

    if (y < 1)
        cloudMask |= ADVMGR_CLOUD_TOP_EDGE;
    else if (y >= MAP_HEIGHT - 1)
        cloudMask |= ADVMGR_CLOUD_BOTTOM_EDGE;

    if (cloudMask == 0) {
        if ((giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH;
        if ((giCurWatchPlayerBit & mapExtra[MAP_WIDTH * y + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_EAST;
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH;
        if ((giCurWatchPlayerBit & *(x + mapExtra + MAP_WIDTH * y - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_WEST;
        if ((giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_EAST;
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_EAST;
        if ((giCurWatchPlayerBit & (x + mapExtra - 1)[(y + 1) * MAP_WIDTH]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_WEST;
        if ((giCurWatchPlayerBit & (x + mapExtra - 1)[(y - 1) * MAP_WIDTH]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_WEST;
    } else {
        if ((cloudMask & ADVMGR_CLOUD_NORTH) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH;
        if ((cloudMask & ADVMGR_CLOUD_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[MAP_WIDTH * y + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH;
        if ((cloudMask & ADVMGR_CLOUD_WEST) == 0 &&
            (giCurWatchPlayerBit & *(x + mapExtra + MAP_WIDTH * y - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_WEST;
        if ((cloudMask & ADVMGR_CLOUD_NORTH_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH_WEST) == 0 &&
            (giCurWatchPlayerBit & (x + mapExtra - 1)[(y + 1) * MAP_WIDTH]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_WEST;
        if ((cloudMask & ADVMGR_CLOUD_NORTH_WEST) == 0 &&
            (giCurWatchPlayerBit & (x + mapExtra - 1)[(y - 1) * MAP_WIDTH]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_WEST;
    }
    return giCloudType[cloudMask];
}

// @match-note
// The retained source-hash object accounts for all 2373 retail instructions, the
// complete 0x1c frame/slots and draw CFG, and all 551 relocation targets; there
// is no data island. The first real code divergence is the MAP_WIDTH boundary
// compare at +0x74. The residual set is four reversed boundary compares, one
// moved continuation, equivalent frame-boolean lowering, and eight commutative
// +31 associations with consequent branch displacements. Both comparison
// polarities, direct versus temporary booleans, both +31 associations, and the
// accessor/direct forms were checked. Revisit after a material TU-state change.
VA(0x0045bb7c, 0x24cb)
void advManager::DrawCell(int mapX, int mapY, int screenX, int screenY,
                          int drawMask, int forceDraw)
{
    int animFrame;
    int heroShadowOffset;
    int cursorFrame;
    int boatFrameIndex;
    int boatShadowFrameOffset;
    int cursorSuppressed;

    if (forceDraw == 0 && bShowIt == 0)
        return;

    s_drawPixelX = screenX * ADVMGR_CELL_PIXELS;
    s_drawPixelY = screenY * ADVMGR_CELL_PIXELS;
    s_drawCell = GetCell(mapX, mapY);

    if (gbAllBlack == 0 &&
        (mapX < 0 || mapY < 0 || mapX >= MAP_WIDTH || mapY >= MAP_HEIGHT)) {
        s_drawStoneTile = -1;
        if (mapX == -1) {
            if (mapY == -1)
                s_drawStoneTile = 16;
            else if (mapY == MAP_HEIGHT)
                s_drawStoneTile = 19;
            else if (mapY >= 0 && mapY < MAP_HEIGHT)
                s_drawStoneTile = (mapY & 3) + 32;
        } else if (mapX == MAP_WIDTH) {
            if (mapY == -1)
                s_drawStoneTile = 17;
            else if (mapY == MAP_HEIGHT)
                s_drawStoneTile = 18;
            else if (mapY >= 0 && mapY < MAP_HEIGHT)
                s_drawStoneTile = (mapY & 3) + 24;
        } else if (mapY == -1) {
            if (mapX >= 0 && mapX < MAP_WIDTH)
                s_drawStoneTile = (mapX & 3) + 20;
        } else if (mapY == MAP_HEIGHT && mapX >= 0 && mapX < MAP_WIDTH) {
            s_drawStoneTile = (mapX & 3) + 28;
        }
        if (s_drawStoneTile == -1)
            s_drawStoneTile = ((mapY + 16) % ADVMGR_CLOUD_VARIANTS) * 4 +
                              (mapX + 16) % ADVMGR_CLOUD_VARIANTS;
        TileToBitmap(m_stoneTiles, s_drawStoneTile, gpWindowManager->m_screen,
                     s_drawPixelX, s_drawPixelY);
    } else {

    if (!(((gbAllBlack == 0 &&
            (giCurWatchPlayerBit & mapExtra[mapY * MAP_WIDTH + mapX]) != 0) ||
           gbDrawingPuzzle != 0))) {
        s_drawCovered = 1;
        if (gbAllBlack != 0)
            s_drawCloudFrame = 0;
        else
            s_drawCloudFrame = GetCloudLookup(mapX, mapY);
        if (s_drawCloudFrame == 0) {
            if (drawMask & ADVMGR_DRAW_CLOUD) {
                TileToBitmap(m_cloudTiles, (mapY + mapX) & 3,
                             gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY);
            }
            return;
        }
        if (s_drawCloudFrame >= 100) {
            s_drawFlipCloud = 1;
            s_drawCloudFrame -= 100;
        } else {
            s_drawFlipCloud = 0;
        }
        if ((s_drawCloudFrame == 1 || s_drawCloudFrame == 5) && (mapX & 1))
            ++s_drawCloudFrame;
        if (s_drawCloudFrame == 3 && (mapY & 1))
            ++s_drawCloudFrame;
    } else {
        s_drawCovered = 0;
    }

    if ((drawMask & ADVMGR_DRAW_CLOUD) != 0 && gbDrawingPuzzle == 0) {
        if (s_drawCovered != 0) {
            if (s_drawFlipCloud != 0) {
                FlipIconToBitmap(m_cloudOverlayIcon, gpWindowManager->m_screen,
                                 s_drawPixelX + 31, s_drawPixelY, s_drawCloudFrame - 1,
                                 0, 0, 0, 0, 0, 0);
            } else {
                IconToBitmap(m_cloudOverlayIcon, gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCloudFrame - 1,
                             0, 0, 0, 0, 0, 0);
            }
        } else if (m_visibilityMapValid &&
                   m_visibilityMap[mapY * MAP_WIDTH + mapX] != 0) {
            if ((m_visibilityMap[mapY * MAP_WIDTH + mapX] & 0x100) != 0) {
                IconToBitmapColorTable(
                    m_objectIcons[17], gpWindowManager->m_screen,
                    s_drawPixelX - 12, s_drawPixelY + 2,
                    (m_visibilityMap[mapY * MAP_WIDTH + mapX] - 1) & 0xff,
                    1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT,
                    0, gColorTableRed, 1);
            } else {
                IconToBitmap(m_objectIcons[17], gpWindowManager->m_screen,
                             s_drawPixelX - 12, s_drawPixelY + 2,
                             (m_visibilityMap[mapY * MAP_WIDTH + mapX] - 1) & 0xff,
                             1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                             ADVMGR_DRAW_CLIP_HEIGHT, 0);
            }
        }
    } else {
        if (drawMask & ADVMGR_DRAW_GROUND) {
            s_drawGroundTile = s_drawCell->m_flags;
            s_drawGroundTile <<= 14;
            s_drawGroundTile |= s_drawCell->m_terrainImageIndex;
            TileToBitmap(m_groundTiles, s_drawGroundTile, gpWindowManager->m_screen,
                         s_drawPixelX, s_drawPixelY);

            if (s_drawCell->m_objectLayerBit0 &&
                (gbDrawingPuzzle == 0 || s_drawCell->m_objectTileset != 56 ||
                 s_drawCell->m_objectIndex != 140) &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->m_objectTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->m_objectIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->m_animatedObject) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->m_objectTileset],
                                     s_drawCell->m_objectIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->m_objectIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->m_extraIndex != 0 &&
                m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->objectLayerBit0 &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objectTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objectIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->animatedObject) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objectTileset],
                                         s_drawExtra->objectIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objectIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->nextIndex != 0 &&
                    m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                else
                    s_drawExtra = 0;
            }

            if (s_drawCell->m_objectLayerBit1 && !s_drawCell->m_objectLayerBit0 &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->m_objectTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->m_objectIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->m_animatedObject) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->m_objectTileset],
                                     s_drawCell->m_objectIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->m_objectIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->m_extraIndex != 0 &&
                m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->objectLayerBit1 && !s_drawExtra->objectLayerBit0 &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objectTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objectIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->animatedObject) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objectTileset],
                                         s_drawExtra->objectIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objectIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->nextIndex != 0 &&
                    m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                else
                    s_drawExtra = 0;
            }
        }

        if (drawMask & ADVMGR_DRAW_OBJECT) {
            if (s_drawCell->m_objectIndex != 0xff && !s_drawCell->m_objectLayerBit0 &&
                !s_drawCell->m_objectLayerBit1 && !s_drawCell->m_unknownObjectFlag &&
                s_drawCell->m_objectTileset != ADVMGR_TILESET_MINE &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->m_objectTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->m_objectIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->m_animatedObject) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->m_objectTileset],
                                     s_drawCell->m_objectIndex)->flags;
                    animFrame = m_updateMaxY % s_drawAnimationLength;
                    if (s_drawCell->m_triggerType == 0xdf) {
                        if (s_drawCell->m_objectMetadata != 0)
                            animFrame = m_updateMaxY % (s_drawAnimationLength - 1);
                        else
                            animFrame = s_drawAnimationLength - 1;
                    }
                    IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 animFrame + s_drawCell->m_objectIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->m_extraIndex != 0 &&
                m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (!s_drawExtra->objectLayerBit0 && !s_drawExtra->objectLayerBit1 && !s_drawExtra->unknownObjectFlag &&
                    s_drawExtra->objectTileset != ADVMGR_TILESET_MINE &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objectTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objectIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->animatedObject) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objectTileset],
                                         s_drawExtra->objectIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objectIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->nextIndex != 0 &&
                    m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                else
                    s_drawExtra = 0;
            }
        }

        if (((drawMask & ADVMGR_DRAW_HERO) ||
             (drawMask & ADVMGR_DRAW_HERO_SHADOW)) &&
            gbDrawingPuzzle == 0) {
            s_drawHasHero = 0;
            s_drawHero = 0;
            if (drawMask & ADVMGR_DRAW_HERO) {
                if (mapX > 0) {
                    s_drawAdjacentCell = GetCell(mapX - 1, mapY);
                    if (s_drawAdjacentCell->m_triggerType == ADVMGR_MONSTER_TRIGGER) {
                        s_drawMine = &gpGame->m_mines[s_drawAdjacentCell->m_objectMetadata];
                        if (s_drawMine->guardianType == 59) {
                            IconToBitmap(m_objectIcons[ADVMGR_MINE_GUARDIAN_ICON_SLOT],
                                         gpWindowManager->m_screen,
                                         s_drawPixelX - 16, s_drawPixelY,
                                         (m_updateMaxY + mapY + mapX) % 15,
                                         1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        } else if (s_drawMine->guardianType != -1) {
                            IconToBitmap(m_objectIcons[ADVMGR_TILESET_BOAT],
                                         gpWindowManager->m_screen,
                                         s_drawPixelX - 32, s_drawPixelY,
                                         s_drawMine->guardianType - 62,
                                         1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                    }
                }

                if (s_drawCell->m_objectTileset == ADVMGR_TILESET_MINE) {
                    if (m_lastQuickViewX == mapX && m_lastQuickViewY == mapY) {
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     (8 - (m_field_0x2ba == 0)) +
                                         s_drawCell->m_objectIndex * 9,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    } else {
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     s_drawCell->m_objectIndex * 9,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawCell->m_objectIndex == 59 || s_drawCell->m_objectIndex == 60)
                            s_drawMonsterFrame = m_viewBounds[mapX & 3] % 6;
                        else
                            s_drawMonsterFrame = monAnimDrawFrame[m_viewBounds[mapX & 3]];
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     s_drawCell->m_objectIndex * 9 + s_drawMonsterFrame + 1,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
            }

            if (s_drawCell->m_triggerType == ADVMGR_BOAT_TRIGGER) {
                s_drawPlayerColor = -1;
                s_drawHeroType = ADVMGR_HERO_TYPE_BOAT;
                s_drawHeroFrame = GetCursorBaseFrame(
                    gpGame->m_boats[s_drawCell->m_objectMetadata].direction);
                s_drawHasHero = 1;
                s_drawHeroYOffset = -10;
            } else {
                s_drawHeroYOffset = 0;
                if (s_drawCell->m_triggerType == ADVMGR_HERO_TRIGGER) {
                    s_drawHero = gpGame->GetHero(s_drawCell->m_objectMetadata);
                    s_drawPlayerColor =
                        gpGame->m_players[s_drawHero->m_owner].m_color;
                    if (s_drawHero->m_eventFlags & 0x80)
                        s_drawHeroType = ADVMGR_HERO_TYPE_BOAT;
                    else
                        s_drawHeroType = s_drawHero->m_cursorType;
                    s_drawHeroFrame = GetCursorBaseFrame(s_drawHero->m_direction);
                    s_drawHasHero = 1;
                    if (s_drawHero->m_eventFlags & 0x80)
                        s_drawHeroYOffset = -10;
                }
            }

            if (s_drawHasHero) {
                if (s_drawHeroFrame & 0x80) {
                    if (drawMask & ADVMGR_DRAW_HERO_SHADOW) {
                        if (m_drawHeroShadows != 0 &&
                            s_drawHeroType != ADVMGR_HERO_TYPE_BOAT) {
                            cursorFrame = s_drawHeroFrame & 0x7f;
                            if (cursorFrame == 51)
                                cursorFrame = 56;
                            if (cursorFrame == 50)
                                cursorFrame = 57;
                            if (cursorFrame == 49)
                                cursorFrame = 58;
                            if (cursorFrame == 47)
                                cursorFrame = 55;
                            if (cursorFrame == 46)
                                cursorFrame = 55;
                            if (cursorFrame >= 9 &&
                                cursorFrame < ADVMGR_HERO_SHADOW_FRAME_END)
                                heroShadowOffset = 50;
                            else
                                heroShadowOffset = 0;
                            IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                                         s_drawPixelX, s_drawPixelY + 31,
                                         cursorFrame + heroShadowOffset,
                                         1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                        if (m_drawHeroShadows != 0 &&
                            s_drawHeroType == ADVMGR_HERO_TYPE_BOAT) {
                            boatFrameIndex = s_drawHeroFrame & 0x7f;
                            if (boatFrameIndex >= 9 &&
                                boatFrameIndex < ADVMGR_HERO_SHADOW_FRAME_END)
                                boatShadowFrameOffset = 36;
                            else
                                boatShadowFrameOffset = 0;
                            IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                                         s_drawPixelX,
                                         s_drawPixelY + 31 + s_drawHeroYOffset,
                                         boatFrameIndex + boatShadowFrameOffset,
                                         1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                    } else {
                        if (s_drawHeroType == ADVMGR_HERO_TYPE_BOAT &&
                            (s_drawCell->m_flags & 4) == 0) {
                            FlipIconToBitmap(m_heroIcons[7], gpWindowManager->m_screen,
                                             s_drawPixelX + 32,
                                             s_drawPixelY + 31 + s_drawHeroYOffset,
                                             s_drawHeroFrame & 0x7f, 1, 0, 0,
                                             ADVMGR_DRAW_CLIP_WIDTH,
                                             ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                        FlipIconToBitmap(m_heroIcons[s_drawHeroType],
                                         gpWindowManager->m_screen,
                                         s_drawPixelX + 32,
                                         s_drawPixelY + 31 + s_drawHeroYOffset,
                                         s_drawHeroFrame & 0x7f, 1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawPlayerColor != -1) {
                            if (s_drawHeroType == ADVMGR_HERO_TYPE_BOAT) {
                                FlipIconToBitmap(m_boatFlagIcons[s_drawPlayerColor],
                                                 gpWindowManager->m_screen,
                                                 s_drawPixelX + 32,
                                                 s_drawPixelY + 31 + s_drawHeroYOffset,
                                                 s_drawHeroFrame & 0x7f,
                                                 1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                            } else {
                                FlipIconToBitmap(m_flagIcons[s_drawPlayerColor],
                                                 gpWindowManager->m_screen,
                                                 s_drawPixelX + 32, s_drawPixelY + 31,
                                                 m_updateMaxY % 8 +
                                                     (s_drawHeroFrame & 0x7f) + 56,
                                                 1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                            }
                        }
                    }
                } else {
                    if (drawMask & ADVMGR_DRAW_HERO_SHADOW) {
                        if (m_drawHeroShadows != 0 &&
                            s_drawHeroType != ADVMGR_HERO_TYPE_BOAT &&
                            (drawMask & ADVMGR_DRAW_HERO_SHADOW)) {
                            IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                                         s_drawPixelX, s_drawPixelY + 31,
                                         s_drawHeroFrame, 1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                        if (m_drawHeroShadows != 0 &&
                            s_drawHeroType == ADVMGR_HERO_TYPE_BOAT) {
                            IconToBitmap(m_boatShadowIcon, gpWindowManager->m_screen,
                                         s_drawPixelX,
                                         s_drawPixelY + 31 + s_drawHeroYOffset,
                                         s_drawHeroFrame, 1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                    } else {
                        if (s_drawHeroType == ADVMGR_HERO_TYPE_BOAT &&
                            (s_drawCell->m_flags & 4) == 0) {
                            IconToBitmap(m_heroIcons[7], gpWindowManager->m_screen,
                                         s_drawPixelX,
                                         s_drawPixelY + 31 + s_drawHeroYOffset,
                                         s_drawHeroFrame, 1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                        IconToBitmap(m_heroIcons[s_drawHeroType], gpWindowManager->m_screen,
                                     s_drawPixelX,
                                     s_drawPixelY + 31 + s_drawHeroYOffset,
                                     s_drawHeroFrame, 1, 0, 0,
                                     ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawPlayerColor != -1) {
                            if (s_drawHeroType == ADVMGR_HERO_TYPE_BOAT) {
                                IconToBitmap(m_boatFlagIcons[s_drawPlayerColor],
                                             gpWindowManager->m_screen,
                                             s_drawPixelX,
                                             s_drawPixelY + 31 + s_drawHeroYOffset,
                                             s_drawHeroFrame & 0x7f,
                                             1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                             ADVMGR_DRAW_CLIP_HEIGHT, 0);
                            } else {
                                IconToBitmap(m_flagIcons[s_drawPlayerColor],
                                             gpWindowManager->m_screen,
                                             s_drawPixelX, s_drawPixelY + 31,
                                             m_updateMaxY % 8 +
                                                 (s_drawHeroFrame & 0x7f) + 56,
                                             1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                             ADVMGR_DRAW_CLIP_HEIGHT, 0);
                            }
                        }
                    }
                }
            }

            if (m_cursorActive != 0 && (s_drawCell->m_flags & 0x40) != 0 &&
                (m_comboHeroDrawn == 0 || (drawMask & ADVMGR_DRAW_HERO_SHADOW)) &&
                m_mapOriginX + 7 == mapX && m_mapOriginY + 7 == mapY) {
                if (drawMask & ADVMGR_DRAW_HERO_SHADOW) {
                    cursorSuppressed = 1;
                } else {
                    DrawCursorShadow();
                    DrawCursor();
                    m_comboHeroDrawn = 1;
                }
            }
        }

        if ((drawMask & ADVMGR_DRAW_OVERLAY) ||
            (drawMask & ADVMGR_DRAW_OVERLAY_TOP)) {
            if ((drawMask & ADVMGR_DRAW_OVERLAY) && s_drawCell->m_objectIndex != 0xff &&
                s_drawCell->m_unknownObjectFlag &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->m_objectTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->m_objectIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->m_animatedObject) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->m_objectTileset],
                                     s_drawCell->m_objectIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->m_objectTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 s_drawCell->m_objectIndex + m_updateMaxY % s_drawAnimationLength + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->m_extraIndex != 0 &&
                m_mapData->Extra(s_drawCell->m_extraIndex)->objectIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->unknownObjectFlag &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objectTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objectIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->animatedObject) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objectTileset],
                                         s_drawExtra->objectIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objectTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     s_drawExtra->objectIndex +
                                         m_updateMaxY % s_drawAnimationLength + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->nextIndex != 0 &&
                    m_mapData->Extra(s_drawExtra->nextIndex)->objectIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                else
                    s_drawExtra = 0;
            }

            if (s_drawCell->m_overlayIndex != 0xff &&
                (((drawMask & ADVMGR_DRAW_OVERLAY) && !s_drawCell->m_drawOverlayOnTop) ||
                 ((drawMask & ADVMGR_DRAW_OVERLAY_TOP) && s_drawCell->m_drawOverlayOnTop)) &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->m_overlayTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->m_overlayTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->m_overlayIndex,
                             s_drawCell->m_overlayTileset == 14, 0, 0,
                             ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->m_animatedOverlay) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->m_overlayTileset],
                                     s_drawCell->m_overlayIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->m_overlayTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->m_overlayIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->m_extraIndex != 0 &&
                m_mapData->Extra(s_drawCell->m_extraIndex)->overlayIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->m_extraIndex);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (((drawMask & ADVMGR_DRAW_OVERLAY) && !s_drawExtra->drawOverlayOnTop) ||
                    ((drawMask & ADVMGR_DRAW_OVERLAY_TOP) && s_drawExtra->drawOverlayOnTop)) {
                    if (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->overlayTileset]) {
                        IconToBitmap(m_objectIcons[s_drawExtra->overlayTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     s_drawExtra->overlayIndex,
                                     s_drawExtra->overlayTileset == 14, 0, 0,
                                     ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawExtra->animatedOverlay) {
                            s_drawAnimationLength =
                                GetIconEntry(m_objectIcons[s_drawExtra->overlayTileset],
                                             s_drawExtra->overlayIndex)->flags;
                            IconToBitmap(m_objectIcons[s_drawExtra->overlayTileset],
                                         gpWindowManager->m_screen,
                                         s_drawPixelX, s_drawPixelY,
                                         m_updateMaxY % s_drawAnimationLength +
                                             s_drawExtra->overlayIndex + 1,
                                         0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                    }
                }
                if (s_drawExtra->nextIndex != 0 &&
                    m_mapData->Extra(s_drawExtra->nextIndex)->overlayIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->nextIndex);
                else
                    s_drawExtra = 0;
            }
        }
    }
    }
}

VA(0x0045e047, 0x93)
class mapCell * advManager::GetCell(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAP_WIDTH || MAP_HEIGHT <= y)
        return m_mapData->Cells();
    else
        return &m_mapData->Row(y)[x];
}

// @match-note
// Complete semantics, 0xb4 frame/slots, radar CFG, and all 122 relocation targets
// agree. The first divergence is commutative operand order at +0x141; the other
// spans are +0x1d3..+0x1eb, +0x234..+0x23d, and +0x47d..+0x488, plus retail
// alignment NOPs. Reversed operands, explicit temporaries, symmetric subscript,
// and pointer-add forms compile to the same order. Revisit after a material
// predecessor/TU-state change.
VA(0x0045e0da, 0x104d)
void advManager::UpdateRadar(int updateScreen, int partial)
{
    int maxXLocal;
    int unusedRadarRowLocal;
    int radarFrameLocal;
    unsigned char radarColorValue = ADVMGR_RADAR_UNSEEN_COLOR;
    float radarScaleState;
    int townXValue;
    int maxYLocal;
    int unusedRadarCoordinateLocal;
    int townYValue;
    int rowRemainderState;
    int unusedRadarDimensionValue;
    int unusedRadarColumnState;
    int mapRow;
    int mapColumnLimit;
    int ownerIndexValue;
    unsigned char *radarPixel;
    mapCell *cellValue;
    int screenColumnIndex;
    unsigned char *screenRowOffset;
    int columnRemainderValue;
    int minYOffset;
    int minXSlot;
    int skipFrameIndex;
    unsigned int objectTilesetLocal;

    // Retail reserves these four unreferenced named-local words above `this`.

    if (partial == 0) {
        minXSlot = 0;
        minYOffset = 0;
        maxXLocal = MAP_WIDTH - 1;
        maxYLocal = MAP_HEIGHT - 1;
    } else {
        minXSlot = m_mapOriginX - ADVMGR_RADAR_PARTIAL_MARGIN;
        minYOffset = m_mapOriginY - ADVMGR_RADAR_PARTIAL_MARGIN;
        maxXLocal = m_mapOriginX + ADVMGR_RADAR_PARTIAL_SPAN;
        maxYLocal = m_mapOriginY + ADVMGR_RADAR_PARTIAL_SPAN;
        if (minXSlot < 0)
            minXSlot = 0;
        if (minYOffset < 0)
            minYOffset = 0;
        if (maxXLocal > MAP_WIDTH - 1)
            maxXLocal = MAP_WIDTH - 1;
        if (maxYLocal > MAP_HEIGHT - 1)
            maxYLocal = MAP_HEIGHT - 1;
    }

    if (gbThisNetHumanPlayer[giCurPlayer] == 0)
        return;

    gpAdvManager->m_openState = 0;
    columnRemainderValue = rowRemainderState = 0;
    switch (MAP_HEIGHT) {
    case ADVMGR_RADAR_MAP_SMALL:
        screenRowOffset = gpWindowManager->m_screen->m_pixels +
            (minYOffset * 4 + ADVMGR_RADAR_TOP) * ADVMGR_RADAR_ROW_GROUPS *
                ADVMGR_RADAR_GROUP_BYTES + ADVMGR_RADAR_LEFT;
        screenColumnIndex = minXSlot * 4;
        break;
    case ADVMGR_RADAR_MAP_MEDIUM:
        screenRowOffset = gpWindowManager->m_screen->m_pixels +
            (minYOffset * 2 + ADVMGR_RADAR_TOP) *
                ADVMGR_RADAR_ROW_GROUPS * ADVMGR_RADAR_GROUP_BYTES +
                ADVMGR_RADAR_LEFT;
        screenColumnIndex = minXSlot * 2;
        break;
    case ADVMGR_RADAR_MAP_LARGE:
        screenRowOffset = gpWindowManager->m_screen->m_pixels +
            (minYOffset + (minYOffset + 2) / 3 + ADVMGR_RADAR_TOP) *
                ADVMGR_RADAR_ROW_GROUPS * ADVMGR_RADAR_GROUP_BYTES +
                ADVMGR_RADAR_LEFT;
        screenColumnIndex = minXSlot + (minXSlot + 2) / 3;
        columnRemainderValue = minXSlot % 3;
        rowRemainderState = minYOffset % 3;
        break;
    default:
        screenRowOffset =
            &gpWindowManager->m_screen->m_pixels[
                (minYOffset + ADVMGR_RADAR_TOP) * ADVMGR_RADAR_ROW_GROUPS *
                ADVMGR_RADAR_GROUP_BYTES] + ADVMGR_RADAR_LEFT;
        screenColumnIndex = minXSlot;
        break;
    }

    for (mapRow = minYOffset; !(maxYLocal < mapRow); ++mapRow) {
        radarPixel = screenRowOffset + screenColumnIndex;
        switch (MAP_HEIGHT) {
        case ADVMGR_RADAR_MAP_SMALL:
            screenRowOffset += ADVMGR_RADAR_SCREEN_PITCH * 4;
            break;
        case ADVMGR_RADAR_MAP_MEDIUM:
            screenRowOffset += ADVMGR_RADAR_SCREEN_PITCH * 2;
            break;
        case ADVMGR_RADAR_MAP_LARGE:
            ++rowRemainderState;
            if (rowRemainderState > 2)
                rowRemainderState = 0;
            if (rowRemainderState != 0)
                screenRowOffset += ADVMGR_RADAR_SCREEN_PITCH;
            else
                screenRowOffset += ADVMGR_RADAR_SCREEN_PITCH * 2;
            break;
        case ADVMGR_RADAR_MAP_XLARGE:
            screenRowOffset += ADVMGR_RADAR_SCREEN_PITCH;
            break;
        }

        for (mapColumnLimit = minXSlot; mapColumnLimit <= maxXLocal; ++mapColumnLimit) {
            if (gbAllBlack != 0 ||
                (giCurPlayerBit & mapExtra[mapRow * MAP_WIDTH + mapColumnLimit]) == 0) {
                radarColorValue = ADVMGR_RADAR_UNSEEN_COLOR;
            } else {
                cellValue = &m_mapData->Row(mapRow)[mapColumnLimit];
                if ((cellValue->m_flags & 0x40) != 0 &&
                    m_mapOriginX + ADVMGR_RADAR_CURRENT_CELL == mapColumnLimit &&
                    m_mapOriginY + ADVMGR_RADAR_CURRENT_CELL == mapRow) {
                    radarColorValue = gOwnerColors[gpGame->m_players[giCurPlayer].m_color];
                } else {
                    if ((cellValue->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) ==
                        ADVMGR_RADAR_TOWN_TRIGGER) {
                        ownerIndexValue = gpGame->m_availableHeroes[cellValue->m_objectMetadata];
                        if (!(giCurPlayer != ownerIndexValue)) {
                            int ownerColorIndex;
                            if (ownerIndexValue >= 0)
                                ownerColorIndex = gpGame->m_players[ownerIndexValue].m_color;
                            else
                                ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                            radarColorValue = gOwnerColors[ownerColorIndex];
                        }
                    } else {
                        objectTilesetLocal = static_cast<unsigned int>(-1);
                        if (cellValue->m_objectIndex != 0xff) {
                            objectTilesetLocal = cellValue->m_objectTileset;
                        } else if (cellValue->m_overlayIndex != 0xff) {
                            objectTilesetLocal = cellValue->m_overlayTileset;
                        }

                        if (cellValue->m_triggerType == ADVMGR_RADAR_TOWN_TILESET_1 ||
                            (objectTilesetLocal == 14 && mapColumnLimit > 0 &&
                             mapColumnLimit < MAP_WIDTH - 1 &&
                             m_mapData->Row(mapRow)[mapColumnLimit - 1].m_triggerType ==
                                 ADVMGR_RADAR_NEIGHBOR_TRIGGER) ||
                            m_mapData->Row(mapRow)[mapColumnLimit + 1].m_triggerType ==
                                ADVMGR_RADAR_NEIGHBOR_TRIGGER) {
                            objectTilesetLocal = ADVMGR_RADAR_TOWN_TILESET_1;
                        }

                        if (objectTilesetLocal == ADVMGR_RADAR_SPECIAL_TILESET &&
                            cellValue->m_triggerType == ADVMGR_RADAR_REEFS_TRIGGER) {
                            radarColorValue = gMapColors[giGroundToTerrain[cellValue->m_terrainImageIndex]] +
                                ADVMGR_RADAR_TERRAIN_SHADE;
                        } else {
                            switch (objectTilesetLocal) {
                            case ADVMGR_RADAR_TOWN_TILESET_1:
                            case ADVMGR_RADAR_TOWN_TILESET_2: {
                                int ownerColorIndex;
                                ownerIndexValue = gpGame->m_townOwners[cellValue->m_objectMetadata];
                                townXValue = gpGame->GetTown(cellValue->m_objectMetadata)->m_x;
                                townYValue = gpGame->GetTown(cellValue->m_objectMetadata)->m_y;
                                if (ownerIndexValue >= 0)
                                    ownerColorIndex = gpGame->m_players[ownerIndexValue].m_color;
                                else
                                    ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                                radarColorValue = gOwnerColors[ownerColorIndex];
                                if (mapRow < townYValue - ADVMGR_RADAR_TOWN_RADIUS ||
                                    townYValue < mapRow ||
                                    mapColumnLimit < townXValue - ADVMGR_RADAR_TOWN_RADIUS ||
                                    mapColumnLimit > townXValue + ADVMGR_RADAR_TOWN_RADIUS)
                                    goto radar_default_object;
                                break;
                            }
                            case 0x16: case 0x17: case 0x18: case 0x19:
                            case 0x1a: case 0x1b: case 0x1f: case 0x20:
                            case 0x21: case 0x22: case 0x2a: case 0x2b:
                            case 0x2c: case 0x31:
                                switch (cellValue->m_triggerType) {
                                case 1: case 0x17: case 0x1d:
                                case 0x81: case 0x97: case 0x9d: {
                                    int ownerColorIndex;
                                    ownerIndexValue = gpGame->m_mineOwners[cellValue->m_objectMetadata];
                                    if (ownerIndexValue >= 0)
                                        ownerColorIndex = gpGame->m_players[ownerIndexValue].m_color;
                                    else
                                        ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                                    radarColorValue = gOwnerColors[ownerColorIndex];
                                    break;
                                }
                                default:
                                    radarColorValue = gMapColors[giGroundToTerrain[cellValue->m_terrainImageIndex]] +
                                        ADVMGR_RADAR_TERRAIN_SHADE;
                                    break;
                                }
                                break;
                            default:
radar_default_object:
                                switch (cellValue->m_triggerType) {
                                case 1: case 0x17: case 0x1d:
                                case 0x81: case 0x97: case 0x9d: {
                                    int ownerColorIndex;
                                    ownerIndexValue = gpGame->m_mineOwners[cellValue->m_objectMetadata];
                                    if (ownerIndexValue >= 0)
                                        ownerColorIndex = gpGame->m_players[ownerIndexValue].m_color;
                                    else
                                        ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                                    radarColorValue = gOwnerColors[ownerColorIndex];
                                    break;
                                }
                                default:
                                    radarColorValue = gMapColors[giGroundToTerrain[cellValue->m_terrainImageIndex]];
                                    break;
                                }
                                break;
                            }
                        }
                    }
                }
            }

            switch (MAP_HEIGHT) {
            case ADVMGR_RADAR_MAP_SMALL:
                memset(radarPixel, radarColorValue, 4);
                memset(radarPixel + ADVMGR_RADAR_SCREEN_PITCH, radarColorValue, 4);
                memset(radarPixel + ADVMGR_RADAR_SCREEN_PITCH * 2, radarColorValue, 4);
                memset(radarPixel + ADVMGR_RADAR_SCREEN_PITCH * 3, radarColorValue, 4);
                radarPixel += 4;
                break;
            case ADVMGR_RADAR_MAP_MEDIUM:
                memset(radarPixel, radarColorValue, 2);
                memset(radarPixel + ADVMGR_RADAR_SCREEN_PITCH, radarColorValue, 2);
                radarPixel += 2;
                break;
            case ADVMGR_RADAR_MAP_LARGE:
                if (columnRemainderValue != 0) {
                    if (rowRemainderState != 0) {
                        radarPixel[0] = radarColorValue;
                        ++radarPixel;
                    } else {
                        radarPixel[0] = radarColorValue;
                        radarPixel[ADVMGR_RADAR_SCREEN_PITCH] = radarColorValue;
                        ++radarPixel;
                    }
                } else if (rowRemainderState != 0) {
                    radarPixel[0] = radarColorValue;
                    radarPixel[1] = radarColorValue;
                    radarPixel += 2;
                } else {
                    radarPixel[0] = radarColorValue;
                    radarPixel[1] = radarColorValue;
                    radarPixel[ADVMGR_RADAR_SCREEN_PITCH] = radarColorValue;
                    radarPixel[ADVMGR_RADAR_SCREEN_PITCH + 1] = radarColorValue;
                    radarPixel += 2;
                }
                ++columnRemainderValue;
                if (columnRemainderValue > 2)
                    columnRemainderValue = 0;
                break;
            case ADVMGR_RADAR_MAP_XLARGE:
                *radarPixel++ = radarColorValue;
                break;
            }
        }
    }

    radarFrameLocal = -1;
    skipFrameIndex = 0;
    if (gbInViewWorld != 0) {
        switch (MAP_HEIGHT) {
        case ADVMGR_RADAR_MAP_SMALL:
            radarScaleState = 4.0f;
            skipFrameIndex = 1;
            break;
        case ADVMGR_RADAR_MAP_MEDIUM:
            radarScaleState = 2.0f;
            if (giViewWorldScale <= 6)
                skipFrameIndex = 1;
            else
                radarFrameLocal = 6;
            break;
        case ADVMGR_RADAR_MAP_LARGE:
            radarScaleState = 1.33f;
            if (giViewWorldScale <= 4)
                skipFrameIndex = 1;
            else if (giViewWorldScale == 6)
                radarFrameLocal = 9;
            else
                radarFrameLocal = 8;
            break;
        default:
            radarScaleState = 1.0f;
            if (giViewWorldScale == 4)
                radarFrameLocal = 7;
            else if (giViewWorldScale == 6)
                radarFrameLocal = 6;
            else
                radarFrameLocal = 4;
            break;
        }
    } else {
        switch (MAP_HEIGHT) {
        case ADVMGR_RADAR_MAP_SMALL:
            radarFrameLocal = 5;
            radarScaleState = 4.0f;
            break;
        case ADVMGR_RADAR_MAP_MEDIUM:
            radarFrameLocal = 3;
            radarScaleState = 2.0f;
            break;
        case ADVMGR_RADAR_MAP_LARGE:
            radarFrameLocal = 2;
            radarScaleState = 1.33f;
            break;
        default:
            radarFrameLocal = 1;
            radarScaleState = 1.0f;
            break;
        }
    }

    if (skipFrameIndex == 0) {
        if (gbInViewWorld != 0) {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<int>(iVWMapOriginX * radarScaleState + 480.0f),
                static_cast<int>(iVWMapOriginY * radarScaleState + 16.0f),
                radarFrameLocal, 181, 0, ADVMGR_RADAR_LEFT, ADVMGR_RADAR_TOP,
                ADVMGR_RADAR_SIZE, ADVMGR_RADAR_SIZE);
        } else {
            m_puzzleIcon->ClipFillToBuffer(
                static_cast<int>(m_mapOriginX * radarScaleState + 480.0f),
                static_cast<int>(m_mapOriginY * radarScaleState + 16.0f),
                radarFrameLocal, 181, 0, ADVMGR_RADAR_LEFT, ADVMGR_RADAR_TOP,
                ADVMGR_RADAR_SIZE, ADVMGR_RADAR_SIZE);
        }
    }

    if (updateScreen != 0) {
        if (partial != 0) {
            gpWindowManager->UpdateScreenRegion(
                static_cast<int>(minXSlot * radarScaleState + 480.0f),
                static_cast<int>(minYOffset * radarScaleState + 16.0f),
                static_cast<int>((maxXLocal - minXSlot + 1) * radarScaleState),
                static_cast<int>((maxYLocal - minYOffset + 1) * radarScaleState));
        } else {
            gpWindowManager->UpdateScreenRegion(ADVMGR_RADAR_LEFT, ADVMGR_RADAR_TOP,
                                                ADVMGR_RADAR_SIZE, ADVMGR_RADAR_SIZE);
        }
    }
}

// @early-stop
// Excluding the 0x7c pointer table and adjacent 0x7b byte lookup, all 1017
// executable instructions, the 0x1fc frame, and all 205 relocation targets match.
// The only displayed operand difference is the delinked byte-table reference
// ([eax + 0x110b] versus [eax]); retail also has two trailing alignment NOPs.
VA(0x0045f127, 0x133e)
void advManager::QuickInfo(int cellX, int cellY)
{
    int quickInfoShowFlag = 1;
    mapCell *currentCell = 0;
    hero *heroLocal = 0;
    int quickInfoScreenX;
    int dialogTopPosition;
    heroWindow *windowLocal;
    unsigned int visitedMaskValue;
    char savedTextLocal[200];
    char guardCaption[200];
    int siteIndexName;
    int siteFrameLocal[2];
    int objectTilesetLocal;
    char uppercaseResult;
    char mapObjectKindValue;
    tag_message message;

    if (gpCurPlayer->CurrentHero() != -1)
        heroLocal = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    else
        heroLocal = 0;

    quickInfoScreenX = cellX * ADVMGR_CELL_PIXELS - 57;
    if (quickInfoScreenX < 30)
        quickInfoScreenX = 30;
    if (quickInfoScreenX + 160 > 464)
        quickInfoScreenX = 304;

    dialogTopPosition = cellY * ADVMGR_CELL_PIXELS - 25;
    if (dialogTopPosition < 16)
        dialogTopPosition = 16;
    if (dialogTopPosition + 96 > 448)
        dialogTopPosition = 352;

    windowLocal = new heroWindow(quickInfoScreenX, dialogTopPosition, "qwikinfo.bin");
    if (windowLocal == 0)
        MemError();
    visitedMaskValue = 0;

    if (m_mapOriginX + cellX < 0 || m_mapOriginX + cellX >= MAP_WIDTH ||
        m_mapOriginY + cellY < 0 || m_mapOriginY + cellY >= MAP_HEIGHT) {
        sprintf(gText, "%s", "Border");
    } else {
        currentCell = GetCell(m_mapOriginX + cellX, m_mapOriginY + cellY);
        if ((giCurPlayerBit &
             (mapExtra + (m_mapOriginY + cellY) * MAP_WIDTH)
                 [m_mapOriginX + cellX]) == 0) {
            sprintf(gText, "%s", "Uncharted Territory");
        } else {

    switch (currentCell->m_triggerType & 0x7f) {
    case ADVMGR_OBJECT_ARTIFACT:
        sprintf(gText, "%s", "Artifact");
        break;
    case ADVMGR_OBJECT_OBELISK:
        if (currentCell->m_triggerType & 0x80) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (gpGame->m_obeliskVisitors[
                         currentCell->m_objectMetadata - ADVMGR_OBELISK_INDEX_BASE] &
                     (1u << giCurPlayer))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_GAZEBO_VISIT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_gazeboVisits & (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_FORT_VISIT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_fortVisits & (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_WITCH_DOCTOR_VISIT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_witchDoctorVisits &
                     (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_MERCENARY_VISIT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_mercenaryCampVisits &
                     (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_STANDING_STONE_ALT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_standingStoneVisits &
                     (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_TREE_ALT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_treeKnowledgeVisits &
                     (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_XANADU_ALT:
        if (heroLocal != 0 && (currentCell->m_triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                    (heroLocal->m_xanaduVisits & (1u << (currentCell->m_objectMetadata & 0x1f)))
                        ? "(already visited)" : "(not visited)");
        } else {
            goto quick_info_default;
        }
        break;
    case ADVMGR_OBJECT_FORT:
        visitedMaskValue = ADVMGR_VISIT_FORT;
        goto quick_info_default;
    case ADVMGR_OBJECT_GAZEBO:
        visitedMaskValue = ADVMGR_VISIT_GAZEBO;
        goto quick_info_default;
    case ADVMGR_OBJECT_MERCENARY_CAMP:
        visitedMaskValue = ADVMGR_VISIT_MERCENARY_CAMP;
        goto quick_info_default;
    case ADVMGR_OBJECT_STANDING_STONES:
        visitedMaskValue = ADVMGR_VISIT_STANDING_STONES;
        goto quick_info_default;
    case ADVMGR_OBJECT_WITCH_DOCTOR_ALT:
        visitedMaskValue = ADVMGR_VISIT_WITCH_DOCTOR;
        goto quick_info_default;
    case ADVMGR_OBJECT_EVENT_SITE:
        visitedMaskValue = ADVMGR_VISIT_EVENT_SITE;
        goto quick_info_default;
    case ADVMGR_OBJECT_EVENT_SITE_ALT:
        visitedMaskValue = ADVMGR_VISIT_XANADU;
        goto quick_info_default;
    case ADVMGR_OBJECT_XANADU:
        visitedMaskValue = ADVMGR_VISIT_TREE_OF_KNOWLEDGE;
        goto quick_info_default;
    case ADVMGR_OBJECT_NONE:
    case 0x13:
    case 0x1c:
    case 0x2c:
    case 0x39:
        sprintf(gText, "%s", gTerrainNames[giGroundToTerrain[currentCell->m_terrainImageIndex]]);
        break;
    case ADVMGR_OBJECT_GUARDED:
        sprintf(gText, "%s", gQuickViewText[currentCell->m_triggerType & 0x7f]);
        goto quick_info_guarded;
    case ADVMGR_OBJECT_MINE:
        if (gpGame->m_mines[currentCell->m_objectMetadata].guardianType != -1) {
            sprintf(gText, "%s %s",
                    gResourceNames[gpGame->m_mines[currentCell->m_objectMetadata].resourceType],
                    "Mine");
quick_info_guarded:
            sprintf(guardCaption, "\n\nguarded by %s %s",
                    GetArmySizeName(
                        gpGame->m_mines[currentCell->m_objectMetadata].guardianCount, 2),
                    gArmyNamesPlural[
                        gpGame->m_mines[currentCell->m_objectMetadata].guardianType]);
            strcat(gText, guardCaption);
        } else {
            sprintf(gText, "%s %s",
                    gResourceNames[gpGame->m_mines[currentCell->m_objectMetadata].resourceType],
                    "Mine");
        }
        break;
    case ADVMGR_OBJECT_RESOURCE:
        sprintf(gText, "%s", gResourceNames[
            (currentCell->m_objectIndex & ADVMGR_RESOURCE_FRAME_PAIR_MASK) / 2]);
        break;
    case ADVMGR_OBJECT_MONSTER:
        if (IsCrystalBallInEffect(m_mapOriginX + cellX, m_mapOriginY + cellY, 8)) {
            sprintf(gText, "%d %s", currentCell->m_objectMetadata & 0xfff,
                    gArmyNamesPlural[currentCell->m_objectIndex]);
        } else {
            sprintf(gText, "%s %s", GetArmySizeName(currentCell->m_objectMetadata & 0xfff, 1),
                    gArmyNamesPlural[currentCell->m_objectIndex]);
        }
        break;
    case ADVMGR_OBJECT_BARRIER:
    case ADVMGR_OBJECT_TENT:
        sprintf(gText, gQuickViewText[currentCell->m_triggerType & 0x7f],
                xBarrierColor[currentCell->m_objectMetadata & 7]);
        uppercaseResult = static_cast<char>(
            toupper(static_cast<int>(static_cast<signed char>(gText[0]))));
        gText[0] = uppercaseResult;
        break;
    case ADVMGR_OBJECT_GENERIC_SITE: {
        mapObjectKindValue = -1;
        if (currentCell->m_objectIndex != 0xff) {
            siteFrameLocal[0] = currentCell->m_objectIndex;
            objectTilesetLocal = currentCell->m_objectTileset;
        } else {
            siteFrameLocal[0] = currentCell->m_overlayIndex;
            objectTilesetLocal = currentCell->m_overlayTileset;
        }
        siteIndexName = -1;
        switch (objectTilesetLocal) {
        case ADVMGR_SITE_TILESET_1:
            if (siteFrameLocal[0] < 0) {
                break;
            } else {
                if (siteFrameLocal[0] < ADVMGR_GENERIC_SITE_1_END)
                    siteIndexName = 0;
                else if (siteFrameLocal[0] < ADVMGR_GENERIC_SITE_2_END) {
                    siteIndexName = 1;
                    visitedMaskValue = ADVMGR_VISIT_GENERIC_HUT;
                }
            }
            break;
        case ADVMGR_SITE_TILESET_2:
            if (siteFrameLocal[0] < 0) {
                break;
            } else {
                if (siteFrameLocal[0] < ADVMGR_GENERIC_ALTAR_END) {
                    siteIndexName = 4;
                    visitedMaskValue = ADVMGR_VISIT_GENERIC_ALTAR;
                } else if (siteFrameLocal[0] < ADVMGR_GENERIC_UNUSED_END) {
                    siteIndexName = -1;
                } else if (siteFrameLocal[0] < ADVMGR_GENERIC_TOWER_END) {
                    siteIndexName = 5;
                    visitedMaskValue = ADVMGR_VISIT_GENERIC_TOWER;
                } else if (siteFrameLocal[0] < ADVMGR_GENERIC_SPRING_END) {
                    siteIndexName = 6;
                    visitedMaskValue = ADVMGR_VISIT_GENERIC_SPRING;
                }
            }
            break;
        case ADVMGR_SITE_TILESET_3:
            if (siteFrameLocal[0] < 0) {
                break;
            } else {
                if (siteFrameLocal[0] < ADVMGR_GENERIC_SITE_3_SPLIT)
                    siteIndexName = 2;
                else if (siteFrameLocal[0] < ADVMGR_GENERIC_SITE_3_END)
                    siteIndexName = 3;
            }
            break;
        }
        if (siteIndexName == -1)
            sprintf(gText, "Unknown");
        else
            sprintf(gText, xGenericSiteNames[siteIndexName]);
        if (heroLocal != 0 && visitedMaskValue != 0) {
            strcat(gText, "\n\n");
            strcat(gText, (heroLocal->m_eventFlags & visitedMaskValue)
                              ? "(already visited)" : "(not visited)");
        }
        break;
    }
    case ADVMGR_OBJECT_RECRUITMENT_SITE: {
        if (currentCell->m_overlayIndex == 0xff) {
            siteFrameLocal[0] = currentCell->m_objectIndex;
            objectTilesetLocal = currentCell->m_objectTileset;
        } else {
            siteFrameLocal[0] = currentCell->m_overlayIndex;
            objectTilesetLocal = currentCell->m_overlayTileset;
        }
        siteIndexName = -1;
        switch (objectTilesetLocal) {
        case ADVMGR_SITE_TILESET_1:
            if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_START) {
                break;
            } else {
                if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_1_END)
                    siteIndexName = 0;
                else if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_2_END)
                    siteIndexName = 1;
                else if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_3_END)
                    siteIndexName = 2;
                else if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_4_END)
                    siteIndexName = 3;
                else if (siteFrameLocal[0] < ADVMGR_RECRUITMENT_5_END)
                    siteIndexName = 4;
            }
            break;
        }
        if (siteIndexName == -1)
            sprintf(gText, "Unknown");
        else
            sprintf(gText, xRecruitmentSiteNames[siteIndexName]);
        break;
    }
    case ADVMGR_OBJECT_REEFS:
        if (currentCell->m_objectTileset == ADVMGR_SITE_TILESET_2) {
            sprintf(gText, "Reefs");
        } else {
            goto quick_info_default;
        }
        break;
    default:
quick_info_default:
    if (visitedMaskValue != 0 && heroLocal != 0) {
        sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->m_triggerType & 0x7f],
                (heroLocal->m_eventFlags & visitedMaskValue)
                    ? "(already visited)" : "(not visited)");
    } else {
        sprintf(gText, "%s", gQuickViewText[currentCell->m_triggerType & 0x7f]);
    }
        break;
    }
        }
    }

quick_info_ready:
    strcpy(savedTextLocal, gText);
    if (giDebugLevel > 0 && currentCell != 0) {
        sprintf(gText, "gi%d obtile%d obi%d ot%d ei%d bl%d %s X%d Y%d",
                currentCell->m_terrainImageIndex, currentCell->m_objectTileset, currentCell->m_objectIndex, currentCell->m_triggerType,
                currentCell->m_objectMetadata, currentCell->m_flags & 8, savedTextLocal,
                m_mapOriginX + cellX, m_mapOriginY + cellY);
    }
    message.type = 0x200;
    message.payload.widget.command = 3;
    message.payload.widget.id = 1;
    message.payload.widget.data.text = gText;
    windowLocal->BroadcastMessage(message);
    gpWindowManager->AddWindow(windowLocal, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(windowLocal);
    delete windowLocal;
}

// @match-note
// Complete semantics and 0x3c frame/explicit slots match; CFG differs only by
// missing five-byte /Ob1 continuations at retail +0x3b (after CurrentHero) and
// +0x1c5 (before the populated locator body). All 25 relocation targets agree.
// Direct m_currentHero access omits +0x3b; CurrentHero() moves it before the
// load. Both loop-bound polarities were checked. Revisit only if the real
// inline accessor/expression context is recovered, or after the SOURCE
// placeholder census reaches zero; do not repeat these spellings beforehand.
VA(0x00460465, 0x348)
void advManager::UpdateHeroLocator(int locatorSlot, int drawWindow,
                                   int updateScreen)
{
    hero *locatorHero8;
    int mobilityFrame18;
    int manaFrame36;
    int selectedHero9;
    int widgetBase7;
    int index3;
    tag_message locatorMessage15;
    int heroId9;

    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;

    if (locatorSlot == -1) {
        selectedHero9 = gpCurPlayer->m_currentHero;
        if (selectedHero9 == ADVMGR_INVALID_HERO)
            return;
        for (index3 = 0; index3 < ADVMGR_LOCATOR_VISIBLE_COUNT; ++index3) {
            if (gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + index3] ==
                selectedHero9)
                locatorSlot = index3;
        }
        if (locatorSlot == -1)
            return;
    }

    widgetBase7 = locatorSlot * ADVMGR_LOCATOR_HERO_WIDGET_STRIDE +
                  ADVMGR_LOCATOR_HERO_WIDGET_BASE;
    locatorMessage15.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    heroId9 = gpCurPlayer->m_heroIds[
        gpCurPlayer->m_heroLocatorPage + locatorSlot];
    locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
    locatorMessage15.payload.widget.id =
        widgetBase7 + ADVMGR_LOCATOR_HERO_BORDER_OFFSET;
    if (gpCurPlayer->m_currentHero == heroId9 &&
        gpCurPlayer->m_currentHero != ADVMGR_INVALID_HERO && !gbAllBlack)
        locatorMessage15.payload.widget.data.value = ADVMGR_LOCATOR_SELECTED_COLOR;
    else
        locatorMessage15.payload.widget.data.value = ADVMGR_LOCATOR_NORMAL_COLOR;
    m_adventureWindow->BroadcastMessage(locatorMessage15);

    if (heroId9 == ADVMGR_INVALID_HERO || gbAllBlack) {
        locatorMessage15.payload.widget.id =
            widgetBase7 + ADVMGR_LOCATOR_HERO_IMAGE_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value =
            locatorSlot + ADVMGR_LOCATOR_HERO_EMPTY_FRAME_BASE;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
        locatorMessage15.payload.widget.data.value = ADVMGR_LOCATOR_HERO_DISABLE_FLAGS;
        for (index3 = 0; index3 <= ADVMGR_LOCATOR_VISIBLE_COUNT - 1; ++index3) {
            locatorMessage15.payload.widget.id = widgetBase7 + index3;
            m_adventureWindow->BroadcastMessage(locatorMessage15);
        }
    } else {
        locatorHero8 = &gpGame->m_heroRecs[heroId9];
        locatorMessage15.payload.widget.id =
            widgetBase7 + ADVMGR_LOCATOR_HERO_IMAGE_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = ADVMGR_LOCATOR_HERO_DEFAULT_FRAME;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
        locatorMessage15.payload.widget.data.value = ADVMGR_LOCATOR_HERO_ENABLE_FLAGS;
        for (index3 = 0; index3 <= ADVMGR_LOCATOR_HERO_WIDGET_STRIDE - 1;
             ++index3) {
            locatorMessage15.payload.widget.id = widgetBase7 + index3;
            m_adventureWindow->BroadcastMessage(locatorMessage15);
        }

        mobilityFrame18 = GetMobilityFrame(locatorHero8->m_remainingMobility);
        locatorMessage15.payload.widget.id =
            widgetBase7 + ADVMGR_LOCATOR_HERO_MOBILITY_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = mobilityFrame18;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        manaFrame36 = GetManaFrame(locatorHero8->m_spellPoints);
        locatorMessage15.payload.widget.id =
            widgetBase7 + ADVMGR_LOCATOR_HERO_MANA_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = manaFrame36;
        m_adventureWindow->BroadcastMessage(locatorMessage15);

        locatorMessage15.payload.widget.id =
            widgetBase7 + ADVMGR_LOCATOR_HERO_PORTRAIT_OFFSET;
        locatorMessage15.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
        locatorMessage15.payload.widget.data.value = locatorHero8->m_portrait;
        m_adventureWindow->BroadcastMessage(locatorMessage15);
    }

    if (drawWindow) {
        m_adventureWindow->DrawWindow(
            ADVMGR_LOCATOR_HERO_DRAW_LEFT, widgetBase7,
            widgetBase7 + ADVMGR_LOCATOR_HERO_BORDER_OFFSET);
        if (updateScreen) {
            gpWindowManager->UpdateScreenRegion(
                ADVMGR_LOCATOR_SCREEN_LEFT,
                locatorSlot * ADVMGR_LOCATOR_SCREEN_ROW_HEIGHT +
                    ADVMGR_LOCATOR_SCREEN_TOP,
                ADVMGR_LOCATOR_SCREEN_WIDTH, ADVMGR_LOCATOR_SCREEN_HEIGHT);
        }
    }
}

VA(0x004607ad, 0x102)
void advManager::UpdateHeroLocators(int drawWindow, int updateScreen)
{
    int locatorSlot;
    double scrollStep;

    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;

    for (locatorSlot = 0; locatorSlot < ADVMGR_LOCATOR_VISIBLE_COUNT;
         ++locatorSlot)
        UpdateHeroLocator(locatorSlot, 0, 0);

    if (gpCurPlayer->m_heroCount < ADVMGR_LOCATOR_PAGE_THRESHOLD) {
        m_scrollLeftButton->m_y = ADVMGR_LOCATOR_SCROLL_NO_PAGES_Y;
    } else {
        scrollStep = static_cast<double>(ADVMGR_LOCATOR_HERO_SCROLL_SPAN) /
                     (gpCurPlayer->m_heroCount -
                      ADVMGR_LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollLeftButton->m_y = static_cast<short>(
            gpCurPlayer->m_heroLocatorPage * scrollStep +
            ADVMGR_LOCATOR_SCROLL_BASE_Y);
    }
    if (drawWindow)
        m_adventureWindow->DrawWindow(updateScreen);
}

VA(0x004608af, 0x2e8)
void advManager::UpdateTownLocators(int drawWindow, int updateScreen)
{
    int locatorSlot;
    tag_message locatorMessage14;
    int townId37;
    double scrollStep;

    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;

    locatorMessage14.type = ADVMGR_LOCATOR_MESSAGE_TYPE;
    for (locatorSlot = 0; locatorSlot < ADVMGR_LOCATOR_VISIBLE_COUNT;
         ++locatorSlot) {
        townId37 = gpCurPlayer->m_townIds[
            gpCurPlayer->m_townLocatorPage + locatorSlot];
        locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_COLOR;
        locatorMessage14.payload.widget.id =
            locatorSlot + ADVMGR_LOCATOR_TOWN_BORDER_BASE;
        if (gpCurPlayer->m_currentTown != ADVMGR_INVALID_CELL &&
            gpCurPlayer->m_currentTown == townId37 && !gbAllBlack)
            locatorMessage14.payload.widget.data.value = ADVMGR_LOCATOR_SELECTED_COLOR;
        else
            locatorMessage14.payload.widget.data.value = ADVMGR_LOCATOR_NORMAL_COLOR;
        m_adventureWindow->BroadcastMessage(locatorMessage14);

        locatorMessage14.payload.widget.id =
            locatorSlot + ADVMGR_LOCATOR_TOWN_IMAGE_BASE;
        if (townId37 == ADVMGR_INVALID_CELL || gbAllBlack) {
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            locatorMessage14.payload.widget.data.value =
                locatorSlot + ADVMGR_LOCATOR_TOWN_EMPTY_FRAME_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command =
                ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            locatorMessage14.payload.widget.data.value =
                ADVMGR_LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command =
                ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            locatorMessage14.payload.widget.data.value =
                ADVMGR_LOCATOR_TOWN_DISABLE_FLAGS;
            locatorMessage14.payload.widget.id =
                locatorSlot + ADVMGR_LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
        } else {
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            locatorMessage14.payload.widget.data.value =
                ADVMGR_LOCATOR_TOWN_ENABLE_FLAGS;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
            locatorMessage14.payload.widget.command = ADVMGR_LOCATOR_COMMAND_SET_FRAME;
            locatorMessage14.payload.widget.data.value =
                gpGame->GetTown(townId37)->m_type +
                ADVMGR_LOCATOR_TOWN_TYPE_FRAME_BASE;
            if (!(gpGame->GetTown(townId37)->m_buildings &
                  TOWN_BUILDING_CASTLE))
                locatorMessage14.payload.widget.data.value +=
                    ADVMGR_LOCATOR_TOWN_VILLAGE_FRAME_OFFSET;
            m_adventureWindow->BroadcastMessage(locatorMessage14);

            if (BitTest(gpGame->m_knownTowns, townId37))
                locatorMessage14.payload.widget.command =
                    ADVMGR_LOCATOR_COMMAND_SET_FLAGS;
            else
                locatorMessage14.payload.widget.command =
                    ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS;
            locatorMessage14.payload.widget.data.value =
                ADVMGR_LOCATOR_TOWN_DISABLE_FLAGS;
            locatorMessage14.payload.widget.id =
                locatorSlot + ADVMGR_LOCATOR_TOWN_FLAG_BASE;
            m_adventureWindow->BroadcastMessage(locatorMessage14);
        }
    }

    if (gpCurPlayer->m_townCount < ADVMGR_LOCATOR_PAGE_THRESHOLD) {
        m_scrollRightButton->m_y = ADVMGR_LOCATOR_SCROLL_NO_PAGES_Y;
    } else {
        scrollStep = static_cast<double>(ADVMGR_LOCATOR_TOWN_SCROLL_SPAN) /
                     (gpCurPlayer->m_townCount -
                      ADVMGR_LOCATOR_PAGE_DENOMINATOR_OFFSET);
        m_scrollRightButton->m_y = static_cast<short>(
            gpCurPlayer->m_townLocatorPage * scrollStep +
            ADVMGR_LOCATOR_SCROLL_BASE_Y);
    }
    if (drawWindow)
        m_adventureWindow->DrawWindow(updateScreen);
}

// @match-note
// Complete semantics, 0x0c frame/slots, switch order, CFG, and all 24
// relocation targets agree. The first and only instruction divergence is the
// missing retail jmp $+0 at +0x12d after UpdBottomViewEnemyTurn. An explicit
// enemy-path goto emits two extra continuations; both expiry-test polarities
// were checked. Revisit only after the SOURCE placeholder census reaches zero.
VA(0x00460b97, 0x1cc)
void advManager::UpdBottomView(int forceUpdate, int drawWindow,
                               int updateScreen)
{
    int updated;

    updated = 0;
    gbForceUpdate = forceUpdate;
    if (giBottomViewOverride == ADVMGR_BOTTOM_VIEW_OVERRIDE_DISABLED)
        return;

    if (giBottomViewOverride > 0) {
        if (KBTickCount() > giBottomViewOverrideEndTime) {
            giBottomViewOverride = ADVMGR_BOTTOM_VIEW_NONE;
        } else {
            switch (giBottomViewOverride) {
            case ADVMGR_BOTTOM_VIEW_NEW_TURN:
                updated = UpdBottomViewNewTurn();
                break;
            case ADVMGR_BOTTOM_VIEW_KINGDOM:
                updated = UpdBottomViewKingdom();
                break;
            case ADVMGR_BOTTOM_VIEW_RESOURCE:
                updated = UpdBottomViewResMsg();
                break;
            }
            goto update_bottom_view;
        }
    }

    if (!gbThisNetHumanPlayer[giCurPlayer] || gbAllBlack ||
        gpGame->m_players[giCurPlayer].m_color != gpCurPlayer->m_color) {
        updated = UpdBottomViewEnemyTurn();
    } else if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO) {
        updated = UpdBottomViewKingdom();
    } else {
        updated = UpdBottomViewHero();
    }

update_bottom_view:
    if (updated && drawWindow) {
        m_adventureWindow->DrawWindow(
            ADVMGR_BOTTOM_VIEW_DRAW_LEFT, ADVMGR_BOTTOM_VIEW_DRAW_TOP,
            ADVMGR_BOTTOM_VIEW_DRAW_BOTTOM);
        if (updateScreen) {
            gpWindowManager->UpdateScreenRegion(
                ADVMGR_BOTTOM_VIEW_PANEL_X, ADVMGR_BOTTOM_VIEW_PANEL_Y,
                ADVMGR_BOTTOM_VIEW_PANEL_WIDTH,
                ADVMGR_BOTTOM_VIEW_PANEL_HEIGHT);
        }
    }
    forceUpdate = gbForceUpdate;
}

VA(0x00460d63, 0x132)
void advManager::ClearBottomView(void)
{
    int widgetIndex;

    if (iCurBottomView == ADVMGR_BOTTOM_VIEW_NONE)
        return;

    for (widgetIndex = 0; widgetIndex < ADVMGR_BOTTOM_VIEW_WIDGET_CAPACITY;
         ++widgetIndex) {
        if (m_bottomViewPrimaryWidgets[widgetIndex] != 0) {
            m_adventureWindow->RemoveWidget(
                m_bottomViewPrimaryWidgets[widgetIndex]);
            delete m_bottomViewPrimaryWidgets[widgetIndex];
        }
        if (m_bottomViewSecondaryWidgets[widgetIndex] != 0) {
            m_adventureWindow->RemoveWidget(
                m_bottomViewSecondaryWidgets[widgetIndex]);
            delete m_bottomViewSecondaryWidgets[widgetIndex];
        }
        m_bottomViewPrimaryWidgets[widgetIndex] = 0;
        m_bottomViewSecondaryWidgets[widgetIndex] = 0;
    }
    iCurBottomViewEnemy = ADVMGR_BOTTOM_VIEW_NO_ENEMY;
    iCurBottomView = ADVMGR_BOTTOM_VIEW_NONE;
    iLastAnimFrame = ADVMGR_BOTTOM_VIEW_NO_ANIMATION;
}

// @match-note
// The 0x38 frame/slots, complete CFG, exact size, and all 73 relocation targets
// agree. The first divergence is commutative global comparison order at
// +0x29e..+0x2c6; later residuals are the moved GetPlayerColor /Ob1 continuation
// at +0x2f5..+0x320 and comparison order at +0x3ef..+0x418. Both comparison
// orders compile identically, while direct member access only moves the inline
// continuation. Revisit after a material TU-state change or in the last-mile phase.
VA(0x00460e95, 0x51b)
int advManager::UpdBottomViewEnemyTurn(void)
{
    int updated;
    tag_message message;

    updated = 0;
    message.type = ADVMGR_ENEMY_TURN_MESSAGE_TYPE;
    if (iCurBottomView != ADVMGR_ENEMY_TURN_VIEW_ID) {
        updated = 1;
        gbForceUpdate = 1;
        ClearBottomView();
        iCurBottomView = ADVMGR_ENEMY_TURN_VIEW_ID;

        m_bottomViewBackground = new iconWidget(
            ADVMGR_ENEMY_TURN_BACKGROUND_X, ADVMGR_ENEMY_TURN_BACKGROUND_Y,
            ADVMGR_ENEMY_TURN_BACKGROUND_WIDTH, ADVMGR_ENEMY_TURN_BACKGROUND_HEIGHT,
            "stonback.icn", 0, 0, ADVMGR_ENEMY_TURN_BACKGROUND_ID,
            ADVMGR_ENEMY_TURN_WIDGET_FLAGS, 1);
        if (m_bottomViewBackground == 0)
            MemError();
        m_adventureWindow->AddWidget(
            m_bottomViewBackground, ADVMGR_ENEMY_TURN_BACKGROUND_Z);

        m_bottomViewHourglassBackground = new iconWidget(
            ADVMGR_ENEMY_TURN_HOURGLASS_X, ADVMGR_ENEMY_TURN_HOURGLASS_Y,
            ADVMGR_ENEMY_TURN_HOURGLASS_WIDTH, ADVMGR_ENEMY_TURN_HOURGLASS_HEIGHT,
            "hourglas.icn", 0, 0, ADVMGR_ENEMY_TURN_HOURGLASS_ID,
            ADVMGR_ENEMY_TURN_WIDGET_FLAGS, 1);
        if (m_bottomViewHourglassBackground == 0)
            MemError();
        m_adventureWindow->AddWidget(
            m_bottomViewHourglassBackground, ADVMGR_ENEMY_TURN_HOURGLASS_Z);
    }

    if (gbForceUpdate ||
        KBTickCount() - iLastSandAnimTime > ADVMGR_ENEMY_TURN_ANIMATION_DELAY) {
        iLastSandAnimTime = KBTickCount();
        iLastAnimFrame = m_updateMaxX;
        if (KBTickCount() - iLastNewSandAnimTime >
            ADVMGR_ENEMY_TURN_ANIMATION_DELAY) {
            iLastNewSandAnimTime = KBTickCount();
            ++iSandAnim;
            if (iSandAnim >= ADVMGR_ENEMY_TURN_SAND_FRAME_LIMIT)
                iSandAnim = ADVMGR_ENEMY_TURN_SAND_RESTART_FRAME;
            updated = 1;

            if (m_bottomViewIcons[1] != 0) {
                message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
                message.payload.widget.id = ADVMGR_ENEMY_TURN_SAND_ID;
                message.payload.widget.data.value = iSandAnim + ADVMGR_ENEMY_TURN_SAND_FRAME_OFFSET;
                m_adventureWindow->BroadcastMessage(message);
            } else {
                m_bottomViewIcons[1] = new iconWidget(
                    ADVMGR_ENEMY_TURN_ANIMATION_X, ADVMGR_ENEMY_TURN_ANIMATION_Y,
                    ADVMGR_ENEMY_TURN_ANIMATION_WIDTH,
                    ADVMGR_ENEMY_TURN_ANIMATION_HEIGHT, "hourglas.icn",
                    iSandAnim + ADVMGR_ENEMY_TURN_SAND_FRAME_OFFSET, 0,
                    ADVMGR_ENEMY_TURN_SAND_ID, ADVMGR_ENEMY_TURN_WIDGET_FLAGS, 1);
                if (m_bottomViewIcons[1] == 0)
                    MemError();
                m_adventureWindow->AddWidget(
                    m_bottomViewIcons[1], ADVMGR_ENEMY_TURN_SAND_Z);
            }
        }
    }

    if (gbForceUpdate || iCurBottomViewEnemy != giCurPlayer) {
        updated = 1;
        iCurBottomViewEnemy = giCurPlayer;
        if (iCurBottomViewEnemy != giCurPlayer)
            iCurHourGlassPhase = 0;
        if (m_bottomViewIcons[0] != 0) {
            message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.payload.widget.id = ADVMGR_ENEMY_TURN_CREST_ID;
            message.payload.widget.data.value =
                gpGame->GetPlayerColor(static_cast<char>(giCurPlayer));
            m_adventureWindow->BroadcastMessage(message);
        } else {
            m_bottomViewIcons[0] = new iconWidget(
                ADVMGR_ENEMY_TURN_CREST_X, ADVMGR_ENEMY_TURN_ANIMATION_Y,
                ADVMGR_ENEMY_TURN_ANIMATION_WIDTH,
                ADVMGR_ENEMY_TURN_ANIMATION_HEIGHT, "brcrest.icn",
                gpGame->GetPlayerColor(static_cast<char>(giCurPlayer)), 0,
                ADVMGR_ENEMY_TURN_CREST_ID, ADVMGR_ENEMY_TURN_WIDGET_FLAGS, 1);
            if (m_bottomViewIcons[0] == 0)
                MemError();
            m_adventureWindow->AddWidget(
                m_bottomViewIcons[0], ADVMGR_ENEMY_TURN_CREST_Z);
        }
    }

    if (gbForceUpdate || iLastHourGlassPhase > iCurHourGlassPhase ||
        iLastHourGlassPhase < 0 ||
        (iCurHourGlassPhase > iLastHourGlassPhase &&
         KBTickCount() - giLastHourGlassUpdateTime >=
             ADVMGR_ENEMY_TURN_PHASE_DELAY)) {
        updated = 1;
        iLastHourGlassPhase = iCurHourGlassPhase;
        giLastHourGlassUpdateTime = KBTickCount();
        if (m_bottomViewIcons[2] != 0) {
            message.payload.widget.command = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.payload.widget.id = ADVMGR_ENEMY_TURN_PHASE_ID;
            message.payload.widget.data.value =
                iCurHourGlassPhase + ADVMGR_ENEMY_TURN_PHASE_FRAME_OFFSET;
            m_adventureWindow->BroadcastMessage(message);
        } else {
            m_bottomViewIcons[2] = new iconWidget(
                ADVMGR_ENEMY_TURN_ANIMATION_X, ADVMGR_ENEMY_TURN_ANIMATION_Y,
                ADVMGR_ENEMY_TURN_ANIMATION_WIDTH,
                ADVMGR_ENEMY_TURN_ANIMATION_HEIGHT, "hourglas.icn",
                iCurHourGlassPhase + ADVMGR_ENEMY_TURN_PHASE_FRAME_OFFSET, 0,
                ADVMGR_ENEMY_TURN_PHASE_ID, ADVMGR_ENEMY_TURN_WIDGET_FLAGS, 1);
            if (m_bottomViewIcons[2] == 0)
                MemError();
            m_adventureWindow->AddWidget(
                m_bottomViewIcons[2], ADVMGR_ENEMY_TURN_PHASE_Z);
        }
    }
    return updated;
}

// @match-note
// Complete semantics/CFG and all 45 relocation targets agree. The first byte
// divergence is the frame allocation at +0x03: retail uses 0x24, ours 0x20.
// Retail slots are dateIconFrame=-0x4, unused=-0x8, dayText=-0xc,
// weekText=-0x10; ours are weekText=-0x4, dayText=-0x8,
// dateIconFrame=-0xc. The decompile/local-use audit found no real source local
// for -0x8, and declaration ordering cannot create it without fake padding.
// Revisit only if later type/predecessor recovery explains the word, or after
// the SOURCE placeholder census reaches zero.
VA(0x004613b0, 0x366)
int advManager::UpdBottomViewNewTurn(void)
{
    int dateIconFrame;
    char *weekText;
    char *dayText;

    dateIconFrame = 0;
    if (!gbForceUpdate && iCurBottomView == ADVMGR_BOTTOM_VIEW_NEW_TURN)
        return 0;

    ClearBottomView();
    iCurBottomView = ADVMGR_BOTTOM_VIEW_NEW_TURN;
    if (gpGame->m_day == ADVMGR_NEW_TURN_FIRST_DAY &&
        (gpGame->m_month != ADVMGR_NEW_TURN_FIRST_DAY ||
         gpGame->m_week != ADVMGR_NEW_TURN_FIRST_DAY ||
         gpGame->m_day != ADVMGR_NEW_TURN_FIRST_DAY))
        dateIconFrame = gpGame->m_week;

    m_bottomViewBackground = new iconWidget(
        ADVMGR_BOTTOM_VIEW_PANEL_X, ADVMGR_BOTTOM_VIEW_PANEL_Y,
        ADVMGR_BOTTOM_VIEW_BACKGROUND_WIDTH, ADVMGR_BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn", 0, 0, ADVMGR_BOTTOM_VIEW_BACKGROUND_ID,
        ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
    if (m_bottomViewBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    m_bottomViewHourglassBackground = new iconWidget(
        ADVMGR_NEW_TURN_DATE_ICON_X, ADVMGR_NEW_TURN_DATE_ICON_Y,
        ADVMGR_NEW_TURN_DATE_ICON_WIDTH, ADVMGR_NEW_TURN_DATE_ICON_HEIGHT,
        "sunmoon.icn", dateIconFrame, 0, ADVMGR_BOTTOM_VIEW_FOREGROUND_ID,
        ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
    if (m_bottomViewHourglassBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

    weekText = static_cast<char *>(BaseAlloc(
        ADVMGR_BOTTOM_VIEW_TEXT_BUFFER_SIZE, ADVMGR_SOURCE_FILE,
        ADVMGR_NEW_TURN_LINE + ADVMGR_NEW_TURN_WEEK_ALLOC_LINE_OFFSET));
    sprintf(weekText, "%s: %d  %s: %d", "Month", gpGame->m_month,
            "Week", gpGame->m_week);
    m_bottomViewAllTexts[0] = new textWidget(
        ADVMGR_NEW_TURN_DATE_TEXT_X, ADVMGR_NEW_TURN_WEEK_TEXT_Y,
        ADVMGR_NEW_TURN_DATE_TEXT_WIDTH, ADVMGR_NEW_TURN_WEEK_TEXT_HEIGHT,
        weekText, "smalfont.fnt", 1, ADVMGR_BOTTOM_VIEW_TEXT_ID,
        ADVMGR_BOTTOM_VIEW_TEXT_FLAGS, 1);
    if (m_bottomViewAllTexts[0] == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    dayText = static_cast<char *>(BaseAlloc(
        ADVMGR_BOTTOM_VIEW_TEXT_BUFFER_SIZE, ADVMGR_SOURCE_FILE,
        ADVMGR_NEW_TURN_LINE + ADVMGR_NEW_TURN_DAY_ALLOC_LINE_OFFSET));
    sprintf(dayText, "%s: %d", "Day", gpGame->m_day);
    m_bottomViewAllTexts[0] = new textWidget(
        ADVMGR_NEW_TURN_DATE_TEXT_X, ADVMGR_NEW_TURN_DAY_TEXT_Y,
        ADVMGR_NEW_TURN_DATE_TEXT_WIDTH, ADVMGR_NEW_TURN_DAY_TEXT_HEIGHT,
        dayText, "bigfont.fnt", 1, ADVMGR_BOTTOM_VIEW_TEXT_ID,
        ADVMGR_BOTTOM_VIEW_TEXT_FLAGS, 1);
    if (m_bottomViewAllTexts[0] == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);
    return 1;
}

VA(0x00461716, 0x35f)
int advManager::UpdBottomViewResMsg(void)
{
    int iconWidth6;
    int iconHeight11;
    int textY19;
    int lineCount10;
    char *messageText2;
    char *resourceCountText6;

    if (!gbForceUpdate && iCurBottomView == ADVMGR_BOTTOM_VIEW_RESOURCE)
        return 0;

    ClearBottomView();
    iCurBottomView = ADVMGR_BOTTOM_VIEW_RESOURCE;
    m_bottomViewBackground = new iconWidget(
        ADVMGR_BOTTOM_VIEW_PANEL_X, ADVMGR_BOTTOM_VIEW_PANEL_Y,
        ADVMGR_BOTTOM_VIEW_BACKGROUND_WIDTH, ADVMGR_BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn", 0, 0, ADVMGR_BOTTOM_VIEW_BACKGROUND_ID,
        ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
    if (m_bottomViewBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    textY19 = 0;
    if (giBottomViewResource < 0) {
        textY19 = ADVMGR_RESOURCE_VIEW_MULTILINE_HEIGHT;
        lineCount10 = smallFont->LineLength(
            gcBottomViewText, ADVMGR_BOTTOM_VIEW_PANEL_WIDTH);
        textY19 -= lineCount10 * ADVMGR_RESOURCE_VIEW_LINE_HEIGHT;
    }
    messageText2 = static_cast<char *>(BaseAlloc(
        strlen(gcBottomViewText) + 1, ADVMGR_SOURCE_FILE,
        ADVMGR_RESOURCE_VIEW_LINE +
            ADVMGR_RESOURCE_VIEW_MESSAGE_ALLOC_LINE_OFFSET));
    sprintf(messageText2, gcBottomViewText);
    m_bottomViewAllTexts[0] = new textWidget(
        ADVMGR_BOTTOM_VIEW_PANEL_X,
        textY19 + ADVMGR_RESOURCE_VIEW_TEXT_BASE_Y,
        ADVMGR_BOTTOM_VIEW_PANEL_WIDTH, ADVMGR_RESOURCE_VIEW_TEXT_HEIGHT,
        messageText2, "smalfont.fnt", 1, ADVMGR_BOTTOM_VIEW_TEXT_ID,
        ADVMGR_BOTTOM_VIEW_TEXT_FLAGS, 1);
    if (m_bottomViewAllTexts[0] == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewAllTexts[0], -1);

    if (giBottomViewResource >= 0) {
        if (giBottomViewResource == ADVMGR_RESOURCE_VIEW_GOLD) {
            iconWidth6 = ADVMGR_RESOURCE_VIEW_GOLD_WIDTH;
            iconHeight11 = ADVMGR_RESOURCE_VIEW_GOLD_HEIGHT;
        } else {
            iconWidth6 = ADVMGR_RESOURCE_VIEW_ICON_WIDTH;
            iconHeight11 = ADVMGR_RESOURCE_VIEW_ICON_HEIGHT;
        }
        m_bottomViewHourglassBackground = new iconWidget(
            (ADVMGR_BOTTOM_VIEW_PANEL_WIDTH - iconWidth6) / 2 +
                ADVMGR_BOTTOM_VIEW_PANEL_X,
            ADVMGR_RESOURCE_VIEW_ICON_BOTTOM - iconHeight11 -
                ADVMGR_RESOURCE_VIEW_ICON_BOTTOM_PADDING,
            iconWidth6, iconHeight11, "resource.icn", giBottomViewResource, 0,
            ADVMGR_BOTTOM_VIEW_FOREGROUND_ID,
            ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
        if (m_bottomViewHourglassBackground == 0)
            MemError();
        m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

        resourceCountText6 = static_cast<char *>(BaseAlloc(
            ADVMGR_BOTTOM_VIEW_COUNT_BUFFER_SIZE, ADVMGR_SOURCE_FILE,
            ADVMGR_RESOURCE_VIEW_LINE +
                ADVMGR_RESOURCE_VIEW_COUNT_ALLOC_LINE_OFFSET));
        sprintf(resourceCountText6, "%d", giBottomViewResourceQty);
        m_bottomViewAllTexts[1] = new textWidget(
            ADVMGR_RESOURCE_VIEW_COUNT_X, ADVMGR_RESOURCE_VIEW_COUNT_Y,
            ADVMGR_RESOURCE_VIEW_COUNT_WIDTH,
            ADVMGR_RESOURCE_VIEW_COUNT_HEIGHT, resourceCountText6,
            "smalfont.fnt", 1, ADVMGR_BOTTOM_VIEW_TEXT_ID_2,
            ADVMGR_BOTTOM_VIEW_TEXT_FLAGS, 1);
        if (m_bottomViewAllTexts[1] == 0)
            MemError();
        m_adventureWindow->AddWidget(m_bottomViewAllTexts[1], -1);
    }
    return 1;
}

VA(0x00461a75, 0x363)
int advManager::UpdBottomViewKingdom(void)
{
    int villageCount37;
    int index11;
    int castleCount12;
    signed char textY5[ADVMGR_KINGDOM_VIEW_ENTRY_COUNT];
    unsigned char textX[ADVMGR_KINGDOM_VIEW_ENTRY_COUNT];
    char *countText14[ADVMGR_KINGDOM_VIEW_ENTRY_COUNT];

    if (!gbForceUpdate && iCurBottomView == ADVMGR_BOTTOM_VIEW_KINGDOM)
        return 0;

    ClearBottomView();
    iCurBottomView = ADVMGR_BOTTOM_VIEW_KINGDOM;
    textY5[0] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[1] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[2] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[3] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[4] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[5] = ADVMGR_KINGDOM_VIEW_RESOURCE_TEXT_Y;
    textY5[6] = ADVMGR_KINGDOM_VIEW_TOWN_TEXT_Y;
    textY5[7] = ADVMGR_KINGDOM_VIEW_TOWN_TEXT_Y;
    textY5[8] = ADVMGR_KINGDOM_VIEW_TOWN_TEXT_Y;
    textX[0] = ADVMGR_KINGDOM_VIEW_WOOD_TEXT_X;
    textX[1] = ADVMGR_KINGDOM_VIEW_MERCURY_TEXT_X;
    textX[2] = ADVMGR_KINGDOM_VIEW_ORE_TEXT_X;
    textX[3] = ADVMGR_KINGDOM_VIEW_SULFUR_TEXT_X;
    textX[4] = ADVMGR_KINGDOM_VIEW_CRYSTAL_TEXT_X;
    textX[5] = ADVMGR_KINGDOM_VIEW_GEMS_TEXT_X;
    textX[6] = ADVMGR_KINGDOM_VIEW_GOLD_TEXT_X;
    textX[7] = ADVMGR_KINGDOM_VIEW_CASTLE_TEXT_X;
    textX[8] = ADVMGR_KINGDOM_VIEW_VILLAGE_TEXT_X;
    villageCount37 = 0;
    castleCount12 = 0;

    m_bottomViewBackground = new iconWidget(
        ADVMGR_BOTTOM_VIEW_PANEL_X, ADVMGR_BOTTOM_VIEW_PANEL_Y,
        ADVMGR_BOTTOM_VIEW_BACKGROUND_WIDTH, ADVMGR_BOTTOM_VIEW_PANEL_HEIGHT,
        "stonback.icn", 0, 0, ADVMGR_BOTTOM_VIEW_BACKGROUND_ID,
        ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
    if (m_bottomViewBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    m_bottomViewHourglassBackground = new iconWidget(
        ADVMGR_KINGDOM_VIEW_ICON_X, ADVMGR_KINGDOM_VIEW_ICON_Y,
        ADVMGR_BOTTOM_VIEW_PANEL_WIDTH, ADVMGR_BOTTOM_VIEW_PANEL_HEIGHT,
        "ressmall.icn", 0, 0, ADVMGR_BOTTOM_VIEW_FOREGROUND_ID,
        ADVMGR_BOTTOM_VIEW_WIDGET_FLAGS, 1);
    if (m_bottomViewHourglassBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewHourglassBackground, -1);

    for (index11 = 0; index11 < gpCurPlayer->m_townCount; ++index11) {
        if (gpGame->m_castleRecs[gpCurPlayer->m_townIds[index11]].m_buildings &
            TOWN_BUILDING_CASTLE)
            ++castleCount12;
        else
            ++villageCount37;
    }

    for (index11 = 0; index11 < ADVMGR_KINGDOM_VIEW_ENTRY_COUNT; ++index11) {
        countText14[index11] = static_cast<char *>(BaseAlloc(
            ADVMGR_BOTTOM_VIEW_COUNT_BUFFER_SIZE, ADVMGR_SOURCE_FILE,
            ADVMGR_KINGDOM_VIEW_LINE +
                ADVMGR_KINGDOM_VIEW_COUNT_ALLOC_LINE_OFFSET));
        if (index11 < ADVMGR_KINGDOM_VIEW_RESOURCE_COUNT)
            sprintf(countText14[index11], "%d", gpCurPlayer->m_resources[index11]);
        else if (index11 == ADVMGR_KINGDOM_VIEW_CASTLE_ENTRY)
            sprintf(countText14[index11], "%d", castleCount12);
        else
            sprintf(countText14[index11], "%d", villageCount37);

        m_bottomViewAllTexts[index11] = new textWidget(
            textX[index11] + ADVMGR_KINGDOM_VIEW_TEXT_X_BASE,
            textY5[index11] + ADVMGR_KINGDOM_VIEW_TEXT_Y_BASE,
            ADVMGR_KINGDOM_VIEW_TEXT_WIDTH,
            ADVMGR_KINGDOM_VIEW_TEXT_HEIGHT, countText14[index11],
            "smalfont.fnt", 1, index11 + ADVMGR_BOTTOM_VIEW_TEXT_ID,
            ADVMGR_BOTTOM_VIEW_TEXT_FLAGS, 1);
        if (m_bottomViewAllTexts[index11] == 0)
            MemError();
        m_adventureWindow->AddWidget(m_bottomViewAllTexts[index11], -1);
    }
    return 1;
}

// @early-stop
// Exact bytes and all 36 relocation targets.
VA(0x00461dd8, 0x583)
int advManager::UpdBottomViewHero(void)
{
    char *armyCountLabelsResult[ADVMGR_BOTTOM_HERO_ARMY_SLOTS];
    icon *monsterIconsLocal;
    int occupiedSlotsLocal;
    hero *targetHero;
    int armySlot;
    int displayIndexData;
    int creature;
    IconEntry *iconEntryValue;
    int groupWidthRef;
    int layoutIndexIndex;
    int iconX;
    int iconY;
    int labelY;
    int labelWidthCount;
    int labelX;
    int creatureBoundsLocal[2];

    if (!gbForceUpdate && iCurBottomView == ADVMGR_BOTTOM_HERO_VIEW_ID)
        return 0;

    ClearBottomView();
    iCurBottomView = ADVMGR_BOTTOM_HERO_VIEW_ID;
    targetHero = gpGame->GetHero(gpCurPlayer->CurrentHero());
    occupiedSlotsLocal = 0;

    m_bottomViewBackground = new iconWidget(
        ADVMGR_BOTTOM_HERO_PANEL_X, ADVMGR_BOTTOM_HERO_PANEL_Y,
        ADVMGR_BOTTOM_HERO_PANEL_WIDTH, ADVMGR_BOTTOM_HERO_PANEL_HEIGHT,
        "stonback.icn", 0, 0, ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE,
        ADVMGR_BOTTOM_HERO_WIDGET_FLAGS, 1);
    if (m_bottomViewBackground == 0)
        MemError();
    m_adventureWindow->AddWidget(m_bottomViewBackground, -1);

    for (armySlot = 0; armySlot < ADVMGR_BOTTOM_HERO_ARMY_SLOTS; ++armySlot) {
        if (targetHero->m_army.m_creatureTypes[armySlot] != ADVMGR_BOTTOM_HERO_EMPTY_SLOT)
            ++occupiedSlotsLocal;
    }

    if (occupiedSlotsLocal != 0) {
        displayIndexData = 0;
        monsterIconsLocal = gpResourceManager->GetIcon("mons32.icn");
        for (armySlot = 0; armySlot < ADVMGR_BOTTOM_HERO_ARMY_SLOTS; ++armySlot) {
            creature = targetHero->m_army.m_creatureTypes[armySlot];
            if (creature != ADVMGR_BOTTOM_HERO_EMPTY_SLOT) {
                unsigned char iconPositions[16] = {
                    50, 3, 96, 3, 50, 17, 73, 17,
                    96, 17, 27, 32, 73, 32, 119, 32
                };
                signed char armyLayouts[ADVMGR_BOTTOM_HERO_ARMY_SLOTS]
                                             [ADVMGR_BOTTOM_HERO_ARMY_SLOTS] = {
                    {3, -1, -1, -1, -1},
                    {2, 4, -1, -1, -1},
                    {0, 1, 6, -1, -1},
                    {0, 1, 5, 6, -1},
                    {0, 1, 5, 6, 7}
                };

                armyCountLabelsResult[displayIndexData] = static_cast<char *>(BaseAlloc(
                    ADVMGR_BOTTOM_HERO_LABEL_BYTES, ADVMGR_SOURCE_FILE,
                    ADVMGR_BOTTOM_HERO_LINE + 0x44));
                if (targetHero->m_army.m_creatureCounts[armySlot] >
                    ADVMGR_BOTTOM_HERO_MAX_FULL_COUNT) {
                    sprintf(armyCountLabelsResult[displayIndexData], "%dk",
                            targetHero->m_army.m_creatureCounts[armySlot] /
                                ADVMGR_BOTTOM_HERO_COUNT_DIVISOR);
                } else {
                    sprintf(armyCountLabelsResult[displayIndexData], "%d",
                            targetHero->m_army.m_creatureCounts[armySlot]);
                }

                layoutIndexIndex = armyLayouts[occupiedSlotsLocal - 1][displayIndexData];
                iconX = iconPositions[layoutIndexIndex * 2];
                iconY = iconPositions[layoutIndexIndex * 2 + 1];
                labelY = iconY + ADVMGR_BOTTOM_HERO_LABEL_Y_OFFSET;
                iconEntryValue = reinterpret_cast<IconEntry *>(
                    creature * sizeof(IconEntry) + monsterIconsLocal->m_data);
                if (layoutIndexIndex == 0 || layoutIndexIndex == 1) {
                    labelY -= ADVMGR_BOTTOM_HERO_TOP_LABEL_SHIFT;
                    if (iconEntryValue->h < ADVMGR_BOTTOM_HERO_TOP_MIN_HEIGHT)
                        iconY += ADVMGR_BOTTOM_HERO_TOP_MIN_HEIGHT - iconEntryValue->h;
                } else if (iconEntryValue->h < ADVMGR_BOTTOM_HERO_LOWER_MIN_HEIGHT) {
                    iconY += ADVMGR_BOTTOM_HERO_LOWER_MIN_HEIGHT - iconEntryValue->h;
                }

                labelWidthCount = smallFont->LineWidth(armyCountLabelsResult[displayIndexData]);
                groupWidthRef = iconEntryValue->w + labelWidthCount;
                if (groupWidthRef > ADVMGR_BOTTOM_HERO_GROUP_WIDTH)
                    groupWidthRef = ADVMGR_BOTTOM_HERO_GROUP_WIDTH;
                iconX -= (groupWidthRef + 1) / 2;
                labelX = groupWidthRef - 1 + iconX - (labelWidthCount - 1);

                m_bottomViewIcons[displayIndexData] = new iconWidget(
                    iconX + ADVMGR_BOTTOM_HERO_PANEL_X,
                    iconY + ADVMGR_BOTTOM_HERO_PANEL_Y,
                    ADVMGR_BOTTOM_HERO_ICON_WIDTH, ADVMGR_BOTTOM_HERO_ICON_HEIGHT,
                    "mons32.icn", creature, 0,
                    displayIndexData + ADVMGR_BOTTOM_HERO_FIRST_ICON_ID,
                    ADVMGR_BOTTOM_HERO_WIDGET_FLAGS, 1);
                if (m_bottomViewIcons[displayIndexData] == 0)
                    MemError();

                m_bottomViewTexts[displayIndexData] = new textWidget(
                    labelX + ADVMGR_BOTTOM_HERO_PANEL_X,
                    labelY + ADVMGR_BOTTOM_HERO_PANEL_Y,
                    ((targetHero->m_army.m_creatureCounts[armySlot] <=
                       ADVMGR_BOTTOM_HERO_MAX_FULL_COUNT) - 1 & 4) +
                        strlen(armyCountLabelsResult[displayIndexData]) *
                            ADVMGR_BOTTOM_HERO_CHARACTER_WIDTH,
                    ADVMGR_BOTTOM_HERO_LABEL_HEIGHT, armyCountLabelsResult[displayIndexData],
                    "smalfont.fnt", 1,
                    displayIndexData + ADVMGR_BOTTOM_HERO_FIRST_TEXT_ID,
                    ADVMGR_BOTTOM_HERO_TEXT_ALIGNMENT, 1);
                if (m_bottomViewTexts[displayIndexData] == 0)
                    MemError();

                m_adventureWindow->AddWidget(m_bottomViewIcons[displayIndexData], -1);
                m_adventureWindow->AddWidget(m_bottomViewTexts[displayIndexData], -1);
                ++displayIndexData;
            }
        }
        gpResourceManager->Dispose(monsterIconsLocal);
    }
    return 1;
}

// @match-note
// Complete semantics, 0xf0 frame/slots, CFG, and all 109 relocation targets
// agree. The first code divergence is the commutative loop comparison at +0x409;
// later residuals are multiply/load order, folding of literal 30 + 6, consequent
// jump displacements, and two retail NOPs. Reversing the loop comparison and
// multiplication operands and splitting the literal sum were checked. Revisit
// after a material TU-state change or in the last-mile phase.
VA(0x0046235b, 0xd32)
void advManager::HeroQuickView(int heroId, int locatorSlot, int windowX, int windowY)
{
    short armyAreaWidthLocal = 160;
    short armyAreaLeftLocal = 22;
    short detailedCreatureY = 124;
    short stackIconWidthData = 32;
    short creatureIconHeight = 32;
    short widgetEnableFlagLocal = 1;
    short portraitWidgetLocal = 2;
    short primaryStatsWidgetValue = 3;
    short playerColorWidgetId = 8;
    iconWidget *stackIconsWidgets[5];
    textWidget *creatureTextWidgetsLocal[5];
    char *armyLabelsStrings[5];
    tag_message quickViewMessageState;
    icon *monsterIconRef;
    hero *targetHero;
    heroWindow *quickWindowSlot;
    int visibleArmyCountState;
    int armyIndex;
    int previousOriginXState;
    int savedOriginY;

    quickViewMessageState.type = 0x200;
    if (heroId == ADVMGR_INVALID_HERO)
        return;

    monsterIconRef = gpResourceManager->GetIcon("mons32.icn");
    targetHero = gpGame->GetHero(heroId);
    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1 ||
        IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, 8)) {
        if (windowX == -1) {
            windowX = 288;
            windowY = locatorSlot * 30 + 97;
        }
        quickWindowSlot = new heroWindow(windowX, windowY, "qhero0.bin");
        if (quickWindowSlot == 0)
            MemError();
        SetWinText(quickWindowSlot, 18);
    } else {
        quickWindowSlot = new heroWindow(windowX, windowY, "qhero1.bin");
        if (quickWindowSlot == 0)
            MemError();
    }

    quickViewMessageState.payload.widget.command = 4;
    quickViewMessageState.payload.widget.id = 2;
    quickViewMessageState.payload.widget.data.value = targetHero->m_portrait;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    quickViewMessageState.payload.widget.command = 4;
    quickViewMessageState.payload.widget.id = 8;
    quickViewMessageState.payload.widget.data.value = gpGame->GetPlayerColor(targetHero->m_owner) * 2;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    ++quickViewMessageState.payload.widget.id;
    ++quickViewMessageState.payload.widget.data.value;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    sprintf(gText, "%s", targetHero->m_name);
    quickViewMessageState.payload.widget.command = 3;
    quickViewMessageState.payload.widget.id = 1;
    quickViewMessageState.payload.widget.data.text = gText;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);

    visibleArmyCountState = 0;
    for (armyIndex = 0; armyIndex < 5; ++armyIndex)
        if (targetHero->m_army.m_creatureTypes[armyIndex] != -1)
            ++visibleArmyCountState;

    if (targetHero->m_owner == giCurPlayer || m_identifyHeroActive == 1 ||
        IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, 8)) {
        for (armyIndex = 0; armyIndex < 4; ++armyIndex) {
            sprintf(gText, "%d", targetHero->Stats(armyIndex));
            quickViewMessageState.payload.widget.id = armyIndex + 3;
            quickViewMessageState.payload.widget.data.text = gText;
            quickWindowSlot->BroadcastMessage(quickViewMessageState);
        }
        sprintf(gText, "%d/%d", targetHero->m_spellPoints,
                targetHero->Stats(3) * 10);
        quickViewMessageState.payload.widget.id = 7;
        quickViewMessageState.payload.widget.data.text = gText;
        quickWindowSlot->BroadcastMessage(quickViewMessageState);

        if (visibleArmyCountState != 0) {
            int armyStartPosition = (160 - visibleArmyCountState * 32) / 2 + 22;
            int displayIndexStateOffset = 0;
            int creature;
            for (armyIndex = 0; armyIndex < visibleArmyCountState; ++armyIndex) {
                while (targetHero->m_army.m_creatureTypes[displayIndexStateOffset] == -1)
                    ++displayIndexStateOffset;
                creature = targetHero->m_army.m_creatureTypes[displayIndexStateOffset];
                if (creature != -1) {
                stackIconsWidgets[armyIndex] = new iconWidget(
                    static_cast<short>(armyIndex * 32 + armyStartPosition -
                        GetIconEntry(monsterIconRef, creature)->x +
                        (32 - GetIconEntry(monsterIconRef, creature)->w) / 2 + 1),
                    static_cast<short>(124 -
                        GetIconEntry(monsterIconRef, creature)->y -
                        GetIconEntry(monsterIconRef, creature)->h + 30),
                    32, 32, "mons32.icn",
                    static_cast<short>(creature), 0, -1, 16, 1);
                if (stackIconsWidgets[armyIndex] == 0)
                    MemError();
                armyLabelsStrings[armyIndex] = static_cast<char *>(BaseAlloc(
                    5, ADVMGR_SOURCE_FILE, ADVMGR_QUICK_VIEW_LINE + 0x9b));
                sprintf(armyLabelsStrings[armyIndex], "%d",
                        targetHero->m_army.m_creatureCounts[displayIndexStateOffset]);
                creatureTextWidgetsLocal[armyIndex] = new textWidget(
                    static_cast<short>(armyIndex * 32 + armyStartPosition),
                    static_cast<short>(124 + 32), 32, 12,
                    armyLabelsStrings[armyIndex], "smalfont.fnt", 1, -1,
                    0x200, 1);
                if (creatureTextWidgetsLocal[armyIndex] == 0)
                    MemError();
                quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
                quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
                }
                ++displayIndexStateOffset;
            }
        }
    } else if (visibleArmyCountState != 0) {
        int rowYCurrent = 73;
        int topRowCount;
        int secondRowCountTotal;
        int creatureTypeId;
        switch (visibleArmyCountState) {
        case 1:
        case 2:
        case 3:
            rowYCurrent += 22;
            topRowCount = visibleArmyCountState;
            secondRowCountTotal = 0;
            break;
        case 4:
            topRowCount = 2;
            secondRowCountTotal = 2;
            break;
        default:
            topRowCount = 2;
            secondRowCountTotal = 3;
            break;
        }

        int displayIndexValue = 0;
        int armySpacing = 160 / topRowCount;
        int slotStartPosition = (armySpacing - 32) / 2 + 22;
        for (armyIndex = 0; armyIndex < topRowCount; ++armyIndex) {
            while (targetHero->m_army.m_creatureTypes[displayIndexValue] == -1)
                ++displayIndexValue;
            creatureTypeId = targetHero->m_army.m_creatureTypes[displayIndexValue];
            stackIconsWidgets[armyIndex] = new iconWidget(
                static_cast<short>(armyIndex * armySpacing +
                    slotStartPosition - GetIconEntry(monsterIconRef, creatureTypeId)->x +
                    (32 - GetIconEntry(monsterIconRef, creatureTypeId)->w) / 2 + 1),
                static_cast<short>(rowYCurrent -
                    GetIconEntry(monsterIconRef, creatureTypeId)->y -
                    GetIconEntry(monsterIconRef, creatureTypeId)->h + 30),
                32, 32, "mons32.icn",
                static_cast<short>(creatureTypeId), 0, -1, 16, 1);
            if (stackIconsWidgets[armyIndex] == 0)
                MemError();
            armyLabelsStrings[armyIndex] = static_cast<char *>(BaseAlloc(
                15, ADVMGR_SOURCE_FILE, ADVMGR_QUICK_VIEW_LINE + 0xe3));
            strcpy(armyLabelsStrings[armyIndex],
                   GetArmySizeName(targetHero->m_army.m_creatureCounts[displayIndexValue], 0));
            creatureTextWidgetsLocal[armyIndex] = new textWidget(
                static_cast<short>(armyIndex * armySpacing + 22),
                static_cast<short>(rowYCurrent + 32), armySpacing, 12,
                armyLabelsStrings[armyIndex], "smalfont.fnt", 1, -1,
                0x200, 1);
            if (creatureTextWidgetsLocal[armyIndex] == 0)
                MemError();
            quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
            quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
            ++displayIndexValue;
        }

        if (secondRowCountTotal != 0) {
            armySpacing = 160 / secondRowCountTotal;
            slotStartPosition = (armySpacing - 32) / 2 + 22;
            rowYCurrent += 44;
            for (armyIndex = topRowCount;
                 armyIndex < topRowCount + secondRowCountTotal; ++armyIndex) {
                while (targetHero->m_army.m_creatureTypes[displayIndexValue] == -1)
                    ++displayIndexValue;
                creatureTypeId = targetHero->m_army.m_creatureTypes[displayIndexValue];
                stackIconsWidgets[armyIndex] = new iconWidget(
                    static_cast<short>((armyIndex - 2) * armySpacing +
                        slotStartPosition -
                        GetIconEntry(monsterIconRef, creatureTypeId)->x +
                        (32 - GetIconEntry(monsterIconRef, creatureTypeId)->w) / 2 + 1),
                    static_cast<short>(rowYCurrent -
                        GetIconEntry(monsterIconRef, creatureTypeId)->y -
                        GetIconEntry(monsterIconRef, creatureTypeId)->h + 30 + 6),
                    32, 32, "mons32.icn",
                    static_cast<short>(creatureTypeId), 0, -1, 16, 1);
                if (stackIconsWidgets[armyIndex] == 0)
                    MemError();
                armyLabelsStrings[armyIndex] = static_cast<char *>(BaseAlloc(
                    15, ADVMGR_SOURCE_FILE, ADVMGR_QUICK_VIEW_LINE + 0x10e));
                strcpy(armyLabelsStrings[armyIndex],
                       GetArmySizeName(targetHero->m_army.m_creatureCounts[displayIndexValue], 0));
                creatureTextWidgetsLocal[armyIndex] = new textWidget(
                    static_cast<short>((armyIndex - 2) * armySpacing + 22),
                    static_cast<short>(rowYCurrent + 38), armySpacing, 12,
                    armyLabelsStrings[armyIndex], "smalfont.fnt", 1, -1,
                    0x200, 1);
                if (creatureTextWidgetsLocal[armyIndex] == 0)
                    MemError();
                quickWindowSlot->AddWidget(stackIconsWidgets[armyIndex], -1);
                quickWindowSlot->AddWidget(creatureTextWidgetsLocal[armyIndex], -1);
                ++displayIndexValue;
            }
        }
    }

    previousOriginXState = m_mapOriginX;
    savedOriginY = m_mapOriginY;
    m_mapOriginX = targetHero->m_x - 7;
    m_mapOriginY = targetHero->m_y - 7;
    UpdateRadar(1, 0);
    gpWindowManager->AddWindow(quickWindowSlot, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(quickWindowSlot);
    delete quickWindowSlot;
    m_mapOriginX = previousOriginXState;
    m_mapOriginY = savedOriginY;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (quickViewMessageState.type == 8 && targetHero->m_owner == giCurPlayer)
        SetHeroContext(static_cast<unsigned char>(targetHero->m_id), 0);
    gpResourceManager->Dispose(monsterIconRef);
}

VA(0x0046308d, 0x120)
char * advManager::GetArmySizeName(int armySize, int grammar)
{
    if (giDebugLevel > 0) {
        sprintf(cArmySizeName, "%d", armySize);
        return cArmySizeName;
    }
    if (armySize < ADVMGR_ARMY_FEW_LIMIT)
        return gArmySizeNames[0][grammar];
    if (armySize < ADVMGR_ARMY_SEVERAL_LIMIT)
        return gArmySizeNames[1][grammar];
    if (armySize < ADVMGR_ARMY_PACK_LIMIT)
        return gArmySizeNames[2][grammar];
    if (armySize < ADVMGR_ARMY_LOTS_LIMIT)
        return gArmySizeNames[3][grammar];
    if (armySize < ADVMGR_ARMY_HORDE_LIMIT)
        return gArmySizeNames[4][grammar];
    if (armySize < ADVMGR_ARMY_THRONG_LIMIT)
        return gArmySizeNames[5][grammar];
    if (armySize < ADVMGR_ARMY_SWARM_LIMIT)
        return gArmySizeNames[6][grammar];
    if (armySize < ADVMGR_ARMY_ZOUNDS_LIMIT)
        return gArmySizeNames[7][grammar];
    return gArmySizeNames[8][grammar];
}

// @early-stop
// The relocation-masked lite diff has no opcode or operand differences: the
// complete 0xec frame/slots and CFG agree, and all 102 external relocation
// targets resolve identically. The residual is delinked relocation-symbol identity.
VA(0x004631ad, 0xc29)
void advManager::TownQuickView(int townId, int locatorSlot, int windowX, int windowY)
{
    icon *monsterIconLocal;
    short portraitWidgetLocal;
    short armyIconHeightState;
    tag_message messageLocal;
    int armyCountLocal;
    int armyIndex;
    short armyIconWidthState;
    short widgetEnabledData;
    short colorWidgetValue;
    int previousOriginXValue;
    heroWindow *townQuickWindow;
    town *quickTownLocal;
    int previousOriginYSlot;
    int informationLevel;
    char *emptyArmyLabel;
    short armyAreaWidth;
    short armyAreaLeftValue;
    widget *emptyArmyTextState;

    armyAreaWidth = 192;
    armyAreaLeftValue = 22;
    armyIconWidthState = 32;
    armyIconHeightState = 32;
    widgetEnabledData = 1;
    portraitWidgetLocal = 2;
    colorWidgetValue = 8;

    if (townId == ADVMGR_INVALID_HERO)
        return;

    monsterIconLocal = gpResourceManager->GetIcon("mons32.icn");
    quickTownLocal = gpGame->GetTown(townId);
    if (windowX == -1) {
        windowX = 328;
        windowY = 176;
    }
    townQuickWindow = new heroWindow(windowX, windowY, "qtown1.bin");
    if (townQuickWindow == 0)
        MemError();

    if (quickTownLocal->m_owner == giCurPlayer || giDebugLevel >= 2) {
        informationLevel = 3;
    } else {
        informationLevel = gpGame->GetNumThievesGuilds(giCurPlayer);
        if (informationLevel > 2)
            informationLevel = 2;
    }
    if (IsCrystalBallInEffect(quickTownLocal->m_x, quickTownLocal->m_y, 8))
        informationLevel = 3;

    SetWinText(townQuickWindow, 19);
    armyCountLocal = 0;
    messageLocal.type = 0x200;
    messageLocal.payload.widget.command = 4;
    messageLocal.payload.widget.id = 2;
    messageLocal.payload.widget.data.value = quickTownLocal->m_type + 9;
    if ((gpGame->GetTown(townId)->m_buildings & 0x40) == 0)
        messageLocal.payload.widget.data.value += 6;
    townQuickWindow->BroadcastMessage(messageLocal);

    if (informationLevel != 3 ||
        BitTest(gpGame->m_knownTowns, static_cast<signed char>(quickTownLocal->m_id)) == 0) {
        messageLocal.payload.widget.command = 6;
        messageLocal.payload.widget.id = 300;
        messageLocal.payload.widget.data.value = 4;
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    if (quickTownLocal->m_owner == -1) {
        messageLocal.payload.widget.command = 6;
        messageLocal.payload.widget.id = 8;
        messageLocal.payload.widget.data.value = 4;
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.payload.widget.id;
        townQuickWindow->BroadcastMessage(messageLocal);
    } else {
        messageLocal.payload.widget.command = 4;
        messageLocal.payload.widget.id = 8;
        messageLocal.payload.widget.data.value = gpGame->GetPlayerColor(quickTownLocal->m_owner) * 2;
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.payload.widget.id;
        ++messageLocal.payload.widget.data.value;
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    sprintf(gText, GetTownName(static_cast<signed char>(quickTownLocal->m_id)));
    messageLocal.payload.widget.command = 3;
    messageLocal.payload.widget.id = 1;
    messageLocal.payload.widget.data.text = gText;
    townQuickWindow->BroadcastMessage(messageLocal);

    armyCountLocal = 0;
    for (armyIndex = 0; armyIndex < 5; ++armyIndex)
        if (quickTownLocal->m_army.m_creatureTypes[armyIndex] != -1)
            ++armyCountLocal;

    if (informationLevel == 0 || armyCountLocal == 0) {
        emptyArmyLabel = static_cast<char *>(BaseAlloc(
            20, ADVMGR_SOURCE_FILE, ADVMGR_TOWN_VIEW_LINE + 0x83));
        if (informationLevel == 0)
            sprintf(emptyArmyLabel, "Unknown");
        else
            sprintf(emptyArmyLabel, "None");
        emptyArmyTextState = new textWidget(13, 117, 211, 12, emptyArmyLabel,
                                       "smalfont.fnt", 1, -1, 0x200, 1);
        if (emptyArmyTextState == 0)
            MemError();
        townQuickWindow->AddWidget(emptyArmyTextState, -1);
    } else {
        int secondRowCountState;
        int creatureSlotLocal;
        char *armyLabelsResult[5];
        int creatureLocal;
        iconWidget *armyIcons[5];
        textWidget *armyTexts[5];
        int displayIndexLocal;
        int widgetIndexWidget;
        int slotWidthSlot;
        int fiveArmyShiftValue;
        int slotStartState;
        int rowY;
        int firstRowCountState;

        rowY = 76;
        switch (armyCountLocal) {
        case 1:
        case 2:
        case 3:
            rowY += 22;
            firstRowCountState = armyCountLocal;
            secondRowCountState = 0;
            break;
        case 4:
            firstRowCountState = 2;
            secondRowCountState = 2;
            break;
        default:
            firstRowCountState = 2;
            secondRowCountState = 3;
            break;
        }

        displayIndexLocal = 0;
        widgetIndexWidget = 0;
        creatureSlotLocal = 0;
        slotWidthSlot = 192 / firstRowCountState;
        slotStartState = (slotWidthSlot - 32) / 2 + 22;
        fiveArmyShiftValue = 0;
        for (armyIndex = 0; armyIndex < firstRowCountState; ++armyIndex) {
            if (armyCountLocal == 5)
                fiveArmyShiftValue = armyIndex == 0 ? 12 : -12;
            while (quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal] == -1)
                ++creatureSlotLocal;
            creatureLocal = quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal];
            armyIcons[widgetIndexWidget] = new iconWidget(
                static_cast<short>(slotWidthSlot * widgetIndexWidget + slotStartState +
                    fiveArmyShiftValue - GetIconEntry(monsterIconLocal, creatureLocal)->x +
                    (32 - GetIconEntry(monsterIconLocal, creatureLocal)->w) / 2 + 1),
                static_cast<short>(rowY - GetIconEntry(monsterIconLocal, creatureLocal)->y -
                    GetIconEntry(monsterIconLocal, creatureLocal)->h + 30),
                32, 32, "mons32.icn",
                static_cast<short>(creatureLocal), 0, -1, 16, 1);
            if (armyIcons[widgetIndexWidget] == 0)
                MemError();
            armyLabelsResult[widgetIndexWidget] = static_cast<char *>(BaseAlloc(
                15, ADVMGR_SOURCE_FILE, ADVMGR_TOWN_VIEW_LINE + 0xd6));
            if (informationLevel == 3)
                sprintf(armyLabelsResult[widgetIndexWidget], "%d",
                        quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal]);
            else if (informationLevel == 2)
                strcpy(armyLabelsResult[widgetIndexWidget], GetArmySizeName(
                    quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal], 0));
            else
                strcpy(armyLabelsResult[widgetIndexWidget], "???");
            armyTexts[widgetIndexWidget] = new textWidget(
                static_cast<short>(slotWidthSlot * widgetIndexWidget + slotStartState +
                    fiveArmyShiftValue - 14), static_cast<short>(rowY + 32), 60, 12,
                armyLabelsResult[widgetIndexWidget], "smalfont.fnt", 1, -1,
                0x200, 1);
            if (armyTexts[widgetIndexWidget] == 0)
                MemError();
            townQuickWindow->AddWidget(armyIcons[widgetIndexWidget], -1);
            townQuickWindow->AddWidget(armyTexts[widgetIndexWidget], -1);
            ++widgetIndexWidget;
            ++creatureSlotLocal;
        }

        if (secondRowCountState != 0) {
            slotWidthSlot = 192 / secondRowCountState;
            slotStartState = (slotWidthSlot - 32) / 2 + 22;
            rowY += 44;
            for (armyIndex = firstRowCountState;
                 armyIndex < secondRowCountState + firstRowCountState; ++armyIndex) {
                while (quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal] == -1)
                    ++creatureSlotLocal;
                creatureLocal = quickTownLocal->m_army.m_creatureTypes[creatureSlotLocal];
                armyIcons[widgetIndexWidget] = new iconWidget(
                    static_cast<short>((widgetIndexWidget - firstRowCountState) *
                        slotWidthSlot + slotStartState -
                        GetIconEntry(monsterIconLocal, creatureLocal)->x +
                        (32 - GetIconEntry(monsterIconLocal, creatureLocal)->w) / 2 + 1),
                    static_cast<short>(rowY - GetIconEntry(monsterIconLocal, creatureLocal)->y -
                        GetIconEntry(monsterIconLocal, creatureLocal)->h + 30),
                    32, 32, "mons32.icn",
                    static_cast<short>(creatureLocal), 0, -1, 16, 1);
                if (armyIcons[widgetIndexWidget] == 0)
                    MemError();
                armyLabelsResult[widgetIndexWidget] = static_cast<char *>(BaseAlloc(
                    15, ADVMGR_SOURCE_FILE, ADVMGR_TOWN_VIEW_LINE + 0x108));
                if (informationLevel == 3)
                    sprintf(armyLabelsResult[widgetIndexWidget], "%d",
                            quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal]);
                else if (informationLevel == 2)
                    strcpy(armyLabelsResult[widgetIndexWidget], GetArmySizeName(
                        quickTownLocal->m_army.m_creatureCounts[creatureSlotLocal], 0));
                else
                    strcpy(armyLabelsResult[widgetIndexWidget], "???");
                armyTexts[widgetIndexWidget] = new textWidget(
                    static_cast<short>((widgetIndexWidget - firstRowCountState) *
                        slotWidthSlot + slotStartState - 14),
                    static_cast<short>(rowY + 32), 60, 12,
                    armyLabelsResult[widgetIndexWidget], "smalfont.fnt", 1, -1,
                    0x200, 1);
                if (armyTexts[widgetIndexWidget] == 0)
                    MemError();
                townQuickWindow->AddWidget(armyIcons[widgetIndexWidget], -1);
                townQuickWindow->AddWidget(armyTexts[widgetIndexWidget], -1);
                ++widgetIndexWidget;
                ++creatureSlotLocal;
            }
        }
    }

    gpWindowManager->AddWindow(townQuickWindow, -1, 1);
    previousOriginXValue = m_mapOriginX;
    previousOriginYSlot = m_mapOriginY;
    m_mapOriginX = quickTownLocal->m_x - 7;
    m_mapOriginY = quickTownLocal->m_y - 7;
    UpdateRadar(1, 0);
    QuickViewWait();
    gpWindowManager->RemoveWindow(townQuickWindow);
    delete townQuickWindow;
    m_mapOriginX = previousOriginXValue;
    m_mapOriginY = previousOriginYSlot;
    UpdateRadar(1, 0);
    CompleteDraw(0);
    UpdateScreen(0, 0);
    if (messageLocal.type == 8 && quickTownLocal->m_owner == giCurPlayer)
        SetTownContext(static_cast<signed char>(quickTownLocal->m_id));
    gpResourceManager->Dispose(monsterIconLocal);
}

VA(0x00463dd6, 0x11f)
void advManager::RedrawAdvScreen(int update, int freeBorder)
{
    if (!bShowIt)
        return;
    gpResourceManager->GetBackdrop("advbord.icn", gpWindowManager->m_screen, 1);
    if (freeBorder) {
        BaseFree(m_adventureBorder, ADVMGR_SOURCE_FILE,
                 ADVMGR_BORDER_FREE_LINE + 9);
        m_adventureBorder = 0;
    }
    SaveAdventureBorder();
    UpdateHeroLocators(0, 0);
    UpdateTownLocators(0, 0);
    UpdBottomView(1, 0, 0);
    m_adventureWindow->DrawWindow(0);
    if (update)
        gpWindowManager->UpdateScreenRegion(0, 0, ADVMGR_SCREEN_WIDTH,
                                            ADVMGR_SCREEN_HEIGHT);
    UpdateRadar(update, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    if (update)
        UpdateScreen(0, 0);
}

VA(0x00463ef5, 0x1f)
void advManager::DeactivateCurrTown(void)
{
    gpCurPlayer->m_currentTown = -1;
}

VA(0x00463f14, 0x27)
void advManager::DeactivateCurrHero(void)
{
    DemobilizeCurrHero();
    gpCurPlayer->m_currentHero = -1;
}

VA(0x00463f3b, 0x5a)
void advManager::MobilizeCurrHero(int update)
{
    if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO)
        return;
    if (m_heroContextLocked)
        return;
    SetHeroContext(gpCurPlayer->m_currentHero, update);
}

// @match-note
// First residual at +0xd9: retail loads m_eventFlags, ORs EAX with 0x80, then
// stores it; ours emits the equivalent OR dword ptr [hero+0xe3],0x80. The 0xc
// frame and all three slots, CFG, remaining instructions, and all 7 relocation
// targets agree. Both `|=` and explicit `= HERO_EVENT_EMBARKED | m_eventFlags`
// collapse to the memory OR; m_objectMetadata bitfield assignment fixed the other residual.
// Revisit with specific new evidence or after the SOURCE placeholder census is
// zero.
VA(0x00463f95, 0x16c)
void advManager::DemobilizeCurrHero(void)
{
    if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO)
        return;
    if (!m_heroContextLocked)
        return;

    m_heroContextLocked = 0;
    hero *currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    StopCursor(1);
    mapCell *currentCell = GetCell(currentHero->m_x, currentHero->m_y);
    currentHero->m_locationType = currentCell->m_triggerType;
    currentHero->m_occupiedTown = currentCell->m_objectMetadata;
    currentHero->m_direction = static_cast<unsigned char>(m_cursorDirection);
    if (m_cursorType == CURSOR_HERO_TYPE_BOAT)
        currentHero->m_eventFlags = HERO_EVENT_EMBARKED | currentHero->m_eventFlags;
    currentCell->m_triggerType = MAP_EVENT_ACTION_FLAG | MAP_EVENT_HERO_INTERACTION;
    currentCell->m_objectMetadata = currentHero->m_id;
    currentCell->m_flags &= ~CURSOR_MAP_VISIBLE_FLAG;
    m_cursorActive = 0;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
}

VA(0x00464101, 0x217)
void advManager::SetTownContext(int townId)
{
    DeactivateCurrHero();
    gpCurPlayer->m_currentTown = static_cast<signed char>(townId);
    town *currentTownValue = gpGame->GetTown(gpCurPlayer->m_currentTown);
    m_mapOriginX = currentTownValue->m_x - ADVMGR_VIEW_CENTER_OFFSET;
    m_mapOriginY = currentTownValue->m_y - ADVMGR_VIEW_CENTER_OFFSET;

    int selectedIndex7 = 0;
    int index;
    for (index = 0; index < gpCurPlayer->m_townCount; ++index) {
        if (gpCurPlayer->m_townIds[index] == townId)
            selectedIndex7 = index;
    }
    if (selectedIndex7 < gpCurPlayer->m_townLocatorPage)
        gpCurPlayer->m_townLocatorPage = static_cast<signed char>(selectedIndex7);
    else if (gpCurPlayer->m_townLocatorPage + ADVMGR_LOCATOR_VISIBLE_COUNT - 1 <
             selectedIndex7)
        gpCurPlayer->m_townLocatorPage = static_cast<signed char>(
            selectedIndex7 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1));

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    HideRoute(0, 0, 1);
    UpdBottomView(1, 1, 1);
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
                         m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET, 1);

    selectedIndex7 = giGroundToTerrain[
        GetCell(currentTownValue->m_x, currentTownValue->m_y)->m_terrainImageIndex];
    if (m_currentTerrain != selectedIndex7) {
        m_currentTerrain = selectedIndex7;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
    }
    gpInputManager->ForceMouseMove();
    m_lastHoverCell = 0;
}

VA(0x00464318, 0x392)
void advManager::SetHeroContext(int heroId, int update)
{
    if (heroId == ADVMGR_INVALID_HERO)
        return;

    DeactivateCurrTown();
    HideRoute(0, 0, 1);
    DeactivateCurrHero();
    m_heroContextLocked = 1;
    gpCurPlayer->m_currentHero = static_cast<signed char>(heroId);
    hero *currentHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    m_mapOriginX = currentHero->m_x - ADVMGR_VIEW_CENTER_OFFSET;
    m_mapOriginY = currentHero->m_y - ADVMGR_VIEW_CENTER_OFFSET;
    m_cursorMapY = 7;
    m_cursorMapX = m_cursorMapY;
    m_previousCursorMapY = ADVMGR_INVALID_CELL;
    m_previousCursorMapX = m_previousCursorMapY;
    if (currentHero->m_eventFlags & HERO_EVENT_EMBARKED)
        m_cursorType = CURSOR_HERO_TYPE_BOAT;
    else
        m_cursorType = currentHero->m_cursorType;
    m_cursorDirection = currentHero->m_direction;
    m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);

    mapCell *currentCell = GetCell(currentHero->m_x, currentHero->m_y);
    currentCell->m_flags |= CURSOR_MAP_VISIBLE_FLAG;
    gpGame->RestoreCell(currentHero->m_x, currentHero->m_y,
                        currentHero->m_locationType, currentHero->m_occupiedTown,
                        0, 4);

    int selectedIndex7 = 0;
    int index;
    for (index = 0; index < gpCurPlayer->m_heroCount; ++index) {
        if (gpCurPlayer->m_heroIds[index] == heroId)
            selectedIndex7 = index;
    }
    if (selectedIndex7 < gpCurPlayer->m_heroLocatorPage)
        gpCurPlayer->m_heroLocatorPage = static_cast<signed char>(selectedIndex7);
    else if (gpCurPlayer->m_heroLocatorPage + ADVMGR_LOCATOR_VISIBLE_COUNT - 1 <
             selectedIndex7)
        gpCurPlayer->m_heroLocatorPage = static_cast<signed char>(
            selectedIndex7 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1));

    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    if (!update && (m_active == 1 || gbThisNetHumanPlayer[giCurPlayer])) {
        Reseed(0, 0);
        SeedTo(currentHero->m_destinationX, currentHero->m_destinationY);
        ShowRoute(0, 0, !update);
    }
    UpdBottomView(1, 1, 1);
    m_cursorActive = 1;
    UpdateRadar(1, 0);
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
                         m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET, 1);

    selectedIndex7 = giGroundToTerrain[currentCell->m_terrainImageIndex];
    if (m_currentTerrain != selectedIndex7) {
        m_currentTerrain = selectedIndex7;
        gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
    }
    if (!gbHeroMoving) {
        gpInputManager->ForceMouseMove();
        m_lastHoverCell = 0;
    }
}

// @match-note
// First residual bytes are +0xcc/+0xd0: retail uses `cmp eax,[message.y]; jle`,
// ours uses `cmp [message.y],eax; jge`; the mirrored upper clamp differs at
// +0xe8/+0xec. The 0x80 frame, every named/compiler slot, CFG, all other code,
// and all 26 relocation targets agree. `message.y < sum` and `sum > message.y`
// lower identically; an empty positive arm plus else adds two 5-byte jumps and
// regresses the match. Revisit with specific new evidence or after the SOURCE
// placeholder census is zero.
VA(0x004646aa, 0x22f)
void advManager::DoHeroKnob(void)
{
    int previousPageSlot = gpCurPlayer->m_heroLocatorPage;
    int locatorCount29 = gpCurPlayer->m_heroCount;
    int newPageState;
    double pageHeight7 = static_cast<double>(ADVMGR_LOCATOR_HERO_SCROLL_SPAN) /
                         (locatorCount29 - ADVMGR_LOCATOR_VISIBLE_COUNT);
    int mouseX4;
    int mouseYState;
    gpMouseManager->MouseCoords(mouseX4, mouseYState);
    int dragOffset5 = mouseYState - m_scrollLeftButton->m_y;
    gpInputManager->Flush();
    tag_message message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP &&
           message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < dragOffset5 + ADVMGR_LOCATOR_SCROLL_BASE_Y)
                message.payload.mouse.y = dragOffset5 + ADVMGR_LOCATOR_SCROLL_BASE_Y;
            if (message.payload.mouse.y > dragOffset5 + ADVMGR_LOCATOR_KNOB_MAX_Y)
                message.payload.mouse.y = dragOffset5 + ADVMGR_LOCATOR_KNOB_MAX_Y;
            gpMouseManager->Main(message);
            m_scrollLeftButton->m_y = message.payload.mouse.y - dragOffset5;
            m_adventureWindow->DrawWindow();
            if (locatorCount29 > ADVMGR_LOCATOR_VISIBLE_COUNT) {
                newPageState = static_cast<int>(
                    (m_scrollLeftButton->m_y - ADVMGR_LOCATOR_SCROLL_BASE_Y) /
                    pageHeight7);
                if (newPageState != previousPageSlot) {
                    gpCurPlayer->m_heroLocatorPage = static_cast<signed char>(newPageState);
                    if (newPageState >
                        locatorCount29 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1))
                        newPageState =
                            locatorCount29 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1);
                    UpdateHeroLocators(0, 1);
                    m_scrollLeftButton->m_y = message.payload.mouse.y - dragOffset5;
                    m_adventureWindow->DrawWindow();
                    previousPageSlot = newPageState;
                }
            }
        }
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
    }
    m_scrollLeftButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateHeroLocators(1, 1);
}

// @match-note
// First residual bytes are +0xcc/+0xd0: retail uses `cmp eax,[message.y]; jle`,
// ours uses `cmp [message.y],eax; jge`; the mirrored upper clamp differs at
// +0xe8/+0xec. The 0x80 frame, every named/compiler slot, CFG, all other code,
// and all 26 relocation targets agree. `message.y < sum` and `sum > message.y`
// lower identically; an empty positive arm plus else adds two 5-byte jumps and
// regresses the match. Revisit with specific new evidence or after the SOURCE
// placeholder census is zero.
VA(0x004648d9, 0x22f)
void advManager::DoTownKnob(void)
{
    int previousPageSlot = gpCurPlayer->m_townLocatorPage;
    int locatorCount29 = gpCurPlayer->m_townCount;
    int newPageState;
    double pageHeight7 = static_cast<double>(ADVMGR_LOCATOR_HERO_SCROLL_SPAN) /
                         (locatorCount29 - ADVMGR_LOCATOR_VISIBLE_COUNT);
    int mouseX4;
    int mouseYState;
    gpMouseManager->MouseCoords(mouseX4, mouseYState);
    int dragOffset5 = mouseYState - m_scrollRightButton->m_y;
    gpInputManager->Flush();
    tag_message message = gpInputManager->GetEvent();

    while (message.type != MESSAGE_LEFT_BUTTON_UP &&
           message.type != MESSAGE_RIGHT_BUTTON_UP) {
        if (message.type == MESSAGE_MOUSE_MOVE) {
            if (message.payload.mouse.y < dragOffset5 + ADVMGR_LOCATOR_SCROLL_BASE_Y)
                message.payload.mouse.y = dragOffset5 + ADVMGR_LOCATOR_SCROLL_BASE_Y;
            if (message.payload.mouse.y > dragOffset5 + ADVMGR_LOCATOR_KNOB_MAX_Y)
                message.payload.mouse.y = dragOffset5 + ADVMGR_LOCATOR_KNOB_MAX_Y;
            gpMouseManager->Main(message);
            m_scrollRightButton->m_y = message.payload.mouse.y - dragOffset5;
            m_adventureWindow->DrawWindow();
            if (locatorCount29 > ADVMGR_LOCATOR_VISIBLE_COUNT) {
                newPageState = static_cast<int>(
                    (m_scrollRightButton->m_y - ADVMGR_LOCATOR_SCROLL_BASE_Y) /
                    pageHeight7);
                if (newPageState != previousPageSlot) {
                    gpCurPlayer->m_townLocatorPage = static_cast<signed char>(newPageState);
                    if (newPageState >
                        locatorCount29 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1))
                        newPageState =
                            locatorCount29 - (ADVMGR_LOCATOR_VISIBLE_COUNT - 1);
                    UpdateTownLocators(0, 1);
                    m_scrollRightButton->m_y = message.payload.mouse.y - dragOffset5;
                    m_adventureWindow->DrawWindow();
                    previousPageSlot = newPageState;
                }
            }
        }
        Process1WindowsMessage();
        message = gpInputManager->GetEvent();
    }
    m_scrollRightButton->m_flags &= ~WIDGET_FLAG_SELECTED;
    UpdateTownLocators(1, 1);
}

// @early-stop
// All 0x397 relocation-masked bytes are identical and all 38 relocation targets
// agree; objdiff's residual is only delinked switch/jump-table local-label identity.
VA(0x00464b08, 0x397)
void advManager::CastSpell(int spell)
{
    hero *currentHeroSlot;
    if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO)
        currentHeroSlot = gpGame->GetHero(gpCurPlayer->m_currentHero);
    else
        currentHeroSlot = 0;

    int guardianTypes1[ADVMGR_MINE_GUARDIAN_VALUE_COUNT];
    mapCell *currentCell;
    int spellPowerValue;
    switch (spell) {
    case ADVENTURE_SPELL_SET_EARTH_GUARDIAN:
        guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX] =
            ADVMGR_MINE_GUARDIAN_EARTH_ELEMENTAL;
        goto setMineGuardian;
    case ADVENTURE_SPELL_SET_AIR_GUARDIAN:
        guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX] =
            ADVMGR_MINE_GUARDIAN_AIR_ELEMENTAL;
        goto setMineGuardian;
    case ADVENTURE_SPELL_SET_FIRE_GUARDIAN:
        guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX] =
            ADVMGR_MINE_GUARDIAN_FIRE_ELEMENTAL;
        goto setMineGuardian;
    case ADVENTURE_SPELL_SET_WATER_GUARDIAN:
        guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX] =
            ADVMGR_MINE_GUARDIAN_WATER_ELEMENTAL;
        goto setMineGuardian;
    case ADVENTURE_SPELL_HAUNT:
        guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX] =
            ADVMGR_MINE_GUARDIAN_GHOST;
        goto setMineGuardian;
setMineGuardian:
        currentCell = gpAdvManager->GetCell(currentHeroSlot->m_x, currentHeroSlot->m_y);
        if (currentCell->m_triggerType != (MAP_EVENT_ACTION_FLAG | MAP_EVENT_MINE)) {
            NormalDialog("You must be standing on the entrance to a mine (sawmills and alchemists don't count) to cast this spell.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
        gpGame->m_mines[currentCell->m_objectMetadata].guardianType =
            static_cast<signed char>(
                guardianTypes1[ADVMGR_MINE_GUARDIAN_TYPE_INDEX]);
        spellPowerValue = currentHeroSlot->Stats(HERO_PRIMARY_SPELL_POWER);
        if (spellPowerValue > ADVMGR_MINE_GUARDIAN_MAX_POWER)
            spellPowerValue = ADVMGR_MINE_GUARDIAN_MAX_POWER;
        gpGame->m_mines[currentCell->m_objectMetadata].guardianCount =
            static_cast<unsigned char>(
                spellPowerValue * ADVMGR_MINE_GUARDIANS_PER_POWER);
        if (spell == ADVENTURE_SPELL_HAUNT)
            gpGame->ClaimMine(currentCell->m_objectMetadata, -1);
        break;
    case ADVENTURE_SPELL_VIEW_MINES:
    case ADVENTURE_SPELL_VIEW_RESOURCES:
    case ADVENTURE_SPELL_VIEW_ARTIFACTS:
    case ADVENTURE_SPELL_VIEW_TOWNS:
    case ADVENTURE_SPELL_VIEW_HEROES:
    case ADVENTURE_SPELL_VIEW_ALL:
        ViewWorld(spell, spell == ADVENTURE_SPELL_VIEW_ALL,
                  spell == ADVENTURE_SPELL_VIEW_ALL);
        break;
    case ADVENTURE_SPELL_IDENTIFY_HERO:
        m_identifyHeroActive = 1;
        NormalDialog("Enemy heroes are now fully identifiable.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        break;
    case ADVENTURE_SPELL_SUMMON_BOAT:
        SummonBoat();
        break;
    case ADVENTURE_SPELL_DIMENSION_DOOR:
    case ADVENTURE_SPELL_TOWN_GATE:
    case ADVENTURE_SPELL_TOWN_PORTAL:
        if (currentHeroSlot->m_remainingMobility == 0) {
            NormalDialog("Your hero is too tired to cast this spell today.  Try again tomorrow.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            return;
        }
        if (currentHeroSlot->m_remainingMobility < ADVMGR_TRAVEL_SPELL_MOBILITY_COST)
            currentHeroSlot->m_remainingMobility = 0;
        else
            currentHeroSlot->m_remainingMobility -= ADVMGR_TRAVEL_SPELL_MOBILITY_COST;
        UpdateHeroLocator(-1, 1, 1);
        if (spell == ADVENTURE_SPELL_DIMENSION_DOOR)
            DimensionDoor();
        else
            TownGate(spell);
        break;
    case ADVENTURE_SPELL_VISIONS:
        if (!DoVisions(currentHeroSlot))
            return;
        break;
    default:
        break;
    }

    if (spell != ADVENTURE_SPELL_DIMENSION_DOOR &&
        spell != ADVENTURE_SPELL_TOWN_GATE &&
        spell != ADVENTURE_SPELL_TOWN_PORTAL)
        gpGame->GetHero(gpCurPlayer->m_currentHero)->UseSpell(spell);
}

VA(0x00464e9f, 0x24c)
int SaveGame(void)
{
    int result11 = 0;
    int humanPlayerCount1 = 0;
    gpAdvManager->DisableButtons();
    gpMouseManager->SetPointer("advmice.mse", ADVMGR_SAVE_POINTER_FRAME,
                               ADVMGR_SAVE_POINTER_DELAY);
    int playerLocal;
    for (playerLocal = 0; playerLocal < ADVMGR_SAVE_PLAYER_COUNT; ++playerLocal) {
        if (!gpGame->m_playerDead[playerLocal] && gbHumanPlayer[playerLocal])
            ++humanPlayerCount1;
    }

    char extension7[ADVMGR_SAVE_EXTENSION_SIZE];
    char patternState[ADVMGR_SAVE_PATTERN_SIZE];
    if (gbInCampaign) {
        sprintf(extension7, ".GMC");
        sprintf(patternState, "*.GMC");
    } else if (xIsPlayingExpansionCampaign) {
        sprintf(extension7, ".GXC");
        sprintf(patternState, "*.GXC");
    } else if (xIsExpansionMap) {
        sprintf(extension7, ".GX%d", humanPlayerCount1);
        sprintf(patternState, "*.GX%d", humanPlayerCount1);
    } else {
        sprintf(extension7, ".GM%d", humanPlayerCount1);
        sprintf(patternState, "*.GM%d", humanPlayerCount1);
    }

    fileRequester *requester2 = new fileRequester(
        ADVMGR_SAVE_REQUESTER_X, ADVMGR_SAVE_REQUESTER_Y,
        FILE_REQUESTER_SAVE_GAME, patternState, gcGamePath, extension7);
    if (requester2 == 0)
        MemError();
    int dialogResult7 = gpExec->DoDialog(requester2);
    if (dialogResult7 == FILE_REQUESTER_OK) {
        result11 = 1;
        bFreshSave = 1;
        result11 = gpGame->SaveGame(gLastFilename, 0, 0);
        if (result11)
            NormalDialog("Game saved successfully.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
    }
    delete requester2;
    gpAdvManager->EnableButtons();
    return result11;
}

VA(0x004650eb, 0xa6)
void advManager::CheckCastSpell(void)
{
    if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO) {
        MobilizeCurrHero(0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        gpMouseManager->SetPointer("advmice.mse", ADVMGR_POINTER_DEFAULT,
                                   ADVMGR_DEFAULT_POINTER_FRAME);
        CastSpell(gpGame->ViewSpells(
            gpGame->GetHero(gpCurPlayer->m_currentHero), 1, NullHandler, 0));
    }
}

// @match-note
// Complete 0x24 frame/slots and message/mouse CFG; all 28 relocation targets
// agree. Residuals are three continuation-jump placements: one extra retail
// jump after the accepted widget path, one after the valid-cell SetPointer,
// and one base-only jump after the invalid-map SetPointer return. Tried both
// shift-condition polarities, nested/flat dialog-result checks, both map/cell
// branch polarities, and both SetPointer arm orders. Revisit only with new TU
// compiler-state evidence or after the SOURCE placeholder census is zero.
VA(0x00465191, 0x31c)
int DimensionDoorHandler(tag_message &message)
{
    if (glTimers[0] < KBTickCount()) {
        gpAdvManager->CompleteDraw(gpAdvManager->m_mapOriginX,
                                   gpAdvManager->m_mapOriginY, 0, 1);
        gpAdvManager->UpdateScreen(0, 0);
    }

    int mouseX = message.payload.mouse.x;
    int mouseY = message.payload.mouse.y;
    int handled = 0;

    switch (message.type) {
    case MESSAGE_WIDGET:
        switch (message.payload.widget.command) {
        case WIDGET_COMMAND_SELECT:
            switch (message.payload.widget.id) {
            case ADVMGR_DIMENSION_DOOR_FIRST_BUTTON:
            case ADVMGR_DIMENSION_DOOR_LAST_BUTTON:
                if (message.payload.widget.parameter &
                    MESSAGE_MODIFIER_LEFT_SHIFT) {
                } else {
                    if (gpWindowManager->m_dialogResult ==
                        ADVMGR_TRAVEL_DIALOG_ACCEPT)
                        handled = 1;
                }
                break;
            }
            break;
        case WIDGET_COMMAND_DESELECT:
            switch (message.payload.widget.id) {
            case ADVMGR_DIMENSION_DOOR_CLOSE_BUTTON:
                gpWindowManager->m_dialogResult = 0;
                handled = 1;
                break;
            }
            break;
        }
        break;

    case MESSAGE_MOUSE_MOVE:
        if (InMapArea(message.payload.mouse.x, message.payload.mouse.y)) {
            mouseX /= ADVMGR_CELL_PIXELS;
            mouseY /= ADVMGR_CELL_PIXELS;
            if (mouseX < 0)
                mouseX = 0;
            if (mouseY < 0)
                mouseY = 0;
            if (mouseX > ADVMGR_DRAW_LAST_CELL)
                mouseX = ADVMGR_DRAW_LAST_CELL;
            if (mouseY > ADVMGR_DRAW_LAST_CELL)
                mouseY = ADVMGR_DRAW_LAST_CELL;

            if (gpAdvManager->m_lastHoverCell != mouseX ||
                gpAdvManager->m_hoverCellY != mouseY) {
                gpAdvManager->m_lastHoverCell = mouseX;
                gpAdvManager->m_hoverCellY = mouseY;
                mapCell *cell = gpAdvManager->GetCell(
                    gpAdvManager->m_mapOriginX + mouseX,
                    gpAdvManager->m_mapOriginY + mouseY);
                if ((cell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG) ||
                    (cell->m_flags & ADVMGR_HOVER_OBJECT_BLOCKED)) {
                    gpWindowManager->m_dialogResult = 0;
                    gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
                } else {
                    gpWindowManager->m_dialogResult = ADVMGR_TRAVEL_DIALOG_ACCEPT;
                    gpMouseManager->SetPointer(ADVMGR_POINTER_MOVE);
                }
            }
        } else {
            gpWindowManager->m_dialogResult = 0;
            gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
            return ADVMGR_DIMENSION_DOOR_UNHANDLED;
        }
        break;
    }

    if (handled) {
        message.payload.widget.id = ADVMGR_DIMENSION_DOOR_FIRST_BUTTON;
        message.payload.widget.command = message.payload.widget.id;
        return ADVMGR_DIMENSION_DOOR_HANDLED;
    }
    return ADVMGR_DIMENSION_DOOR_UNHANDLED;
}

// @match-note
// The retained source-hash object has the complete 0x1c frame/slots, draw CFG, and all
// 161 relocation targets. Its residuals are delinked biased bComboDraw aliases,
// commutative flat-index/min-max evaluation, and three retail alignment NOPs.
// Direct and reversed index/min-max spellings and explicit temporaries compile to
// the same evaluation order. Revisit after a material TU-state change.
VA(0x004654ad, 0x11a9)
int advManager::ComboDraw(int originX, int originY, int animate)
{
    int updateCount;
    int mapCellX;
    int column;
    int mapRow;
    mapCell *cell;
    int mapYValue;

    PollSound();
    if (bShowIt == 0)
        return 0;

    if (m_forceCompleteDraw != 0) {
        CompleteDraw(originX, originY, 0, 1);
        return 1;
    }

    if (animate != 0) {
        giFrameCount += giFrameStep;
        if (giFrameCount < ADVMGR_COMBO_FRAME_LIMIT) {
            Process1WindowsMessage();
            if (glTimers[0] < KBTickCount())
                glTimers[0] = KBTickCount() + ADVMGR_TIMER_DELAY;
            PollSound();
            return 0;
        } else {
            giFrameCount = 0;
        }
    }

    m_previousOriginX = m_mapOriginX;
    m_previousOriginY = m_mapOriginY;
    memset(bComboDraw, 0, ADVMGR_COMBO_CLEAR_BYTES);
    m_comboHeroDrawn = 0;

    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
            if (column + originX >= 0 && column + originX < MAP_WIDTH &&
                mapRow + originY >= 0 && mapRow + originY < MAP_HEIGHT) {
                cell = GetCell(column + originX, mapRow + originY);

                if (cell->m_animatedObject || cell->m_animatedOverlay)
                    ++bComboDraw[column][mapRow];
                if ((cell->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) == 0x28)
                    ++bComboDraw[column][mapRow];
                if ((cell->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) == 1)
                    ++bComboDraw[column][mapRow];

                if (cell->m_triggerType == 0x98) {
                    ++bComboDraw[column][mapRow];
                    ++bComboDraw[column - 1][mapRow];
                    if (GetCloudLookup(column + originX, mapRow + originY) != 0) {
                        bComboDraw[column + 1][mapRow] += ADVMGR_COMBO_CLOUD_MARK;
                        if (mapRow >= 1) {
                            bComboDraw[column][mapRow - 1] += ADVMGR_COMBO_CLOUD_MARK;
                            bComboDraw[column + 1][mapRow - 1] += ADVMGR_COMBO_CLOUD_MARK;
                        }
                    } else {
                        ++bComboDraw[column + 1][mapRow];
                        if (mapRow >= 1) {
                            ++bComboDraw[column][mapRow - 1];
                            ++bComboDraw[column + 1][mapRow - 1];
                        }
                    }
                }

                if (cell->m_triggerType == ADVMGR_HERO_TRIGGER ||
                    cell->m_triggerType == ADVMGR_BOAT_TRIGGER) {
                    ++bComboDraw[column][mapRow];
                    if (GetCloudLookup(column + originX, mapRow + originY) != 0) {
                        bComboDraw[column + 1][mapRow] += ADVMGR_COMBO_CLOUD_MARK;
                        bComboDraw[column][mapRow + 1] += ADVMGR_COMBO_CLOUD_MARK;
                        bComboDraw[column + 1][mapRow + 1] += ADVMGR_COMBO_CLOUD_MARK;
                        bComboDraw[column + 2][mapRow] += ADVMGR_COMBO_CLOUD_MARK;
                        if (mapRow >= 1)
                            bComboDraw[column][mapRow - 1] += ADVMGR_COMBO_CLOUD_MARK;
                        if (column >= 1) {
                            bComboDraw[column - 1][mapRow] += ADVMGR_COMBO_CLOUD_MARK;
                            *(bComboDraw[column - 1] + mapRow + 1) +=
                                ADVMGR_COMBO_CLOUD_MARK;
                            if (column >= 2)
                                bComboDraw[column - 2][mapRow] += ADVMGR_COMBO_CLOUD_MARK;
                            if (mapRow >= 1)
                                ++bComboDraw[column - 2][mapRow - 1];
                        }
                    } else {
                        ++bComboDraw[column + 1][mapRow];
                        ++bComboDraw[column][mapRow + 1];
                        ++bComboDraw[column + 1][mapRow + 1];
                        ++bComboDraw[column + 2][mapRow];
                        if (mapRow >= 1)
                            ++bComboDraw[column][mapRow - 1];
                        if (column >= 1) {
                            ++bComboDraw[column - 1][mapRow];
                            ++bComboDraw[column - 1][mapRow + 1];
                            if (column >= 2)
                                ++bComboDraw[column - 2][mapRow];
                            if (mapRow >= 1)
                                ++bComboDraw[column - 2][mapRow - 1];
                        }
                    }
                }
            }
        }
    }

    for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
        for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow) {
            if (bComboDraw[0][mapRow + column * ADVMGR_COMBO_GRID_CELLS] != 0) {
                if (column + originX < 0 || column + originX >= MAP_WIDTH ||
                    mapRow + originY < 0 || mapRow + originY >= MAP_HEIGHT) {
                    *(bComboDraw[column] + mapRow) = 0;
                } else if (*(bComboDraw[column] + mapRow) < ADVMGR_COMBO_CLOUD_MARK &&
                           GetCloudLookup(column + originX, mapRow + originY) == 0) {
                    *(bComboDraw[column] + mapRow) = 0;
                }
            }
        }
    }

    if (m_heroContextLocked != 0) {
        for (mapRow = ADVMGR_COMBO_HERO_PANEL_TOP;
             mapRow <= ADVMGR_COMBO_HERO_PANEL_BOTTOM - 1; ++mapRow) {
            for (column = ADVMGR_COMBO_HERO_PANEL_LEFT;
                 column <= ADVMGR_COMBO_HERO_PANEL_RIGHT - 1; ++column) {
                ++bComboDraw[column][mapRow];
            }
        }
    }

    if (m_cursorType == ADVMGR_CURSOR_ROUTE) {
        ++bComboDraw[6][5];
        ++bComboDraw[7][5];
        ++bComboDraw[8][5];
    }

    for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
        for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow) {
            cell = GetCell(column + originX, mapRow + originY);
            if (cell->m_triggerType == ADVMGR_MONSTER_TRIGGER) {
                if (gpGame->m_mines[cell->m_objectMetadata].guardianType == ADVMGR_MONSTER_GHOST) {
                    ++bComboDraw[column][mapRow];
                    ++bComboDraw[column + 1][mapRow];
                    if (column < ADVMGR_COMBO_VIEW_CELLS)
                        ++bComboDraw[column + 2][mapRow];
                    if (column > 0)
                        ++bComboDraw[column - 1][mapRow];
                    if (mapRow > 0) {
                        ++bComboDraw[column][mapRow - 1];
                        ++bComboDraw[column + 1][mapRow - 1];
                        if (column < ADVMGR_COMBO_VIEW_CELLS)
                            ++bComboDraw[column + 2][mapRow - 1];
                        if (column > 0)
                            ++bComboDraw[column - 1][mapRow - 1];
                    }
                    if (mapRow > 1) {
                        ++bComboDraw[column][mapRow - 2];
                        ++bComboDraw[column + 1][mapRow - 2];
                        if (column < ADVMGR_COMBO_VIEW_CELLS)
                            ++bComboDraw[column + 2][mapRow - 2];
                        if (column > 0)
                            ++bComboDraw[column - 1][mapRow - 2];
                    }
                } else if (mapRow > 0 && bComboDraw[column][mapRow - 1] != 0) {
                    ++bComboDraw[column][mapRow];
                }
            }
        }
    }

    if (m_visibilityMapValid != 0 && m_visibilityMap != 0) {
        for (mapRow = 1; mapRow < ADVMGR_COMBO_VIEW_CELLS - 1; ++mapRow) {
            for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
                if (bComboDraw[column][mapRow] == 0)
                    continue;
                mapCellX = column + originX;
                mapYValue = mapRow + originY;
                if (mapCellX < 0 || mapCellX > MAP_WIDTH - 1 || mapYValue < 1 ||
                    mapYValue > MAP_HEIGHT - 2)
                    continue;
                if (m_visibilityMap[mapYValue * MAP_WIDTH + mapCellX] != 0)
                    ++bComboDraw[column][mapRow + 1];
                if (m_visibilityMap[(mapYValue - 1) * MAP_WIDTH + mapCellX] != 0)
                    ++bComboDraw[column][mapRow - 1];
            }
        }
    }

    gpMouseManager->m_cursorReady = 0;
    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow)
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow] != 0)
                DrawCell(column + originX, mapRow + originY, column, mapRow,
                         ADVMGR_DRAW_GROUND, 0);

    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow)
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow] != 0)
                DrawCell(column + originX, mapRow + originY, column, mapRow,
                         ADVMGR_DRAW_HERO_SHADOW, 0);

    for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
        if (bComboDraw[column][0] != 0)
            DrawCell(column + originX, originY, column, 0, ADVMGR_DRAW_OBJECT, 0);

    for (mapRow = 1; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow) {
        PollSound();
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow - 1] != 0)
                DrawCell(column + originX, mapRow + originY - 1, column, mapRow - 1,
                         ADVMGR_DRAW_HERO, 0);
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow - 1] != 0)
                DrawCell(column + originX, mapRow + originY - 1, column, mapRow - 1,
                         ADVMGR_DRAW_OVERLAY, 0);
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
            if (column + originX == giDeferObjDrawX && mapRow + originY == giDeferObjDrawY)
                continue;
            if (column + originX == giDeferObjDrawX &&
                mapRow + originY == giDeferObjDrawY + 1) {
                DrawCell(column + originX, mapRow + originY - 1, column, mapRow - 1,
                         ADVMGR_DRAW_OBJECT, 0);
            }
            if (bComboDraw[column][mapRow] != 0)
                DrawCell(column + originX, mapRow + originY, column, mapRow,
                         ADVMGR_DRAW_OBJECT, 0);
        }
    }

    for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
        if (bComboDraw[column][ADVMGR_COMBO_VIEW_CELLS - 1] != 0)
            DrawCell(column + originX, originY + ADVMGR_COMBO_VIEW_CELLS - 1,
                     column, ADVMGR_COMBO_VIEW_CELLS - 1, ADVMGR_DRAW_HERO, 0);
    for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
        if (bComboDraw[column][ADVMGR_COMBO_VIEW_CELLS - 1] != 0)
            DrawCell(column + originX, originY + ADVMGR_COMBO_VIEW_CELLS - 1,
                     column, ADVMGR_COMBO_VIEW_CELLS - 1, ADVMGR_DRAW_OVERLAY, 0);

    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow)
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow] != 0)
                DrawCell(column + originX, mapRow + originY, column, mapRow,
                         ADVMGR_DRAW_OVERLAY_TOP, 0);
    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow)
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column)
            if (bComboDraw[column][mapRow] != 0)
                DrawCell(column + originX, mapRow + originY, column, mapRow,
                         ADVMGR_DRAW_CLOUD, 0);

    DrawAdventureBorder();
    gpMouseManager->m_cursorReady = 1;
    PollSound();
    UpdBottomView(0, 1, 1);

    giLimitUpdMinX = ADVMGR_COMBO_VIEW_CELLS;
    giLimitUpdMinY = ADVMGR_COMBO_VIEW_CELLS;
    giLimitUpdMaxX = 0;
    giLimitUpdMaxY = 0;
    updateCount = 0;
    for (mapRow = 0; mapRow < ADVMGR_COMBO_VIEW_CELLS; ++mapRow) {
        for (column = 0; column < ADVMGR_COMBO_VIEW_CELLS; ++column) {
            if (bComboDraw[column][mapRow] != 0) {
                ++updateCount;
                if (column < giLimitUpdMinX)
                    giLimitUpdMinX = column;
                if (giLimitUpdMaxX < column)
                    giLimitUpdMaxX = column;
                if (giLimitUpdMinY > mapRow)
                    giLimitUpdMinY = mapRow;
                if (giLimitUpdMaxY < mapRow)
                    giLimitUpdMaxY = mapRow;
            }
        }
    }

    giLimitUpdMinX *= ADVMGR_CELL_PIXELS;
    giLimitUpdMinY *= ADVMGR_CELL_PIXELS;
    giLimitUpdMaxX = (giLimitUpdMaxX + 1) * ADVMGR_CELL_PIXELS - 1;
    giLimitUpdMaxY = (giLimitUpdMaxY + 1) * ADVMGR_CELL_PIXELS - 1;
    if (giLimitUpdMinX < ADVMGR_COMBO_UPDATE_MIN)
        giLimitUpdMinX = ADVMGR_COMBO_UPDATE_MIN;
    if (giLimitUpdMaxX > ADVMGR_COMBO_UPDATE_MAX)
        giLimitUpdMaxX = ADVMGR_COMBO_UPDATE_MAX;
    if (giLimitUpdMinY < ADVMGR_COMBO_UPDATE_MIN)
        giLimitUpdMinY = ADVMGR_COMBO_UPDATE_MIN;
    if (giLimitUpdMaxY > ADVMGR_COMBO_UPDATE_MAX)
        giLimitUpdMaxY = ADVMGR_COMBO_UPDATE_MAX;

    if (giLimitUpdMaxX < giLimitUpdMinX || giLimitUpdMaxY < giLimitUpdMinY) {
        giLimitUpdMinX = giLimitUpdMaxX - 1;
        giLimitUpdMinY = giLimitUpdMaxY - 1;
        return 0;
    }
    return 1;
}

VA(0x00466656, 0x38)
int advManager::ComboDraw(int update)
{
    return ComboDraw(m_mapOriginX, m_mapOriginY, update);
}

// @early-stop
// Raw bytes differ only at +0x1dd/+0x1e0, +0x1f4/+0x1f7,
// +0x1fb/+0x1fe, and +0x215/+0x218: four commutative /Od add operand
// orders. The frame, size, logic, and all 18 relocation targets agree.
VA(0x0046668e, 0x338)
void advManager::SetEnvironmentOrigin(int originX, int originY, int stopSounds)
{
    int soundLayer;
    int maxSounds = ADVMGR_SOUND_CELL_COUNT;
    int soundRadius;
    int edgeOffset;

    if (gpSoundManager->m_samplesReady == 0)
        return;

    for (edgeOffset = 0; edgeOffset < ADVMGR_SOUND_CELL_COUNT; ++edgeOffset) {
        if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE) {
            LogInt("SEO a", edgeOffset, ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                   ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                   ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                   ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                   ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                   ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED);
            if (stopSounds != 0) {
                LogInt("SEO b", edgeOffset, ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                       ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                       ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                       ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                       ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED,
                       ADVMGR_ENVIRONMENT_SOUND_LOG_UNUSED);
                gpSoundManager->StopSample(
                    m_loopingSamples[m_activeSounds[edgeOffset].soundId]
                        ->m_activeSample);
                m_activeSounds[edgeOffset].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
                m_activeSounds[edgeOffset].volume =
                    ADVMGR_ENVIRONMENT_SOUND_DEFAULT_VOLUME;
            } else {
                m_activeSounds[edgeOffset].volume =
                    ADVMGR_ENVIRONMENT_SOUND_DEFAULT_VOLUME;
            }
        }
    }

    if (originX == ADVMGR_ENVIRONMENT_SOUND_NONE)
        return;

    if (gConfig.soundVolume != 0) {
        m_activeSoundMask = 0;
        for (soundLayer = ADVMGR_ENVIRONMENT_SOUND_FIRST_LAYER;
             soundLayer <= ADVMGR_ENVIRONMENT_SOUND_LAYER_COUNT; ++soundLayer) {
            InsertSound(originX, originY, 0, soundLayer);
            for (soundRadius = 0; soundRadius < ADVMGR_SOUND_CELL_COUNT;
                 ++soundRadius) {
                for (edgeOffset = 0; edgeOffset < soundRadius * 2; ++edgeOffset) {
                    InsertSound(originX - soundRadius + edgeOffset,
                                originY - soundRadius, soundRadius, soundLayer);
                    InsertSound(originX + soundRadius,
                                originY - soundRadius + edgeOffset, soundRadius,
                                soundLayer);
                    InsertSound(originX + soundRadius - edgeOffset,
                                originY + soundRadius, soundRadius, soundLayer);
                    InsertSound(originX - soundRadius,
                                originY + soundRadius - edgeOffset, soundRadius,
                                soundLayer);
                }
            }
        }

        for (edgeOffset = 0; edgeOffset < ADVMGR_SOUND_CELL_COUNT; ++edgeOffset) {
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE &&
                m_activeSounds[edgeOffset].volume >
                    ADVMGR_ENVIRONMENT_SOUND_MAX_DISTANCE) {
                gpSoundManager->StopSample(
                    m_loopingSamples[m_activeSounds[edgeOffset].soundId]
                        ->m_activeSample);
                m_activeSounds[edgeOffset].soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;
            }
            if (m_activeSounds[edgeOffset].soundId != ADVMGR_ENVIRONMENT_SOUND_NONE &&
                (m_activeSoundMask &
                 (1 << m_activeSounds[edgeOffset].soundId)) != 0) {
                gpSoundManager->ModifySample(
                    m_loopingSamples[m_activeSounds[edgeOffset].soundId]
                        ->m_activeSample,
                    ADVMGR_ENVIRONMENT_SOUND_PAN,
                    ADVMGR_ENVIRONMENT_VOLUME(m_activeSounds[edgeOffset].volume));
            }
        }
    }
}

VA(0x004669c6, 0x69)
void advManager::CheckLoadSample(int index)
{
    if (m_loopingSamples[index] == 0) {
        TrimLoopingSounds(ADVMGR_LOOPING_SOUND_LIMIT);
        sprintf(gText, "loop%04d.82M", index);
        m_loopingSamples[index] = gpResourceManager->GetSample(gText);
    }
}

// @early-stop
// All 0x4c1 bytes are identical after masking 39 relocations. Retail delinks
// 37 switch-table local-label relocations as the containing function.
VA(0x00466a2f, 0x4c1)
int advManager::GetSoundId(int x, int y)
{
    mapCell *currentCell = &m_mapData->Row(y)[x];
    int soundId = ADVMGR_ENVIRONMENT_SOUND_NONE;

    if (!giGroundToTerrain[currentCell->m_terrainImageIndex] &&
        (giGroundShape[currentCell->m_terrainImageIndex] & ADVMGR_SOUND_GROUND_SHAPE_MASK))
        return ADVMGR_SOUND_COASTLINE;

    if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG) {
        switch (currentCell->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK) {
        case ADVMGR_SOUND_OBJECT_ARCHER_HOUSE:
            return ADVMGR_SOUND_DWELLING;
        case ADVMGR_SOUND_OBJECT_DWARF_COTTAGE:
            return ADVMGR_SOUND_DWELLING;
        case ADVMGR_SOUND_OBJECT_PEASANT_HUT:
            return ADVMGR_SOUND_DWELLING;
        case ADVMGR_SOUND_OBJECT_LOG_CABIN:
            return ADVMGR_SOUND_DWELLING;
        case ADVMGR_SOUND_OBJECT_SIRENS:
            return ADVMGR_SOUND_DWELLING;
        case ADVMGR_SOUND_OBJECT_RUINS:
            return ADVMGR_SOUND_RUINS;
        case ADVMGR_SOUND_OBJECT_DERELICT_SHIP:
            return ADVMGR_SOUND_DERELICT_SHIP;
        case ADVMGR_SOUND_OBJECT_TRADING_POST:
            return ADVMGR_SOUND_TRADING_POST;
        case ADVMGR_SOUND_OBJECT_SHRINE_FIRST:
            return ADVMGR_SOUND_SHRINE;
        case ADVMGR_SOUND_OBJECT_SHRINE_SECOND:
            return ADVMGR_SOUND_SHRINE;
        case ADVMGR_SOUND_OBJECT_SHRINE_THIRD:
            return ADVMGR_SOUND_SHRINE;
        case ADVMGR_SOUND_OBJECT_BUOY:
            return ADVMGR_SOUND_BUOY;
        case ADVMGR_SOUND_OBJECT_SHIPWRECK:
            return ADVMGR_SOUND_SHIPWRECK;
        case ADVMGR_SOUND_OBJECT_WHIRLPOOL:
            return ADVMGR_SOUND_COAST;
        case ADVMGR_SOUND_OBJECT_CAMPFIRE:
            return ADVMGR_SOUND_CAMPFIRE;
        case ADVMGR_SOUND_OBJECT_WINDMILL:
            return ADVMGR_SOUND_WINDMILL;
        case ADVMGR_SOUND_OBJECT_FOUNTAIN:
            return ADVMGR_SOUND_FOUNTAIN;
        case ADVMGR_SOUND_OBJECT_ARTESIAN_SPRING:
            return ADVMGR_SOUND_FOUNTAIN;
        case ADVMGR_SOUND_OBJECT_WATERING_HOLE:
            return ADVMGR_SOUND_WATERING_HOLE;
        case ADVMGR_SOUND_OBJECT_STONE_LITHS:
            return ADVMGR_SOUND_STONE_LITHS;
        case ADVMGR_SOUND_OBJECT_ORACLE:
            return ADVMGR_SOUND_ORACLE;
        case ADVMGR_SOUND_OBJECT_WATER_WHEEL:
            return ADVMGR_SOUND_WATER_WHEEL;
        case ADVMGR_SOUND_OBJECT_ALCHEMIST_LAB:
            if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_ALCHEMIST_LAB_ACTION;
            break;
        case ADVMGR_SOUND_OBJECT_MINE:
            if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_MINE;
            break;
        case ADVMGR_SOUND_OBJECT_ABANDONED_MINE:
            if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_ABANDONED_MINE;
            break;
        case ADVMGR_SOUND_OBJECT_SAWMILL:
            if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_SAWMILL;
            break;
        case ADVMGR_SOUND_OBJECT_DAEMON_CAVE:
            if (currentCell->m_triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_DAEMON_CAVE;
            break;
        }
    } else {
        switch (currentCell->m_triggerType) {
        case ADVMGR_SOUND_OBJECT_TAR_PIT:
            return ADVMGR_SOUND_TAR_PIT;
        case ADVMGR_SOUND_OBJECT_LAVA_POOL:
            if (currentCell->m_objectIndex >= ADVMGR_SOUND_ALCHEMIST_FRAME_FIRST &&
                currentCell->m_objectIndex <= ADVMGR_SOUND_ALCHEMIST_FRAME_LAST)
                return ADVMGR_SOUND_ALCHEMIST_LAB;
            else
                return ADVMGR_SOUND_LAVA_POOL;
        case ADVMGR_SOUND_OBJECT_VOLCANO:
            if (currentCell->m_objectTileset == ADVMGR_SOUND_TILESET_SMALL_VOLCANO ||
                currentCell->m_objectTileset == ADVMGR_SOUND_TILESET_LARGE_VOLCANO)
                return ADVMGR_SOUND_LARGE_VOLCANO;
            else
                return ADVMGR_SOUND_SMALL_VOLCANO;
        case ADVMGR_SOUND_OBJECT_WATER_LAKE:
            if (currentCell->m_objectTileset == ADVMGR_SOUND_TILESET_WATER_LAKE_UNUSED)
                break;
            return ADVMGR_SOUND_WATERING_HOLE;
        }

        switch (currentCell->m_objectTileset) {
        case ADVMGR_SOUND_TILESET_STREAM:
            return ADVMGR_SOUND_STREAM;
        case ADVMGR_SOUND_TILESET_WATER:
            if (currentCell->m_objectIndex == ADVMGR_SOUND_SEAGULL_FRAME_FIRST ||
                currentCell->m_objectIndex == ADVMGR_SOUND_SEAGULL_FRAME_LAST)
                return ADVMGR_SOUND_SEAGULLS;
            break;
        }
    }

    return ADVMGR_ENVIRONMENT_SOUND_NONE;
}

// @early-stop
// Exact frame and instruction stream outside +0x2e..+0x3c. Retail evaluates
// MAP_HEIGHT > mapY as a 15-byte reversed compare; MSVC emits the equivalent
// 14-byte compare here. The resulting size delta is one byte, all 9 external
// relocation addresses agree, and semantics-preserving AST operand swaps were
// exhausted. The constant-pool relocation spelling differs but its address agrees.
VA(0x00466ef0, 0x23a)
void advManager::InsertSound(int x, int mapY, int distance, int soundLayer)
{
    int soundSlot;
    int distanceLimit;
    int activeIndex;
    int soundId;

    if (!(x >= 0 && mapY >= 0 && x < MAP_WIDTH && MAP_HEIGHT > mapY))
        return;

    soundId = GetSoundId(x, mapY);
    if (soundId == ADVMGR_ENVIRONMENT_SOUND_NONE)
        return;

    for (activeIndex = 0; activeIndex < ADVMGR_SOUND_CELL_COUNT; ++activeIndex) {
        if (m_activeSounds[activeIndex].soundId == soundId) {
            if (m_activeSounds[activeIndex].volume > distance) {
                m_activeSounds[activeIndex].volume = distance;
                m_activeSoundMask |=
                    1 << m_activeSounds[activeIndex].soundId;
            }
            return;
        }
    }

    if (soundLayer == ADVMGR_ENVIRONMENT_SOUND_FIRST_LAYER)
        return;

    distanceLimit = distance;
    soundSlot = ADVMGR_ENVIRONMENT_SOUND_NONE;
    for (activeIndex = 0; activeIndex < ADVMGR_SOUND_CELL_COUNT;
         ++activeIndex) {
        if (m_activeSounds[activeIndex].volume > distanceLimit) {
            distanceLimit = m_activeSounds[activeIndex].volume;
            soundSlot = activeIndex;
        }
    }

    if (soundSlot != ADVMGR_ENVIRONMENT_SOUND_NONE) {
        if (m_activeSounds[soundSlot].soundId !=
            ADVMGR_ENVIRONMENT_SOUND_NONE) {
            gpSoundManager->StopSample(
                m_loopingSamples[m_activeSounds[soundSlot].soundId]
                    ->m_activeSample);
        }
        m_activeSounds[soundSlot].soundId = soundId;
        m_activeSounds[soundSlot].volume = distance;
        CheckLoadSample(soundId);
        m_loopingSamples[soundId]->m_volume =
            ADVMGR_ENVIRONMENT_VOLUME(distance);
        m_loopingSamples[soundId]->m_loopCount = 0;
        m_loopingSamples[soundId]->m_channelType =
            ADVMGR_ENVIRONMENT_SOUND_CHANNEL_TYPE;
        gpSoundManager->MemorySample(m_loopingSamples[soundId]);
        m_activeSoundMask ^=
            1 << m_activeSounds[soundSlot].soundId;
    }
}

// @match-note
// The exact 0x24 frame/slots, CFG, instruction stream outside +0x343..+0x359,
// and all 44 relocation targets agree. Retail emits a 23-byte load/OR/store for
// m_eventFlags; ours folds to the equivalent 13-byte memory OR, accounting for
// the entire ten-byte size delta. Direct `|=`, explicit load/OR/store, enum/int
// temporaries, and both operand orders all fold to the memory OR. Revisit only
// after a material TU-state change or in the last-mile phase.
VA(0x0046712a, 0x40f)
void advManager::TeleportTo(hero *mapHero, int destinationX, int destinationY,
                            int, int skipMapChange)
{
    int savedShow11;
    int terrain5;
    mapCell *oldCell2;
    int oldCellFlag26;
    int unused47;
    mapCell *destinationCell29;
    int fizzleTime36;
    town *occupiedTown47;

    savedShow11 = bShowIt;
    if (skipMapChange == 0) {
        SendMapChange(ADVMGR_TELEPORT_MAP_CHANGE, mapHero->m_id,
                      static_cast<unsigned char>(destinationX),
                      static_cast<unsigned char>(destinationY),
                      ADVMGR_TELEPORT_MAP_CHANGE_VALUE, 0, 0);
    }

    destinationCell29 = GetCell(destinationX, destinationY);
    oldCell2 = GetCell(mapHero->m_x, mapHero->m_y);
    if (mapHero->m_locationType == HERO_TOWN_LOCATION) {
        occupiedTown47 = gpGame->GetTown(mapHero->m_occupiedTown);
        occupiedTown47->m_occupyingHeroId = ADVMGR_INVALID_HERO;
    }

    oldCellFlag26 = 0;
    if (oldCell2->m_flags & ADVMGR_TELEPORT_CELL_OBJECT_FLAG) {
        oldCell2->m_flags -= ADVMGR_TELEPORT_CELL_OBJECT_FLAG;
        oldCellFlag26 = 1;
    } else {
        gpGame->RestoreCell(mapHero->m_x, mapHero->m_y,
                            mapHero->m_locationType, mapHero->m_occupiedTown,
                            0, ADVMGR_TELEPORT_RESTORE_MODE);
    }

    CompleteDraw(0);
    if (gbThisNetHumanPlayer[giCurPlayer] == 0) {
        if ((gConfig.blackoutComputer == 0 &&
             MapExtraPosAndAdjacentsSet(mapHero->m_x, mapHero->m_y,
                                        giCurWatchPlayerBit)) ||
            MapExtraPosAndAdjacentsSet(destinationX, destinationY,
                                       giCurWatchPlayerBit)) {
            bShowIt = 1;
        } else {
            bShowIt = 0;
        }
    }

    if (savedShow11 != 0)
        HideRoute(1, 1, 1);

    if (bShowIt != 0) {
        m_mapOriginX = destinationX - ADVMGR_TELEPORT_VIEW_CENTER;
        m_mapOriginY = destinationY - ADVMGR_TELEPORT_VIEW_CENTER;
        DelayMilli(ADVMGR_TELEPORT_DELAY);
    }

    mapHero->m_x = destinationX;
    mapHero->m_y = destinationY;
    gpGame->SetVisibility(
        m_mapOriginX + ADVMGR_TELEPORT_VIEW_CENTER,
        m_mapOriginY + ADVMGR_TELEPORT_VIEW_CENTER, giCurPlayer,
        giVisRange[mapHero->m_secondarySkills[HERO_SKILL_SCOUTING]] +
            (static_cast<unsigned int>(mapHero->HasArtifact(
                 ADVMGR_TELEPORT_TELESCOPE_ARTIFACT)) >= 1));

    if (bShowIt != 0) {
        destinationCell29->m_flags |= ADVMGR_TELEPORT_CELL_OBJECT_FLAG;
        gpWindowManager->SaveFizzleSource(
            ADVMGR_UPDATE_VIEWPORT_ORIGIN, ADVMGR_UPDATE_VIEWPORT_ORIGIN,
            ADVMGR_UPDATE_VIEWPORT_SIZE, ADVMGR_UPDATE_VIEWPORT_SIZE);
        CompleteDraw(0);
        PollSound();
        fizzleTime36 = ADVMGR_TELEPORT_FIZZLE_TIME;
        if (gbThisNetHumanPlayer[giCurPlayer] == 0)
            fizzleTime36 -= ADVMGR_TELEPORT_REMOTE_FIZZLE_ADJUSTMENT;
        gpWindowManager->FizzleForward(
            ADVMGR_UPDATE_VIEWPORT_ORIGIN, ADVMGR_UPDATE_VIEWPORT_ORIGIN,
            ADVMGR_UPDATE_VIEWPORT_SIZE, ADVMGR_UPDATE_VIEWPORT_SIZE,
            ADVMGR_ENVIRONMENT_SOUND_NONE, 0, 0);
        PollSound();
    } else {
        mapHero->m_locationType = destinationCell29->m_triggerType;
        mapHero->m_occupiedTown = destinationCell29->m_objectMetadata;
        if (oldCellFlag26 != 0) {
            destinationCell29->m_flags |= ADVMGR_TELEPORT_CELL_OBJECT_FLAG;
        } else {
            destinationCell29->m_triggerType = ADVMGR_HERO_TRIGGER;
            destinationCell29->m_objectMetadata =
                static_cast<unsigned char>(mapHero->m_id);
        }
        if (m_cursorType == BOAT_CURSOR_TYPE) {
            mapHero->m_eventFlags =
                HERO_EVENT_EMBARKED | mapHero->m_eventFlags;
        }
        m_cursorActive = 0;
    }

    SetEnvironmentOrigin(m_mapOriginX + ADVMGR_TELEPORT_VIEW_CENTER,
                         m_mapOriginY + ADVMGR_TELEPORT_VIEW_CENTER, 1);
    terrain5 = giGroundToTerrain[destinationCell29->m_terrainImageIndex];
    if (m_currentTerrain != terrain5) {
        m_currentTerrain = terrain5;
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[m_currentTerrain]);
    }
    Reseed(0, 0);
    UpdateRadar(1, 0);
    CompleteDraw(0);
    ForceNewHover();
}

VA(0x00467539, 0x1fb)
void advManager::DimensionDoor(void)
{
    hero *targetHero;
    heroWindow *dimensionDoorWindow;
    int x;
    int y;
    mapCell *targetCell;

    dimensionDoorWindow = new heroWindow(0, 0, "dimdoor.bin");
    if (dimensionDoorWindow == 0)
        MemError();
    gpWindowManager->DoDialog(dimensionDoorWindow, DimensionDoorHandler, 0);
    delete dimensionDoorWindow;

    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (gpWindowManager->m_dialogResult == ADVMGR_TRAVEL_DIALOG_ACCEPT) {
        x = m_lastHoverCell + m_mapOriginX;
        y = m_hoverCellY + m_mapOriginY;
        targetCell = GetCell(x, y);
        if (((targetHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
             giGroundToTerrain[targetCell->m_terrainImageIndex]) ||
            (!(targetHero->m_eventFlags & HERO_EVENT_EMBARKED) &&
             !giGroundToTerrain[targetCell->m_terrainImageIndex])) {
            NormalDialog("Dimension Door failed!!!",
                         ADVMGR_OPTION_DIALOG_MESSAGE,
                         ADVMGR_OPTION_DIALOG_NONE,
                         ADVMGR_OPTION_DIALOG_NONE,
                         ADVMGR_OPTION_DIALOG_NONE, 0,
                         ADVMGR_OPTION_DIALOG_NONE, 0,
                         ADVMGR_OPTION_DIALOG_NONE, 0);
            UpdateRadar(1, 0);
        } else {
            gpSoundManager->SwitchAmbientMusic(ADVMGR_TRAVEL_MUSIC);
            TeleportTo(targetHero, x, y, 0, 0);
            gpSoundManager->SwitchAmbientMusic(
                giTerrainToMusicTrack[m_currentTerrain]);
        }
        gpGame->GetHero(gpCurPlayer->m_currentHero)
            ->UseSpell(ADVMGR_DIMENSION_DOOR_SPELL);
    } else {
        UpdateRadar(1, 0);
    }
}

VA(0x00467734, 0x129)
int TownPortalHandler(tag_message &message)
{
    tag_message choiceMessage;

    if (!gpSoundManager->MusicPlaying() && gpAdvManager->m_active == 1) {
        gpSoundManager->SwitchAmbientMusic(
            giTerrainToMusicTrack[gpAdvManager->m_currentTerrain]);
    }

    if (message.type == ADVMGR_TOWN_PORTAL_MESSAGE) {
        switch (message.payload.widget.command) {
        case ADVMGR_TOWN_PORTAL_COMMAND_SELECT:
            switch (message.payload.widget.id) {
            case ADVMGR_TOWN_PORTAL_FIRST_CHOICE:
            case ADVMGR_TOWN_PORTAL_LAST_CHOICE:
                choiceMessage.type = ADVMGR_TOWN_PORTAL_MESSAGE;
                choiceMessage.payload.widget.id = ADVMGR_TOWN_PORTAL_CHOICE_WIDGET;
                choiceMessage.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_CHOICE;
                townPortalWin->BroadcastMessage(choiceMessage);
                giTownPortalChoice = choiceMessage.payload.widget.data.value;
                gpWindowManager->m_dialogResult = message.payload.widget.id;
                message.payload.widget.id = ADVMGR_TOWN_PORTAL_CLOSE_COMMAND;
                message.payload.widget.command = message.payload.widget.id;
                return ADVMGR_TOWN_PORTAL_HANDLED;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return ADVMGR_TOWN_PORTAL_UNHANDLED;
}

// @early-stop
// Exact 0x40 frame, 0x43e size, CFG, and all bytes outside the signed loop
// comparison at +0x2bb..+0x2c7. Retail has `8b45d4 3945f8 0f8d`; ours has
// the equivalent operand-reversed `8b45f8 3945d4 0f8e`. Direct/reversed,
// negated, empty-arm, `| 0`, semantic-name, and AST permutations do not steer
// this TU-cumulative /Od operand order (tu-cumulative-eval-order.md).
VA(0x0046785d, 0x43e)
void advManager::TownGate(int spellId)
{
    int distance0;
    hero *targetHero;
    tag_message message;
    int selectedTownIndex;
    int nearestDistance;
    int townListIndex;

    nearestDistance = ADVMGR_TOWN_PORTAL_DISTANCE_LIMIT;
    selectedTownIndex = ADVMGR_INVALID_HERO;
    targetHero = gpGame->GetHero(gpCurPlayer->m_currentHero);

    if (gpCurPlayer->m_townCount == 0) {
        NormalDialog("No available town.  Spell Failed!!!",
                     ADVMGR_OPTION_DIALOG_MESSAGE,
                     ADVMGR_OPTION_DIALOG_NONE,
                     ADVMGR_OPTION_DIALOG_NONE,
                     ADVMGR_OPTION_DIALOG_NONE, 0,
                     ADVMGR_OPTION_DIALOG_NONE, 0,
                     ADVMGR_OPTION_DIALOG_NONE, 0);
        return;
    }
    if (targetHero->m_eventFlags & HERO_EVENT_EMBARKED) {
        NormalDialog(
            "Spell Failed!!!  You must be on land for this spell to work.",
            ADVMGR_OPTION_DIALOG_MESSAGE,
            ADVMGR_OPTION_DIALOG_NONE,
            ADVMGR_OPTION_DIALOG_NONE,
            ADVMGR_OPTION_DIALOG_NONE, 0,
            ADVMGR_OPTION_DIALOG_NONE, 0,
            ADVMGR_OPTION_DIALOG_NONE, 0);
        return;
    }

    if (spellId == ADVMGR_TOWN_PORTAL_SPELL) {
        townPortalWin = new heroWindow(ADVMGR_TOWN_PORTAL_WINDOW_X,
                                       ADVMGR_TOWN_PORTAL_WINDOW_Y,
                                       "townport.bin");
        sprintf(gText, "{Town Portal}\n\nSelect town to port to.");
        message.type = ADVMGR_TOWN_PORTAL_MESSAGE;
        message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_TEXT;
        message.payload.widget.id = ADVMGR_TOWN_PORTAL_TITLE_WIDGET;
        message.payload.widget.data.text = gText;
        townPortalWin->BroadcastMessage(message);

        for (townListIndex = 0;
             townListIndex < gpCurPlayer->m_townCount;
             ++townListIndex) {
            sprintf(gText, gpGame->m_castleRecs[
                               gpCurPlayer->TownId(townListIndex)].m_name);
            message.type = ADVMGR_TOWN_PORTAL_MESSAGE;
            message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_ADD_TOWN;
            message.payload.widget.id = ADVMGR_TOWN_PORTAL_CHOICE_WIDGET;
            message.payload.widget.data.text = gText;
            townPortalWin->BroadcastMessage(message);
        }
        message.payload.widget.command = ADVMGR_TOWN_PORTAL_COMMAND_FINISH;
        message.payload.widget.data.text = 0;
        townPortalWin->BroadcastMessage(message);
        gpWindowManager->DoDialog(townPortalWin, TownPortalHandler, 0);
        selectedTownIndex = giTownPortalChoice;
        delete townPortalWin;
        if (gpWindowManager->m_dialogResult ==
            ADVMGR_TOWN_PORTAL_FIRST_CHOICE) {
            return;
        }
    } else {
        for (townListIndex = 0;
             townListIndex < gpCurPlayer->m_townCount;
             ++townListIndex) {
            distance0 =
                abs(gpGame->m_castleRecs[
                        gpCurPlayer->m_townIds[townListIndex]].m_y -
                    targetHero->m_y) +
                abs(gpGame->m_castleRecs[
                        gpCurPlayer->m_townIds[townListIndex]].m_x -
                    targetHero->m_x);
            if (distance0 < nearestDistance) {
                nearestDistance = distance0;
                selectedTownIndex = townListIndex;
            }
        }
    }

    if (gpGame->m_castleRecs[
            gpCurPlayer->m_townIds[selectedTownIndex]].m_occupyingHeroId !=
        ADVMGR_INVALID_HERO) {
        NormalDialog("Nearest town occupied.  Spell Failed!!!",
                     ADVMGR_OPTION_DIALOG_MESSAGE,
                     ADVMGR_OPTION_DIALOG_NONE,
                     ADVMGR_OPTION_DIALOG_NONE,
                     ADVMGR_OPTION_DIALOG_NONE, 0,
                     ADVMGR_OPTION_DIALOG_NONE, 0,
                     ADVMGR_OPTION_DIALOG_NONE, 0);
        return;
    }

    gpSoundManager->SwitchAmbientMusic(ADVMGR_TRAVEL_MUSIC);
    TeleportTo(
        targetHero,
        gpGame->m_castleRecs[
            gpCurPlayer->m_townIds[selectedTownIndex]].m_x,
        gpGame->m_castleRecs[
            gpCurPlayer->m_townIds[selectedTownIndex]].m_y,
        0, 0);
    targetHero->UseSpell(spellId);
    gpGame->m_castleRecs[
        gpCurPlayer->m_townIds[selectedTownIndex]].m_occupyingHeroId =
        targetHero->m_id;
    gpGame->m_castleRecs[
        gpCurPlayer->m_townIds[selectedTownIndex]].GiveSpells(0);
    targetHero->m_locationType = HERO_TOWN_LOCATION;
    targetHero->m_occupiedTown =
        gpCurPlayer->m_townIds[selectedTownIndex];
    gpSoundManager->SwitchAmbientMusic(
        giTerrainToMusicTrack[m_currentTerrain]);
}

// @match-note
// The exact 0x40 frame/slots, complete CFG, 0x5ac size, and 41 relocation sites
// agree after manual interior-alias resolution. The first code residual is the
// CurrentHero /Ob1 continuation at +0x174 versus retail +0x180; the screen-bound
// sums at +0x3e8..+0x40d reverse commutative stack loads, and the unreachable
// +0x5a3 jump has a different local target. Direct/accessor hero spellings and
// both coordinate operand orders were checked. The two apparent relocation
// extras resolve to normalDirTable+2 at retail RVA 0xfaa7a; one is delinked as
// the interior string symbol at RVA 0xfaa79. Revisit after material TU-state change.
VA(0x00467c9b, 0x5ac)
void advManager::SummonBoat(void) {
    int boatIndex9;
    mapCell* destinationCell;
    int foundBoat;
    hero* currentHero11;
    int destinationX10;
    int direction5;
    int destinationY15;
    int foundDestination9;
    int currentHeroId3;
    boatRecord* boat1;
    mapCell* oldBoatCell26;
    int screenX4;
    int screenY3;
    int fizzleHeight;
    int fizzleWidth;

    currentHero11 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    foundDestination9 = 0;
    foundBoat = 0;
    destinationCell = GetCell(
        m_mapOriginX + ADVMGR_SUMMON_CENTER_OFFSET,
        m_mapOriginY + ADVMGR_SUMMON_CENTER_OFFSET
    );
    if (!giGroundToTerrain[destinationCell->m_terrainImageIndex]) {
        return;
    } else {

        for (direction5 = 0; direction5 < ADVMGR_SUMMON_DIRECTION_COUNT; ++direction5) {
            destinationX10 =
                normalDirTable[direction5].x + m_mapOriginX + ADVMGR_SUMMON_CENTER_OFFSET;
            destinationY15 =
                normalDirTable[direction5].y + m_mapOriginY + ADVMGR_SUMMON_CENTER_OFFSET;
            if (destinationX10 < 0 || destinationX10 >= MAP_WIDTH || destinationY15 < 0
                || destinationY15 >= MAP_HEIGHT) {
                continue;
            }

            destinationCell = GetCell(destinationX10, destinationY15);
            if (destinationCell->m_objectIndex == 0xff && destinationCell->m_triggerType == 0
                && !giGroundToTerrain[destinationCell->m_terrainImageIndex]) {
                foundDestination9 = 1;
                break;
            }
        }

        if (foundDestination9) {
            currentHeroId3 = gpCurPlayer->CurrentHero();
            for (boatIndex9 = 0; boatIndex9 < ADVMGR_SUMMON_BOAT_COUNT; ++boatIndex9) {
                if (gpGame->m_boatSlots[boatIndex9] != -1
                    && gpGame->m_boats[boatIndex9].heroId
                           == (currentHeroId3 | ADVMGR_SUMMON_OCCUPIED_FLAG)) {
                    foundBoat = 1;
                    break;
                }
            }

            if (!foundBoat) {
                for (boatIndex9 = 0; boatIndex9 < ADVMGR_SUMMON_BOAT_COUNT; ++boatIndex9) {
                    if (gpGame->m_boatSlots[boatIndex9] != -1
                        && (gpGame->m_boats[boatIndex9].heroId & ADVMGR_SUMMON_OCCUPIED_FLAG)
                        && gpGame->m_boats[boatIndex9].owner == giCurPlayer
                        && abs(gpGame->m_boats[boatIndex9].y - currentHero11->m_y)
                                   + abs(gpGame->m_boats[boatIndex9].x - currentHero11->m_x)
                               > ADVMGR_SUMMON_MIN_DISTANCE) {
                        foundBoat = 1;
                        break;
                    }
                }
            }

            if (foundBoat) {
                boat1 = &gpGame->m_boats[boatIndex9];
                oldBoatCell26 = GetCell(boat1->x, boat1->y);
                gpGame->RestoreCell(
                    boat1->x,
                    boat1->y,
                    boat1->savedTriggerType,
                    boat1->savedEventData,
                    0,
                    ADVMGR_SUMMON_RESTORE_MODE
                );

                if (boat1->x >= m_mapOriginX && boat1->x < m_mapOriginX + ADVMGR_HOVER_VIEW_CELLS
                    && boat1->y >= m_mapOriginY
                    && boat1->y < m_mapOriginY + ADVMGR_HOVER_VIEW_CELLS) {
                    screenX4 = (boat1->x - m_mapOriginX) * ADVMGR_CELL_PIXELS
                               - ADVMGR_SUMMON_FIZZLE_X_OFFSET;
                    if (screenX4 < ADVMGR_SUMMON_SCREEN_MARGIN) {
                        screenX4 = ADVMGR_SUMMON_SCREEN_MARGIN;
                    }
                    screenY3 = (boat1->y - m_mapOriginY) * ADVMGR_CELL_PIXELS
                               - ADVMGR_SUMMON_FIZZLE_X_OFFSET;
                    if (screenY3 < ADVMGR_SUMMON_SCREEN_MARGIN) {
                        screenY3 = ADVMGR_SUMMON_SCREEN_MARGIN;
                    }

                    fizzleWidth = ADVMGR_SUMMON_FIZZLE_WIDTH;
                    fizzleHeight = ADVMGR_SUMMON_FIZZLE_HEIGHT;
                    if (screenX4 + fizzleWidth >= ADVMGR_SUMMON_SCREEN_LIMIT) {
                        fizzleWidth = ADVMGR_SUMMON_SCREEN_LIMIT - screenX4;
                    }
                    if (screenY3 + fizzleHeight >= ADVMGR_SUMMON_SCREEN_LIMIT) {
                        fizzleHeight = ADVMGR_SUMMON_SCREEN_LIMIT - screenY3;
                    }
                    gpWindowManager
                        ->SaveFizzleSource(screenX4, screenY3, fizzleWidth, fizzleHeight);
                    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                    gpWindowManager
                        ->FizzleForward(screenX4, screenY3, fizzleWidth, fizzleHeight, -1, 0, 0);
                }

                boat1->x = static_cast<signed char>(
                    normalDirTable[direction5].x + m_mapOriginX + ADVMGR_SUMMON_CENTER_OFFSET
                );
                boat1->y = static_cast<signed char>(
                    normalDirTable[direction5].y + m_mapOriginY + ADVMGR_SUMMON_CENTER_OFFSET
                );
                boat1->savedTriggerType = destinationCell->m_triggerType;
                boat1->savedEventData = static_cast<unsigned char>(destinationCell->m_objectMetadata);
                destinationCell->m_triggerType = ADVMGR_SUMMON_BOAT_TRIGGER;
                destinationCell->m_objectMetadata = boatIndex9;

                gpWindowManager->SaveFizzleSource(
                    ADVMGR_SUMMON_TARGET_X,
                    ADVMGR_SUMMON_TARGET_Y,
                    ADVMGR_SUMMON_TARGET_WIDTH,
                    ADVMGR_SUMMON_TARGET_HEIGHT
                );
                CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
                gpWindowManager->FizzleForward(
                    ADVMGR_SUMMON_TARGET_X,
                    ADVMGR_SUMMON_TARGET_Y,
                    ADVMGR_SUMMON_TARGET_WIDTH,
                    ADVMGR_SUMMON_TARGET_HEIGHT,
                    -1,
                    0,
                    0
                );
            }
        }

        UpdateScreen(0, 0);
        Reseed(0, 0);
        if (!foundBoat) {
            NormalDialog(
                "Summon Boat failed!!!",
                ADVMGR_OPTION_DIALOG_MESSAGE,
                ADVMGR_OPTION_DIALOG_NONE,
                ADVMGR_OPTION_DIALOG_NONE,
                ADVMGR_OPTION_DIALOG_NONE,
                0,
                ADVMGR_OPTION_DIALOG_NONE,
                0,
                ADVMGR_OPTION_DIALOG_NONE,
                0
            );
        }
        return;
    }
}

VA(0x00468247, 0x4d9)
void advManager::ShowRoute(int redraw, int, int updateButton)
{
    int routeReachable8;
    int pathFound5;
    int routeX1;
    mapCell *nextCell7;
    int previousDirection0;
    hero *currentHero0;
    int routeY1;
    int direction;
    int terrainCost;
    int remainingMobility2;
    int pathIndex;
    int currentTerrain0;
    mapCell *currentCell2;
    int routeFrame;
    int buttonFrame;

    routeReachable8 = 0;
    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;

    if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO) {
        HideRoute(redraw, 0, 1);
        return;
    }

    currentHero0 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    if (currentHero0->m_destinationX == ADVMGR_INVALID_CELL) {
        HideRoute(redraw, 1, 1);
        return;
    }

    pathFound5 = gpSearchArray->BuildPath(
        currentHero0->m_x, currentHero0->m_y,
        currentHero0->m_destinationX, currentHero0->m_destinationY,
        ADVMGR_ROUTE_PATH_COST_LIMIT);
    if (gpSearchArray->m_pathLength > 0 && pathFound5 > 0) {
        memset(m_visibilityMap, 0, MAP_WIDTH * MAP_HEIGHT * 2);
        m_visibilityMapValid = 1;
        remainingMobility2 = currentHero0->m_remainingMobility;
        routeX1 = currentHero0->m_x;
        routeY1 = currentHero0->m_y;

        for (pathIndex = gpSearchArray->m_pathLength - 1;
             pathIndex >= 0; --pathIndex) {
            direction =
                gpSearchArray->m_storage.path.directions[pathIndex + 1];
            currentCell2 = GetCell(routeX1, routeY1);
            routeX1 += normalDirTable[direction].x;
            routeY1 += normalDirTable[direction].y;
            nextCell7 = GetCell(routeX1, routeY1);
            currentTerrain0 = giGroundToTerrain[currentCell2->m_terrainImageIndex];
            terrainCost = CalcTerrainCost(
                giGroundToTerrain[nextCell7->m_terrainImageIndex], direction & 1,
                ADVMGR_ROUTE_TERRAIN_COST_INFINITY,
                currentHero0->m_secondarySkills[HERO_SKILL_PATHFINDING],
                currentCell2->m_isRoad, nextCell7->m_isRoad);
            remainingMobility2 -= CalcTerrainCost(
                currentTerrain0, direction & 1, remainingMobility2,
                currentHero0->m_secondarySkills[HERO_SKILL_PATHFINDING],
                currentCell2->m_isRoad, nextCell7->m_isRoad);

            if (direction & 1) {
                if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_0)
                    routeFrame = 0;
                else if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_1)
                    routeFrame = 1;
                else if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_2)
                    routeFrame = 2;
                else if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_3)
                    routeFrame = 3;
                else if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_4)
                    routeFrame = 4;
                else if (terrainCost == ADVMGR_ROUTE_DIAGONAL_COST_5)
                    routeFrame = 5;
                else
                    routeFrame = 1;
            } else {
                if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_0)
                    routeFrame = 0;
                else if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_1)
                    routeFrame = 1;
                else if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_2)
                    routeFrame = 2;
                else if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_3)
                    routeFrame = 3;
                else if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_4)
                    routeFrame = 4;
                else if (terrainCost == ADVMGR_ROUTE_STRAIGHT_COST_5)
                    routeFrame = 5;
                else
                    routeFrame = 1;
            }

            if (pathIndex == 0) {
                m_visibilityMap[routeY1 * (MAP_WIDTH | 0) + routeX1] = 1;
            } else {
                previousDirection0 =
                    gpSearchArray->m_storage.path.directions[pathIndex];
                m_visibilityMap[routeY1 * (MAP_WIDTH | 0) + routeX1] =
                    static_cast<unsigned short>(
                        gbArrow[previousDirection0][direction | 0] +
                        routeFrame * ADVMGR_ROUTE_ARROW_FRAME_STRIDE +
                        ADVMGR_ROUTE_ARROW_FRAME_OFFSET);
            }

            if (remainingMobility2 < 0) {
                m_visibilityMap[routeY1 * (MAP_WIDTH | 0) + routeX1] +=
                    ADVMGR_ROUTE_DAY_MASK;
            } else {
                routeReachable8 = 1;
            }
        }

        if (updateButton) {
            buttonFrame = routeReachable8 ? ADVMGR_BUTTON_DISABLE
                                          : ADVMGR_BUTTON_ENABLE;
            gpWindowManager->BroadcastMessage(
                ADVMGR_BUTTON_MESSAGE, buttonFrame, ADVMGR_BUTTON_TARGET,
                ADVMGR_BUTTON_BROADCAST_FLAGS);
        }
    } else {
        HideRoute(redraw, 1, 1);
    }

    if (redraw) {
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

VA(0x00468720, 0x107)
void advManager::HideRoute(int redraw, int clearDestination, int updateButton)
{
    hero *currentHero;

    if (!gbThisNetHumanPlayer[giCurPlayer] &&
        (!giDebugLevel || !giShowComputerRoute))
        return;

    if (updateButton) {
        gpWindowManager->BroadcastMessage(
            ADVMGR_BUTTON_MESSAGE, ADVMGR_BUTTON_ENABLE,
            ADVMGR_BUTTON_TARGET, ADVMGR_BUTTON_BROADCAST_FLAGS);
    }

    if (clearDestination &&
        gpCurPlayer->m_currentHero != ADVMGR_INVALID_HERO) {
        currentHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
        currentHero->m_destinationX = ADVMGR_INVALID_CELL;
        currentHero->m_destinationY = ADVMGR_INVALID_CELL;
    }

    if (!m_visibilityMapValid)
        return;

    m_visibilityMapValid = 0;
    if (redraw) {
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

// @early-stop
// all 52 instructions, both inline-continuation jumps, and ten relocation targets match
VA(0x00468827, 0x8d)
void advManager::CheckDimHero(void) {
    if (gbThisNetHumanPlayer[giCurPlayer]) {
        if (gpCurPlayer->CurrentHero() == ADVMGR_INVALID_HERO) {
            return;
        }
        if (!gpGame->IsMobile(gpCurPlayer->CurrentHero())) {
            ShowRoute(1, 0, 0);
            UpdateHeroLocators(1, 1);
            gpAdvManager->CheckDimNextHeroBut();
        }
    }
}

VA(0x004688b4, 0x6b)
void advManager::CheckDimNextHeroBut(void) {
    int frame;
    if (!gbThisNetHumanPlayer[giCurPlayer] || !gpCurPlayer->HasMobileHero())
        frame = ADVMGR_BUTTON_ENABLE;
    else
        frame = ADVMGR_BUTTON_DISABLE;
    gpWindowManager->BroadcastMessage(ADVMGR_BUTTON_MESSAGE, frame,
                                      ADVMGR_BUTTON_BROADCAST_ARG,
                                      ADVMGR_BUTTON_BROADCAST_FLAGS);
}

VA(0x0046891f, 0x138)
void advManager::SeedTo(int targetX, int targetY)
{
    hero *currentHero;

    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;
    if (gpCurPlayer->m_currentHero == ADVMGR_INVALID_HERO)
        return;

    currentHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    if (!giSeedingValid) {
        gpSearchArray->SeedPosition(
            currentHero->m_x, currentHero->m_y, m_cursorDirection,
            ADVMGR_ROUTE_PATH_COST_LIMIT,
            m_cursorType == ADVMGR_POINTER_SAIL, 0,
            currentHero->m_remainingMobility,
            currentHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
            targetX, targetY, 0, 1);
    } else if (!giFullySeeded) {
        gpSearchArray->SeedPosition(
            currentHero->m_x, currentHero->m_y, m_cursorDirection,
            ADVMGR_ROUTE_PATH_COST_LIMIT,
            m_cursorType == ADVMGR_POINTER_SAIL, 0,
            currentHero->m_remainingMobility,
            currentHero->m_secondarySkills[HERO_SKILL_PATHFINDING],
            targetX, targetY, 1, 1);
    }
}

VA(0x00468a57, 0x5f)
void advManager::ForceNewHover(void)
{
    int x;
    int y;

    if (!gbThisNetHumanPlayer[giCurPlayer])
        return;
    gpMouseManager->MouseCoords(x, y);
    m_lastHoverCell = ADVMGR_INVALID_CELL;
    ProcessHover(x, y);
}

// @early-stop
// All non-table bytes match. The 32-byte jump table has the same eight case
// offsets; retail delinks its entries as ScreenScroll while base retains local labels.
VA(0x00468ab6, 0x1a6)
void advManager::ScreenScroll(int direction, int updatePointer)
{
    int originX;
    int originY;

    originX = m_mapOriginX;
    originY = m_mapOriginY;
    iLastScrollTime = KBTickCount();

    switch (direction) {
    case ADVMGR_SCROLL_NORTH:
        --originY;
        break;
    case ADVMGR_SCROLL_NORTH_EAST:
        ++originX;
        --originY;
        break;
    case ADVMGR_SCROLL_EAST:
        ++originX;
        break;
    case ADVMGR_SCROLL_SOUTH_EAST:
        ++originX;
        ++originY;
        break;
    case ADVMGR_SCROLL_SOUTH:
        ++originY;
        break;
    case ADVMGR_SCROLL_SOUTH_WEST:
        --originX;
        ++originY;
        break;
    case ADVMGR_SCROLL_WEST:
        --originX;
        break;
    case ADVMGR_SCROLL_NORTH_WEST:
        --originX;
        --originY;
        break;
    }

    if (updatePointer)
        gpMouseManager->SetPointer(
            direction + ADVMGR_HOVER_SCROLL_FRAME_FIRST);

    if (originX < ADVMGR_SCROLL_MIN_ORIGIN)
        originX = ADVMGR_SCROLL_MIN_ORIGIN;
    if (MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN < originX)
        originX = MAP_WIDTH - ADVMGR_VIEW_EDGE_MARGIN;
    if (originY < ADVMGR_SCROLL_MIN_ORIGIN)
        originY = ADVMGR_SCROLL_MIN_ORIGIN;
    if (MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN < originY)
        originY = MAP_HEIGHT - ADVMGR_VIEW_EDGE_MARGIN;

    if (m_mapOriginX != originX || m_mapOriginY != originY) {
        DemobilizeCurrHero();
        m_mapOriginX = originX;
        m_mapOriginY = originY;
        UpdateRadar(1, 0);
        CompleteDraw(0);
        UpdateScreen(0, 0);
    }
}

VA(0x00468c5c, 0x1bb)
void advManager::CheckScreenScroll(void)
{
    int mouseX6;
    int mouseY1;
    int oldOriginX9;
    int oldOriginY3;

    if (KBTickCount() - iLastScrollTime > ADVMGR_SCROLL_TICK_INTERVAL) {
        iLastScrollTime = KBTickCount();
        oldOriginX9 = m_mapOriginX;
        oldOriginY3 = m_mapOriginY;
        gpMouseManager->MouseCoords(mouseX6, mouseY1);

        if (mouseX6 >= 0 && mouseX6 < ADVMGR_SCREEN_WIDTH &&
            mouseY1 >= 0 && mouseY1 < ADVMGR_SCREEN_HEIGHT) {
            if (mouseX6 < ADVMGR_SCROLL_BORDER) {
                if (mouseY1 < ADVMGR_SCROLL_BORDER)
                    ScreenScroll(ADVMGR_SCROLL_NORTH_WEST, 1);
                else if (mouseY1 >
                         ADVMGR_SCREEN_HEIGHT - ADVMGR_SCROLL_BORDER)
                    ScreenScroll(ADVMGR_SCROLL_SOUTH_WEST, 1);
                else
                    ScreenScroll(ADVMGR_SCROLL_WEST, 1);
            } else if (mouseX6 >
                       ADVMGR_SCREEN_WIDTH - ADVMGR_SCROLL_BORDER - 1) {
                if (mouseY1 < ADVMGR_SCROLL_BORDER)
                    ScreenScroll(ADVMGR_SCROLL_NORTH_EAST, 1);
                else if (mouseY1 >
                         ADVMGR_SCREEN_HEIGHT - ADVMGR_SCROLL_BORDER)
                    ScreenScroll(ADVMGR_SCROLL_SOUTH_EAST, 1);
                else
                    ScreenScroll(ADVMGR_SCROLL_EAST, 1);
            } else if (mouseY1 < ADVMGR_SCROLL_BORDER) {
                ScreenScroll(ADVMGR_SCROLL_NORTH, 1);
            } else if (mouseY1 >
                       ADVMGR_SCREEN_HEIGHT - ADVMGR_SCROLL_BORDER) {
                ScreenScroll(ADVMGR_SCROLL_SOUTH, 1);
            }
        }

        if (gpMouseManager->m_cursorFrame >=
                ADVMGR_HOVER_SCROLL_FRAME_FIRST &&
            gpMouseManager->m_cursorFrame < ADVMGR_HOVER_SCROLL_FRAME_END &&
            m_mapOriginX == oldOriginX9 && m_mapOriginY == oldOriginY3) {
            gpMouseManager->SetPointer(ADVMGR_POINTER_DEFAULT);
        }
    }
}

VA(0x00468e17, 0x91)
int advManager::MouseInScrollZone(void)
{
    int x;
    int y;

    gpMouseManager->MouseCoords(x, y);
    if (x >= 0 && x < ADVMGR_SCREEN_WIDTH && y >= 0 && y < ADVMGR_SCREEN_HEIGHT) {
        if (x < ADVMGR_SCROLL_BORDER || x > ADVMGR_SCREEN_WIDTH - ADVMGR_SCROLL_BORDER - 1
            || y < ADVMGR_SCROLL_BORDER || y > ADVMGR_SCREEN_HEIGHT - ADVMGR_SCROLL_BORDER)
            return 1;
    }
    return 0;
}

VA(0x00468ea8, 0x2b8)
void advManager::SetInitialMapOrigin(void)
{
    game *gameState;  // outer-scope retail slot retained in the frame
    town *currentTown9;
    playerData *initialPlayer8;
    hero *initialHero5;
    town *initialTown9;

    gpWindowManager->BroadcastMessage(
        ADVMGR_BUTTON_MESSAGE, ADVMGR_BUTTON_ENABLE,
        ADVMGR_BUTTON_TARGET, ADVMGR_BUTTON_BROADCAST_FLAGS);
    m_hoverCellY = 0;
    m_lastHoverCell = m_hoverCellY;
    m_cursorActive = 0;
    gbHeroMoving = 0;

    if (gbThisNetHumanPlayer[giCurPlayer] &&
        gpCurPlayer->CurrentTown() != ADVMGR_INVALID_CELL) {
        currentTown9 = reinterpret_cast<town *>(
            &gpGame->m_castleRecs[gpCurPlayer->CurrentTown()]);
        m_mapOriginX = currentTown9->m_x - ADVMGR_VIEW_CENTER_OFFSET;
        m_mapOriginY = currentTown9->m_y - ADVMGR_VIEW_CENTER_OFFSET;
    } else if (gbThisNetHumanPlayer[giCurPlayer] &&
               gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO) {
        MobilizeCurrHero(0);
    } else {
        if (gbThisNetHumanPlayer[giCurPlayer])
            initialPlayer8 = gpCurPlayer;
        else
            initialPlayer8 = reinterpret_cast<playerData *>(
                &gpGame->m_players[giThisGamePos]);

        if (initialPlayer8->m_heroCount > 0) {
            initialHero5 =
                &gpGame->m_heroRecs[initialPlayer8->m_heroIds[0]];
            m_mapOriginX = initialHero5->m_x - ADVMGR_VIEW_CENTER_OFFSET;
            m_mapOriginY = initialHero5->m_y - ADVMGR_VIEW_CENTER_OFFSET;
        } else if (initialPlayer8->m_townCount > 0) {
            initialTown9 = reinterpret_cast<town *>(
                &gpGame->m_castleRecs[initialPlayer8->m_townIds[0]]);
            m_mapOriginX = initialTown9->m_x - ADVMGR_VIEW_CENTER_OFFSET;
            m_mapOriginY = initialTown9->m_y - ADVMGR_VIEW_CENTER_OFFSET;
        } else {
            m_mapOriginX = 0;
            m_mapOriginY = 0;
        }
    }

    m_currentTerrain = giGroundToTerrain[
        GetCell(m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
                m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET)->m_terrainImageIndex];
    gpSoundManager->SwitchAmbientMusic(
        giTerrainToMusicTrack[m_currentTerrain]);
    SetEnvironmentOrigin(
        m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
        m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET, 1);
    Reseed(0, 0);
    CheckDimNextHeroBut();
}

VA(0x00469160, 0x1be)
void advManager::LoadRemote(void)
{
    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpMouseManager->SetPointer(
            "advmice.mse", ADVMGR_POINTER_DEFAULT,
            ADVMGR_DEFAULT_POINTER_FRAME);
    }

    gpGame->LoadGame(gConfig.rmtRCName, 0, 1);
    if ((gpGame->m_day != 1 ||
         (gpGame->m_week == 1 && gpGame->m_month == 1)) &&
        gbRemoteOn && gbThisNetHumanPlayer[giCurPlayer]) {
        gpSoundManager->m_samplesReady = 1;
        gpSoundManager->SwitchAmbientMusic(WAIT_AMBIENT_MUSIC);
        gpSoundManager->m_samplesReady = 0;
        giForceSwitchMusic = KBTickCount();
    }

    if (gpGame->m_playerDead[giCurPlayer])
        ComputeAdvNetControl();

    if (gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->CancelComputerScreen();
        gbThisNetGotAdventureControl = 1;
        gpSoundManager->m_samplesReady = 0;
    }

    gpGame->DoNewTurn();
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdateRadar(1, 0);
    UpdBottomView(1, 1, 1);
    gpAdvManager->ForceNewHover();
    SendMapChange(
        ADVMGR_REMOTE_LOAD_MAP_CHANGE, 0, 0, 0,
        ADVMGR_DEFAULT_POINTER_FRAME, 0, 0);
    gpSoundManager->m_samplesReady = 1;
}

// @early-stop
// All non-table bytes and 17 external targets match. The command lookup and
// seven case offsets match; retail delinks nine local relocations as this function.
VA(0x0046931e, 0x20c)
char * advManager::CheckHandleNet(void)
{
    AdventureRemotePacket *packet9;
    int playerExited5;
    SPlayerExit exitInfo4;

    packet9 = reinterpret_cast<AdventureRemotePacket *>(
        GetRemoteData(ADVMGR_REMOTE_DATA_REQUEST));
    if (packet9 &&
        (packet9->type == ADVMGR_REMOTE_PACKET_TYPE_GAME ||
         packet9->type == ADVMGR_REMOTE_PACKET_TYPE_ALT)) {
        switch (packet9->command) {
        case ADVMGR_REMOTE_COMMAND_SAVE_GAME:
            playerExited5 = packet9->savePlayerExited;
            if (!gpGame->ReceiveSaveGame(
                    packet9->saveDataSize, packet9->saveCrc,
                    packet9->saveTransmitCrc, packet9->sender)) {
                ShutDown(0);
            }
            if (playerExited5) {
                exitInfo4.netPosition = packet9->sender;
                exitInfo4.gamePosition = static_cast<signed char>(
                    NetPosToGamePos(packet9->sender));
                exitInfo4.updateNetworkControl = 0;
                exitInfo4.eliminated = 1;
                exitInfo4.hostReported = 1;
                exitInfo4.timedOut = 1;
                ReceiveRemotePlayerExit(exitInfo4);
            }
            LoadRemote();
            break;

        case ADVMGR_REMOTE_COMMAND_POP_NET_BOX:
            PopNetBox(packet9->payload, packet9->sender);
            break;

        case ADVMGR_REMOTE_COMMAND_COMBAT:
            if (gbInCombat) {
                return reinterpret_cast<char *>(packet9);
            } else {
                DoNetCombat(reinterpret_cast<char *>(packet9));
            }
            break;

        case ADVMGR_REMOTE_COMMAND_PLAYER_EXIT:
            LogStr("Receive Remote Player Exit");
            ReceiveRemotePlayerExit(
                *reinterpret_cast<SPlayerExit *>(packet9->payload));
            break;

        case ADVMGR_REMOTE_COMMAND_HOST_PLAYER_EXIT:
            LogStr("Host Reports Player Exit");
            ReceiveHostReportsPlayerExit(
                packet9->sender,
                *reinterpret_cast<SPlayerExit *>(packet9->payload), 0);
            break;

        case ADVMGR_REMOTE_COMMAND_GROUP_MAP_CHANGE:
            ProcessIncomingGroupMapChange(packet9->payload);
            break;

        default:
            return reinterpret_cast<char *>(packet9);
        }
    }
    return 0;
}

VA(0x0046952a, 0xcd)
int advManager::CheckHandleNetPlayerWait(
    struct tag_message &message, int doMain)
{
    if (message.type == ADVMGR_REMOTE_WAIT_MOUSE_MESSAGE)
        gpMouseManager->Main(message);

    CheckDoMain(1, doMain);
    if (message.type == ADVMGR_REMOTE_WAIT_COMMAND_MESSAGE) {
        switch (message.payload.widget.command) {
        case ADVMGR_REMOTE_WAIT_POP_NET_BOX_COMMAND:
            PopNetBox(0, -1);
            break;

        case ADVMGR_REMOTE_WAIT_EXIT_COMMAND:
            if (message.payload.widget.parameter & ADVMGR_REMOTE_WAIT_EXIT_MODIFIER_MASK) {
                message.type = ADVMGR_REMOTE_WAIT_EXIT_MESSAGE;
                message.payload.widget.command = 1;
                return ADVMGR_REMOTE_WAIT_EXIT_RESULT;
            }

        default:
            break;
        }
    }

    UpdBottomView(0, 1, 1);
    return 0;
}

// @early-stop
// Retail adds continuation jumps at +0x14e and +0x1c9. Excluding those two
// five-byte jumps, every opcode/operand, the 0x28 frame/slots and CFG, and all
// six relocation targets match.
VA(0x004695f7, 0x1d5)
void advManager::TrimLoopingSounds(int maxSamples)
{
    if (giHighMemBuffer > 0)
        maxSamples += giHighMemBuffer / ADVMGR_HIGH_MEMORY_BUFFER_DIVISOR;

    if (MAP_WIDTH != ADVMGR_XLARGE_MAP_SIZE)
        ++maxSamples;

    if (maxSamples >= ADVMGR_LOOPING_SAMPLE_COUNT)
        return;

    int loadedSampleCount = 0;
    char retainedSamples[ADVMGR_LOOPING_SAMPLE_COUNT];
    memset(retainedSamples, 0, sizeof(retainedSamples));

    int soundIndex;
    for (soundIndex = 0; soundIndex < ADVMGR_SOUND_CELL_COUNT; ++soundIndex) {
        if (m_activeSounds[soundIndex].soundId >= 0 &&
            m_activeSounds[soundIndex].soundId < ADVMGR_LOOPING_SAMPLE_COUNT) {
            ++retainedSamples[m_activeSounds[soundIndex].soundId];
        }
    }

    for (soundIndex = 0; soundIndex < ADVMGR_LOOPING_SAMPLE_COUNT; ++soundIndex) {
        if (retainedSamples[soundIndex] != 0)
            ++loadedSampleCount;
    }

    if (loadedSampleCount < maxSamples) {
        for (soundIndex = 0; soundIndex < ADVMGR_LOOPING_SAMPLE_COUNT;
             ++soundIndex) {
            if (retainedSamples[soundIndex] == 0 &&
                m_loopingSamples[soundIndex] != 0) {
                ++retainedSamples[soundIndex];
                ++loadedSampleCount;
                if (loadedSampleCount >= maxSamples)
                    break;
            }
        }
    }

    for (soundIndex = 0; soundIndex < ADVMGR_LOOPING_SAMPLE_COUNT; ++soundIndex) {
        if (m_loopingSamples[soundIndex] != 0 &&
            retainedSamples[soundIndex] == 0) {
            gpResourceManager->Dispose(m_loopingSamples[soundIndex]);
            m_loopingSamples[soundIndex] = 0;
        }
    }
}

VA(0x004697cc, 0xd5)
void advManager::DisableButtons(void)
{
    if (gpAdvManager->m_active != 1)
        return;
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.payload.widget.command = ADVMGR_BUTTON_DISABLE;
    msg.payload.widget.data.value = ADVMGR_BUTTON_TARGET;
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_1; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_2; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_3; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_4; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_5; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_6; m_adventureWindow->BroadcastMessage(msg);
}

VA(0x004698a1, 0xd5)
void advManager::EnableButtons(void)
{
    if (gpAdvManager->m_active != 1)
        return;
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.payload.widget.command = ADVMGR_BUTTON_ENABLE;
    msg.payload.widget.data.value = ADVMGR_BUTTON_TARGET;
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_1; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_2; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_3; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_4; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_5; m_adventureWindow->BroadcastMessage(msg);
    msg.payload.widget.id = ADVMGR_BUTTON_SLOT_6; m_adventureWindow->BroadcastMessage(msg);
}

// @early-stop
// Relocation-masked instructions are byte-identical. All eight relocation
// sites and target addresses agree; objdiff retains only retail/base names for
// the "% " and source-path string-pool entries.
VA(0x00469976, 0x145)
void advManager::SaveAdventureBorder(void)
{
    if (m_adventureBorder != 0)
        return;

    m_adventureBorder = static_cast<unsigned char *>(
        BaseAlloc(ADVMGR_BORDER_BUFFER_SIZE, ADVMGR_SOURCE_FILE,
                  ADVMGR_BORDER_FREE_LINE + 4));
    unsigned char *savedPixel = m_adventureBorder;
    unsigned char *screenPixel = gpWindowManager->m_screen->m_pixels;
    int row;
    for (row = 0; row < ADVMGR_BORDER_EDGE_SIZE; ++row) {
        memcpy(savedPixel, screenPixel, ADVMGR_BORDER_ROW_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_ROW_BYTES;
    }
    for (row = ADVMGR_BORDER_EDGE_SIZE; row < ADVMGR_BORDER_MIDDLE_END;
         ++row) {
        memcpy(savedPixel, screenPixel, ADVMGR_BORDER_SIDE_BYTES);
        memcpy(savedPixel + ADVMGR_BORDER_SIDE_BYTES,
               screenPixel + ADVMGR_BORDER_MIDDLE_END,
               ADVMGR_BORDER_SIDE_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_SAVED_SIDE_BYTES;
    }
    for (row = ADVMGR_BORDER_MIDDLE_END; row < ADVMGR_SCREEN_HEIGHT; ++row) {
        memcpy(savedPixel, screenPixel, ADVMGR_BORDER_ROW_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_ROW_BYTES;
    }
}

// @early-stop
// All 87 instructions match after masking relocation bytes, and all six
// relocation sites and target addresses agree. The remaining objdiff score is
// delinked symbol identity only.
VA(0x00469abb, 0x134)
void advManager::DrawAdventureBorder(void)
{
    if (m_adventureBorder == 0)
        return;
    if (gbNoBorder != 0)
        return;

    unsigned char *screenPixel = gpWindowManager->m_screen->m_pixels;
    unsigned char *savedPixel = m_adventureBorder;
    int row;
    for (row = 0; row < ADVMGR_BORDER_EDGE_SIZE; ++row) {
        memcpy(screenPixel, savedPixel, ADVMGR_BORDER_ROW_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_ROW_BYTES;
    }
    for (row = ADVMGR_BORDER_EDGE_SIZE; row < ADVMGR_BORDER_MIDDLE_END;
         ++row) {
        memcpy(screenPixel, savedPixel, ADVMGR_BORDER_SIDE_BYTES);
        memcpy(screenPixel + ADVMGR_BORDER_MIDDLE_END,
               savedPixel + ADVMGR_BORDER_SIDE_BYTES,
               ADVMGR_BORDER_SIDE_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_SAVED_SIDE_BYTES;
    }
    for (row = ADVMGR_BORDER_MIDDLE_END; row < ADVMGR_SCREEN_HEIGHT; ++row) {
        memcpy(screenPixel, savedPixel, ADVMGR_BORDER_ROW_BYTES);
        screenPixel += ADVMGR_BORDER_SCREEN_PITCH;
        savedPixel += ADVMGR_BORDER_ROW_BYTES;
    }
}

// @match-note
// Complete 0x4 frame, edge/interior loops, duplicated eligibility/exclusion CFG,
// success writes, and all 52 relocation targets. The instruction streams align;
// residuals are the six delinked static identities and four excluded-Y comparison
// operand orientations. Tried cursor/end loop reversals, a shared body (wrong CFG),
// compound duplicated predicates, and both excluded-Y inequality orientations.
// Revisit only with new static-symbol/TU evidence or in the last-mile phase.
VA(0x00469bef, 0x3d3)
int advManager::FindAdjacentMonster(int originX, int originY, int *monsterX,
                                    int *monsterY, int excludedX, int excludedY)
{
    s_adjacentMonsterEndX = originX + ADVMGR_ADJACENT_MONSTER_END_OFFSET;
    s_adjacentMonsterEndY = originY + ADVMGR_ADJACENT_MONSTER_END_OFFSET;

    if (originX > 0 && originY > 0 && originX < MAP_WIDTH - 1 &&
        originY < MAP_HEIGHT - 1) {
        for (s_adjacentMonsterX = originX - ADVMGR_ADJACENT_MONSTER_RADIUS;
             s_adjacentMonsterX < s_adjacentMonsterEndX;
             ++s_adjacentMonsterX) {
            for (s_adjacentMonsterY = originY - ADVMGR_ADJACENT_MONSTER_RADIUS;
                 s_adjacentMonsterY < s_adjacentMonsterEndY;
                 ++s_adjacentMonsterY) {
                if (m_mapData->Row(s_adjacentMonsterY)[s_adjacentMonsterX]
                        .m_triggerType == ADVMGR_ADJACENT_MONSTER_TRIGGER) {
                    if (originY > s_adjacentMonsterY) {
                        if ((GetCell(originX, originY)->m_objectIndex ==
                                 ADVMGR_ADJACENT_OBJECT_INDEX_NONE ||
                             GetCell(originX, originY)->m_objectTileset ==
                                 ADVMGR_CLEAR_GROUND_TILESET ||
                             (GetCell(originX, originY)->m_flags &
                              ADVMGR_HOVER_UNREACHABLE)) &&
                            (s_adjacentMonsterX != excludedX ||
                             excludedY != s_adjacentMonsterY))
                            goto foundAdjacentMonster;
                    } else if (s_adjacentMonsterX != excludedX ||
                               excludedY != s_adjacentMonsterY) {
                        goto foundAdjacentMonster;
                    }
                }
            }
        }
    } else {
        if (originX == MAP_WIDTH - 1)
            s_adjacentMonsterEndX = originX + 1;
        if (originY == MAP_HEIGHT - 1)
            s_adjacentMonsterEndY = originY + 1;
        if (originX == 0)
            s_adjacentMonsterMinX = 0;
        else
            s_adjacentMonsterMinX = originX - ADVMGR_ADJACENT_MONSTER_RADIUS;
        if (originY == 0)
            s_adjacentMonsterMinY = 0;
        else
            s_adjacentMonsterMinY = originY - ADVMGR_ADJACENT_MONSTER_RADIUS;

        for (s_adjacentMonsterX = s_adjacentMonsterMinX;
             s_adjacentMonsterX < s_adjacentMonsterEndX;
             ++s_adjacentMonsterX) {
            for (s_adjacentMonsterY = s_adjacentMonsterMinY;
                 s_adjacentMonsterY < s_adjacentMonsterEndY;
                 ++s_adjacentMonsterY) {
                if (m_mapData->Row(s_adjacentMonsterY)[s_adjacentMonsterX]
                        .m_triggerType == ADVMGR_ADJACENT_MONSTER_TRIGGER) {
                    if (originY > s_adjacentMonsterY) {
                        if ((GetCell(originX, originY)->m_objectIndex ==
                                 ADVMGR_ADJACENT_OBJECT_INDEX_NONE ||
                             GetCell(originX, originY)->m_objectTileset ==
                                 ADVMGR_CLEAR_GROUND_TILESET ||
                             (GetCell(originX, originY)->m_flags &
                              ADVMGR_HOVER_UNREACHABLE)) &&
                            (s_adjacentMonsterX != excludedX ||
                             excludedY != s_adjacentMonsterY))
                            goto foundAdjacentMonster;
                    } else if (s_adjacentMonsterX != excludedX ||
                               excludedY != s_adjacentMonsterY) {
                        goto foundAdjacentMonster;
                    }
                }
            }
        }
    }
    return 0;

foundAdjacentMonster:
    *monsterX = s_adjacentMonsterX;
    *monsterY = s_adjacentMonsterY;
    return 1;
}

// @match-note
// Complete 0xc frame/slots, including the retail non-advancing dead-player
// loop, and all 16 relocation targets agree. The only two divergences load the
// local player before giCurPlayer instead of giCurPlayer before the local at
// the two loop guards. Both operand spellings of player!=giCurPlayer were
// tried. Revisit only with new TU compiler-state evidence or after the SOURCE
// placeholder census is zero.
VA(0x00469fc2, 0x125)
void ComputeAdvNetControl(void)
{
    if (!gbRemoteOn) {
        gbThisNetGotAdventureControl = 1;
    } else {
        int selectedPlayer = -1;
        int player;
        if (gpGame->m_playerDead[giCurPlayer]) {
            player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
            while (giCurPlayer != player) {
                if (!gpGame->m_playerDead[player] && gbHumanPlayer[player]) {
                    gbThisNetGotAdventureControl = gbThisNetHumanPlayer[player];
                    return;
                }
            }
        }

        player = (giCurPlayer + 1) % GAME_PLAYER_COUNT;
        while (giCurPlayer != player) {
            player = (player + 1) % GAME_PLAYER_COUNT;
            if (!gpGame->m_playerDead[player] && gbHumanPlayer[player])
                selectedPlayer = player;
        }
        gbThisNetGotAdventureControl = gbThisNetHumanPlayer[selectedPlayer];
    }
}

// @early-stop
// target loads MAP_WIDTH/MAP_HEIGHT before the local operand; base loads the local first
VA(0x0046a0e7, 0xf6)
int MapExtraPosAndAdjacentsSet(int x, int y, unsigned char mask)
{
    if (mapExtra[MAP_WIDTH * y + x] & mask)
        return 1;
    for (int checkX = x - 1; checkX <= x + 1; ++checkX) {
        if (checkX < 0 || checkX >= MAP_WIDTH)
            continue;
        for (int checkY = y - 1; checkY <= y + 1; ++checkY) {
            if (checkY < 0 || checkY > MAP_HEIGHT - 1)
                continue;
            if (mapExtra[checkY * MAP_WIDTH + checkX] & mask)
                return 1;
        }
    }
    return 0;
}

// @match-note
// Complete 0x6c frame and retail slots (Y/window/icon/X adjustment/count/Y
// adjustment/row/piece/end/pixel/order[48]/X); all 54 relocation targets
// agree. Residuals are three string-pool names, the commuted Y*5+X*2 lowering,
// and pixel<rowEnd lowering as jbe instead of jae. Tried both term orders,
// constant-left multiplication, and an explicit >= break (which regressed).
// Revisit only with new expression/TU evidence or after the SOURCE placeholder
// census is zero.
VA(0x0046a1dd, 0x4c6)
void advManager::ViewPuzzle(void)
{
    gpGame->SetupPuzzlePieces(giCurPlayer, 0);
    unsigned char puzzleOrderLocal[ADVMGR_PUZZLE_PIECE_COUNT] = {
        23, 7, 44, 5, 24, 47, 1, 39, 16, 36, 11, 45,
        31, 2, 30, 38, 43, 4, 3, 14, 40, 37, 34, 0,
        12, 17, 35, 42, 15, 8, 26, 41, 28, 46, 10, 22,
        21, 6, 32, 18, 19, 29, 13, 27, 9, 20, 33, 25
    };
    int puzzlePiecesVisible = 0;

    gpSoundManager->SwitchAmbientMusic(ADVMGR_PUZZLE_MUSIC);
    gpMouseManager->SetPointer("advmice.mse", ADVMGR_POINTER_DEFAULT,
                               ADVMGR_DEFAULT_POINTER_FRAME);
    icon *puzzleIconObjectPointer = gpResourceManager->GetIcon("puzzle.icn");
    int pieceIndexPosition;
    for (pieceIndexPosition = 0;
         pieceIndexPosition < ADVMGR_PUZZLE_PIECE_COUNT;
         ++pieceIndexPosition)
        puzzleIconObjectPointer->DrawToBuffer(0, 0, pieceIndexPosition, 0);

    gpWindowManager->UpdateScreenRegion(
        ADVMGR_PUZZLE_VIEW_ORIGIN, ADVMGR_PUZZLE_VIEW_ORIGIN,
        ADVMGR_PUZZLE_VIEW_SIZE, ADVMGR_PUZZLE_VIEW_SIZE);
    gpWindowManager->SaveFizzleSource(
        ADVMGR_PUZZLE_VIEW_ORIGIN, ADVMGR_PUZZLE_VIEW_ORIGIN,
        ADVMGR_PUZZLE_VIEW_SIZE, ADVMGR_PUZZLE_VIEW_SIZE);
    heroWindow *puzzleWindowObject = new heroWindow(
        ADVMGR_PUZZLE_WINDOW_X, ADVMGR_PUZZLE_WINDOW_Y, "viewpuzl.bin");
    if (puzzleWindowObject == 0)
        MemError();
    gpWindowManager->AddWindow(puzzleWindowObject, -1, 1);

    int puzzleXViewCurrent = gpGame->m_ultimateArtifactX -
                             ADVMGR_PUZZLE_COORDINATE_OFFSET;
    int puzzleYTop = gpGame->m_ultimateArtifactY -
                     ADVMGR_PUZZLE_COORDINATE_OFFSET;
    int xAdjustmentOffset = 0;
    int yAdjustmentOffsetLocal = 0;
    xAdjustmentOffset = (gpGame->m_ultimateArtifactX +
                         gpGame->m_ultimateArtifactY) %
                            ADVMGR_PUZZLE_ALIGNMENT_DIVISOR -
                        1;
    yAdjustmentOffsetLocal = (gpGame->m_ultimateArtifactX * 2 +
                              gpGame->m_ultimateArtifactY * 5) %
                                 ADVMGR_PUZZLE_ALIGNMENT_DIVISOR -
                             1;
    if ((gpGame->m_ultimateArtifactX + gpGame->m_ultimateArtifactY) %
            ADVMGR_PUZZLE_ALIGNMENT_DIVISOR ==
        1) {
        if (xAdjustmentOffset > 0)
            ++xAdjustmentOffset;
        else if (xAdjustmentOffset < 0)
            --xAdjustmentOffset;
    } else if ((gpGame->m_ultimateArtifactX +
                gpGame->m_ultimateArtifactY) % 2 == 1) {
        if (yAdjustmentOffsetLocal > 0)
            ++yAdjustmentOffsetLocal;
        else if (yAdjustmentOffsetLocal < 0)
            --yAdjustmentOffsetLocal;
    }

    puzzleXViewCurrent += xAdjustmentOffset;
    puzzleYTop += yAdjustmentOffsetLocal;
    PuzzleDraw(puzzleXViewCurrent, puzzleYTop, gpGame->m_ultimateArtifactX,
               gpGame->m_ultimateArtifactY);

    int rowCounterNumber;
    unsigned char *pixelIterator;
    unsigned char *rowLimitAddress;
    for (rowCounterNumber = ADVMGR_PUZZLE_VIEW_ORIGIN;
         rowCounterNumber < ADVMGR_PUZZLE_VIEW_END;
         ++rowCounterNumber) {
        pixelIterator = gpWindowManager->m_screen->m_pixels +
                        rowCounterNumber * ADVMGR_SCREEN_WIDTH +
                        ADVMGR_PUZZLE_VIEW_ORIGIN;
        rowLimitAddress = pixelIterator + ADVMGR_PUZZLE_VIEW_SIZE;
        for (; pixelIterator < rowLimitAddress; ++pixelIterator)
            *pixelIterator = gColorTableTan[*pixelIterator];
    }

    for (pieceIndexPosition = 0;
         pieceIndexPosition < ADVMGR_PUZZLE_PIECE_COUNT;
         ++pieceIndexPosition) {
        if (!BitTest(puzzlePiecesRemoved, pieceIndexPosition)) {
            puzzleIconObjectPointer->DrawToBuffer(
                0, 0, puzzleOrderLocal[pieceIndexPosition], 0);
            ++puzzlePiecesVisible;
        }
    }
    if (puzzlePiecesVisible != ADVMGR_PUZZLE_PIECE_COUNT)
        gpWindowManager->FizzleForward(
            ADVMGR_PUZZLE_VIEW_ORIGIN, ADVMGR_PUZZLE_VIEW_ORIGIN,
            ADVMGR_PUZZLE_VIEW_SIZE, ADVMGR_PUZZLE_VIEW_SIZE,
            ADVMGR_PUZZLE_FIZZLE_TIME, 0, 0);
    else
        gpWindowManager->ReleaseFizzleSource();

    gpWindowManager->DoDialog(puzzleWindowObject, EventWindowHandler, 0);
    delete puzzleWindowObject;
    CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
    UpdateScreen(0, 0);
    UpdateRadar(1, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
}

// @early-stop
// all 49 instructions and five relocation targets match; residual is the TU object span
VA(0x0046a6a3, 0x81)
void advManager::PuzzleDraw(int left, int top, int right, int bottom)
{
    gbDrawingPuzzle = 1;
    CompleteDraw(left, top, 0, 0);
    gbDrawingPuzzle = 0;
    IconToBitmap(m_puzzleIcon, gpWindowManager->m_screen,
                 (right - left) * ADVMGR_PUZZLE_TILE_SIZE - ADVMGR_PUZZLE_X_TRIM,
                 (bottom - top) * ADVMGR_PUZZLE_TILE_SIZE, 0, 1, 0, 0,
                 ADVMGR_SCREEN_HEIGHT, ADVMGR_SCREEN_HEIGHT, 0);
}

// @early-stop
// Excluding the 0x10-byte jump table at RVA 0x6a9a2, every instruction and
// operand matches after normalizing branch destinations. All 46 relocation
// sites and target addresses agree; the residual is the delinked local table
// base/labels and string-pool symbol identities.
VA(0x0046a724, 0x2ac)
void advManager::AdvPanel(void)
{
    heroWindow *adventurePanel;
    {
        TrimLoopingSounds(ADVMGR_LOOPING_SOUND_LIMIT);
        gpMouseManager->SetPointer("advmice.mse", ADVMGR_POINTER_DEFAULT,
                                   ADVMGR_DEFAULT_POINTER_FRAME);
        int heroWasMobilized = m_heroContextLocked;
        tag_message message;
        DemobilizeCurrHero();

        adventurePanel = new heroWindow(
            ADVMGR_PANEL_WINDOW_X, ADVMGR_PANEL_WINDOW_Y, "apanel.bin");
        if (adventurePanel == 0)
            MemError();
        if (gpCurPlayer->CurrentHero() == ADVMGR_INVALID_HERO) {
            message.type = MESSAGE_WIDGET;
            message.payload.widget.id = ADVMGR_PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
            message.payload.widget.data.value = ADVMGR_BUTTON_TARGET;
            adventurePanel->BroadcastMessage(message);
            message.payload.widget.id = ADVMGR_PANEL_DISABLED_WIDGET;
            message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
            message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
            adventurePanel->BroadcastMessage(message);
        }

        gpWindowManager->DoDialog(adventurePanel, APanelHandler, 0);
        delete adventurePanel;
        switch (gpWindowManager->m_dialogResult) {
            case ADVMGR_PANEL_SCENARIO_INFO:
                if (gbInCampaign) {
                    SetEnvironmentOrigin(-1, -1, 1);
                    gpGame->ShowCampaignInfo(1, 0);
                    SetEnvironmentOrigin(
                        m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
                        m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET, 1);
                    RedrawAdvScreen(1, 0);
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[m_currentTerrain]);
                } else if (xIsPlayingExpansionCampaign) {
                    SetEnvironmentOrigin(-1, -1, 1);
                    xCampaign.ShowInfo(1, 0);
                    SetEnvironmentOrigin(
                        m_mapOriginX + ADVMGR_VIEW_CENTER_OFFSET,
                        m_mapOriginY + ADVMGR_VIEW_CENTER_OFFSET, 1);
                    RedrawAdvScreen(1, 0);
                    gpSoundManager->SwitchAmbientMusic(
                        giTerrainToMusicTrack[m_currentTerrain]);
                } else {
                    gpGame->ShowScenInfo();
                }
                break;
            case ADVMGR_PANEL_SEARCH:
                ProcessSearch(ADVMGR_INVALID_CELL, ADVMGR_INVALID_CELL);
                break;
            case ADVMGR_PANEL_VIEW_WORLD:
                ViewWorld(ADVENTURE_SPELL_VIEW_ALL, 0, 0);
                break;
            case ADVMGR_PANEL_VIEW_PUZZLE:
                ViewPuzzle();
                break;
        }

        if (heroWasMobilized)
            MobilizeCurrHero(0);
    }
}

// @match-note
// Complete 0x18 frame, shift-help/selection CFG, and all 8 external relocations agree.
// The diff first diverges at the delinked 0x10-byte switch table at RVA 0x6aa99;
// the 23 preceding instructions match. Tried compound command tests in both arms and
// switch-based command dispatch in both arms; the retained mixed form is closest.
// Revisit only after shared message/layout evidence changes or in the last-mile phase.
VA(0x0046a9d0, 0x1ca)
int APanelHandler(tag_message &message)
{
    int handled = 0;
    if (message.type == MESSAGE_WIDGET) {
        if (message.payload.widget.parameter & MESSAGE_MODIFIER_LEFT_SHIFT) {
            if (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
                message.payload.widget.command ==
                    WIDGET_COMMAND_ALTERNATE_SELECT) {
                int helpIndex = ADVMGR_PANEL_NO_HELP;
                switch (message.payload.widget.id) {
                case ADVMGR_PANEL_VIEW_WORLD:
                    helpIndex = 0;
                    break;
                case ADVMGR_PANEL_VIEW_PUZZLE:
                    helpIndex = 1;
                    break;
                case ADVMGR_PANEL_SCENARIO_INFO:
                    helpIndex = 2;
                    break;
                case ADVMGR_PANEL_SEARCH:
                    helpIndex = 3;
                    break;
                case ADVMGR_PANEL_CLOSE_WIDGET:
                    helpIndex = ADVMGR_PANEL_CLOSE_HELP;
                    break;
                }
                if (helpIndex >= 0)
                    NormalDialog(gAPanelHelp[helpIndex], 4, -1, -1, -1,
                                 0, -1, 0, -1, 0);
            }
        } else {
            switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT:
                switch (message.payload.widget.id) {
                case ADVMGR_PANEL_VIEW_WORLD:
                case ADVMGR_PANEL_VIEW_PUZZLE:
                case ADVMGR_PANEL_SCENARIO_INFO:
                case ADVMGR_PANEL_SEARCH:
                case ADVMGR_PANEL_CLOSE_WIDGET:
                    handled = 1;
                    break;
                }
                break;
            }
        }
    }

    if (handled) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = message.payload.widget.id;
        return ADVMGR_DIMENSION_DOOR_HANDLED;
    }
    return ADVMGR_DIMENSION_DOOR_UNHANDLED;
}

// @match-note
// Complete modal/control-disable CFG and all 30 external relocations agree. The first
// 102 diff lines align apart from string identities, then the helper stops at the
// delinked result switch table. Retained direct remote-disable broadcasts and result
// switch after testing equivalent compound and nested spellings.
// Revisit only after shared dialog/message evidence changes or in the last-mile phase.
VA(0x0046ab9a, 0x1e4)
int advManager::ControlPanel(void)
{
    TrimLoopingSounds(ADVMGR_LOOPING_SOUND_LIMIT);
    int selectedCommand = ADVMGR_PANEL_NO_HELP;
    gpMouseManager->SetPointer("advmice.mse", ADVMGR_POINTER_DEFAULT,
                               ADVMGR_DEFAULT_POINTER_FRAME);
    int heroWasMobilized = m_heroContextLocked;
    DemobilizeCurrHero();

    heroWindow *controlPanel = new heroWindow(
        ADVMGR_PANEL_WINDOW_X, ADVMGR_PANEL_WINDOW_Y, "cpanel.bin");
    if (controlPanel == 0)
        MemError();
    if (gbRemoteOn) {
        tag_message message;
        message.type = MESSAGE_WIDGET;
        message.payload.widget.id = ADVMGR_CONTROL_RESTART;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        controlPanel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = ADVMGR_BUTTON_TARGET;
        controlPanel->BroadcastMessage(message);
        message.payload.widget.id = ADVMGR_CONTROL_NEW_GAME;
        message.payload.widget.command = WIDGET_COMMAND_SET_FLAGS;
        message.payload.widget.data.value = WIDGET_COMMAND_DIMMED;
        controlPanel->BroadcastMessage(message);
        message.payload.widget.command = WIDGET_COMMAND_CLEAR_FLAGS;
        message.payload.widget.data.value = ADVMGR_BUTTON_TARGET;
        controlPanel->BroadcastMessage(message);
    }

    gpWindowManager->DoDialog(controlPanel, CPanelHandler, 0);
    delete controlPanel;
    switch (gpWindowManager->m_dialogResult) {
    case ADVMGR_CONTROL_RESTART:
    case ADVMGR_CONTROL_NEW_GAME:
    case ADVMGR_CONTROL_MAIN_MENU:
        selectedCommand = gpWindowManager->m_dialogResult;
        break;
    case ADVMGR_CONTROL_SAVE_GAME:
        SaveGame();
        break;
    }
    if (heroWasMobilized)
        MobilizeCurrHero(0);
    if (selectedCommand != ADVMGR_PANEL_NO_HELP)
        gGameCommand = selectedCommand;
    return selectedCommand != ADVMGR_PANEL_NO_HELP;
}

// @match-note
// Complete 200-byte confirmation buffer, shift-help and confirmation CFG, and all 26
// external relocations agree. The remaining bytes are compiler lowering around the
// widget switches and bFreshSave confirmation arm. Tried compound command tests and
// alternate nesting; the retained source preserves retail body order and relocations.
// Revisit only after shared dialog/message evidence changes or in the last-mile phase.
VA(0x0046ad7e, 0x304)
int CPanelHandler(tag_message &message)
{
    int handled = 0;
    if (message.type == MESSAGE_WIDGET) {
        if (message.payload.widget.parameter & MESSAGE_MODIFIER_LEFT_SHIFT) {
            if (message.payload.widget.command == WIDGET_COMMAND_SELECT ||
                message.payload.widget.command ==
                    WIDGET_COMMAND_ALTERNATE_SELECT) {
                int helpIndex = ADVMGR_PANEL_NO_HELP;
                switch (message.payload.widget.id) {
                case ADVMGR_CONTROL_RESTART:
                    helpIndex = ADVMGR_CONTROL_RESTART_HELP;
                    break;
                case ADVMGR_CONTROL_NEW_GAME:
                    helpIndex = ADVMGR_CONTROL_NEW_GAME_HELP;
                    break;
                case ADVMGR_CONTROL_MAIN_MENU:
                    helpIndex = ADVMGR_CONTROL_MAIN_MENU_HELP;
                    break;
                case ADVMGR_CONTROL_SAVE_GAME:
                    helpIndex = ADVMGR_CONTROL_SAVE_GAME_HELP;
                    break;
                case ADVMGR_PANEL_CLOSE_WIDGET:
                    helpIndex = ADVMGR_CONTROL_CLOSE_HELP;
                    break;
                }
                if (helpIndex >= 0)
                    NormalDialog(gCPanelHelp[helpIndex], 4, -1, -1, -1,
                                 0, -1, 0, -1, 0);
            }
        } else {
            switch (message.payload.widget.command) {
            case WIDGET_COMMAND_DESELECT: {
                char confirmation[ADVMGR_CONTROL_CONFIRMATION_SIZE];
                switch (message.payload.widget.id) {
                case ADVMGR_CONTROL_RESTART:
                    strcpy(confirmation,
                           "Are you sure you want to restart?  (Your current game will be lost)");
                    break;
                case ADVMGR_CONTROL_NEW_GAME:
                    strcpy(confirmation,
                           "Are you sure you want to load a new game?  (Your current game will be lost)");
                    break;
                case ADVMGR_CONTROL_MAIN_MENU:
                    strcpy(confirmation, "Are you sure you want to quit?");
                    break;
                case ADVMGR_CONTROL_SAVE_GAME:
                case ADVMGR_PANEL_CLOSE_WIDGET:
                    handled = 1;
                    break;
                default:
                    break;
                }
                if (message.payload.widget.id == ADVMGR_CONTROL_RESTART ||
                    message.payload.widget.id == ADVMGR_CONTROL_NEW_GAME ||
                    message.payload.widget.id == ADVMGR_CONTROL_MAIN_MENU) {
                    handled = 1;
                    if (!bFreshSave) {
                        NormalDialog(confirmation, 2, -1, -1, -1,
                                     0, -1, 0, -1, 0);
                        if (gpWindowManager->m_dialogResult == ADVMGR_DIALOG_OK)
                            handled = 0;
                    }
                }
                break;
            }
            }
        }
    }

    if (handled) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = WIDGET_COMMAND_DIALOG_SELECT;
        message.payload.widget.command = message.payload.widget.id;
        return ADVMGR_DIMENSION_DOOR_HANDLED;
    }
    return ADVMGR_DIMENSION_DOOR_UNHANDLED;
}

// @early-stop
// Exact size and instruction stream; all 34 external relocations agree. The remaining
// objdiff residuals are gConfig field-overlay and pooled-string symbol identities.
VA(0x0046b082, 0x197)
void advManager::SystemOptions(void)
{
    TrimLoopingSounds(ADVMGR_LOOPING_SOUND_LIMIT);
    gpMouseManager->SetPointer("advmice.mse", ADVMGR_POINTER_DEFAULT,
                               ADVMGR_DEFAULT_POINTER_FRAME);
    int oldInterfaceMode = gConfig.evilInterfaceUsage;
    int oldWalkSpeed = gConfig.walkSpeed;
    int heroWasMobilized = m_heroContextLocked;
    bPrefsChanged = 0;
    DemobilizeCurrHero();

    cPanel = new heroWindow(
        ADVMGR_SYSTEM_OPTIONS_WINDOW_X, ADVMGR_SYSTEM_OPTIONS_WINDOW_Y,
        "spanel.bin");
    if (cPanel == 0)
        MemError();
    SetWinText(cPanel, ADVMGR_SYSTEM_OPTIONS_TITLE);
    UpdateSystemOptions(1);
    gpWindowManager->DoDialog(cPanel, SystemOptionsHandler, 0);
    delete cPanel;

    if (gConfig.walkSpeed != oldWalkSpeed) {
        int sampleIndex;
        for (sampleIndex = 0; sampleIndex < ADVMGR_CURSOR_SAMPLE_COUNT;
             ++sampleIndex)
            gpResourceManager->Dispose(m_cursorSamples[sampleIndex]);
        GetCursorSampleSet(gConfig.walkSpeed);
    }
    if (bPrefsChanged)
        WritePrefs();
    if (gConfig.evilInterfaceUsage != oldInterfaceMode)
        CheckSetEvilInterface(1, -1);
    if (heroWasMobilized)
        MobilizeCurrHero(0);
}

// @match-note
// Complete 0x24 frame, all 18 frame/text broadcasts, draw CFG, and all 68 external
// relocations agree. Residual lowering is around music-source, blackout-computer, and
// slow-video expressions plus gConfig/string overlay identities. Tried direct ternary
// and explicit branch forms; retained forms follow retail broadcast order.
// Revisit only after shared config layout evidence changes or in the last-mile phase.
VA(0x0046b219, 0x35f)
void UpdateSystemOptions(int initialDraw)
{
    tag_message message;
    int musicQuality;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_FRAME;

    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME;
    message.payload.widget.data.value = gConfig.musicVolume != 0;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_SOUND_VOLUME;
    if (gConfig.soundVolume == 0)
        message.payload.widget.data.value = ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE;
    else
        message.payload.widget.data.value =
            ADVMGR_SYSTEM_OPTIONS_SOUND_FRAME_BASE + 1;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_HERO_SPEED;
    message.payload.widget.data.value =
        gConfig.walkSpeed + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_MUSIC_SOURCE;
    if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI)
        musicQuality = 0;
    else if (gConfig.useOpera == CONFIG_OPERA_DISABLED)
        musicQuality = 1;
    else
        musicQuality = 2;
    message.payload.widget.data.value =
        musicQuality + ADVMGR_SYSTEM_OPTIONS_MUSIC_SOURCE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_SHOW_ROUTE;
    message.payload.widget.data.value =
        (gConfig.showRoute == 0) + ADVMGR_SYSTEM_OPTIONS_ROUTE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_COMPUTER_SPEED;
    if (gConfig.blackoutComputer == 0)
        message.payload.widget.data.value =
            gConfig.computerWalkSpeed + ADVMGR_SYSTEM_OPTIONS_SPEED_FRAME_BASE;
    else
        message.payload.widget.data.value =
            ADVMGR_SYSTEM_OPTIONS_COMPUTER_HIDDEN_FRAME;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_INTERFACE;
    message.payload.widget.data.value =
        gConfig.evilInterfaceUsage + ADVMGR_SYSTEM_OPTIONS_INTERFACE_FRAME_BASE;
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_VIDEO;
    message.payload.widget.data.value =
        ADVMGR_SYSTEM_OPTIONS_VIDEO_FRAME_BASE + (gConfig.slowVideo != 0);
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_COLOR_CURSOR;
    message.payload.widget.data.value =
        gConfig.gfx[0].colorMouseCursor + ADVMGR_SYSTEM_OPTIONS_CURSOR_FRAME_BASE;
    cPanel->BroadcastMessage(message);

    message.payload.widget.command = ADVMGR_SYSTEM_OPTIONS_SET_TEXT;
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[gConfig.musicVolume];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_SOUND_VOLUME +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[gConfig.soundVolume];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_HERO_SPEED +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = walkSpeedText[gConfig.walkSpeed];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_MUSIC_SOURCE +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = musicQualityText[musicQuality];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_SHOW_ROUTE +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text = onOffText[gConfig.showRoute];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_COMPUTER_SPEED +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    if (gConfig.blackoutComputer == 0)
        message.payload.widget.data.text =
            walkSpeedText[gConfig.computerWalkSpeed];
    else
        message.payload.widget.data.text = "Don't Show";
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_INTERFACE +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text =
        gInterfaceTypeText[gConfig.evilInterfaceUsage];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_VIDEO +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text =
        cSlowVideoLevelText[gConfig.slowVideo != 0];
    cPanel->BroadcastMessage(message);
    message.payload.widget.id = ADVMGR_SYSTEM_OPTION_COLOR_CURSOR +
                                ADVMGR_SYSTEM_OPTIONS_TEXT_ID_OFFSET;
    message.payload.widget.data.text =
        cBWMouseText[gConfig.gfx[0].colorMouseCursor];
    cPanel->BroadcastMessage(message);

    if (initialDraw == 0)
        cPanel->DrawWindow(1, 0, ADVMGR_SYSTEM_OPTIONS_DRAW_MASK);
}

// @early-stop
// Exact size, all non-relocation bytes, and all 94 relocation sites match. Residuals are
// delinked jump-table labels, gConfig field overlays, and string-pool symbol names.
VA(0x0046b578, 0x672)
int SystemOptionsHandler(struct tag_message& message) {
    int preferencesChanged = 0;
    char textData[120];
    int accepted = 0;

    if (message.type == ADVMGR_SYSTEM_OPTIONS_MESSAGE) {
        if (message.payload.widget.parameter & ADVMGR_SYSTEM_OPTIONS_CONTEXT_FLAG) {
            if (message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_ACTIVATE
                || message.payload.widget.command == ADVMGR_SYSTEM_OPTIONS_HOVER) {
                int helpIndex = ADVMGR_OPTION_DIALOG_NONE;

                switch (message.payload.widget.id) {
                    case ADVMGR_SYSTEM_OPTIONS_DIALOG_ACCEPT:
                        helpIndex = 0;
                        break;
                    case ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME:
                        helpIndex = 1;
                        break;
                    case ADVMGR_SYSTEM_OPTION_SOUND_VOLUME:
                        helpIndex = 2;
                        break;
                    case ADVMGR_SYSTEM_OPTION_HERO_SPEED:
                        helpIndex = 3;
                        break;
                    case ADVMGR_SYSTEM_OPTION_MUSIC_SOURCE:
                        helpIndex = 4;
                        break;
                    case ADVMGR_SYSTEM_OPTION_SHOW_ROUTE:
                        helpIndex = 5;
                        break;
                    case ADVMGR_SYSTEM_OPTION_COMPUTER_SPEED:
                        helpIndex = 6;
                        break;
                    case ADVMGR_SYSTEM_OPTION_INTERFACE:
                        helpIndex = 7;
                        break;
                    case ADVMGR_SYSTEM_OPTION_VIDEO:
                        helpIndex = 8;
                        break;
                    case ADVMGR_SYSTEM_OPTION_COLOR_CURSOR:
                        helpIndex = 9;
                        break;
                }

                if (helpIndex >= 0) {
                    NormalDialog(
                        gSPanelHelp[helpIndex],
                        ADVMGR_OPTION_DIALOG_HELP,
                        ADVMGR_OPTION_DIALOG_NONE,
                        ADVMGR_OPTION_DIALOG_NONE,
                        ADVMGR_OPTION_DIALOG_NONE,
                        0,
                        ADVMGR_OPTION_DIALOG_NONE,
                        0,
                        ADVMGR_OPTION_DIALOG_NONE,
                        0
                    );
                }
            }
        } else {
            switch (message.payload.widget.command) {
                case ADVMGR_SYSTEM_OPTIONS_ACCEPT:
                    switch (message.payload.widget.id) {
                        case ADVMGR_SYSTEM_OPTIONS_DIALOG_ACCEPT:
                            accepted = 1;
                            break;
                    }
                    break;

                case ADVMGR_SYSTEM_OPTIONS_ACTIVATE: {
                    switch (message.payload.widget.id) {
                        case ADVMGR_SYSTEM_OPTION_MUSIC_VOLUME:
                            if (gConfig.musicVolume == 0 && gpSoundManager->m_cdReady == 0
                                && gpSoundManager->m_midiReady == 0) {
                                NormalDialog(
                                    "Neither MIDI nor Redbook music is currently available on this "
                                    "system.",
                                    ADVMGR_OPTION_DIALOG_MESSAGE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0
                                );
                                break;
                            }
                            gConfig.musicVolume =
                                (gConfig.musicVolume + 1) % ADVMGR_OPTION_VOLUME_LEVELS;
                            gpSoundManager->AdjustMusicVolumes();
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_SOUND_VOLUME:
                            if (gConfig.soundVolume == 0 && gpSoundManager->m_digitalDriver == 0) {
                                NormalDialog(
                                    "Digital sound is not currently available on this system.",
                                    ADVMGR_OPTION_DIALOG_MESSAGE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0
                                );
                                break;
                            }
                            gConfig.soundVolume =
                                (gConfig.soundVolume + 1) % ADVMGR_OPTION_VOLUME_LEVELS;
                            gpSoundManager->AdjustSoundVolumes();
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_HERO_SPEED:
                            ++gConfig.walkSpeed;
                            gConfig.walkSpeed %= ADVMGR_OPTION_HERO_SPEED_LEVELS;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_COMPUTER_SPEED:
                            if (gConfig.blackoutComputer) {
                                gConfig.blackoutComputer = 0;
                                gConfig.computerWalkSpeed = ADVMGR_OPTION_COMPUTER_SPEED_DEFAULT;
                            } else if (gConfig.computerWalkSpeed
                                       < ADVMGR_OPTION_COMPUTER_SPEED_MAX) {
                                ++gConfig.computerWalkSpeed;
                            } else {
                                gConfig.blackoutComputer = 1;
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_MUSIC_SOURCE:
                            if (gConfig.musicSource == CONFIG_MUSIC_SOURCE_MIDI) {
                                if (gpSoundManager->m_cdStarted == 0) {
                                    gpSoundManager->CDStartup();
                                }
                                if (gpSoundManager->m_cdReady == 0) {
                                    NormalDialog(
                                        "Unable to set up CD stereo music.  Your CD player might "
                                        "be in use by another program, or your sound driver might "
                                        "not support CD stereo.",
                                        ADVMGR_OPTION_DIALOG_MESSAGE,
                                        ADVMGR_OPTION_DIALOG_NONE,
                                        ADVMGR_OPTION_DIALOG_NONE,
                                        ADVMGR_OPTION_DIALOG_NONE,
                                        0,
                                        ADVMGR_OPTION_DIALOG_NONE,
                                        0,
                                        ADVMGR_OPTION_DIALOG_NONE,
                                        0
                                    );
                                    break;
                                }
                                gpSoundManager->SetMusicQuality(CONFIG_MUSIC_SOURCE_CD);
                                gConfig.useOpera = CONFIG_OPERA_DISABLED;
                            } else if (gConfig.useOpera == CONFIG_OPERA_DISABLED) {
                                gConfig.useOpera = CONFIG_OPERA_ENABLED;
                            } else {
                                if (gpSoundManager->m_midiStarted == 0) {
                                    gpSoundManager->MIDIStartup();
                                }
                                if (gpSoundManager->m_midiReady == 0) {
                                    gConfig.useOpera = CONFIG_OPERA_ENABLED - gConfig.useOpera;
                                } else {
                                    gpSoundManager->SetMusicQuality(CONFIG_MUSIC_SOURCE_MIDI);
                                }
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_SHOW_ROUTE:
                            gConfig.showRoute = 1 - gConfig.showRoute;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_INTERFACE:
                            gConfig.evilInterfaceUsage =
                                (gConfig.evilInterfaceUsage + 1) % ADVMGR_OPTION_INTERFACE_COUNT;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_VIDEO:
                            if (gbLowMemory) {
                                NormalDialog(
                                    "You don't have enough memory for non-interlaced video.",
                                    ADVMGR_OPTION_DIALOG_MESSAGE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0,
                                    ADVMGR_OPTION_DIALOG_NONE,
                                    0
                                );
                                break;
                            }
                            if (gConfig.slowVideo) {
                                gConfig.slowVideo = 0;
                            } else {
                                gConfig.slowVideo = 1;
                            }
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            break;

                        case ADVMGR_SYSTEM_OPTION_COLOR_CURSOR:
                            gConfig.gfx[0].colorMouseCursor = 1 - gConfig.gfx[0].colorMouseCursor;
                            preferencesChanged = 1;
                            bPrefsChanged = 1;
                            gpMouseManager->SetColorMice(gConfig.gfx[0].colorMouseCursor);
                            break;
                    }
                    break;
                }
            }
        }
    }

    if (preferencesChanged) {
        UpdateSystemOptions(0);
    }
    if (accepted) {
        gpWindowManager->m_dialogResult = message.payload.widget.id;
        message.payload.widget.id = ADVMGR_SYSTEM_OPTION_FIRST;
        message.payload.widget.command = message.payload.widget.id;
        return ADVMGR_SYSTEM_OPTIONS_HANDLED;
    }
    return ADVMGR_SYSTEM_OPTIONS_UNHANDLED;
}

VA(0x0046bbea, 0x7f)
int GetMobilityFrame(int mobility) {
    int frame = mobility * ADVMGR_MOBILITY_SCALE / ADVMGR_MOBILITY_DIVISOR;
    if (frame < 0) {
        frame = 0;
    }
    if (frame > ADVMGR_MOBILITY_TOP_THRESHOLD) {
        frame = ADVMGR_FRAME_TOP;
    } else if (frame > ADVMGR_MOBILITY_HIGH_THRESHOLD) {
        frame = ADVMGR_FRAME_HIGH;
    } else if (frame > ADVMGR_MOBILITY_MID_THRESHOLD) {
        frame = ADVMGR_FRAME_MID;
    }
    return frame;
}

VA(0x0046bc69, 0x7f)
int GetManaFrame(int mana) {
    int frame = mana / ADVMGR_MANA_DIVISOR;
    if (frame == 0 && mana >= ADVMGR_MANA_MIN_VISIBLE)
        frame = 1;
    if (frame > ADVMGR_MANA_TOP_THRESHOLD)
        frame = ADVMGR_FRAME_TOP;
    else if (frame > ADVMGR_MANA_HIGH_THRESHOLD)
        frame = ADVMGR_FRAME_HIGH;
    else if (frame > ADVMGR_MANA_MID_THRESHOLD)
        frame = ADVMGR_FRAME_MID;
    return frame;
}

// @early-stop
// Exact bytes and all 42 relocation targets.
VA(0x0046bce8, 0x559)
int advManager::DoVisions(hero *visionHero)
{
    char visionMessageResult[ADVMGR_VISIONS_MESSAGE_BUFFER_SIZE];
    int creatureData;
    int nearestDistanceState;
    int nearestXId;
    int nearestYData;
    int scanXType;
    int scanYLocal;
    mapCell *cellData;
    int joiningCount;
    int monsterCountIndex;
    int currentDistanceId;
    int forcedJoinState;
    float strengthRatioCurrent;
    int joiningCostIndex;

    nearestDistanceState = ADVMGR_VISIONS_NO_MONSTER_DISTANCE;
    nearestYData = -1;
    nearestXId = nearestYData;
    for (scanXType = visionHero->m_x - ADVMGR_VISIONS_RADIUS;
         scanXType <= visionHero->m_x + ADVMGR_VISIONS_RADIUS; ++scanXType) {
        for (scanYLocal = visionHero->m_y - ADVMGR_VISIONS_RADIUS;
             scanYLocal <= visionHero->m_y + ADVMGR_VISIONS_RADIUS; ++scanYLocal) {
            cellData = GetCell(scanXType, scanYLocal);
            if (cellData->m_triggerType == ADVMGR_VISIONS_MONSTER_TRIGGER) {
                if (nearestDistanceState >
                    (currentDistanceId = abs(visionHero->m_x - scanXType) +
                                         abs(visionHero->m_y - scanYLocal))) {
                    nearestDistanceState = currentDistanceId;
                    nearestXId = scanXType;
                    nearestYData = scanYLocal;
                }
            }
        }
    }

    if (nearestDistanceState == ADVMGR_VISIONS_NO_MONSTER_DISTANCE) {
        NormalDialog(
            "You must be within 3 spaces of a monster for the Visions spell to work.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 0;
    }

    cellData = GetCell(nearestXId, nearestYData);
    creatureData = cellData->m_objectIndex;
    forcedJoinState = cellData->m_objectMetadata & MONSTER_JOIN_FORCED;
    monsterCountIndex = cellData->m_objectMetadata & MONSTER_COUNT_MASK;
    sprintf(gText, "{%d %s}\n\n", monsterCountIndex, gArmyNamesPlural[creatureData]);
    strengthRatioCurrent = static_cast<float>(
        gpPhilAI->FightValueOfStack(&visionHero->m_army, visionHero, 0, 0, 0, 0)) /
        static_cast<float>(gMonsterDatabase[creatureData].fightValue * monsterCountIndex);

    if (visionHero->m_army.CanJoin(creatureData) &&
        strengthRatioCurrent > MONSTER_STRENGTH_JOIN &&
        !visionHero->HasArtifact(MONSTER_NO_JOIN_ARTIFACT) &&
        creatureData != MONSTER_GENIE && creatureData != MONSTER_EARTH_ELEMENTAL &&
        creatureData != MONSTER_AIR_ELEMENTAL && creatureData != MONSTER_FIRE_ELEMENTAL &&
        creatureData != MONSTER_WATER_ELEMENTAL) {
        if (forcedJoinState) {
            sprintf(visionMessageResult, "The creatures are willing to join us!");
            strcat(gText, visionMessageResult);
            goto showVision;
        } else if (visionHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] !=
                   MONSTER_DIPLOMACY_NONE) {
            if (visionHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] ==
                MONSTER_DIPLOMACY_EXPERT) {
                joiningCount = monsterCountIndex;
            } else if (visionHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] ==
                       MONSTER_DIPLOMACY_ADVANCED) {
                joiningCount = monsterCountIndex / 2;
            } else {
                joiningCount = monsterCountIndex / 4;
            }
            if (joiningCount == 0)
                joiningCount = 1;

            joiningCostIndex = gMonsterDatabase[creatureData].cost * monsterCountIndex * 2;
            if (joiningCostIndex >
                gpGame->m_players[visionHero->m_owner].m_resources[RES_GOLD]) {
                if (strengthRatioCurrent > MONSTER_STRENGTH_FLEE) {
                    goto creaturesFlee;
                } else {
                    goto creaturesFight;
                }
            }

            if (joiningCount == monsterCountIndex) {
                sprintf(visionMessageResult,
                        "All the creatures will join us...", joiningCostIndex);
            } else {
                sprintf(visionMessageResult,
                        "%d of the creatures will join us...",
                        monsterCountIndex, joiningCostIndex);
            }
            strcat(gText, visionMessageResult);
            goto showVision;
        }
    }

    if (strengthRatioCurrent > MONSTER_STRENGTH_FLEE) {
creaturesFlee:
        sprintf(visionMessageResult,
                "These weak creatures will surely flee before us.");
        strcat(gText, visionMessageResult);
        goto showVision;
    }
creaturesFight:
    sprintf(visionMessageResult,
            "I fear these creatures are in the mood for a fight.");
    strcat(gText, visionMessageResult);
    goto showVision;

showVision:
    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
    return 1;
}

// @match-note
// Complete hero loop, artifact/radius semantics, frame slots, and all 6 external
// relocations agree. The first byte-level divergence is one extra retail continuation
// jump after the loop guard; the remaining instruction stream aligns. Tried repeated
// inline hero expressions and retained named locals, which is the closest source shape.
// Revisit after inline GetHero evidence changes or in the last-mile phase.
VA(0x0046c241, 0xd7)
int advManager::IsCrystalBallInEffect(int x, int y, int radius)
{
    int heroIndex;
    hero *crystalHero;
    int distance;
    for (heroIndex = 0; heroIndex < gpCurPlayer->m_heroCount; ++heroIndex) {
        crystalHero = gpGame->GetHero(gpCurPlayer->m_heroIds[heroIndex]);
        if (crystalHero->HasArtifact(ADVMGR_CRYSTAL_BALL_ARTIFACT)) {
            distance = static_cast<int>(sqrt(static_cast<double>(
                (crystalHero->m_y - y) * (crystalHero->m_y - y) +
                (crystalHero->m_x - x) * (crystalHero->m_x - x))));
            if (distance <= radius)
                return 1;
        }
    }
    return 0;
}

VA(0x0046c318, 0x85)
unsigned char StopOnTrigger(class mapCell *cell)
{
    int type = cell->m_triggerType & ADVMGR_TRIGGER_TYPE_MASK;
    if (type != ADVMGR_SPECIAL_TRIGGER)
        return bStopOnTrigger[type];

    int trigger = cell->m_objectMetadata;
    trigger &= ADVMGR_SPECIAL_TRIGGER_MASK;
    switch (trigger) {
    case ADVMGR_TRIGGER_EVENT_5:
    case ADVMGR_TRIGGER_EVENT_6:
        return 1;
    }
    return 0;
}


// ===== vtable advManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x0045665f, 0x9c9)  int advManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00457028, 0x40a)  void advManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x00457d6c, 0xfda)  int advManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(advManager, 0x004eb6c8);

// ---- globals (definitions, RVA order) ----
DATA(0x004f57b0) int giLimitUpdMinX = -1;
DATA(0x004f57b4) int iLastScrollTime = 0;
DATA(0x004f57b8) int iSandAnim = 0;
DATA(0x004f57bc) int giLastHourGlassUpdateTime = 0;
DATA(0x004f57c0) int TrigX = 0;
DATA(0x004f57c4) int TrigY = 0;
DATA(0x004f57c8) int iCurBottomView = 0;
DATA(0x004f57cc) int iCurBottomViewEnemy = -1;
DATA(0x004f57d0) int iCurHourGlassPhase = 0;
DATA(0x004f57d4) int iLastHourGlassPhase = 1;
DATA(0x004f57d8) int gbForceUpdate = 0;
DATA(0x004f59e8) int giCheatSeq = 0;
DATA(0x004f59ec) int iQWE = 0;
DATA(0x004f5e38) unsigned char monAnimDrawFrame[15];
DATA(0x004f60e0) int iLastSandAnimTime = 0;
DATA(0x004f60e4) int iLastNewSandAnimTime = 0;
DATA(0x004f6720) int giFrameCount = 0;
DATA(0x00527ec8) class heroWindow *cPanel;
DATA(0x00527ed0) int iThisMaxY;
DATA(0x00527edc) int giTownPortalChoice;
DATA(0x00527ee0) int iThisMinY;
DATA(0x00527ee8) class heroWindow *townPortalWin;
DATA(0x00527ef0) struct tag_message USMsg;
DATA(0x00527f14) int giFrameStep;
DATA(0x00527f28) char cArmySizeName[12];
DATA(0x00527f34) int giLimitUpdMaxX;
DATA(0x00527f38) int giLimitUpdMaxY;
DATA(0x00527f40) int bPrefsChanged;
DATA(0x00527f4c) int giLimitUpdMinY;
DATA(0x00527f50) signed char bComboDraw[18][18];
DATA(0x005280b0) struct tag_message CDMsg;
DATA(0x005280d4) int iLastAnimFrame;
