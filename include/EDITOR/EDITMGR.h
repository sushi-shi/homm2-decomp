#ifndef HOMM2_EDITOR_EDITMGR_H
#define HOMM2_EDITOR_EDITMGR_H


#include <Ints.h>
#include <stdio.h>
#include <BASE/message.h>
#include <SOURCE/REQUEST.h>
#include <EDITOR/mapcell.h>
#include <EDITOR/OVERLAY.h>

typedef i32 FileRequesterMode;

typedef enum EditClearMask {

    EDIT_CLEAR_ALL       = 0xffff,
    EDIT_CLEAR_ROAD_MASK = 0xfc7f
} EditClearMask;

typedef enum EditGroundShape {


    EDIT_SHAPE_PLAIN              = 0,
    EDIT_SHAPE_NORTH_EDGE         = 1,
    EDIT_SHAPE_NORTH_EAST_CORNER  = 2,
    EDIT_SHAPE_EAST_EDGE          = 3,
    EDIT_SHAPE_NORTH_EAST_INNER   = 4,


    EDIT_SHAPE_SHORE_NORTH_EDGE   = 5,
    EDIT_SHAPE_SHORE_CORNER       = 6,
    EDIT_SHAPE_SHORE_EAST_EDGE    = 7,
    EDIT_SHAPE_SHORE_INNER        = 8,
    EDIT_SHAPE_CORNER_SHORE_FAR   = 10,
    EDIT_SHAPE_CORNER_SHORE_NEAR  = 11,
    EDIT_SHAPE_NORTH_EDGE_SHORE   = 12,
    EDIT_SHAPE_EAST_EDGE_SHORE    = 13,
    EDIT_SHAPE_SHORE_EDGE_BORDER  = 14,
    EDIT_SHAPE_SHORE_SIDE_BORDER  = 15,
    EDIT_SHAPE_NORTH_EDGE_ALT     = 16,
    EDIT_SHAPE_EAST_EDGE_ALT      = 17,
    EDIT_SHAPE_DECORATED_FIRST    = 18,
    EDIT_SHAPE_DECORATED_SECOND   = 19,
    EDIT_SHAPE_DECORATED_THIRD    = 20,
    EDIT_SHAPE_DECORATED_FOURTH   = 21,
    EDIT_SHAPE_COUNT              = 22,

    EDIT_SHAPE_MASK               = 0x7f,


    EDIT_GROUND_PLAIN             = 0,
    EDIT_GROUND_VARIED            = 1,
    EDIT_GROUND_VARIANTS          = 2,
    EDIT_GROUND_TILES_PER_SHAPE   = 20
} EditGroundShape;

#pragma pack(push, 1)

struct EditMapRecord {
    u8 x;
    u8 y;
    u8 type;
};
#pragma pack(pop)


class textFile {
public:
    FILE* m_file;

    textFile(void);
    ~textFile();
    operator FILE*(void);
};


union EditCornerCounts {
    u8 corner[4];
    u32 any;
};


extern overlayType gOverlayTypes[OVERLAY_TYPE_COUNT];
extern u8 gObjectClassCategories[OVERLAY_CLASS_COUNT];
extern u32 gObjectClassTerrains[OVERLAY_CLASS_COUNT];

extern SMapHeader gEditMapHeader;

extern char* gTextFileName;

extern b32 gVaryTiles;

extern b32 gLinesRemoved;

extern u8 gClearTilesets[TILESET_COUNT];


void SetCellGround(i32 x, i32 y, i32 terrain, i32 shape);


void ClearTextFile(void);
void AppendTextLine(const char* text);
void WriteTextHeader(i32 x, i32 y, const char* kind);
void ReadTextLine(FILE* file, char* line);
bool FindTextHeader(FILE* file, i32 x, i32 y, const char* kind);


b32 PickMap(FileRequesterMode mode);

char* MakeMapCode(i32 serial);

void ShowStatusWarning(const char* text);


i32 ChooseGroundTile(i32 terrain, i32 shape, b32 vary, i32 x, i32 y, b32 force, float chance);

i32 InMapArea(i32 x, i32 y);


void FillInOverlayTiles(void);


b32 LocationHasSpecialDetails(i32 triggerType);


b32 HasExtraObjectData(i32 triggerType);
void DeleteExtraObjectData(u32 index);

void CalculatePlayerNumbers(void);


void ResetPlayerAvailability(void);

i32 FileOptions(void);
MessageDispatchResult FileOptionsHandler(struct tag_message& message);

void UpdateEditorSystemOptions(b32 initialDraw);
MessageDispatchResult EditorSystemOptionsHandler(struct tag_message& message);


b8 UsesExpansionObjects(void);

#endif
