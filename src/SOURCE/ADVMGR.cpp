// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\ADVMGR.OBJ   from: (directly linked into exe)
// functions: 92   data: 33
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_carcass_types.h>
#include <_globals_model.h>
#include <BASE/icon.h>
#include <BASE/iconWidget.h>
#include <BASE/Icon2b.h>
#include <BASE/Misc.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/inputManager.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/sample.h>
#include <BASE/soundManager.h>
#include <BASE/tileset.h>
#include <BASE/WINMGR.h>
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
#include <stdio.h>
#include <string.h>
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
    m_viewLeft = 0;
    m_viewTop = 4;
    m_viewRight = 11;
    m_viewBottom = 13;
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
        m_activeSounds[index].soundId = -1;
        m_activeSounds[index].volume = 127;
        m_activeSoundCount = 0;
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
int advManager::ProcessHover(int, int) { return 0; }

VA(0x0045b094, 0x21a)
void advManager::UpdateScreen(int, int) {}

VA(0x0045b2ae, 0x4eb)
void advManager::CompleteDraw(int, int, int, int) {}

VA(0x0045b799, 0x3a)
void advManager::CompleteDraw(int update)
{
    CompleteDraw(m_mapOriginX, m_mapOriginY, update, 1);
}

VA(0x0045b7d3, 0x3a9)
int advManager::GetCloudLookup(int, int) { return 0; }

VA(0x0045bb7c, 0x24cb)
void advManager::DrawCell(int, int, int, int, int, int) {}

VA(0x0045e047, 0x93)
class mapCell * advManager::GetCell(int, int) { return 0; }

VA(0x0045e0da, 0x104d)
void advManager::UpdateRadar(int, int) {}

VA(0x0045f127, 0x133e)
void advManager::QuickInfo(int, int) {}

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
int advManager::UpdBottomViewEnemyTurn(void) { return 0; }

VA(0x004613b0, 0x366)
int advManager::UpdBottomViewNewTurn(void) { return 0; }

VA(0x00461716, 0x35f)
int advManager::UpdBottomViewResMsg(void) { return 0; }

VA(0x00461a75, 0x363)
int advManager::UpdBottomViewKingdom(void) { return 0; }

VA(0x00461dd8, 0x583)
int advManager::UpdBottomViewHero(void) { return 0; }

VA(0x0046235b, 0xd32)
void advManager::HeroQuickView(int, int, int, int) {}

VA(0x0046308d, 0x120)
char * advManager::GetArmySizeName(int, int) { return 0; }

VA(0x004631ad, 0xc29)
void advManager::TownQuickView(int, int, int, int) {}

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

VA(0x004654ad, 0x11a9)
int advManager::ComboDraw(int, int, int) { return 0; }

VA(0x00466656, 0x38)
int advManager::ComboDraw(int update)
{
    return ComboDraw(m_mapOriginX, m_mapOriginY, update);
}

VA(0x0046668e, 0x338)
void advManager::SetEnvironmentOrigin(int, int, int) {}

VA(0x004669c6, 0x69)
void advManager::CheckLoadSample(int index)
{
    if (m_loopingSamples[index] == 0) {
        TrimLoopingSounds(ADVMGR_LOOPING_SOUND_LIMIT);
        sprintf(gText, "loop%04d.82M", index);
        m_loopingSamples[index] = gpResourceManager->GetSample(gText);
    }
}

VA(0x00466a2f, 0x4c1)
int advManager::GetSoundId(int, int) { return 0; }

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
void advManager::SummonBoat(void) {}

VA(0x00468247, 0x4d9)
void advManager::ShowRoute(int, int, int) {}

VA(0x00468720, 0x107)
void advManager::HideRoute(int, int, int) {}

// @early-stop
// all 52 instructions, both inline-continuation jumps, and ten relocation targets match
VA(0x00468827, 0x8d)
void advManager::CheckDimHero(void)
{
    if (gbThisNetHumanPlayer[giCurPlayer]) {
        if (gpCurPlayer->CurrentHero() == ADVMGR_INVALID_HERO)
            return;
        if (!gpGame->IsMobile(gpCurPlayer->CurrentHero())) {
            ShowRoute(1, 0, 0);
            UpdateHeroLocators(1, 1);
            gpAdvManager->CheckDimNextHeroBut();
        }
    }
}

VA(0x004688b4, 0x6b)
void advManager::CheckDimNextHeroBut(void)
{
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

VA(0x0046b578, 0x672)
int SystemOptionsHandler(struct tag_message &) { return 0; }

VA(0x0046bbea, 0x7f)
int GetMobilityFrame(int mobility)
{
    int frame = mobility * ADVMGR_MOBILITY_SCALE / ADVMGR_MOBILITY_DIVISOR;
    if (frame < 0)
        frame = 0;
    if (frame > ADVMGR_MOBILITY_TOP_THRESHOLD)
        frame = ADVMGR_FRAME_TOP;
    else if (frame > ADVMGR_MOBILITY_HIGH_THRESHOLD)
        frame = ADVMGR_FRAME_HIGH;
    else if (frame > ADVMGR_MOBILITY_MID_THRESHOLD)
        frame = ADVMGR_FRAME_MID;
    return frame;
}

VA(0x0046bc69, 0x7f)
int GetManaFrame(int mana)
{
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
int advManager::DoVisions(class hero *) { return 0; }

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
DATA(0x004f5e38) unsigned char *monAnimDrawFrame;
DATA(0x004f60e0) int iLastSandAnimTime;
DATA(0x004f60e4) int iLastNewSandAnimTime;
DATA(0x004f6720) int giFrameCount;
DATA(0x00527ec8) class heroWindow *cPanel;
DATA(0x00527ed0) int iThisMaxY;
DATA(0x00527edc) int giTownPortalChoice;
DATA(0x00527ee0) int iThisMinY;
DATA(0x00527ee8) class heroWindow *townPortalWin;
DATA(0x00527f14) int giFrameStep;
DATA(0x00527f28) char *cArmySizeName;
DATA(0x00527f34) int giLimitUpdMaxX;
DATA(0x00527f38) int giLimitUpdMaxY;
DATA(0x00527f40) int bPrefsChanged;
DATA(0x00527f4c) int giLimitUpdMinY;
DATA(0x005280d4) int iLastAnimFrame;
