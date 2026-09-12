// Exercise the real mouseManager selection/toggle logic with synthetic assets.
// Only resource I/O, drawing, and the host interfaces are replaced by fixtures.
#include <BASE/bitmap.h>
#include <BASE/heroWindowManager.h>
#include <BASE/icon.h>
#include <BASE/Icon2b.h>
#include <BASE/MonochromeCursor.h>
#include <BASE/mouseManager.h>
#include <BASE/resourceManager.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace {
int failures = 0, resourceDepth = 0, reads = 0, colorDraws = 0;
std::vector<std::string> names;
std::string colorName;

void Expect(bool ok, const char* message) {
    if (!ok) {
        std::fprintf(stderr, "cursor selection: %s\n", message);
        ++failures;
    }
}

class Video final : public platform::IVideo {
public:
    bool visible = true;
    int selected = 0, resets = 0;
    platform::MonochromeCursor cursor;
    bool Open(const platform::DisplayMode&) override { return true; }
    void Close() override {}
    std::uint8_t* Pixels() override { return nullptr; }
    int Pitch() const override { return 640; }
    platform::Size Resolution() const override { return {640, 480}; }
    void SetPalette(const platform::Color*, int, int) override {}
    void Blit(const platform::Rect&, platform::Point) override {}
    void Present() override {}
    void ShowCursor(bool value) override { visible = value; }
    bool SetMonochromeCursor(const platform::MonochromeCursor& value) override {
        cursor = value;
        ++selected;
        return true;
    }
    void ResetCursor() override { ++resets; }
} video;

class Input final : public platform::IInput {
public:
    bool Poll(platform::Event&) override { return false; }
    bool IsKeyDown(platform::Key) const override { return false; }
    bool IsMouseButtonDown(platform::MouseButton) const override { return false; }
    platform::Point MousePosition() const override { return {320, 240}; }
    void WarpMouse(platform::Point) override {}
    void StartTextInput() override {}
    void StopTextInput() override {}
} input;

platform::Backend backend{&video, &input};
}

namespace platform {
Backend& Get() { return backend; }
}

resource::resource(ResourceCategory category, u32l id, i16 count, resource* next)
    : m_resourceType(category), m_refCount(count), m_id(id), m_next(next) {}
resource::~resource() = default;
baseManager::baseManager() = default;
bitmap::bitmap() : resource(RESOURCE_CATEGORY_BITMAP, 0, 0, nullptr),
    m_bitmapType(BITMAP_TYPE_MEMORY), m_width(640), m_height(480), m_pixels(nullptr) {}
bitmap::bitmap(BitmapType, i16, i16) : bitmap() {}
bitmap::~bitmap() = default;
void bitmap::CopyToCareful(bitmap*, i32, i32, i32, i32, i32, i32) {}
heroWindowManager::heroWindowManager() = default;
i32 heroWindowManager::Open(i32) { return 0; }
void heroWindowManager::Close() {}
MessageDispatchResult heroWindowManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
resourceManager::resourceManager() = default;
i32 resourceManager::Open(i32) { return 0; }
void resourceManager::Close() {}
MessageDispatchResult resourceManager::Main(tag_message&) { return MESSAGE_DISPATCH_CONTINUE; }
void resourceManager::Dispose(resource*) {}
void resourceManager::SavePosition() { ++resourceDepth; }
void resourceManager::RestorePosition() { --resourceDepth; }
u32l resourceManager::MakeId(const char* name, i32) {
    names.emplace_back(name);
    return 1;
}
u32l resourceManager::GetFileSize(u32l) { return mouse_cursor::ResourceBytes; }
void resourceManager::PointToFile(u32l) {}
void resourceManager::ReadBlock(void* output, u32l size) {
    Expect(size == mouse_cursor::ResourceBytes, "bounded bitmap read");
    std::array<u8, mouse_cursor::ResourceBytes> bytes {};
    bytes[0] = 33;
    bytes[2] = bytes[4] = 32;
    bytes[6] = 1;
    bytes[7] = 2;
    std::memcpy(output, bytes.data(), bytes.size());
    ++reads;
}
icon* resourceManager::GetIcon(const char* name) {
    colorName = name;
    return nullptr;
}
void ShutDown(const char* message) { std::fprintf(stderr, "%s\n", message); std::abort(); }
void ProcessAssert(i32 condition, const char*, i32) { Expect(condition != 0, "game assertion"); }
void CheckChangeCursor(i32, i32, i32) {}
void BlitBitmapToScreenNoMouseCheck(bitmap*, i32, i32, i32, i32, i32, i32) {}
void IconToBitmap(icon*, bitmap*, i32, i32, i32, IconDrawClipMode, i32, i32, i32, i32, i32) {
    ++colorDraws;
}

