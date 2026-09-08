#ifndef HOMM2_IRONFIST_CAMPAIGNS_H
#define HOMM2_IRONFIST_CAMPAIGNS_H

#include <array>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <Ints.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/KB.h>

namespace tinyxml2 {
class XMLDocument;
class XMLNode;
}

namespace ironfist {

struct CampaignTrackPoint {
    i32 x = 0;
    i32 y = 0;
};

struct CampaignTransition {
    std::optional<i32> movie;
    std::optional<i32> award;
    std::set<i32> unlocks;
};

struct ScenarioDefinition {
    std::string name;
    std::string description;
    i32 difficulty = 0;
    CampaignTrackPoint track;
    std::array<SCampaignChoice, EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT> choices;
    std::optional<i32> replayMovie;
    CampaignTransition victory;
    std::set<std::pair<i32, i32>> heroesToLoad;
    std::set<std::pair<i32, i32>> heroesToSave;

    ScenarioDefinition() {
        for (auto& choice : choices) {
            choice = {};
            choice.type = CAMPAIGN_CHOICE_NONE;
        }
    }
};

// Authored data only. Played maps, selected bonuses, and carried heroes belong
// to ExpCampaign and state::CampaignState, never to this catalog.
struct CampaignDefinition {
    i32 id = -1;
    std::string name;
    std::string shortName;
    std::string sourceFile;
    CampaignTransition start;
    std::vector<ScenarioDefinition> scenarios;

    const ScenarioDefinition& Scenario(i32 map) const { return scenarios.at(map); }
    const CampaignTransition& After(i32 map) const {
        return map == -1 ? start : Scenario(map).victory;
    }
};

class CampaignCatalog {
public:
    const CampaignDefinition* Find(ExpansionCampaignId id) const;
    const CampaignDefinition& At(ExpansionCampaignId id) const;
    // Validate and allocate before publishing; replacement drops every old rule.
    void Replace(CampaignDefinition definition);
    void Swap(CampaignCatalog& other) noexcept;

private:
    std::map<i32, CampaignDefinition> m_definitions;
};

CampaignCatalog& Campaigns();
void ValidateCampaignDefinition(const CampaignDefinition& definition);
bool ParseCampaignDefinition(tinyxml2::XMLNode* root, CampaignDefinition& out, std::string& error);
void WriteCampaignDefinition(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root,
                             const CampaignDefinition& definition);

void InitializeCampaigns();
b32 IsCustomCampaign(ExpansionCampaignId id);
const SCampaignChoice* CampaignChoice(ExpansionCampaignId id, i32 map, i32 choiceIdx);

// Reads a .cmp (campaign metadata XML) and returns its campaign ID, or -1.
i32 LoadCampaignFromFile(const std::string& filename);

void LoadCampaignSavedHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx);
void SaveCampaignHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx);

} // namespace ironfist

#endif
