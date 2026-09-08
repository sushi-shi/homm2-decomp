#include <SOURCE/HighScoreIO.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Platform.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

bool Expect(bool condition, const char* description) {
    if (!condition)
        std::fprintf(stderr, "high-score I/O mismatch: %s\n", description);
    return condition;
}

std::array<u8, 100> Record(u8 score) {
    std::array<u8, 100> result{};
    std::memcpy(result.data(), "Player", 7);
    std::memcpy(result.data() + 17, "Scenario", 9);
    result[58] = score;
    result[62] = 15;
    result[66] = 2;
    result[70] = 1;
    return result;
}

bool WriteFixture(const std::filesystem::path& path, const std::vector<u8>& bytes) {
    std::ofstream output(path, std::ios::binary);
    output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    output.close();
    return !output.fail();
}

}

int main() {
    const auto nonce = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto root = std::filesystem::temp_directory_path()
        / ("homm2-high-score-test-" + std::to_string(nonce));
    std::filesystem::create_directories(root / "DATA");
    if (setenv("HOMM2_DATA", root.c_str(), 1) != 0
        || setenv("XDG_DATA_HOME", (root / "state").c_str(), 1) != 0
        || !platform::Startup())
        return 1;

    bool valid = true;
    std::vector<u8> bytes;
    for (u8 index = 0; index < HIGH_SCORE_ENTRY_COUNT; ++index) {
        const auto record = Record(index + 1);
        bytes.insert(bytes.end(), record.begin(), record.end());
    }
    // Extra bytes must not affect reading the ten-record table or its canary.
    bytes.insert(bytes.end(), 2000, 0xff);
    valid &= Expect(WriteFixture(root / "DATA" / "scores.bin", bytes), "write table fixture");
    i32 file = platform::FileOpen("DATA/scores.bin", platform::FileMode::Read);
    struct Guarded {
        HighScoreEntry entries[HIGH_SCORE_ENTRY_COUNT];
        u32 canary = 0x1234abcd;
    } table{};
    for (i32 index = 0; index < HIGH_SCORE_ENTRY_COUNT; ++index) {
        valid &= Expect(ReadHighScoreEntry(file, table.entries[index]), "read complete record");
        valid &= Expect(table.entries[index].score == index + 1
            && table.entries[index].days == 15 && table.entries[index].scenario == 2
            && std::strcmp(table.entries[index].playerName, "Player") == 0,
            "retain every entry in order");
        valid &= Expect(platform::FileTell(file) == (index + 1) * 100,
                        "consume one 100-byte record per call");
    }
    valid &= Expect(table.canary == 0x1234abcd, "table canary");
    platform::FileClose(file);

    bytes.resize(199);
    valid &= Expect(WriteFixture(root / "DATA" / "scores.bin", bytes), "write short tail");
    file = platform::FileOpen("DATA/scores.bin", platform::FileMode::Read);
    HighScoreEntry first{};
    HighScoreEntry incomplete{};
    valid &= Expect(ReadHighScoreEntry(file, first) && first.score == 1, "complete prefix");
    valid &= Expect(!ReadHighScoreEntry(file, incomplete) && incomplete.score == HIGH_SCORE_EMPTY
        && incomplete.playerName[0] == 0 && first.score == 1, "short tail becomes empty");
    platform::FileClose(file);

    auto record = Record(42);
    std::fill_n(record.data(), 17, 'X');
    valid &= Expect(WriteFixture(root / "DATA" / "scores.bin", {record.begin(), record.end()}),
                    "write unterminated name");
    file = platform::FileOpen("DATA/scores.bin", platform::FileMode::Read);
    valid &= Expect(!ReadHighScoreEntry(file, first) && first.score == HIGH_SCORE_EMPTY,
                    "reject unterminated name");
    platform::FileClose(file);

    record = Record(42);
    std::fill_n(record.data() + 17, 41, 'X');
    valid &= Expect(WriteFixture(root / "DATA" / "scores.bin", {record.begin(), record.end()}),
                    "write unterminated scenario");
    file = platform::FileOpen("DATA/scores.bin", platform::FileMode::Read);
    valid &= Expect(!ReadHighScoreEntry(file, first), "reject unterminated scenario");
    platform::FileClose(file);

    HighScoreEntry entry{};
    std::strcpy(entry.playerName, "Player");
    std::strcpy(entry.scenarioName, "Scenario");
    entry.score = 0x12345678;
    entry.days = -2;
    entry.scenario = 2;
    entry.cheated = 1;
    file = platform::FileOpen("DATA/written.bin", platform::FileMode::Write);
    valid &= Expect(WriteHighScoreEntry(file, entry), "write explicit record");
    valid &= Expect(platform::FileTell(file) == 100, "wire size");
    platform::FileClose(file);
    file = platform::FileOpen("DATA/written.bin", platform::FileMode::Read);
    std::array<u8, 100> written{};
    valid &= Expect(platform::FileReadExact(file, written.data(), written.size()), "read wire bytes");
    auto expected = Record(0x78);
    expected[59] = 0x56; expected[60] = 0x34; expected[61] = 0x12;
    expected[62] = 0xfe; expected[63] = expected[64] = expected[65] = 0xff;
    valid &= Expect(written == expected, "little-endian wire record");
    platform::FileSeek(file, 0);
    valid &= Expect(ReadHighScoreEntry(file, first) && first.score == entry.score
        && first.days == -2, "signed fields round trip");
    platform::FileClose(file);

    platform::Shutdown();
    std::filesystem::remove_all(root);
    return valid ? 0 : 1;
}
