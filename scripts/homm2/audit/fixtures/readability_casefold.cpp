#include <SOURCE/KB.h>
#include <BASE/message.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <BASE/Misc.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

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
    playerData player;
    for (i32 bits = 0; bits < 256; ++bits) {
        player.m_barrierTents = static_cast<i8>(bits);
        for (i32 color = 0; color < 8; ++color) {
            if (PLAYER_HAS_VISITED_TENT(player, color) != (bits & (1 << color))) {
                printf("tent mask mismatch: bits %d color %d\n", bits, color);
                return 4;
            }
        }
    }
    for (i32 number = -1000; number <= 1000; ++number) {
        const bool interior = number >= 0 && number <= 116 && number % 13 >= 1 && number % 13 <= 11;
        const i32 nextLevel = number < 4 ? number + 1 : 5;
        if (IS_INTERIOR_COMBAT_HEX(number) != interior || NEXT_MAGE_GUILD_LEVEL(number) != nextLevel) {
            printf("hex/level mismatch: %d\n", number);
            return 5;
        }
    }
    for (i32 dx = -16; dx <= 16; ++dx) {
        for (i32 dy = -16; dy <= 16; ++dy) {
            const i32 manhattan = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            i32 euclidean = 0;
            while ((euclidean + 1) * (euclidean + 1) <= dx * dx + dy * dy)
                ++euclidean;
            if (MANHATTAN_LENGTH(dx, dy) != manhattan || INTEGER_VECTOR_LENGTH(dx, dy) != euclidean) {
                printf("distance mismatch: %d, %d\n", dx, dy);
                return 6;
            }
        }
    }
    printf("Domain formulas: 2048 tent masks, 2001 hex/level inputs and 1089 delta pairs pass\n");
    i32 descriptors[2];
    if (_pipe(descriptors, 64, _O_BINARY) != 0)
        return 7;
    i16 values[2] = {-12345, 0};
    i32 writeIndex = 0;
    i32 readIndex = 1;
    i32 fdEvaluations = 0;
    if (WRITE_FILE_VALUE((++fdEvaluations, descriptors[1]), values[writeIndex++]) != sizeof(i16)
        || fdEvaluations != 1 || writeIndex != 1
        || READ_FILE_VALUE((++fdEvaluations, descriptors[0]), values[readIndex++]) != sizeof(i16)
        || fdEvaluations != 2 || readIndex != 2 || values[0] != values[1])
        return 8;
    indexArray writtenRecord = {0xabcd, 0x2345};
    indexArray readRecord = {0, 0};
    if (WRITE_FILE_VALUE(descriptors[1], writtenRecord) != sizeof(writtenRecord)
        || READ_FILE_VALUE(descriptors[0], readRecord) != sizeof(readRecord)
        || readRecord.key != writtenRecord.key || readRecord.value != writtenRecord.value)
        return 9;
    u8 lastByte = 0x11;
    if (WRITE_FILE_VALUE(descriptors[1], lastByte) != 1)
        return 10;
    close(descriptors[1]);
    i32 partialValue = 0x55667788;
    if (READ_FILE_VALUE(descriptors[0], partialValue) != 1 || partialValue != 0x55667711
        || READ_FILE_VALUE(descriptors[0], partialValue) != 0 || partialValue != 0x55667711)
        return 11;
    close(descriptors[0]);
    printf("File values: scalar/record sizes, single operand evaluation, short read and EOF pass\n");
    return 0;
}
