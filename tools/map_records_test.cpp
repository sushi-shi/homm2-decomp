#include <SOURCE/MapRecords.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>

#include <array>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using map_records::Kind;
using map_records::Record;

bool Expect(bool valid, const char* description) {
    if (!valid) std::fprintf(stderr, "map record mismatch: %s\n", description);
    return valid;
}

std::vector<u8> Valid(Kind kind) {
    switch (kind) {
    case Kind::Hero:
    case Kind::Jail: return std::vector<u8>(sizeof(mapHeroExtra), 0);
    case Kind::Town: return std::vector<u8>(sizeof(mapTownExtra), 0);
    case Kind::Sign: return std::vector<u8>(sizeof(signEventExtra), 0);
    case Kind::Rumour: return std::vector<u8>(sizeof(rumourEventExtra), 0);
    case Kind::TimeEvent: return std::vector<u8>(sizeof(timeEventExtra), 0);
    case Kind::MapEvent: return std::vector<u8>(sizeof(EventExtra), 0);
    case Kind::Sphinx: return std::vector<u8>(sizeof(mapEventExtra), 0);
    }
    throw std::runtime_error("unknown kind");
}

bool Boundaries() {
    bool valid = true;
    for (Kind kind : {Kind::Hero, Kind::Jail, Kind::Town, Kind::Sign, Kind::Rumour,
                      Kind::TimeEvent, Kind::MapEvent, Kind::Sphinx}) {
        auto record = Valid(kind);
        valid &= Expect(map_records::RecordError(record, kind) == nullptr, "valid minimal record");
        for (std::size_t length = 0; length < record.size(); ++length)
            valid &= Expect(map_records::RecordError(Record(record).first(length), kind) != nullptr,
                            "reject every truncated prefix");
        if (kind != Kind::Hero && kind != Kind::Jail) {
            record.back() = 'x';
            if (kind == Kind::Town)
                std::memset(record.data() + offsetof(mapTownExtra, name), 'x', EVENT_RECORD_TOWN_NAME_SIZE);
            valid &= Expect(map_records::RecordError(record, kind) != nullptr, "missing terminator");
        }
    }
    auto hero = Valid(Kind::Hero);
    hero[offsetof(mapHeroExtra, hasCustomHero)] = 1;
    hero[offsetof(mapHeroExtra, heroId)] = 255;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) != nullptr, "negative hero array index");
    hero[offsetof(mapHeroExtra, heroId)] = 70;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) == nullptr, "custom portrait above hero roster");
    hero[offsetof(mapHeroExtra, hasCustomSkills)] = 1;
    hero[offsetof(mapHeroExtra, skillTypes)] = 14;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) != nullptr, "skill index outside array");
    hero[offsetof(mapHeroExtra, skillTypes)] = 0;
    hero[offsetof(mapHeroExtra, skillLevels)] = 127;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) != nullptr, "invalid skill level");
    hero = Valid(Kind::Hero);
    hero[offsetof(mapHeroExtra, hasCustomArmy)] = 1;
    hero[offsetof(mapHeroExtra, troopCounts)] = 1;
    hero[offsetof(mapHeroExtra, troopTypes)] = 255;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) != nullptr, "invalid nonempty troop type");
    hero[offsetof(mapHeroExtra, troopCounts)] = 0;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) == nullptr, "unused troop slot sentinel");
    hero[offsetof(mapHeroExtra, hasCustomName)] = 1;
    std::memset(hero.data() + offsetof(mapHeroExtra, name), 'n', EVENT_RECORD_HERO_NAME_SIZE);
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) != nullptr, "bounded custom hero name");
    hero[offsetof(mapHeroExtra, name) + EVENT_RECORD_HERO_NAME_SIZE - 1] = 0;
    valid &= Expect(map_records::RecordError(hero, Kind::Hero) == nullptr, "last byte name terminator");
    hero[offsetof(mapHeroExtra, heroClass)] = 6;
    valid &= Expect(map_records::RecordError(hero, Kind::Jail) != nullptr, "jailed hero faction index");

    auto sphinx = Valid(Kind::Sphinx);
    sphinx[offsetof(mapEventExtra, answerCount)] = 9;
    valid &= Expect(map_records::RecordError(sphinx, Kind::Sphinx) != nullptr, "answer count exceeds array");
    sphinx[offsetof(mapEventExtra, answerCount)] = 1;
    std::memset(sphinx.data() + offsetof(mapEventExtra, answers), 'a', EVENT_RECORD_MAP_ANSWER_SIZE);
    valid &= Expect(map_records::RecordError(sphinx, Kind::Sphinx) != nullptr, "unterminated fixed answer");
    sphinx[offsetof(mapEventExtra, answers) + EVENT_RECORD_MAP_ANSWER_SIZE - 1] = 0;
    valid &= Expect(map_records::RecordError(sphinx, Kind::Sphinx) == nullptr, "full answer terminator");
    sphinx[offsetof(mapEventExtra, artifact)] = 103;
    valid &= Expect(map_records::RecordError(sphinx, Kind::Sphinx) != nullptr, "artifact outside table");
    auto town = Valid(Kind::Town);
    town[offsetof(mapTownExtra, color)] = 6;
    valid &= Expect(map_records::RecordError(town, Kind::Town) != nullptr, "town owner color index");
    town[offsetof(mapTownExtra, color)] = 255;
    town[offsetof(mapTownExtra, hasCustomBuildings)] = 1;
    town[offsetof(mapTownExtra, mageGuildLevel)] = 6;
    valid &= Expect(map_records::RecordError(town, Kind::Town) != nullptr, "mage guild level outside table");
    const std::array<u8, 8> text{1, 2, 'a', 'b', 0, 'p', 'a', 'd'};
    valid &= Expect(map_records::Text(text, 2) == "ab", "bounded text stops at first NUL");
    valid &= Expect(map_records::Text(text, 80).empty(), "out of bounds text offset");
    const std::array<u8, 2> partial{0xe2, 0x82};
    valid &= Expect(map_records::Text(partial, 0).size() == 2, "bounded truncated UTF-8 bytes");
    return valid;
}

