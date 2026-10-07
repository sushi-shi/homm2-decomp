// The detail tool (eventsManager) and the dialogs of its unit: the raw cell
// editor, the monster and ultimate artifact editor and the random map
// generator's settings. The unit name EVENTMGR is descriptive: no retail
// assertion names it; Open stores the class name "eventsManager".

#include <va.h>
#include <EDITOR/EVENTMGR.h>
#include <EDITOR/eventsManager.h>
#include <EDITOR/editManager.h>
#include <EDITOR/EDITOR.h>
#include <EDITOR/mapcell.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/IconDraw.h>
#include <BASE/iconWidget.h>
#include <BASE/inputManager.h>
#include <BASE/message.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <BASE/widget.h>
#include <BASE/widgetKind.h>
#include <SOURCE/KB.h>
#include <SOURCE/kbwin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Zero-initialized: .bss in definition order.
DATA(0x004a4ba4) iconWidget* gDensityTracks[RANDOM_MAP_DENSITY_COUNT] = {NULL};
DATA(0x004a4bb8) iconWidget* gTerrainKnobs[RANDOM_MAP_TERRAIN_COUNT] = {NULL};
DATA(0x004a4bd8) iconWidget* gDensityKnobs[RANDOM_MAP_DENSITY_COUNT] = {NULL};
DATA(0x004a4bec) b32 gEditUltimateArtifact = false;
DATA(0x004a4bf0) iconWidget* gTerrainTracks[RANDOM_MAP_TERRAIN_COUNT] = {NULL};
DATA(0x004a4c10) i32 gMonsterCountEdit = 0;
DATA(0x004a4c14) heroWindow* gNewMapWindow = NULL;

VA(0x00411670, 0x1f)
eventsManager::eventsManager(void) {}

VA(0x0041168f, 0x66)
i32 eventsManager::Open(i32 priority) {
    gEditManager->m_window->DrawWindow();
    m_overlayIcon = gpResourceManager->GetIcon("overlay.icn");
    m_messageMask = BASE_MANAGER_ACCEPT_EXECUTIVE;
    m_priority = priority;
    m_active = true;
    strcpy(m_name, "eventsManager");
    return 0;
}

VA(0x004116f5, 0x53)
void eventsManager::Close(void) {
    gpResourceManager->Dispose(m_overlayIcon);
    if (!gbClosingApp) {
        gEditManager->DrawMap();
        gEditManager->UpdateMapView();
        gEditManager->DrawRadar(true);
    }
    m_active = false;
}

