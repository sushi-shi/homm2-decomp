#ifndef HOMM2_EDITOR_EVENTMGR_H
#define HOMM2_EDITOR_EVENTMGR_H


#include <Ints.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>

class heroWindow;
class iconWidget;

typedef enum EventsManagerLayout {

    EVENTS_DIALOG_X = 16,
    EVENTS_DIALOG_Y = 16,


    EVENTS_HOVER_DETAIL_COLOR = 90,
    EVENTS_HOVER_COLOR        = 10,


    EVENTS_ARTIFACT_SPRITE_FRAMES = 2
} EventsManagerLayout;

typedef enum CellWindowConstant {


    CELL_WINDOW_FIRST_FIELD     = 0x2bc,
    CELL_WINDOW_FIRST_FLAG      = 0x40,
    CELL_WINDOW_LAST_FLAG       = 0x47,
    CELL_WINDOW_ACTION_TOGGLE   = 0x48,
    CELL_WINDOW_BYTE_MASK       = 0xff,

    CELL_WINDOW_MAX_TILESET     = 15,

    CELL_WINDOW_DEBUG_LEVEL     = 1
} CellWindowConstant;

typedef enum CellWindowField {

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
} CellWindowField;

typedef enum MonsterWindowConstant {


    MONSTER_WINDOW_COUNT            = 0x20a,
    MONSTER_WINDOW_MAX_COUNT        = 4000,
    ULTIMATE_ARTIFACT_WINDOW_MAX_RADIUS = 127
} MonsterWindowConstant;

typedef enum NewMapWindowConstant {


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

    NEW_MAP_TRACK_FRAME            = 20,
    NEW_MAP_KNOB_FRAME             = 2,
    NEW_MAP_SLIDER_FILL_COLOR      = 1,


    NEW_MAP_KNOB_LEFT              = 157,
    NEW_MAP_KNOB_RIGHT             = 383,
    NEW_MAP_KNOB_TRAVEL            = 227,
    NEW_MAP_KNOB_GRAB              = 8,
    NEW_MAP_PERCENT                = 100,

    NEW_MAP_MINIMUM_LAND           = 20,
    NEW_MAP_MAXIMUM_WATER          = 75
} NewMapWindowConstant;


#define NEW_MAP_ALL_PERCENT      100.0
#define NEW_MAP_ONE_PERCENT      1.0
#define NEW_MAP_PERCENT_ROUNDING 0.5

typedef enum NewMapTerrain {


    NEW_MAP_NO_TERRAIN    = -1,
    NEW_MAP_TERRAIN_WATER = 0,
    NEW_MAP_TERRAIN_GRASS = 1
} NewMapTerrain;

MessageDispatchResult CellWindowHandler(struct tag_message& message);
MessageDispatchResult MonsterWindowHandler(struct tag_message& message);


b32 NewMapDialog(void);
void UpdateNewMapWindow(void);


void BalanceTerrainPercents(i32 changedTerrain);
MessageDispatchResult NewMapWindowHandler(struct tag_message& message);


void DragNewMapSlider(b32 terrainRow, i32 index);


extern b32 gEditUltimateArtifact;
extern i32 gMonsterCountEdit;
extern iconWidget* gDensityTracks[RANDOM_MAP_DENSITY_COUNT];
extern iconWidget* gTerrainKnobs[RANDOM_MAP_TERRAIN_COUNT];
extern iconWidget* gDensityKnobs[RANDOM_MAP_DENSITY_COUNT];
extern iconWidget* gTerrainTracks[RANDOM_MAP_TERRAIN_COUNT];
extern heroWindow* gNewMapWindow;

#endif
