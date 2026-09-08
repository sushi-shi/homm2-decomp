#include <SOURCE/REQUEST.h>
#include <SOURCE/Localization.h>
#include <SOURCE/game.h>
#include <SOURCE/KB.h>
#include <SOURCE/X_GLOBAL.h>
#include <BASE/Utf8.h>
#include <PLATFORM/Platform.h>

static_assert(sizeof(SMapHeader) == MAP_HEADER_SIZE);

const char* MapHeaderError(const SMapHeader& header) {
    if (header.magic != MAP_HEADER_MAGIC_BASE_GAME
        && header.magic != MAP_HEADER_MAGIC_EXPANSION_GAME)
        return "unknown map format";
    if (header.width == 0 || header.width > MAP_DIMENSION_XLARGE
        || header.height == 0 || header.height > MAP_DIMENSION_XLARGE)
        return "invalid map dimensions";
    if (header.difficulty.value() >= H2EnumIndex(DIFFICULTY_COUNT))
        return "invalid map difficulty";
    if (header.playerCount == 0 || header.playerCount > MAP_HEADER_PLAYER_COUNT
        || header.minHumanPlayers > header.maxHumanPlayers
        || header.maxHumanPlayers > header.playerCount)
        return "invalid player counts";

    unsigned enabled = 0;
    for (unsigned player = 0; player < MAP_HEADER_PLAYER_COUNT; ++player) {
        if (header.playerEnabled[player] > 1 || header.playerCanHuman[player] > 1
            || header.playerCanComputer[player] > 1)
            return "invalid player flags";
        if (header.playerEnabled[player]) {
            ++enabled;
            // Retail maps use both 6 and 7 in addition to the six factions.
            const i8 race = header.playerRace[player].value();
            if (race < 0 || race > H2EnumIndex(FACTION_RANDOM))
                return "invalid player faction";
        }
    }
    if (enabled != header.playerCount)
        return "player count does not match enabled slots";
    if (H2EnumIndex(header.victoryCondition) > H2EnumIndex(MAP_VICTORY_ACCUMULATE_GOLD)
        || H2EnumIndex(header.lossCondition) > H2EnumIndex(MAP_LOSS_TIME))
        return "invalid victory or loss condition";
    if (header.victoryCondition == MAP_VICTORY_FIND_ARTIFACT
        && header.victoryConditionValue > H2EnumIndex(ARTIFACT_COUNT))
        return "invalid victory artifact";
    if (header.rumourCount > GAME_RUMOUR_EVENT_CAPACITY
        || header.timeEventCount > GAME_TIME_EVENT_CAPACITY)
        return "too many map events";
    if (!localization::HasTextTerminator(localization::TextField(header.name)))
        return "unterminated map name";
    if (!localization::HasTextTerminator(localization::TextField(header.description)))
        return "unterminated map description";
    return nullptr;
}

localization::TextEncoding GetMapHeaderTextEncoding(const SMapHeader* header) {
    if (header == nullptr)
        return localization::DefaultFileTextEncoding();
    const std::string_view fields[] = {
        localization::TextField(header->name),
        localization::TextField(header->description),
    };
    return localization::DetectTextEncoding(
        fields, sizeof(fields) / sizeof(fields[0]), localization::DefaultFileTextEncoding()
    );
}

i32 GetMapHeader(const char* filename, struct SMapHeader* header) {
    if (header == nullptr)
        return 0;
    *header = {};
    utf8::Format(gText, GLOBAL_TEXT_BUFFER_SIZE, "%s%s", gcMapPath, filename);
    i32 file = platform::FileOpen(gText, platform::FileMode::Read);
    if (file == -1) {
        return 0;
    }
    const bool complete = platform::FileReadExact(file, header, sizeof(*header));
    platform::FileClose(file);
    const char* error = complete ? MapHeaderError(*header) : "truncated map header";
    if (error != nullptr) {
        platform::Host().Log(platform::LogLevel::Warning,
            (std::string("Invalid map '") + filename + "': " + error).c_str());
        *header = {};
        return 0;
    }
    return 1;
}
