#ifndef HOMM2_EDITOR_EVENTSMANAGER_H
#define HOMM2_EDITOR_EVENTSMANAGER_H


#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/baseManager.h>
#include <BASE/dialog.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>
#include <SOURCE/kbTypes.h>

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

    CELL_WINDOW_MAX_TILESET     = 15
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
    NEW_MAP_SCATTER_TERRAIN          = 1100,
    NEW_MAP_CENTRE_TERRAIN           = 1101,
    NEW_MAP_GENERATE_UNSEEN        = 1300,

    NEW_MAP_PLAYERS_BASE           = 1498,
    NEW_MAP_MIN_PLAYERS            = 2,
    NEW_MAP_DEFAULT_PLAYERS        = 4,
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

class icon;
struct mapEventExtra;
struct rumourEventExtra;
struct signEventExtra;

typedef enum EventsDialogButton {


    EVENTS_DIALOG_CANCEL = DIALOG_BUTTON_1,
    EVENTS_DIALOG_OK     = DIALOG_BUTTON_2
} EventsDialogButton;

typedef enum EventsArtifactList {


    EVENTS_HIDDEN_ARTIFACT_COUNT =
        (ARTIFACT_SPELL_SCROLL) - (ARTIFACT_EDITOR_ANY_ULTIMATE) + 1
} EventsArtifactList;

typedef enum EventTextConstant {


    EVENT_TEXT_CAPACITY = 2000,
    EVENT_TEXT_FIELD    = 0x78,

    EVENTS_NUMBER_TEXT_SIZE = 20,
    EVENTS_FIELD_TEXT_SIZE  = 50
} EventTextConstant;


#define FINISH_EDIT_DIALOG(message)                                                                \
    ((message).type = MESSAGE_WIDGET,                                                              \
     (message).payload.widget.id = (WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

#pragma pack(push, 1)
class eventsManager : public baseManager {
public:

    icon* m_overlayIcon;

    eventsManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;
    virtual MessageDispatchResult Main(struct tag_message& message) override;

    void EditCell(i32 x, i32 y);

    void EditMonster(i32 x, i32 y, b32 ultimateArtifact);

    i32 EditEvent(i32 extra);
    void FillInEventEdit(struct EventExtra* event);

    void EditHero(i32 x, i32 y, b32 jailed);
    void FillInHeroEdit(struct HeroExtra* hero);

    void EditTown(i32 x, i32 y);
    void FillInTownEdit(struct TownExtra* town);

    i32 EditSphinx(i32 extra);
    void FillInSphinxEdit(mapEventExtra* sphinx);

    i32 EditRumour(i32 extra);
    void FillInRumourEdit(rumourEventExtra* rumour);

    void EditSign(i32 x, i32 y);
    void FillInSignEdit(signEventExtra* sign);

    void EditSpellScroll(i32* spell);
};
#pragma pack(pop)

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
