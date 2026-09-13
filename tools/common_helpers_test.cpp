#include <BASE/IconRle.h>
#include <BASE/message.h>
#include <BASE/widget.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/KB_TYPES.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/searchArray.h>
#include <cstdlib>

static_assert(IS_ELEMENTAL_CREATURE(CREATURE_EARTH_ELEMENTAL));
static_assert(IS_ELEMENTAL_CREATURE(CREATURE_WATER_ELEMENTAL));
static_assert(!IS_ELEMENTAL_CREATURE(CREATURE_GHOST));
static_assert(IS_GOLEM_CREATURE(CREATURE_STEEL_GOLEM));
static_assert(IS_DRAGON_CREATURE(CREATURE_BONE_DRAGON));
static_assert(IS_LICH_CREATURE(CREATURE_POWER_LICH));
static_assert(IS_VAMPIRE_CREATURE(CREATURE_VAMPIRE_LORD));
static_assert(IS_TROLL_CREATURE(CREATURE_WAR_TROLL));
static_assert(!IS_LICH_CREATURE(CREATURE_SKELETON));
static_assert(ICON_FITS_CLIP(2, 3, 10, 11, 2, 3, 10, 11));
static_assert(!ICON_FITS_CLIP(2, 3, 11, 11, 2, 3, 10, 11));
static_assert(CURSOR_EMPTY_OBJECT_INDEX == MAPCELL_SPRITE_NONE);
static_assert(SEARCH_NO_OBJECT == MAPCELL_SPRITE_NONE);
static_assert(CURSOR_OBJECT_PASSABLE_FLAG == H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY));
static_assert(SEARCH_CELL_BLOCKED == H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY));

static void Check(bool condition) {
    if (!condition)
        std::abort();
}

int main() {
    tag_message message{};
    SET_WIDGET_MESSAGE(message, WIDGET_COMMAND_SET_TEXT, 37);
    Check(message.type == MESSAGE_WIDGET);
    Check(message.payload.widget.command == WIDGET_COMMAND_SET_TEXT);
    Check(message.payload.widget.id == 37);

    struct Geometry { i32 m_x, m_y, m_width, m_height; } geometry{};
    struct Words {
        i32 position = 0;
        i32 ReadWord() { return ++position; }
    } words;
    READ_WIDGET_GEOMETRY(geometry, &words);
    Check(geometry.m_x == 1 && geometry.m_y == 2);
    Check(geometry.m_width == 3 && geometry.m_height == 4 && words.position == 4);
    Check(WIDGET_CONTAINS_LOCAL_POINT(geometry, 1, 2));
    Check(WIDGET_CONTAINS_LOCAL_POINT(geometry, 3, 5));
    Check(!WIDGET_CONTAINS_LOCAL_POINT(geometry, 4, 5));
    Check(!WIDGET_CONTAINS_LOCAL_POINT(geometry, 3, 6));

    mapCell cell{};
    cell.m_objectIndex = 1;
    cell.SetObjectTileset(TILESET_OBJNTOWN);
    Check(CELL_HAS_NON_SHADOW_OBJECT(&cell));
    cell.m_flags = H2EnumIndex(MAP_CELL_OBJECT_SHADOW_ONLY);
    Check(!CELL_HAS_NON_SHADOW_OBJECT(&cell));
    cell.m_flags = 0;
    cell.SetObjectTileset(TILESET_DUMMY);
    Check(!CELL_HAS_NON_SHADOW_OBJECT(&cell));
    cell.SetObjectTileset(TILESET_OBJNTOWN);
    cell.m_objectIndex = MAPCELL_SPRITE_NONE;
    Check(!CELL_HAS_NON_SHADOW_OBJECT(&cell));
    return 0;
}
