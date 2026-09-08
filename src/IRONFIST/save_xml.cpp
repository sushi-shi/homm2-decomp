#include <IRONFIST/save_xml.h>

#include <cstdio>
#include <cstring>
#include <BASE/Utf8.h>
#include <PLATFORM/Platform.h>

#include <map>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <IRONFIST/session.h>
#include <IRONFIST/scripting.h>

#include <EDITOR/mapcell.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/playerData.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::save {

namespace {

const char* RequiredAttribute(tinyxml2::XMLElement* element, const char* name) {
    const char* value = element->Attribute(name);
    if (!value)
        throw std::invalid_argument(std::string("Missing ") + name + " on " + element->Name());
    return value;
}

template <typename Container>
decltype(auto) CheckedSlot(Container& records, i32 index) {
    if (index < 0 || static_cast<size_t>(index) >= std::size(records))
        throw std::out_of_range("Record index outside session storage");
    return records[index];
}

template <size_t Size>
void ReadText(tinyxml2::XMLElement* element, char (&destination)[Size]) {
    utf8::Copy(destination, Size, element->GetText());
}

} // namespace


template <size_t Count>
static void WriteRelations(
    tinyxml2::XMLDocument* doc, tinyxml2::XMLNode* root, const char* groupName,
    const char* entryName, const bool (&relations)[Count][Count]
) {
    auto* group = doc->NewElement(groupName);
    for (size_t source = 0; source < Count; ++source) {
        for (size_t destination = 0; destination < Count; ++destination) {
            if (!relations[source][destination])
                continue;
            auto* entry = doc->NewElement(entryName);
            entry->SetAttribute("source", static_cast<i32>(source));
            entry->SetAttribute("destination", static_cast<i32>(destination));
            group->InsertEndChild(entry);
        }
    }
    // An empty group is a complete snapshot too: it cancels script defaults.
    root->InsertEndChild(group);
}

using Relations = std::vector<std::pair<i32, i32>>;

static Relations ReadRelations(tinyxml2::XMLNode* root, const char* entryName, i32 count) {
    Relations result;
    for (auto* entry = root->FirstChildElement(entryName); entry;
         entry = entry->NextSiblingElement(entryName)) {
        const i32 source = entry->IntAttribute("source", -1);
        const i32 destination = entry->IntAttribute("destination", -1);
        if (source >= 0 && source < count && destination >= 0 && destination < count)
            result.emplace_back(source, destination);
    }
    return result;
}

