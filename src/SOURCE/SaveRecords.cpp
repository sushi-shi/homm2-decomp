#include <SOURCE/SaveRecords.h>
#include <SOURCE/game.h>
#include <SOURCE/ExpCampaign.h>

#include <bit>
#include <limits>
#include <tuple>
#include <type_traits>

namespace save_records {
namespace {
// The scalar type is part of the file contract, including enum storage widths.
template<class T, class = void> struct Scalar {
    using StorageType = T;
    static StorageType Get(T value) { return value; }
    static void Set(T& value, StorageType bits) { value = bits; }
};
template<class T> struct Scalar<T, std::enable_if_t<std::is_enum_v<T>>> {
    using StorageType = std::underlying_type_t<T>;
    static StorageType Get(T value) { return static_cast<StorageType>(value); }
    static void Set(T& value, StorageType bits) { value = static_cast<T>(bits); }
};
template<> struct Scalar<float> {
    using StorageType = u32;
    static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);
    static StorageType Get(float value) { return std::bit_cast<StorageType>(value); }
    static void Set(float& value, StorageType bits) { value = std::bit_cast<float>(bits); }
};
template<class E, class S> struct Scalar<H2EnumStorage<E, S>> {
    using StorageType = S;
    static S Get(H2EnumStorage<E, S> value) { return static_cast<S>(value.value()); }
    static void Set(H2EnumStorage<E, S>& value, S bits) { value = bits; }
};
template<class E, class S> struct Scalar<H2SteppedEnumStorage<E, S>> {
    using StorageType = S;
    static S Get(H2SteppedEnumStorage<E, S> value) { return value.value(); }
    static void Set(H2SteppedEnumStorage<E, S>& value, S bits) { value = bits; }
};
template<class E, class S> struct Scalar<H2OpenCodeStorage<E, S>> {
    using StorageType = S;
    static S Get(H2OpenCodeStorage<E, S> value) { return value.value(); }
    static void Set(H2OpenCodeStorage<E, S>& value, S bits) { value = bits; }
};

template<class T> constexpr std::size_t FieldSize() {
    using Value = std::remove_cvref_t<T>;
    if constexpr (std::is_array_v<Value>)
        return std::extent_v<Value> * FieldSize<std::remove_extent_t<Value>>();
    else if constexpr (requires { Value::extent; }) {
        static_assert(Value::extent != std::dynamic_extent);
        return Value::extent * FieldSize<typename Value::element_type>();
    } else
        return sizeof(typename Scalar<Value>::StorageType);
}

template<class T> void Put(u8*& out, const T& value) {
    using Value = std::remove_cvref_t<T>;
    if constexpr (std::is_array_v<Value> || requires { Value::extent; }) {
        for (const auto& item : value) Put(out, item);
    } else {
        using S = typename Scalar<Value>::StorageType;
        static_assert(std::is_integral_v<S> && (sizeof(S) == 1 || sizeof(S) == 2 || sizeof(S) == 4));
        using U = std::make_unsigned_t<S>;
        const U bits = static_cast<U>(Scalar<Value>::Get(value));
        for (std::size_t i = 0; i < sizeof(S); ++i)
            *out++ = static_cast<u8>(bits >> (8 * i));
    }
}

template<class T> void Get(const u8*& in, T& value) {
    using Value = std::remove_cvref_t<T>;
    if constexpr (std::is_array_v<Value> || requires { Value::extent; }) {
        for (auto& item : value) Get(in, item);
    } else {
        using S = typename Scalar<Value>::StorageType;
        using U = std::make_unsigned_t<S>;
        U bits = 0;
        for (std::size_t i = 0; i < sizeof(S); ++i)
            bits = static_cast<U>(bits | (static_cast<u32>(*in++) << (8 * i)));
        Scalar<Value>::Set(value, std::bit_cast<S>(bits));
    }
}

template<class Record, class... T> Record Encode(std::tuple<T...> fields) {
    static_assert((FieldSize<T>() + ... + 0) == std::tuple_size_v<Record>);
    Record result{};
    u8* out = result.data();
    std::apply([&](const auto&... item) { (Put(out, item), ...); }, fields);
    return result;
}

template<class Record, class... T>
bool Decode(std::span<const u8> bytes, std::tuple<T...> fields) {
    static_assert((FieldSize<T>() + ... + 0) == std::tuple_size_v<Record>);
    if (bytes.size() != std::tuple_size_v<Record>) return false;
    const u8* in = bytes.data();
    std::apply([&](auto&... item) { (Get(in, item), ...); }, fields);
    return true;
}

auto ArmyFields(auto& v) {
    return std::tie(
        v.m_creatureTypes,
        v.m_creatureCounts);
}

auto HeroBaseFields(auto& v) {
    return std::tuple_cat(
        std::tie(
        v.m_spellPoints,
        v.m_id,
        v.m_owner,
        v.m_lastHeroInteractionTurn,
        v.m_lastInteractionHeroId,
        v.m_lastTownInteractionTurn,
        v.m_visitedTownId,
        v.m_name,
        v.m_cursorType,
        v.m_portrait,
        v.m_x,
        v.m_y,
        v.m_destinationX,
        v.m_destinationY,
        v.m_boatId,
        v.m_boatDestY,
        v.m_boatTravelRange,
        v.m_direction,
        v.m_locationType,
        v.m_occupiedTown,
        v.m_mobility,
        v.m_remainingMobility,
        v.m_experience,
        v.m_level,
        v.m_primaryStats,
        v.m_morale,
        v.m_luck,
        v._pad_0x46,
        v.m_gazeboVisits,
        v.m_fortVisits,
        v.m_witchDoctorVisits,
        v.m_mercenaryCampVisits,
        v.m_standingStoneVisits,
        v.m_treeKnowledgeVisits,
        v.m_xanaduVisits,
        v.m_randomSeed,
        v.m_enabled),
        ArmyFields(v.m_army),
        std::tie(
        v.m_secondarySkills,
        v.m_secondarySkillOrder,
        v.m_secondarySkillCount,
        v.m_spells,
        v.m_artifacts,
        v.m_eventFlags,
        v.m_isCaptain,
        v.m_aiFightValue));
}

auto HeroExpansionFields(auto& v) {
    return std::tuple_cat(HeroBaseFields(v), std::tie(v.m_artifactExtra));
}

auto TownFields(auto& v) {
    return std::tuple_cat(
        std::tie(
        v.m_id,
        v.m_owner,
        v.m_threat,
        v.m_type,
        v.m_x,
        v.m_y,
        v.m_boatX,
        v.m_boatY),
        ArmyFields(v.m_army),
        std::tie(
        v.m_occupyingHeroId,
        v.m_buildings,
        v.m_buildState,
        v.m_unknown1d,
        v.m_garrison,
        v.m_onMap,
        v.m_mayNotUpgradeToCastle,
        v.m_formation,
        v.m_originalOwner,
        v.m_extraIndex,
        v.m_spellPad,
        v.m_spellCounts,
        v.m_turnsOwned,
        v.m_name));
}

// Retail serializes 119 of the 120 reserved campaign bytes.
auto CampaignFields(auto& v) {
    return std::tuple_cat(
        std::tie(
        v.m_campaignType,
        v.m_campaignStartingSide,
        v.m_campaignScenario,
        v.m_campaignScenarioCompleted,
        v.m_campaignScenarioBonus,
        v.m_campaignScenarioDays,
        v.m_unknown7d,
        v.m_campaignAwards,
        v.m_campaignChoice,
        v.m_campaignMapEnabled,
        v.m_campaignScore,
        v.m_campaignCarryoverCreatureTypes,
        v.m_campaignCarryoverCreatureCounts,
        v.m_campaignScenarioWon,
        v.m_campaignCheated),
        std::tuple{std::span{v._pad_0xd2}.template first<119>()});
}

auto SetupFields(auto& v) {
    return std::tie(
        v.m_setupPlayerColor,
        v.m_playerHandicap,
        v.m_setupPlayerRace,
        v.m_setupPlayerNetworkId,
        v.m_difficulty,
        v.m_mapFilename,
        v.m_setupPlayerType,
        v.m_selectedSetupPlayer,
        v.m_newGameInitialized,
        v.m_newGameHumanCount,
        v._pad_0x47c);
}

auto ExpansionCampaignFields(auto& v, auto& windowSlot) {
    return std::tuple_cat(
        std::tie(
        v.m_campaignId,
        v.m_currentMap,
        v.m_mapCount,
        v.m_mapChoices,
        v.m_mapsPlayed,
        v.m_mapDays,
        v.m_awards,
        v.m_bonusChoices,
        v.m_pad_0x3f,
        v.m_viewMap),
        std::tie(windowSlot, v.m_viewOnly));
}

auto PlayerFields(auto& v, auto& cheated, auto& reserved, auto& firstBarrier) {
    return std::tuple_cat(
        std::tie(
        v.m_color,
        v.m_heroCount,
        v.m_currentHero,
        v.m_heroLocatorPage,
        v.m_heroIds,
        v.m_availableHeroIds),
        std::tie(reserved, cheated),
        std::tie(
        v.m_cheatValue,
        v.m_aiDifficulty,
        v.m_minimumHeroCount,
        v.m_evilInterface,
        v.m_ultimateArtifactHintChance,
        v.m_ultimateArtifactHintX,
        v.m_ultimateArtifactHintY,
        v.m_daysLeft,
        v.m_townCount,
        v.m_currentTown,
        v.m_townLocatorPage,
        v.m_townIds,
        v.m_resources,
        v.m_aiData.m_income),
        std::tie(firstBarrier, v.m_barrierTents, v.m_unknownad));
}

auto MineFields(auto& v) {
    return std::tie(
        v.id,
        v.owner,
        v.resourceType,
        v.guardianType,
        v.guardianCount,
        v.x,
        v.y);
}

auto BoatFields(auto& v) {
    return std::tie(
        v.id,
        v.x,
        v.y,
        v.direction,
        v.savedTriggerType,
        v.savedEventData,
        v.heroId,
        v.owner);
}

} // namespace

