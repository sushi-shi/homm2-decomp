#include <SOURCE/KB.h>
#include <BASE/message.h>
#include <EDITOR/mapcell.h>
#include <stdio.h>

i32 __cdecl main() {
    for (i32 value = 0; value < 256; ++value) {
        const char input = static_cast<char>(value);
        const i32 upper = ((value >= 'a' && value <= 'z') || value >= 0xe0)
            ? value - 32 : (value == 0xb8 ? 0xa8 : value);
        const i32 lower = ((value >= 'A' && value <= 'Z') || (value >= 0xc0 && value <= 0xdf))
            ? value + 32 : (value == 0xa8 ? 0xb8 : value);
        if (static_cast<u8>(CyrillicToUpper(input)) != upper
            || static_cast<u8>(CyrillicToLower(input)) != lower) {
            printf("casefold mismatch at %d\n", value);
            return 1;
        }
        const i32 type = value - 128;
        if (IS_ELEMENTAL_CREATURE(type) != (type >= 62 && type <= 65)
            || IS_GOLEM_CREATURE(type) != (type == 40 || type == 41)
            || IS_DRAGON_CREATURE(type) != (type == 35 || type == 36 || type == 37 || type == 56)
            || IS_LICH_CREATURE(type) != (type == 54 || type == 55)
            || IS_VAMPIRE_CREATURE(type) != (type == 52 || type == 53)
            || IS_TROLL_CREATURE(type) != (type == 17 || type == 18)
            || IS_WIDGET_SELECTION_COMMAND(type) != (type == 12 || type == 14)) {
            printf("classification mismatch at %d\n", type);
            return 2;
        }
    }
    printf("CP1251: all 256 uppercase and 256 lowercase inputs pass\n");
    printf("Classification: all 256 signed-byte inputs for six creature sets and selection pass\n");
    mapCell cell;
    for (i32 tileset = 0; tileset < 64; ++tileset) {
        for (i32 flags = 0; flags < 256; ++flags) {
            for (i32 empty = 0; empty < 2; ++empty) {
                cell.m_objectIndex = empty ? 255 : 0;
                cell.m_objectTileset = static_cast<TilesetId>(tileset);
                cell.m_flags = static_cast<u8>(flags);
                const bool expected = !empty && tileset != 47 && (flags & 128) == 0;
                if (CELL_HAS_NON_SHADOW_OBJECT(&cell) != expected) {
                    printf("sprite mismatch: tileset %d flags %d empty %d\n", tileset, flags, empty);
                    return 3;
                }
            }
        }
    }
    printf("Sprite predicate: all 32768 tileset/flag/sentinel combinations pass\n");
    return 0;
}
