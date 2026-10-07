#ifndef HOMM2_EDITOR_OVERLAY_H
#define HOMM2_EDITOR_OVERLAY_H

// The object tool (src/EDITOR/OVERLAY.cpp, Price of Loyalty editor
// overlay.cpp): the editor runs overlayManager as the tool manager while the
// object tool is selected; Open stores the class name "overlayManager". The
// unit also places objects on the map for the tool and the random map
// generator. Descriptive names: every member, function and datum but the
// class name.

#include <va.h>
#include <Ints.h>
#include <BASE/baseManager.h>

class border;
class heroWindow;
class icon;
class iconWidget;
class textWidget;
struct tag_message;

H2_ENUM_BEGIN(OverlayGridConstant)
    // An object's cells: an 8 x 6 grid whose bottom-right cell (the anchor)
    // sits on the map cell the object is placed by.
    OVERLAY_GRID_WIDTH      = 8,
    OVERLAY_GRID_HEIGHT     = 6,
    OVERLAY_GRID_CELLS      = OVERLAY_GRID_WIDTH * OVERLAY_GRID_HEIGHT,
    // A grid mask keeps a bit per cell in two words, the top rows' bits
    // first: bit 47 is cell (0, 0) and bit 0 the anchor.
    OVERLAY_GRID_WORDS      = 2,
    OVERLAY_GRID_TOP        = 0,
    OVERLAY_GRID_BOTTOM     = 1,
    OVERLAY_GRID_WORD_BITS  = 32,
    // The anchor's grid cell: an object is placed by the map cell its
    // anchor sits on.
    OVERLAY_ANCHOR_X        = OVERLAY_GRID_WIDTH - 1,
    OVERLAY_ANCHOR_Y        = OVERLAY_GRID_HEIGHT - 1,
    // The anchor cell, the frame a resource marker draws.
    OVERLAY_GRID_ANCHOR     = OVERLAY_GRID_CELLS - 1,
    // overlayType::frames: a cell without a part.
    OVERLAY_NO_FRAME        = 0xff,
    // The catalogue (gOverlayTypes).
    OVERLAY_TYPE_COUNT      = 956
H2_ENUM_END(OverlayGridConstant)

// One bit per grid cell (see OVERLAY_GRID_WORDS).
typedef u32 OverlayGrid[OVERLAY_GRID_WORDS];

// overlayType::category: the class of objects a catalogue entry belongs to.
H2_ENUM_BEGIN(OverlayCategory)
    OVERLAY_CATEGORY_TERRAIN  = 0,
    OVERLAY_CATEGORY_TREASURE = 1,
    OVERLAY_CATEGORY_MONSTER  = 2,
    OVERLAY_CATEGORY_ARTIFACT = 3,
    OVERLAY_CATEGORY_TOWN     = 5,
    // The parts the editor places with other objects: town shadows and
    // grounds, flags and resource markers. No class lists them.
    OVERLAY_CATEGORY_PART     = 6,
    OVERLAY_CATEGORY_HERO     = 7
H2_ENUM_END(OverlayCategory)

// overlayType::flags.
H2_ENUM_BEGIN(OverlayTypeFlag)
    // A resource marker goes only on a mine's entrance.
    OVERLAY_FLAG_MINE_MARKER    = 1,
    // The resource cells show the type's resource marker (color).
    OVERLAY_FLAG_SHOWS_RESOURCE = 2
H2_ENUM_END(OverlayTypeFlag)

// overlayType::frameNumbering: where FillInOverlayTiles starts numbering a
// type's parts in its tileset. Any other value continues after the previous
// type (the catalogue spells it 9999); a negative one keeps the catalogue's
// frames.
H2_ENUM_BEGIN(OverlayFrameNumbering)
    // From frame 0.
    OVERLAY_FRAMES_OWN      = 0,
    // From the previous type's first frame: the two share their parts.
    OVERLAY_FRAMES_SHARED   = 1111,
    OVERLAY_FRAMES_CONTINUE = 9999
H2_ENUM_END(OverlayFrameNumbering)