VA(0x00411748, 0x491)
MessageDispatchResult eventsManager::Main(tag_message& message) {
    i32 clickX;
    mapCell* clickedCell;
    i32 clickY;
    i32 hoverX;
    i32 spell;
    mapCell* cursorCell;
    i32 hoverY;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_SELECT:
                    if (HAS(message.payload.widget.modifiers, MESSAGE_MODIFIER_RIGHT_BUTTON))
                        break;
                    clickX = gEditManager->m_cursorX;
                    clickY = gEditManager->m_cursorY;
                    clickedCell = gMap.GetCell(clickX, clickY);
                    switch (message.payload.widget.id) {
                        case EDIT_CONTROL_MAP:
                            if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)
                                || clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                                || clickedCell->m_triggerType
                                       == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE))
                                EditTown(clickX, clickY);
                            else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE))
                                EditSign(clickX, clickY);
                            else if (clickedCell->m_triggerType
                                     == MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT))
                                EditEvent(clickedCell->m_objectMetadata);
                            else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX))
                                EditSphinx(clickedCell->m_objectMetadata);
                            else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_MONSTER)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_WEAK)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_MEDIUM)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_MONSTER_STRONG)
                                     || clickedCell->m_triggerType
                                            == MAP_ACTION_TRIGGER(
                                                MAP_OBJECT_RANDOM_MONSTER_VERY_STRONG
                                            ))
                                EditMonster(clickX, clickY, false);
                            else if (clickedCell->m_triggerType
                                     == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_ULTIMATE_ARTIFACT))
                                EditMonster(clickX, clickY, true);
                            else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_HERO))
                                EditHero(clickX, clickY, false);
                            else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_ARTIFACT)
                                     && clickedCell->m_objectIndex / EVENTS_ARTIFACT_SPRITE_FRAMES
                                            == IDX(ARTIFACT_SPELL_SCROLL)) {
                                spell = clickedCell->m_objectMetadata;
                                EditSpellScroll(&spell);
                                clickedCell->m_objectMetadata = spell;
                            } else if (clickedCell->m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL))
                                EditHero(clickX, clickY, true);
                            else
                                EditCell(clickX, clickY);
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_MOUSE_MOVE:
            if (InMapArea(message.payload.mouse.screenX, message.payload.mouse.screenY)) {
                hoverX = message.payload.mouse.screenX;
                hoverY = message.payload.mouse.screenY;
                gEditManager->ScreenToCell(hoverX, hoverY);
                hoverX += gEditManager->m_viewX;
                hoverY += gEditManager->m_viewY;
                if (gEditManager->m_cursorX != hoverX || gEditManager->m_cursorY != hoverY) {
                    gEditManager->m_cursorX = hoverX;
                    gEditManager->m_cursorY = hoverY;
                    cursorCell = gMap.GetCell(hoverX, hoverY);
                    hoverX -= gEditManager->m_viewX;
                    hoverY -= gEditManager->m_viewY;
                    hoverX = hoverX * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_LEFT;
                    hoverY = hoverY * gZoomTileSize[gEditManager->m_zoomLevel] + EDIT_VIEW_TOP;
                    gEditManager->DrawMap();
                    if (LocationHasSpecialDetails(cursorCell->m_triggerType))
                        m_overlayIcon->FillToBuffer(
                            hoverX, hoverY, gEditManager->m_zoomLevel, EVENTS_HOVER_DETAIL_COLOR,
                            ICON_DRAW_NORMAL, NULL);
                    else
                        m_overlayIcon->FillToBuffer(
                            hoverX, hoverY, gEditManager->m_zoomLevel, EVENTS_HOVER_COLOR,
                            ICON_DRAW_NORMAL, NULL);
                    gEditManager->UpdateMapView();
                    gEditManager->UpdateCursor();
                }
            }
            return MESSAGE_DISPATCH_CONSUME;
    }
    return MESSAGE_DISPATCH_CONTINUE;
}

VA(0x00411bd9, 0x48d)
void eventsManager::EditCell(i32 x, i32 y) {
    mapCell original;
    // Never read: a slot of the retail frame.
    i32 unusedCode;
    char text[EVENTS_NUMBER_TEXT_SIZE];
    tag_message message;

    if (giDebugLevel < CELL_WINDOW_DEBUG_LEVEL) {
        NormalDialog(
            localization::Tr("editor.events.cell.details_unavailable"), NORMAL_DIALOG_INFO);
        return;
    }
    const i16 textBase = CELL_WINDOW_FIRST_FIELD;
    // Never read: a slot of the retail frame.
    const i16 toggleBase = CELL_WINDOW_FIRST_FLAG;
    gEditCell = gMap.GetCell(x, y);
    original = *gEditCell;
    gEditDialog = new heroWindow(EVENTS_DIALOG_X, EVENTS_DIALOG_Y, "cellwin.bin");
    message.type = MESSAGE_WIDGET;
    message.payload.widget.command = WIDGET_COMMAND_SET_TEXT;
    message.payload.widget.data.text = text;
    sprintf(text, "%d", gEditCell->m_terrainImageIndex);
    message.payload.widget.id = textBase + CELL_FIELD_TERRAIN_IMAGE;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_objectTileset);
    message.payload.widget.id = textBase + CELL_FIELD_OBJECT_TILESET;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_objectIndex);
    message.payload.widget.id = textBase + CELL_FIELD_OBJECT_INDEX;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_overlayTileset);
    message.payload.widget.id = textBase + CELL_FIELD_OVERLAY_TILESET;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_overlayIndex);
    message.payload.widget.id = textBase + CELL_FIELD_OVERLAY_INDEX;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_animatedObject);
    message.payload.widget.id = textBase + CELL_FIELD_ANIMATED_OBJECT;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_animatedOverlay);
    message.payload.widget.id = textBase + CELL_FIELD_ANIMATED_OVERLAY;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_objectLayerBit1);
    message.payload.widget.id = textBase + CELL_FIELD_OBJECT_LAYER;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_isRoad);
    message.payload.widget.id = textBase + CELL_FIELD_ROAD;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_triggerType);
    message.payload.widget.id = textBase + CELL_FIELD_TRIGGER_TYPE;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_objectMetadata);
    message.payload.widget.id = textBase + CELL_FIELD_OBJECT_METADATA;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_extraIndex);
    message.payload.widget.id = textBase + CELL_FIELD_EXTRA_INDEX;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_objectLink);
    message.payload.widget.id = textBase + CELL_FIELD_OBJECT_LINK;
    gEditDialog->BroadcastMessage(message);
    sprintf(text, "%d", gEditCell->m_overlayLink);
    message.payload.widget.id = textBase + CELL_FIELD_OVERLAY_LINK;
    gEditDialog->BroadcastMessage(message);
    gpWindowManager->DoDialog(gEditDialog, CellWindowHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult == EVENTS_DIALOG_CANCEL)
        *gMap.GetCell(x, y) = original;
    else
        gEditManager->m_mapChanged = true;
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

