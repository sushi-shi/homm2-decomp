#ifndef HOMM2_EDITOR_EDITMGR_H
#define HOMM2_EDITOR_EDITMGR_H

// The editor manager's unit (src/EDITOR/EDITMGR.cpp, the 2.0 editor's
// EDITMGR.CPP): its free functions and data. The class itself is
// editManager (editManager.h).

#include <va.h>
#include <stdio.h>
#include <BASE/message.h>
#include <SOURCE/REQUEST.h>
#include <EDITOR/mapcell.h>

struct tag_message;

H2_ENUM_BEGIN(EditClearMask)
    // ClearArea's layer masks: everything, or what a road may cross.
    EDIT_CLEAR_ALL       = 0xffff,
    EDIT_CLEAR_ROAD_MASK = 0xfc7f
H2_ENUM_END(EditClearMask)

H2_ENUM_BEGIN(EditGroundShape)
    // giGroundShape: a terrain's plain tile, its border runs against water
    // (the edges and corners take the cell's flip flags), the second edge
    // runs and the decorated plain tiles.
    EDIT_SHAPE_PLAIN              = 0,
    EDIT_SHAPE_NORTH_EDGE         = 1,
    EDIT_SHAPE_NORTH_EAST_CORNER  = 2,
    EDIT_SHAPE_EAST_EDGE          = 3,
    EDIT_SHAPE_NORTH_EAST_INNER   = 4,
    // The coast borders (water or beach on that side), the borders where a
    // coast and another terrain meet, and an edge or corner with the coast
    // beyond its corner.
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
    // The shape without the varied-tile bit (GROUND_SHAPE_FLIPPED).
    EDIT_SHAPE_MASK               = 0x7f,
    // ChooseGroundTile's tile lists: plain and varied tiles, at most 20 of
    // each per terrain and shape.
    EDIT_GROUND_PLAIN             = 0,
    EDIT_GROUND_VARIED            = 1,
    EDIT_GROUND_VARIANTS          = 2,
    EDIT_GROUND_TILES_PER_SHAPE   = 20
H2_ENUM_END(EditGroundShape)

#pragma pack(push, 1)
// A town or capturable-site record of the map file.
struct EditMapRecord {
    u8 x;
    u8 y;
    u8 type;
};
#pragma pack(pop)

// The map text import's file: closed on every return.
class textFile {
public:
    FILE* m_file;

    textFile(void);
    ~textFile();
    operator FILE*(void);
};

// BlendShallowWater's count of a water cell's shaded neighbours by the coast
// corner (a flip state, 0-3) they face; `any` tests all four at once.
union EditCornerCounts {
    u8 corner[4];
    u32 any;
};

// The header of the edited map (its name, size and players).
extern SMapHeader gEditMapHeader;
// The map text export's file (set while it runs).
#define gTextFileName gTextFileNameBlockBuffer // spelling fixes .bss order
extern char* gTextFileName;
// Set while BlendTerrain may pick ground variants.
#define gVaryTiles gVaryTilesBase // spelling fixes .bss order
extern b32 gVaryTiles;
// Set when ClearArea erased a road or stream part (to redraw the lines).
#define gLinesRemoved gLinesRemovedLocal // spelling fixes .bss order
extern b32 gLinesRemoved;
// ClearArea's object filter: the tilesets whose objects it erases.
#define gClearTilesets gClearTilesetsInstance // spelling fixes .bss order
extern u8 gClearTilesets[TILESET_COUNT];

// Gives the cell a new tile of the terrain and shape unless it already has
// one.
void SetCellGround(i32 x, i32 y, i32 terrain, i32 shape);
// The map text export's line writers: ClearTextFile empties the .TXT,
// AppendTextLine adds a line and WriteTextHeader a blank line and an
// object's heading. The import reads a line without its newline
// (ReadTextLine) and checks a blank line and an object's heading
// (FindTextHeader).
void ClearTextFile(void);
void AppendTextLine(H2_CONST char* text);
void WriteTextHeader(i32 x, i32 y, H2_CONST char* kind);
void ReadTextLine(FILE* file, char* line);
bool FindTextHeader(FILE* file, i32 x, i32 y, H2_CONST char* kind);
// The map file requester: loads or saves (`mode`, a FileRequesterMode) and
// stores the chosen file name in gMapFileName.
b32 PickMap(i32 mode);
// A map code of the serial (a letter from 'V' and three base-26 letters).
char* MakeMapCode(i32 serial);
// Shows a warning on the status bar with a beep.
void ShowStatusWarning(char* text);
// The ground tile of a terrain and shape: the plain or a varied tile (vary),
// whose variant is rolled at (x, y) with the given chance (force: whenever
// the terrain has variants).
i32 ChooseGroundTile(i32 terrain, i32 shape, b32 vary, i32 x, i32 y, b32 force, float chance);
// Whether screen point (x, y) lies on the map view.
i32 InMapArea(i32 x, i32 y);
// Numbers the parts of every catalogue entry (gOverlayTypes) by the frames
// of its tileset.
void FillInOverlayTiles(void);
// Whether a cell's object (its trigger) keeps a map-extra record, and
// freeing one record (the later ones and their users move down).
b32 HasExtraObjectData(i32 triggerType);
// Whether a cell's object (its trigger type) has a detail editor: towns,
// signs, bottles, events, sphinxes, monsters, the ultimate artifact, heroes
// and jails.
b32 LocationHasSpecialDetails(i32 triggerType);
void DeleteExtraObjectData(u32 index);
// Counts the players who may play and gives each one its faction.
void CalculatePlayerNumbers(void);
// Marks the players whose towns or heroes the map holds
// (gEditMapHeader.playerEnabled), then recounts.
void ResetPlayerAvailability(void);
// The file menu (ecpanel.bin): the button chosen, or EDIT_FILE_MENU_NONE.
i32 FileOptions(void);
MessageDispatchResult FileOptionsHandler(struct tag_message& message);
// The system options dialog's toggles and handler.
void UpdateEditorSystemOptions(b32 initialDraw);
MessageDispatchResult EditorSystemOptionsHandler(struct tag_message& message);
// Whether the map needs the expansion (an event, sphinx, castle, hero,
// artifact or object only the expansion has): it then saves as .MX2.
b8 UsesExpansionObjects(void);

#endif
