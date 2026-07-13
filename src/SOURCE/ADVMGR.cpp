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
#include <SOURCE/HERO.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/GAME.h>
#include <SOURCE/PHILAI.h>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADVMGR_SOURCE_FILE "I:\\Projects\\Heroes\\Prog\\SOURCE\\ADVMGR.CPP"
#define ADVMGR_QUICK_VIEW_LINE (*reinterpret_cast<const short *>("\x76\x21"))
#define ADVMGR_TOWN_VIEW_LINE (*reinterpret_cast<const short *>("\x5f\x5e"))
#define ADVMGR_BOTTOM_HERO_LINE (*reinterpret_cast<const short *>("\x5f\x21"))
#define ADVMGR_ENVIRONMENT_VOLUME(distance)                                \
    (reinterpret_cast<const int *>("\x40\0\0\0\x39\0\0\0\x28\0\0\0" \
                                   "\x15\0\0\0\x07\0\0\0\x05\0\0\0") \
         [distance])

DATA(0x00527eb8) static unsigned short s_drawGroundTile;
DATA(0x00527ec0) static int s_drawPixelY;
DATA(0x00527ec4) static mapCellExtra *s_drawExtra;
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
DATA(0x0052809c) static int s_drawFlipCloud;
DATA(0x005280a0) static int s_drawHeroFrame;
DATA(0x005280a4) static int s_drawPixelX;
DATA(0x005280a8) static int s_drawPlayerColor;
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
    m_currentSampleSet = 0;
    m_field_0x276 = 1;
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
    m_cursorCellX = 0;
    m_cursorCellY = 0;
    m_cursorCellIndex = 0;
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
        m_visibilityMap = new unsigned char[MAP_WIDTH * MAP_HEIGHT * 2];
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

    GetCursorSampleSet(gCursorSampleSet);
    if (!gbThisNetHumanPlayer[giCurPlayer]) {
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }

    glTimers[0] = KBTickCount() + ADVMGR_TIMER_DELAY;
    int oldSampleVolume = gSampleVolume;
    if (gSampleVolume != 0)
        gSampleVolume = 10;
    SetInitialMapOrigin();

    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    int oldPlayer = giCurPlayer;
    int oldShowIt = bShowIt;
    giCurPlayer = giCurWatchPlayer;
    gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
    bShowIt = 1;
    RedrawAdvScreen(1, 0);
    giCurPlayer = oldPlayer;
    bShowIt = oldShowIt;
    gpCurPlayer = reinterpret_cast<playerData *>(&gpGame->m_players[giCurPlayer]);
    if (!gbThisNetHumanPlayer[giCurPlayer])
        gpGame->ShowComputerScreen();
    KBChangeMenu(hmnuAdv);
    ForceNewHover();
    gpWindowManager->FadeScreen(0, 8, gPalette);
    giBottomViewOverride = 0;
    gSampleVolume = oldSampleVolume;
    gpSoundManager->AdjustSoundVolumes();
    field_0xc = ADVMGR_MANAGER_MESSAGE;
    field_0x10 = id;
    m_active = 1;
    strcpy(name, "advManager");
    return 0;
}