VA(0x00412066, 0x386)
MessageDispatchResult CellWindowHandler(tag_message& message) {
    // Never read: a slot of the retail frame.
    const i16 firstTextId = CELL_WINDOW_FIRST_FIELD;
    const i16 firstToggleId = CELL_WINDOW_FIRST_FLAG;
    i32 value;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case EVENTS_DIALOG_CANCEL:
                        case EVENTS_DIALOG_OK:
                            gpWindowManager->m_dialogResult = message.payload.widget.id;
                            FINISH_EDIT_DIALOG(message);
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    switch (message.payload.widget.id) {
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_TERRAIN_IMAGE:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OBJECT_TILESET:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OBJECT_INDEX:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OVERLAY_TILESET:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OVERLAY_INDEX:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_ANIMATED_OBJECT:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_ANIMATED_OVERLAY:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OBJECT_LAYER:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_ROAD:
                        case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_TRIGGER_TYPE:
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            value = atoi(message.payload.widget.data.text);
                            if (value < 0)
                                break;
                            // Rows 6, 8 and 9 store the fields HoMM1's cell
                            // editor kept there (flags, trigger type and
                            // metadata), not the ones EditCell fills them from.
                            switch (message.payload.widget.id) {
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_TERRAIN_IMAGE:
                                    gEditCell->m_terrainImageIndex = value & CELL_WINDOW_BYTE_MASK;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OBJECT_TILESET:
                                    if (value > CELL_WINDOW_MAX_TILESET)
                                        value = CELL_WINDOW_MAX_TILESET;
                                    gEditCell->m_objectTileset = value;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OBJECT_INDEX:
                                    gEditCell->m_objectIndex = value & CELL_WINDOW_BYTE_MASK;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OVERLAY_TILESET:
                                    if (value > CELL_WINDOW_MAX_TILESET)
                                        value = CELL_WINDOW_MAX_TILESET;
                                    gEditCell->m_overlayTileset = value;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_OVERLAY_INDEX:
                                    gEditCell->m_overlayIndex = value & CELL_WINDOW_BYTE_MASK;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_ANIMATED_OVERLAY:
                                    gEditCell->m_flags = value;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_ROAD:
                                    gEditCell->m_triggerType = value;
                                    break;
                                case CELL_WINDOW_FIRST_FIELD + CELL_FIELD_TRIGGER_TYPE:
                                    gEditCell->m_objectMetadata = value;
                                    break;
                            }
                            gEditDialog->DrawWindow();
                            break;
                        case CELL_WINDOW_FIRST_FLAG:
                        case CELL_WINDOW_FIRST_FLAG + 1:
                        case CELL_WINDOW_FIRST_FLAG + 2:
                        case CELL_WINDOW_FIRST_FLAG + 3:
                        case CELL_WINDOW_FIRST_FLAG + 4:
                        case CELL_WINDOW_FIRST_FLAG + 5:
                        case CELL_WINDOW_FIRST_FLAG + 6:
                        case CELL_WINDOW_LAST_FLAG:
                            gEditCell->m_flags ^= 1 << (message.payload.widget.id - firstToggleId);
                            message.type = MESSAGE_WIDGET;
                            message.payload.widget.command =
                                gEditCell->m_flags & (1 << (message.payload.widget.id - firstToggleId))
                                    ? WIDGET_COMMAND_SET_FLAGS
                                    : WIDGET_COMMAND_CLEAR_FLAGS;
                            message.payload.widget.data.value = WIDGET_FLAG_DRAW;
                            gEditDialog->BroadcastMessage(message);
                            gEditDialog->DrawWindow();
                            break;
                        case CELL_WINDOW_ACTION_TOGGLE:
                            gEditCell->m_triggerType ^= MAP_TRIGGER_ACTION_FLAG;
                            message.type = MESSAGE_WIDGET;
                            message.payload.widget.command =
                                gEditCell->m_triggerType & MAP_TRIGGER_ACTION_FLAG
                                    ? WIDGET_COMMAND_SET_FLAGS
                                    : WIDGET_COMMAND_CLEAR_FLAGS;
                            message.payload.widget.data.value = WIDGET_FLAG_DRAW;
                            gEditDialog->BroadcastMessage(message);
                            gEditDialog->DrawWindow();
                            break;
                    }
                    break;
            }
            break;
        case MESSAGE_KEY_DOWN:
            switch (message.payload.keyboard.keyCode) {
                case INPUT_SCAN_ESCAPE:
                    FINISH_EDIT_DIALOG(message);
                    return MESSAGE_DISPATCH_FORWARD;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x004123ec, 0x215)