#pragma pack(push, 1)
// An entry of the editor's object catalogue: the tileset its parts come
// from, its class, the masks of its grid cells and each cell's frame.
struct overlayType {
    // The type's catalogue index, which the editor compares types by.
    i32 id;
    // The catalogue's group key: the type's own id, the first of a run of
    // variants (the town shadows) or 5001 (the mountain fillers). The editor
    // never reads it.
    i32 group;
    // The picker's sort key within a class.
    i32 ordinal;
    // The adventure tileset (TilesetId) its parts come from.
    i8 tileset;
    // OverlayCategory.
    i8 category;
    // How often ScatterDecorations picks it, in tenths of a percent.
    u16 frequency;
    // The frames an animated part runs through after its own.
    u8 animationFrames;
    // The cells the object occupies.
    OverlayGrid occupiedRows;
    // The terrains (a TerrainType bit each) whose classes list it and on
    // which ScatterDecorations scatters it.
    u32 terrainMask;
    // The grounds its object-layer parts may stand on.
    u32 groundMask;
    // The cells whose part goes on the overlay layer.
    OverlayGrid overlayRows;
    // The cells that only cast a shadow.
    OverlayGrid shadowRows;
    // The animated cells.
    OverlayGrid animatedRows;
    // The cells a resource marker goes on (OVERLAY_FLAG_SHOWS_RESOURCE).
    OverlayGrid resourceRows;
    // A town's or hero's player colour (OVERLAY_NO_COLOR: none); a resource
    // marker's resource.
    u8 color;
    // OverlayTypeFlag.
    u8 flags;
    // The cells that run the object's event.
    OverlayGrid entranceRows;
    // Its object parts draw on the high layer.
    u8 highLayer;
    // The map object (MapObjectType) its cells take as their trigger.
    u8 trigger;
    // The grid columns from its leftmost part to the anchor's, as
    // FillInOverlayTiles measures them; the editor never reads it.
    u8 width;
    // OverlayFrameNumbering.
    i32 frameNumbering;
    // Each grid cell's part: a frame of the tileset (OVERLAY_NO_FRAME: none),
    // numbered by FillInOverlayTiles.
    u8 frames[OVERLAY_GRID_CELLS];
};
#pragma pack(pop)
SIZE(overlayType, 0x82);

H2_ENUM_BEGIN(OverlayClassConstant)
    // The object tool's classes: one per terrain (water to beach), then
    // towns, monsters, heroes, artifacts and treasure.
    OVERLAY_CLASS_COUNT       = 14,
    OVERLAY_CLASS_TERRAINS    = 9,
    // gObjectClassTerrains of the classes not listed by terrain.
    OVERLAY_CLASS_ANY_TERRAIN = 0xfff
H2_ENUM_END(OverlayClassConstant)