tinyxml2::XMLError XmlFile::Save(const char* fileName, const SessionData& data) {
    tempDoc->Clear();
    decodeError.clear();
    tinyxml2::XMLNode* pRoot = tempDoc->NewElement("ironfist_save");
    tempDoc->InsertFirstChild(pRoot);

    xml::PushBack(tempDoc, pRoot, "allowAIArmySharing", data.adventure.allowAIArmySharing);
    xml::PushBack(tempDoc, pRoot, "mapWidth", data.world.width);
    xml::PushBack(tempDoc, pRoot, "mapHeight", data.world.height);
    xml::PushBack(tempDoc, pRoot, "gameDifficulty", data.records.m_difficultyRating);
    xml::PushBack(tempDoc, pRoot, "monthType", H2EnumIndex(data.monthType));
    xml::PushBack(tempDoc, pRoot, "monthTypeExtra", data.monthExtra);
    xml::PushBack(tempDoc, pRoot, "weekType", H2EnumIndex(data.weekType));
    xml::PushBack(tempDoc, pRoot, "weekTypeExtra", data.weekExtra);
    xml::PushBack(tempDoc, pRoot, "giMapChangeCtr", data.mapChangeCounter);
    xml::PushBack(tempDoc, pRoot, "numPlayers", static_cast<i32>(data.records.m_playerCount));
    xml::PushBack(tempDoc, pRoot, "giCurPlayer", data.currentPlayer);
    xml::PushBack(tempDoc, pRoot, "couldBeNumDefeatedPlayers", static_cast<i32>(data.records.m_deadPlayerCount));
    xml::PushBack(tempDoc, pRoot, "day", data.records.m_day);
    xml::PushBack(tempDoc, pRoot, "week", data.records.m_week);
    xml::PushBack(tempDoc, pRoot, "month", data.records.m_month);
    xml::PushBack(tempDoc, pRoot, "numObelisks", static_cast<i32>(data.records.m_obeliskCount));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactLocX", static_cast<i32>(data.records.m_ultimateArtifactX));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactLocY", static_cast<i32>(data.records.m_ultimateArtifactY));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactIdx", static_cast<i32>(data.records.m_ultimateArtifactId.value()));
    xml::PushBack(tempDoc, pRoot, "currentRumor", data.records.m_rumour);
    xml::PushBack(tempDoc, pRoot, "numRumors", data.records.m_rumourEventCount);
    xml::PushBack(tempDoc, pRoot, "numEvents", data.records.m_timeEventCount);
    xml::PushBack(tempDoc, pRoot, "numMapEvents", data.records.m_mapEventCount);
    xml::PushBack(tempDoc, pRoot, "iMaxMapExtra", static_cast<i32>(data.world.objects.size()));
    xml::PushBack(tempDoc, pRoot, "difficulty", static_cast<i32>(data.records.m_difficulty.value()));
    xml::PushBack(tempDoc, pRoot, "mapFilename", data.records.m_mapFilename);
    xml::PushBack(tempDoc, pRoot, "relatedToNewGameSelection", static_cast<i32>(data.records.m_selectedSetupPlayer));
    xml::PushBack(tempDoc, pRoot, "relatedToNewGameInit", static_cast<i32>(data.records.m_newGameInitialized));
    xml::PushBack(tempDoc, pRoot, "numHumanPlayers", static_cast<i32>(data.records.m_newGameHumanCount));
    xml::PushBack(tempDoc, pRoot, "gbIAmGreatest", data.greatestPlayer);
    const i32 campaignType = data.campaignType;
    xml::PushBack(tempDoc, pRoot, "campaignType", campaignType);
    xml::PushBack(tempDoc, pRoot, "expansionMap", data.expansionMap);

    tinyxml2::XMLElement* pElement;
    if (campaignType) {
        pElement = tempDoc->NewElement("campaign");
        if (campaignType == CAMPAIGN_ORIGINAL) {
            xml::PushBack(tempDoc, pElement, "campID", static_cast<i32>(data.records.m_campaignType.value()));
            xml::PushBack(tempDoc, pElement, "campIDanother", static_cast<i32>(data.records.m_campaignStartingSide.value()));
            xml::PushBack(tempDoc, pElement, "campMapID", static_cast<i32>(data.records.m_campaignScenario));
            xml::PushBack(tempDoc, pElement, "campUnknown", static_cast<i32>(data.records.m_unknown7d));
            xml::PushBack(tempDoc, pElement, "campDaysPlayedCurrent", data.records.m_campaignScore);
            xml::PushBack(tempDoc, pElement, "campMaybeWon", static_cast<i32>(data.records.m_campaignScenarioWon));
            xml::PushBack(tempDoc, pElement, "campHasCheated", static_cast<i32>(data.records.m_campaignCheated));

            WriteCampaignDDArray(
                tempDoc, pElement, "campMapsWon", data.records.m_campaignScenarioCompleted
            );
            WriteCampaignDDArray(
                tempDoc, pElement, "campDaysPlayed", data.records.m_campaignScenarioBonus
            );
            WriteCampaignDDArray(
                tempDoc, pElement, "campDaysPlayed2", data.records.m_campaignScenarioDays
            );
            WriteCampaignDDArray(tempDoc, pElement, "campChoices", data.records.m_campaignChoice);
            WriteCampaignDDArray(
                tempDoc, pElement, "campMapsPlayed", data.records.m_campaignMapEnabled
            );

            xml::WriteArray(tempDoc, pElement, "campBonuses", data.records.m_campaignAwards);
            xml::WriteArray(
                tempDoc, pElement, "campPlayerCreatures", data.records.m_campaignCarryoverCreatureTypes
            );
            xml::WriteArray(
                tempDoc, pElement, "campPlayerCreatureQuantities",
                data.records.m_campaignCarryoverCreatureCounts
            );
        } else if (campaignType == CAMPAIGN_EXPANSION) {
            xml::PushBack(tempDoc, pElement, "campaignID", H2EnumIndex(data.expansion.m_campaignId));
            xml::PushBack(tempDoc, pElement, "currentMapID", H2EnumIndex(data.expansion.m_currentMap));
            xml::PushBack(tempDoc, pElement, "numMaps", data.expansion.m_mapCount);
            xml::PushBack(tempDoc, pElement, "mightBeScenarioID", H2EnumIndex(data.expansion.m_viewMap));
            xml::PushBack(tempDoc, pElement, "anIntVariable", data.expansion.m_viewOnly);

            xml::WriteArray(tempDoc, pElement, "mapChoice", data.expansion.m_mapChoices);
            xml::WriteArray(tempDoc, pElement, "mapsPlayed", data.expansion.m_mapsPlayed);
            xml::WriteArray(tempDoc, pElement, "daysPlayed", data.expansion.m_mapDays);
            xml::WriteArray(tempDoc, pElement, "awards", data.expansion.m_awards);
            xml::WriteArray(tempDoc, pElement, "bonusChoices", data.expansion.m_bonusChoices);

            // saved hero for autosaved saves
            for (auto& i : data.campaign.savedHeroData) {
                const state::CampaignState::PartialHeroData* savedHero = &i.second;
                tinyxml2::XMLElement* savedHeroElem = tempDoc->NewElement("savedHero");
                savedHeroElem->SetAttribute("index", i.first);
                xml::WriteArray(tempDoc, savedHeroElem, "primarySkills", savedHero->primarySkills);
                xml::WriteArray(tempDoc, savedHeroElem, "skillIndex", savedHero->skillIndex);
                xml::WriteArray(
                    tempDoc, savedHeroElem, "secondarySkillLevel", savedHero->secondarySkillLevel
                );
                for (i32 j = 0; j < KB_SPELL_TABLE_CAPACITY; j++) {
                    if (savedHero->spellsLearned[j]) {
                        tinyxml2::XMLElement* spellElem = tempDoc->NewElement("spell");
                        spellElem->SetAttribute("idx", j);
                        savedHeroElem->InsertEndChild(spellElem);
                    }
                }
                xml::PushBack(
                    tempDoc, savedHeroElem, "numSecSkillsKnown", savedHero->numSecSkillsKnown
                );
                xml::PushBack(tempDoc, savedHeroElem, "experience", savedHero->experience);

                pElement->InsertEndChild(savedHeroElem);
            }

            // A custom campaign's save carries its whole definition, so it
            // loads even if the .cmp vanishes from CAMPAIGNS/.
            if (data.campaignDefinition)
                WriteCampaignDefinition(tempDoc, pRoot, *data.campaignDefinition);
        }
        pRoot->InsertEndChild(pElement);
    }

    pElement = tempDoc->NewElement("mapHeader");
    const SMapHeader* mh = &data.records.m_mapHeader;
    xml::PushBack(tempDoc, pElement, "field_0", static_cast<i32>(mh->magic));
    xml::PushBack(
        tempDoc, pElement, "field_4", static_cast<i32>((mh->difficulty.value() | (mh->unknown5 << 8)))
    );
    xml::PushBack(tempDoc, pElement, "width", static_cast<i32>(mh->width));
    xml::PushBack(tempDoc, pElement, "height", static_cast<i32>(mh->height));
    xml::PushBack(tempDoc, pElement, "numPlayers", static_cast<i32>(mh->playerCount));
    xml::PushBack(tempDoc, pElement, "minHumans", static_cast<i32>(mh->minHumanPlayers));
    xml::PushBack(tempDoc, pElement, "maxHumans", static_cast<i32>(mh->maxHumanPlayers));
    xml::PushBack(tempDoc, pElement, "winConditionType", static_cast<i32>(mh->victoryCondition));
    xml::PushBack(tempDoc, pElement, "relatedToWinConditionType", static_cast<i32>(mh->computerAlsoWins));
    xml::PushBack(tempDoc, pElement, "allowDefeatAllVictory", static_cast<i32>(mh->allowNormalVictory));
    xml::PushBack(tempDoc, pElement, "winConditionArgumentOrLocX", mh->victoryConditionValue);
    xml::PushBack(tempDoc, pElement, "lossConditionType", static_cast<i32>(mh->lossCondition));
    xml::PushBack(
        tempDoc, pElement, "lossConditionArgumentOrLocX", static_cast<i32>((mh->lossConditionValue & 0xff))
    );
    xml::PushBack(tempDoc, pElement, "field_24", static_cast<i32>((mh->lossConditionValue >> 8)));
    xml::PushBack(tempDoc, pElement, "noStartingHeroInCastle", static_cast<i32>(mh->unknown25));
    xml::PushBack(tempDoc, pElement, "winConditionArgumentOrLocY", mh->victoryTownY);
    xml::PushBack(tempDoc, pElement, "lossConditionArgumentOrLocY", mh->lossTownY);
    xml::PushBack(tempDoc, pElement, "relatedToPlayerColorOrSide", mh->victorySideThreshold);
    xml::PushBack(tempDoc, pElement, "name", mh->name);
    xml::PushBack(tempDoc, pElement, "description", mh->description);
    xml::PushBack(tempDoc, pElement, "field_1A0", static_cast<i32>(mh->rumourCount));
    xml::PushBack(tempDoc, pElement, "field_1A1", static_cast<i32>(mh->timeEventCount));
    xml::PushBack(tempDoc, pElement, "numRumors", static_cast<i32>(mh->rumourCount));
    xml::PushBack(tempDoc, pElement, "numEvents", static_cast<i32>(mh->timeEventCount));
    xml::WriteArray(tempDoc, pElement, "hasPlayer", mh->playerEnabled);
    xml::WriteArray(tempDoc, pElement, "playerMayBeHuman", mh->playerCanHuman);
    xml::WriteArray(tempDoc, pElement, "playerMayBeComp", mh->playerCanComputer);
    xml::WriteArray(tempDoc, pElement, "playerFactions", mh->playerRace);
    pRoot->InsertEndChild(pElement);

    xml::WriteArray(tempDoc, pRoot, "playerNames", data.playerNames);
    xml::WriteArray(tempDoc, pRoot, "deadPlayers", data.records.m_playerDead);

    bchar playerAlive[H2EnumIndex(GAME_PLAYER_COUNT)];
    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); ++i) {
        playerAlive[i] = data.humanPlayers[i];
        if (data.records.m_playerDead[i])
            playerAlive[i] = false;
    }

    xml::WriteArray(tempDoc, pRoot, "alivePlayers", playerAlive);
    xml::WriteArray(tempDoc, pRoot, "heroHireStatus", data.records.m_availableHeroes);
    xml::WriteArray(tempDoc, pRoot, "relatedToPlayerPosAndColor", data.records.m_setupPlayerColor);
    xml::WriteArray(tempDoc, pRoot, "playerHandicap", data.records.m_playerHandicap);
    xml::WriteArray(tempDoc, pRoot, "newGameSelectedFaction", data.records.m_setupPlayerRace);
    xml::WriteArray(
        tempDoc, pRoot, "somePlayerCodeOr10IfMayBeHuman", data.records.m_setupPlayerNetworkId
    );
    xml::WriteArray(tempDoc, pRoot, "somePlayerNumData", data.records.m_setupPlayerType);
    xml::WriteArray(tempDoc, pRoot, "field_47C", data.records._pad_0x47c);
    xml::WriteArray(tempDoc, pRoot, "field_2773", data.records.m_castleOwners);
    xml::WriteArray(tempDoc, pRoot, "builtToday", data.records.m_dailyEventFlags);
    xml::WriteArray(tempDoc, pRoot, "field_60A6", data.records.m_mineOwners);
    xml::WriteArray(tempDoc, pRoot, "randomArtifacts", data.generatedArtifacts);
    xml::WriteArray(tempDoc, pRoot, "boatBuilt", data.records.m_boatSlots);
    xml::WriteArray(tempDoc, pRoot, "obeliskVisitedMasks", data.records.m_obeliskVisitors);
    xml::WriteArray(tempDoc, pRoot, "field_637D", data.records.m_defaultPlayerNames);
    xml::WriteArray(tempDoc, pRoot, "rumorIndices", data.records.m_rumourEventIndices);
    xml::WriteArray(tempDoc, pRoot, "eventIndices", data.records.m_timeEventIndices);
    xml::WriteArray(tempDoc, pRoot, "mapEventIndices", data.records.m_mapEventIndices);

    for (size_t i = 1; i < data.world.objects.size(); ++i) {
        auto* extraElem = tempDoc->NewElement("mapExtra");
        extraElem->SetAttribute("index", static_cast<i32>(i));
        for (i8 value : data.world.objects[i]) {
            auto* byte = tempDoc->NewElement("ppMapExtra");
            byte->SetAttribute("value", static_cast<i32>(value));
            extraElem->InsertEndChild(byte);
        }
        pRoot->InsertEndChild(extraElem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++) {
        const playerData* player = &data.records.m_players[i];
        tinyxml2::XMLElement* playerElem = tempDoc->NewElement("playerData");
        playerElem->SetAttribute("index", i);

        xml::PushBack(tempDoc, playerElem, "color", static_cast<i32>(player->m_color));
        xml::PushBack(tempDoc, playerElem, "numHeroes", static_cast<i32>(player->m_heroCount));
        xml::PushBack(tempDoc, playerElem, "curHeroIdx", static_cast<i32>(player->m_currentHero));
        xml::PushBack(
            tempDoc, playerElem, "relatedToSomeSortOfHeroCountOrIdx",
            static_cast<i32>(player->m_heroLocatorPage)
        );
        xml::PushBack(tempDoc, playerElem, "hasCheated", static_cast<i32>(data.records.m_cheated));
        xml::PushBack(tempDoc, playerElem, "puzzlePieces", static_cast<i32>(player->m_cheatValue));
        xml::PushBack(tempDoc, playerElem, "personality", H2EnumIndex(player->m_aiDifficulty));
        xml::PushBack(
            tempDoc, playerElem, "relatedToMaxOrNumHeroes", static_cast<i32>(player->m_minimumHeroCount)
        );
        xml::PushBack(tempDoc, playerElem, "hasEvilFaction", static_cast<i32>(player->m_evilInterface));
        xml::PushBack(
            tempDoc, playerElem, "aiNumberPuzzlePieces",
            static_cast<i32>(player->m_ultimateArtifactHintChance)
        );
        xml::PushBack(
            tempDoc, playerElem, "aiProbableUltimateArtifactX",
            static_cast<i32>(player->m_ultimateArtifactHintX)
        );
        xml::PushBack(
            tempDoc, playerElem, "aiProbableUltimateArtifactY",
            static_cast<i32>(player->m_ultimateArtifactHintY)
        );
        xml::PushBack(tempDoc, playerElem, "daysLeftWithoutCastle", static_cast<i32>(player->m_daysLeft));
        xml::PushBack(tempDoc, playerElem, "numCastles", static_cast<i32>(player->m_townCount));
        xml::PushBack(tempDoc, playerElem, "mightBeCurCastleIdx", static_cast<i32>(player->m_currentTown));
        xml::PushBack(tempDoc, playerElem, "relatedToUnknown", static_cast<i32>(player->m_townLocatorPage));
        xml::PushBack(tempDoc, playerElem, "barrierTentsVisited", static_cast<i32>(player->m_barrierTents));

        xml::WriteArray(tempDoc, playerElem, "heroesOwned", player->m_heroIds);
        xml::WriteArray(tempDoc, playerElem, "heroesForPurchase", player->m_availableHeroIds);
        xml::WriteArray(tempDoc, playerElem, "castlesOwned", player->m_townIds);
        xml::WriteArray(tempDoc, playerElem, "resources", player->m_resources);
        xml::WriteArray(tempDoc, playerElem, "_4_2_1", player->m_unknownad);
        xml::WriteArray(tempDoc, playerElem, "resourcesIncome", player->m_aiData.m_income);

        pRoot->InsertEndChild(playerElem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_TOWN_COUNT); i++) {
        const town* twn = &data.records.m_castleRecs[i];
        tinyxml2::XMLElement* townElem = tempDoc->NewElement("town");
        townElem->SetAttribute("index", i);
        xml::PushBack(tempDoc, townElem, "idx", static_cast<i32>(twn->m_id));
        xml::PushBack(tempDoc, townElem, "ownerIdx", static_cast<i32>(twn->m_owner));
        xml::PushBack(tempDoc, townElem, "alignment", static_cast<i32>(twn->m_threat));
        xml::PushBack(tempDoc, townElem, "factionID", static_cast<i32>(twn->m_type.value()));
        xml::PushBack(tempDoc, townElem, "x", static_cast<i32>(twn->m_x));
        xml::PushBack(tempDoc, townElem, "y", static_cast<i32>(twn->m_y));
        xml::PushBack(tempDoc, townElem, "buildDockRelated", static_cast<i32>(twn->m_boatX));
        xml::PushBack(tempDoc, townElem, "boatCell", static_cast<i32>(twn->m_boatY));
        xml::PushBack(tempDoc, townElem, "visitingHeroIdx", static_cast<i32>(twn->m_occupyingHeroId));
        xml::PushBack(tempDoc, townElem, "buildingsBuiltFlags", static_cast<u32>(twn->m_buildings));
        xml::PushBack(tempDoc, townElem, "mageGuildLevel", static_cast<i32>(twn->m_buildState));
        xml::PushBack(tempDoc, townElem, "field_1D", static_cast<i32>(twn->m_unknown1d));
        xml::PushBack(tempDoc, townElem, "exists", static_cast<i32>(twn->m_onMap));
        xml::PushBack(
            tempDoc, townElem, "mayNotBeUpgradedToCastle", static_cast<i32>(twn->m_mayNotUpgradeToCastle)
        );
        xml::PushBack(tempDoc, townElem, "field_38", static_cast<i32>(twn->m_formation));
        xml::PushBack(tempDoc, townElem, "playerPos", static_cast<i32>(twn->m_originalOwner));
        xml::PushBack(tempDoc, townElem, "extraIdx", twn->m_extraIndex);
        xml::PushBack(tempDoc, townElem, "field_55", static_cast<i32>((twn->m_turnsOwned & 0xff)));
        xml::PushBack(tempDoc, townElem, "field_63", static_cast<i32>((twn->m_turnsOwned >> 8)));
        xml::PushBack(tempDoc, townElem, "name", twn->m_name);

        xml::WriteArray(tempDoc, townElem, "numCreaturesInDwelling", twn->m_garrison);
        i8 numSpellsOfLevel[TOWN_MAGE_GUILD_LEVEL_COUNT];
        for (i32 j = 0; j < TOWN_MAGE_GUILD_LEVEL_COUNT; j++)
            numSpellsOfLevel[j] = twn->m_spellCounts[j + TOWN_MAGE_GUILD_FIRST_LEVEL];
        xml::WriteArray(tempDoc, townElem, "numSpellsOfLevel", numSpellsOfLevel);

        for (i32 j = 0; j < TOWN_MAGE_GUILD_LEVEL_COUNT * TOWN_MAGE_GUILD_SPELLS_PER_LEVEL;
             j++) {
            i32 x = j % TOWN_MAGE_GUILD_LEVEL_COUNT;
            i32 y = j / TOWN_MAGE_GUILD_LEVEL_COUNT;
            tinyxml2::XMLElement* spellElem = tempDoc->NewElement("mageGuildSpell");
            spellElem->SetAttribute("level", x);
            spellElem->SetAttribute("idx", y);
            spellElem->SetAttribute("spell", static_cast<i32>(twn->m_spells[x][y].value()));
            townElem->InsertEndChild(spellElem);
        }

        for (i32 j = 0; j < ARMY_GROUP_SLOT_COUNT; j++) {
            tinyxml2::XMLElement* creatElem = tempDoc->NewElement("garrisonCreature");
            creatElem->SetAttribute("index", j);
            creatElem->SetAttribute("type", static_cast<i32>(twn->m_army.m_creatureTypes[j].value()));
            creatElem->SetAttribute("quantity", twn->m_army.m_quantities[j]);
            townElem->InsertEndChild(creatElem);
        }

        pRoot->InsertEndChild(townElem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_MINE_COUNT); i++) {
        const mineRecord* m = &data.records.m_mines[i];
        tinyxml2::XMLElement* mineElem = tempDoc->NewElement("mine");
        mineElem->SetAttribute("index", i);
        mineElem->SetAttribute("x", m->x);
        mineElem->SetAttribute("y", m->y);
        mineElem->SetAttribute("field_0", m->id);
        mineElem->SetAttribute("owner", m->owner);
        mineElem->SetAttribute("type", static_cast<i32>(m->resourceType.value()));
        mineElem->SetAttribute("guardianType", static_cast<i32>(m->guardianType.value()));
        mineElem->SetAttribute("guardianQty", m->guardianCount);
        pRoot->InsertEndChild(mineElem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_BOAT_COUNT); i++) {
        const boatRecord* b = &data.records.m_boats[i];
        tinyxml2::XMLElement* boatElem = tempDoc->NewElement("boat");
        boatElem->SetAttribute("index", i);
        boatElem->SetAttribute("idx", b->id);
        boatElem->SetAttribute("x", b->x);
        boatElem->SetAttribute("y", b->y);
        boatElem->SetAttribute("direction", static_cast<i32>(b->direction.value()));
        boatElem->SetAttribute("underlyingObjType", static_cast<i32>(b->savedTriggerType.value()));
        boatElem->SetAttribute("underlyingObjExtra", b->savedEventData);
        boatElem->SetAttribute("heroIdx", b->heroId & 0x7f);
        boatElem->SetAttribute("isEmpty", (b->heroId >> 7) & 1);
        boatElem->SetAttribute("owner", b->owner);
        pRoot->InsertEndChild(boatElem);
    }

    pElement = tempDoc->NewElement("map");
    const WorldRecords* map = &data.world;
    pElement->SetAttribute("width", map->width);
    pElement->SetAttribute("height", map->height);
    pElement->SetAttribute("numCellExtras", static_cast<i32>(map->extras.size()));
    for (i32 i = 0; i < map->height * map->width; i++) {
        const mapCell* c = &map->cells[i];
        tinyxml2::XMLElement* mapElement = tempDoc->NewElement("mapCell");
        mapElement->SetAttribute("index", i);
        mapElement->SetAttribute("groundIndex", c->m_terrainImageIndex);
        mapElement->SetAttribute("hasObject", c->m_animatedObject);
        mapElement->SetAttribute("isRoad", c->m_isRoad);
        mapElement->SetAttribute("objTileset", H2EnumIndex(c->ObjectTileset()));
        mapElement->SetAttribute("objectIndex", c->m_objectIndex);
        mapElement->SetAttribute("field_4_1", c->m_objectLayerBit0);
        mapElement->SetAttribute("isShadow", c->m_objectLayerBit1);
        mapElement->SetAttribute("field_4_3", c->m_objectDrawnAsOverlay);
        mapElement->SetAttribute("extraInfo", c->m_objectMetadata);
        mapElement->SetAttribute("hasOverlay", c->m_animatedOverlay);
        mapElement->SetAttribute("hasLateOverlay", c->m_drawOverlayOnTop);
        mapElement->SetAttribute("overlayTileset", H2EnumIndex(c->OverlayTileset()));
        mapElement->SetAttribute("overlayIndex", c->m_overlayIndex);
        mapElement->SetAttribute("flags", c->m_flags);
        mapElement->SetAttribute("objType", static_cast<i32>(c->m_triggerType.value()));
        mapElement->SetAttribute("extraIdx", c->m_extraIndex);
        pElement->InsertEndChild(mapElement);
    }

    for (i32 i = 0; i < static_cast<i32>(map->extras.size()); i++) {
        const mapCellExtra* e = &map->extras[i];
        tinyxml2::XMLElement* mapElement = tempDoc->NewElement("mapCellExtra");
        mapElement->SetAttribute("index", i);
        mapElement->SetAttribute("nextIdx", e->nextIndex);
        mapElement->SetAttribute("animatedObject", e->animatedObject);
        mapElement->SetAttribute("objTileset", H2EnumIndex(e->ObjectTileset()));
        mapElement->SetAttribute("objectIndex", e->objectIndex);
        mapElement->SetAttribute("field_4_1", e->objectLayerBit0);
        mapElement->SetAttribute("field_4_2", e->objectLayerBit1);
        mapElement->SetAttribute("field_4_3", e->objectDrawnAsOverlay);
        mapElement->SetAttribute("field_4_4", e->objectMetadata);
        mapElement->SetAttribute("animatedLateOverlay", e->animatedOverlay);
        mapElement->SetAttribute("hasLateOverlay", e->drawOverlayOnTop);
        mapElement->SetAttribute("tileset", H2EnumIndex(e->OverlayTileset()));
        mapElement->SetAttribute("overlayIndex", e->overlayIndex);
        pElement->InsertEndChild(mapElement);
    }
    pRoot->InsertEndChild(pElement);

    for (i32 i = 0; i < data.world.width * data.world.height; i++) {
        tinyxml2::XMLElement* elem = tempDoc->NewElement("mapRevealed");
        elem->SetAttribute("index", i);
        elem->SetAttribute("x", i % data.world.width);
        elem->SetAttribute("y", i / data.world.width);
        elem->SetAttribute("value", data.world.visibility[i]);
        pRoot->InsertEndChild(elem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_HERO_COUNT); i++) {
        const hero* hro = &data.records.m_heroRecs[i];
        tinyxml2::XMLElement* heroElement = tempDoc->NewElement("hero");
        heroElement->SetAttribute("index", i);
        xml::PushBack(tempDoc, heroElement, "idx", static_cast<i32>(hro->m_id));
        xml::PushBack(tempDoc, heroElement, "spellpoints", hro->m_spellPoints);
        xml::PushBack(tempDoc, heroElement, "ownerIdx", static_cast<i32>(hro->m_owner));
        xml::PushBack(
            tempDoc, heroElement, "aiLastHeroInteractionTurn", hro->m_lastHeroInteractionTurn
        );
        xml::PushBack(
            tempDoc, heroElement, "aiLastHeroInteractionIdx", static_cast<i32>(hro->m_lastInteractionHeroId)
        );
        xml::PushBack(
            tempDoc, heroElement, "aiLastTownInteractionTurn", hro->m_lastTownInteractionTurn
        );
        xml::PushBack(tempDoc, heroElement, "aiLastTownInteractionIdx", static_cast<i32>(hro->m_visitedTownId));
        xml::PushBack(tempDoc, heroElement, "name", hro->m_name);
        xml::PushBack(tempDoc, heroElement, "experience", hro->m_experience);
        xml::PushBack(tempDoc, heroElement, "factionID", static_cast<i32>(hro->m_cursorType.value()));
        xml::PushBack(tempDoc, heroElement, "heroID", static_cast<i32>(hro->m_portrait.value()));
        xml::PushBack(tempDoc, heroElement, "x", hro->m_x);
        xml::PushBack(tempDoc, heroElement, "y", hro->m_y);
        xml::PushBack(tempDoc, heroElement, "aiTargetX", hro->m_destinationX);
        xml::PushBack(tempDoc, heroElement, "aiTargetY", hro->m_destinationY);
        xml::PushBack(tempDoc, heroElement, "aiPatrolX", static_cast<i32>(hro->m_patrolX));
        xml::PushBack(tempDoc, heroElement, "aiPatrolY", static_cast<i32>(hro->m_patrolY));
        xml::PushBack(tempDoc, heroElement, "patrolDistance", static_cast<i32>(hro->m_patrolRadius));
        xml::PushBack(tempDoc, heroElement, "directionFacing", static_cast<i32>(hro->m_direction.value()));
        xml::PushBack(tempDoc, heroElement, "occupiedObjType", static_cast<i32>(hro->m_locationType.value()));
        xml::PushBack(tempDoc, heroElement, "occupiedObjVal", hro->m_occupiedTown);
        xml::PushBack(tempDoc, heroElement, "mobility", hro->m_mobility);
        xml::PushBack(tempDoc, heroElement, "remainingMobility", hro->m_remainingMobility);
        xml::PushBack(tempDoc, heroElement, "oldLevel", hro->m_level);
        xml::PushBack(tempDoc, heroElement, "attack", static_cast<i32>(hro->m_primaryStats[0]));
        xml::PushBack(tempDoc, heroElement, "defense", static_cast<i32>(hro->m_primaryStats[1]));
        xml::PushBack(tempDoc, heroElement, "spellpower", static_cast<i32>(hro->m_primaryStats[2]));
        xml::PushBack(tempDoc, heroElement, "knowledge", static_cast<i32>(hro->m_primaryStats[3]));
        xml::PushBack(tempDoc, heroElement, "field_43", static_cast<i32>(hro->m_primaryStats[4]));
        xml::PushBack(tempDoc, heroElement, "tempMoraleBonuses", static_cast<i32>(hro->m_morale));
        xml::PushBack(tempDoc, heroElement, "tempLuckBonuses", static_cast<i32>(hro->m_luck));
        xml::PushBack(tempDoc, heroElement, "gazeboesVisited", static_cast<i32>(hro->m_gazeboVisits));
        xml::PushBack(tempDoc, heroElement, "fortsVisited", static_cast<i32>(hro->m_fortVisits));
        xml::PushBack(
            tempDoc, heroElement, "witchDoctorHutsVisited", static_cast<i32>(hro->m_witchDoctorVisits)
        );
        xml::PushBack(
            tempDoc, heroElement, "mercenaryCampsVisited", static_cast<i32>(hro->m_mercenaryCampVisits)
        );
        xml::PushBack(
            tempDoc, heroElement, "standingStonesVisited", static_cast<i32>(hro->m_standingStoneVisits)
        );
        xml::PushBack(
            tempDoc, heroElement, "treesOfKnowledgeVisited", static_cast<i32>(hro->m_treeKnowledgeVisits)
        );
        xml::PushBack(tempDoc, heroElement, "xanadusVisited", static_cast<i32>(hro->m_xanaduVisits));
        xml::PushBack(tempDoc, heroElement, "randomSeed", static_cast<i32>(hro->m_randomSeed));
        xml::PushBack(tempDoc, heroElement, "wisdomLastOffered", static_cast<i32>(hro->m_enabled));
        xml::PushBack(tempDoc, heroElement, "numSecSkillsKnown", hro->m_secondarySkillCount);
        xml::PushBack(tempDoc, heroElement, "flags", H2EnumIndex(hro->m_eventFlags));
        xml::PushBack(tempDoc, heroElement, "isCaptain", static_cast<i32>(hro->m_isCaptain));
        xml::PushBack(tempDoc, heroElement, "aiParamFV", hro->m_aiFightValue);

        for (i32 j = 0; j < ARMY_GROUP_SLOT_COUNT; j++) {
            tinyxml2::XMLElement* armyElem = tempDoc->NewElement("army");
            armyElem->SetAttribute("index", j);
            armyElem->SetAttribute("type", static_cast<i32>(hro->m_army.m_creatureTypes[j].value()));
            armyElem->SetAttribute("quantity", hro->m_army.m_quantities[j]);
            heroElement->InsertEndChild(armyElem);
        }

        for (i32 j = 0; j < H2EnumIndex(HERO_SKILL_COUNT); j++) {
            tinyxml2::XMLElement* ssElem = tempDoc->NewElement("secondarySkill");
            ssElem->SetAttribute("index", j);
            ssElem->SetAttribute("level", static_cast<i32>(hro->m_secondarySkills[j]));
            ssElem->SetAttribute("idx", hro->m_secondarySkillOrder[j]);
            heroElement->InsertEndChild(ssElem);
        }

        for (i32 j = 0; j < KB_SPELL_TABLE_CAPACITY; j++) {
            if (hro->m_spells[j]) {
                tinyxml2::XMLElement* spellElem = tempDoc->NewElement("spell");
                spellElem->SetAttribute("idx", j);
                heroElement->InsertEndChild(spellElem);
            }
        }

        for (i32 j = 0; j < HERO_ARTIFACT_SLOT_COUNT; j++) {
            tinyxml2::XMLElement* artElem = tempDoc->NewElement("artifact");
            artElem->SetAttribute("index", j);
            artElem->SetAttribute("id", static_cast<i32>(hro->m_artifacts[j].value()));
            artElem->SetAttribute("spell", static_cast<i32>(hro->m_artifactExtra[j]));
            heroElement->InsertEndChild(artElem);
        }
        pRoot->InsertEndChild(heroElement);
    }

    pElement = tempDoc->NewElement("disallowedBuildings");
    for (i32 j = 0; j < H2EnumIndex(GAME_TOWN_COUNT); j++) {
        for (i32 bit = 0; bit < 32; bit++) {
            if (data.adventure.disallowedBuildings[j][bit]) {
                tinyxml2::XMLElement* buildElem = tempDoc->NewElement("building");
                buildElem->SetAttribute("town", j);
                buildElem->SetAttribute("building", bit);
                pElement->InsertEndChild(buildElem);
            }
        }
    }
    pRoot->InsertEndChild(pElement);

    const auto& adventure = data.adventure;
    WriteRelations(tempDoc, pRoot, "sharedVision", "share", adventure.sharePlayerVision);
    WriteRelations(tempDoc, pRoot, "forcedHeroChases", "chase", adventure.forcedComputerPlayerChases);
    WriteMapVariables(pRoot, data);
    const std::string& script = data.scriptSource;
    if (script.length())
        xml::PushBack(tempDoc, pRoot, "script", script.c_str());
    const std::string path = platform::Files().Resolve(fileName, platform::FileMode::Write);
    return tempDoc->SaveFile(path.c_str());
}

