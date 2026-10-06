#include <BASE/Raster.h>
#include <BASE/bitmap.h>
#include <BASE/tileset.h>
#include <BASE/TILE.h>
#include <BASE/bmap2.h>
#include <BASE/resourceManager.h>
#include <BASE/heroWindowManager.h>
#include <SOURCE/dimPalette.h>

#include <array>
#include <bit>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>

namespace {
struct Rejected {};
std::vector<u8> member;
std::size_t memberAt = 0;
int allocations = 0;
int failures = 0;
void Expect(bool value, const char* message) {
    if (!value) {
        std::fprintf(stderr, "%s\n", message);
        ++failures;
    }
}
u16 Word(const u8* bytes) {
    return static_cast<u16>(bytes[0] | (static_cast<u16>(bytes[1]) << 8));
}
u32 Dword(const u8* bytes) {
    return static_cast<u32>(Word(bytes)) | (static_cast<u32>(Word(bytes + 2)) << 16);
}
void Record(u16 first, u16 width, u16 height, std::size_t payload) {
    member = {static_cast<u8>(first), static_cast<u8>(first >> 8),
              static_cast<u8>(width), static_cast<u8>(width >> 8),
              static_cast<u8>(height), static_cast<u8>(height >> 8)};
    for (std::size_t index = 0; index < payload; ++index)
        member.push_back(static_cast<u8>(index + 1));
    allocations = 0;
}
struct Surface {
    std::vector<u8> storage;
    bitmap image;
    Surface(i16 width, i16 height) : storage(static_cast<std::size_t>(width) * height + 2, 0xaa) {
        image.m_width = width;
        image.m_height = height;
        image.m_pixels = storage.data() + 1;
    }
    ~Surface() { image.m_pixels = nullptr; }
    void Guards() const {
        Expect(storage.front() == 0xaa && storage.back() == 0xaa, "raster destination guard changed");
    }
    bool Untouched() const {
        return std::all_of(storage.begin(), storage.end(), [](u8 value) { return value == 0xaa; });
    }
};
}

// Only the selected-member byte source and host services are substituted.
// The test links the production BMP/TIL constructors, allocators' callers,
// bitmap copy methods, tile blitter, and raster operations.
baseManager::baseManager() : m_next(nullptr), m_prev(nullptr), m_messageMask(BASE_MANAGER_MESSAGE_MASK_ALL),
                            m_priority(0), m_name{}, m_active(false) {}
resourceManager::resourceManager() = default;
i32 resourceManager::Open(i32) { return 0; }
void resourceManager::Close() {}
MessageDispatchResult resourceManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
void resourceManager::PointToFile(u32l) { memberAt = 0; }
u32l resourceManager::GetFileSize(u32l) { return static_cast<u32l>(member.size()); }
void resourceManager::ReadBlock(void* destination, u32l bytes) {
    if (bytes > member.size() - memberAt)
        throw Rejected{};
    std::memcpy(destination, member.data() + memberAt, bytes);
    memberAt += bytes;
}
i16 resourceManager::ReadWord() {
    std::array<u8, 2> bytes{};
    ReadBlock(bytes.data(), bytes.size());
    return std::bit_cast<i16>(Word(bytes.data()));
}
resourceManager manager;
resourceManager* gpResourceManager = &manager;
heroWindowManager::heroWindowManager() : m_screen(nullptr) {}
i32 heroWindowManager::Open(i32) { return 0; }
void heroWindowManager::Close() {}
MessageDispatchResult heroWindowManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
heroWindowManager windows;
heroWindowManager* gpWindowManager = &windows;
extern "C" void PollSound() {}
void ShutDown(const char*) { throw Rejected{}; }
void* BaseAlloc(u32 bytes, const char*, i32) {
    ++allocations;
    return std::malloc(bytes);
}
void BaseFree(void* memory, const char*, i32) { std::free(memory); }
u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];