bool Tables() {
    bool valid = true;
    std::array<mapCell, 2> cells{};
    std::array<mapCellExtra, 4> extras{};
    cells[0].m_extraIndex = 1;
    extras[1].nextIndex = 2;
    extras[3].nextIndex = MAPCELL_EXTRA_FREE;
    valid &= Expect(map_records::CellDataError(cells, extras) == nullptr, "valid chain and unused free slot");
    extras[2].nextIndex = 1;
    valid &= Expect(map_records::CellDataError(cells, extras) != nullptr, "cyclic chain");
    extras[2].nextIndex = 4;
    valid &= Expect(map_records::CellDataError(cells, extras) != nullptr, "chain index past allocation");
    extras[2].nextIndex = MAPCELL_EXTRA_FREE;
    valid &= Expect(map_records::CellDataError(cells, extras) != nullptr, "reachable free marker");
    extras[2].nextIndex = 0;
    cells[1].m_extraIndex = 2;
    valid &= Expect(map_records::CellDataError(cells, extras) == nullptr, "shared terminating chain suffix");

    const auto sign = Valid(Kind::Sign);
    const auto hero = Valid(Kind::Hero);
    const auto event = Valid(Kind::MapEvent);
    std::array<Record, 3> records{{{}, sign, hero}};
    const auto check = [&](bool accepted, bool newMap, const char* description) {
        return Expect((map_records::ExtraTableError(cells, records, {}, {}, {}, newMap) == nullptr)
                        == accepted, description);
    };
    cells = {};
    cells[0].m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN);
    valid &= check(true, true, "absent optional sign text");
    cells[0].m_objectMetadata = 1;
    valid &= check(true, true, "valid sign reference");
    cells[0].m_objectMetadata = 3;
    valid &= check(false, true, "record index past table");
    cells[0].m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_HERO);
    cells[0].m_objectMetadata = 2;
    valid &= check(true, true, "new hero record");
    cells[1] = cells[0];
    valid &= check(false, true, "duplicate consumable record");
    records[2] = {};
    valid &= check(true, false, "consumed hero extras ignored when loading a save");
    valid &= check(false, true, "missing new hero extra");
    std::array<u16, 1> indices{0};
    valid &= Expect(map_records::ExtraTableError({}, records, indices, {}, {}, true) != nullptr,
                    "zero event index is not a record");
    records[1] = event;
    indices[0] = 1;
    valid &= Expect(map_records::ExtraTableError({}, records, {}, {}, indices, false) == nullptr,
                    "saved map event list");
    std::array<mapCell, 51> eventCells{};
    for (auto& cell : eventCells) {
        cell.m_triggerType = MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT);
        cell.m_objectMetadata = 1;
    }
    valid &= Expect(map_records::ExtraTableError(eventCells, records, {}, {}, {}, true) != nullptr,
                    "map event cells exceed runtime capacity");
    return valid;
}