tinyxml2::XMLError XmlFile::Read(const char* fileName, SessionData& data) {
    decodeError.clear();
    const std::string path = platform::Files().Resolve(fileName, platform::FileMode::Read);
    const auto result = tempDoc->LoadFile(path.c_str());
    if (result != tinyxml2::XML_SUCCESS)
        return result;
    try {
        SessionData parsed;
        auto* root = tempDoc->FirstChildElement("ironfist_save");
        if (!root)
            throw std::invalid_argument("Missing ironfist_save root");
        ReadRoot(root, parsed);
        if (!parsed.hasWorld || parsed.world.width <= 0 || parsed.world.height <= 0)
            throw std::invalid_argument("A saved game needs a world map");
        if (parsed.records.m_playerCount <= 0 || parsed.records.m_playerCount > GAME_PLAYER_COUNT
            || parsed.currentPlayer < 0 || parsed.currentPlayer >= parsed.records.m_playerCount)
            throw std::invalid_argument("Invalid current player in saved game");
        data = std::move(parsed);
        return tinyxml2::XML_SUCCESS;
    } catch (const std::exception& error) {
        decodeError = error.what();
        return tinyxml2::XML_ERROR_PARSING;
    }
}

const char* XmlFile::GetError() {
    return decodeError.empty() ? xml::XMLFile::GetError() : decodeError.c_str();
}

