#include <IRONFIST/campaigns.h>

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <system_error>
#include <tinyxml2.h>

#include <IRONFIST/dialog.h>
#include <IRONFIST/state.h>
#include <IRONFIST/paths.h>
#include <PLATFORM/Platform.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {

CampaignCatalog& Campaigns() {
    static CampaignCatalog catalog;
    return catalog;
}

const CampaignDefinition* CampaignCatalog::Find(ExpansionCampaignId id) const {
    const auto entry = definitions_.find(H2EnumIndex(id));
    return entry == definitions_.end() ? nullptr : &entry->second;
}

const CampaignDefinition& CampaignCatalog::At(ExpansionCampaignId id) const {
    return definitions_.at(H2EnumIndex(id));
}

void CampaignCatalog::Replace(CampaignDefinition definition) {
    ValidateCampaignDefinition(definition);
    const i32 id = definition.id;
    definitions_.insert_or_assign(id, std::move(definition));
}

void CampaignCatalog::Swap(CampaignCatalog& other) noexcept {
    definitions_.swap(other.definitions_);
}

b32 IsCustomCampaign(ExpansionCampaignId id) {
    return H2EnumIndex(id) >= H2EnumIndex(EXPANSION_CAMPAIGN_COUNT);
}

const SCampaignChoice* CampaignChoice(ExpansionCampaignId id, i32 map, i32 choiceIdx) {
    return &Campaigns().At(id).Scenario(map).choices.at(choiceIdx);
}

namespace {
bool ReadDefinitionFile(const std::filesystem::path& path, CampaignDefinition& definition,
                        std::string& error) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(path.string().c_str()) != tinyxml2::XML_SUCCESS) {
        error = doc.ErrorStr() ? doc.ErrorStr() : "Could not read campaign file";
        return false;
    }
    if (!ParseCampaignDefinition(doc.RootElement(), definition, error))
        return false;
    definition.sourceFile = path.filename().string();
    return true;
}
}

void InitializeCampaigns() {
    CampaignCatalog catalog;
    for (i32 c = 0; c < H2EnumIndex(EXPANSION_CAMPAIGN_COUNT); c++) {
        CampaignDefinition definition;
        definition.id = c;
        definition.name = xHSCampaignNames[c];
        definition.shortName = xShortCampaignNames[c];
        definition.scenarios.resize(expansionCampaignMapCounts[c]);
        for (i32 m = 0; m < expansionCampaignMapCounts[c]; m++) {
            auto& scenario = definition.scenarios[m];
            scenario.name = xScenarioName[c][m];
            scenario.description = xScenarioDescription[c][m];
            scenario.difficulty = H2EnumIndex(expansionCampaignDifficulty[c][m]);
            scenario.track = {expansionCampaignTrackXY[c][m][0], expansionCampaignTrackXY[c][m][1]};
            std::copy_n(xCampaignChoices[c][m], scenario.choices.size(), scenario.choices.begin());
        }
        catalog.Replace(std::move(definition));
    }

    // Sort before resolving duplicate IDs so the catalog is reproducible.
    namespace fs = std::filesystem;
    std::error_code listError;
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(ResolveDataPath("CAMPAIGNS"), listError)) {
        if (entry.is_regular_file() && strcasecmp(entry.path().extension().string().c_str(), ".cmp") == 0)
            files.push_back(entry.path());
    }
    std::sort(files.begin(), files.end());
    for (const auto& file : files) {
        CampaignDefinition definition;
        std::string error;
        if (ReadDefinitionFile(file, definition, error)) {
            if (!catalog.Find(ExpansionCampaignIdFromCode(definition.id))) {
                catalog.Replace(std::move(definition));
                continue;
            }
            error = "Duplicate campaign ID";
        }
        const std::string message = file.filename().string() + ": " + error;
        platform::Host().Log(platform::LogLevel::Warning, message.c_str());
    }
    Campaigns().Swap(catalog);
}

i32 LoadCampaignFromFile(const std::string& filename) {
    CampaignDefinition definition;
    std::string error;
    if (!ReadDefinitionFile(std::filesystem::path(ResolveDataPath("CAMPAIGNS")) / filename,
                            definition, error)) {
        std::string message = "Could not load " + filename + "\n" + error;
        H2MessageBox(message);
        return -1;
    }
    const i32 id = definition.id;
    Campaigns().Replace(std::move(definition));
    return id;
}

void LoadCampaignSavedHero(i32 playerId, i32 ownedHeroIdx, i32 saveIdx) {
    if (playerId < 0 || playerId >= GAME_PLAYER_COUNT || ownedHeroIdx < 0
        || ownedHeroIdx >= gpGame->m_players[playerId].m_heroCount)
        return;
    const i32 heroIdx = gpGame->m_players[playerId].m_heroIds[ownedHeroIdx];
    if (heroIdx < 0 || heroIdx >= GAME_HERO_COUNT)
        return;
    hero* heroRec = &gpGame->m_heroRecs[heroIdx];
    const auto entry = state::Get().campaign.savedHeroData.find(saveIdx);
    if (entry == state::Get().campaign.savedHeroData.end())
        return;
    const auto* savedHero = &entry->second;

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
    if (playerId < 0 || playerId >= GAME_PLAYER_COUNT || ownedHeroIdx < 0
        || ownedHeroIdx >= gpGame->m_players[playerId].m_heroCount)
        return;
    const i32 heroIdx = gpGame->m_players[playerId].m_heroIds[ownedHeroIdx];
    if (heroIdx < 0 || heroIdx >= GAME_HERO_COUNT)
        return;
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
