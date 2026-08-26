#include <IRONFIST/campaigns.h>

#include <filesystem>
#include <string>
#include <strings.h>
#include <system_error>

#include <tinyxml2.h>

#include <IRONFIST/dialog.h>
#include <IRONFIST/state.h>
#include <IRONFIST/paths.h>
#include <IRONFIST/xml_utils.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {

std::map<i32, std::string> CampaignNames;
std::map<i32, std::string> CampaignShortNames;
std::map<i32, i32> CampaignMapCounts;
std::map<i32, std::map<i32, std::string>> ScenarioNames;
std::map<i32, std::map<i32, std::string>> ScenarioDescriptions;
std::map<i32, std::map<i32, i32>> CampaignDifficulties;
std::map<i32, std::map<i32, CampaignTrackPoint>> CampaignTrack;
std::map<i32, std::map<i32, std::map<i32, SCampaignChoice>>> CampaignChoices;
std::map<i32, std::map<i32, std::set<i32>>> MapsToComplete;
std::map<i32, std::map<i32, i32>> ReplayMovies;
std::map<i32, std::map<i32, i32>> VictoryMovies;
std::map<i32, std::map<i32, i32>> AwardsToGive;
std::map<i32, std::map<i32, std::set<std::pair<i32, i32>>>> HeroesToLoad;
std::map<i32, std::map<i32, std::set<std::pair<i32, i32>>>> HeroesToSave;
std::map<i32, std::string> CampaignSourceFiles;

b32 IsCustomCampaign(ExpansionCampaignId id) {
    return H2EnumIndex(id) >= H2EnumIndex(EXPANSION_CAMPAIGN_COUNT);
}

SCampaignChoice* CampaignChoice(ExpansionCampaignId id, i32 map, i32 choiceIdx) {
    return &CampaignChoices[H2EnumIndex(id)][map][choiceIdx];
}

void InitializeCampaigns() {
    for (i32 c = 0; c < H2EnumIndex(EXPANSION_CAMPAIGN_COUNT); c++) {
        CampaignNames[c] = xHSCampaignNames[c];
        CampaignShortNames[c] = xShortCampaignNames[c];
        CampaignMapCounts[c] = expansionCampaignMapCounts[c];
        for (i32 m = 0; m < expansionCampaignMapCounts[c]; m++) {
            ScenarioNames[c][m] = xScenarioName[c][m];
            ScenarioDescriptions[c][m] = xScenarioDescription[c][m];
            CampaignDifficulties[c][m] = H2EnumIndex(expansionCampaignDifficulty[c][m]);
            CampaignTrack[c][m] = {
                expansionCampaignTrackXY[c][m][0],
                expansionCampaignTrackXY[c][m][1]
            };
            for (i32 n = 0; n < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; n++)
                CampaignChoices[c][m][n] = xCampaignChoices[c][m][n];
        }
    }

    // Every .cmp under CAMPAIGNS/ registers itself under the ID it names.
    namespace fs = std::filesystem;
    std::error_code listError;
    fs::path campaignDir = ResolveDataPath("CAMPAIGNS");
    for (const fs::directory_entry& entry : fs::directory_iterator(campaignDir, listError)) {
        const std::string name = entry.path().filename().string();
        if (name.size() < 4 || strcasecmp(name.c_str() + name.size() - 4, ".cmp") != 0)
            continue;
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(entry.path().string().c_str()) != tinyxml2::XML_SUCCESS)
            continue;
        i32 campaignId = ReadCampaignMetadata(doc.FirstChild());
        if (campaignId != -1)
            CampaignSourceFiles[campaignId] = name;
    }
}

