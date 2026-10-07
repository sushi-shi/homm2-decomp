// The game record keeps its packed save layout, so the map and the typed
// fields inside it sit at offsets their types would not otherwise allow. Run
// the production map code and a field wrapper at those offsets; the sanitized
// build checks every access for alignment.
#include <EDITOR/fullMap.h>
#include <SOURCE/game.h>
#include <PLATFORM/Platform.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <new>
#include <string>

static_assert(offsetof(game, m_worldMap) % sizeof(void*) != 0);
static_assert(offsetof(game, m_viewSpell) % sizeof(i32) != 0);

void ShutDown(const char* message) {
    std::fprintf(stderr, "packed records shutdown: %s\n", message != nullptr ? message : "");
    std::abort();
}
void* BaseAlloc(u32 size, const char*, i32) { return std::calloc(size, 1); }
void BaseFree(void* memory, const char*, i32) { std::free(memory); }
namespace localization {
const char* Tr(const char* id) { return id; }
}

namespace {

bool Expect(bool valid, const char* description) {
    if (!valid)
        std::fprintf(stderr, "packed record mismatch: %s\n", description);
    return valid;
}

// Only the members under test are constructed; the rest of the record stays
// raw storage, as the engine's other subsystems are not linked here.
bool MapRoundTrip(game& state) {
    fullMap& map = *new (&state.m_worldMap) fullMap;
    map.Init(MAP_DIMENSION_SMALL, MAP_DIMENSION_SMALL);
    map.GetCell(3, 4)->m_objectIndex = 7;
    const i32 written = platform::FileOpen("map.bin", platform::FileMode::Write);
    if (written == -1)
        return Expect(false, "open map for writing");
    map.Write(written);
    platform::FileClose(written);
    map.Close();
    map.Init(MAP_DIMENSION_SMALL, MAP_DIMENSION_SMALL);

    const i32 read = platform::FileOpen("map.bin", platform::FileMode::Read);
    if (read == -1)
        return Expect(false, "open map for reading");
    map.Read(read, 0);
    platform::FileClose(read);
    bool valid = Expect(map.width == MAP_DIMENSION_SMALL && map.height == MAP_DIMENSION_SMALL,
                        "map dimensions survive the round trip");
    valid &= Expect(map.GetCell(3, 4)->m_objectIndex == 7, "map cell survives the round trip");
    map.~fullMap();
    return valid;
}

bool FieldWrapper(game& state) {
    auto& spell = state.m_viewSpell;
    spell = SPELL_FIREBALL;
    spell += 1;
    return Expect(spell.value() == H2EnumIndex(SPELL_FIREBALL) + 1,
                  "typed field at an unaligned record offset");
}

}

int main() {
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root = std::filesystem::temp_directory_path()
        / ("homm2-packed-records-test-" + std::to_string(nonce));
    std::filesystem::create_directories(root);
    if (setenv("HOMM2_DATA", root.c_str(), 1) != 0
        || setenv("XDG_DATA_HOME", (root / "state").c_str(), 1) != 0
        || !platform::Startup()) {
        std::filesystem::remove_all(root);
        return 1;
    }

    void* storage = std::calloc(1, sizeof(game));
    auto& state = *static_cast<game*>(storage);
    bool valid = MapRoundTrip(state);
    valid &= FieldWrapper(state);
    std::free(storage);

    platform::Shutdown();
    std::filesystem::remove_all(root);
    return valid ? 0 : 1;
}
