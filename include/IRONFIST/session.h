#ifndef HOMM2_IRONFIST_SESSION_H
#define HOMM2_IRONFIST_SESSION_H

#include <string>
#include <vector>

#include <IRONFIST/scripting.h>
#include <IRONFIST/state.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>

namespace ironfist {

// Values crossing the persistence boundary. No live manager, Lua state,
// window, resource handle, or owning engine allocation belongs in this data.
struct GameRecords {
#define IRONFIST_GAME_FIELD(member) decltype(game::member) member{};
#include <IRONFIST/session_fields.inc>
#undef IRONFIST_GAME_FIELD
};

struct CampaignRecords {
#define IRONFIST_CAMPAIGN_FIELD(member) decltype(ExpCampaign::member) member{};
#include <IRONFIST/session_fields.inc>
#undef IRONFIST_CAMPAIGN_FIELD
};

struct WorldRecords {
    i32 width = 0;
    i32 height = 0;
    std::vector<mapCell> cells;
    std::vector<mapCellExtra> extras;
    std::vector<u8> visibility;
    std::vector<std::vector<i8>> objects;
};

struct SessionData {
    GameRecords records;
    WorldRecords world;
    CampaignRecords expansion;
    state::CampaignState campaign;
    state::AdventureState adventure;
    bool hasWorld = false;
    bool hasSharedVision = false;
    bool hasForcedChases = false;
    bool hasBuildingBans = false;
    bool hasAIArmySharing = false;
    i32 campaignType = 0;
    bool expansionMap = false;
    std::string campaignMetadata;
    CalendarPeriodType monthType{};
    i32 monthExtra = 0;
    CalendarPeriodType weekType{};
    i32 weekExtra = 0;
    i32 mapChangeCounter = 0;
    i32 currentPlayer = 0;
    b32 greatestPlayer = false;
    char playerNames[X_GLOBAL_PLAYER_COUNT][GLOBAL_PLAYER_NAME_SIZE]{};
    b32 humanPlayers[GAME_PLAYER_COUNT]{};
    std::vector<i32> generatedArtifacts;
    std::string scriptSource;
    script::LuaTable mapVariables;
};

} // namespace ironfist

#endif