VA(0x00457028, 0x40a)
void advManager::Close(void)
{
    ClearBottomView();
    gpMouseManager->SetPointer(-1);
    if (!bEnteringTown || gSoundTransition || !gCdMusic) {
        gpSoundManager->SwitchAmbientMusic(-1);
        gpSoundManager->StopAllSamples(1);
    } else {
        gpSoundManager->StopAllSamples(0);
    }
    if (m_adventureBorder != 0) {
        BaseFree(m_adventureBorder, "I:\\Projects\\Heroes\\Prog\\SOURCE\\ADVMGR.CPP",
                 const_000f59a8 + 22);
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

// @early-stop
// Retail is 0x6c0 and reconstruction is 0x6b1 with 502 aligned entries; ordinary opcodes/operands match, leaving only gConfig/jump-table symbol identity and two /Ob1 continuation placements.
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
        currentHeroState->m_destinationX = GetCommandTargetX(),
        currentHeroState->m_destinationY = GetCommandTargetY();
        // fall through
    case ADVMGR_COMMAND_CONTINUE_ROUTE:
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
                eventCellState = MoveHero(gpSearchArray->m_storage.path.directions[pathIndexLocal + 1],
                                          pathIndexLocal == 0,
                                          &TrigX, &TrigY, &movementChangedResult, 0,
                                          &movementEndedLocal, 0);
                UpdateHeroLocator(-1, 1, 1);
                if (eventCellState != 0)
                    break;
                if (movementChangedResult || movementEndedLocal || gbHitEvent)
                    break;
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
            reinterpret_cast<town *>(&gpGame->m_castleRecs[gpCurPlayer->CurrentTown()])->m_x,
            reinterpret_cast<town *>(&gpGame->m_castleRecs[gpCurPlayer->CurrentTown()])->m_y);
        reinterpret_cast<town *>(
            &gpGame->m_castleRecs[gpCurPlayer->CurrentTown()])->View(0);
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
        SetHeroContext(GetCell(m_lastHoverCell + m_mapOriginX,
                               m_hoverCellY + m_mapOriginY)->w4hi, 0);
        break;

    case ADVMGR_COMMAND_SELECT_TOWN:
        SetTownContext(GetCell(m_lastHoverCell + m_mapOriginX,
                               m_hoverCellY + m_mapOriginY)->w4hi);
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
             gpGame->m_players[player].evilInterface != gbUseEvilInterface)
        shouldChange = 1;

    if (shouldChange) {
        gbUseEvilInterface = 1 - gbUseEvilInterface;
        if (redraw) {
            tag_message interfaceMessage;
            interfaceMessage.type = ADVMGR_INTERFACE_MESSAGE;
            interfaceMessage.field4 = ADVMGR_INTERFACE_REPLACE_RESOURCE;
            int translationIndex;
            for (translationIndex = 0;
                 translationIndex < ADVMGR_INTERFACE_TRANSLATION_COUNT; ++translationIndex) {
                interfaceMessage.field8 = gpResourceManager->MakeId(
                    cEvilTranslate[translationIndex][1 - gbUseEvilInterface], 0);
                interfaceMessage.field18 = gpResourceManager->MakeId(
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
// Retail is 0xfda and reconstruction is 0x1016 with an exact 0x48 frame; excluding the 0x20/0x94 jump tables, every non-jump opcode/operand matches and the 60-byte delta is twelve net five-byte /Ob1/block-boundary jumps.
// All 232 relocation sites agree apart from delinked local/string identities and retail empty_stub versus the reconstructed CreateColorTables call.
VA(0x00457d6c, 0xfda)
int advManager::Main(struct tag_message &message)
{
    if (KBTickCount() > glTimers[0] && ComboDraw(1))
        UpdateScreen(1, 0);
    if (gbGameOver) {
        message.type = 0x4000;
        message.field4 = 1;
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
    if (!gbNoSound && gMidiEnabled && giForceSwitchMusic > 0 &&
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
        switch (message.field4) {
        case 13:
            if (!(message.fieldC & 0x200))
                processResult = ProcessDeSelect(&message, &exitRequestedFlag, eventCellsResult);
            break;
        case 12:
        case 14: {
            int helpIndexState;
            if (message.fieldC & 0x200) {
                helpIndexState = -1;
                switch (message.field8) {
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
                    goto finish_message;
                }
            }
            processResult = ProcessSelect(&message, eventCellsResult);
            goto finish_message;
        }
        }
        goto finish_message;
    case 4:
        processResult = ProcessHover(message.field10, message.field14);
        goto finish_message;
    case 1:
        break;
    default:
        goto finish_message;
    }

    moveDirectionState = -1;
    if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO)
        currentHero = gpGame->GetHero(gpCurPlayer->CurrentHero());
    else
        currentHero = 0;
    if (giDebugLevel < 1 &&
        (message.field4 == 61 || message.field4 == 62 ||
         message.field4 == 63 || message.field4 == 64 ||
         message.field4 == 65 || message.field4 == 66 ||
         message.field4 == 67 || message.field4 == 68 ||
         message.field4 == 87 || message.field4 == 88))
        goto finish_message;

    switch (message.field4) {
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
            cheatHero = gpGame->GetHero(gpCurPlayer->CurrentHero());
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
        }
        break;
    }
    case 72:
        if (message.fieldC & 0xc) ScreenScroll(0, 0); else moveDirectionState = 0;
        break;
    case 73:
        if (message.fieldC & 0xc) ScreenScroll(1, 0); else moveDirectionState = 1;
        break;
    case 77:
        if (message.fieldC & 0xc) ScreenScroll(2, 0); else moveDirectionState = 2;
        break;
    case 81:
        if (message.fieldC & 0xc) ScreenScroll(3, 0); else moveDirectionState = 3;
        break;
    case 80:
        if (message.fieldC & 0xc) ScreenScroll(4, 0); else moveDirectionState = 4;
        break;
    case 79:
        if (message.fieldC & 0xc) ScreenScroll(5, 0); else moveDirectionState = 5;
        break;
    case 75:
        if (message.fieldC & 0xc) ScreenScroll(6, 0); else moveDirectionState = 6;
        break;
    case 71:
        if (message.fieldC & 0xc) ScreenScroll(7, 0); else moveDirectionState = 7;
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

    if (gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO && moveDirectionState >= 0) {
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
    }

finish_message:
    if (eventCellsResult[0] != 0)
        DoEvent(eventCellsResult[0], TrigX, TrigY);
    if (gbGameOver || exitRequestedFlag == 1 || giMenuCommand != -1) {
        message.type = 0x4000;
        message.field4 = 1;
        return 2;
    }
    return processResult;
}

VA(0x00458d46, 0x22)
void advManager::Reseed(int, int)
{
    giSeedingValid = 0;
}

// @early-stop
// Retail is 0xeb1 and reconstruction is 0xeb0 with 999 aligned instructions and an exact 0xb8 frame; residuals are commutative load order at +0x73b..+0x762 and +0xa94..+0xd21, one /Ob1 continuation move, and one block-boundary jump at +0xd71.
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
    mouseX = message->field10;
    mouseY = message->field14;

    switch (message->field8) {
    case 105:
    case 112:
    case 119:
    case 126: {
        pageState = (message->field8 - 105) / 7;
        if (pageState >= gpCurPlayer->m_heroCount)
            break;
        objectTypeState = gpCurPlayer->m_heroIds[gpCurPlayer->m_heroLocatorPage + pageState];
        if (message->fieldC & 0x200) {
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
            gpCurPlayer->m_townLocatorPage + message->field8 - 16];
        if (message->fieldC & 0x200) {
            TownQuickView(objectTypeState, message->field8 - 16, -1, -1);
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
        if (message->fieldC & 0x200) {
            if (!visible) {
                QuickInfo(m_lastHoverCell, m_hoverCellY);
            } else {
                if (m_lastHoverCell == 7 && m_hoverCellY == 7 &&
                    gpCurPlayer->CurrentHero() != ADVMGR_INVALID_HERO && m_heroContextLocked) {
                    objectTypeState = MAP_EVENT_HERO_INTERACTION;
                    objectIdIndex = gpCurPlayer->CurrentHero();
                } else {
                    objectTypeState = currentCell->triggerType & 0x7f;
                    objectIdIndex = currentCell->w4hi;
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
                } else if ((!mobileResult || (message->fieldC & 0xc) ||
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
                objectTypeState = currentCell->triggerType & 0x7f;
                objectIdIndex = currentCell->w4hi;
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
        if (message->fieldC & 0x200) {
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
                    if (radarMessage.field4 < ADVMGR_RADAR_LEFT)
                        radarMessage.field4 = ADVMGR_RADAR_LEFT;
                    if (radarMessage.field4 >= ADVMGR_RADAR_RIGHT)
                        radarMessage.field4 = ADVMGR_RADAR_RIGHT - 1;
                    if (radarMessage.field8 < ADVMGR_RADAR_TOP)
                        radarMessage.field8 = ADVMGR_RADAR_TOP;
                    if (radarMessage.field8 >= ADVMGR_RADAR_BOTTOM)
                        radarMessage.field8 = ADVMGR_RADAR_BOTTOM - 1;
                    gpMouseManager->Main(radarMessage);
                    mouseX = static_cast<int>((radarMessage.field4 - 480) / radarScale);
                    mouseY = static_cast<int>((radarMessage.field8 - 16) / radarScale);
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

    if ((message->fieldC & 0x200) &&
        message->field8 >= ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE &&
        message->field8 <= ADVMGR_BOTTOM_VIEW_LAST_MESSAGE) {
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
    switch (message->field8) {
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

    if (message->field8 >= ADVMGR_BOTTOM_VIEW_FIRST_MESSAGE &&
        message->field8 <= ADVMGR_BOTTOM_VIEW_LAST_MESSAGE) {
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
// Both sides are 0x5c8 bytes / 390 instructions; only the GetNullSample /Ob1 jmp $+0 moves from leading to trailing, plus relocation symbol naming differences.
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
    if (!((currentCell->objIndex == 0xff ||
           currentCell->objTileset == ADVMGR_CLEAR_GROUND_TILESET) &&
          currentCell->ovlIndex == 0xff)) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Try searching on clear ground.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }
    if (!giGroundToTerrain[currentCell->tile]) {
        if (!gbHumanPlayer[giCurPlayer])
            goto search_end;
        NormalDialog("Try looking on land!!!",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    }

            if (gbHumanPlayer[giCurPlayer])
                digSampleState = LoadPlaySample("DIGSOUND.82M");
            if (currentCell->objIndex == 0xff ||
                currentCell->objTileset == ADVMGR_CLEAR_GROUND_TILESET) {
                currentCell->objTileset = ADVMGR_DIG_HOLE_TILESET;
                currentCell->objIndex = ADVMGR_DIG_HOLE_FRAME;
                currentCell->w4a = 1;
                currentCell->w4b = 1;
                currentCell->field8 |= 0x80;
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
    gpCurPlayer->m_canDig = 0;
    return 1;
}

VA(0x0045a644, 0xa50)
// @early-stop reloc-masked bytes differ only at +0xc5, +0x2d8, +0x9df, and
// +0x9fb: four displacement bytes selecting equivalent local return
// trampolines. All 106 relocation sites and external targets agree.
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
                if ((hoverCellLocal->triggerType & ADVMGR_TRIGGER_TYPE_MASK) == ADVMGR_HOVER_TOWN
                    && gpGame->GetTown(hoverCellLocal->w4hi)->m_owner == giCurPlayer) {
                    gpMouseManager->SetPointer(ADVMGR_POINTER_TOWN);
                    m_selectedCell = ADVMGR_COMMAND_TOWN_VIEW;
                    return 1;
                } else {
                    if ((hoverCellLocal->triggerType & ADVMGR_TRIGGER_TYPE_MASK)
                            == ADVMGR_HOVER_HERO
                        && gpGame->GetHero(hoverCellLocal->w4hi)->m_owner == giCurPlayer) {
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

                if (hoverCellLocal->field8 & ADVMGR_HOVER_OBJECT_BLOCKED) {
                    if ((hoverCellLocal->triggerType & ADVMGR_TRIGGER_TYPE_MASK)
                        == ADVMGR_HOVER_TOWN) {
                        hoverTownCell = gpGame->GetTown(hoverCellLocal->w4hi);
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
                       || giGroundToTerrain[hoverCellLocal->tile]
                       || hoverCellLocal->triggerType == ADVMGR_HERO_TRIGGER
                       || hoverCellLocal->triggerType == ADVMGR_BOAT_TRIGGER
                       || hoverCellLocal->triggerType == ADVMGR_HOVER_SHIPWRECK_TRIGGER)
                      && (m_cursorType != ADVMGR_CURSOR_ROUTE
                          || !giGroundToTerrain[hoverCellLocal->tile]
                          || hoverCellLocal->triggerType == ADVMGR_HOVER_COAST))) {
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

                    switch (hoverCellLocal->triggerType & ADVMGR_TRIGGER_TYPE_MASK) {
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
                            if (gpGame->GetHero(hoverCellLocal->w4hi)->m_owner != giCurPlayer) {
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
                            hoverTownCell = gpGame->GetTown(hoverCellLocal->w4hi);
                            if ((hoverCellLocal->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
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
                                  && (!(hoverCellLocal->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                                      || !StopOnTrigger(hoverCellLocal))
                                  && (gpMouseManager->SetPointer(
                                          pointerBaseCursor + ADVMGR_POINTER_ATTACK
                                      ),
                                      1))) {
                                if (hoverCellLocal->triggerType & ADVMGR_TRIGGER_ACTION_FLAG) {
                                    if (m_cursorType != ADVMGR_CURSOR_ROUTE) {
                                        if (giGroundToTerrain[hoverCellLocal->tile]) {
                                            gpMouseManager->SetPointer(
                                                pointerBaseCursor + ADVMGR_POINTER_ACTION
                                            );
                                        } else if (hoverCellLocal->triggerType
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
                                        if (!giGroundToTerrain[hoverCellLocal->tile]) {
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

VA(0x0045b094, 0x21a)
// @early-stop identical assembly and all 31 relocation targets. The sole
// reloc-masked byte difference is the branch displacement at +0x42: retail
// jumps to the epilogue while this build targets the adjacent jmp-to-epilogue.
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

VA(0x0045b2ae, 0x4eb)
// @early-stop exact size and all 23 relocation targets. Reloc-masked bytes
// differ only in twelve commutative X-coordinate additions: operand bytes
// +0xf9/+0xfc, +0x15c/+0x15f, +0x19e/+0x1a1, +0x210/+0x213,
// +0x254/+0x257, +0x29d/+0x2a0, +0x2e1/+0x2e4, +0x323/+0x326,
// +0x368/+0x36b, +0x3a8/+0x3ab, +0x403/+0x406, and +0x463/+0x466
// exchange the equivalent [ebp-4] drawX and [ebp+8] originX operands.
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
        if ((giCurWatchPlayerBit & mapExtra[y * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_EAST;
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH;
        if ((giCurWatchPlayerBit & *(x + mapExtra + y * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_WEST;
        if ((giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_EAST;
        if ((giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_EAST;
        if ((giCurWatchPlayerBit & *(x + mapExtra + (y + 1) * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_WEST;
        if ((giCurWatchPlayerBit & *(x + mapExtra + (y - 1) * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_WEST;
    } else {
        if ((cloudMask & ADVMGR_CLOUD_NORTH) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH;
        if ((cloudMask & ADVMGR_CLOUD_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[y * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH;
        if ((cloudMask & ADVMGR_CLOUD_WEST) == 0 &&
            (giCurWatchPlayerBit & *(x + mapExtra + y * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_WEST;
        if ((cloudMask & ADVMGR_CLOUD_NORTH_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y - 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH_EAST) == 0 &&
            (giCurWatchPlayerBit & mapExtra[(y + 1) * MAP_WIDTH + x + 1]) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_EAST;
        if ((cloudMask & ADVMGR_CLOUD_SOUTH_WEST) == 0 &&
            (giCurWatchPlayerBit & *(x + mapExtra + (y + 1) * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_SOUTH_WEST;
        if ((cloudMask & ADVMGR_CLOUD_NORTH_WEST) == 0 &&
            (giCurWatchPlayerBit & *(x + mapExtra + (y - 1) * MAP_WIDTH - 1)) == 0)
            cloudMask |= ADVMGR_CLOUD_NORTH_WEST;
    }
    return giCloudType[cloudMask];
}

// @early-stop
// raw-masked: all 2373 retail instructions and 551 relocations accounted; residuals
// are four boundary compares, one continuation jump, equivalent frame-boolean lowering,
// eight +31 associations and consequent displacements; this function has no data island
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
                   *reinterpret_cast<unsigned short *>(
                       m_visibilityMap + mapX * 2 + mapY * MAP_WIDTH * 2) != 0) {
            if ((*reinterpret_cast<unsigned short *>(
                     m_visibilityMap + mapX * 2 + mapY * MAP_WIDTH * 2) & 0x100) != 0) {
                IconToBitmapColorTable(
                    m_objectIcons[17], gpWindowManager->m_screen,
                    s_drawPixelX - 12, s_drawPixelY + 2,
                    (*reinterpret_cast<unsigned short *>(
                         m_visibilityMap + mapX * 2 + mapY * MAP_WIDTH * 2) - 1) & 0xff,
                    1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT,
                    0, gColorTableRed, 1);
            } else {
                IconToBitmap(m_objectIcons[17], gpWindowManager->m_screen,
                             s_drawPixelX - 12, s_drawPixelY + 2,
                             (*reinterpret_cast<unsigned short *>(
                                  m_visibilityMap + mapX * 2 + mapY * MAP_WIDTH * 2) - 1) & 0xff,
                             1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                             ADVMGR_DRAW_CLIP_HEIGHT, 0);
            }
        }
    } else {
        if (drawMask & ADVMGR_DRAW_GROUND) {
            s_drawGroundTile = s_drawCell->field8;
            s_drawGroundTile <<= 14;
            s_drawGroundTile |= s_drawCell->tile;
            TileToBitmap(m_groundTiles, s_drawGroundTile, gpWindowManager->m_screen,
                         s_drawPixelX, s_drawPixelY);

            if (s_drawCell->w4a &&
                (gbDrawingPuzzle == 0 || s_drawCell->objTileset != 56 ||
                 s_drawCell->objIndex != 140) &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->objTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->objIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->objFlag0) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->objTileset],
                                     s_drawCell->objIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->objIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->extra != 0 &&
                m_mapData->Extra(s_drawCell->extra)->objIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->extra);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->f4a &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->objFlag) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objTileset],
                                         s_drawExtra->objIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->index != 0 &&
                    m_mapData->Extra(s_drawExtra->index)->objIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->index);
                else
                    s_drawExtra = 0;
            }

            if (s_drawCell->w4b && !s_drawCell->w4a &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->objTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->objIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->objFlag0) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->objTileset],
                                     s_drawCell->objIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->objIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->extra != 0 &&
                m_mapData->Extra(s_drawCell->extra)->objIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->extra);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->f4b && !s_drawExtra->f4a &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->objFlag) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objTileset],
                                         s_drawExtra->objIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->index != 0 &&
                    m_mapData->Extra(s_drawExtra->index)->objIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->index);
                else
                    s_drawExtra = 0;
            }
        }

        if (drawMask & ADVMGR_DRAW_OBJECT) {
            if (s_drawCell->objIndex != 0xff && !s_drawCell->w4a &&
                !s_drawCell->w4b && !s_drawCell->w4c &&
                s_drawCell->objTileset != ADVMGR_TILESET_MINE &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->objTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->objIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->objFlag0) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->objTileset],
                                     s_drawCell->objIndex)->flags;
                    animFrame = m_updateMaxY % s_drawAnimationLength;
                    if (s_drawCell->triggerType == 0xdf) {
                        if (s_drawCell->w4hi != 0)
                            animFrame = m_updateMaxY % (s_drawAnimationLength - 1);
                        else
                            animFrame = s_drawAnimationLength - 1;
                    }
                    IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 animFrame + s_drawCell->objIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->extra != 0 &&
                m_mapData->Extra(s_drawCell->extra)->objIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->extra);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (!s_drawExtra->f4a && !s_drawExtra->f4b && !s_drawExtra->f4c &&
                    s_drawExtra->objTileset != ADVMGR_TILESET_MINE &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->objFlag) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objTileset],
                                         s_drawExtra->objIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     m_updateMaxY % s_drawAnimationLength +
                                         s_drawExtra->objIndex + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->index != 0 &&
                    m_mapData->Extra(s_drawExtra->index)->objIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->index);
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
                    if (s_drawAdjacentCell->triggerType == ADVMGR_MONSTER_TRIGGER) {
                        s_drawMine = &gpGame->m_mines[s_drawAdjacentCell->w4hi];
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

                if (s_drawCell->objTileset == ADVMGR_TILESET_MINE) {
                    if (m_lastQuickViewX == mapX && m_lastQuickViewY == mapY) {
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     (8 - (m_field_0x2ba == 0)) +
                                         s_drawCell->objIndex * 9,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    } else {
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     s_drawCell->objIndex * 9,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawCell->objIndex == 59 || s_drawCell->objIndex == 60)
                            s_drawMonsterFrame = m_viewBounds[mapX & 3] % 6;
                        else
                            s_drawMonsterFrame = monAnimDrawFrame[m_viewBounds[mapX & 3]];
                        IconToBitmap(m_objectIcons[ADVMGR_TILESET_MONSTER],
                                     gpWindowManager->m_screen,
                                     s_drawPixelX + 16, s_drawPixelY + 30,
                                     s_drawCell->objIndex * 9 + s_drawMonsterFrame + 1,
                                     1, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
            }

            if (s_drawCell->triggerType == ADVMGR_BOAT_TRIGGER) {
                s_drawPlayerColor = -1;
                s_drawHeroType = ADVMGR_HERO_TYPE_BOAT;
                s_drawHeroFrame = GetCursorBaseFrame(
                    gpGame->m_boats[s_drawCell->w4hi].direction);
                s_drawHasHero = 1;
                s_drawHeroYOffset = -10;
            } else {
                s_drawHeroYOffset = 0;
                if (s_drawCell->triggerType == ADVMGR_HERO_TRIGGER) {
                    s_drawHero = gpGame->GetHero(s_drawCell->w4hi);
                    s_drawPlayerColor =
                        gpGame->m_players[s_drawHero->m_owner].color;
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
                        if (m_field_0x276 != 0 &&
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
                        if (m_field_0x276 != 0 &&
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
                            (s_drawCell->field8 & 4) == 0) {
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
                        if (m_field_0x276 != 0 &&
                            s_drawHeroType != ADVMGR_HERO_TYPE_BOAT &&
                            (drawMask & ADVMGR_DRAW_HERO_SHADOW)) {
                            IconToBitmap(m_shadowIcon, gpWindowManager->m_screen,
                                         s_drawPixelX, s_drawPixelY + 31,
                                         s_drawHeroFrame, 1, 0, 0,
                                         ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                        if (m_field_0x276 != 0 &&
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
                            (s_drawCell->field8 & 4) == 0) {
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

            if (m_cursorActive != 0 && (s_drawCell->field8 & 0x40) != 0 &&
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
            if ((drawMask & ADVMGR_DRAW_OVERLAY) && s_drawCell->objIndex != 0xff &&
                s_drawCell->w4c &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->objTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->objIndex,
                             0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->objFlag0) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->objTileset],
                                     s_drawCell->objIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->objTileset], gpWindowManager->m_screen,
                                 s_drawPixelX, s_drawPixelY,
                                 s_drawCell->objIndex + m_updateMaxY % s_drawAnimationLength + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->extra != 0 &&
                m_mapData->Extra(s_drawCell->extra)->objIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->extra);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (s_drawExtra->f4c &&
                    (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->objTileset])) {
                    IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 s_drawExtra->objIndex, 0, 0, 0,
                                 ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    if (s_drawExtra->objFlag) {
                        s_drawAnimationLength =
                            GetIconEntry(m_objectIcons[s_drawExtra->objTileset],
                                         s_drawExtra->objIndex)->flags;
                        IconToBitmap(m_objectIcons[s_drawExtra->objTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     s_drawExtra->objIndex +
                                         m_updateMaxY % s_drawAnimationLength + 1,
                                     0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                    }
                }
                if (s_drawExtra->index != 0 &&
                    m_mapData->Extra(s_drawExtra->index)->objIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->index);
                else
                    s_drawExtra = 0;
            }

            if (s_drawCell->ovlIndex != 0xff &&
                (((drawMask & ADVMGR_DRAW_OVERLAY) && !s_drawCell->ovlFlag1) ||
                 ((drawMask & ADVMGR_DRAW_OVERLAY_TOP) && s_drawCell->ovlFlag1)) &&
                (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawCell->ovlTileset])) {
                IconToBitmap(m_objectIcons[s_drawCell->ovlTileset], gpWindowManager->m_screen,
                             s_drawPixelX, s_drawPixelY, s_drawCell->ovlIndex,
                             s_drawCell->ovlTileset == 14, 0, 0,
                             ADVMGR_DRAW_CLIP_WIDTH, ADVMGR_DRAW_CLIP_HEIGHT, 0);
                if (s_drawCell->ovlFlag0) {
                    s_drawAnimationLength =
                        GetIconEntry(m_objectIcons[s_drawCell->ovlTileset],
                                     s_drawCell->ovlIndex)->flags;
                    IconToBitmap(m_objectIcons[s_drawCell->ovlTileset],
                                 gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                 m_updateMaxY % s_drawAnimationLength + s_drawCell->ovlIndex + 1,
                                 0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                 ADVMGR_DRAW_CLIP_HEIGHT, 0);
                }
            }

            if (s_drawCell->extra != 0 &&
                m_mapData->Extra(s_drawCell->extra)->ovlIndex != 0xff)
                s_drawExtra = m_mapData->Extra(s_drawCell->extra);
            else
                s_drawExtra = 0;
            while (s_drawExtra != 0) {
                if (((drawMask & ADVMGR_DRAW_OVERLAY) && !s_drawExtra->ovlFlag1) ||
                    ((drawMask & ADVMGR_DRAW_OVERLAY_TOP) && s_drawExtra->ovlFlag1)) {
                    if (gbDrawingPuzzle == 0 || bPuzzleDraw[s_drawExtra->ovlTileset]) {
                        IconToBitmap(m_objectIcons[s_drawExtra->ovlTileset],
                                     gpWindowManager->m_screen, s_drawPixelX, s_drawPixelY,
                                     s_drawExtra->ovlIndex,
                                     s_drawExtra->ovlTileset == 14, 0, 0,
                                     ADVMGR_DRAW_CLIP_WIDTH,
                                     ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        if (s_drawExtra->ovlFlag0) {
                            s_drawAnimationLength =
                                GetIconEntry(m_objectIcons[s_drawExtra->ovlTileset],
                                             s_drawExtra->ovlIndex)->flags;
                            IconToBitmap(m_objectIcons[s_drawExtra->ovlTileset],
                                         gpWindowManager->m_screen,
                                         s_drawPixelX, s_drawPixelY,
                                         m_updateMaxY % s_drawAnimationLength +
                                             s_drawExtra->ovlIndex + 1,
                                         0, 0, 0, ADVMGR_DRAW_CLIP_WIDTH,
                                         ADVMGR_DRAW_CLIP_HEIGHT, 0);
                        }
                    }
                }
                if (s_drawExtra->index != 0 &&
                    m_mapData->Extra(s_drawExtra->index)->ovlIndex != 0xff)
                    s_drawExtra = m_mapData->Extra(s_drawExtra->index);
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
    if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT)
        return m_mapData->cells;
    else
        return &m_mapData->Row(y)[x];
}

// @early-stop
// raw-masked: exact logic/frame and 122-reloc external-target audit; operand order at
// +0x141..0x15a, +0x1d3..0x1eb, +0x234..0x23d, and +0x47d..0x488, plus retail NOPs
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
        screenRowOffset =
            &gpWindowManager->m_screen->m_pixels[
                (minYOffset * 2 + ADVMGR_RADAR_TOP) *
                ADVMGR_RADAR_ROW_GROUPS * ADVMGR_RADAR_GROUP_BYTES] +
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
                if ((cellValue->field8 & 0x40) != 0 &&
                    m_mapOriginX + ADVMGR_RADAR_CURRENT_CELL == mapColumnLimit &&
                    m_mapOriginY + ADVMGR_RADAR_CURRENT_CELL == mapRow) {
                    radarColorValue = gOwnerColors[gpGame->m_players[giCurPlayer].color];
                } else {
                    if ((cellValue->triggerType & ADVMGR_TRIGGER_TYPE_MASK) ==
                        ADVMGR_RADAR_TOWN_TRIGGER) {
                        ownerIndexValue = gpGame->m_availableHeroes[cellValue->w4hi];
                        if (!(giCurPlayer != ownerIndexValue)) {
                            int ownerColorIndex;
                            if (ownerIndexValue >= 0)
                                ownerColorIndex = gpGame->m_players[ownerIndexValue].color;
                            else
                                ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                            radarColorValue = gOwnerColors[ownerColorIndex];
                        }
                    } else {
                        objectTilesetLocal = static_cast<unsigned int>(-1);
                        if (cellValue->objIndex != 0xff) {
                            objectTilesetLocal = cellValue->objTileset;
                        } else if (cellValue->ovlIndex != 0xff) {
                            objectTilesetLocal = cellValue->ovlTileset;
                        }

                        if (cellValue->triggerType == ADVMGR_RADAR_TOWN_TILESET_1 ||
                            (objectTilesetLocal == 14 && mapColumnLimit > 0 &&
                             mapColumnLimit < MAP_WIDTH - 1 &&
                             m_mapData->Row(mapRow)[mapColumnLimit - 1].triggerType ==
                                 ADVMGR_RADAR_NEIGHBOR_TRIGGER) ||
                            m_mapData->Row(mapRow)[mapColumnLimit + 1].triggerType ==
                                ADVMGR_RADAR_NEIGHBOR_TRIGGER) {
                            objectTilesetLocal = ADVMGR_RADAR_TOWN_TILESET_1;
                        }

                        if (objectTilesetLocal == ADVMGR_RADAR_SPECIAL_TILESET &&
                            cellValue->triggerType == ADVMGR_RADAR_REEFS_TRIGGER) {
                            radarColorValue = gMapColors[giGroundToTerrain[cellValue->tile]] +
                                ADVMGR_RADAR_TERRAIN_SHADE;
                        } else {
                            switch (objectTilesetLocal) {
                            case ADVMGR_RADAR_TOWN_TILESET_1:
                            case ADVMGR_RADAR_TOWN_TILESET_2: {
                                int ownerColorIndex;
                                ownerIndexValue = gpGame->m_townOwners[cellValue->w4hi];
                                townXValue = gpGame->GetTown(cellValue->w4hi)->m_x;
                                townYValue = gpGame->GetTown(cellValue->w4hi)->m_y;
                                if (ownerIndexValue >= 0)
                                    ownerColorIndex = gpGame->m_players[ownerIndexValue].color;
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
                                switch (cellValue->triggerType) {
                                case 1: case 0x17: case 0x1d:
                                case 0x81: case 0x97: case 0x9d: {
                                    int ownerColorIndex;
                                    ownerIndexValue = gpGame->m_mineOwners[cellValue->w4hi];
                                    if (ownerIndexValue >= 0)
                                        ownerColorIndex = gpGame->m_players[ownerIndexValue].color;
                                    else
                                        ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                                    radarColorValue = gOwnerColors[ownerColorIndex];
                                    break;
                                }
                                default:
                                    radarColorValue = gMapColors[giGroundToTerrain[cellValue->tile]] +
                                        ADVMGR_RADAR_TERRAIN_SHADE;
                                    break;
                                }
                                break;
                            default:
radar_default_object:
                                switch (cellValue->triggerType) {
                                case 1: case 0x17: case 0x1d:
                                case 0x81: case 0x97: case 0x9d: {
                                    int ownerColorIndex;
                                    ownerIndexValue = gpGame->m_mineOwners[cellValue->w4hi];
                                    if (ownerIndexValue >= 0)
                                        ownerColorIndex = gpGame->m_players[ownerIndexValue].color;
                                    else
                                        ownerColorIndex = ADVMGR_RADAR_NEUTRAL_OWNER;
                                    radarColorValue = gOwnerColors[ownerColorIndex];
                                    break;
                                }
                                default:
                                    radarColorValue = gMapColors[giGroundToTerrain[cellValue->tile]];
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
// instruction-exact bodies/frame and 205-reloc audit; only map-index operand grouping,
// switch break-stub/dispatcher placement, and retail trailing NOPs differ cumulatively
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
             mapExtra[(m_mapOriginX + cellX) +
                      (m_mapOriginY + cellY) * MAP_WIDTH]) == 0) {
            sprintf(gText, "%s", "Uncharted Territory");
        } else {

    switch (currentCell->triggerType & 0x7f) {
    case ADVMGR_OBJECT_ARTIFACT:
        sprintf(gText, "%s", "Artifact");
        goto quick_info_ready;
    case ADVMGR_OBJECT_OBELISK:
        if (currentCell->triggerType & 0x80) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (gpGame->m_obeliskVisitors[
                         currentCell->w4hi - ADVMGR_OBELISK_INDEX_BASE] &
                     (1u << giCurPlayer))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_GAZEBO_VISIT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_gazeboVisits & (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_FORT_VISIT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_fortVisits & (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_WITCH_DOCTOR_VISIT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_witchDoctorVisits &
                     (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_MERCENARY_VISIT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_mercenaryCampVisits &
                     (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_STANDING_STONE_ALT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_standingStoneVisits &
                     (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_TREE_ALT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_treeKnowledgeVisits &
                     (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_XANADU_ALT:
        if (heroLocal != 0 && (currentCell->triggerType & 0x80)) {
            sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                    (heroLocal->m_xanaduVisits & (1u << (currentCell->w4hi & 0x1f)))
                        ? "(already visited)" : "(not visited)");
            goto quick_info_ready;
        }
        break;
    case ADVMGR_OBJECT_FORT:
        visitedMaskValue = ADVMGR_VISIT_FORT;
        break;
    case ADVMGR_OBJECT_GAZEBO:
        visitedMaskValue = ADVMGR_VISIT_GAZEBO;
        break;
    case ADVMGR_OBJECT_MERCENARY_CAMP:
        visitedMaskValue = ADVMGR_VISIT_MERCENARY_CAMP;
        break;
    case ADVMGR_OBJECT_STANDING_STONES:
        visitedMaskValue = ADVMGR_VISIT_STANDING_STONES;
        break;
    case ADVMGR_OBJECT_WITCH_DOCTOR_ALT:
        visitedMaskValue = ADVMGR_VISIT_WITCH_DOCTOR;
        break;
    case ADVMGR_OBJECT_EVENT_SITE:
        visitedMaskValue = ADVMGR_VISIT_EVENT_SITE;
        break;
    case ADVMGR_OBJECT_EVENT_SITE_ALT:
        visitedMaskValue = ADVMGR_VISIT_XANADU;
        break;
    case ADVMGR_OBJECT_XANADU:
        visitedMaskValue = ADVMGR_VISIT_TREE_OF_KNOWLEDGE;
        break;
    case ADVMGR_OBJECT_NONE:
    case 0x13:
    case 0x1c:
    case 0x2c:
    case 0x39:
        sprintf(gText, "%s", gTerrainNames[giGroundToTerrain[currentCell->tile]]);
        goto quick_info_ready;
    case ADVMGR_OBJECT_GUARDED: {
        sprintf(gText, "%s", gQuickViewText[currentCell->triggerType & 0x7f]);
        goto quick_info_guarded;
    }
    case ADVMGR_OBJECT_MINE: {
        if (gpGame->m_mines[currentCell->w4hi].guardianType != -1) {
            sprintf(gText, "%s %s",
                    gResourceNames[gpGame->m_mines[currentCell->w4hi].resourceType],
                    "Mine");
            goto quick_info_guarded;
        }
        goto quick_info_unguarded_mine;
    }
quick_info_guarded:
        sprintf(guardCaption, "\n\nguarded by %s %s",
                GetArmySizeName(gpGame->m_mines[currentCell->w4hi].guardianCount, 2),
                gArmyNamesPlural[gpGame->m_mines[currentCell->w4hi].guardianType]);
        strcat(gText, guardCaption);
        goto quick_info_ready;
quick_info_unguarded_mine:
        sprintf(gText, "%s %s",
                gResourceNames[gpGame->m_mines[currentCell->w4hi].resourceType],
                "Mine");
        goto quick_info_ready;
    case ADVMGR_OBJECT_RESOURCE:
        sprintf(gText, "%s", gResourceNames[
            (currentCell->objIndex & ADVMGR_RESOURCE_FRAME_PAIR_MASK) / 2]);
        goto quick_info_ready;
    case ADVMGR_OBJECT_MONSTER:
        if (IsCrystalBallInEffect(m_mapOriginX + cellX, m_mapOriginY + cellY, 8)) {
            sprintf(gText, "%d %s", currentCell->w4hi & 0xfff,
                    gArmyNamesPlural[currentCell->objIndex]);
        } else {
            sprintf(gText, "%s %s", GetArmySizeName(currentCell->w4hi & 0xfff, 1),
                    gArmyNamesPlural[currentCell->objIndex]);
        }
        goto quick_info_ready;
    case ADVMGR_OBJECT_BARRIER:
    case ADVMGR_OBJECT_TENT:
        sprintf(gText, gQuickViewText[currentCell->triggerType & 0x7f],
                xBarrierColor[currentCell->w4hi & 7]);
        uppercaseResult = static_cast<char>(
            toupper(static_cast<int>(static_cast<signed char>(gText[0]))));
        gText[0] = uppercaseResult;
        goto quick_info_ready;
    case ADVMGR_OBJECT_GENERIC_SITE: {
        mapObjectKindValue = -1;
        if (currentCell->objIndex != 0xff) {
            siteFrameLocal[0] = currentCell->objIndex;
            objectTilesetLocal = currentCell->objTileset;
        } else {
            siteFrameLocal[0] = currentCell->ovlIndex;
            objectTilesetLocal = currentCell->ovlTileset;
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
        goto quick_info_ready;
    }
    case ADVMGR_OBJECT_RECRUITMENT_SITE: {
        if (currentCell->ovlIndex == 0xff) {
            siteFrameLocal[0] = currentCell->objIndex;
            objectTilesetLocal = currentCell->objTileset;
        } else {
            siteFrameLocal[0] = currentCell->ovlIndex;
            objectTilesetLocal = currentCell->ovlTileset;
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
        goto quick_info_ready;
    }
    case ADVMGR_OBJECT_REEFS:
        if (currentCell->objTileset == ADVMGR_SITE_TILESET_2) {
            sprintf(gText, "Reefs");
            goto quick_info_ready;
        }
        break;
    }

    if (visitedMaskValue != 0 && heroLocal != 0) {
        sprintf(gText, "%s\n\n%s", gQuickViewText[currentCell->triggerType & 0x7f],
                (heroLocal->m_eventFlags & visitedMaskValue)
                    ? "(already visited)" : "(not visited)");
    } else {
        sprintf(gText, "%s", gQuickViewText[currentCell->triggerType & 0x7f]);
    }
        }
    }

quick_info_ready:
    strcpy(savedTextLocal, gText);
    if (giDebugLevel > 0 && currentCell != 0) {
        sprintf(gText, "gi%d obtile%d obi%d ot%d ei%d bl%d %s X%d Y%d",
                currentCell->tile, currentCell->objTileset, currentCell->objIndex, currentCell->triggerType,
                currentCell->w4hi, currentCell->field8 & 8, savedTextLocal,
                m_mapOriginX + cellX, m_mapOriginY + cellY);
    }
    message.type = 0x200;
    message.field4 = 3;
    message.field8 = 1;
    message.text = gText;
    windowLocal->BroadcastMessage(message);
    gpWindowManager->AddWindow(windowLocal, -1, 1);
    QuickViewWait();
    gpWindowManager->RemoveWindow(windowLocal);
    delete windowLocal;
}

VA(0x00460465, 0x348)
void advManager::UpdateHeroLocator(int, int, int) {}

VA(0x004607ad, 0x102)
void advManager::UpdateHeroLocators(int, int) {}

VA(0x004608af, 0x2e8)
void advManager::UpdateTownLocators(int, int) {}

VA(0x00460b97, 0x1cc)
void advManager::UpdBottomView(int, int, int) {}

VA(0x00460d63, 0x132)
void advManager::ClearBottomView(void) {}

VA(0x00460e95, 0x51b)
// @early-stop exact size and 73 relocation sites. Residuals are the
// GetPlayerColor /Ob1 continuation at +0x2f5..+0x320 (leading versus trailing)
// and equivalent commutative global comparisons at +0x29e..+0x2c6 and
// +0x3ef..+0x418; the only unmasked bytes outside the moved continuation are
// the equivalent relational opcodes at +0x3fa and +0x418.
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
                message.field4 = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
                message.field8 = ADVMGR_ENEMY_TURN_SAND_ID;
                message.field18 = iSandAnim + ADVMGR_ENEMY_TURN_SAND_FRAME_OFFSET;
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
            message.field4 = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.field8 = ADVMGR_ENEMY_TURN_CREST_ID;
            message.field18 =
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
            message.field4 = ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME;
            message.field8 = ADVMGR_ENEMY_TURN_PHASE_ID;
            message.field18 =
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

VA(0x004613b0, 0x366)
int advManager::UpdBottomViewNewTurn(void) { return 0; }

VA(0x00461716, 0x35f)
int advManager::UpdBottomViewResMsg(void) { return 0; }

VA(0x00461a75, 0x363)
int advManager::UpdBottomViewKingdom(void) { return 0; }

VA(0x00461dd8, 0x583)
// @early-stop exact bytes and all 36 relocation targets
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

// @early-stop
// raw-masked: exact frame/body and 109 relocations; only loop-compare/multiply operand
// order, folding of literal 30 + 6, consequent jump displacements, and two retail NOPs
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
    if (targetHero->m_owner == giCurPlayer || m_currentSampleSet == 1 ||
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

    quickViewMessageState.field4 = 4;
    quickViewMessageState.field8 = 2;
    quickViewMessageState.field18 = targetHero->m_unknown18;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    quickViewMessageState.field4 = 4;
    quickViewMessageState.field8 = 8;
    quickViewMessageState.field18 = gpGame->GetPlayerColor(targetHero->m_owner) * 2;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    ++quickViewMessageState.field8;
    ++quickViewMessageState.field18;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);
    sprintf(gText, "%s", targetHero->m_name);
    quickViewMessageState.field4 = 3;
    quickViewMessageState.field8 = 1;
    quickViewMessageState.text = gText;
    quickWindowSlot->BroadcastMessage(quickViewMessageState);

    visibleArmyCountState = 0;
    for (armyIndex = 0; armyIndex < 5; ++armyIndex)
        if (targetHero->m_army.m_creatureTypes[armyIndex] != -1)
            ++visibleArmyCountState;

    if (targetHero->m_owner == giCurPlayer || m_currentSampleSet == 1 ||
        IsCrystalBallInEffect(targetHero->m_x, targetHero->m_y, 8)) {
        for (armyIndex = 0; armyIndex < 4; ++armyIndex) {
            sprintf(gText, "%d", targetHero->Stats(armyIndex));
            quickViewMessageState.field8 = armyIndex + 3;
            quickViewMessageState.text = gText;
            quickWindowSlot->BroadcastMessage(quickViewMessageState);
        }
        sprintf(gText, "%d/%d", targetHero->m_spellPoints,
                targetHero->Stats(3) * 10);
        quickViewMessageState.field8 = 7;
        quickViewMessageState.text = gText;
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
// raw-masked: exact frame/body and 102 relocations; only commutative operand-order
// bytes at twelve measured sites and three retail alignment NOPs remain
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
    messageLocal.field4 = 4;
    messageLocal.field8 = 2;
    messageLocal.field18 = quickTownLocal->m_type + 9;
    if ((gpGame->GetTown(townId)->m_buildings & 0x40) == 0)
        messageLocal.field18 += 6;
    townQuickWindow->BroadcastMessage(messageLocal);

    if (informationLevel != 3 ||
        BitTest(gpGame->m_knownTowns, static_cast<signed char>(quickTownLocal->m_id)) == 0) {
        messageLocal.field4 = 6;
        messageLocal.field8 = 300;
        messageLocal.field18 = 4;
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    if (quickTownLocal->m_owner == -1) {
        messageLocal.field4 = 6;
        messageLocal.field8 = 8;
        messageLocal.field18 = 4;
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.field8;
        townQuickWindow->BroadcastMessage(messageLocal);
    } else {
        messageLocal.field4 = 4;
        messageLocal.field8 = 8;
        messageLocal.field18 = gpGame->GetPlayerColor(quickTownLocal->m_owner) * 2;
        townQuickWindow->BroadcastMessage(messageLocal);
        ++messageLocal.field8;
        ++messageLocal.field18;
        townQuickWindow->BroadcastMessage(messageLocal);
    }

    sprintf(gText, GetTownName(static_cast<signed char>(quickTownLocal->m_id)));
    messageLocal.field4 = 3;
    messageLocal.field8 = 1;
    messageLocal.text = gText;
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
void advManager::RedrawAdvScreen(int, int) {}

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

VA(0x00463f95, 0x16c)
void advManager::DemobilizeCurrHero(void) {}

VA(0x00464101, 0x217)
void advManager::SetTownContext(int) {}

VA(0x00464318, 0x392)
void advManager::SetHeroContext(int, int) {}

VA(0x004646aa, 0x22f)
void advManager::DoHeroKnob(void) {}

VA(0x004648d9, 0x22f)
void advManager::DoTownKnob(void) {}

VA(0x00464b08, 0x397)
void advManager::CastSpell(int) {}

VA(0x00464e9f, 0x24c)
int SaveGame(void) { return 0; }

VA(0x004650eb, 0xa6)
void advManager::CheckCastSpell(void) {}

VA(0x00465191, 0x31c)
int DimensionDoorHandler(struct tag_message &) { return 0; }

// @early-stop
// instruction-exact frame/body and 161 relocations; residuals are delinked biased
// bComboDraw aliases, commutative flat-index/min-max evaluation, and three retail NOPs
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

                if (cell->objFlag0 || cell->ovlFlag0)
                    ++bComboDraw[column][mapRow];
                if ((cell->triggerType & ADVMGR_TRIGGER_TYPE_MASK) == 0x28)
                    ++bComboDraw[column][mapRow];
                if ((cell->triggerType & ADVMGR_TRIGGER_TYPE_MASK) == 1)
                    ++bComboDraw[column][mapRow];

                if (cell->triggerType == 0x98) {
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

                if (cell->triggerType == ADVMGR_HERO_TRIGGER ||
                    cell->triggerType == ADVMGR_BOAT_TRIGGER) {
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
            if (cell->triggerType == ADVMGR_MONSTER_TRIGGER) {
                if (gpGame->m_mines[cell->w4hi].guardianType == ADVMGR_MONSTER_GHOST) {
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
                if (*reinterpret_cast<unsigned short *>(
                        m_visibilityMap + mapCellX * 2 +
                        mapYValue * MAP_WIDTH * 2) != 0)
                    ++bComboDraw[column][mapRow + 1];
                if (*reinterpret_cast<unsigned short *>(
                        m_visibilityMap + mapCellX * 2 +
                        (mapYValue - 1) * MAP_WIDTH * 2) != 0)
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

    if (gSampleVolume != 0) {
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

    if (!giGroundToTerrain[currentCell->tile] &&
        (giGroundShape[currentCell->tile] & ADVMGR_SOUND_GROUND_SHAPE_MASK))
        return ADVMGR_SOUND_COASTLINE;

    if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG) {
        switch (currentCell->triggerType & ADVMGR_TRIGGER_TYPE_MASK) {
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
            if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_ALCHEMIST_LAB_ACTION;
            break;
        case ADVMGR_SOUND_OBJECT_MINE:
            if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_MINE;
            break;
        case ADVMGR_SOUND_OBJECT_ABANDONED_MINE:
            if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_ABANDONED_MINE;
            break;
        case ADVMGR_SOUND_OBJECT_SAWMILL:
            if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_SAWMILL;
            break;
        case ADVMGR_SOUND_OBJECT_DAEMON_CAVE:
            if (currentCell->triggerType & ADVMGR_TRIGGER_ACTION_FLAG)
                return ADVMGR_SOUND_DAEMON_CAVE;
            break;
        }
    } else {
        switch (currentCell->triggerType) {
        case ADVMGR_SOUND_OBJECT_TAR_PIT:
            return ADVMGR_SOUND_TAR_PIT;
        case ADVMGR_SOUND_OBJECT_LAVA_POOL:
            if (currentCell->objIndex >= ADVMGR_SOUND_ALCHEMIST_FRAME_FIRST &&
                currentCell->objIndex <= ADVMGR_SOUND_ALCHEMIST_FRAME_LAST)
                return ADVMGR_SOUND_ALCHEMIST_LAB;
            else
                return ADVMGR_SOUND_LAVA_POOL;
        case ADVMGR_SOUND_OBJECT_VOLCANO:
            if (currentCell->objTileset == ADVMGR_SOUND_TILESET_SMALL_VOLCANO ||
                currentCell->objTileset == ADVMGR_SOUND_TILESET_LARGE_VOLCANO)
                return ADVMGR_SOUND_LARGE_VOLCANO;
            else
                return ADVMGR_SOUND_SMALL_VOLCANO;
        case ADVMGR_SOUND_OBJECT_WATER_LAKE:
            if (currentCell->objTileset == ADVMGR_SOUND_TILESET_WATER_LAKE_UNUSED)
                break;
            return ADVMGR_SOUND_WATERING_HOLE;
        }

        switch (currentCell->objTileset) {
        case ADVMGR_SOUND_TILESET_STREAM:
            return ADVMGR_SOUND_STREAM;
        case ADVMGR_SOUND_TILESET_WATER:
            if (currentCell->objIndex == ADVMGR_SOUND_SEAGULL_FRAME_FIRST ||
                currentCell->objIndex == ADVMGR_SOUND_SEAGULL_FRAME_LAST)
                return ADVMGR_SOUND_SEAGULLS;
            break;
        }
    }

    return ADVMGR_ENVIRONMENT_SOUND_NONE;
}

VA(0x00466ef0, 0x23a)
void advManager::InsertSound(int, int, int, int) {}

VA(0x0046712a, 0x40f)
void advManager::TeleportTo(class hero *, int, int, int, int) {}

VA(0x00467539, 0x1fb)
void advManager::DimensionDoor(void) {}

VA(0x00467734, 0x129)
int TownPortalHandler(struct tag_message &) { return 0; }

VA(0x0046785d, 0x43e)
void advManager::TownGate(int) {}

VA(0x00467c9b, 0x5ac)
// @early-stop exact size and 41 relocation sites. Residuals are the
// CurrentHero /Ob1 continuation at +0x174..+0x184, equivalent w4hi bitfield
// read-modify-write evaluation order at +0x4de..+0x4ed, and the unreachable
// post-return local jump at +0x5a3..+0x5a6. Retail's +0xc2 and +0x4a2
// relocations are delinked as a string but address normalDirTable in context.
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
    if (!giGroundToTerrain[destinationCell->tile]) {
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
            if (destinationCell->objIndex == 0xff && destinationCell->triggerType == 0
                && !giGroundToTerrain[destinationCell->tile]) {
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
                boat1->savedTriggerType = destinationCell->triggerType;
                boat1->savedEventData = static_cast<unsigned char>(destinationCell->w4hi);
                destinationCell->triggerType = ADVMGR_SUMMON_BOAT_TRIGGER;
                destinationCell->w4hi = boatIndex9;

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
void advManager::ShowRoute(int, int, int) {}

VA(0x00468720, 0x107)
void advManager::HideRoute(int, int, int) {}

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
void advManager::SeedTo(int, int) {}

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

VA(0x00468ab6, 0x1a6)
void advManager::ScreenScroll(int, int) {}

VA(0x00468c5c, 0x1bb)
void advManager::CheckScreenScroll(void) {}

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
void advManager::SetInitialMapOrigin(void) {}

VA(0x00469160, 0x1be)
void advManager::LoadRemote(void) {}

VA(0x0046931e, 0x20c)
char * advManager::CheckHandleNet(void) { return 0; }

VA(0x0046952a, 0xcd)
int advManager::CheckHandleNetPlayerWait(struct tag_message &, int) { return 0; }

VA(0x004695f7, 0x1d5)
void advManager::TrimLoopingSounds(int) {}

VA(0x004697cc, 0xd5)
void advManager::DisableButtons(void)
{
    if (gpAdvManager->m_active != 1)
        return;
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.field4 = ADVMGR_BUTTON_DISABLE;
    msg.field18 = ADVMGR_BUTTON_TARGET;
    msg.field8 = ADVMGR_BUTTON_SLOT_1; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_2; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_3; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_4; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_5; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_6; m_adventureWindow->BroadcastMessage(msg);
}

VA(0x004698a1, 0xd5)
void advManager::EnableButtons(void)
{
    if (gpAdvManager->m_active != 1)
        return;
    tag_message msg;
    msg.type = ADVMGR_BUTTON_MESSAGE;
    msg.field4 = ADVMGR_BUTTON_ENABLE;
    msg.field18 = ADVMGR_BUTTON_TARGET;
    msg.field8 = ADVMGR_BUTTON_SLOT_1; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_2; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_3; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_4; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_5; m_adventureWindow->BroadcastMessage(msg);
    msg.field8 = ADVMGR_BUTTON_SLOT_6; m_adventureWindow->BroadcastMessage(msg);
}

VA(0x00469976, 0x145)
void advManager::SaveAdventureBorder(void) {}

VA(0x00469abb, 0x134)
void advManager::DrawAdventureBorder(void) {}

VA(0x00469bef, 0x3d3)
int advManager::FindAdjacentMonster(int, int, int *, int *, int, int) { return 0; }

VA(0x00469fc2, 0x125)
void ComputeAdvNetControl(void) {}

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

VA(0x0046a1dd, 0x4c6)
void advManager::ViewPuzzle(void) {}

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

VA(0x0046a724, 0x2ac)
void advManager::AdvPanel(void) {}

VA(0x0046a9d0, 0x1ca)
int APanelHandler(struct tag_message &) { return 0; }

VA(0x0046ab9a, 0x1e4)
int advManager::ControlPanel(void) { return 0; }

VA(0x0046ad7e, 0x304)
int CPanelHandler(struct tag_message &) { return 0; }

VA(0x0046b082, 0x197)
void advManager::SystemOptions(void) {}

VA(0x0046b219, 0x35f)
void UpdateSystemOptions(int) {}

// @early-stop
// Exact size, all non-relocation bytes, and all 94 relocation sites match. Residuals are
// delinked jump-table labels, gConfig field overlays, and string-pool symbol names.
VA(0x0046b578, 0x672)
int SystemOptionsHandler(struct tag_message& message) {
    int preferencesChanged = 0;
    char textData[120];
    int accepted = 0;

    if (message.type == ADVMGR_SYSTEM_OPTIONS_MESSAGE) {
        if (message.fieldC & ADVMGR_SYSTEM_OPTIONS_CONTEXT_FLAG) {
            if (message.field4 == ADVMGR_SYSTEM_OPTIONS_ACTIVATE
                || message.field4 == ADVMGR_SYSTEM_OPTIONS_HOVER) {
                int helpIndex = ADVMGR_OPTION_DIALOG_NONE;

                switch (message.field8) {
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
            switch (message.field4) {
                case ADVMGR_SYSTEM_OPTIONS_ACCEPT:
                    switch (message.field8) {
                        case ADVMGR_SYSTEM_OPTIONS_DIALOG_ACCEPT:
                            accepted = 1;
                            break;
                    }
                    break;

                case ADVMGR_SYSTEM_OPTIONS_ACTIVATE: {
                    switch (message.field8) {
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
                            if (gConfig.soundQuality == ADVMGR_OPTION_MUSIC_MIDI) {
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
                                gpSoundManager->SetMusicQuality(ADVMGR_OPTION_MUSIC_CD);
                                gConfig.useOpera = 0;
                            } else if (gConfig.useOpera == 0) {
                                gConfig.useOpera = 1;
                            } else {
                                if (gpSoundManager->m_midiStarted == 0) {
                                    gpSoundManager->MIDIStartup();
                                }
                                if (gpSoundManager->m_midiReady == 0) {
                                    gConfig.useOpera = 1 - gConfig.useOpera;
                                } else {
                                    gpSoundManager->SetMusicQuality(ADVMGR_OPTION_MUSIC_MIDI);
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
        gpWindowManager->m_dialogResult = message.field8;
        message.field8 = ADVMGR_SYSTEM_OPTION_FIRST;
        message.field4 = message.field8;
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

VA(0x0046bce8, 0x559)
// @early-stop exact bytes and all 42 relocation targets
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
            if (cellData->triggerType == ADVMGR_VISIONS_MONSTER_TRIGGER) {
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
    creatureData = cellData->objIndex;
    forcedJoinState = cellData->w4hi & MONSTER_JOIN_FORCED;
    monsterCountIndex = cellData->w4hi & MONSTER_COUNT_MASK;
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
                gpGame->m_players[visionHero->m_owner].resources[RES_GOLD]) {
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

VA(0x0046c241, 0xd7)
int advManager::IsCrystalBallInEffect(int, int, int) { return 0; }

VA(0x0046c318, 0x85)
unsigned char StopOnTrigger(class mapCell *cell)
{
    int type = cell->triggerType & ADVMGR_TRIGGER_TYPE_MASK;
    if (type != ADVMGR_SPECIAL_TRIGGER)
        return bStopOnTrigger[type];

    int trigger = cell->w4hi;
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
DATA(0x004f57b0) int giLimitUpdMinX;
DATA(0x004f57b4) int iLastScrollTime;
DATA(0x004f57b8) int iSandAnim;
DATA(0x004f57bc) int giLastHourGlassUpdateTime;
DATA(0x004f57c0) int TrigX;
DATA(0x004f57c4) int TrigY;
DATA(0x004f57c8) int iCurBottomView;
DATA(0x004f57cc) int iCurBottomViewEnemy;
DATA(0x004f57d0) int iCurHourGlassPhase;
DATA(0x004f57d4) int iLastHourGlassPhase;
DATA(0x004f57d8) int gbForceUpdate;
DATA(0x004f59e8) int giCheatSeq;
DATA(0x004f59ec) int iQWE;
DATA(0x004f5e38) unsigned char monAnimDrawFrame[15];
DATA(0x004f60e0) int iLastSandAnimTime;
DATA(0x004f60e4) int iLastNewSandAnimTime;
DATA(0x004f6720) int giFrameCount;
DATA(0x00527ec8) class heroWindow *cPanel;
DATA(0x00527ed0) int iThisMaxY;
DATA(0x00527edc) int giTownPortalChoice;
DATA(0x00527ee0) int iThisMinY;
DATA(0x00527ee8) class heroWindow *townPortalWin;
DATA(0x00527f14) int giFrameStep;
DATA(0x00527f28) char cArmySizeName[12];
DATA(0x00527f34) int giLimitUpdMaxX;
DATA(0x00527f38) int giLimitUpdMaxY;
DATA(0x00527f40) int bPrefsChanged;
DATA(0x00527f4c) int giLimitUpdMinY;
DATA(0x00527f50) signed char bComboDraw[18][18];
DATA(0x005280d4) int iLastAnimFrame;
