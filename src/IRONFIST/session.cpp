#include <IRONFIST/session.h>
#include <IRONFIST/runtime.h>

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>

#include <BASE/Misc.h>
#include <BASE/Utf8.h>
#include <IRONFIST/artifacts.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/save_xml.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Strings.h>
#include <SOURCE/advManager.h>
#include <SOURCE/netwin.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/searchArray.h>

namespace ironfist::runtime {
namespace {

template <typename T>
void CopyRecord(T& destination, const T& source) {
    destination = source;
}

template <typename T, size_t Count>
void CopyRecord(T (&destination)[Count], const T (&source)[Count]) {
    for (size_t i = 0; i < Count; ++i)
        CopyRecord(destination[i], source[i]);
}

struct TrackedDelete {
    void operator()(void* pointer) const {
        if (pointer)
            H2_FREE(pointer);
    }
};

template <typename T>
using TrackedStorage = std::unique_ptr<T, TrackedDelete>;

template <typename T>
TrackedStorage<T> AllocateRecords(size_t count) {
    if (!count)
        return {};
    auto* records = static_cast<T*>(H2_ALLOC(count * sizeof(T)));
    std::memset(records, 0, count * sizeof(T));
    return TrackedStorage<T>(records);
}

// Prepare every owned buffer before replacing the active world's storage.
struct PreparedWorld {
    fullMap map;
    TrackedStorage<u8> visibility;
    TrackedStorage<void*> objects;
    TrackedStorage<i16> objectSizes;
    std::vector<TrackedStorage<i8>> objectBytes;

    explicit PreparedWorld(const WorldRecords& data) {
        map.width = data.width;
        map.height = data.height;
        const size_t cellCount = static_cast<size_t>(data.width) * data.height;
        if (data.width < 0 || data.height < 0 || data.width > MAP_DIMENSION_XLARGE
            || data.height > MAP_DIMENSION_XLARGE || data.cells.size() != cellCount
            || data.visibility.size() != cellCount)
            throw std::invalid_argument("Inconsistent world records");
        auto cells = AllocateRecords<mapCell>(cellCount);
        std::copy(data.cells.begin(), data.cells.end(), cells.get());
        map.cells = cells.release();
        auto extras = AllocateRecords<mapCellExtra>(data.extras.size());
        std::copy(data.extras.begin(), data.extras.end(), extras.get());
        map.extras = extras.release();
        map.extraCount = static_cast<i32>(data.extras.size());
        visibility = AllocateRecords<u8>(cellCount);
        std::copy(data.visibility.begin(), data.visibility.end(), visibility.get());
        objects = AllocateRecords<void*>(data.objects.size());
        objectSizes = AllocateRecords<i16>(data.objects.size());
        objectBytes.reserve(data.objects.size());
        for (size_t i = 0; i < data.objects.size(); ++i) {
            const auto& source = data.objects[i];
            if (source.size() > 32767)
                throw std::invalid_argument("Map object exceeds engine record capacity");
            auto bytes = AllocateRecords<i8>(source.size());
            std::copy(source.begin(), source.end(), bytes.get());
            objects.get()[i] = bytes.get();
            objectSizes.get()[i] = static_cast<i16>(source.size());
            objectBytes.push_back(std::move(bytes));
        }
    }