b32 gbColorMice = false;
i32 gbPutzingWithMouseCtr = 0;
ConfigExecutable giCurExe = CONFIG_EXECUTABLE_GAME;
namespace {
resourceManager resources;
heroWindowManager windows;
bitmap screen;
}
resourceManager* gpResourceManager = &resources;
heroWindowManager* gpWindowManager = &windows;

int main() {
    windows.m_screen = &screen;
    windows.m_updateFlags = 0;
    mouseManager mouse;
    mouse.Open(0);
    mouse.SetPointer("advmco.icn", 0, MOUSE_AUTO_CURSOR_TYPE);
    Expect(names.back() == "ADVMBW01.BMP" && video.selected == 1, "adventure cursor loads");
    Expect(video.cursor.hotspot.x == 2 && video.cursor.hotspot.y == 3, "adventure hotspot");
    Expect(video.cursor.data[0] == 0x40 && video.cursor.mask[0] == 0xc0, "correct white/black shape");
    mouse.SetPointer(1);
    Expect(names.back() == "ADVMBW02.BMP" && video.selected == 2, "frame changes");
    mouse.SetPointer(0);
    Expect(reads == 2 && video.selected == 3, "resource cache reuse");
    mouse.SetPointer(0);
    Expect(video.selected == 3, "identical selection is stable");
    mouse.ReallyHidePointer();
    mouse.SetPointer("cmseco.icn", 0, MOUSE_AUTO_CURSOR_TYPE);
    Expect(names.back() == "CMSEBW01.BMP" && video.selected == 4, "same frame/new role changes shape");
    Expect(!video.visible, "hidden cursor stays hidden on shape change");
    Expect(video.cursor.hotspot.x == 10 && video.cursor.hotspot.y == 9, "combat hotspot");
    mouse.SetPointer("spelco.icn", 0, MOUSE_AUTO_CURSOR_TYPE);
    Expect(names.back() == "SPELBW00.BMP" && video.selected == 5, "spell numbering starts at zero");
    Expect(video.cursor.hotspot.x == 15 && video.cursor.hotspot.y == 15, "monochrome spell hotspot");
    mouse.SetPointer("", MOUSE_KEEP_CURRENT_FRAME, MOUSE_CURSOR_COMBAT);
    Expect(mouse.m_cursorFrame == 0 && mouse.m_cursorType == MOUSE_CURSOR_COMBAT,
           "keep frame across explicit role change");
    Expect(video.selected == 6, "role change selects cached shape");
    mouse.ReallyShowPointer();
    Expect(video.visible, "explicit show");

    // Both modes preserve the same role and frame. Color rendering is observed
    // through IconToBitmap; monochrome rendering through the host interface.
    mouse.SetColorMice(true);
    Expect(gbColorMice && !video.visible && colorName == "CMSECO.ICN", "switch to color");
    Expect(mouse.m_cursorFrame == 0 && mouse.m_cursorType == MOUSE_CURSOR_COMBAT && colorDraws > 0,
           "color mode keeps shared selection and draws");
    mouse.SetPointer(1);
    const int prior = video.selected;
    mouse.SetColorMice(false);
    Expect(!gbColorMice && video.visible && video.selected == prior + 1, "switch back to monochrome");
    Expect(names.back() == "CMSEBW02.BMP" && mouse.m_cursorFrame == 1,
           "mode toggle retains current action");
    Expect(resourceDepth == 0 && gbPutzingWithMouseCtr == 0 && !gbInSetPointer,
           "selection preserves resource and reentrancy state");
    mouse.Close();
    Expect(video.resets == 1 && video.visible, "close restores OS cursor");
    std::printf("mouse cursor selection: %d resource reads, %d shapes, %d failures\n",
                reads, video.selected, failures);
    return failures != 0;
}