void eventsManager::EditMonster(i32 x, i32 y, b32 ultimateArtifact) {
    // Never read: a slot of the retail frame.
    i32 unused;
    char buffer[EVENTS_NUMBER_TEXT_SIZE];
    tag_message message;

    gEditUltimateArtifact = ultimateArtifact;
    gEditCell = gMap.GetCell(x, y);
    if (ultimateArtifact) {
        gEditDialog = new heroWindow(EVENTS_DIALOG_X, EVENTS_DIALOG_Y, "ultaedit.bin");
        SetWinText(gEditDialog, EVENTS_WINDOW_TEXT_ULTIMATE_ARTIFACT);
    } else {
        gEditDialog = new heroWindow(EVENTS_DIALOG_X, EVENTS_DIALOG_Y, "monedit.bin");
        SetWinText(gEditDialog, EVENTS_WINDOW_TEXT_MONSTER);
    }
    gMonsterCountEdit = gEditCell->m_objectMetadata;
    sprintf(buffer, "%d", gEditCell->m_objectMetadata);
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, MONSTER_WINDOW_COUNT);
    message.payload.widget.data.text = buffer;
    gEditDialog->BroadcastMessage(message);
    gpWindowManager->DoDialog(gEditDialog, MonsterWindowHandler, 0);
    delete gEditDialog;
    if (gpWindowManager->m_dialogResult != EVENTS_DIALOG_CANCEL) {
        gEditManager->m_mapChanged = true;
        gEditCell->m_objectMetadata = gMonsterCountEdit;
    }
    gEditManager->DrawMap();
    gEditManager->UpdateMapView();
}

