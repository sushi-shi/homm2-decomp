#include <IRONFIST/campaigns.h>

#include <limits>
#include <stdexcept>
#include <tinyxml2.h>
#include <IRONFIST/xml_utils.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/playerData.h>
#include <SOURCE/SMACKMGR.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist {
namespace {

void Require(bool condition, const char* message) {
    if (!condition)
        throw std::invalid_argument(message);
}

i32 Integer(tinyxml2::XMLElement* element, const char* attribute = nullptr) {
    i32 value;
    Require(element && (attribute ? element->QueryIntAttribute(attribute, &value)
                                  : element->QueryIntText(&value)) == tinyxml2::XML_SUCCESS,
            "Missing or invalid campaign integer");
    return value;
}

std::string Text(tinyxml2::XMLElement* element, const char* attribute = nullptr) {
    const char* value = element ? (attribute ? element->Attribute(attribute) : element->GetText()) : nullptr;
    Require(value, "Missing campaign text");
    return value;
}

void ValidateChoice(const SCampaignChoice& choice) {
    const i32 value = choice.value;
    i32 limit = 0;
    switch (choice.type) {
        case CAMPAIGN_CHOICE_NONE: return;
        case CAMPAIGN_CHOICE_RESOURCE: limit = H2EnumIndex(RES_COUNT); break;
        case CAMPAIGN_CHOICE_ARTIFACT: limit = KB_ARTIFACT_TABLE_CAPACITY; break;
        case CAMPAIGN_CHOICE_SPELL:
        case CAMPAIGN_CHOICE_SPELL_SCROLL: limit = H2EnumIndex(KB_SPELL_TABLE_CAPACITY); break;
        case CAMPAIGN_CHOICE_SECONDARY_SKILL:
            limit = H2EnumIndex(HERO_SKILL_COUNT);
            Require(choice.amount > 0 && choice.amount < H2EnumIndex(HERO_SKILL_LEVEL_COUNT),
                    "Invalid campaign skill level");
            break;
        case CAMPAIGN_CHOICE_CREATURES: limit = KB_CREATURE_TABLE_CAPACITY; break;
        case CAMPAIGN_CHOICE_ALIGNMENT: limit = KB_FACTION_TABLE_CAPACITY; break;
        case CAMPAIGN_CHOICE_PRIMARY_SKILL: limit = H2EnumIndex(HERO_PRIMARY_STAT_COUNT); break;
        case CAMPAIGN_CHOICE_PUZZLE_PIECES:
        case CAMPAIGN_CHOICE_EXPERIENCE:
            Require(value >= 0, "Negative campaign bonus value");
            return;
        default: throw std::invalid_argument("Unknown campaign bonus type");
    }
    Require(value >= 0 && value < limit, "Campaign bonus field is outside its table");
}

void ValidateTransition(const CampaignTransition& transition, size_t maps) {
    for (i32 map : transition.unlocks)
        Require(map >= 0 && static_cast<size_t>(map) < maps, "Campaign unlock targets an undefined map");
    if (transition.award)
        Require(*transition.award >= 0 && *transition.award < EXPANSION_CAMPAIGN_AWARD_COUNT,
                "Campaign award is outside the award table");
    if (transition.movie)
        Require(*transition.movie >= 0 && *transition.movie < SMACK_OPTION_COUNT, "Invalid campaign movie");
}

void WriteIndexed(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root,
                  const char* name, i32 index, i32 value) {
    auto* element = doc->NewElement(name);
    element->SetAttribute("index", index);
    element->SetAttribute("value", value);
    root->InsertEndChild(element);
}

void WriteIndexed(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root,
                  const char* name, i32 index, const std::string& value) {
    auto* element = doc->NewElement(name);
    element->SetAttribute("index", index);
    element->SetAttribute("value", value.c_str());
    root->InsertEndChild(element);
}

void WriteTransition(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root,
                     const CampaignTransition& transition, i32 completed) {
    if (transition.movie)
        WriteIndexed(doc, root, "victorySMK", completed, *transition.movie);
    if (transition.award)
        WriteIndexed(doc, root, "award", completed, *transition.award);
    for (i32 map : transition.unlocks)
        WriteIndexed(doc, root, "mapToComplete", completed, map);
}

} // namespace