class Reader {
    Record bytes;
public:
    explicit Reader(Record input) : bytes(input) {}
    Record Take(std::size_t count) {
        if (count > bytes.size()) throw std::runtime_error("truncated map fixture");
        const auto result = bytes.first(count);
        bytes = bytes.subspan(count);
        return result;
    }
    u32 Number(std::size_t count) {
        u32 result = 0;
        const auto data = Take(count);
        for (std::size_t i = 0; i < count; ++i) result |= u32(data[i]) << (8 * i);
        return result;
    }
    std::vector<u16> Indices(std::size_t count) {
        std::vector<u16> result;
        for (std::size_t i = 0; i < count; ++i) result.push_back(static_cast<u16>(Number(2)));
        return result;
    }
};

bool Retail(const char* path) {
    std::ifstream stream(path, std::ios::binary);
    const std::vector<u8> bytes((std::istreambuf_iterator<char>(stream)), {});
    Reader input(bytes);
    const auto header = input.Take(420);
    const auto width = input.Number(4), height = input.Number(4);
    if (width == 0 || height == 0 || width > 144 || height > 144)
        throw std::runtime_error("invalid fixture dimensions");
    std::vector<mapCell> cells(width * height);
    for (auto& cell : cells) std::memcpy(&cell, input.Take(20).data(), sizeof(cell));
    const auto extraCount = input.Number(4);
    if (extraCount > map_records::ExtraCapacity) throw std::runtime_error("too many fixture cell extras");
    std::vector<mapCellExtra> extras(extraCount);
    for (auto& extra : extras) std::memcpy(&extra, input.Take(15).data(), sizeof(extra));
    input.Take(216 + (header[0] == 90 ? 216 : 432) + 1);
    const auto rumours = input.Indices(header[418]);
    const auto events = input.Indices(header[419]);
    const auto count = input.Number(4);
    if (count == 0 || count > map_records::ExtraCapacity) throw std::runtime_error("invalid fixture record count");
    std::vector<Record> records(count);
    for (std::size_t i = 1; i < count; ++i) records[i] = input.Take(input.Number(2));
    const char* error = map_records::CellDataError(cells, extras);
    if (!error) error = map_records::ExtraTableError(cells, records, rumours, events, {}, true);
    if (error) std::fprintf(stderr, "%s: %s\n", path, error);
    return error == nullptr;
}

}

int main(int argc, char** argv) {
    try {
        if (argc > 1) {
            bool valid = true;
            for (int i = 1; i < argc; ++i) valid &= Retail(argv[i]);
            if (valid) std::printf("Validated map records and cell chains in %d retail maps\n", argc - 1);
            return valid ? 0 : 1;
        }
        return Boundaries() && Tables() ? 0 : 1;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