    void Commit() {
        ClearMapExtra();
        gpGame->m_worldMap.Close();
        std::swap(gpGame->m_worldMap.cells, map.cells);
        std::swap(gpGame->m_worldMap.extras, map.extras);
        gpGame->m_worldMap.width = map.width;
        gpGame->m_worldMap.height = map.height;
        gpGame->m_worldMap.extraCount = map.extraCount;
        const bool resized = MAP_WIDTH != map.width || MAP_HEIGHT != map.height || !bMapInitialized;
        MAP_WIDTH = map.width;
        MAP_HEIGHT = map.height;
        bMapInitialized = MAP_WIDTH > 0 && MAP_HEIGHT > 0;
        if (resized && bMapInitialized && gpSearchArray)
            gpSearchArray->Init();
        if (mapExtra)
            H2_FREE(mapExtra);
        mapExtra = visibility.release();
        iMaxMapExtra = static_cast<i32>(objectBytes.size());
        ppMapExtra = objects.release();
        pwSizeOfMapExtra = objectSizes.release();
        for (auto& bytes : objectBytes)
            bytes.release();
        if (gpAdvManager)
            gpAdvManager->m_mapData = &gpGame->m_worldMap;
    }
};

void RestorePolicy(const SessionData& data) {
    auto& policy = state::Get().adventure;
    if (data.hasAIArmySharing)
        gpGame->SetAIArmySharing(data.adventure.allowAIArmySharing);
    if (data.hasBuildingBans)
        CopyRecord(policy.disallowedBuildings, data.adventure.disallowedBuildings);
    if (data.hasSharedVision)
        CopyRecord(policy.sharePlayerVision, data.adventure.sharePlayerVision);
    if (data.hasForcedChases)
        CopyRecord(policy.forcedComputerPlayerChases, data.adventure.forcedComputerPlayerChases);
    else {
        for (i32 source = 0; source < GAME_HERO_COUNT; ++source)
            for (i32 destination = 0; destination < GAME_HERO_COUNT; ++destination)
                if (data.adventure.forcedComputerPlayerChases[source][destination])
                    gpGame->ForceHeroChase(source, destination, true);
    }
}

void SetLoadedPlayerContext(const SessionData& data) {
    i32 humans = 0;
    for (i32 player = 0; player < GAME_PLAYER_COUNT; ++player) {
        gbHumanPlayer[player] = data.humanPlayers[player] && humans < iWSLastMsgNumHumanPlayers;
        if (gbHumanPlayer[player])
            ++humans;
        gbThisNetHumanPlayer[player] = gbHumanPlayer[player] && (!gbRemoteOn || player == giThisGamePos);
    }
    giCurPlayer = data.currentPlayer;
    gpCurPlayer = giCurPlayer >= 0 && giCurPlayer < GAME_PLAYER_COUNT
        ? &gpGame->m_players[giCurPlayer] : nullptr;
    giCurPlayerBit = gpCurPlayer ? static_cast<u8>(1 << giCurPlayer) : 0;
    giCurWatchPlayer = giCurPlayer;
    // A spectator or a defeated network player may have no local active seat.
    for (i32 offset = 0; offset < gpGame->m_playerCount; ++offset) {
        const i32 player = (giCurPlayer + offset) % gpGame->m_playerCount;
        if (gbThisNetHumanPlayer[player]) {
            giCurWatchPlayer = player;
            break;
        }
    }
    giCurWatchPlayerBit = giCurWatchPlayer >= 0 && giCurWatchPlayer < GAME_PLAYER_COUNT
        ? static_cast<u8>(1 << giCurWatchPlayer) : 0;
    bShowIt = gpCurPlayer && gbThisNetHumanPlayer[giCurPlayer];
}

std::string SaveFilePath(const std::string& name) {
    const char* directory = platform::CompareIgnoringCase(name.c_str(), "RMT", 3) == 0
        ? ".\\DATA\\" : ".\\GAMES\\";
    return directory + name;
}

} // namespace

SessionData CaptureSession() {
    SessionData data;
#define IRONFIST_GAME_FIELD(member) CopyRecord(data.records.member, gpGame->member);
#define IRONFIST_CAMPAIGN_FIELD(member) CopyRecord(data.expansion.member, xCampaign.member);
#include <IRONFIST/session_fields.inc>
#undef IRONFIST_GAME_FIELD
#undef IRONFIST_CAMPAIGN_FIELD
    data.monthType = giMonthType;
    data.monthExtra = giMonthTypeExtra;
    data.weekType = giWeekType;
    data.weekExtra = giWeekTypeExtra;
    data.mapChangeCounter = giMapChangeCtr;
    data.currentPlayer = giCurPlayer;
    data.greatestPlayer = gbIAmGreatest;
    CopyRecord(data.playerNames, cPlayerNames);
    for (i32 i = 0; i < GAME_PLAYER_COUNT; ++i)
        data.humanPlayers[i] = gbHumanPlayer[i];
    const auto& map = gpGame->m_worldMap;
    data.world.width = map.width;
    data.world.height = map.height;
    const i32 cellCount = map.width * map.height;
    if (cellCount > 0) {
        data.world.cells.assign(map.cells, map.cells + cellCount);
        data.world.visibility.assign(mapExtra, mapExtra + cellCount);
    }
    if (map.extraCount > 0)
        data.world.extras.assign(map.extras, map.extras + map.extraCount);
    data.world.objects.resize(iMaxMapExtra);
    for (i32 i = 0; i < iMaxMapExtra; ++i) {
        const i16 size = pwSizeOfMapExtra[i];
        if (size > 0) {
            data.world.objects[i].resize(size);
            if (ppMapExtra[i])
                std::memcpy(data.world.objects[i].data(), ppMapExtra[i], size);
        }
    }
    data.hasWorld = true;
    data.adventure = state::Get().adventure;
    data.hasSharedVision = data.hasForcedChases = data.hasBuildingBans = data.hasAIArmySharing = true;
    data.campaign = state::Get().campaign;
    data.campaignType = gbInCampaign ? save::CAMPAIGN_ORIGINAL
        : xIsPlayingExpansionCampaign ? save::CAMPAIGN_EXPANSION : save::CAMPAIGN_NONE;
    data.expansionMap = xIsExpansionMap;
    if (data.campaignType == save::CAMPAIGN_EXPANSION) {
        tinyxml2::XMLDocument document;
        auto* root = document.NewElement("session");
        document.InsertEndChild(root);
        WriteCampaignMetadata(&document, root);
        if (auto* metadata = root->FirstChildElement()) {
            tinyxml2::XMLPrinter printer;
            metadata->Accept(&printer);
            data.campaignMetadata = printer.CStr();
        }
    }
    data.generatedArtifacts = SerializeGeneratedArtifacts();
    data.scriptSource = script::ActiveScriptContents();
    data.mapVariables = script::LoadMapVariablesFromLua();
    return data;
}

void RestoreSession(const SessionData& data) {
    std::unique_ptr<PreparedWorld> world;
    if (data.hasWorld)
        world = std::make_unique<PreparedWorld>(data.world);
    auto campaign = data.campaign;

    // Replace records while callbacks are unavailable. All lifecycle changes
    // happen here; codecs only construct SessionData.
    BeginSessionLoad();
    if (gpAdvManager)
        gpAdvManager->PurgeMapChangeQueue();
    if (world)
        world->Commit();
#define IRONFIST_GAME_FIELD(member) CopyRecord(gpGame->member, data.records.member);
#define IRONFIST_CAMPAIGN_FIELD(member) CopyRecord(xCampaign.member, data.expansion.member);
#include <IRONFIST/session_fields.inc>
#undef IRONFIST_GAME_FIELD
#undef IRONFIST_CAMPAIGN_FIELD
    state::Get().campaign.savedHeroData.swap(campaign.savedHeroData);
    giMonthType = data.monthType;
    giMonthTypeExtra = data.monthExtra;
    giWeekType = data.weekType;
    giWeekTypeExtra = data.weekExtra;
    giMapChangeCtr = data.mapChangeCounter;
    gbIAmGreatest = data.greatestPlayer;
    CopyRecord(cPlayerNames, data.playerNames);
    utf8::Copy(gMapName, GLOBAL_MAP_NAME_SIZE, gpGame->m_mapFilename);
    gbInCampaign = data.campaignType == save::CAMPAIGN_ORIGINAL;
    xIsPlayingExpansionCampaign = data.campaignType == save::CAMPAIGN_EXPANSION;
    xIsExpansionMap = data.expansionMap;
    gbGameOver = false;
    gpGame->m_gameLoaded = true;
    giCurTurn = gpGame->m_day + 7 * (gpGame->m_week - 1) + 28 * (gpGame->m_month - 1);
    SetLoadedPlayerContext(data);
    DeserializeGeneratedArtifacts(data.generatedArtifacts);
    if (!data.campaignMetadata.empty()) {
        tinyxml2::XMLDocument metadata;
        metadata.Parse(data.campaignMetadata.c_str());
        ReadCampaignMetadata(metadata.RootElement());
    }

    // Script defaults observe the restored records. Persisted runtime choices
    // consistently win afterwards, including AI sharing and building bans.
    if (data.scriptSource.empty())
        script::InitializeWithoutMap();
    else
        script::InitializeFromSave(data.scriptSource);
    script::WriteMapVariablesToLua(data.mapVariables);
    RestorePolicy(data);
    FinishSessionLoad();
}

std::string FileExtension(b32 isPickLoad) {
    if (gbInCampaign)
        return ".GMC";
    if (xIsPlayingExpansionCampaign) {
        const i32 campaign = H2EnumIndex(xCampaign.m_campaignId);
        return campaign < 4 ? ".GXC" : campaign == 4 ? ".GIC" : ".GCC";
    }
    i32 humans = isPickLoad ? iWSLastMsgNumHumanPlayers : 0;
    if (!isPickLoad)
        for (i32 player = 0; player < GAME_PLAYER_COUNT; ++player)
            if (!gpGame->m_playerDead[player] && gbHumanPlayer[player])
                ++humans;
    return std::string((isPickLoad && gbRemoteOn && xNetHasOldPlayers) || !xIsExpansionMap ? ".GM" : ".GX")
        + std::to_string(humans);
}

i32 SaveGame(const char* filename, i32 autosave) {
    gpAdvManager->DemobilizeCurrHero();
    std::string name = filename;
    if (autosave)
        name += FileExtension(false);
    if (platform::CompareIgnoringCase(name.c_str(), "RMT", 3)
        && platform::CompareIgnoringCase(name.c_str(), "AUTOSAVE", 8)
        && platform::CompareIgnoringCase(name.c_str(), "PLYREXIT", 8))
        utf8::Copy(gpGame->m_saveName, sizeof(gpGame->m_saveName), filename);
    const auto data = CaptureSession();
    save::XmlFile xml;
    if (xml.Save(SaveFilePath(name).c_str(), data) != tinyxml2::XML_SUCCESS) {
        DisplayError(std::string("Could not save XML. ") + xml.GetError(), "Ironfist save");
        return 0;
    }
    return 1;
}

LoadResult LoadGame(const char* filename, i32 loadFromFile) {
    if (!loadFromFile) {
        BeginSessionLoad();
        return LoadResult::Retail;
    }
    const std::string path = SaveFilePath(filename);
    const i32 file = platform::FileOpen(path.c_str(), platform::FileMode::Read);
    char firstByte = 0;
    if (file != -1) {
        if (!platform::FileReadExact(file, &firstByte, 1))
            firstByte = 0;
        platform::FileClose(file);
    }
    if (firstByte != '<') {
        BeginSessionLoad();
        return LoadResult::Retail;
    }
    SessionData data;
    save::XmlFile xml;
    if (xml.Read(path.c_str(), data) != tinyxml2::XML_SUCCESS) {
        DisplayError(std::string("Could not load XML. ") + xml.GetError(), "Ironfist load");
        return LoadResult::Failed;
    }
    RestoreSession(data);
    if (platform::CompareIgnoringCase(filename, "RMT", 3))
        utf8::Copy(gpGame->m_saveName, sizeof(gpGame->m_saveName), filename);
    gpAdvManager->m_heroContextLocked = false;
    gpGame->SetupAdjacentMons();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    return LoadResult::Loaded;
}

void RetailGameLoaded() {
    script::InitializeWithoutMap();
    FinishSessionLoad();
}

} // namespace ironfist::runtime