// The .cmp schema follows upstream Ironfist so its campaign files load
// unchanged; the same block rides inside XML saves for ID stability.
i32 ReadCampaignMetadata(tinyxml2::XMLNode* root) {
    i32 campaignID = -1;
    if (root == NULL)
        return campaignID;
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        if (elem == NULL)
            continue;
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "id") {
            elem->QueryIntText(&campaignID);
        } else if (name == "name") {
            const char* text = elem->GetText();
            CampaignNames[campaignID] = text ? text : "empty name";
        } else if (name == "shortName") {
            xml::QueryText(elem, CampaignShortNames[campaignID]);
        } else if (name == "numMaps") {
            elem->QueryIntText(&CampaignMapCounts[campaignID]);
        } else if (name == "scenarioName") {
            const char* text = elem->Attribute("value");
            if (text)
                ScenarioNames[campaignID][index] = text;
        } else if (name == "scenarioDescription") {
            const char* text = elem->Attribute("value");
            if (text)
                ScenarioDescriptions[campaignID][index] = text;
        } else if (name == "scenarioDifficulty") {
            CampaignDifficulties[campaignID][index] = value;
        } else if (name == "scenarioIcon") {
            i32 scenarioID = elem->IntAttribute("scenarioID");
            CampaignTrack[campaignID][scenarioID].x = elem->IntAttribute("x");
            CampaignTrack[campaignID][scenarioID].y = elem->IntAttribute("y");
        } else if (name == "choice") {
            i32 scenarioID = elem->IntAttribute("scenarioID");
            i32 choiceID = elem->IntAttribute("id");
            SCampaignChoice choice;
            choice.type = static_cast<CampaignChoiceType>(elem->IntAttribute("type"));
            choice.value = static_cast<i16>(elem->IntAttribute("field"));
            choice.amount = static_cast<i16>(elem->IntAttribute("amount"));
            CampaignChoices[campaignID][scenarioID][choiceID] = choice;
        } else if (name == "replaySMK") {
            ReplayMovies[campaignID][index] = value;
        } else if (name == "victorySMK") {
            VictoryMovies[campaignID][index] = value;
        } else if (name == "mapToComplete") {
            MapsToComplete[campaignID][index].insert(value);
        } else if (name == "award") {
            AwardsToGive[campaignID][index] = value;
        } else if (name == "saveHero") {
            i32 scenarioID = elem->IntAttribute("scenarioID");
            i32 playerID = elem->IntAttribute("playerID");
            i32 ownedHeroID = elem->IntAttribute("ownedHeroID");
            HeroesToSave[campaignID][scenarioID].insert({playerID, ownedHeroID});
        } else if (name == "loadHero") {
            i32 scenarioID = elem->IntAttribute("scenarioID");
            i32 playerID = elem->IntAttribute("playerID");
            i32 ownedHeroID = elem->IntAttribute("ownedHeroID");
            HeroesToLoad[campaignID][scenarioID].insert({playerID, ownedHeroID});
        }
    }
    // The map cap is retail-fixed; a fatter .cmp would corrupt ExpCampaign.
    if (CampaignMapCounts.count(campaignID)
        && CampaignMapCounts[campaignID] > EXPANSION_CAMPAIGN_MAX_MAP_COUNT)
        CampaignMapCounts[campaignID] = EXPANSION_CAMPAIGN_MAX_MAP_COUNT;
    return campaignID;
}

void WriteCampaignMetadata(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root) {
    i32 campaignID = H2EnumIndex(xCampaign.m_campaignId);
    if (!IsCustomCampaign(xCampaign.m_campaignId))
        return;

    tinyxml2::XMLElement* metadata = doc->NewElement("campaignMetadata");
    xml::PushBack(doc, metadata, "id", campaignID);
    xml::PushBack(doc, metadata, "name", CampaignNames[campaignID].c_str());
    xml::PushBack(doc, metadata, "shortName", CampaignShortNames[campaignID].c_str());
    xml::PushBack(doc, metadata, "numMaps", CampaignMapCounts[campaignID]);
    xml::WriteArray(doc, metadata, "scenarioName", ScenarioNames[campaignID]);
    xml::WriteArray(doc, metadata, "scenarioDescription", ScenarioDescriptions[campaignID]);
    xml::WriteArray(doc, metadata, "scenarioDifficulty", CampaignDifficulties[campaignID]);

    for (auto& track : CampaignTrack[campaignID]) {
        tinyxml2::XMLElement* icn = doc->NewElement("scenarioIcon");
        icn->SetAttribute("scenarioID", track.first);
        icn->SetAttribute("x", track.second.x);
        icn->SetAttribute("y", track.second.y);
        metadata->InsertEndChild(icn);
    }

    for (auto& scenario : CampaignChoices[campaignID]) {
        for (auto& choiceRow : scenario.second) {
            tinyxml2::XMLElement* choiceElem = doc->NewElement("choice");
            choiceElem->SetAttribute("scenarioID", scenario.first);
            choiceElem->SetAttribute("id", choiceRow.first);
            choiceElem->SetAttribute("type", H2EnumIndex(choiceRow.second.type));
            choiceElem->SetAttribute("field", choiceRow.second.value);
            choiceElem->SetAttribute("amount", choiceRow.second.amount);
            metadata->InsertEndChild(choiceElem);
        }
    }

    xml::WriteArray(doc, metadata, "replaySMK", ReplayMovies[campaignID]);
    xml::WriteArray(doc, metadata, "victorySMK", VictoryMovies[campaignID]);
    for (auto& mapRow : MapsToComplete[campaignID]) {
        for (i32 opened : mapRow.second) {
            tinyxml2::XMLElement* mapElem = doc->NewElement("mapToComplete");
            mapElem->SetAttribute("index", mapRow.first);
            mapElem->SetAttribute("value", opened);
            metadata->InsertEndChild(mapElem);
        }
    }
    xml::WriteArray(doc, metadata, "award", AwardsToGive[campaignID]);

    for (auto& scenario : HeroesToSave[campaignID]) {
        for (auto& heroData : scenario.second) {
            tinyxml2::XMLElement* heroElem = doc->NewElement("saveHero");
            heroElem->SetAttribute("scenarioID", scenario.first);
            heroElem->SetAttribute("playerID", heroData.first);
            heroElem->SetAttribute("ownedHeroID", heroData.second);
            metadata->InsertEndChild(heroElem);
        }
    }
    for (auto& scenario : HeroesToLoad[campaignID]) {
        for (auto& heroData : scenario.second) {
            tinyxml2::XMLElement* heroElem = doc->NewElement("loadHero");
            heroElem->SetAttribute("scenarioID", scenario.first);
            heroElem->SetAttribute("playerID", heroData.first);
            heroElem->SetAttribute("ownedHeroID", heroData.second);
            metadata->InsertEndChild(heroElem);
        }
    }
    root->InsertEndChild(metadata);
}

