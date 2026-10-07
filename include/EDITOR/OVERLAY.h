#ifndef HOMM2_EDITOR_OVERLAY_H
#define HOMM2_EDITOR_OVERLAY_H


#include <Ints.h>
#include <Ints.h>
#include <BASE/baseManager.h>

class border;
class heroWindow;
class icon;
class iconWidget;
class textWidget;
struct tag_message;

typedef enum OverlayGridConstant {


    OVERLAY_GRID_WIDTH      = 8,
    OVERLAY_GRID_HEIGHT     = 6,
    OVERLAY_GRID_CELLS      = OVERLAY_GRID_WIDTH * OVERLAY_GRID_HEIGHT,


    OVERLAY_GRID_WORDS      = 2,
    OVERLAY_GRID_TOP        = 0,
    OVERLAY_GRID_BOTTOM     = 1,
    OVERLAY_GRID_WORD_BITS  = 32,


    OVERLAY_ANCHOR_X        = OVERLAY_GRID_WIDTH - 1,
    OVERLAY_ANCHOR_Y        = OVERLAY_GRID_HEIGHT - 1,

    OVERLAY_GRID_ANCHOR     = OVERLAY_GRID_CELLS - 1,

    OVERLAY_NO_FRAME        = 0xff,

    OVERLAY_TYPE_COUNT      = 956
} OverlayGridConstant;


typedef u32 OverlayGrid[OVERLAY_GRID_WORDS];


typedef enum OverlayCategory {
    OVERLAY_CATEGORY_TERRAIN  = 0,
    OVERLAY_CATEGORY_TREASURE = 1,
    OVERLAY_CATEGORY_MONSTER  = 2,
    OVERLAY_CATEGORY_ARTIFACT = 3,
    OVERLAY_CATEGORY_TOWN     = 5,


    OVERLAY_CATEGORY_PART     = 6,
    OVERLAY_CATEGORY_HERO     = 7
} OverlayCategory;


typedef enum OverlayTypeFlag {

    OVERLAY_FLAG_MINE_MARKER    = 1,

    OVERLAY_FLAG_SHOWS_RESOURCE = 2
} OverlayTypeFlag;


typedef enum OverlayFrameNumbering {

    OVERLAY_FRAMES_OWN      = 0,

    OVERLAY_FRAMES_SHARED   = 1111,
    OVERLAY_FRAMES_CONTINUE = 9999
} OverlayFrameNumbering;

#pragma pack(push, 1)


struct overlayType {

    i32 id;


    i32 group;

    i32 ordinal;

    i8 tileset;

    i8 category;

    u16 frequency;

    u8 animationFrames;

    OverlayGrid occupiedRows;


    u32 terrainMask;

    u32 groundMask;

    OverlayGrid overlayRows;

    OverlayGrid shadowRows;

    OverlayGrid animatedRows;

    OverlayGrid resourceRows;


    u8 color;

    u8 flags;

    OverlayGrid entranceRows;

    u8 highLayer;

    u8 trigger;


    u8 width;

    i32 frameNumbering;


    u8 frames[OVERLAY_GRID_CELLS];
};
#pragma pack(pop)

typedef enum OverlayClassConstant {


    OVERLAY_CLASS_COUNT       = 14,
    OVERLAY_CLASS_TERRAINS    = 9,

    OVERLAY_CLASS_ANY_TERRAIN = 0xfff
} OverlayClassConstant;

typedef enum OverlayCatalogueEntry {


    OVERLAY_MINES_WATER       = 4,
    OVERLAY_MINES_WASTELAND   = 17,
    OVERLAY_MINES_GRASS       = 28,
    OVERLAY_MINES_SNOW        = 39,
    OVERLAY_MINES_SWAMP       = 50,
    OVERLAY_MINES_LAVA        = 61,
    OVERLAY_MINES_DESERT      = 72,
    OVERLAY_MINES_DIRT        = 85,

    OVERLAY_RESOURCE_MARKERS  = 128,

    OVERLAY_TOWN_FLAGS        = 134,
    OVERLAY_TOWN_FLAG_PARTS   = 2,

    OVERLAY_RANDOM_MONSTER             = 214,
    OVERLAY_RANDOM_MONSTER_WEAK        = 215,
    OVERLAY_RANDOM_MONSTER_MEDIUM      = 216,
    OVERLAY_RANDOM_MONSTER_STRONG      = 217,
    OVERLAY_RANDOM_MONSTER_VERY_STRONG = 218,

    OVERLAY_RANDOM_TREASURE_ARTIFACT   = 337,
    OVERLAY_RANDOM_MINOR_ARTIFACT      = 338,
    OVERLAY_RANDOM_MAJOR_ARTIFACT      = 339,


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

    OVERLAY_ANCIENT_LAMP         = 832,
    OVERLAY_RANDOM_RESOURCE      = 833,
    OVERLAY_TREASURE_CHEST       = 834,


    OVERLAY_TOWN_FIRST        = 835,
    OVERLAY_TOWN_LAST         = 918,
    OVERLAY_TOWN_SHADOWS      = 919,
    OVERLAY_TOWN_VARIANTS     = 12,

    OVERLAY_TOWN_KINDS        = 2,


    OVERLAY_TOWN_GROUNDS      = 930,
    OVERLAY_TOWN_GRASS_GROUND = 931,


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

    OVERLAY_TOWN_OUTLINE      = 955
} OverlayCatalogueEntry;


typedef enum OverlayColor {
    OVERLAY_NO_COLOR = 6
} OverlayColor;

typedef enum OverlaySelection {

    OVERLAY_NONE = -1
} OverlaySelection;


struct ClassButtonPosition {
    i16 x;
    i16 y;
};

#pragma pack(push, 1)
class overlayManager : public baseManager {
public:
    i32 unused36;

    icon* m_cellIcon;

    icon* m_paletteIcon;

    overlayType m_types[OVERLAY_TYPE_COUNT];
    i32 m_typeCount;

    i32 m_width;
    i32 m_height;
    iconWidget* m_highlight;
    textWidget* m_className;
    border* m_classButtons[OVERLAY_CLASS_COUNT];

    heroWindow* m_picker;
    iconWidget* m_pickerTrack;
    iconWidget* m_pickerKnob;

    b32 m_previewDrawn;

    overlayManager(void);
    virtual i32 Open(i32 priority) override;
    virtual void Close(void) override;

    void ShowClass(b32 update);
    virtual MessageDispatchResult Main(struct tag_message& message) override;


    void DrawFootprint(i32 left, i32 top, i32 width, i32 height, overlayType* type, b32 clip);


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

    b32 LoadClass(i32 objectClass);
    void MeasureOverlay(overlayType* type);
    void DrawSelectedOverlay(void);


    b32 SelectOverlay(i32 index);
    void DrawPicker(b32 update);

    i32 PickOverlay(i32 objectClass);
    MessageDispatchResult PickerMain(struct tag_message& message);


    void DragPickerKnob(b32 trackClick, i32 mouseX, i32 mouseY);
};
#pragma pack(pop)


b32 OverlayGridHas(u32* grid, i32 x, i32 y);


b32 CanPlaceOverlay(overlayType* type, i32 left, i32 top, b32 overObjects);

void RemoveReplacedObjects(overlayType* type, i32 left, i32 top);


b32 PlaceOverlay(overlayType* type, i32 x, i32 y, b32 newLink);


b32 PlaceResourceMarker(overlayType* type, i32 x, i32 y, b32 requireMine);

MessageDispatchResult PickerHandler(struct tag_message& message);


extern i32 gSelectedOverlay;

#endif
