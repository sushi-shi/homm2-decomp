#include <BASE/bitmap.h>
#include <BASE/font.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/IconEntry.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
#include <SOURCE/dimPalette.h>
#include <SOURCE/Localization.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace {
constexpr int Width = 640;
constexpr int Height = 480;
constexpr u8 Background = 40;
constexpr u8 White = 250;
constexpr int GlyphCount = 96;
constexpr int GlyphHeight = 6;
constexpr int TextX = 100;
constexpr int TextY = 100;
constexpr int TextWidth = 20;
constexpr int TextHeight = 18;

// A glyph with a white vertical stroke and a one-pixel shadow. All printable characters
// share it, so wrapping/alignment exercise the production font implementation
// without requiring copyrighted game assets.
const u8 GlyphPixels[] = {
    3, 90, White, 90, 0xc0, 0x41, 0,
    3, 90, White, 90, 0xc0, 0x41, 0,
    3, 90, White, 90, 0xc0, 0x41, 0,
    3, 90, White, 90, 0xc0, 0x41, 0,
    3, 90, White, 90, 0xc0, 0x41, 0,
    3, 90, White, 90, 0xc0, 0x41, 0,
    0x80
};
localization::FontProfile Profile = localization::FontProfile::Latin;
}

// Only resource loading and manager ownership are stubbed. FONT.cpp, UTF-8
// traversal and both glyph blitters are the unchanged production sources.
resource::resource(ResourceCategory category, u32l id, i16 count, resource* next)
    : m_resourceType(category), m_refCount(count), m_id(id), m_next(next) {}
resource::~resource() = default;
icon::icon(u32l id) : resource(RESOURCE_CATEGORY_ICON, id, 1, nullptr) {
    m_frameCount = GlyphCount;
    const auto headerSize = sizeof(IconEntry) * GlyphCount;
    m_data = new u8[headerSize + sizeof(GlyphPixels)];
    for (int index = 0; index < GlyphCount; ++index) {
        const IconEntry entry{0, 0, 4, GlyphHeight, 0, static_cast<i32>(headerSize)};
        std::memcpy(m_data + index * sizeof(IconEntry), &entry, sizeof(entry));
    }
    std::memcpy(m_data + headerSize, GlyphPixels, sizeof(GlyphPixels));
}
inline icon::~icon() { delete[] m_data; }
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
i16 resourceManager::ReadWord() { return GlyphHeight; }
void resourceManager::Read13(void* dest) { std::memcpy(dest, "testfont.icn", 13); }
icon* resourceManager::GetIcon(const char*) { return new icon(0); }
void resourceManager::Dispose(resource* item) { delete item; }
void resourceManager::DisableLocaleAggregates() { std::abort(); }
namespace localization {
FontProfile ActiveFontProfile() { return Profile; }
void RejectResourceProfile(const char*) { std::abort(); }
}

u8 uDimPal[DIM_PALETTE_SET_COUNT][DIM_PALETTE_LEVEL_COUNT][DIM_PALETTE_COLOR_COUNT];
u8 gColorTableLighten[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableNoCycle[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableYellow[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableScenWin[DIM_PALETTE_COLOR_COUNT];
u8 gColorTableDarkGray[DIM_PALETTE_COLOR_COUNT];
b32 gbLoadingMonoIcon = false;
namespace {
heroWindowManager WindowManager;
resourceManager ResourceManager;
}
heroWindowManager* gpWindowManager = &WindowManager;
resourceManager* gpResourceManager = &ResourceManager;

int main() {
    for (auto& set : uDimPal)
        for (auto& level : set)
            for (int index = 0; index < 256; ++index)
                level[index] = static_cast<u8>(index / 2);
    for (int index = 0; index < 256; ++index) {
        gColorTableYellow[index] = static_cast<u8>(index);
        gColorTableScenWin[index] = static_cast<u8>(index);
        gColorTableDarkGray[index] = static_cast<u8>(index);
    }
    font textFont(0);
    bitmap screen;
    WindowManager.m_screen = &screen;
    int cases = 0;
    for (const auto profile : {localization::FontProfile::Latin,
                               localization::FontProfile::BukaCyrillic}) {
        Profile = profile;
        for (const char* text : {"111", "111\n111", "111 111 111"}) {
            for (const auto alignment : {FONT_ALIGN_LEFT, FONT_ALIGN_CENTER,
                                         FONT_ALIGN_RIGHT, FONT_ALIGN_CENTER_BOTH}) {
                for (const auto mode : {FONT_DRAW_DEFAULT, FONT_DRAW_YELLOW,
                                       FONT_DRAW_DARK_GRAY, FONT_DRAW_SCENARIO_WIN}) {
                    std::vector<u8> full(Width * Height, Background);
                    screen.m_pixels = full.data();
                    textFont.DrawBoundedString(text, TextX, TextY, TextWidth, TextHeight,
                                               mode, alignment);
                    if (std::count(full.begin(), full.end(), White) == 0) {
                        std::fputs("test fixture did not draw white glyph pixels\n", stderr);
                        return 1;
                    }
                    for (const SLimitData clip : {
                             SLimitData{98, 110, 98, 120},
                             SLimitData{110, 122, 98, 120},
                             SLimitData{107, 107, 102, 114},
                             SLimitData{98, 122, 103, 103},
                             SLimitData{98, 122, 98, 120},
                             SLimitData{80, 99, 100, 117},
                             SLimitData{100, 119, 118, 125},
                             SLimitData{110, 109, 100, 117}}) {
                        // Distinct pre-existing neighbour pixels must survive
                        // outside the dirty region, including glyph shadows.
                        std::vector<u8> actual(Width * Height, 77);
                        for (int y = clip.top; y <= clip.bottom; ++y)
                            for (int x = clip.left; x <= clip.right; ++x)
                                actual[y * Width + x] = Background;
                        screen.m_pixels = actual.data();
                        textFont.DrawBoundedString(text, TextX, TextY, TextWidth, TextHeight,
                                                   mode, alignment, &clip);
                        for (int y = 0; y < Height; ++y) {
                            for (int x = 0; x < Width; ++x) {
                                const auto index = y * Width + x;
                                const bool inside = x >= clip.left && x <= clip.right
                                    && y >= clip.top && y <= clip.bottom;
                                const u8 expected = inside ? full[index] : 77;
                                if (actual[index] != expected) {
                                    std::fprintf(stderr,
                                        "font clip differs at (%d,%d), case %d: %u != %u\n",
                                        x, y, cases, actual[index], expected);
                                    return 1;
                                }
                            }
                        }
                        ++cases;
                    }
                }
            }
        }
    }
    std::printf("font damage clipping: %d cases passed\n", cases);
    return 0;
}
