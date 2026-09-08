#include <SOURCE/HighScoreIO.h>
#include <SOURCE/KB.h>
#include <PLATFORM/Binary.h>
#include <PLATFORM/File.h>

#include <array>
#include <bit>
#include <cstring>

namespace {

using Record = std::array<u8, 100>;
constexpr std::size_t ScenarioNameOffset = 17;
constexpr std::size_t ScoreOffset = 58;
constexpr std::size_t DaysOffset = 62;
constexpr std::size_t ScenarioOffset = 66;
constexpr std::size_t CheatedOffset = 70;
constexpr std::size_t ReservedOffset = 71;

bool TerminatedNames(const HighScoreEntry& entry) {
    return std::memchr(entry.playerName, 0, sizeof(entry.playerName)) != nullptr
        && std::memchr(entry.scenarioName, 0, sizeof(entry.scenarioName)) != nullptr;
}

i32 ReadInteger(const Record& bytes, std::size_t offset) {
    u32 value = 0;
    platform::binary::ReadU32(bytes.data(), bytes.size(), offset, value);
    return std::bit_cast<i32>(value);
}

}

bool ReadHighScoreEntry(i32 file, HighScoreEntry& entry) {
    entry = {};
    entry.score = HIGH_SCORE_EMPTY;
    Record bytes{};
    if (!platform::FileReadExact(file, bytes.data(), bytes.size()))
        return false;
    HighScoreEntry parsed{};
    std::memcpy(parsed.playerName, bytes.data(), sizeof(parsed.playerName));
    std::memcpy(parsed.scenarioName, bytes.data() + ScenarioNameOffset, sizeof(parsed.scenarioName));
    parsed.score = ReadInteger(bytes, ScoreOffset);
    parsed.days = ReadInteger(bytes, DaysOffset);
    parsed.scenario = ReadInteger(bytes, ScenarioOffset);
    parsed.cheated = static_cast<char>(bytes[CheatedOffset]);
    std::memcpy(parsed.reserved, bytes.data() + ReservedOffset, sizeof(parsed.reserved));
    if (parsed.score == HIGH_SCORE_EMPTY)
        return true;
    if (!TerminatedNames(parsed))
        return false;
    entry = parsed;
    return true;
}

bool WriteHighScoreEntry(i32 file, const HighScoreEntry& entry) {
    if (!TerminatedNames(entry))
        return false;
    Record bytes{};
    std::memcpy(bytes.data(), entry.playerName, sizeof(entry.playerName));
    std::memcpy(bytes.data() + ScenarioNameOffset, entry.scenarioName, sizeof(entry.scenarioName));
    // Conversion by value avoids binding a reference to a packed integer.
    platform::binary::WriteU32(bytes.data(), bytes.size(), ScoreOffset, static_cast<u32>(entry.score));
    platform::binary::WriteU32(bytes.data(), bytes.size(), DaysOffset, static_cast<u32>(entry.days));
    platform::binary::WriteU32(bytes.data(), bytes.size(), ScenarioOffset, static_cast<u32>(entry.scenario));
    bytes[CheatedOffset] = static_cast<u8>(entry.cheated);
    std::memcpy(bytes.data() + ReservedOffset, entry.reserved, sizeof(entry.reserved));
    return platform::FileWriteExact(file, bytes.data(), bytes.size());
}
