#include <SOURCE/REQUEST.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>
#include <PLATFORM/Platform.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

// The production loader needs only these startup-owned values.
char gText[GLOBAL_TEXT_BUFFER_SIZE];
char gcMapPath[GLOBAL_MAP_PATH_SIZE] = "MAPS/";

namespace localization {
TextEncoding DefaultFileTextEncoding() { return TextEncoding::Windows1252; }
}

namespace {

bool Expect(bool valid, const char* description) {
    if (!valid)
        std::fprintf(stderr, "map header mismatch: %s\n", description);
    return valid;
}

SMapHeader Header() {
    SMapHeader header{};
    header.magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    header.width = header.height = MAP_DIMENSION_SMALL;
    header.difficulty = DIFFICULTY_NORMAL;
    header.playerCount = header.maxHumanPlayers = 2;
    header.playerEnabled[0] = header.playerEnabled[1] = 1;
    header.playerCanHuman[0] = header.playerCanHuman[1] = 1;
    header.playerRace[0] = FACTION_RANDOM;
    header.playerRace[1] = FACTION_NEUTRAL;
    std::strcpy(header.name, "A valid map");
    std::strcpy(header.description, "Description");
    return header;
}

bool SaveHeader(const std::filesystem::path& path, const SMapHeader& header,
                std::size_t size = sizeof(SMapHeader)) {
    std::ofstream output(path, std::ios::binary);
    output.write(reinterpret_cast<const char*>(&header), static_cast<std::streamsize>(size));
    output.close();
    return !output.fail();
}

bool BoundedText() {
    using namespace localization;
    bool valid = true;
    const char complete[] = {'a', 'b', 'c', 0};
    const char missing[] = {'a', 'b', 'c'};
    const char empty[] = {0};
    const char russian[] = {'\xd0', '\xaf', 0, 'x'};
    const char partial[] = {'\xe2', '\x82'};
    valid &= Expect(HasTextTerminator(TextField(complete)), "last-byte terminator");
    valid &= Expect(!HasTextTerminator(TextField(missing)), "missing terminator");
    valid &= Expect(DecodeText(TextField(missing), TextEncoding::Windows1252) == "abc",
                    "bounded decoding cannot read the next field");
    valid &= Expect(DecodeText(TextField(empty), TextEncoding::Utf8).empty(), "empty field");
    valid &= Expect(DecodeText(TextField(russian), TextEncoding::Utf8) == "\xd0\xaf",
                    "ignore padding after the terminator");
    std::string_view fields[] = {TextField(complete), TextField(russian)};
    valid &= Expect(DetectTextEncoding(fields, 2, TextEncoding::Windows1252)
                        == TextEncoding::Utf8, "UTF-8 provenance");
    fields[1] = TextField(partial);
    valid &= Expect(DetectTextEncoding(fields, 2, TextEncoding::Windows1251)
                        == TextEncoding::Windows1251, "truncated UTF-8 uses legacy provenance");
    return valid;
}

}

int main(int argc, char** argv) {
    // Optional read-only survey of actual map headers, kept out of asset-free CTest.
    if (argc > 1) {
        for (int index = 1; index < argc; ++index) {
            SMapHeader header{};
            std::ifstream input(argv[index], std::ios::binary);
            input.read(reinterpret_cast<char*>(&header), sizeof(header));
            const char* error = input ? MapHeaderError(header) : "truncated header";
            if (error != nullptr) {
                std::fprintf(stderr, "%s: %s\n", argv[index], error);
                return 1;
            }
        }
        return 0;
    }

    bool valid = BoundedText();
    auto raw = std::make_unique<SMapHeader>();
    std::memset(raw.get(), 'A', sizeof(*raw));
    raw->name[0] = 0;
    valid &= Expect(GetMapHeaderTextEncoding(raw.get()) == localization::TextEncoding::Windows1252,
                    "audit unterminated header probe stays bounded");

    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root = std::filesystem::temp_directory_path()
        / ("homm2-map-header-test-" + std::to_string(nonce));
    std::filesystem::create_directories(root / "MAPS");
    if (setenv("HOMM2_DATA", root.c_str(), 1) != 0
        || setenv("XDG_DATA_HOME", (root / "state").c_str(), 1) != 0
        || !platform::Startup()) {
        std::filesystem::remove_all(root);
        return 1;
    }

    const auto check = [&](const SMapHeader& header, bool accepted, const char* description,
                           std::size_t size = sizeof(SMapHeader)) {
        SMapHeader loaded{};
        const bool written = SaveHeader(root / "MAPS" / "test.mp2", header, size);
        const bool result = written && GetMapHeader("test.mp2", &loaded) != 0;
        return Expect(written && result == accepted, description)
            && Expect(accepted ? std::memcmp(&loaded, &header, sizeof(header)) == 0
                               : loaded.magic == 0, "loader result state");
    };

    SMapHeader header = Header();
    valid &= check(header, true, "valid expansion map");
    header.magic = MAP_HEADER_MAGIC_BASE_GAME;
    valid &= check(header, true, "valid base-game map");
    valid &= check(header, false, "short record", sizeof(header) - 1);
    valid &= Expect(GetMapHeader("missing.mp2", &header) == 0 && header.magic == 0,
                    "missing file clears old header");

    header = Header();
    std::fill_n(header.name, sizeof(header.name) - 1, 'N');
    std::fill_n(header.description, sizeof(header.description) - 1, 'D');
    valid &= check(header, true, "full-capacity terminated strings");
    header.name[sizeof(header.name) - 1] = 'N';
    valid &= check(header, false, "unterminated name");
    header = Header();
    std::memset(header.description, 'D', sizeof(header.description));
    valid &= check(header, false, "unterminated description");
    header = Header(); header.magic = 0;
    valid &= check(header, false, "unknown format");
    header = Header(); header.width = 0;
    valid &= check(header, false, "zero width");
    header = Header(); header.height = 145;
    valid &= check(header, false, "height exceeds map storage");
    header = Header(); header.playerCount = 7;
    valid &= check(header, false, "player count exceeds storage");
    header = Header(); header.minHumanPlayers = 3;
    valid &= check(header, false, "inconsistent human count");
    header = Header(); header.playerEnabled[2] = 1;
    valid &= check(header, false, "inconsistent enabled count");
    header = Header(); header.playerCanComputer[0] = 2;
    valid &= check(header, false, "invalid player flag");
    header = Header(); header.playerRace[0] = FactionTypeFromCode(8);
    valid &= check(header, false, "invalid faction");
    header = Header(); header.difficulty = GameDifficultyFromCode(5);
    valid &= check(header, false, "invalid difficulty");
    header = Header(); header.victoryCondition = static_cast<MapVictoryCondition>(6);
    valid &= check(header, false, "invalid victory condition");
    header = Header(); header.lossCondition = static_cast<MapLossCondition>(4);
    valid &= check(header, false, "invalid loss condition");
    header = Header(); header.victoryCondition = MAP_VICTORY_FIND_ARTIFACT;
    header.victoryConditionValue = 65535;
    valid &= check(header, false, "invalid victory artifact index");
    header = Header(); header.rumourCount = 31;
    valid &= check(header, false, "rumour count exceeds storage");
    header = Header(); header.timeEventCount = 51;
    valid &= check(header, false, "time-event count exceeds storage");

    platform::Shutdown();
    std::filesystem::remove_all(root);
    return valid ? 0 : 1;
}
