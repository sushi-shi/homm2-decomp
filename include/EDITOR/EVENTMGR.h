#ifndef HOMM2_EDITOR_EVENTMGR_H
#define HOMM2_EDITOR_EVENTMGR_H

// The detail tool's unit (src/EDITOR/EVENTMGR.cpp): the raw cell editor's
// and the monster editor's handlers, the random map generator's settings
// dialog, their constants and data. The class itself is eventsManager
// (eventsManager.h).

#include <va.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>

class heroWindow;
class iconWidget;

H2_ENUM_BEGIN(EventsManagerLayout)
    // The cell and monster dialogs open at (16, 16).
    EVENTS_DIALOG_X = 16,
    EVENTS_DIALOG_Y = 16,
    // The hovered cell's outline (overlay.icn) in its palette colour over a
    // cell with its own editor, and over any other.
    EVENTS_HOVER_DETAIL_COLOR = 90,
    EVENTS_HOVER_COLOR        = 10,
    // An artifact sprite's index halves to the artifact; the spell scroll
    // opens the scroll's spell editor.
    EVENTS_ARTIFACT_SPRITE_FRAMES = 2
H2_ENUM_END(EventsManagerLayout)

H2_ENUM_BEGIN(CellWindowConstant)
    // cellwin.bin: a text field per mapCell field from CELL_WINDOW_FIRST_FIELD
    // (CellWindowField order), a toggle per m_flags bit from
    // CELL_WINDOW_FIRST_FLAG and a toggle for the trigger's action bit.
    CELL_WINDOW_FIRST_FIELD     = 0x2bc,
    CELL_WINDOW_FIRST_FLAG      = 0x40,
    CELL_WINDOW_LAST_FLAG       = 0x47,
    CELL_WINDOW_ACTION_TOGGLE   = 0x48,
    CELL_WINDOW_BYTE_MASK       = 0xff,
    // The largest tileset a tileset field accepts.
    CELL_WINDOW_MAX_TILESET     = 15,
    // The debug level the raw cell editor needs.
    CELL_WINDOW_DEBUG_LEVEL     = 1
H2_ENUM_END(CellWindowConstant)

H2_ENUM_BEGIN(CellWindowField)
    // cellwin.bin's text fields, offsets from CELL_WINDOW_FIRST_FIELD.
    CELL_FIELD_TERRAIN_IMAGE   = 0,
    CELL_FIELD_OBJECT_TILESET  = 1,
    CELL_FIELD_OBJECT_INDEX    = 2,
    CELL_FIELD_OVERLAY_TILESET = 3,
    CELL_FIELD_OVERLAY_INDEX   = 4,
    CELL_FIELD_ANIMATED_OBJECT  = 5,
    CELL_FIELD_ANIMATED_OVERLAY = 6,
    CELL_FIELD_OBJECT_SHADOW    = 7,
    CELL_FIELD_ROAD            = 8,
    CELL_FIELD_TRIGGER_TYPE    = 9,
    CELL_FIELD_OBJECT_METADATA = 10,
    CELL_FIELD_EXTRA_INDEX     = 11,
    CELL_FIELD_OBJECT_LINK     = 12,
    CELL_FIELD_OVERLAY_LINK    = 13
H2_ENUM_END(CellWindowField)

H2_ENUM_BEGIN(MonsterWindowConstant)
    // monedit.bin and ultaedit.bin: the monster count or the ultimate
    // artifact's dig radius (mapCell::m_objectMetadata; 0 lets the game
    // choose).
    MONSTER_WINDOW_COUNT            = 0x20a,
    MONSTER_WINDOW_MAX_COUNT        = 4000,
    ULTIMATE_ARTIFACT_WINDOW_MAX_RADIUS = 127
H2_ENUM_END(MonsterWindowConstant)

