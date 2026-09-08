#include <SOURCE/SaveNames.h>

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>

static_assert(sizeof(save_names::LegacyFilename) == 14);

bool Check(std::string_view input, std::string_view expected) {
    const std::string original(input);
    const auto record = save_names::ToLegacyFilename(input);
    const auto end = std::find(record.begin(), record.end(), '\0');
    const bool valid = end != record.end()
        && std::string_view(record.data(), static_cast<std::size_t>(end - record.begin())) == expected
        && std::all_of(end, record.end(), [](char value) { return value == 0; })
        && input == original;
    if (!valid)
        std::fprintf(stderr, "legacy save filename mismatch for input of %zu bytes\n", input.size());
    return valid;
}

int main() {
    bool valid = true;
    valid &= Check("", "");
    valid &= Check("AUTOSAVE", "AUTOSAVE");
    valid &= Check("save.gm1", "SAVE.gm1");
    valid &= Check("A new game.GX1", "ANEWGAME.GX1");
    valid &= Check("VICTORY_E_01.GXC", "VICTORY_.GXC");
    valid &= Check("PLYREXIT.GM2", "PLYREXIT.GM2");
    valid &= Check(std::string(101, 'a'), "AAAAAAAA");
    valid &= Check(std::string(10000, 'A') + ".GX1", "AAAAAAAA.GX1");
    valid &= Check("save." + std::string(10000, 'x'), "SAVE.xxx");
    valid &= Check("new game", "NEWGAME");
    valid &= Check("a.b.GM1", "AB.GM1");
    valid &= Check("file.", "FILE.");
    valid &= Check(".GX1", ".GX1");
    valid &= Check("caf\xc3\xa9_\xd0\xaf.GX1", "CAF_.GX1");
    valid &= Check("caf\xc3\xa9.\xd0\xafGX1", "CAF.GX1");
    valid &= Check(std::string_view("save\0ignored.GX1", 16), "SAVE");
    return valid ? 0 : 1;
}
