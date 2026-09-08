#include <SOURCE/MapRecords.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>

#include <algorithm>
#include <cstring>
#include <vector>

namespace map_records {
namespace {

static_assert(sizeof(mapHeroExtra) == 62 && sizeof(mapTownExtra) == 41);
static_assert(offsetof(mapEventExtra, riddle) == 136);
static_assert(offsetof(timeEventExtra, message) == 49);
static_assert(offsetof(EventExtra, message) == 49);

bool Terminated(Record record, std::size_t offset, std::size_t capacity) {
    return offset < record.size() && capacity <= record.size() - offset
        && std::memchr(record.data() + offset, 0, capacity) != nullptr;
}

template<class T>
T Copy(Record bytes) {
    T record{};
    std::memcpy(&record, bytes.data(), sizeof(record));
    return record;
}

template<class T>
bool ArmyValid(const T& record) {
    for (std::size_t i = 0; i < EVENT_RECORD_ARMY_SLOT_COUNT; ++i) {
        const i32 creature = H2EnumIndex(record.troopTypes[i]);
        if (record.troopCounts[i] > 0 && record.troopCounts[i] <= 32767
            && (creature < 0 || creature >= H2EnumIndex(CREATURE_COUNT)))
            return false;
    }
    return true;
}

const char* HeroError(Record bytes, bool jail) {
    if (bytes.size() < sizeof(mapHeroExtra)) return "short hero extra";
    const auto hero = Copy<mapHeroExtra>(bytes);
    if (hero.hasCustomHero && hero.heroId < 0) return "negative custom hero index";
    if (hero.hasCustomArmy && !ArmyValid(hero)) return "invalid hero creature index";
    if (jail && (H2EnumIndex(hero.heroClass) < 0
                 || H2EnumIndex(hero.heroClass) >= H2EnumIndex(FACTION_COUNT)))
        return "invalid jailed hero faction";
    if (hero.hasCustomName
        && !Terminated(bytes, offsetof(mapHeroExtra, name), sizeof(hero.name)))
        return "unterminated hero extra name";
    for (std::size_t i = 0; i < EVENT_RECORD_HERO_ARTIFACT_COUNT; ++i) {
        if (hero.artifacts[i] >= H2EnumIndex(ARTIFACT_COUNT))
            return "invalid hero artifact index";
    }
    if (hero.hasCustomSkills) {
        for (std::size_t i = 0; i < EVENT_RECORD_SKILL_CAPACITY; ++i) {
            if (hero.skillTypes[i] == -1) continue;
            if (hero.skillTypes[i] < 0 || hero.skillTypes[i] >= H2EnumIndex(HERO_SKILL_COUNT)
                || hero.skillLevels[i] < H2EnumIndex(HERO_SKILL_LEVEL_BASIC)
                || hero.skillLevels[i] > H2EnumIndex(HERO_SKILL_LEVEL_EXPERT))
                return "invalid hero secondary skill";
        }
    }
    return nullptr;
}

const char* TownError(Record bytes) {
    if (bytes.size() < sizeof(mapTownExtra)) return "short town extra";
    const auto town = Copy<mapTownExtra>(bytes);
    if (town.color < -1 || town.color >= GAME_PLAYER_COUNT) return "invalid town color";
    if (town.hasCustomBuildings && (town.mageGuildLevel < 0 || town.mageGuildLevel > 5))
        return "invalid town mage guild level";
    if (town.hasCustomArmy && !ArmyValid(town)) return "invalid town creature index";
    if (!Terminated(bytes, offsetof(mapTownExtra, name), sizeof(town.name)))
        return "unterminated town extra name";
    return nullptr;
}

}

std::string_view Text(Record record, std::size_t offset) {
    if (offset >= record.size()) return {};
    const std::string_view text(reinterpret_cast<const char*>(record.data() + offset),
                                record.size() - offset);
    return text.substr(0, text.find('\0'));
}

const char* RecordError(Record bytes, Kind kind) {
    std::size_t textOffset = 0;
    switch (kind) {
    case Kind::Hero: return HeroError(bytes, false);
    case Kind::Jail: return HeroError(bytes, true);
    case Kind::Town: return TownError(bytes);
    case Kind::Sign: textOffset = offsetof(signEventExtra, text); break;
    case Kind::Rumour: textOffset = offsetof(rumourEventExtra, text); break;
    case Kind::Sphinx: textOffset = offsetof(mapEventExtra, riddle); break;
    case Kind::TimeEvent: textOffset = offsetof(timeEventExtra, message); break;
    case Kind::MapEvent: textOffset = offsetof(EventExtra, message); break;
    }
    if (bytes.size() <= textOffset) return "short map text extra";
    if (!Terminated(bytes, textOffset, bytes.size() - textOffset))
        return "unterminated map extra text";
    if (kind == Kind::Sphinx) {
        const auto sphinx = Copy<mapEventExtra>(bytes);
        if (sphinx.answerCount > EVENT_RECORD_MAP_ANSWER_COUNT)
            return "too many sphinx answers";
        for (std::size_t i = 0; i < sphinx.answerCount; ++i) {
            if (!Terminated(bytes, offsetof(mapEventExtra, answers)
                            + i * EVENT_RECORD_MAP_ANSWER_SIZE, EVENT_RECORD_MAP_ANSWER_SIZE))
                return "unterminated sphinx answer";
        }
        if (sphinx.artifact < -1 || sphinx.artifact >= H2EnumIndex(ARTIFACT_COUNT))
            return "invalid sphinx artifact index";
    } else if (kind == Kind::MapEvent) {
        const auto event = Copy<EventExtra>(bytes);
        if (event.artifact < -1 || event.artifact >= H2EnumIndex(ARTIFACT_COUNT))
            return "invalid map event artifact index";
    }
    return nullptr;
}

const char* CellDataError(std::span<const mapCell> cells,
                          std::span<const mapCellExtra> extras) {
    if (extras.size() > ExtraCapacity) return "too many map cell extras";
    // Mark each reachable chain once. Free/unreferenced editor slots are allowed.
    std::vector<u8> state(extras.size(), 0);
    for (const auto& cell : cells) {
        std::size_t index = cell.m_extraIndex;
        while (index != 0) {
            if (index >= extras.size() || index == MAPCELL_EXTRA_FREE)
                return "map cell extra index outside table";
            if (state[index] == 2) break;
            if (state[index] == 1) return "cyclic map cell extra chain";
            state[index] = 1;
            index = extras[index].nextIndex;
        }
        index = cell.m_extraIndex;
        while (index != 0 && state[index] == 1) {
            state[index] = 2;
            index = extras[index].nextIndex;
        }
    }
    return nullptr;
}

const char* ExtraTableError(std::span<const mapCell> cells,
                            std::span<const Record> records,
                            std::span<const u16> rumours,
                            std::span<const u16> timeEvents,
                            std::span<const u16> mapEvents, bool newMap) {
    if (records.empty() || records.size() > ExtraCapacity) return "invalid map extra count";
    if (rumours.size() > GAME_RUMOUR_EVENT_CAPACITY
        || timeEvents.size() > GAME_TIME_EVENT_CAPACITY
        || mapEvents.size() > GAME_MAP_EVENT_CAPACITY) return "too many map events";
    const auto check = [&](std::size_t index, Kind kind) -> const char* {
        if (index == 0 || index >= records.size()) return "map extra index outside table";
        return RecordError(records[index], kind);
    };
    for (u16 index : rumours) if (const char* error = check(index, Kind::Rumour)) return error;
    for (u16 index : timeEvents) if (const char* error = check(index, Kind::TimeEvent)) return error;
    for (u16 index : mapEvents) if (const char* error = check(index, Kind::MapEvent)) return error;
    std::size_t eventCount = 0;
    std::vector<bool> consumed(records.size(), false);
    for (const auto& cell : cells) {
        const auto trigger = cell.m_triggerType;
        const auto type = trigger & MAP_TRIGGER_TYPE_MASK;
        const auto index = cell.m_objectMetadata;
        Kind kind;
        if (trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN)
            || trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE)) {
            if (index == 0) continue; // Empty signs display a stock message.
            kind = Kind::Sign;
        } else if (trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX)) {
            kind = Kind::Sphinx;
        } else if (newMap && type == MAP_OBJECT_HERO) {
            kind = Kind::Hero;
            if (cell.m_objectIndex >= GAME_PLAYER_COUNT * 7) return "invalid hero map sprite";
        } else if (newMap && trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_JAIL)) {
            kind = Kind::Jail;
        } else if (newMap && trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_MAP_EVENT)) {
            kind = Kind::MapEvent;
            if (++eventCount > GAME_MAP_EVENT_CAPACITY) return "too many map event cells";
        } else if (newMap && (trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_CASTLE)
                            || trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_TOWN)
                            || trigger == MAP_ACTION_TRIGGER(MAP_OBJECT_RANDOM_CASTLE))) {
            kind = Kind::Town;
        } else {
            continue;
        }
        if (const char* error = check(index, kind)) return error;
        if (kind == Kind::Town || kind == Kind::Hero || kind == Kind::Jail) {
            if (consumed[index]) return "shared consumed map extra";
            consumed[index] = true;
        }
    }
    return nullptr;
}

}