H2_ENUM_BEGIN(NewMapWindowConstant)
    // editnew.bin: a track and a knob (escroll.icn) per terrain and density
    // row, the arrow buttons, the town placement pair, the generate-unseen
    // toggle and a radio button per player count.
    NEW_MAP_FIRST_TERRAIN_DECREASE = 100,
    NEW_MAP_FIRST_TERRAIN_INCREASE = 200,
    NEW_MAP_FIRST_TERRAIN_TRACK    = 400,
    NEW_MAP_FIRST_TERRAIN_KNOB     = 500,
    NEW_MAP_FIRST_DENSITY_DECREASE = 600,
    NEW_MAP_FIRST_DENSITY_INCREASE = 700,
    NEW_MAP_FIRST_DENSITY_TRACK    = 900,
    NEW_MAP_FIRST_DENSITY_KNOB     = 1000,
    NEW_MAP_SCATTER_TOWNS          = 1100,
    NEW_MAP_CENTRE_TOWNS           = 1101,
    NEW_MAP_GENERATE_UNSEEN        = 1300,
    // The player count radio buttons: NEW_MAP_PLAYERS_BASE + players.
    NEW_MAP_PLAYERS_BASE           = 1498,
    NEW_MAP_MIN_PLAYERS            = 2,
    NEW_MAP_MAX_PLAYERS            = 6,
    NEW_MAP_TRACK_X                = 154,
    NEW_MAP_TRACK_WIDTH            = 250,
    NEW_MAP_TRACK_HEIGHT           = 16,
    NEW_MAP_KNOB_WIDTH             = 17,
    NEW_MAP_KNOB_HEIGHT            = 8,
    NEW_MAP_KNOB_Y_OFFSET          = 3,
    NEW_MAP_ROW_HEIGHT             = 25,
    NEW_MAP_FIRST_TERRAIN_Y        = 51,
    NEW_MAP_FIRST_DENSITY_Y        = 295,
    // The scroll bar frames of escroll.icn the rows draw.
    NEW_MAP_TRACK_FRAME            = 20,
    NEW_MAP_KNOB_FRAME             = 2,
    NEW_MAP_SLIDER_FILL_COLOR      = 1,
    // A knob travels from NEW_MAP_KNOB_LEFT to NEW_MAP_KNOB_RIGHT and is
    // grabbed by its middle; UpdateNewMapWindow places it over
    // NEW_MAP_KNOB_TRAVEL pixels.
    NEW_MAP_KNOB_LEFT              = 157,
    NEW_MAP_KNOB_RIGHT             = 383,
    NEW_MAP_KNOB_TRAVEL            = 227,
    NEW_MAP_KNOB_GRAB              = 8,
    NEW_MAP_PERCENT                = 100,
    // The land terrains keep at least 20 percent; water is capped at 75.
    NEW_MAP_MINIMUM_LAND           = 20,
    NEW_MAP_MAXIMUM_WATER          = 75
H2_ENUM_END(NewMapWindowConstant)

// The generator's shares are doubles in percent: the whole map, the arrow
// buttons' step and the least share a rebalance leaves, and the slack
// before water counts as over its cap.
#define NEW_MAP_ALL_PERCENT      100.0
#define NEW_MAP_ONE_PERCENT      1.0
#define NEW_MAP_PERCENT_ROUNDING 0.5

H2_ENUM_BEGIN(NewMapTerrain)
    // gTerrainPercent's rows; BalanceTerrainPercents takes NEW_MAP_NO_TERRAIN
    // as the dialog closes.
    NEW_MAP_NO_TERRAIN    = -1,
    NEW_MAP_TERRAIN_WATER = 0,
    NEW_MAP_TERRAIN_GRASS = 1
H2_ENUM_END(NewMapTerrain)

MessageDispatchResult CellWindowHandler(struct tag_message& message);
MessageDispatchResult MonsterWindowHandler(struct tag_message& message);
// Sets up the random map generator (editnew.bin); returns false when
// cancelled.
b32 NewMapDialog(void);
void UpdateNewMapWindow(void);
// After a slider changed terrain changedTerrain, tops grass up to the land
// minimum. Without one (NEW_MAP_NO_TERRAIN, as the window closes) it scales
// the other terrains to fill 100 percent, then caps water and rebalances
// from it.
void BalanceTerrainPercents(i32 changedTerrain);
MessageDispatchResult NewMapWindowHandler(struct tag_message& message);
// Drags a generator slider: a terrain row when terrainRow is set, else a
// density row.
void DragNewMapSlider(b32 terrainRow, i32 index);

// Whether the monster dialog edits the ultimate artifact's radius, and the
// count or radius it edits.
extern b32 gEditUltimateArtifact;
extern i32 gMonsterCountEdit;
extern iconWidget* gDensityTracks[RANDOM_MAP_DENSITY_COUNT];
extern iconWidget* gTerrainKnobs[RANDOM_MAP_TERRAIN_COUNT];
extern iconWidget* gDensityKnobs[RANDOM_MAP_DENSITY_COUNT];
extern iconWidget* gTerrainTracks[RANDOM_MAP_TERRAIN_COUNT];
extern heroWindow* gNewMapWindow;

#endif
