// Compare incremental combat redraws with a fresh full render. No game assets
// are needed: the rows exercise each ICN run type, including shadow/recolour.
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/dimPalette.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

namespace {
constexpr int Width = 640;
constexpr int Height = 480;
constexpr int SpriteX = 100;
constexpr int SpriteY = 100;
constexpr int SpriteWidth = 12;
constexpr int SpriteHeight = 4;

// A single 13-byte frame header followed by solid, shadow, recolour and
// literal rows. The shadow palette intentionally changes on repeated draws.
const u8 IconBody[] = {
    0, 0, 0, 0, SpriteWidth, 0, SpriteHeight, 0, 0, 13, 0, 0, 0,
    0xcc, 90, 0,
    0xc0, 0x40, SpriteWidth, 0,
    0xc0, 0x80, SpriteWidth, 0,
    SpriteWidth, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 0,
    0x80
};
}

// Resource/manager scaffolding only; ICON.cpp and the production blitters are
// linked unchanged, including the icon loader and CombatClipDrawToBuffer.
u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];
b32 gbComputeExtent = true;
b32 gbSaveBiggestExtent = false;
b32 gbReturnAfterComputeExtent = false;
b32 gbLimitToExtent = false;
b32 gbCurrArmyDrawn = true;
i32 giMinExtentX = 0, giMinExtentY = 0, giMaxExtentX = Width - 1, giMaxExtentY = 443;

resource::resource(ResourceCategory category, u32l id, i16 count, resource* next)
    : m_resourceType(category), m_refCount(count), m_id(id), m_next(next) {}
resource::~resource() = default;
bitmap::bitmap()
    : resource(RESOURCE_CATEGORY_BITMAP, 0, RESOURCE_REFERENCE_UNMANAGED, nullptr),
      m_bitmapType(BITMAP_TYPE_MEMORY), m_width(Width), m_height(Height), m_pixels(nullptr) {}
bitmap::~bitmap() = default;
baseManager::baseManager() = default;
heroWindowManager::heroWindowManager() : m_screen(nullptr) {}
i32 heroWindowManager::Open(i32) { return 0; }
void heroWindowManager::Close() {}
MessageDispatchResult heroWindowManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
resourceManager::resourceManager() = default;
i32 resourceManager::Open(i32) { return 0; }
void resourceManager::Close() {}
MessageDispatchResult resourceManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
void resourceManager::PointToFile(u32l) {}
i16 resourceManager::ReadWord() { return 1; }
i32l resourceManager::ReadLong() { return sizeof(IconBody); }
void resourceManager::ReadBlock(void* dest, u32l size) {
    if (size != sizeof(IconBody))
        std::abort();
    std::memcpy(dest, IconBody, size);
}
void* BaseAlloc(u32 size, const char*, i32) { return std::malloc(size); }
void BaseFree(void* ptr, const char*, i32) { std::free(ptr); }

namespace {
heroWindowManager WindowManager;
resourceManager ResourceManager;
}
heroWindowManager* gpWindowManager = &WindowManager;
resourceManager* gpResourceManager = &ResourceManager;