VA(0x00412601, 0x20f)
MessageDispatchResult MonsterWindowHandler(tag_message& message) {
    b8 outOfRange;
    i32 value;
    i32 original;
    tag_message reply;

    switch (message.type) {
        case MESSAGE_WIDGET:
            switch (message.payload.widget.command) {
                case WIDGET_NOTIFY_DESELECT:
                    switch (message.payload.widget.id) {
                        case EVENTS_DIALOG_CANCEL:
                        case EVENTS_DIALOG_OK:
                            gpWindowManager->m_dialogResult = message.payload.widget.id;
                            FINISH_EDIT_DIALOG(message);
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                    break;
                case WIDGET_NOTIFY_SELECT:
                    switch (message.payload.widget.id) {
                        case MONSTER_WINDOW_COUNT:
                            outOfRange = false;
                            message.payload.widget.command = WIDGET_COMMAND_GET_TEXT;
                            gEditDialog->BroadcastMessage(message);
                            value = atoi(message.payload.widget.data.text);
                            original = value;
                            if (value < 0)
                                value = 0;
                            if (gEditUltimateArtifact) {
                                if (value > ULTIMATE_ARTIFACT_WINDOW_MAX_RADIUS)
                                    value = ULTIMATE_ARTIFACT_WINDOW_MAX_RADIUS;
                            } else if (value > MONSTER_WINDOW_MAX_COUNT) {
                                value = MONSTER_WINDOW_MAX_COUNT;
                            }
                            if (value != original)
                                outOfRange = true;
                            gMonsterCountEdit = value;
                            sprintf(gText, "%d", gMonsterCountEdit);
                            SET_WIDGET_MESSAGE(reply, WIDGET_COMMAND_SET_TEXT, MONSTER_WINDOW_COUNT);
                            reply.payload.widget.data.text = gText;
                            gEditDialog->BroadcastMessage(reply);
                            gEditDialog->DrawWindow();
                            if (outOfRange) {
                                if (gEditUltimateArtifact)
                                    NormalDialog(
                                        localization::Tr("editor.events.ultimate_artifact.radius_range"),
                                        NORMAL_DIALOG_INFO
                                    );
                                else
                                    NormalDialog(
                                        localization::Tr("editor.events.monster.count_range"),
                                        NORMAL_DIALOG_INFO
                                    );
                            }
                            break;
                    }
                    break;
                // The retail handler tests the key-down message type against
                // the widget command here, so Escape never reaches this case.
                case BaseWidgetCommand(IDX(MESSAGE_KEY_DOWN)):
                    switch (message.payload.keyboard.keyCode) {
                        case INPUT_SCAN_ESCAPE:
                            FINISH_EDIT_DIALOG(message);
                            return MESSAGE_DISPATCH_FORWARD;
                    }
                    break;
            }
            break;
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x00412810, 0x354)
b32 NewMapDialog(void) {
    i32 i;

    gNewMapWindow = new heroWindow(0, 0, "editnew.bin");
    if (gNewMapWindow == NULL)
        MemError();
    for (i = 0; i < RANDOM_MAP_TERRAIN_COUNT; i++) {
        gTerrainTracks[i] = new iconWidget(
            NEW_MAP_TRACK_X,
            i * NEW_MAP_ROW_HEIGHT + NEW_MAP_FIRST_TERRAIN_Y,
            NEW_MAP_TRACK_WIDTH,
            NEW_MAP_TRACK_HEIGHT,
            "escroll.icn",
            NEW_MAP_TRACK_FRAME,
            ICON_DRAW_NORMAL,
            i + NEW_MAP_FIRST_TERRAIN_TRACK,
            WIDGET_KIND_ICON_DIRECT,
            NEW_MAP_SLIDER_FILL_COLOR
        );
        gNewMapWindow->AddWidget(gTerrainTracks[i], -1);
        gTerrainKnobs[i] = new iconWidget(
            NEW_MAP_KNOB_LEFT,
            i * NEW_MAP_ROW_HEIGHT + NEW_MAP_FIRST_TERRAIN_Y + NEW_MAP_KNOB_Y_OFFSET,
            NEW_MAP_KNOB_WIDTH,
            NEW_MAP_KNOB_HEIGHT,
            "escroll.icn",
            NEW_MAP_KNOB_FRAME,
            ICON_DRAW_NORMAL,
            i + NEW_MAP_FIRST_TERRAIN_KNOB,
            WIDGET_KIND_ICON_DIRECT,
            NEW_MAP_SLIDER_FILL_COLOR
        );
        gNewMapWindow->AddWidget(gTerrainKnobs[i], -1);
    }
    for (i = 0; i < RANDOM_MAP_DENSITY_COUNT; i++) {
        gDensityTracks[i] = new iconWidget(
            NEW_MAP_TRACK_X,
            i * NEW_MAP_ROW_HEIGHT + NEW_MAP_FIRST_DENSITY_Y,
            NEW_MAP_TRACK_WIDTH,
            NEW_MAP_TRACK_HEIGHT,
            "escroll.icn",
            NEW_MAP_TRACK_FRAME,
            ICON_DRAW_NORMAL,
            i + NEW_MAP_FIRST_DENSITY_TRACK,
            WIDGET_KIND_ICON_DIRECT,
            NEW_MAP_SLIDER_FILL_COLOR
        );
        gNewMapWindow->AddWidget(gDensityTracks[i], -1);
        gDensityKnobs[i] = new iconWidget(
            NEW_MAP_KNOB_LEFT,
            i * NEW_MAP_ROW_HEIGHT + NEW_MAP_FIRST_DENSITY_Y + NEW_MAP_KNOB_Y_OFFSET,
            NEW_MAP_KNOB_WIDTH,
            NEW_MAP_KNOB_HEIGHT,
            "escroll.icn",
            NEW_MAP_KNOB_FRAME,
            ICON_DRAW_NORMAL,
            i + NEW_MAP_FIRST_DENSITY_KNOB,
            WIDGET_KIND_ICON_DIRECT,
            NEW_MAP_SLIDER_FILL_COLOR
        );
        gNewMapWindow->AddWidget(gDensityKnobs[i], -1);
    }
    UpdateNewMapWindow();
    gpWindowManager->DoDialog(gNewMapWindow, NewMapWindowHandler, 0);
    delete gNewMapWindow;
    gNewMapWindow = NULL;
    BalanceTerrainPercents(NEW_MAP_NO_TERRAIN);
    if (gpWindowManager->m_dialogResult == EVENTS_DIALOG_CANCEL)
        return false;
    return true;
}

VA(0x00412b64, 0x166)
void UpdateNewMapWindow(void) {
    tag_message message;
    i32 i;

    for (i = 0; i < RANDOM_MAP_TERRAIN_COUNT; i++)
        gTerrainKnobs[i]->m_x =
            NEW_MAP_KNOB_TRAVEL * gTerrainPercent[i] / NEW_MAP_ALL_PERCENT + NEW_MAP_KNOB_LEFT;
    for (i = 0; i < RANDOM_MAP_DENSITY_COUNT; i++)
        gDensityKnobs[i]->m_x =
            NEW_MAP_KNOB_TRAVEL * gDensityPercent[i] / NEW_MAP_ALL_PERCENT + NEW_MAP_KNOB_LEFT;
    message.type = MESSAGE_WIDGET;
    message.payload.widget.data.value = WIDGET_FLAG_DRAW;
    message.payload.widget.id = NEW_MAP_SCATTER_TOWNS;
    message.payload.widget.command =
        gScatterTowns ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gNewMapWindow->BroadcastMessage(message);
    message.payload.widget.id = NEW_MAP_CENTRE_TOWNS;
    message.payload.widget.command =
        gScatterTowns ? WIDGET_COMMAND_CLEAR_FLAGS : WIDGET_COMMAND_SET_FLAGS;
    gNewMapWindow->BroadcastMessage(message);
    message.payload.widget.id = NEW_MAP_GENERATE_UNSEEN;
    message.payload.widget.command =
        gGenerateUnseen ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
    gNewMapWindow->BroadcastMessage(message);
    for (i = NEW_MAP_MIN_PLAYERS; i <= NEW_MAP_MAX_PLAYERS; i++) {
        message.payload.widget.id = i + NEW_MAP_PLAYERS_BASE;
        message.payload.widget.command =
            gRandomMapPlayers == i ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
        gNewMapWindow->BroadcastMessage(message);
    }
}

#define othersTotal unfixedTotal // frame-slot spelling
#define landTotal total          // frame-slot spelling
VA(0x00412cca, 0x193)
void BalanceTerrainPercents(i32 changedTerrain) {
    double landTotal;
    i32 i;
    double ratio;
    double remaining;
    double othersTotal;

    remaining = NEW_MAP_ALL_PERCENT - gTerrainPercent[changedTerrain];
    othersTotal = 0.0;
    landTotal = 0.0;
    for (i = 0; i < RANDOM_MAP_TERRAIN_COUNT; i++)
        if (i != changedTerrain)
            othersTotal += gTerrainPercent[i];
    for (i = NEW_MAP_TERRAIN_GRASS; i < RANDOM_MAP_TERRAIN_COUNT; i++)
        landTotal += gTerrainPercent[i];
    if (changedTerrain != NEW_MAP_NO_TERRAIN) {
        if (landTotal < NEW_MAP_MINIMUM_LAND)
            gTerrainPercent[NEW_MAP_TERRAIN_GRASS] += NEW_MAP_MINIMUM_LAND - landTotal;
    } else {
        if (othersTotal < NEW_MAP_ONE_PERCENT) {
            othersTotal = NEW_MAP_ONE_PERCENT;
            if (gTerrainPercent[NEW_MAP_TERRAIN_WATER] < NEW_MAP_ONE_PERCENT)
                gTerrainPercent[NEW_MAP_TERRAIN_WATER] = NEW_MAP_ONE_PERCENT;
            else
                gTerrainPercent[NEW_MAP_TERRAIN_GRASS] = NEW_MAP_ONE_PERCENT;
        }
        ratio = remaining / othersTotal;
        for (i = 0; i < RANDOM_MAP_TERRAIN_COUNT; i++)
            if (i != changedTerrain)
                gTerrainPercent[i] = ratio * gTerrainPercent[i];
        if (gTerrainPercent[NEW_MAP_TERRAIN_WATER]
            > NEW_MAP_MAXIMUM_WATER + NEW_MAP_PERCENT_ROUNDING) {
            gTerrainPercent[NEW_MAP_TERRAIN_WATER] = NEW_MAP_MAXIMUM_WATER;
            BalanceTerrainPercents(NEW_MAP_TERRAIN_WATER);
        }
    }
}
#undef othersTotal
#undef landTotal

VA(0x00412e5d, 0x3de)
MessageDispatchResult NewMapWindowHandler(tag_message& message) {
    b32 redraw = false;
    i32 index;

    if (message.type != MESSAGE_WIDGET)
        return MESSAGE_DISPATCH_CONTINUE;
    switch (message.payload.widget.command) {
        case WIDGET_NOTIFY_DESELECT:
            redraw = true;
            if (message.payload.widget.id >= NEW_MAP_FIRST_TERRAIN_DECREASE
                && message.payload.widget.id
                       < NEW_MAP_FIRST_TERRAIN_DECREASE + RANDOM_MAP_TERRAIN_COUNT) {
                index = message.payload.widget.id - NEW_MAP_FIRST_TERRAIN_DECREASE;
                gTerrainPercent[index] -= NEW_MAP_ONE_PERCENT;
                if (gTerrainPercent[index] < 0.0)
                    gTerrainPercent[index] = 0.0;
                BalanceTerrainPercents(index);
            } else if (message.payload.widget.id >= NEW_MAP_FIRST_TERRAIN_INCREASE
                       && message.payload.widget.id
                              < NEW_MAP_FIRST_TERRAIN_INCREASE + RANDOM_MAP_TERRAIN_COUNT) {
                index = message.payload.widget.id - NEW_MAP_FIRST_TERRAIN_INCREASE;
                gTerrainPercent[index] += NEW_MAP_ONE_PERCENT;
                if (gTerrainPercent[index] > NEW_MAP_ALL_PERCENT)
                    gTerrainPercent[index] = NEW_MAP_ALL_PERCENT;
                BalanceTerrainPercents(index);
            } else if (message.payload.widget.id >= NEW_MAP_FIRST_DENSITY_DECREASE
                       && message.payload.widget.id
                              < NEW_MAP_FIRST_DENSITY_DECREASE + RANDOM_MAP_DENSITY_COUNT) {
                index = message.payload.widget.id - NEW_MAP_FIRST_DENSITY_DECREASE;
                gDensityPercent[index] -= NEW_MAP_ONE_PERCENT;
                if (gDensityPercent[index] < 0.0)
                    gDensityPercent[index] = 0.0;
            } else if (message.payload.widget.id >= NEW_MAP_FIRST_DENSITY_INCREASE
                       && message.payload.widget.id
                              < NEW_MAP_FIRST_DENSITY_INCREASE + RANDOM_MAP_DENSITY_COUNT) {
                index = message.payload.widget.id - NEW_MAP_FIRST_DENSITY_INCREASE;
                gDensityPercent[index] += NEW_MAP_ONE_PERCENT;
                if (gDensityPercent[index] > NEW_MAP_ALL_PERCENT)
                    gDensityPercent[index] = NEW_MAP_ALL_PERCENT;
            } else {
                redraw = false;
                if (message.payload.widget.id == EVENTS_DIALOG_OK
                    || message.payload.widget.id == EVENTS_DIALOG_CANCEL) {
                    gpWindowManager->m_dialogResult = message.payload.widget.id;
                    FINISH_EDIT_DIALOG(message);
                    return MESSAGE_DISPATCH_FORWARD;
                }
            }
            break;
        case WIDGET_NOTIFY_SELECT:
            // The density rows' ranges span RANDOM_MAP_TERRAIN_COUNT ids too.
            if (message.payload.widget.id >= NEW_MAP_FIRST_TERRAIN_TRACK
                && message.payload.widget.id
                       < NEW_MAP_FIRST_TERRAIN_TRACK + RANDOM_MAP_TERRAIN_COUNT)
                DragNewMapSlider(true, message.payload.widget.id - NEW_MAP_FIRST_TERRAIN_TRACK);
            else if (message.payload.widget.id >= NEW_MAP_FIRST_TERRAIN_KNOB
                     && message.payload.widget.id
                            < NEW_MAP_FIRST_TERRAIN_KNOB + RANDOM_MAP_TERRAIN_COUNT)
                DragNewMapSlider(true, message.payload.widget.id - NEW_MAP_FIRST_TERRAIN_KNOB);
            else if (message.payload.widget.id >= NEW_MAP_FIRST_DENSITY_TRACK
                     && message.payload.widget.id
                            < NEW_MAP_FIRST_DENSITY_TRACK + RANDOM_MAP_TERRAIN_COUNT)
                DragNewMapSlider(false, message.payload.widget.id - NEW_MAP_FIRST_DENSITY_TRACK);
            else if (message.payload.widget.id >= NEW_MAP_FIRST_DENSITY_KNOB
                     && message.payload.widget.id
                            < NEW_MAP_FIRST_DENSITY_KNOB + RANDOM_MAP_TERRAIN_COUNT)
                DragNewMapSlider(false, message.payload.widget.id - NEW_MAP_FIRST_DENSITY_KNOB);
            if (message.payload.widget.id >= NEW_MAP_SCATTER_TOWNS
                && message.payload.widget.id <= NEW_MAP_CENTRE_TOWNS) {
                gScatterTowns = message.payload.widget.id == NEW_MAP_SCATTER_TOWNS;
                redraw = true;
            }
            if (message.payload.widget.id == NEW_MAP_GENERATE_UNSEEN) {
                gGenerateUnseen = 1 - gGenerateUnseen;
                redraw = true;
            }
            if (message.payload.widget.id >= NEW_MAP_PLAYERS_BASE + NEW_MAP_MIN_PLAYERS
                && message.payload.widget.id <= NEW_MAP_PLAYERS_BASE + NEW_MAP_MAX_PLAYERS) {
                gRandomMapPlayers = message.payload.widget.id - NEW_MAP_PLAYERS_BASE;
                redraw = true;
            }
            break;
    }
    if (redraw) {
        UpdateNewMapWindow();
        gNewMapWindow->DrawWindow();
    }
    return MESSAGE_DISPATCH_CONSUME;
}

VA(0x0041323b, 0x233)
void DragNewMapSlider(b32 terrainRow, i32 index) {
    tag_message last;
    double knobPercent;
    i32 x;
    i32 y;
    tag_message event;

    gpMouseManager->MouseCoords(x, y);
    gpInputManager->Flush();
    event.type = MESSAGE_MOUSE_MOVE;
    event.payload.mouse.x = x;
    event.payload.mouse.y = y;
    while (event.type != MESSAGE_LEFT_BUTTON_UP && event.type != MESSAGE_RIGHT_BUTTON_UP) {
        Process1WindowsMessage();
        if (event.type == MESSAGE_MOUSE_MOVE) {
            last = event;
            while (event.type == MESSAGE_MOUSE_MOVE) {
                last = event;
                event = gpInputManager->GetEvent();
            }
            last.payload.mouse.x -= NEW_MAP_KNOB_GRAB;
            if (last.payload.mouse.x < NEW_MAP_KNOB_LEFT)
                last.payload.mouse.x = NEW_MAP_KNOB_LEFT;
            if (last.payload.mouse.x > NEW_MAP_KNOB_RIGHT)
                last.payload.mouse.x = NEW_MAP_KNOB_RIGHT;
            gpMouseManager->Main(last);
            knobPercent = (last.payload.mouse.x - NEW_MAP_KNOB_LEFT) * NEW_MAP_PERCENT
                          / (NEW_MAP_KNOB_RIGHT - NEW_MAP_KNOB_LEFT);
            if (terrainRow) {
                gTerrainPercent[index] = knobPercent;
                BalanceTerrainPercents(index);
            } else
                gDensityPercent[index] = knobPercent;
            UpdateNewMapWindow();
            gNewMapWindow->DrawWindow();
        } else
            event = gpInputManager->GetEvent();
    }
    gpInputManager->Flush();
    if (terrainRow) {
        gTerrainKnobs[index]->m_flags &= ~WIDGET_FLAG_SELECTED;
        gTerrainTracks[index]->m_flags &= ~WIDGET_FLAG_SELECTED;
    } else {
        gDensityKnobs[index]->m_flags &= ~WIDGET_FLAG_SELECTED;
        gDensityTracks[index]->m_flags &= ~WIDGET_FLAG_SELECTED;
    }
}

// Compiler-emitted vtable; the marker is a census claim, not a definition.
VTBL(eventsManager, 0x0045b3a8)
