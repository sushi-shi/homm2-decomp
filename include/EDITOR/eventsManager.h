#ifndef HOMM2_EDITOR_EVENTSMANAGER_H
#define HOMM2_EDITOR_EVENTSMANAGER_H

// The detail tool (src/EDITOR/EVENTMGR.cpp): a click on a map cell opens the
// editor for the object there. The unit name EVENTMGR is descriptive; Open
// stores the class name "eventsManager". Each object's own dialog editor
// lives in its own unit (evntedit, heroedit, ridledit, rumredit, signedit,
// townedit, x_spedit); EVENTMGR keeps the raw cell editor, the monster and
// ultimate artifact editor and the random map generator's settings dialog,
// declared after the class. This header also holds the vocabulary every
// object dialog shares.

#include <match.h>
#include <Domains.h>
#include <H2/Macros.h>
#include <BASE/baseManager.h>
#include <BASE/dialog.h>
#include <BASE/message.h>
#include <EDITOR/EDITOR.h>
#include <SOURCE/kbTypes.h>

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
    CELL_WINDOW_MAX_TILESET     = 15
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
    // row, the arrow buttons, the terrain scatter/centre pair (gScatterTerrain),
    // the generate-unseen toggle and a radio button per player count.
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
    // The player count radio buttons: NEW_MAP_PLAYERS_BASE + players.
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
    // The settings' percent scale (gTerrainPercent, gDensityPercent): the
    // generator paints a terrain at NEW_MAP_PERCENT over the whole map.
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

class icon;
struct mapEventExtra;
struct rumourEventExtra;
struct signEventExtra;

H2_ENUM_BEGIN(EventsDialogButton)
    // The editor dialogs' closing buttons (heroWindowManager::m_dialogResult):
    // cancel keeps the edited cell or record as it was.
    EVENTS_DIALOG_CANCEL = DIALOG_BUTTON_1,
    EVENTS_DIALOG_OK     = DIALOG_BUTTON_2
H2_ENUM_END(EventsDialogButton)

H2_ENUM_BEGIN(EventsArtifactList)
    // The dialogs' artifact lists leave out the editor-only artifacts from
    // ARTIFACT_EDITOR_ANY_ULTIMATE through the spell scroll.
    EVENTS_HIDDEN_ARTIFACT_COUNT =
        IDX(ARTIFACT_SPELL_SCROLL) - IDX(ARTIFACT_EDITOR_ANY_ULTIMATE) + 1
H2_ENUM_END(EventsArtifactList)

H2_ENUM_BEGIN(EventTextConstant)
    // The buffer a record's message is edited in, and the dialog's text
    // field (evntedit.bin, ridledit.bin, rumredit.bin).
    EVENT_TEXT_CAPACITY = 2000,
    EVENT_TEXT_FIELD    = 0x78,
    // The buffers the dialogs format a number or a field's text in.
    EVENTS_NUMBER_TEXT_SIZE = 20,
    EVENTS_FIELD_TEXT_SIZE  = 50
H2_ENUM_END(EventTextConstant)

// Closes the running dialog: the dialog manager reads the select command.
#define FINISH_EDIT_DIALOG(message)                                                                \
    ((message).type = MESSAGE_WIDGET,                                                              \
     (message).payload.widget.id = IDX(WIDGET_COMMAND_DIALOG_SELECT),                              \
     (message).payload.widget.command = WIDGET_COMMAND_DIALOG_SELECT)

#pragma pack(push, 1)
class eventsManager : public baseManager {
public:
    // The outline drawn over the hovered cell (overlay.icn).
    icon* m_overlayIcon;

    eventsManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    // The raw cell editor (debug level 1 and above).
    void EditCell(i32 x, i32 y);
    // A monster's count, or the ultimate artifact's dig radius.
    void EditMonster(i32 x, i32 y, b32 ultimateArtifact);
    // evntedit: a map event.
    i32 EditEvent(i32 extra);
    void FillInEventEdit(struct EventExtra* event);
    // heroedit: a hero, or a jailed hero.
    void EditHero(i32 x, i32 y, b32 jailed);
    void FillInHeroEdit(struct HeroExtra* hero);
    // townedit: a town or castle.
    void EditTown(i32 x, i32 y);
    void FillInTownEdit(struct TownExtra* town);
    // ridledit: the sphinx's riddle, answers and reward.
    i32 EditSphinx(i32 extra);
    void FillInSphinxEdit(mapEventExtra* sphinx);
    // rumredit: one of the map's rumours.
    i32 EditRumour(i32 extra);
    void FillInRumourEdit(rumourEventExtra* rumour);
    // signedit: a sign's or a bottle's message.
    void EditSign(i32 x, i32 y);
    void FillInSignEdit(signEventExtra* sign);
    // x_spedit: a spell scroll's spell.
    void EditSpellScroll(i32* spell);
};
#pragma pack(pop)
SIZE(eventsManager, 0x3a);

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

#endif // HOMM2_EDITOR_EVENTSMANAGER_H
