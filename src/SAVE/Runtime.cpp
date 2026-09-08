#include <SAVE/Runtime.h>

#include <BASE/Misc.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Runtime.h>
#include <SOURCE/Localization.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/wingraph.h>

#include <algorithm>
#include <memory>
#include <utility>

namespace savegame {
namespace {

template <typename T>
void Copy(T& destination, const T& source) { destination = source; }

template <typename T, std::size_t N>
void Copy(T (&destination)[N], const T (&source)[N]) {
    for (std::size_t i = 0; i < N; ++i) Copy(destination[i], source[i]);
}

void CopyPlayer(playerData& destination, const playerData& source) {
#define SAVE_PLAYER_FIELD(name) Copy(destination.name, source.name);
#include <SAVE/PlayerFields.def>
#undef SAVE_PLAYER_FIELD
    destination.m_aiDifficulty = source.m_aiDifficulty;
}

struct File {
    i32 handle;
    ~File() { if (handle >= 0) platform::FileClose(handle); }
};

}

bool Capture(const game& source, Snapshot& state, std::string& error) {
    const auto& map = source.m_worldMap;
    if (map.width <= 0 || map.height <= 0 || map.width > 144 || map.height > 144
        || map.cells == nullptr || mapExtra == nullptr
        || map.extraCount < 0 || map.extraCount > 65536
        || (map.extraCount != 0 && map.extras == nullptr)
        || iMaxMapExtra <= 0 || iMaxMapExtra > 65536
        || ppMapExtra == nullptr || pwSizeOfMapExtra == nullptr) {
        error = "the current map is incomplete";
        return false;
    }
    Snapshot captured;
    captured.kind = gbInCampaign ? Kind::Campaign
        : xIsPlayingExpansionCampaign ? Kind::ExpansionCampaign : Kind::Scenario;
    captured.expansionMap = xIsExpansionMap;
    captured.greatest = gbIAmGreatest;
    captured.currentPlayer = giCurPlayer;
    captured.monthType = H2EnumIndex(giMonthType);
    captured.monthExtra = giMonthTypeExtra;
    captured.weekType = H2EnumIndex(giWeekType);
    captured.weekExtra = giWeekTypeExtra;
    captured.mapChangeSequence = giMapChangeCtr;
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i) captured.human[i] = gbHumanPlayer[i];
    captured.playerNames = cPlayerNames;
    captured.playerSystemIds = source.m_playerSystemIds;
    captured.resourceEncoding = localization::CurrentFileTextEncoding();
    captured.header = source.m_mapHeader;
    captured.mapFilename = source.m_mapFilename;
    captured.rumour = source.m_rumour;
#define SAVE_GAME_FIELD(name) Copy(captured.name, source.name);
#include <SAVE/GameFields.def>
#undef SAVE_GAME_FIELD
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i) CopyPlayer(captured.players[i], source.m_players[i]);
    for (i32 i = 0; i < GAME_HERO_COUNT; ++i) captured.heroes[i] = source.m_heroRecs[i];
    for (i32 i = 0; i < GAME_TOWN_COUNT; ++i) captured.towns[i] = source.m_castleRecs[i];
    std::copy(std::begin(source.m_mines), std::end(source.m_mines), captured.mines.begin());
    std::copy(std::begin(source.m_boats), std::end(source.m_boats), captured.boats.begin());
    if (captured.kind == Kind::ExpansionCampaign) {
        auto& expansion = captured.expansion;
        expansion.campaignId = H2EnumIndex(xCampaign.m_campaignId);
        expansion.currentMap = H2EnumIndex(xCampaign.m_currentMap);
        expansion.mapCount = xCampaign.m_mapCount;
        Copy(expansion.mapChoices, xCampaign.m_mapChoices);
        Copy(expansion.mapsPlayed, xCampaign.m_mapsPlayed);
        Copy(expansion.mapDays, xCampaign.m_mapDays);
        Copy(expansion.awards, xCampaign.m_awards);
        Copy(expansion.bonusChoices, xCampaign.m_bonusChoices);
    }
    captured.width = map.width;
    captured.height = map.height;
    const std::size_t cellCount = static_cast<std::size_t>(map.width) * static_cast<std::size_t>(map.height);
    captured.cells.assign(map.cells, map.cells + cellCount);
    captured.fog.assign(mapExtra, mapExtra + cellCount);
    captured.overlays.resize(static_cast<std::size_t>(map.extraCount));
    // Free slots contain no initialized object/overlay data. Do not inspect
    // their bitfields or persist allocator residue.
    for (i32 i = 1; i < map.extraCount; ++i) {
        if (map.extras[i].nextIndex == MAPCELL_EXTRA_FREE)
            captured.overlays[i].nextIndex = MAPCELL_EXTRA_FREE;
        else
            captured.overlays[i] = map.extras[i];
    }
    captured.events.resize(static_cast<std::size_t>(iMaxMapExtra));
    for (i32 i = 1; i < iMaxMapExtra; ++i) {
        if (pwSizeOfMapExtra[i] < 0) {
            error = "a map event has an invalid length";
            return false;
        }
        auto& event = captured.events[i];
        event.resize(static_cast<std::size_t>(pwSizeOfMapExtra[i]));
        if (ppMapExtra[i] != nullptr)
            std::memcpy(event.data(), ppMapExtra[i], event.size());
    }
    if (!Validate(captured, error)) return false;
    state = std::move(captured);
    return true;
}

