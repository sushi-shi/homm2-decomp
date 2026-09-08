#include <IMPORT/LegacySave.h>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>

namespace legacy_save {
namespace {

class Reader {
public:
    explicit Reader(std::span<const u8> bytes) : m_bytes(bytes) {}

    std::size_t Remaining() const { return m_bytes.size() - m_position; }
    std::span<const u8> Raw(std::size_t size) {
        if (size > Remaining()) throw savegame::FormatError("truncated legacy save");
        const auto result = m_bytes.subspan(m_position, size);
        m_position += size;
        return result;
    }

    template <typename T> requires std::is_integral_v<T>
    void Field(T& value) {
        // Windows long was always four bytes, including on a 64-bit importer.
        constexpr std::size_t size = std::min(sizeof(T), std::size_t{4});
        const auto bytes = Raw(size);
        u64 bits = 0;
        for (std::size_t i = 0; i < size; ++i) bits |= static_cast<u64>(bytes[i]) << (8 * i);
        if constexpr (std::is_signed_v<T>) {
            if (bits & (u64{1} << (size * 8 - 1))) bits |= (~u64{0}) << (size * 8);
            value = static_cast<T>(std::bit_cast<i64>(bits));
        } else {
            value = static_cast<T>(bits);
        }
    }

    void Field(float& value) {
        u32 bits{};
        Field(bits);
        value = std::bit_cast<float>(bits);
    }

    template <typename Enum, typename Storage>
    void Field(H2EnumStorage<Enum, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }
    template <typename Enum, typename Storage>
    void Field(H2SteppedEnumStorage<Enum, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }
    template <typename Code, typename Storage>
    void Field(H2OpenCodeStorage<Code, Storage>& value) {
        Storage code{};
        Field(code);
        value = code;
    }
    template <typename T, std::size_t N>
    void Field(T (&values)[N]) { for (auto& value : values) Field(value); }
    template <typename... T>
    void Fields(T&... values) { (Field(values), ...); }
    template <typename T>
    T Number() { T value{}; Field(value); return value; }

    std::string FixedText(std::size_t size) {
        const auto bytes = Raw(size);
        const auto end = std::find(bytes.begin(), bytes.end(), 0);
        if (end == bytes.end()) throw savegame::FormatError("unterminated legacy text field");
        return {reinterpret_cast<const char*>(bytes.data()), static_cast<std::size_t>(end - bytes.begin())};
    }

    std::string Text(std::size_t size, localization::TextEncoding encoding) {
        return localization::DecodeText(FixedText(size).c_str(), encoding);
    }