i32 LoadCampaignFromFile(const std::string& filename) {
    std::string path = ResolveDataPath("CAMPAIGNS") + "/" + filename;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.LoadFile(path.c_str());
    if (err != tinyxml2::XML_SUCCESS) {
        std::string message = "Could not load " + filename + "\n"
            + std::string(doc.ErrorStr() ? doc.ErrorStr() : "");
        H2MessageBox(message);
        return -1;
    }
    return ReadCampaignMetadata(doc.FirstChild());
}

void LoadCampaignSavedHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx) {
    i32 heroIdx = gpGame->m_players[playerId].m_heroIds[ownedHeroIdx];
    hero* heroRec = &gpGame->m_heroRecs[heroIdx];
    state::CampaignState::PartialHeroData* savedHero = &state::Get().campaign.savedHeroData[saveIdx];

    for (i32 i = 0; i < H2EnumIndex(HERO_PRIMARY_STAT_COUNT); i++)
        heroRec->m_primaryStats[i] = savedHero->primarySkills[i];
    for (i32 i = 0; i < H2EnumIndex(HERO_SKILL_COUNT); i++) {
        heroRec->m_secondarySkillOrder[i] = savedHero->skillIndex[i];
        heroRec->m_secondarySkills[i] = savedHero->secondarySkillLevel[i];
    }
    for (i32 i = 0; i < H2EnumIndex(KB_SPELL_TABLE_CAPACITY); i++)
        heroRec->m_spells[i] = savedHero->spellsLearned[i];
    heroRec->m_secondarySkillCount = savedHero->numSecSkillsKnown;
    heroRec->m_experience = savedHero->experience;
}

void SaveCampaignHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx) {
    i32 heroIdx = gpGame->m_players[playerId].m_heroIds[ownedHeroIdx];
    hero* heroRec = &gpGame->m_heroRecs[heroIdx];
    state::CampaignState::PartialHeroData* savedHero = &state::Get().campaign.savedHeroData[saveIdx];

    for (i32 i = 0; i < H2EnumIndex(HERO_PRIMARY_STAT_COUNT); i++)
        savedHero->primarySkills[i] = heroRec->m_primaryStats[i];
    for (i32 i = 0; i < H2EnumIndex(HERO_SKILL_COUNT); i++) {
        savedHero->skillIndex[i] = heroRec->m_secondarySkillOrder[i];
        savedHero->secondarySkillLevel[i] = H2EnumIndex(heroRec->m_secondarySkills[i]);
    }
    for (i32 i = 0; i < H2EnumIndex(KB_SPELL_TABLE_CAPACITY); i++)
        savedHero->spellsLearned[i] = heroRec->m_spells[i];
    savedHero->numSecSkillsKnown = heroRec->m_secondarySkillCount;
    savedHero->experience = heroRec->m_experience;
}

} // namespace ironfist