void Apply(game& destination, Snapshot state) {
    // All strings and vectors have already been parsed and allocated. The
    // remaining allocations use the engine's normal fatal allocation policy.
    destination.SetMapSize(state.width, state.height);
    destination.m_worldMap.Init(state.width, state.height);
    auto& map = destination.m_worldMap;
    std::copy(state.cells.begin(), state.cells.end(), map.cells);
    map.extraCount = static_cast<i32>(state.overlays.size());
    if (map.extraCount != 0) {
        map.extras = static_cast<mapCellExtra*>(H2_ALLOC(state.overlays.size() * sizeof(mapCellExtra)));
        std::copy(state.overlays.begin(), state.overlays.end(), map.extras);
    }
    std::copy(state.fog.begin(), state.fog.end(), mapExtra);
    ClearMapExtra();
    iMaxMapExtra = static_cast<i32>(state.events.size());
    ppMapExtra = static_cast<void**>(H2_ALLOC(state.events.size() * sizeof(void*)));
    pwSizeOfMapExtra = static_cast<i16*>(H2_ALLOC(state.events.size() * sizeof(i16)));
    std::fill_n(ppMapExtra, iMaxMapExtra, nullptr);
    std::fill_n(pwSizeOfMapExtra, iMaxMapExtra, 0);
    for (i32 i = 1; i < iMaxMapExtra; ++i) {
        const auto& event = state.events[i];
        pwSizeOfMapExtra[i] = static_cast<i16>(event.size());
        if (!event.empty()) {
            ppMapExtra[i] = H2_ALLOC(event.size());
            std::memcpy(ppMapExtra[i], event.data(), event.size());
        }
    }
#define SAVE_GAME_FIELD(name) Copy(destination.name, state.name);
#include <SAVE/GameFields.def>
#undef SAVE_GAME_FIELD
    destination.m_mapHeader = state.header;
    destination.m_mapFilename = std::move(state.mapFilename);
    destination.m_rumour = std::move(state.rumour);
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i) {
        destination.m_players[i] = playerData{};
        CopyPlayer(destination.m_players[i], state.players[i]);
    }
    for (i32 i = 0; i < GAME_HERO_COUNT; ++i)
        static_cast<HeroState&>(destination.m_heroRecs[i]) = std::move(state.heroes[i]);
    for (i32 i = 0; i < GAME_TOWN_COUNT; ++i)
        static_cast<TownState&>(destination.m_castleRecs[i]) = std::move(state.towns[i]);
    std::copy(state.mines.begin(), state.mines.end(), std::begin(destination.m_mines));
    std::copy(state.boats.begin(), state.boats.end(), std::begin(destination.m_boats));
    gbInCampaign = state.kind == Kind::Campaign;
    xIsPlayingExpansionCampaign = state.kind == Kind::ExpansionCampaign;
    xIsExpansionMap = state.expansionMap;
    gbIAmGreatest = state.greatest;
    giCurPlayer = state.currentPlayer;
    giMonthType = CalendarPeriodTypeFromCode(state.monthType);
    giMonthTypeExtra = state.monthExtra;
    giWeekType = CalendarPeriodTypeFromCode(state.weekType);
    giWeekTypeExtra = state.weekExtra;
    giMapChangeCtr = state.mapChangeSequence;
    cPlayerNames = std::move(state.playerNames);
    destination.m_playerSystemIds = std::move(state.playerSystemIds);
    giNumHumanPlayers = 0;
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i) {
        gbHumanPlayer[i] = state.human[i];
        if (state.human[i] && !destination.m_playerDead[i]) ++giNumHumanPlayers;
        gbThisNetHumanPlayer[i] = state.human[i] && (!gbRemoteOn || i == giThisGamePos);
    }
    localization::SetCurrentFileTextEncoding(state.resourceEncoding);
    if (xIsPlayingExpansionCampaign) {
        const auto& expansion = state.expansion;
        xCampaign.m_campaignId = ExpansionCampaignIdFromCode(expansion.campaignId);
        xCampaign.m_currentMap = ExpansionCampaignMapFromCode(expansion.currentMap);
        xCampaign.m_mapCount = expansion.mapCount;
        Copy(xCampaign.m_mapChoices, expansion.mapChoices);
        Copy(xCampaign.m_mapsPlayed, expansion.mapsPlayed);
        Copy(xCampaign.m_mapDays, expansion.mapDays);
        Copy(xCampaign.m_awards, expansion.awards);
        Copy(xCampaign.m_bonusChoices, expansion.bonusChoices);
    }
}

bool ReadFile(const std::string& path, Snapshot& state, std::string& error) {
    File file{platform::FileOpen(path.c_str(), platform::FileMode::Read)};
    if (file.handle < 0) { error = "cannot open saved game"; return false; }
    const i32 size = platform::FileLength(file.handle);
    if (size < 0 || static_cast<std::size_t>(size) > MaximumFileBytes) {
        error = "saved game exceeds the size limit";
        return false;
    }
    std::vector<u8> bytes(static_cast<std::size_t>(size));
    if (!platform::FileReadExact(file.handle, bytes.data(), size)) {
        error = "cannot read saved game";
        return false;
    }
    return Decode(bytes, state, error);
}

bool WriteFile(const std::string& path, const Snapshot& state, std::string& error) {
    std::vector<u8> bytes;
    if (!Encode(state, bytes, error)) return false;
    File file{platform::FileOpen(path.c_str(), platform::FileMode::Write)};
    if (file.handle < 0) { error = "cannot create saved game"; return false; }
    if (!platform::FileWriteExact(file.handle, bytes.data(), static_cast<i32>(bytes.size()))) {
        error = "cannot write saved game";
        return false;
    }
    return true;
}

}