script::LuaTable XmlFile::ReadTable(tinyxml2::XMLNode* root) {
    script::LuaTable table;
    if (!root)
        return table;

    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        if (name == "table") {
            std::string tableId = RequiredAttribute(elem, "tableId");
            script::MapVariable& variable = table[tableId];
            variable.type = script::MapVariableType::Table;
            variable.table = ReadTable(elem);
        } else if (name == "tableElement")
            ReadTableElement(elem, table);
    }
    return table;
}

void XmlFile::ReadTableElement(tinyxml2::XMLElement* elem, script::LuaTable& table) {
    script::MapVariable& variable = table[RequiredAttribute(elem, "key")];
    variable.type = script::ParseMapVariableType(RequiredAttribute(elem, "type"));
    if (!script::IsScalar(variable.type))
        throw std::invalid_argument("Invalid scalar map variable type");
    variable.value = RequiredAttribute(elem, "value");
}

void XmlFile::WriteMapVarTable(
    tinyxml2::XMLNode* dest, const std::string& id, const script::LuaTable& table
) {
    tinyxml2::XMLElement* tableElem = tempDoc->NewElement("table");
    tableElem->SetAttribute("tableId", id.c_str());

    for (const auto& [key, variable] : table) {
        if (script::IsTable(variable.type)) {
            WriteMapVarTable(tableElem, key, variable.table);
        } else {
            auto* elem = tempDoc->NewElement("tableElement");
            elem->SetAttribute("key", key.c_str());
            elem->SetAttribute("type", script::MapVariableTypeName(variable.type).c_str());
            elem->SetAttribute("value", variable.value.c_str());
            tableElem->InsertEndChild(elem);
        }
    }
    dest->InsertEndChild(tableElem);
}

