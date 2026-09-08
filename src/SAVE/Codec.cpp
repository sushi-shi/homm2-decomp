#include <SAVE/Snapshot.h>
#include <SAVE/Combat.h>

#include <algorithm>
#include <cmath>

namespace savegame {
namespace {

template <typename Archive, typename T>
void Army(Archive& archive, T& army) {
    archive.Fields(army.m_creatureTypes, army.m_creatureCounts);
}

template <typename Archive, typename T>
void Hero(Archive& a, T& h) {
    a.Fields(h.m_spellPoints, h.m_id, h.m_owner, h.m_lastHeroInteractionTurn,
             h.m_lastInteractionHeroId, h.m_lastTownInteractionTurn, h.m_visitedTownId);
    a.Text(h.m_name);
    a.Fields(h.m_cursorType, h.m_portrait, h.m_x, h.m_y, h.m_destinationX, h.m_destinationY,
             h.m_boatId, h.m_boatDestY, h.m_boatTravelRange, h.m_direction, h.m_locationType,
             h.m_occupiedTown, h.m_mobility, h.m_remainingMobility, h.m_experience, h.m_level,
             h.m_primaryStats, h.m_morale, h.m_luck, h.m_gazeboVisits, h.m_fortVisits,
             h.m_witchDoctorVisits, h.m_mercenaryCampVisits, h.m_standingStoneVisits,
             h.m_treeKnowledgeVisits, h.m_xanaduVisits, h.m_randomSeed, h.m_enabled);
    Army(a, h.m_army);
    a.Fields(h.m_secondarySkills, h.m_secondarySkillOrder, h.m_secondarySkillCount,
             h.m_spells, h.m_artifacts);
    a.EnumField(h.m_eventFlags, [](u32 code) { return HeroEventFlagFromCode(code); });
    a.Fields(h.m_isCaptain, h.m_aiFightValue, h.m_artifactExtra);
}

template <typename Archive, typename T>
void Town(Archive& a, T& t) {
    a.Fields(t.m_id, t.m_owner, t.m_threat, t.m_type, t.m_x, t.m_y, t.m_boatX, t.m_boatY);
    Army(a, t.m_army);
    a.Fields(t.m_occupyingHeroId, t.m_buildings, t.m_buildState, t.m_garrison,
             t.m_onMap, t.m_mayNotUpgradeToCastle, t.m_formation, t.m_originalOwner, t.m_extraIndex);
    // The last spell slot overlaps the first spell-count byte in the runtime
    // union. Persist the complete logical backing store exactly once.
    a.Fields(t.m_spellSlots, t.m_spellCounts[1], t.m_spellCounts[2], t.m_spellCounts[3],
             t.m_spellCounts[4], t.m_spellCounts[5], t.m_turnsOwned);
    a.Text(t.m_name);
}

template <typename Archive, typename T>
void Player(Archive& a, T& p) {
#define SAVE_PLAYER_FIELD(name) a.Field(p.name);
#include <SAVE/PlayerFields.def>
#undef SAVE_PLAYER_FIELD
    a.EnumField(p.m_aiDifficulty, [](i32 code) { return PlayerPersonalityFromCode(code); });
}

template <typename Archive, typename T>
void Mine(Archive& a, T& mine) {
    a.Fields(mine.id, mine.owner, mine.resourceType, mine.guardianType, mine.guardianCount, mine.x, mine.y);
}

template <typename Archive, typename T>
void Boat(Archive& a, T& boat) {
    a.Fields(boat.id, boat.x, boat.y, boat.direction, boat.savedTriggerType,
             boat.savedEventData, boat.heroId, boat.owner);
}

void SetCellOverlay(mapCell& cell, u8 overlay) {
    cell.m_animatedOverlay = overlay & 1u;
    cell.m_drawOverlayOnTop = (overlay >> 1) & 1u;
    cell.SetOverlayTileset(TilesetIdFromCode(overlay >> 2));
}

void SetExtraBytes(mapCellExtra& cell, u8 object, u8 metadata, u8 overlay) {
    cell.animatedObject = object & 1u;
    cell.SetObjectTileset(TilesetIdFromCode(object >> 1));
    cell.objectLayerBit0 = metadata & 1u;
    cell.objectLayerBit1 = (metadata >> 1) & 1u;
    cell.objectDrawnAsOverlay = (metadata >> 2) & 1u;
    const u8 objectMetadata = metadata >> 3;
    cell.objectMetadata = objectMetadata & 0x1f;
    cell.animatedOverlay = overlay & 1u;
    cell.drawOverlayOnTop = (overlay >> 1) & 1u;
    cell.SetOverlayTileset(TilesetIdFromCode(overlay >> 2));
}

template <typename Archive, typename T>
void Cell(Archive& a, T& cell) {
    a.Fields(cell.m_terrainImageIndex, cell.m_objTypeBits, cell.m_objectIndex, cell.m_objectData);
    u8 overlay = static_cast<u8>(cell.m_animatedOverlay | (cell.m_drawOverlayOnTop << 1)
                                 | (cell.m_overlayTilesetBits << 2));
    a.Field(overlay);
    if constexpr (Archive::Reading) SetCellOverlay(cell, overlay);
    a.Fields(cell.m_overlayIndex, cell.m_flags, cell.m_triggerType, cell.m_extraIndex);
}

template <typename Archive, typename T>
void Overlay(Archive& a, T& cell) {
    u16 next = cell.nextIndex;
    a.Field(next);
    if constexpr (Archive::Reading) cell.nextIndex = next;
    u8 object = static_cast<u8>(cell.animatedObject | (cell.objectTilesetBits << 1));
    u8 metadata = static_cast<u8>(cell.objectLayerBit0 | (cell.objectLayerBit1 << 1)
                                  | (cell.objectDrawnAsOverlay << 2) | (cell.objectMetadata << 3));
    u8 overlay = static_cast<u8>(cell.animatedOverlay | (cell.drawOverlayOnTop << 1)
                                 | (cell.overlayTilesetBits << 2));
    a.Fields(object, cell.objectIndex, metadata, overlay, cell.overlayIndex);
    if constexpr (Archive::Reading) SetExtraBytes(cell, object, metadata, overlay);
}

template <typename Archive, typename T>
void ResourceText(Archive& a, T& field, localization::TextEncoding encoding) {
    if constexpr (Archive::Reading) {
        std::string text;
        a.Text(text);
        if (!localization::EncodeText(text.c_str(), encoding, field, sizeof(field)))
            throw FormatError("map header text does not fit its source resource record");
    } else {
        const char* end = static_cast<const char*>(std::memchr(field, 0, sizeof(field)));
        if (end == nullptr) throw FormatError("unterminated map header text");
        a.Text(localization::DecodeText(std::string(field, end).c_str(), encoding));
    }
}

template <typename Archive, typename T>
void MapHeader(Archive& a, T& h, localization::TextEncoding encoding) {
    // The resource header is packed. Marshal multibyte fields through aligned
    // locals instead of binding references to unaligned members.
    u32 magic = h.magic;
    u16 victoryValue = h.victoryConditionValue;
    u16 lossValue = h.lossConditionValue;
    u16 victoryY = h.victoryTownY;
    u16 lossY = h.lossTownY;
    u16 sideThreshold = h.victorySideThreshold;
    a.Fields(magic, h.difficulty, h.unknown5, h.width, h.height, h.playerEnabled,
             h.playerCanHuman, h.playerCanComputer, h.playerCount, h.minHumanPlayers,
             h.maxHumanPlayers);
    a.EnumField(h.victoryCondition, [](u8 code) { return MapVictoryConditionFromCode(code); });
    a.Fields(h.computerAlsoWins, h.allowNormalVictory, victoryValue);
    a.EnumField(h.lossCondition, [](u8 code) { return MapLossConditionFromCode(code); });
    a.Fields(lossValue, h.unknown25, h.playerRace, victoryY, lossY, sideThreshold, h.reserved32);
    ResourceText(a, h.name, encoding);
    ResourceText(a, h.description, encoding);
    a.Fields(h.rumourCount, h.timeEventCount);
    if constexpr (Archive::Reading) {
        h.magic = magic;
        h.victoryConditionValue = victoryValue;
        h.lossConditionValue = lossValue;
        h.victoryTownY = victoryY;
        h.lossTownY = lossY;
        h.victorySideThreshold = sideThreshold;
    }
}

template <typename Archive, typename T>
void Expansion(Archive& a, T& c) {
    a.Fields(c.campaignId, c.currentMap, c.mapCount, c.mapChoices, c.mapsPlayed,
             c.mapDays, c.awards, c.bonusChoices);
}

template <typename Archive, typename Vector, typename Element>
void Records(Archive& a, Vector& records, std::size_t limit, Element element) {
    u32 count = static_cast<u32>(records.size());
    a.Field(count);
    if (count > limit) throw FormatError("save record count exceeds its limit");
    if constexpr (Archive::Reading) records.resize(count);
    for (auto& record : records) element(a, record);
}

void Blob(Writer& a, const std::vector<u8>& blob) {
    a.Word(static_cast<u32>(blob.size()));
    a.Raw(blob);
}

void Blob(Reader& a, std::vector<u8>& blob) {
    const u32 count = a.Word();
    const auto bytes = a.Raw(count);
    blob.assign(bytes.begin(), bytes.end());
}

template <typename Archive, typename T>
void Payload(Archive& a, T& s) {
    u32 kind = static_cast<u32>(s.kind);
    i32 encoding = static_cast<i32>(s.resourceEncoding);
    a.Fields(kind, s.expansionMap, s.greatest, s.currentPlayer, s.monthType, s.monthExtra,
             s.weekType, s.weekExtra, s.mapChangeSequence, s.human, encoding);
    if (kind > 2 || encoding < 0 || encoding > 2) throw FormatError("unknown save kind or resource encoding");
    if constexpr (Archive::Reading) {
        s.kind = kind == 0 ? Kind::Scenario : kind == 1 ? Kind::Campaign : Kind::ExpansionCampaign;
        s.resourceEncoding = encoding == 0 ? localization::TextEncoding::Utf8
            : encoding == 1 ? localization::TextEncoding::Windows1252 : localization::TextEncoding::Windows1251;
    }
    for (auto& name : s.playerNames) a.Text(name);
    for (auto& id : s.playerSystemIds) a.Text(id);
    a.Text(s.mapFilename);
    a.Text(s.rumour);
    MapHeader(a, s.header, s.resourceEncoding);
    Expansion(a, s.expansion);
#define SAVE_GAME_FIELD(name) a.Field(s.name);
#include <SAVE/GameFields.def>
#undef SAVE_GAME_FIELD
    for (auto& player : s.players) Player(a, player);
    for (auto& hero : s.heroes) Hero(a, hero);
    for (auto& town : s.towns) Town(a, town);
    for (auto& mine : s.mines) Mine(a, mine);
    for (auto& boat : s.boats) Boat(a, boat);
    a.Fields(s.width, s.height);
    Records(a, s.cells, 144 * 144, [](auto& ar, auto& cell) { Cell(ar, cell); });
    Records(a, s.overlays, 65536, [](auto& ar, auto& cell) { Overlay(ar, cell); });
    Blob(a, s.fog);
    Records(a, s.events, 65536, [](auto& ar, auto& bytes) { Blob(ar, bytes); });
}

}

bool Encode(const Snapshot& state, std::vector<u8>& file, std::string& error) {
    if (!Validate(state, error)) return false;
    try {
        Writer writer;
        Payload(writer, state);
        file = WrapPayload(writer.bytes);
        return true;
    } catch (const FormatError& failure) {
        error = failure.what();
        return false;
    }
}

bool Decode(std::span<const u8> file, Snapshot& state, std::string& error) {
    try {
        Reader reader(UnwrapPayload(file));
        Snapshot parsed;
        Payload(reader, parsed);
        reader.Finish();
        if (!Validate(parsed, error)) return false;
        state = std::move(parsed);
        return true;
    } catch (const FormatError& failure) {
        error = failure.what();
        return false;
    }
}

namespace {

template <typename Archive, typename Optional, typename Codec>
void OptionalRecord(Archive& archive, Optional& record, Codec codec) {
    bool present = record.has_value();
    archive.Field(present);
    if constexpr (Archive::Reading) {
        if (present) record.emplace();
        else record.reset();
    }
    if (present) codec(archive, *record);
}

template <typename Archive, typename T>
void CombatPayload(Archive& archive, T& state) {
    u32 tag = 0x314d4243; // CBM1 distinguishes combat from a saved game.
    archive.Field(tag);
    if (tag != 0x314d4243) throw FormatError("unknown combat state version");
    archive.Fields(state.x, state.y, state.setupX, state.setupY, state.randomSeed,
                   state.result, state.retreat, state.surrender, state.firstGold, state.secondGold);
    OptionalRecord(archive, state.firstHero, [](auto& a, auto& h) { Hero(a, h); });
    OptionalRecord(archive, state.secondHero, [](auto& a, auto& h) { Hero(a, h); });
    OptionalRecord(archive, state.town, [](auto& a, auto& t) { Town(a, t); });
    OptionalRecord(archive, state.firstArmy, [](auto& a, auto& army) { Army(a, army); });
    OptionalRecord(archive, state.secondArmy, [](auto& a, auto& army) { Army(a, army); });
}

bool ValidCombat(const CombatState& state, std::string& error) {
    const auto heroValid = [](const auto& h) {
        return !h || (h->m_owner >= -1 && h->m_owner < GAME_PLAYER_COUNT
            && h->m_secondarySkillCount >= 0 && h->m_secondarySkillCount <= HERO_SECONDARY_SKILL_CAPACITY
            && std::isfinite(h->m_aiFightValue));
    };
    if (state.x < 0 || state.x >= 144 || state.y < 0 || state.y >= 144
        || state.setupX < -1 || state.setupX >= 144 || state.setupY < -1 || state.setupY >= 144
        || !heroValid(state.firstHero) || !heroValid(state.secondHero)
        || (state.town && (state.town->m_owner < -1 || state.town->m_owner >= GAME_PLAYER_COUNT))) {
        error = "invalid combat state";
        return false;
    }
    return true;
}

}

bool EncodeCombat(const CombatState& state, std::vector<u8>& bytes, std::string& error) {
    if (!ValidCombat(state, error)) return false;
    try {
        Writer writer;
        CombatPayload(writer, state);
        if (writer.bytes.size() > MaximumCombatBytes - 20)
            throw FormatError("combat state exceeds the size limit");
        bytes = WrapPayload(writer.bytes);
        return true;
    } catch (const FormatError& failure) {
        error = failure.what();
        return false;
    }
}

bool DecodeCombat(std::span<const u8> bytes, CombatState& state, std::string& error) {
    try {
        if (bytes.size() > MaximumCombatBytes) throw FormatError("combat state exceeds the size limit");
        Reader reader(UnwrapPayload(bytes));
        CombatState decoded;
        CombatPayload(reader, decoded);
        reader.Finish();
        if (!ValidCombat(decoded, error)) return false;
        state = std::move(decoded);
        return true;
    } catch (const FormatError& failure) {
        error = failure.what();
        return false;
    }
}

}