void ValidateCampaignDefinition(const CampaignDefinition& definition) {
    Require(definition.id >= 0 && definition.id < std::numeric_limits<i32>::max(), "Invalid campaign ID");
    Require(!definition.name.empty() && !definition.shortName.empty(), "Campaign needs a name and short name");
    Require(!definition.scenarios.empty() && definition.scenarios.size() <= EXPANSION_CAMPAIGN_MAX_MAP_COUNT,
            "Campaign map count exceeds engine storage");
    ValidateTransition(definition.start, definition.scenarios.size());
    if (IsCustomCampaign(ExpansionCampaignIdFromCode(definition.id)))
        Require(!definition.start.unlocks.empty(), "Custom campaign has no starting scenario");
    for (const auto& scenario : definition.scenarios) {
        Require(!scenario.name.empty(), "Campaign scenario needs a name");
        Require(scenario.difficulty >= 0 && scenario.difficulty < H2EnumIndex(DIFFICULTY_COUNT),
                "Invalid campaign difficulty");
        for (const auto& choice : scenario.choices)
            ValidateChoice(choice);
        ValidateTransition(scenario.victory, definition.scenarios.size());
        if (scenario.replayMovie)
            Require(*scenario.replayMovie >= 0 && *scenario.replayMovie < SMACK_OPTION_COUNT,
                    "Invalid campaign replay movie");
        for (const auto* heroes : {&scenario.heroesToLoad, &scenario.heroesToSave})
            for (auto [player, hero] : *heroes)
                Require(player >= 0 && player < GAME_PLAYER_COUNT && hero >= 0 && hero < PLAYER_HERO_CAPACITY,
                        "Campaign carryover hero is outside player storage");
    }
}

bool ParseCampaignDefinition(tinyxml2::XMLNode* root, CampaignDefinition& out, std::string& error) {
    try {
        Require(root, "Missing campaign metadata");
        CampaignDefinition definition;
        definition.id = Integer(root->FirstChildElement("id"));
        Require(definition.id >= H2EnumIndex(EXPANSION_CAMPAIGN_COUNT), "Custom campaign cannot replace a retail ID");
        definition.name = Text(root->FirstChildElement("name"));
        definition.shortName = Text(root->FirstChildElement("shortName"));
        if (auto* source = root->FirstChildElement("sourceFile"))
            definition.sourceFile = Text(source);
        const i32 maps = Integer(root->FirstChildElement("numMaps"));
        Require(maps > 0 && maps <= EXPANSION_CAMPAIGN_MAX_MAP_COUNT, "Invalid campaign map count");
        definition.scenarios.resize(maps);
        std::set<std::pair<std::string, i32>> scalarFields;
        for (auto* elem = root->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
            const std::string name = elem->Name();
            if (name == "id" || name == "name" || name == "shortName" || name == "numMaps" || name == "sourceFile") {
                Require(scalarFields.insert({name, -1}).second, "Duplicate campaign header field");
                continue;
            }
            if (name == "victorySMK" || name == "award" || name == "mapToComplete") {
                // The file uses completed-map counts: zero is campaign entry,
                // one is the first scenario's victory. Runtime data is zero based.
                const i32 completed = Integer(elem, "index");
                Require(completed >= 0 && completed <= maps, "Campaign transition has no matching scenario");
                auto& transition = completed == 0 ? definition.start : definition.scenarios[completed - 1].victory;
                const i32 value = Integer(elem, "value");
                if (name == "mapToComplete") {
                    Require(transition.unlocks.insert(value).second, "Duplicate campaign unlock");
                } else {
                    Require(scalarFields.insert({name, completed}).second, "Duplicate campaign transition field");
                    if (name == "award")
                        transition.award = value;
                    else if (value != -1)
                        transition.movie = value;
                }
                continue;
            }
            const bool scenarioID = name == "scenarioIcon" || name == "choice" || name == "saveHero" || name == "loadHero";
            const i32 index = Integer(elem, scenarioID ? "scenarioID" : "index");
            Require(index >= 0 && index < maps, "Campaign field refers to an undefined scenario");
            auto& scenario = definition.scenarios[index];
            if (name != "choice" && name != "saveHero" && name != "loadHero")
                Require(scalarFields.insert({name, index}).second, "Duplicate scenario field");
            if (name == "scenarioName") scenario.name = Text(elem, "value");
            else if (name == "scenarioDescription") scenario.description = Text(elem, "value");
            else if (name == "scenarioDifficulty") scenario.difficulty = Integer(elem, "value");
            else if (name == "scenarioIcon") scenario.track = {Integer(elem, "x"), Integer(elem, "y")};
            else if (name == "replaySMK") {
                const i32 value = Integer(elem, "value");
                if (value != -1)
                    scenario.replayMovie = value;
            } else if (name == "choice") {
                const i32 choiceID = Integer(elem, "id");
                Require(choiceID >= 0 && choiceID < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT, "Invalid campaign choice index");
                Require(scalarFields.insert({"choice", index * EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT + choiceID}).second,
                        "Duplicate campaign choice");
                const i32 type = Integer(elem, "type");
                const i32 value = Integer(elem, "field");
                const i32 amount = Integer(elem, "amount");
                Require(type >= 0 && type <= H2EnumIndex(CAMPAIGN_CHOICE_SPELL_SCROLL), "Invalid campaign choice type");
                Require(value >= std::numeric_limits<i16>::min() && value <= std::numeric_limits<i16>::max()
                        && amount >= std::numeric_limits<i16>::min() && amount <= std::numeric_limits<i16>::max(),
                        "Campaign choice exceeds native storage");
                auto& choice = scenario.choices[choiceID];
                choice.type = CampaignChoiceTypeFromCode(type);
                choice.value = static_cast<i16>(value);
                choice.amount = static_cast<i16>(amount);
            } else if (name == "saveHero" || name == "loadHero") {
                auto& heroes = name == "saveHero" ? scenario.heroesToSave : scenario.heroesToLoad;
                Require(heroes.insert({Integer(elem, "playerID"), Integer(elem, "ownedHeroID")}).second,
                        "Duplicate campaign carryover hero");
            } else {
                throw std::invalid_argument("Unknown campaign metadata field: " + name);
            }
        }
        ValidateCampaignDefinition(definition);
        out = std::move(definition);
        error.clear();
        return true;
    } catch (const std::exception& failure) {
        error = failure.what();
        return false;
    }
}