int main() {
    for (auto& set : uDimPal)
        for (auto& level : set)
            for (int i = 0; i < 256; ++i)
                level[i] = static_cast<u8>(i / 2);
    std::array<u8, 256> remap{};
    for (int i = 0; i < 256; ++i)
        remap[i] = static_cast<u8>(255 - i);
    std::array<i8, Height> shear{};
    shear.fill(2);

    const std::unique_ptr<resource> spriteOwner(new icon(0));
    auto& sprite = *static_cast<icon*>(spriteOwner.get());
    bitmap screen;
    WindowManager.m_screen = &screen;
    const std::vector<u8> background(Width * Height, 200);
    int failures = 0;
    for (const auto orientation : {ICON_DRAW_NORMAL, ICON_DRAW_FLIPPED}) {
        const int anchor = orientation == ICON_DRAW_NORMAL ? SpriteX : SpriteX + SpriteWidth - 1;
        for (int effect = 0; effect < 3; ++effect) {
            auto* colorTable = effect == 1 ? remap.data() : nullptr;
            auto* yModify = effect == 2 ? shear.data() : nullptr;
            auto expected = background;
            screen.m_pixels = expected.data();
            gbLimitToExtent = false;
            SLimitData limits{};
            sprite.CombatClipDrawToBuffer(anchor, SpriteY, 0, &limits, orientation,
                                         77, colorTable, yModify);

            // Restore only the dirty rectangle, as DrawFrame does, then redraw
            // an intersecting stationary sprite. Include cuts at either edge,
            // both edges, and a vertical slice of the sprite.
            for (const SLimitData dirty : {
                     SLimitData{SpriteX + 4, SpriteX + 20, SpriteY, SpriteY + 3},
                     SLimitData{SpriteX - 4, SpriteX + 6, SpriteY, SpriteY + 3},
                     SLimitData{SpriteX + 4, SpriteX + 6, SpriteY, SpriteY + 3},
                     SLimitData{SpriteX - 4, SpriteX + 20, SpriteY + 1, SpriteY + 1}}) {
                auto actual = expected;
                screen.m_pixels = actual.data();
                gbLimitToExtent = true;
                giMinExtentX = dirty.left;
                giMaxExtentX = dirty.right;
                giMinExtentY = dirty.top;
                giMaxExtentY = dirty.bottom;
                for (int repeat = 0; repeat < 3; ++repeat) {
                    for (int row = dirty.top; row <= dirty.bottom; ++row)
                        std::copy(background.begin() + row * Width + dirty.left,
                                  background.begin() + row * Width + dirty.right + 1,
                                  actual.begin() + row * Width + dirty.left);
                    sprite.CombatClipDrawToBuffer(anchor, SpriteY, 0, &limits, orientation,
                                                 77, colorTable, yModify);
                }
                if (actual != expected) {
                    const auto mismatch = std::mismatch(actual.begin(), actual.end(), expected.begin());
                    const auto index = mismatch.first - actual.begin();
                    std::fprintf(stderr,
                        "partial redraw differs: flipped=%d effect=%d clip=(%d,%d)-(%d,%d) "
                        "pixel=(%td,%td) actual=%u expected=%u\n",
                        orientation == ICON_DRAW_FLIPPED, effect, dirty.left, dirty.top,
                        dirty.right, dirty.bottom, index % Width, index / Width,
                        *mismatch.first, *mismatch.second);
                    ++failures;
                }
            }

            // PowEffect retains its dirty bounds between animation frames,
            // and DrawFrame expands each edge by one pixel on each redraw.
            // A clipped run must not become a moving rectangular cutout on
            // this stationary neighbour as those bounds grow past it.
            auto growing = expected;
            screen.m_pixels = growing.data();
            gbLimitToExtent = true;
            for (int frame = 0; frame < 9; ++frame) {
                giMinExtentX = SpriteX + 5 - frame;
                giMaxExtentX = SpriteX + 6 + frame;
                giMinExtentY = SpriteY + 1 - frame;
                giMaxExtentY = SpriteY + 2 + frame;
                for (int row = giMinExtentY; row <= giMaxExtentY; ++row)
                    std::copy(background.begin() + row * Width + giMinExtentX,
                              background.begin() + row * Width + giMaxExtentX + 1,
                              growing.begin() + row * Width + giMinExtentX);
                sprite.CombatClipDrawToBuffer(anchor, SpriteY, 0, &limits, orientation,
                                             77, colorTable, yModify);
                if (growing != expected) {
                    std::fprintf(stderr,
                        "expanding redraw differs: flipped=%d effect=%d frame=%d\n",
                        orientation == ICON_DRAW_FLIPPED, effect, frame);
                    ++failures;
                }
            }
        }
    }
    return failures == 0 ? 0 : 1;
}
