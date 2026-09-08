#include <SAVE/Snapshot.h>
#include <SOURCE/EVENTS.h>

#include <algorithm>
#include <cmath>

namespace savegame {

bool Validate(const Snapshot& s, std::string& error) {
    const auto fail = [&](const char* message) { error = message; return false; };
    if (s.width <= 0 || s.height <= 0 || s.width > 144 || s.height > 144)
        return fail("invalid map dimensions");
    const std::size_t cells = static_cast<std::size_t>(s.width) * static_cast<std::size_t>(s.height);
    if (s.cells.size() != cells || s.fog.size() != cells)
        return fail("map dimensions do not match its cell data");
    if (s.m_playerCount <= 0 || s.m_playerCount > GAME_PLAYER_COUNT
        || s.currentPlayer < 0 || s.currentPlayer >= s.m_playerCount)
        return fail("invalid current player");
    if (s.m_deadPlayerCount < 0 || s.m_deadPlayerCount > s.m_playerCount
        || s.m_day < 1 || s.m_day > 7 || s.m_week < 1 || s.m_week > 4 || s.m_month < 1)
        return fail("invalid turn state");
    if (s.events.empty() || s.events.size() > 65536 || s.overlays.size() > 65536)
        return fail("invalid map record count");
    for (const auto& event : s.events) {
        if (event.size() > 32767) return fail("map event exceeds the runtime record limit");
    }
    const auto terminatedEvent = [&](std::size_t index, std::size_t textOffset) {
        if (index == 0 || index >= s.events.size()) return false;
        const auto& event = s.events[index];
        return event.size() > textOffset
            && std::find(event.data() + textOffset, event.data() + event.size(), 0)
                != event.data() + event.size();
    };
    const auto validEvents = [&](const auto& indices, std::size_t count, std::size_t textOffset) {
        if (count > std::size(indices)) return false;
        for (std::size_t index = 0; index < count; ++index) {
            if (!terminatedEvent(indices[index], textOffset)) return false;
        }
        return true;
    };
    if (!validEvents(s.m_rumourEventIndices, s.m_rumourEventCount, offsetof(rumourEventExtra, text))
        || !validEvents(s.m_timeEventIndices, s.m_timeEventCount, offsetof(timeEventExtra, message))
        || !validEvents(s.m_mapEventIndices, s.m_mapEventCount, offsetof(EventExtra, message)))
        return fail("map event is missing or has invalid text bounds");
    // Overlay chains are traversed until index zero; a cycle would otherwise
    // hang drawing/pathfinding. Only live chains matter: unused allocator
    // slots may retain links to other unused slots.
    std::vector<u8> visited(s.overlays.size());
    for (const mapCell& cell : s.cells) {
        if ((cell.m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SIGN)
             || cell.m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_BOTTLE))
            && cell.m_objectMetadata != 0
            && !terminatedEvent(cell.m_objectMetadata, offsetof(signEventExtra, text)))
            return fail("sign or bottle event has invalid text bounds");
        if (cell.m_triggerType == MAP_ACTION_TRIGGER(MAP_OBJECT_SPHINX)) {
            if (!terminatedEvent(cell.m_objectMetadata, offsetof(mapEventExtra, riddle)))
                return fail("Sphinx event has invalid text bounds");
            const auto& event = s.events[cell.m_objectMetadata];
            if (event[offsetof(mapEventExtra, answerCount)] > EVENT_RECORD_MAP_ANSWER_COUNT)
                return fail("Sphinx answer count exceeds its table");
            for (std::size_t i = 0; i < event[offsetof(mapEventExtra, answerCount)]; ++i) {
                const auto first = event.data() + offsetof(mapEventExtra, answers) + i * EVENT_RECORD_MAP_ANSWER_SIZE;
                if (std::find(first, first + EVENT_RECORD_MAP_ANSWER_SIZE, 0) == first + EVENT_RECORD_MAP_ANSWER_SIZE)
                    return fail("unterminated Sphinx answer");
            }
        }
        const std::size_t first = cell.m_extraIndex;
        std::size_t index = first;
        while (index != 0) {
            if (index >= s.overlays.size()) return fail("map overlay references a missing record");
            if (visited[index] == 1) return fail("cyclic map overlay chain");
            if (visited[index] == 2) break;
            if (s.overlays[index].nextIndex == MAPCELL_EXTRA_FREE)
                return fail("map overlay references a free record");
            visited[index] = 1;
            index = s.overlays[index].nextIndex;
        }
        index = first;
        while (index != 0 && visited[index] == 1) {
            visited[index] = 2;
            index = s.overlays[index].nextIndex;
        }
    }
    for (const playerData& p : s.players) {
        if (p.m_heroCount < 0 || p.m_heroCount > PLAYER_HERO_CAPACITY
            || p.m_townCount < 0 || p.m_townCount > GAME_TOWN_COUNT
            || p.m_currentHero < -1 || p.m_currentHero >= GAME_HERO_COUNT
            || p.m_currentTown < -1 || p.m_currentTown >= GAME_TOWN_COUNT
            || p.m_color < 0 || p.m_color >= GAME_PLAYER_COUNT)
            return fail("invalid player army or town count");
        for (i32 index = 0; index < p.m_heroCount; ++index) {
            if (p.m_heroIds[index] < 0 || p.m_heroIds[index] >= GAME_HERO_COUNT)
                return fail("invalid player hero index");
        }
        for (i32 index = 0; index < p.m_townCount; ++index) {
            if (p.m_townIds[index] < 0 || p.m_townIds[index] >= GAME_TOWN_COUNT)
                return fail("invalid player town index");
        }
    }
    for (const HeroState& h : s.heroes) {
        if (h.m_owner < -1 || h.m_owner >= GAME_PLAYER_COUNT
            || h.m_secondarySkillCount < 0 || h.m_secondarySkillCount > HERO_SECONDARY_SKILL_CAPACITY
            || (h.m_owner >= 0 && (h.m_x < 0 || h.m_y < 0 || h.m_x >= s.width || h.m_y >= s.height))
            || !std::isfinite(h.m_aiFightValue))
            return fail("invalid hero state");
        for (const auto& skill : h.m_secondarySkills)
            if (H2EnumIndex(skill) < H2EnumIndex(HERO_SKILL_LEVEL_NONE)
                || H2EnumIndex(skill) >= H2EnumIndex(HERO_SKILL_LEVEL_COUNT))
                return fail("invalid hero secondary skill level");
        for (const auto& artifact : h.m_artifacts)
            if (H2EnumIndex(artifact) < H2EnumIndex(ARTIFACT_NONE)
                || H2EnumIndex(artifact) >= H2EnumIndex(ARTIFACT_COUNT))
                return fail("invalid hero artifact");
    }
    for (const TownState& t : s.towns) {
        if (t.m_owner < -1 || t.m_owner >= GAME_PLAYER_COUNT
            || (t.m_onMap && (t.m_x >= s.width || t.m_y >= s.height)))
            return fail("invalid town state");
    }
    if (s.kind == Kind::ExpansionCampaign
        && (s.expansion.campaignId < 0 || s.expansion.campaignId >= H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)
            || s.expansion.mapCount < 1 || s.expansion.mapCount > EXPANSION_CAMPAIGN_MAX_MAP_COUNT
            || s.expansion.currentMap < 0 || s.expansion.currentMap >= s.expansion.mapCount))
        return fail("invalid expansion campaign state");
    return true;
}

}