namespace {
void ConstructorBounds() {
    u32 bytes = 0;
    Expect(images::RasterPayloadSize(432, 32, 32, 442368, bytes) && bytes == 442368,
           "retail tile payload size rejected");
    Expect(!images::RasterPayloadSize(432, 32, 32, 442367, bytes), "short tile payload accepted");
    Expect(!images::RasterPayloadSize(65535, 65535, 65535, 0xffffffffu, bytes), "tile product overflow");
    Expect(!images::RasterPayloadSize(0xffffffffu, 0x7fffffff, 0x7fffffff,
                                      std::numeric_limits<u64>::max(), bytes), "wide product overflow");
    for (std::size_t length = 0; length < 6; ++length) {
        member.assign(length, 0);
        try {
            bitmap bad(u32l{1});
            Expect(false, "truncated BMP header accepted");
        } catch (const Rejected&) {}
        try {
            tileset bad(1);
            Expect(false, "truncated TIL header accepted");
        } catch (const Rejected&) {}
    }
    for (const auto dimensions : {std::array<u16, 3>{0, 2, 2}, {1, 0, 2}, {1, 2, 0},
                                  {65535, 65535, 65535}, {1, 2, 3}}) {
        Record(dimensions[0], dimensions[1], dimensions[2], 5);
        try {
            tileset bad(1);
            Expect(false, "malformed TIL constructor succeeded");
        } catch (const Rejected&) {
            Expect(allocations == 0, "malformed TIL allocated before checking size");
        }
    }
    for (const auto dimensions : {std::array<u16, 3>{34, 2, 2}, {33, 0, 2}, {33, 2, 0},
                                  {33, 0xffff, 2}, {33, 2, 0xffff}, {33, 2, 3}}) {
        Record(dimensions[0], dimensions[1], dimensions[2], 5);
        try {
            bitmap bad(u32l{1});
            Expect(false, "malformed BMP constructor succeeded");
        } catch (const Rejected&) {}
    }
    Record(33, 2, 3, 6);
    bitmap valid(u32l{1});
    Expect(valid.m_width == 2 && valid.m_height == 3 && memberAt == 12
           && std::memcmp(valid.m_pixels, member.data() + 6, 6) == 0,
           "production BMP constructor changed valid pixels");
    try {
        bitmap negative(BITMAP_TYPE_NONE, -1, 4);
        Expect(false, "negative memory bitmap accepted");
    } catch (const Rejected&) {}
    bitmap empty(BITMAP_TYPE_NONE, 4, 0);
    Expect(empty.m_pixels == nullptr, "empty memory bitmap allocated pixels");
}

void CopiesAndFills() {
    Surface source(4, 3), target(5, 4);
    for (u8 index = 0; index < 12; ++index)
        source.image.m_pixels[index] = static_cast<u8>(index + 1);
    source.image.CopyTo(&target.image, -1, 1, 0, 0, 4, 3);
    Expect(target.image.m_pixels[5] == 2 && target.image.m_pixels[7] == 4
           && target.image.m_pixels[15] == 10 && target.image.m_pixels[17] == 12,
           "CopyTo did not use actual strides and clipped source position");
    target.Guards();
    Surface screen(4, 3);
    windows.m_screen = &screen.image;
    source.image.DrawToBufferCareful(-1, -1);
    Expect(screen.image.m_pixels[0] == 6 && screen.image.m_pixels[2] == 8
           && screen.image.m_pixels[4] == 10 && screen.image.m_pixels[6] == 12,
           "careful drawing cropped the wrong source coordinates");
    windows.m_screen = nullptr;
    screen.Guards();
    Surface grab(4, 3);
    grab.image.GrabBitmapCareful(&source.image, -1, -1);
    Expect(grab.image.m_pixels[5] == 1 && grab.image.m_pixels[7] == 3
           && grab.image.m_pixels[9] == 5 && grab.image.m_pixels[0] == 0xaa,
           "negative source coordinates shifted grabbed pixels incorrectly");
    grab.Guards();
    Surface overlap(4, 4);
    for (u8 index = 0; index < 16; ++index)
        overlap.image.m_pixels[index] = index;
    overlap.image.CopyToCareful(&overlap.image, 1, 1, 0, 0, 3, 3);
    Expect(overlap.image.m_pixels[5] == 0 && overlap.image.m_pixels[9] == 4
           && overlap.image.m_pixels[13] == 8 && overlap.image.m_pixels[15] == 10,
           "overlapping rows were copied after being overwritten");
    overlap.Guards();
    Surface alias(4, 4);
    for (u8 index = 0; index < 16; ++index)
        alias.image.m_pixels[index] = index;
    bitmap otherStride;
    otherStride.m_width = 2;
    otherStride.m_height = 8;
    otherStride.m_pixels = alias.image.m_pixels;
    BlitBitmap(&alias.image, 1, 0, 2, 4, &otherStride, 0, 1);
    otherStride.m_pixels = nullptr;
    Expect(alias.image.m_pixels[2] == 1 && alias.image.m_pixels[4] == 5
           && alias.image.m_pixels[6] == 9 && alias.image.m_pixels[8] == 13,
           "shared pixels with different strides were overwritten during copying");
    alias.Guards();
    Surface fill(4, 3);
    FillBitmapArea(&fill.image, -1, -1, 3, 3, 7);
    Expect(std::count(fill.storage.begin(), fill.storage.end(), 7) == 4, "fill did not crop to surface");
    uDimPal[2][3][7] = 21;
    DimBitmapArea(&fill.image, -1, -1, 3, 3, 11);
    Expect(std::count(fill.storage.begin(), fill.storage.end(), 21) == 4, "dim table 11 or bounds wrong");
    const auto before = fill.storage;
    DimBitmapArea(&fill.image, 0, 0, 4, 3, 12);
    FillBitmapAreaClip(&fill.image, 3, 0, 1, 3, 9, 0, 0, 4, 3);
    Expect(fill.storage == before, "invalid dim level or retail fill-edge guard changed pixels");
    for (i32 edge : {std::numeric_limits<i32>::min(), std::numeric_limits<i32>::max()}) {
        FillBitmapArea(&fill.image, edge, edge, 5, 5, 1);
        DimBitmapArea(&fill.image, edge, edge, 5, 5, 0);
        BlitBitmap(&source.image, edge, edge, 5, 5, &fill.image, 0, 0);
        BlitBitmap(&source.image, 0, 0, 5, 5, &fill.image, edge, edge);
    }
    Expect(fill.storage == before, "extreme raster coordinates wrote pixels");
    fill.Guards();
}

void TileBounds() {
    Record(2, 3, 2, 12);
    tileset tiles(1);
    Expect(tiles.m_dataSize == 12 && memberAt == 18, "TIL constructor retained wrong payload size");
    for (u32 flags : {0u, static_cast<u32>(TILE_FLIP_HORIZONTAL), static_cast<u32>(TILE_FLIP_VERTICAL),
                      static_cast<u32>(TILE_FLIP_HORIZONTAL | TILE_FLIP_VERTICAL)}) {
        Surface output(3, 2);
        TileToBitmap(&tiles, flags | 1, &output.image, -1, 0);
        for (int y = 0; y < 2; ++y)
            for (int x = 0; x < 2; ++x) {
                const int sy = (flags & TILE_FLIP_VERTICAL) != 0 ? 1 - y : y;
                const int sx = (flags & TILE_FLIP_HORIZONTAL) != 0 ? 1 - x : x + 1;
                Expect(output.image.m_pixels[y * 3 + x] == 7 + sy * 3 + sx,
                       "clipped flipped tile selected the wrong pixel");
            }
        output.Guards();
    }
    Surface rejected(3, 2);
    TileToBitmap(&tiles, 2, &rejected.image, 0, 0);
    TileToBitmap(&tiles, 0, &rejected.image, std::numeric_limits<i32>::min(), 0);
    TileToBitmap(&tiles, 0, &rejected.image, 0, std::numeric_limits<i32>::max());
    tiles.m_dataSize = 5;
    TileToBitmap(&tiles, 0, &rejected.image, 0, 0);
    Expect(rejected.Untouched(), "invalid tile index, span or position wrote pixels");
}

void RetailArchive(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    const std::vector<u8> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    if (bytes.size() < 2) {
        Expect(false, "cannot read retail archive");
        return;
    }
    const u16 count = Word(bytes.data());
    if (count > (bytes.size() - 2) / 12 || count > bytes.size() / 15) {
        Expect(false, "invalid retail archive directory");
        return;
    }
    std::size_t bitmaps = 0, tileCount = 0;
    for (u16 index = 0; index < count; ++index) {
        const auto nameAt = bytes.size() - count * 15 + index * 15;
        const char* name = reinterpret_cast<const char*>(bytes.data() + nameAt);
        const std::string label(name, std::find(name, name + 15, '\0'));
        const bool bitmapFile = label.ends_with(".BMP"), tileFile = label.ends_with(".TIL");
        if (!bitmapFile && !tileFile)
            continue;
        const u32 offset = Dword(bytes.data() + 2 + index * 12 + 4);
        const u32 size = Dword(bytes.data() + 2 + index * 12 + 8);
        if (offset > bytes.size() || size > bytes.size() - offset) {
            Expect(false, "invalid retail raster member extent");
            continue;
        }
        member.assign(bytes.begin() + offset, bytes.begin() + offset + size);
        try {
            if (bitmapFile) {
                bitmap image(u32l{1});
                Expect(std::memcmp(image.m_pixels, member.data() + 6,
                                   static_cast<std::size_t>(image.m_width) * image.m_height) == 0,
                       "retail bitmap payload changed");
                ++bitmaps;
            } else {
                tileset tiles(1);
                for (u32 tile = 0; tile < tiles.m_tileCount; ++tile) {
                    const u32 area = static_cast<u32>(tiles.m_tileWidth) * tiles.m_tileHeight;
                    for (u32 mode = 0; mode < 4; ++mode) {
                        const u32 flags = tile | ((mode & 1) != 0 ? TILE_FLIP_HORIZONTAL : 0)
                                               | ((mode & 2) != 0 ? TILE_FLIP_VERTICAL : 0);
                        Surface output(static_cast<i16>(tiles.m_tileWidth), static_cast<i16>(tiles.m_tileHeight));
                        TileToBitmap(&tiles, flags, &output.image, 0, 0);
                        for (u32 y = 0; y < tiles.m_tileHeight; ++y)
                            for (u32 x = 0; x < tiles.m_tileWidth; ++x) {
                                const u32 sy = (mode & 2) != 0 ? tiles.m_tileHeight - 1 - y : y;
                                const u32 sx = (mode & 1) != 0 ? tiles.m_tileWidth - 1 - x : x;
                                Expect(output.image.m_pixels[y * tiles.m_tileWidth + x]
                                       == member[6 + tile * area + sy * tiles.m_tileWidth + sx],
                                       "retail tile pixels differ");
                            }
                        output.Guards();
                    }
                    ++tileCount;
                }
            }
        } catch (const Rejected&) {
            std::fprintf(stderr, "retail raster rejected: %s\n", label.c_str());
            ++failures;
        }
    }
    std::printf("%s: %zu bitmap records, %zu tiles in four orientations\n", filename, bitmaps, tileCount);
}
}

int main(int argc, char** argv) {
    ConstructorBounds();
    CopiesAndFills();
    TileBounds();
    for (int index = 1; index < argc; ++index)
        RetailArchive(argv[index]);
    return failures == 0 ? 0 : 1;
}