void XmlFile::WriteMapVariables(tinyxml2::XMLNode* dest, const SessionData& data) {
    script::LuaTable mapVariables = data.mapVariables;

    // Keep the upstream records for older readers, even without user variables.
    for (i32 i = 0; i != H2EnumIndex(GAME_HERO_COUNT); ++i) {
        for (i32 j = 0; j != H2EnumIndex(GAME_HERO_COUNT); ++j) {
            if (data.adventure.forcedComputerPlayerChases[i][j]) {
                std::string mapVariableId =
                    "_AICHASE_" + std::to_string(i) + "_" + std::to_string(j) + "_";
                script::MapVariable& variable = mapVariables[mapVariableId];
                variable.type = script::MapVariableType::Boolean;
                variable.value = "true";
            }
        }
    }
    for (const auto& [id, variable] : mapVariables) {
        tinyxml2::XMLElement* elem = tempDoc->NewElement("mapVariable");
        elem->SetAttribute("id", id.c_str());
        elem->SetAttribute("type", script::MapVariableTypeName(variable.type).c_str());
        if (script::IsTable(variable.type)) {
            WriteMapVarTable(elem, id, variable.table);
        } else if (script::IsScalar(variable.type)) {
            elem->SetAttribute("value", variable.value.c_str());
        } else {
            throw std::invalid_argument("Invalid map variable type in session data");
        }
        dest->InsertEndChild(elem);
    }
}

void XmlFile::ReadCampaign(tinyxml2::XMLNode* root, i32 campaignType, SessionData& data) {
    data.campaign.savedHeroData.clear();
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (campaignType == CAMPAIGN_ORIGINAL) {
            i32 campId = elem->IntAttribute("campID");
            i32 mapId = elem->IntAttribute("mapID");
            if (name == "campID") xml::QueryCharText(elem, reinterpret_cast<u8*>(&data.records.m_campaignType));
            else if (name == "campIDanother") xml::QueryCharText(elem, reinterpret_cast<u8*>(&data.records.m_campaignStartingSide));
            else if (name == "campMapID") xml::QueryCharText(elem, &data.records.m_campaignScenario);
            else if (name == "campUnknown") xml::QueryCharText(elem, &data.records.m_unknown7d);
            else if (name == "campDaysPlayedCurrent") xml::QueryShortText(elem, &data.records.m_campaignScore);
            else if (name == "campMaybeWon") xml::QueryCharText(elem, &data.records.m_campaignScenarioWon);
            else if (name == "campHasCheated") xml::QueryCharText(elem, &data.records.m_campaignCheated);
            else if (name == "campMapsWon") CheckedSlot(CheckedSlot(data.records.m_campaignScenarioCompleted, campId), mapId) = value;
            else if (name == "campDaysPlayed") CheckedSlot(CheckedSlot(data.records.m_campaignScenarioBonus, campId), mapId) = static_cast<i16>(value);
            else if (name == "campDaysPlayed2") CheckedSlot(CheckedSlot(data.records.m_campaignScenarioDays, campId), mapId) = static_cast<i16>(value);
            else if (name == "campChoices") CheckedSlot(CheckedSlot(data.records.m_campaignChoice, campId), mapId) = value;
            else if (name == "campMapsPlayed") CheckedSlot(CheckedSlot(data.records.m_campaignMapEnabled, campId), mapId) = value;
            else if (name == "campBonuses") CheckedSlot(data.records.m_campaignAwards, index) = value;
            else if (name == "campPlayerCreatures") CheckedSlot(data.records.m_campaignCarryoverCreatureTypes, index) = static_cast<i16>(value);
            else if (name == "campPlayerCreatureQuantities") CheckedSlot(data.records.m_campaignCarryoverCreatureCounts, index) = static_cast<i16>(value);
        } else if (campaignType == CAMPAIGN_EXPANSION) {
            i32 intValue;
        if (name == "campaignID") { elem->QueryIntText(&intValue); data.expansion.m_campaignId = ExpansionCampaignIdFromCode(intValue); }
        else if (name == "currentMapID") { elem->QueryIntText(&intValue); data.expansion.m_currentMap = ExpansionCampaignMapFromCode(intValue); }
            else if (name == "numMaps") elem->QueryIntText(&data.expansion.m_mapCount);
        else if (name == "mightBeScenarioID") { elem->QueryIntText(&intValue); data.expansion.m_viewMap = ExpansionCampaignMapFromCode(intValue); }
            else if (name == "anIntVariable") elem->QueryIntText(&data.expansion.m_viewOnly);
            else if (name == "mapChoice") CheckedSlot(data.expansion.m_mapChoices, index) = value;
            else if (name == "mapsPlayed") CheckedSlot(data.expansion.m_mapsPlayed, index) = value;
            else if (name == "daysPlayed") CheckedSlot(data.expansion.m_mapDays, index) = static_cast<i16>(value);
            else if (name == "awards") CheckedSlot(data.expansion.m_awards, index) = value;
            else if (name == "bonusChoices") CheckedSlot(data.expansion.m_bonusChoices, index) = value;
            else if (name == "savedHero") ReadCampaignSavedHero(elem, data);
        }
    }
}

void XmlFile::ReadCampaignSavedHero(tinyxml2::XMLNode* root, SessionData& data) {
    i32 savedHeroIdx = root->ToElement()->IntAttribute("index");
    state::CampaignState::PartialHeroData* savedHero =
        &data.campaign.savedHeroData[savedHeroIdx];
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "primarySkills") CheckedSlot(savedHero->primarySkills, index) = value;
        if (name == "skillIndex") CheckedSlot(savedHero->skillIndex, index) = value;
        if (name == "secondarySkillLevel") CheckedSlot(savedHero->secondarySkillLevel, index) = value;
        if (name == "spell") CheckedSlot(savedHero->spellsLearned, elem->IntAttribute("idx")) = 1;
        else if (name == "numSecSkillsKnown") elem->QueryIntText(&savedHero->numSecSkillsKnown);
        else if (name == "experience") elem->QueryIntText(&savedHero->experience);
    }
}