H2_ENUM_BEGIN(OverlayCatalogueEntry)
    // Each ground's mines: a mine is the entry its resource (one of
    // ore, sulfur, crystal, gems, gold) is past its ground's.
    OVERLAY_MINES_WATER       = 4,
    OVERLAY_MINES_WASTELAND   = 17,
    OVERLAY_MINES_GRASS       = 28,
    OVERLAY_MINES_SNOW        = 39,
    OVERLAY_MINES_SWAMP       = 50,
    OVERLAY_MINES_LAVA        = 61,
    OVERLAY_MINES_DESERT      = 72,
    OVERLAY_MINES_DIRT        = 85,
    // The resource markers, one per resource.
    OVERLAY_RESOURCE_MARKERS  = 128,
    // A town flag's left and right part, two per player colour.
    OVERLAY_TOWN_FLAGS        = 134,
    // The random monsters: any, then by strength.
    OVERLAY_RANDOM_MONSTER             = 214,
    OVERLAY_RANDOM_MONSTER_WEAK        = 215,
    OVERLAY_RANDOM_MONSTER_MEDIUM      = 216,
    OVERLAY_RANDOM_MONSTER_STRONG      = 217,
    OVERLAY_RANDOM_MONSTER_VERY_STRONG = 218,
    // The random artifacts.
    OVERLAY_RANDOM_TREASURE_ARTIFACT   = 337,
    OVERLAY_RANDOM_MINOR_ARTIFACT      = 338,
    OVERLAY_RANDOM_MAJOR_ARTIFACT      = 339,
    // Each ground's obelisk, sawmill and alchemist's lab (one lab serves
    // every ground but snow).
    OVERLAY_OBELISK_GRASS       = 514,
    OVERLAY_OBELISK_SNOW        = 550,
    OVERLAY_ALCHEMIST_LAB_SNOW  = 552,
    OVERLAY_SAWMILL_SNOW        = 556,
    OVERLAY_OBELISK_SWAMP       = 598,
    OVERLAY_OBELISK_LAVA        = 615,
    OVERLAY_SAWMILL_LAVA        = 618,
    OVERLAY_OBELISK_DESERT      = 655,
    OVERLAY_SAWMILL_DESERT      = 661,
    OVERLAY_OBELISK_DIRT        = 709,
    OVERLAY_OBELISK_WASTELAND   = 742,
    OVERLAY_SAWMILL_WASTELAND   = 743,
    OVERLAY_ALCHEMIST_LAB       = 770,
    OVERLAY_SAWMILL_DIRT        = 774,
    OVERLAY_STONE_LITHS         = 777,
    OVERLAY_SAWMILL_GRASS       = 789,
    // The Price of Loyalty sites whose cells keep their kind in the cell's
    // metadata: the generic sites (GenericSiteType order), the recruitment
    // sites (RecruitSiteType order), and the barriers and traveller tents,
    // one per colour.
    OVERLAY_ALCHEMIST_TOWER_SITE = 790,
    OVERLAY_ARENA_SITE           = 791,
    OVERLAY_BARROW_MOUNDS_SITE   = 792,
    OVERLAY_EARTH_ALTAR_SITE     = 793,
    OVERLAY_AIR_ALTAR_SITE       = 794,
    OVERLAY_FIRE_ALTAR_SITE      = 795,
    OVERLAY_WATER_ALTAR_SITE     = 796,
    OVERLAY_HUT_OF_MAGI_SITE     = 797,
    OVERLAY_EYE_OF_MAGI_SITE     = 798,
    OVERLAY_BARRIERS             = 799,
    OVERLAY_TRAVELER_TENTS       = 807,
    OVERLAY_STABLES_SITE         = 815,
    OVERLAY_MERMAID_SITE         = 817,
    OVERLAY_SIRENS_SITE          = 818,
    // The treasures.
    OVERLAY_ANCIENT_LAMP         = 832,
    OVERLAY_RANDOM_RESOURCE      = 833,
    OVERLAY_TREASURE_CHEST       = 834,
    // The towns and castles, twelve per player colour (a castle and a
    // town of each faction, neutral last), and the shadow of each of the
    // twelve.
    OVERLAY_TOWN_FIRST        = 835,
    OVERLAY_TOWN_LAST         = 918,
    OVERLAY_TOWN_SHADOWS      = 919,
    OVERLAY_TOWN_VARIANTS     = 12,
    // The ground under a town, one per terrain; grass when the terrain
    // left of a dragged town's entrance is water.
    OVERLAY_TOWN_GROUNDS      = 930,
    OVERLAY_TOWN_GRASS_GROUND = 931,
    // The random castles and towns, a castle and a town per colour
    // (neutral last), and their two shadows after the first pair.
    OVERLAY_RANDOM_TOWN_FIRST   = 939,
    OVERLAY_RANDOM_TOWN_SHADOWS = 941,
    OVERLAY_RANDOM_TOWN_LAST    = 954,
    OVERLAY_RANDOM_CASTLE_0     = OVERLAY_RANDOM_TOWN_FIRST,
    OVERLAY_RANDOM_CASTLE_1     = 943,
    OVERLAY_RANDOM_CASTLE_2     = 945,
    OVERLAY_RANDOM_CASTLE_3     = 947,
    OVERLAY_RANDOM_CASTLE_4     = 949,
    OVERLAY_RANDOM_CASTLE_5     = 951,
    OVERLAY_RANDOM_NEUTRAL_TOWN = OVERLAY_RANDOM_TOWN_LAST,
    // The outline DrawFootprint and CanPlaceOverlay take for every town.
    OVERLAY_TOWN_OUTLINE      = 955
H2_ENUM_END(OverlayCatalogueEntry)

