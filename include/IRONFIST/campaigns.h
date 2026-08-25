#ifndef HOMM2_IRONFIST_CAMPAIGNS_H
#define HOMM2_IRONFIST_CAMPAIGNS_H

#include <map>
#include <set>
#include <string>
#include <utility>

#include <Ints.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/KB.h>

namespace tinyxml2 {
class XMLDocument;
class XMLNode;
}

/*
 * Campaigns become data.  The retail expansion tables seed these map-keyed
 * stores at startup, and every campaign metadata file in CAMPAIGNS
 * registers a further
 * campaign from ID 4 up, so a custom campaign runs through the same
 * X_CAMPGN code paths as the bundled four.
 */

struct IronfistTrackPoint {
    i32 x;
    i32 y;
};

extern std::map<i32, std::string> ironfistCampaignNames;
extern std::map<i32, std::string> ironfistCampaignShortNames;
extern std::map<i32, i32> ironfistCampaignMapCounts;
extern std::map<i32, std::map<i32, std::string>> ironfistScenarioNames;
extern std::map<i32, std::map<i32, std::string>> ironfistScenarioDescriptions;
extern std::map<i32, std::map<i32, i32>> ironfistCampaignDifficulties;
extern std::map<i32, std::map<i32, IronfistTrackPoint>> ironfistCampaignTrack;
extern std::map<i32, std::map<i32, std::map<i32, SCampaignChoice>>> ironfistCampaignChoices;

// Custom campaigns only: which scenarios a victory opens, which movies to
// play, which award a map grants, and which heroes carry between maps.
extern std::map<i32, std::map<i32, std::set<i32>>> ironfistMapsToComplete;
extern std::map<i32, std::map<i32, i32>> ironfistReplayMovies;
extern std::map<i32, std::map<i32, i32>> ironfistVictoryMovies;
extern std::map<i32, std::map<i32, i32>> ironfistAwardsToGive;
extern std::map<i32, std::map<i32, std::set<std::pair<i32, i32>>>> ironfistHeroesToLoad;
extern std::map<i32, std::map<i32, std::set<std::pair<i32, i32>>>> ironfistHeroesToSave;

// The .cmp file each custom campaign came from, so a save can re-resolve
// its campaign ID on load no matter what else sits in CAMPAIGNS/.
extern std::map<i32, std::string> ironfistCampaignSourceFiles;

void InitializeCampaigns();
b32 IsCustomCampaign(ExpansionCampaignId id);
SCampaignChoice* CampaignChoice(ExpansionCampaignId id, i32 map, i32 choiceIdx);

// Reads a .cmp (campaign metadata XML) and returns its campaign ID, or -1.
i32 LoadCampaignFromFile(const std::string& filename);
i32 ReadCampaignMetadata(tinyxml2::XMLNode* root);
void WriteCampaignMetadata(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root);

void LoadCampaignSavedHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx);
void SaveCampaignHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx);

#endif