void XmlFile::ReadMapHeader(tinyxml2::XMLNode* root, SessionData& data) {
    SMapHeader* mh = &data.records.m_mapHeader;
    i32 lossValueLow = mh->lossConditionValue & 0xff;
    i32 lossValueHigh = mh->lossConditionValue >> 8;
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        i32 intValue = 0;
        if (name == "field_0") {
            elem->QueryIntText(&intValue);
            mh->magic = intValue;
        }
        else if (name == "field_4") {
            elem->QueryIntText(&intValue);
            mh->difficulty = static_cast<u8>(intValue & 0xff);
            mh->unknown5 = static_cast<u8>((intValue >> 8) & 0xff);
        }
        else if (name == "width") xml::QueryCharText(elem, &mh->width);
        else if (name == "height") xml::QueryCharText(elem, &mh->height);
        else if (name == "numPlayers") xml::QueryCharText(elem, &mh->playerCount);
        else if (name == "minHumans") xml::QueryCharText(elem, &mh->minHumanPlayers);
        else if (name == "maxHumans") xml::QueryCharText(elem, &mh->maxHumanPlayers);
        else if (name == "winConditionType") xml::QueryCharText(elem, reinterpret_cast<u8*>(&mh->victoryCondition));
        else if (name == "relatedToWinConditionType") xml::QueryCharText(elem, &mh->computerAlsoWins);
        else if (name == "allowDefeatAllVictory") xml::QueryCharText(elem, &mh->allowNormalVictory);
        else if (name == "winConditionArgumentOrLocX") xml::QueryShortText(elem, reinterpret_cast<i16*>(&mh->victoryConditionValue));
        else if (name == "lossConditionType") xml::QueryCharText(elem, reinterpret_cast<u8*>(&mh->lossCondition));
        else if (name == "lossConditionArgumentOrLocX") {
            elem->QueryIntText(&intValue);
            lossValueLow = intValue & 0xff;
        }
        else if (name == "field_24") {
            elem->QueryIntText(&intValue);
            lossValueHigh = intValue & 0xff;
        }
        else if (name == "noStartingHeroInCastle") xml::QueryCharText(elem, &mh->unknown25);
        else if (name == "winConditionArgumentOrLocY") xml::QueryShortText(elem, reinterpret_cast<i16*>(&mh->victoryTownY));
        else if (name == "lossConditionArgumentOrLocY") xml::QueryShortText(elem, reinterpret_cast<i16*>(&mh->lossTownY));
        else if (name == "relatedToPlayerColorOrSide") xml::QueryShortText(elem, reinterpret_cast<i16*>(&mh->victorySideThreshold));
        else if (name == "name") ReadText(elem, mh->name);
        else if (name == "description") ReadText(elem, mh->description);
        else if (name == "field_1A0") xml::QueryCharText(elem, &mh->rumourCount);
        else if (name == "field_1A1") xml::QueryCharText(elem, &mh->timeEventCount);
        else if (name == "hasPlayer") CheckedSlot(mh->playerEnabled, index) = value;
        else if (name == "playerMayBeHuman") CheckedSlot(mh->playerCanHuman, index) = value;
        else if (name == "playerMayBeComp") CheckedSlot(mh->playerCanComputer, index) = value;
        else if (name == "playerFactions") CheckedSlot(mh->playerRace, index) = static_cast<i8>(value);
    }
    mh->lossConditionValue = static_cast<u16>(lossValueLow | (lossValueHigh << 8));
}

void XmlFile::ReadMap(tinyxml2::XMLNode* root, SessionData& data) {
    data.world.width = root->ToElement()->IntAttribute("width");
    data.world.height = root->ToElement()->IntAttribute("height");
    if (data.world.width < 0 || data.world.height < 0
        || data.world.width > MAP_DIMENSION_XLARGE || data.world.height > MAP_DIMENSION_XLARGE)
        throw std::invalid_argument("Invalid map dimensions");
    const i32 extraCount = root->ToElement()->IntAttribute("numCellExtras");
    if (extraCount < 0 || extraCount > 65536)
        throw std::invalid_argument("Invalid map cell extra count");
    data.world.cells.resize(data.world.width * data.world.height);
    data.world.visibility.resize(data.world.cells.size());
    data.world.extras.resize(extraCount);
    data.hasWorld = true;

    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        if (name == "mapCell") {
            mapCell* cell = &CheckedSlot(data.world.cells, index);
            cell->m_terrainImageIndex = elem->IntAttribute("groundIndex");
            cell->m_animatedObject = elem->IntAttribute("hasObject");
            cell->m_isRoad = elem->IntAttribute("isRoad");
                cell->SetObjectTileset(TilesetIdFromCode(elem->IntAttribute("objTileset")));
            cell->m_objectIndex = elem->IntAttribute("objectIndex");
            cell->m_objectLayerBit0 = elem->IntAttribute("field_4_1");
            cell->m_objectLayerBit1 = elem->IntAttribute("isShadow");
            cell->m_objectDrawnAsOverlay = elem->IntAttribute("field_4_3");
            cell->m_objectMetadata = elem->IntAttribute("extraInfo");
            cell->m_animatedOverlay = elem->IntAttribute("hasOverlay");
            cell->m_drawOverlayOnTop = elem->IntAttribute("hasLateOverlay");
                cell->SetOverlayTileset(TilesetIdFromCode(elem->IntAttribute("overlayTileset")));
            cell->m_overlayIndex = elem->IntAttribute("overlayIndex");
            cell->m_flags = elem->IntAttribute("flags");
            cell->m_triggerType = static_cast<u8>(elem->IntAttribute("objType"));
            cell->m_extraIndex = elem->IntAttribute("extraIdx");
        } else if (name == "mapCellExtra") {
            mapCellExtra* ext = &CheckedSlot(data.world.extras, index);
            ext->nextIndex = elem->IntAttribute("nextIdx");
            ext->animatedObject = elem->IntAttribute("animatedObject");
                ext->SetObjectTileset(TilesetIdFromCode(elem->IntAttribute("objTileset")));
            ext->objectIndex = elem->IntAttribute("objectIndex");
            ext->objectLayerBit0 = elem->IntAttribute("field_4_1");
            ext->objectLayerBit1 = elem->IntAttribute("field_4_2");
            ext->objectDrawnAsOverlay = elem->IntAttribute("field_4_3");
            ext->objectMetadata = elem->IntAttribute("field_4_4");
            ext->animatedOverlay = elem->IntAttribute("animatedLateOverlay");
            ext->drawOverlayOnTop = elem->IntAttribute("hasLateOverlay");
                ext->SetOverlayTileset(TilesetIdFromCode(elem->IntAttribute("tileset")));
            ext->overlayIndex = elem->IntAttribute("overlayIndex");
        }
    }
}

void XmlFile::ReadMapExtra(tinyxml2::XMLNode* root, SessionData& data) {
    auto& bytes = CheckedSlot(data.world.objects, root->ToElement()->IntAttribute("index"));
    bytes.clear();
    for (auto* byte = root->FirstChildElement("ppMapExtra"); byte;
         byte = byte->NextSiblingElement("ppMapExtra")) {
        if (bytes.size() >= 32767)
            throw std::invalid_argument("Map object exceeds engine record capacity");
        bytes.push_back(static_cast<i8>(byte->IntAttribute("value")));
    }
}

void XmlFile::ReadPlayerData(tinyxml2::XMLNode* root, i32 dataIndex, SessionData& data) {
    playerData* pdata = &CheckedSlot(data.records.m_players, dataIndex);
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "color") xml::QueryCharText(elem, &pdata->m_color);
        else if (name == "numHeroes") xml::QueryCharText(elem, &pdata->m_heroCount);
        else if (name == "curHeroIdx") xml::QueryCharText(elem, &pdata->m_currentHero);
        else if (name == "relatedToSomeSortOfHeroCountOrIdx") xml::QueryCharText(elem, &pdata->m_heroLocatorPage);
        else if (name == "hasCheated") xml::QueryCharText(elem, &data.records.m_cheated);
        else if (name == "puzzlePieces") xml::QueryCharText(elem, &pdata->m_cheatValue);
        else if (name == "personality") {
            i32 personality;
            elem->QueryIntText(&personality);
            pdata->m_aiDifficulty = PlayerPersonalityFromCode(personality);
        }
        else if (name == "relatedToMaxOrNumHeroes") xml::QueryCharText(elem, &pdata->m_minimumHeroCount);
        else if (name == "hasEvilFaction") xml::QueryCharText(elem, &pdata->m_evilInterface);
        else if (name == "aiNumberPuzzlePieces") xml::QueryCharText(elem, &pdata->m_ultimateArtifactHintChance);
        else if (name == "aiProbableUltimateArtifactX") xml::QueryCharText(elem, &pdata->m_ultimateArtifactHintX);
        else if (name == "aiProbableUltimateArtifactY") xml::QueryCharText(elem, &pdata->m_ultimateArtifactHintY);
        else if (name == "daysLeftWithoutCastle") xml::QueryCharText(elem, &pdata->m_daysLeft);
        else if (name == "numCastles") xml::QueryCharText(elem, &pdata->m_townCount);
        else if (name == "mightBeCurCastleIdx") xml::QueryCharText(elem, &pdata->m_currentTown);
        else if (name == "relatedToUnknown") xml::QueryCharText(elem, &pdata->m_townLocatorPage);
        else if (name == "barrierTentsVisited") xml::QueryCharText(elem, &pdata->m_barrierTents);
        else if (name == "heroesOwned") CheckedSlot(pdata->m_heroIds, index) = value;
        else if (name == "heroesForPurchase") CheckedSlot(pdata->m_availableHeroIds, index) = value;
        else if (name == "castlesOwned") CheckedSlot(pdata->m_townIds, index) = value;
        else if (name == "resources") CheckedSlot(pdata->m_resources, index) = value;
        else if (name == "resourcesIncome") CheckedSlot(pdata->m_aiData.m_income, index) = value;
        else if (name == "_4_2_1") CheckedSlot(pdata->m_unknownad, index) = value;
    }
}

