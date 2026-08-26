#include <IRONFIST/artifacts.h>

#include <cstdlib>
#include <map>
#include <string>

#include <tinyxml2.h>

#include <IRONFIST/dialog.h>
#include <IRONFIST/paths.h>

#include <SOURCE/KB_TYPES.h>
#include <SOURCE/Localization.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {

static std::map<i32, std::string> artifactNames;
static std::map<i32, std::string> artifactDescriptions;
static std::map<i32, std::string> artifactEvents;
static std::map<i32, i32> artifactIsCursed;
static std::map<i32, i32> isGenerated;
static std::map<i32, i32> artifactIsCampaignOnly;

static const char* gArtifactEvents[KB_ARTIFACT_TABLE_CAPACITY] = { 0 };

static const char* QueryTextAttribute(tinyxml2::XMLElement* el, const char* attribute) {
    const char* text = NULL;
    el->QueryStringAttribute(attribute, &text);
    return text != NULL ? text : "invalid name";
}

static void ReadArtifactData(tinyxml2::XMLNode* root) {
    for (tinyxml2::XMLNode* artifact = root->FirstChild(); artifact;
         artifact = artifact->NextSibling()) {
        tinyxml2::XMLElement* artElem = artifact->ToElement();
        const i32 id = artElem->IntAttribute("id");
        if (id < 0 || id >= KB_ARTIFACT_TABLE_CAPACITY) {
            continue;
        }

        std::string tableId = "table.gArtifactNames." + std::to_string(id);
        artifactNames[id] = localization::TranslateExternal(
            tableId.c_str(), QueryTextAttribute(artElem, "name")
        );
        gArtifactNames[id] = &(artifactNames[id][0]);
        std::string levelName(QueryTextAttribute(artElem, "level"));
        ArtifactLevelMask level = ARTIFACT_LEVEL_UNUSED;
        if (levelName == "ultimate")
            level = ARTIFACT_LEVEL_ULTIMATE;
        else if (levelName == "major")
            level = ARTIFACT_LEVEL_MAJOR;
        else if (levelName == "minor")
            level = ARTIFACT_LEVEL_MINOR;
        else if (levelName == "treasure")
            level = ARTIFACT_LEVEL_TREASURE;
        else if (levelName == "spellbook")
            level = ARTIFACT_LEVEL_SPELLBOOK;
        else if (levelName == "unused")
            level = ARTIFACT_LEVEL_UNUSED;

        gArtifactLevel[id] = level;

        artifactIsCursed[id] = artElem->BoolAttribute("cursed", 0);
        artifactIsCampaignOnly[id] = artElem->BoolAttribute("campaign_only", 0);

        std::string title;
        std::string description;
        for (tinyxml2::XMLNode* property = artElem->FirstChild(); property;
             property = property->NextSibling()) {
            tinyxml2::XMLElement* propElem = property->ToElement();
            std::string name = propElem->Name();
            const char* text = propElem->GetText();
            if (name == "title") {
                title = text != NULL ? text : "";
            } else if (name == "description") {
                description = text != NULL ? text : "";
            } else if (name == "event") {
                tableId = "table.gArtifactEvent." + std::to_string(id);
                artifactEvents[id] = localization::TranslateExternal(
                    tableId.c_str(), text != NULL ? text : ""
                );
                gArtifactEvents[id] = &artifactEvents[id][0];
            }
        }
        tableId = "table.gArtifactDesc." + std::to_string(id);
        const std::string englishDescription = "{" + title + "}\n\n" + description;
        artifactDescriptions[id] = localization::TranslateExternal(
            tableId.c_str(), englishDescription.c_str()
        );
        gArtifactDesc[id] = &artifactDescriptions[id][0];
    }
}

void LoadArtifacts() {
    tinyxml2::XMLDocument doc(true);
    tinyxml2::XMLError err = doc.LoadFile(ResolveDataPath("DATA/artifacts.xml").c_str());
    if (err) {
        std::string message = "Could not load XML. " + std::string(doc.ErrorStr());
        DisplayError(message, "Ironfist data");
        exit(1);
    }
    ReadArtifactData(doc.FirstChild());
}

bool IsCursedArtifact(ArtifactType artifact) {
    const i32 artifactId = H2EnumIndex(artifact);
    if (artifactIsCursed.find(artifactId) == artifactIsCursed.end()) {
        return false;
    }

    return artifactIsCursed[artifactId] != 0;
}

bool IsArtifactValid(i32 id) {
    if (artifactNames.find(id) == artifactNames.end()) {
        return false;
    }
    if (GetArtifactName(id).empty()) {
        return false;
    }
    if (GetArtifactLevel(id) == H2EnumIndex(ARTIFACT_LEVEL_UNUSED)) {
        return false;
    }

    return true;
}

bool IsArtifactGenerated(i32 id) {
    return isGenerated[id] == 1;
}

bool IsArtifactGenerationAllowed(i32 id) {
    if (!IsArtifactValid(id)) {
        return false;
    }
    if (IsArtifactGenerated(id)) {
        return false;
    }
    if (id == H2EnumIndex(ARTIFACT_SPELL_SCROLL)) {
        // A spell scroll is incomplete without its associated spell payload,
        // so the generic artifact generator must not select one by ID alone.
        return false;
    }
    if (artifactIsCampaignOnly[id]) {
        return false;
    }

    return true;
}

void GenerateArtifact(i32 id) {
    isGenerated[id] = 1;
}

void ResetGeneratedArtifacts() {
    isGenerated.clear();
}

void ResetGeneratedArtifacts(i32 matchingLevels) {
    for (auto& art : isGenerated) {
        const i32 id = art.first;
        if (H2EnumIndex(gArtifactLevel[id] & matchingLevels)) {
            art.second = 0;
        }
    }
}

void DeserializeGeneratedArtifacts(const std::vector<i32>& source) {
    ResetGeneratedArtifacts();
    for (auto i = 0u; i < source.size(); ++i) {
        isGenerated[i] = source[i];
    }
}

std::vector<i32> SerializeGeneratedArtifacts() {
    std::vector<i32> artifacts;
    if (isGenerated.empty()) {
        return artifacts;
    }
    artifacts.resize(isGenerated.rbegin()->first + 1, 0);
    for (auto art : isGenerated) {
        artifacts[art.first] = art.second;
    }
    return artifacts;
}

i32 GetArtifactLevel(i32 id) {
    return static_cast<i32>(gArtifactLevel[id].value());
}

std::string GetArtifactName(i32 id) {
    return artifactNames[id];
}

std::string GetArtifactDescription(i32 id) {
    return artifactDescriptions[id];
}

const char* GetArtifactEvent(i32 id) {
    if (id < 0 || id >= KB_ARTIFACT_TABLE_CAPACITY) {
        return "";
    }
    if (gArtifactEvents[id] != NULL) {
        return gArtifactEvents[id];
    }
    if (id < KB_ARTIFACT_LEVEL_COUNT && gArtifactEvent[id] != NULL) {
        return gArtifactEvent[id];
    }
    return "";
}

} // namespace ironfist