Army EncodeArmy(const armyGroup& value) {
    return Encode<Army>(ArmyFields(value));
}
bool DecodeArmy(std::span<const u8> bytes, armyGroup& value) {
    return Decode<Army>(bytes, ArmyFields(value));
}
HeroBase EncodeHeroBase(const hero& value) {
    return Encode<HeroBase>(HeroBaseFields(value));
}
bool DecodeHeroBase(std::span<const u8> bytes, hero& value) {
    return Decode<HeroBase>(bytes, HeroBaseFields(value));
}
HeroExpansion EncodeHeroExpansion(const hero& value) {
    return Encode<HeroExpansion>(HeroExpansionFields(value));
}
bool DecodeHeroExpansion(std::span<const u8> bytes, hero& value) {
    return Decode<HeroExpansion>(bytes, HeroExpansionFields(value));
}
Town EncodeTown(const town& value) {
    return Encode<Town>(TownFields(value));
}
bool DecodeTown(std::span<const u8> bytes, town& value) {
    return Decode<Town>(bytes, TownFields(value));
}
Campaign EncodeCampaign(const game& value) {
    return Encode<Campaign>(CampaignFields(value));
}
bool DecodeCampaign(std::span<const u8> bytes, game& value) {
    return Decode<Campaign>(bytes, CampaignFields(value));
}
Setup EncodeSetup(const game& value) {
    return Encode<Setup>(SetupFields(value));
}
bool DecodeSetup(std::span<const u8> bytes, game& value) {
    return Decode<Setup>(bytes, SetupFields(value));
}
Mine EncodeMine(const mineRecord& value) {
    return Encode<Mine>(MineFields(value));
}
bool DecodeMine(std::span<const u8> bytes, mineRecord& value) {
    return Decode<Mine>(bytes, MineFields(value));
}
Boat EncodeBoat(const boatRecord& value) {
    return Encode<Boat>(BoatFields(value));
}
bool DecodeBoat(std::span<const u8> bytes, boatRecord& value) {
    return Decode<Boat>(bytes, BoatFields(value));
}

Player EncodePlayer(const playerData& value, u8 cheated) {
    const u8 reserved[42]{};
    return Encode<Player>(PlayerFields(value, cheated, reserved, value.m_barrierTents));
}
bool DecodePlayer(std::span<const u8> bytes, playerData& value, u8& cheated) {
    u8 reserved[42]{};
    i8 firstBarrier = 0;
    // The second barrier byte wins, matching the retail reader. AI fields that
    // are absent from the save are deliberately left unchanged.
    return Decode<Player>(bytes, PlayerFields(value, cheated, reserved, firstBarrier));
}
ExpansionCampaign EncodeExpansionCampaign(const ExpCampaign& value) {
    // Retail wrote a process-local window address here. Keep its four-byte slot,
    // but never serialize or reconstruct a runtime pointer.
    const u32 windowSlot = 0;
    return Encode<ExpansionCampaign>(ExpansionCampaignFields(value, windowSlot));
}
bool DecodeExpansionCampaign(std::span<const u8> bytes, ExpCampaign& value) {
    u32 windowSlot = 0;
    return Decode<ExpansionCampaign>(bytes, ExpansionCampaignFields(value, windowSlot));
}
} // namespace save_records