void XmlFile::ReadHero(tinyxml2::XMLNode* root, i32 heroIndex, SessionData& data) {
    hero* hro = &CheckedSlot(data.records.m_heroRecs, heroIndex);
    // Ironfist's hero::Clear: reset the identity and learned data the XML
    // only writes sparsely.
    hro->m_id = 0;
    hro->m_owner = 0;
    hro->m_x = 0;
    hro->m_y = 0;
    hro->m_cursorType = FactionTypeFromOrdinal(0);
    hro->m_portrait = HeroPortraitFromOrdinal(0);
    CheckedSlot(hro->m_name, 0) = '\0';
    memset(hro->m_spells, 0, sizeof(hro->m_spells));
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        if (name == "idx") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_id));
        else if (name == "spellpoints") xml::QueryShortText(elem, &hro->m_spellPoints);
        else if (name == "ownerIdx") xml::QueryCharText(elem, &hro->m_owner);
        else if (name == "aiLastHeroInteractionTurn") xml::QueryShortText(elem, &hro->m_lastHeroInteractionTurn);
        else if (name == "aiLastHeroInteractionIdx") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_lastInteractionHeroId));
        else if (name == "aiLastTownInteractionTurn") xml::QueryShortText(elem, &hro->m_lastTownInteractionTurn);
        else if (name == "aiLastTownInteractionIdx") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_visitedTownId));
        else if (name == "name") ReadText(elem, hro->m_name);
        else if (name == "experience") elem->QueryIntText(&hro->m_experience);
        else if (name == "factionID") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_cursorType));
        else if (name == "heroID") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_portrait));
        else if (name == "x") elem->QueryIntText(&hro->m_x);
        else if (name == "y") elem->QueryIntText(&hro->m_y);
        else if (name == "aiTargetX") elem->QueryIntText(&hro->m_destinationX);
        else if (name == "aiTargetY") elem->QueryIntText(&hro->m_destinationY);
        else if (name == "aiPatrolX") xml::QueryCharText(elem, &hro->m_patrolX);
        else if (name == "aiPatrolY") xml::QueryCharText(elem, &hro->m_patrolY);
        else if (name == "patrolDistance") xml::QueryCharText(elem, &hro->m_patrolRadius);
        else if (name == "directionFacing") xml::QueryCharText(elem, reinterpret_cast<u8*>(&hro->m_direction));
        else if (name == "occupiedObjType") {
            i16 locationType;
            xml::QueryShortText(elem, &locationType);
            hro->m_locationType = locationType;
        }
        else if (name == "occupiedObjVal") xml::QueryShortText(elem, &hro->m_occupiedTown);
        else if (name == "mobility") elem->QueryIntText(&hro->m_mobility);
        else if (name == "remainingMobility") elem->QueryIntText(&hro->m_remainingMobility);
        else if (name == "oldLevel") xml::QueryShortText(elem, &hro->m_level);
        else if (name == "attack") xml::QueryCharText(elem, &CheckedSlot(hro->m_primaryStats, 0));
        else if (name == "defense") xml::QueryCharText(elem, &CheckedSlot(hro->m_primaryStats, 1));
        else if (name == "spellpower") xml::QueryCharText(elem, &CheckedSlot(hro->m_primaryStats, 2));
        else if (name == "knowledge") xml::QueryCharText(elem, &CheckedSlot(hro->m_primaryStats, 3));
        else if (name == "field_43") xml::QueryCharText(elem, &CheckedSlot(hro->m_primaryStats, 4));
        else if (name == "tempMoraleBonuses") xml::QueryCharText(elem, &hro->m_morale);
        else if (name == "tempLuckBonuses") xml::QueryCharText(elem, &hro->m_luck);
        else if (name == "gazeboesVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_gazeboVisits));
        else if (name == "fortsVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_fortVisits));
        else if (name == "witchDoctorHutsVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_witchDoctorVisits));
        else if (name == "mercenaryCampsVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_mercenaryCampVisits));
        else if (name == "standingStonesVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_standingStoneVisits));
        else if (name == "treesOfKnowledgeVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_treeKnowledgeVisits));
        else if (name == "xanadusVisited") elem->QueryIntText(reinterpret_cast<i32*>(&hro->m_xanaduVisits));
        else if (name == "randomSeed") xml::QueryCharText(elem, &hro->m_randomSeed);
        else if (name == "wisdomLastOffered") xml::QueryCharText(elem, &hro->m_enabled);
        else if (name == "flags") {
            i32 flags;
            elem->QueryIntText(&flags);
            hro->m_eventFlags = HeroEventFlagFromCode(flags);
        }
        else if (name == "isCaptain") xml::QueryCharText(elem, &hro->m_isCaptain);
        else if (name == "aiParamFV") elem->QueryFloatText(&hro->m_aiFightValue);
        else if (name == "army") {
            CheckedSlot(hro->m_army.m_creatureTypes, index) = static_cast<i8>(elem->IntAttribute("type"));
            CheckedSlot(hro->m_army.m_quantities, index) = static_cast<i16>(elem->IntAttribute("quantity"));
        } else if (name == "secondarySkill") {
            CheckedSlot(hro->m_secondarySkills, index) =
                HeroSkillLevelFromCode(elem->IntAttribute("level"));
            CheckedSlot(hro->m_secondarySkillOrder, index) = elem->IntAttribute("idx");
        } else if (name == "numSecSkillsKnown")
            elem->QueryIntText(&hro->m_secondarySkillCount);
        else if (name == "spell") {
            index = elem->IntAttribute("idx");
            if (index >= 0 && index < KB_SPELL_TABLE_CAPACITY)
                CheckedSlot(hro->m_spells, index) = 1;
        }
        else if (name == "artifact") {
            CheckedSlot(hro->m_artifacts, index) = static_cast<i8>(elem->IntAttribute("id"));
            CheckedSlot(hro->m_artifactExtra, index) = static_cast<i8>(elem->IntAttribute("spell"));
        }
    }
}

void XmlFile::ReadTown(tinyxml2::XMLNode* root, i32 townIdx, SessionData& data) {
    town* twn = &CheckedSlot(data.records.m_castleRecs, townIdx);
    i32 turnsOwnedLow = twn->m_turnsOwned & 0xff;
    i32 turnsOwnedHigh = twn->m_turnsOwned >> 8;
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "idx") xml::QueryCharText(elem, &twn->m_id);
        else if (name == "ownerIdx") xml::QueryCharText(elem, &twn->m_owner);
        else if (name == "alignment") xml::QueryCharText(elem, &twn->m_threat);
        else if (name == "factionID") xml::QueryCharText(elem, reinterpret_cast<char*>(&twn->m_type));
        else if (name == "x") xml::QueryCharText(elem, &twn->m_x);
        else if (name == "y") xml::QueryCharText(elem, &twn->m_y);
        else if (name == "buildDockRelated") xml::QueryCharText(elem, &twn->m_boatX);
        else if (name == "boatCell") xml::QueryCharText(elem, &twn->m_boatY);
        else if (name == "visitingHeroIdx") xml::QueryCharText(elem, &twn->m_occupyingHeroId);
        else if (name == "buildingsBuiltFlags") {
            u32 buildings;
            elem->QueryUnsignedText(&buildings);
            twn->m_buildings = buildings;
        }
        else if (name == "mageGuildLevel") xml::QueryCharText(elem, &twn->m_buildState);
        else if (name == "field_1D") xml::QueryCharText(elem, &twn->m_unknown1d);
        else if (name == "exists") xml::QueryCharText(elem, &twn->m_onMap);
        else if (name == "mayNotBeUpgradedToCastle") xml::QueryCharText(elem, &twn->m_mayNotUpgradeToCastle);
        else if (name == "field_38") xml::QueryCharText(elem, &twn->m_formation);
        else if (name == "playerPos") xml::QueryCharText(elem, &twn->m_originalOwner);
        else if (name == "extraIdx") xml::QueryShortText(elem, reinterpret_cast<i16*>(&twn->m_extraIndex));
        else if (name == "field_55") {
            i16 low;
            xml::QueryShortText(elem, &low);
            turnsOwnedLow = low & 0xff;
        }
        else if (name == "field_63") {
            i16 high;
            xml::QueryShortText(elem, &high);
            turnsOwnedHigh = high & 0xff;
        }
        else if (name == "name") ReadText(elem, twn->m_name);
        else if (name == "garrisonCreature") {
            CheckedSlot(twn->m_army.m_creatureTypes, index) = static_cast<i8>(elem->IntAttribute("type"));
            CheckedSlot(twn->m_army.m_quantities, index) = static_cast<i16>(elem->IntAttribute("quantity"));
        } else if (name == "mageGuildSpell") {
            i32 level = elem->IntAttribute("level");
            i32 idx = elem->IntAttribute("idx");
            i32 spell = elem->IntAttribute("spell");
            CheckedSlot(CheckedSlot(twn->m_spells, level), idx) = static_cast<i8>(spell);
        } else if (name == "numCreaturesInDwelling")
            CheckedSlot(twn->m_garrison, index) = static_cast<i16>(value);
        else if (name == "numSpellsOfLevel")
            CheckedSlot(twn->m_spellCounts, index + TOWN_MAGE_GUILD_FIRST_LEVEL) = static_cast<i8>(value);
    }
    twn->m_turnsOwned = static_cast<u16>(turnsOwnedLow | (turnsOwnedHigh << 8));
}

void XmlFile::ReadRoot(tinyxml2::XMLNode* root, SessionData& data) {
    if (auto* map = root->FirstChildElement("map"))
        ReadMap(map, data);
    if (auto* countElement = root->FirstChildElement("iMaxMapExtra")) {
        const i32 count = countElement->IntText();
        if (count < 0 || count > 65536)
            throw std::invalid_argument("Invalid map object count");
        data.world.objects.resize(count);
    }
    if (auto* campaign = root->FirstChildElement("campaignType"))
        data.campaignType = campaign->IntText();
    Relations legacyChases;
    for (tinyxml2::XMLNode* child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "sharedVision") {
            for (const auto& [source, destination] : ReadRelations(elem, "share", GAME_PLAYER_COUNT))
                data.adventure.sharePlayerVision[source][destination] = true;
            data.hasSharedVision = true;
        } else if (name == "forcedHeroChases") {
            for (const auto& [source, destination] : ReadRelations(elem, "chase", GAME_HERO_COUNT))
                data.adventure.forcedComputerPlayerChases[source][destination] = true;
            data.hasForcedChases = true;
        } else if (name == "allowAIArmySharing") {
            bool allow = true;
            elem->QueryBoolText(&allow);
            data.adventure.allowAIArmySharing = allow;
            data.hasAIArmySharing = true;
        } else if (name == "disallowedBuildings") {
            data.hasBuildingBans = true;
            for (tinyxml2::XMLNode* build = elem->FirstChildElement(); build;
                 build = build->NextSiblingElement()) {
                tinyxml2::XMLElement* buildElem = build->ToElement();
                const i32 townIndex = buildElem->IntAttribute("town");
                if (townIndex >= 0 && townIndex < GAME_TOWN_COUNT) {
                    const i32 building = buildElem->IntAttribute("building");
                    if (building >= 0 && building < 32)
                        data.adventure.disallowedBuildings[townIndex][building] = true;
                }
            }
        }
        else if (name == "mapWidth" || name == "mapHeight") {}
        else if (name == "gameDifficulty") xml::QueryShortText(elem, &data.records.m_difficultyRating);
        else if (name == "monthType") {
            i32 monthType;
            elem->QueryIntText(&monthType);
            data.monthType = CalendarPeriodTypeFromCode(monthType);
        }
        else if (name == "monthTypeExtra") elem->QueryIntText(&data.monthExtra);
        else if (name == "weekType") {
            i32 weekType;
            elem->QueryIntText(&weekType);
            data.weekType = CalendarPeriodTypeFromCode(weekType);
        }
        else if (name == "weekTypeExtra") elem->QueryIntText(&data.weekExtra);
        else if (name == "giMapChangeCtr") {
            elem->QueryIntText(&data.mapChangeCounter);
        }
        else if (name == "numPlayers") xml::QueryCharText(elem, &data.records.m_playerCount);
        else if (name == "giCurPlayer") elem->QueryIntText(&data.currentPlayer);
        else if (name == "couldBeNumDefeatedPlayers") xml::QueryCharText(elem, &data.records.m_deadPlayerCount);
        else if (name == "day") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_day));
        else if (name == "week") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_week));
        else if (name == "month") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_month));
        else if (name == "numObelisks") xml::QueryCharText(elem, &data.records.m_obeliskCount);
        else if (name == "ultimateArtifactLocX") xml::QueryCharText(elem, &data.records.m_ultimateArtifactX);
        else if (name == "ultimateArtifactLocY") xml::QueryCharText(elem, &data.records.m_ultimateArtifactY);
        else if (name == "ultimateArtifactIdx") xml::QueryCharText(elem, reinterpret_cast<i8*>(&data.records.m_ultimateArtifactId));
        else if (name == "currentRumor") ReadText(elem, data.records.m_rumour);
        else if (name == "numRumors") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_rumourEventCount));
        else if (name == "numEvents") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_timeEventCount));
        else if (name == "numMapEvents") xml::QueryShortText(elem, reinterpret_cast<i16*>(&data.records.m_mapEventCount));
        else if (name == "iMaxMapExtra") {}
        else if (name == "difficulty") xml::QueryCharText(elem, reinterpret_cast<i8*>(&data.records.m_difficulty));
        else if (name == "mapFilename") {
            ReadText(elem, data.records.m_mapFilename);
        }
        else if (name == "relatedToNewGameSelection") xml::QueryCharText(elem, &data.records.m_selectedSetupPlayer);
        else if (name == "relatedToNewGameInit") xml::QueryCharText(elem, &data.records.m_newGameInitialized);
        else if (name == "numHumanPlayers") xml::QueryCharText(elem, &data.records.m_newGameHumanCount);
        else if (name == "gbIAmGreatest") elem->QueryIntText(&data.greatestPlayer);
        else if (name == "campaignType") {}
        else if (name == "expansionMap") data.expansionMap = elem->BoolText();
        else if (name == "mapHeader") ReadMapHeader(elem, data);
        else if (name == "playerNames") {
            i32 playerIndex;
            if (elem->QueryIntAttribute("index", &playerIndex) == tinyxml2::XML_SUCCESS
                && playerIndex >= 0 && playerIndex < GAME_PLAYER_COUNT) {
                // WriteArray stores names in attributes. Accept the text form
                // from older/handwritten saves as a fallback.
                const char* playerName = elem->Attribute("value");
                if (!playerName)
                    playerName = elem->GetText();
                utf8::Copy(CheckedSlot(data.playerNames, playerIndex), sizeof(CheckedSlot(data.playerNames, playerIndex)), playerName);
            }
        }
        else if (name == "deadPlayers") CheckedSlot(data.records.m_playerDead, index) = value;
        else if (name == "alivePlayers") CheckedSlot(data.humanPlayers, index) = value;
        else if (name == "heroHireStatus") CheckedSlot(data.records.m_availableHeroes, index) = value;
        else if (name == "relatedToPlayerPosAndColor") CheckedSlot(data.records.m_setupPlayerColor, index) = value;
        else if (name == "playerHandicap") CheckedSlot(data.records.m_playerHandicap, index) = value;
        else if (name == "newGameSelectedFaction") CheckedSlot(data.records.m_setupPlayerRace, index) = FactionTypeFromCode(value);
        else if (name == "somePlayerCodeOr10IfMayBeHuman") CheckedSlot(data.records.m_setupPlayerNetworkId, index) = value;
        else if (name == "somePlayerNumData") CheckedSlot(data.records.m_setupPlayerType, index) = value;
        else if (name == "field_47C") CheckedSlot(data.records._pad_0x47c, index) = value;
        else if (name == "field_2773") CheckedSlot(data.records.m_castleOwners, index) = value;
        else if (name == "builtToday") CheckedSlot(data.records.m_dailyEventFlags, index) = value;
        else if (name == "field_60A6") CheckedSlot(data.records.m_mineOwners, index) = value;
        else if (name == "randomArtifacts") data.generatedArtifacts.push_back(value);
        else if (name == "boatBuilt") CheckedSlot(data.records.m_boatSlots, index) = value;
        else if (name == "obeliskVisitedMasks") CheckedSlot(data.records.m_obeliskVisitors, index) = value;
        else if (name == "field_637D") CheckedSlot(data.records.m_defaultPlayerNames, index) = value;
        else if (name == "rumorIndices") CheckedSlot(data.records.m_rumourEventIndices, index) = value;
        else if (name == "eventIndices") {
            if (index < GAME_TIME_EVENT_CAPACITY)
                CheckedSlot(data.records.m_timeEventIndices, index) = value;
        }
        else if (name == "mapEventIndices") {
            if (index < GAME_MAP_EVENT_CAPACITY)
                CheckedSlot(data.records.m_mapEventIndices, index) = value;
        }
        else if (name == "mapRevealed") CheckedSlot(data.world.visibility, index) = value;
        else if (name == "mine") {
            mineRecord* m = &CheckedSlot(data.records.m_mines, index);
            m->x = elem->IntAttribute("x");
            m->y = elem->IntAttribute("y");
            m->id = elem->IntAttribute("field_0");
            m->owner = elem->IntAttribute("owner");
            m->resourceType = static_cast<i8>(elem->IntAttribute("type"));
            m->guardianType = static_cast<i8>(elem->IntAttribute("guardianType"));
            m->guardianCount = elem->IntAttribute("guardianQty");
        }
        else if (name == "boat") {
            boatRecord* b = &CheckedSlot(data.records.m_boats, index);
            b->id = elem->IntAttribute("idx");
            b->x = elem->IntAttribute("x");
            b->y = elem->IntAttribute("y");
            b->direction = static_cast<i8>(elem->IntAttribute("direction"));
            b->savedTriggerType = static_cast<u8>(elem->IntAttribute("underlyingObjType"));
            b->savedEventData = elem->IntAttribute("underlyingObjExtra");
            b->heroId = static_cast<i8>(
                (elem->IntAttribute("heroIdx") & 0x7f) | (elem->IntAttribute("isEmpty") << 7)
            );
            b->owner = elem->IntAttribute("owner");
        }
        else if (name == "script") {
            const char* script = elem->GetText();
            if (script)
                data.scriptSource = script;
        }
        else if (name == "mapExtra") ReadMapExtra(elem, data);
        else if (name == "playerData") ReadPlayerData(elem, index, data);
        else if (name == "town") ReadTown(elem, index, data);
        else if (name == "map") {}
        else if (name == "hero") ReadHero(elem, index, data);
        else if (name == "campaign") ReadCampaign(elem, data.campaignType, data);
        else if (name == "campaignMetadata") {
            CampaignDefinition definition;
            std::string error;
            if (data.campaignDefinition || !ParseCampaignDefinition(elem, definition, error))
                throw std::invalid_argument(error.empty() ? "Duplicate campaign definition" : error);
            data.campaignDefinition = std::move(definition);
        }
        else if (name == "mapVariable") {
            std::string mapVariableId = RequiredAttribute(elem, "id");

            script::MapVariableType mapVariableType = script::ParseMapVariableType(RequiredAttribute(elem, "type"));
            i32 x;
            i32 y;
            i32 end = 0;
            if ((mapVariableType == script::MapVariableType::Boolean)
                && (sscanf(mapVariableId.c_str(), "_AICHASE_%d_%d_%n", &x, &y, &end) == 2)
                && static_cast<size_t>(end) == mapVariableId.size()) {
                const std::string chaseValue = xml::QueryTextAttribute(elem, "value");
                if (chaseValue == "true" || chaseValue == "1")
                    legacyChases.emplace_back(x, y);
                // These are engine records, not variables to inject into Lua.
                continue;
            }
            script::MapVariable& variable = data.mapVariables[mapVariableId];
            variable.type = mapVariableType;
            if (script::IsTable(mapVariableType)) {
                variable.table = ReadTable(elem->FirstChild());
            } else if (script::IsScalar(mapVariableType)) {
                variable.value = RequiredAttribute(elem, "value");
            } else {
                throw std::invalid_argument("Invalid map variable type: " + mapVariableId);
            }
        }
    }

    if (!data.hasForcedChases) {
        for (const auto& [source, destination] : legacyChases) {
            if (source >= 0 && source < GAME_HERO_COUNT && destination >= 0 && destination < GAME_HERO_COUNT)
                data.adventure.forcedComputerPlayerChases[source][destination] = true;
        }
    }
}

} // namespace ironfist::save