    void Marker() {
        if (Number<i32>() != 1234) throw savegame::FormatError("invalid legacy save section marker");
    }
    void Finish() const {
        if (Remaining() != 0) throw savegame::FormatError("unexpected data after legacy save");
    }

private:
    std::span<const u8> m_bytes;
    std::size_t m_position = 0;
};

void Army(Reader& r, armyGroup& army) {
    r.Fields(army.m_creatureTypes, army.m_creatureCounts);
}

void Hero(Reader& r, HeroState& h, bool expansion, localization::TextEncoding encoding) {
    Reader record(r.Raw(expansion ? 250 : 236));
    record.Fields(h.m_spellPoints, h.m_id, h.m_owner, h.m_lastHeroInteractionTurn,
                  h.m_lastInteractionHeroId, h.m_lastTownInteractionTurn, h.m_visitedTownId);
    h.m_name = record.Text(13, encoding);
    record.Fields(h.m_cursorType, h.m_portrait, h.m_x, h.m_y, h.m_destinationX, h.m_destinationY,
                  h.m_boatId, h.m_boatDestY, h.m_boatTravelRange, h.m_direction, h.m_locationType,
                  h.m_occupiedTown, h.m_mobility, h.m_remainingMobility, h.m_experience,
                  h.m_level, h.m_primaryStats, h.m_morale, h.m_luck);
    record.Raw(1); // Retail alignment before the visit masks.
    record.Fields(h.m_gazeboVisits, h.m_fortVisits, h.m_witchDoctorVisits, h.m_mercenaryCampVisits,
                  h.m_standingStoneVisits, h.m_treeKnowledgeVisits, h.m_xanaduVisits,
                  h.m_randomSeed, h.m_enabled);
    Army(record, h.m_army);
    record.Fields(h.m_secondarySkills, h.m_secondarySkillOrder, h.m_secondarySkillCount,
                  h.m_spells, h.m_artifacts);
    h.m_eventFlags = HeroEventFlagFromCode(record.Number<u32>());
    record.Fields(h.m_isCaptain, h.m_aiFightValue);
    if (expansion) record.Field(h.m_artifactExtra);
    record.Finish();
}

void Town(Reader& r, TownState& t, localization::TextEncoding encoding) {
    Reader record(r.Raw(100));
    record.Fields(t.m_id, t.m_owner, t.m_threat, t.m_type, t.m_x, t.m_y, t.m_boatX, t.m_boatY);
    Army(record, t.m_army);
    record.Fields(t.m_occupyingHeroId, t.m_buildings, t.m_buildState);
    record.Raw(1);
    record.Fields(t.m_garrison, t.m_onMap, t.m_mayNotUpgradeToCastle, t.m_formation,
                  t.m_originalOwner, t.m_extraIndex, t.m_spellSlots,
                  t.m_spellCounts[1], t.m_spellCounts[2], t.m_spellCounts[3],
                  t.m_spellCounts[4], t.m_spellCounts[5], t.m_turnsOwned);
    t.m_name = record.Text(13, encoding);
    record.Finish();
}

void Player(Reader& r, playerData& p, u8& cheated) {
    r.Fields(p.m_color, p.m_heroCount, p.m_currentHero, p.m_heroLocatorPage,
             p.m_heroIds, p.m_availableHeroIds);
    r.Raw(42);
    cheated |= r.Number<u8>();
    r.Field(p.m_cheatValue);
    p.m_aiDifficulty = PlayerPersonalityFromCode(r.Number<i32>());
    r.Fields(p.m_minimumHeroCount, p.m_evilInterface, p.m_ultimateArtifactHintChance,
             p.m_ultimateArtifactHintX, p.m_ultimateArtifactHintY, p.m_daysLeft,
             p.m_townCount, p.m_currentTown, p.m_townLocatorPage, p.m_townIds,
             p.m_resources, p.m_aiData.m_income);
    r.Raw(1); // Retail wrote the barrier byte twice.
    r.Fields(p.m_barrierTents, p.m_unknownad);
}

void Header(Reader& r, SMapHeader& h) {
    Reader record(r.Raw(420));
    u32 magic{};
    u16 victoryValue{}, lossValue{}, victoryY{}, lossY{}, sideThreshold{};
    record.Fields(magic, h.difficulty, h.unknown5, h.width, h.height, h.playerEnabled,
                  h.playerCanHuman, h.playerCanComputer, h.playerCount, h.minHumanPlayers,
                  h.maxHumanPlayers);
    h.victoryCondition = MapVictoryConditionFromCode(record.Number<u8>());
    record.Fields(h.computerAlsoWins, h.allowNormalVictory, victoryValue);
    h.lossCondition = MapLossConditionFromCode(record.Number<u8>());
    record.Fields(lossValue, h.unknown25, h.playerRace, victoryY, lossY, sideThreshold, h.reserved32);
    const std::string name = record.FixedText(60);
    const std::string description = record.FixedText(300);
    std::memcpy(h.name, name.c_str(), name.size() + 1);
    std::memcpy(h.description, description.c_str(), description.size() + 1);
    record.Fields(h.rumourCount, h.timeEventCount);
    record.Finish();
    h.magic = magic;
    h.victoryConditionValue = victoryValue;
    h.lossConditionValue = lossValue;
    h.victoryTownY = victoryY;
    h.lossTownY = lossY;
    h.victorySideThreshold = sideThreshold;
}

void Setup(Reader& r, savegame::Snapshot& s, localization::TextEncoding encoding) {
    Reader record(r.Raw(65));
    record.Fields(s.m_setupPlayerColor, s.m_playerHandicap, s.m_setupPlayerRace,
                  s.m_setupPlayerNetworkId, s.m_difficulty);
    s.mapFilename = record.Text(13, encoding);
    record.Fields(s.m_setupPlayerType, s.m_selectedSetupPlayer, s.m_newGameInitialized,
                  s.m_newGameHumanCount);
    record.Raw(18);
    record.Finish();
}

void Campaign(Reader& r, savegame::Snapshot& s) {
    Reader record(r.Raw(327));
    record.Fields(s.m_campaignType, s.m_campaignStartingSide, s.m_campaignScenario,
                  s.m_campaignScenarioCompleted, s.m_campaignScenarioBonus, s.m_campaignScenarioDays);
    record.Raw(1);
    record.Fields(s.m_campaignAwards, s.m_campaignChoice, s.m_campaignMapEnabled,
                  s.m_campaignScore, s.m_campaignCarryoverCreatureTypes, s.m_campaignCarryoverCreatureCounts,
                  s.m_campaignScenarioWon, s.m_campaignCheated);
    record.Raw(record.Remaining()); // 119 bytes of unused retail state.
}

void Expansion(Reader& r, savegame::ExpansionState& s) {
    Reader record(r.Raw(79));
    record.Fields(s.campaignId, s.currentMap, s.mapCount, s.mapChoices, s.mapsPlayed,
                  s.mapDays, s.awards, s.bonusChoices);
    // This old record included GUI pointers and padding. They are never restored.
    record.Raw(16);
    record.Finish();
}

void Cell(Reader& r, mapCell& c) {
    r.Fields(c.m_terrainImageIndex, c.m_objTypeBits, c.m_objectIndex, c.m_objectData);
    const u8 overlay = r.Number<u8>();
    c.m_animatedOverlay = overlay & 1u;
    c.m_drawOverlayOnTop = (overlay >> 1) & 1u;
    c.SetOverlayTileset(TilesetIdFromCode(overlay >> 2));
    r.Fields(c.m_overlayIndex, c.m_flags, c.m_triggerType, c.m_extraIndex);
}

void Overlay(Reader& r, mapCellExtra& c) {
    c.nextIndex = r.Number<u16>();
    const u8 object = r.Number<u8>();
    r.Field(c.objectIndex);
    const u8 metadata = r.Number<u8>();
    const u8 overlay = r.Number<u8>();
    r.Field(c.overlayIndex);
    c.animatedObject = object & 1u;
    c.SetObjectTileset(TilesetIdFromCode(object >> 1));
    c.objectLayerBit0 = metadata & 1u;
    c.objectLayerBit1 = (metadata >> 1) & 1u;
    c.objectDrawnAsOverlay = (metadata >> 2) & 1u;
    const u8 objectMetadata = metadata >> 3;
    c.objectMetadata = objectMetadata & 0x1f;
    c.animatedOverlay = overlay & 1u;
    c.drawOverlayOnTop = (overlay >> 1) & 1u;
    c.SetOverlayTileset(TilesetIdFromCode(overlay >> 2));
}

template <std::size_t N>
void EventIndices(Reader& r, u16& count, u16 (&indices)[N]) {
    r.Field(count);
    r.Raw(2); // The old four-byte header duplicated the first index.
    if (count > N) throw savegame::FormatError("legacy event count exceeds its table");
    for (u16 i = 0; i < count; ++i) r.Field(indices[i]);
}

void Payload(Reader& r, savegame::Snapshot& s, localization::TextEncoding encoding) {
    s.resourceEncoding = encoding;
    s.width = r.Number<i32>();
    const bool expansion = s.width == -1;
    if (expansion) r.Field(s.width);
    r.Field(s.height);
    if (s.width <= 0 || s.width > 144 || s.height <= 0 || s.height > 144)
        throw savegame::FormatError("invalid legacy map dimensions");
    Header(r, s.header);
    Setup(r, s, encoding);
    s.greatest = r.Number<u8>() != 0;
    r.Field(s.m_difficultyRating);
    s.monthType = r.Number<i8>();
    s.monthExtra = r.Number<i8>();
    s.weekType = r.Number<i8>();
    s.weekExtra = r.Number<i8>();
    for (auto& name : s.playerNames) name = r.Text(21, encoding);
    r.Raw(36);
    const i32 kind = r.Number<i32>();
    if (kind == 1) {
        s.kind = savegame::Kind::Campaign;
        Campaign(r, s);
    } else if (kind == 2) {
        s.kind = savegame::Kind::ExpansionCampaign;
        Expansion(r, s.expansion);
    } else if (kind != 0) {
        throw savegame::FormatError("unknown legacy campaign type");
    }
    if (expansion) s.expansionMap = r.Number<u8>() != 0;
    r.Field(s.mapChangeSequence);
    r.Raw(14);
    r.Field(s.m_playerCount);
    s.currentPlayer = r.Number<i8>();
    r.Fields(s.m_deadPlayerCount, s.m_playerDead);
    for (bool& human : s.human) human = r.Number<u8>() != 0;
    r.Fields(s.m_day, s.m_week, s.m_month);
    for (auto& player : s.players) Player(r, player, s.m_cheated);
    r.Field(s.m_obeliskCount);
    for (auto& hero : s.heroes) Hero(r, hero, expansion, encoding);
    r.Field(s.m_availableHeroes);
    for (auto& town : s.towns) Town(r, town, encoding);
    r.Fields(s.m_castleOwners, s.m_dailyEventFlags);
    for (auto& mine : s.mines)
        r.Fields(mine.id, mine.owner, mine.resourceType, mine.guardianType, mine.guardianCount, mine.x, mine.y);
    r.Field(s.m_mineOwners);
    const std::size_t artifacts = expansion ? std::size(s.m_randomArtifacts)
        : static_cast<std::size_t>(H2EnumIndex(ARTIFACT_MAGIC_BOOK) + 1);
    for (std::size_t i = 0; i < artifacts; ++i) r.Field(s.m_randomArtifacts[i]);
    for (auto& boat : s.boats)
        r.Fields(boat.id, boat.x, boat.y, boat.direction, boat.savedTriggerType, boat.savedEventData, boat.heroId, boat.owner);
    r.Fields(s.m_boatSlots, s.m_obeliskVisitors, s.m_ultimateArtifactX, s.m_ultimateArtifactY, s.m_ultimateArtifactId);
    s.rumour = r.Text(301, encoding);
    for (auto& id : s.playerSystemIds) id = r.FixedText(4);
    EventIndices(r, s.m_rumourEventCount, s.m_rumourEventIndices);
    EventIndices(r, s.m_timeEventCount, s.m_timeEventIndices);
    EventIndices(r, s.m_mapEventCount, s.m_mapEventIndices);
    r.Marker();
    const i32 events = r.Number<i32>();
    if (events < 1 || events > 65536) throw savegame::FormatError("invalid legacy map event table size");
    r.Marker();
    s.events.resize(static_cast<std::size_t>(events));
    for (i32 i = 1; i < events; ++i) {
        r.Marker();
        const i16 size = r.Number<i16>();
        if (size < 0) throw savegame::FormatError("negative legacy map event size");
        const auto bytes = r.Raw(static_cast<std::size_t>(size));
        s.events[i].assign(bytes.begin(), bytes.end());
    }
    r.Marker();
    const std::size_t cells = static_cast<std::size_t>(s.width) * static_cast<std::size_t>(s.height);
    const auto fog = r.Raw(cells);
    s.fog.assign(fog.begin(), fog.end());
    r.Marker();
    const i32 mapWidth = r.Number<i32>();
    const i32 mapHeight = r.Number<i32>();
    if (mapWidth != s.width || mapHeight != s.height)
        throw savegame::FormatError("legacy map dimensions disagree");
    s.cells.resize(cells);
    for (auto& cell : s.cells) Cell(r, cell);
    const i32 overlays = r.Number<i32>();
    if (overlays < 0 || overlays > 65536) throw savegame::FormatError("invalid legacy map overlay table size");
    s.overlays.resize(static_cast<std::size_t>(overlays));
    for (auto& overlay : s.overlays) Overlay(r, overlay);
    r.Marker();
    r.Finish();
}

}

bool Decode(std::span<const u8> bytes, localization::TextEncoding encoding,
            savegame::Snapshot& state, std::string& error) {
    try {
        if (bytes.size() > savegame::MaximumFileBytes) throw savegame::FormatError("legacy save exceeds the size limit");
        Reader reader(bytes);
        savegame::Snapshot imported;
        Payload(reader, imported, encoding);
        if (!savegame::Validate(imported, error)) return false;
        state = std::move(imported);
        return true;
    } catch (const savegame::FormatError& failure) {
        error = failure.what();
        return false;
    }
}

}