void WriteCampaignDefinition(tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root,
                             const CampaignDefinition& definition) {
    ValidateCampaignDefinition(definition);
    auto* metadata = doc->NewElement("campaignMetadata");
    xml::PushBack(doc, metadata, "id", definition.id);
    xml::PushBack(doc, metadata, "name", definition.name.c_str());
    xml::PushBack(doc, metadata, "shortName", definition.shortName.c_str());
    if (!definition.sourceFile.empty())
        xml::PushBack(doc, metadata, "sourceFile", definition.sourceFile.c_str());
    xml::PushBack(doc, metadata, "numMaps", static_cast<i32>(definition.scenarios.size()));
    WriteTransition(doc, metadata, definition.start, 0);
    for (i32 map = 0; map < static_cast<i32>(definition.scenarios.size()); ++map) {
        const auto& scenario = definition.scenarios[map];
        WriteIndexed(doc, metadata, "scenarioName", map, scenario.name);
        WriteIndexed(doc, metadata, "scenarioDescription", map, scenario.description);
        WriteIndexed(doc, metadata, "scenarioDifficulty", map, scenario.difficulty);
        auto* icon = doc->NewElement("scenarioIcon");
        icon->SetAttribute("scenarioID", map);
        icon->SetAttribute("x", scenario.track.x);
        icon->SetAttribute("y", scenario.track.y);
        metadata->InsertEndChild(icon);
        for (i32 index = 0; index < EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT; ++index) {
            const auto& choice = scenario.choices[index];
            auto* element = doc->NewElement("choice");
            element->SetAttribute("scenarioID", map);
            element->SetAttribute("id", index);
            element->SetAttribute("type", H2EnumIndex(choice.type));
            element->SetAttribute("field", choice.value);
            element->SetAttribute("amount", choice.amount);
            metadata->InsertEndChild(element);
        }
        if (scenario.replayMovie)
            WriteIndexed(doc, metadata, "replaySMK", map, *scenario.replayMovie);
        WriteTransition(doc, metadata, scenario.victory, map + 1);
        for (const bool save : {false, true})
            for (auto [player, hero] : save ? scenario.heroesToSave : scenario.heroesToLoad) {
                auto* element = doc->NewElement(save ? "saveHero" : "loadHero");
                element->SetAttribute("scenarioID", map);
                element->SetAttribute("playerID", player);
                element->SetAttribute("ownedHeroID", hero);
                metadata->InsertEndChild(element);
            }
    }
    root->InsertEndChild(metadata);
}

} // namespace ironfist
