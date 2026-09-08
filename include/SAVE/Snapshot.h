#ifndef HOMM2_SAVE_SNAPSHOT_H
#define HOMM2_SAVE_SNAPSHOT_H

#include <SAVE/Format.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/LegacyText.h>

#include <array>
#include <string>

namespace savegame {

enum class Kind : u32 { Scenario = 0, Campaign = 1, ExpansionCampaign = 2 };

struct ExpansionState {
    i32 campaignId = -1;
    i32 currentMap = -1;
    i32 mapCount = 0;
    u8 mapChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT]{};
    u8 mapsPlayed[EXPANSION_CAMPAIGN_MAX_MAP_COUNT]{};
    i16 mapDays[EXPANSION_CAMPAIGN_MAX_MAP_COUNT]{};
    u8 awards[EXPANSION_CAMPAIGN_AWARD_COUNT]{};
    u8 bonusChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT]{};
};

// No engine allocation owners or GUI objects: this can be validated in
// isolation and is also the output of the separate legacy-save importer.
struct Snapshot {
    Kind kind = Kind::Scenario;
    bool expansionMap = false;
    bool greatest = false;
    i32 currentPlayer = 0;
    i32 monthType = 0;
    i32 monthExtra = 0;
    i32 weekType = 0;
    i32 weekExtra = 0;
    i32 mapChangeSequence = 0;
    bool human[GAME_PLAYER_COUNT]{};
    std::array<std::string, GAME_PLAYER_COUNT> playerNames;
    std::array<std::string, GAME_PLAYER_COUNT> playerSystemIds;
    // Opaque records from the installed map retain their resource encoding.
    // Runtime names and rumours are always stored separately as UTF-8.
    localization::TextEncoding resourceEncoding = localization::TextEncoding::Windows1252;
    SMapHeader header{};
    std::string mapFilename;
    std::string rumour;
    ExpansionState expansion;
#define SAVE_GAME_FIELD(name) decltype(game::name) name{};
#include <SAVE/GameFields.def>
#undef SAVE_GAME_FIELD
    std::array<playerData, GAME_PLAYER_COUNT> players{};
    std::array<HeroState, GAME_HERO_COUNT> heroes{};
    std::array<TownState, GAME_TOWN_COUNT> towns{};
    std::array<mineRecord, GAME_MINE_COUNT> mines{};
    std::array<boatRecord, GAME_BOAT_COUNT> boats{};
    i32 width = 0;
    i32 height = 0;
    std::vector<mapCell> cells;
    std::vector<mapCellExtra> overlays;
    std::vector<u8> fog;
    std::vector<std::vector<u8>> events;
};

bool Validate(const Snapshot& state, std::string& error);
bool Encode(const Snapshot& state, std::vector<u8>& file, std::string& error);
bool Decode(std::span<const u8> file, Snapshot& state, std::string& error);

}

#endif