// overlayType::color of a town or hero without a player.
H2_ENUM_BEGIN(OverlayColor)
    OVERLAY_NO_COLOR = 6
H2_ENUM_END(OverlayColor)

H2_ENUM_BEGIN(OverlaySelection)
    // gSelectedOverlay, and PickOverlay's result, without an object.
    OVERLAY_NONE = -1
H2_ENUM_END(OverlaySelection)

// A class button's place on the tool panel.
struct ClassButtonPosition {
    i16 x;
    i16 y;
};

#pragma pack(push, 1)
class overlayManager : public baseManager {
public:
    i32 unused36;
    // overlay.icn: the footprint cells.
    icon* m_cellIcon;
    // objpalet.icn: the panel and the picker's boxes.
    icon* m_paletteIcon;
    // The selected class's catalogue entries in picker order (LoadClass).
    overlayType m_types[OVERLAY_TYPE_COUNT];
    i32 m_typeCount;
    // The selected object's extent in grid cells (MeasureOverlay).
    i32 m_width;
    i32 m_height;
    iconWidget* m_highlight;
    textWidget* m_className;
    border* m_classButtons[OVERLAY_CLASS_COUNT];
    // The picker window and its scroll track and knob.
    heroWindow* m_picker;
    iconWidget* m_pickerTrack;
    iconWidget* m_pickerKnob;
    // Main drew the selected object over the map view.
    b32 m_previewDrawn;

    overlayManager(void);
    virtual i32 Open(i32 priority) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    // Names the selected class and outlines its button.
    void ShowClass(b32 update);
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    // Outlines the cells of type's grid from (8 - width, 6 - height) on at
    // screen (left, top), each in its part's colour.
    void DrawFootprint(i32 left, i32 top, i32 width, i32 height, overlayType* type, b32 clip);
    // Draws the parts of type's grid from (8 - width, 6 - height) on at
    // screen (x, y); a town also draws its shadow and the ground under the
    // map cell (cellX, cellY) when given.
    void DrawOverlay(
        overlayType* type,
        i32 x,
        i32 y,
        b32 clip,
        i32 width,
        i32 height,
        b32 update,
        i32 cellX,
        i32 cellY
    );
    // Fills m_types with the class's entries, sorted; false when none.
    b32 LoadClass(i32 objectClass);
    void MeasureOverlay(overlayType* type);
    void DrawSelectedOverlay(void);
    // Selects the catalogue entry's class and the entry; false when no
    // class lists it.
    b32 SelectOverlay(i32 index);
    void DrawPicker(b32 update);
    // The picker; returns the chosen m_types index (OVERLAY_NONE: none).
    i32 PickOverlay(i32 objectClass);
    MessageDispatchResult PickerMain(struct tag_message& message);
    // Follows the pointer along the picker's track (until the button is
    // released, or one step for a track click).
    void DragPickerKnob(b32 trackClick, i32 mouseX, i32 mouseY);
};
#pragma pack(pop)
SIZE(overlayManager, 0x1e616);

// Whether cell (x, y) of an object's grid is set in grid.
b32 OverlayGridHas(u32* grid, i32 x, i32 y);
// Whether type fits with its grid's top-left on map cell (left, top); without
// overObjects, no part may go on a cell that has an object.
b32 CanPlaceOverlay(overlayType* type, i32 left, i32 top, b32 overObjects);
// Removes the objects whose parts type's parts would replace.
void RemoveReplacedObjects(overlayType* type, i32 left, i32 top);
// Places type with its grid's top-left on map cell (x, y), as a new
// placement link when newLink is set; false (with a status warning) when it
// does not fit or a map limit is reached.
b32 PlaceOverlay(overlayType* type, i32 x, i32 y, b32 newLink);
// Puts the resource marker type on map cell (x, y) (requireMine: only on a
// mine's entrance).
b32 PlaceResourceMarker(overlayType* type, i32 x, i32 y, b32 requireMine);
// The picker window's handler: the tool manager's PickerMain.
MessageDispatchResult PickerHandler(struct tag_message& message);

// The selected m_types entry (OVERLAY_NONE: none).
extern i32 gSelectedOverlay;

#endif
