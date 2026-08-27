#include <IRONFIST/save_xml.h>

#include <cstdio>
#include <cstring>
#include <fcntl.h>

#include <BASE/Misc.h>
#include <BASE/Utf8.h>
#include <PLATFORM/File.h>
#include <PLATFORM/Platform.h>
#include <PLATFORM/Strings.h>
#include <SOURCE/netwin.h>
#include <string.h>

#include <map>
#include <string>
#include <vector>

#include <IRONFIST/artifacts.h>
#include <IRONFIST/campaigns.h>
#include <IRONFIST/dialog.h>
#include <IRONFIST/state.h>
#include <IRONFIST/hooks.h>
#include <IRONFIST/runtime.h>
#include <IRONFIST/scripting.h>

#include <EDITOR/fullMap.h>
#include <EDITOR/mapcell.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/GAME.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/playerData.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/town.h>
#include <SOURCE/X_GLOBAL.h>

namespace ironfist::save {

i32 GetCampaignType(void) {
    if (gbInCampaign)
        return CAMPAIGN_ORIGINAL;
    if (xIsPlayingExpansionCampaign)
        return CAMPAIGN_EXPANSION;
    return CAMPAIGN_NONE;
}

tinyxml2::XMLError XmlFile::Save(const char* fileName) {
    tinyxml2::XMLNode* pRoot = tempDoc->NewElement("ironfist_save");
    tempDoc->InsertFirstChild(pRoot);

    xml::PushBack(tempDoc, pRoot, "allowAIArmySharing", gpGame->IsAIArmySharingAllowed());
    xml::PushBack(tempDoc, pRoot, "mapWidth", gpGame->m_worldMap.width);
    xml::PushBack(tempDoc, pRoot, "mapHeight", gpGame->m_worldMap.height);
    xml::PushBack(tempDoc, pRoot, "gameDifficulty", gpGame->m_difficultyRating);
    xml::PushBack(tempDoc, pRoot, "monthType", H2EnumIndex(giMonthType));
    xml::PushBack(tempDoc, pRoot, "monthTypeExtra", giMonthTypeExtra);
    xml::PushBack(tempDoc, pRoot, "weekType", H2EnumIndex(giWeekType));
    xml::PushBack(tempDoc, pRoot, "weekTypeExtra", giWeekTypeExtra);
    xml::PushBack(tempDoc, pRoot, "giMapChangeCtr", giMapChangeCtr);
    xml::PushBack(tempDoc, pRoot, "numPlayers", static_cast<i32>(gpGame->m_playerCount));
    xml::PushBack(tempDoc, pRoot, "giCurPlayer", giCurPlayer);
    xml::PushBack(tempDoc, pRoot, "couldBeNumDefeatedPlayers", static_cast<i32>(gpGame->m_deadPlayerCount));
    xml::PushBack(tempDoc, pRoot, "day", gpGame->m_day);
    xml::PushBack(tempDoc, pRoot, "week", gpGame->m_week);
    xml::PushBack(tempDoc, pRoot, "month", gpGame->m_month);
    xml::PushBack(tempDoc, pRoot, "numObelisks", static_cast<i32>(gpGame->m_obeliskCount));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactLocX", static_cast<i32>(gpGame->m_ultimateArtifactX));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactLocY", static_cast<i32>(gpGame->m_ultimateArtifactY));
    xml::PushBack(tempDoc, pRoot, "ultimateArtifactIdx", static_cast<i32>(gpGame->m_ultimateArtifactId.value()));
    xml::PushBack(tempDoc, pRoot, "currentRumor", gpGame->m_rumour);
    xml::PushBack(tempDoc, pRoot, "numRumors", gpGame->m_rumourEventCount);
    xml::PushBack(tempDoc, pRoot, "numEvents", gpGame->m_timeEventCount);
    xml::PushBack(tempDoc, pRoot, "numMapEvents", gpGame->m_mapEventCount);
    xml::PushBack(tempDoc, pRoot, "iMaxMapExtra", iMaxMapExtra);
    xml::PushBack(tempDoc, pRoot, "difficulty", static_cast<i32>(gpGame->m_difficulty.value()));
    xml::PushBack(tempDoc, pRoot, "mapFilename", gpGame->m_mapFilename);
    xml::PushBack(tempDoc, pRoot, "relatedToNewGameSelection", static_cast<i32>(gpGame->m_selectedSetupPlayer));
    xml::PushBack(tempDoc, pRoot, "relatedToNewGameInit", static_cast<i32>(gpGame->m_newGameInitialized));
    xml::PushBack(tempDoc, pRoot, "numHumanPlayers", static_cast<i32>(gpGame->m_newGameHumanCount));
    xml::PushBack(tempDoc, pRoot, "gbIAmGreatest", gbIAmGreatest);
    i32 campaignType = GetCampaignType();
    xml::PushBack(tempDoc, pRoot, "campaignType", campaignType);

    tinyxml2::XMLElement* pElement;
    if (campaignType) {
        pElement = tempDoc->NewElement("campaign");
        if (campaignType == CAMPAIGN_ORIGINAL) {
            xml::PushBack(tempDoc, pElement, "campID", static_cast<i32>(gpGame->m_campaignType.value()));
            xml::PushBack(tempDoc, pElement, "campIDanother", static_cast<i32>(gpGame->m_campaignStartingSide.value()));
            xml::PushBack(tempDoc, pElement, "campMapID", static_cast<i32>(gpGame->m_campaignScenario));
            xml::PushBack(tempDoc, pElement, "campUnknown", static_cast<i32>(gpGame->m_unknown7d));
            xml::PushBack(tempDoc, pElement, "campDaysPlayedCurrent", gpGame->m_campaignScore);
            xml::PushBack(tempDoc, pElement, "campMaybeWon", static_cast<i32>(gpGame->m_campaignScenarioWon));
            xml::PushBack(tempDoc, pElement, "campHasCheated", static_cast<i32>(gpGame->m_campaignCheated));

            WriteCampaignDDArray(
                tempDoc, pElement, "campMapsWon", gpGame->m_campaignScenarioCompleted
            );
            WriteCampaignDDArray(
                tempDoc, pElement, "campDaysPlayed", gpGame->m_campaignScenarioBonus
            );
            WriteCampaignDDArray(
                tempDoc, pElement, "campDaysPlayed2", gpGame->m_campaignScenarioDays
            );
            WriteCampaignDDArray(tempDoc, pElement, "campChoices", gpGame->m_campaignChoice);
            WriteCampaignDDArray(
                tempDoc, pElement, "campMapsPlayed", gpGame->m_campaignMapEnabled
            );

            xml::WriteArray(tempDoc, pElement, "campBonuses", gpGame->m_campaignAwards);
            xml::WriteArray(
                tempDoc, pElement, "campPlayerCreatures", gpGame->m_campaignCarryoverCreatureTypes
            );
            xml::WriteArray(
                tempDoc, pElement, "campPlayerCreatureQuantities",
                gpGame->m_campaignCarryoverCreatureCounts
            );
        } else if (campaignType == CAMPAIGN_EXPANSION) {
            xml::PushBack(tempDoc, pElement, "campaignID", H2EnumIndex(xCampaign.m_campaignId));
            xml::PushBack(tempDoc, pElement, "currentMapID", H2EnumIndex(xCampaign.m_currentMap));
            xml::PushBack(tempDoc, pElement, "numMaps", xCampaign.m_mapCount);
            xml::PushBack(tempDoc, pElement, "mightBeScenarioID", H2EnumIndex(xCampaign.m_viewMap));
            xml::PushBack(tempDoc, pElement, "anIntVariable", xCampaign.m_viewOnly);

            xml::WriteArray(tempDoc, pElement, "mapChoice", xCampaign.m_mapChoices);
            xml::WriteArray(tempDoc, pElement, "mapsPlayed", xCampaign.m_mapsPlayed);
            xml::WriteArray(tempDoc, pElement, "daysPlayed", xCampaign.m_mapDays);
            xml::WriteArray(tempDoc, pElement, "awards", xCampaign.m_awards);
            xml::WriteArray(tempDoc, pElement, "bonusChoices", xCampaign.m_bonusChoices);

            // saved hero for autosaved saves
            for (auto& i : state::Get().campaign.savedHeroData) {
                state::CampaignState::PartialHeroData* savedHero = &i.second;
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
            WriteCampaignMetadata(tempDoc, pRoot);
        }
        pRoot->InsertEndChild(pElement);
    }

    pElement = tempDoc->NewElement("mapHeader");
    SMapHeader* mh = &gpGame->m_mapHeader;
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

    xml::WriteArray(tempDoc, pRoot, "playerNames", cPlayerNames);
    xml::WriteArray(tempDoc, pRoot, "deadPlayers", gpGame->m_playerDead);

    bchar playerAlive[H2EnumIndex(GAME_PLAYER_COUNT)];
    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); ++i) {
        playerAlive[i] = gbHumanPlayer[i];
        if (gpGame->m_playerDead[i])
            playerAlive[i] = false;
    }

    xml::WriteArray(tempDoc, pRoot, "alivePlayers", playerAlive);
    xml::WriteArray(tempDoc, pRoot, "heroHireStatus", gpGame->m_availableHeroes);
    xml::WriteArray(tempDoc, pRoot, "relatedToPlayerPosAndColor", gpGame->m_setupPlayerColor);
    xml::WriteArray(tempDoc, pRoot, "playerHandicap", gpGame->m_playerHandicap);
    xml::WriteArray(tempDoc, pRoot, "newGameSelectedFaction", gpGame->m_setupPlayerRace);
    xml::WriteArray(
        tempDoc, pRoot, "somePlayerCodeOr10IfMayBeHuman", gpGame->m_setupPlayerNetworkId
    );
    xml::WriteArray(tempDoc, pRoot, "somePlayerNumData", gpGame->m_setupPlayerType);
    xml::WriteArray(tempDoc, pRoot, "field_47C", gpGame->_pad_0x47c);
    xml::WriteArray(tempDoc, pRoot, "field_2773", gpGame->m_castleOwners);
    xml::WriteArray(tempDoc, pRoot, "builtToday", gpGame->m_dailyEventFlags);
    xml::WriteArray(tempDoc, pRoot, "field_60A6", gpGame->m_mineOwners);
    xml::WriteArray(tempDoc, pRoot, "randomArtifacts", SerializeGeneratedArtifacts());
    xml::WriteArray(tempDoc, pRoot, "boatBuilt", gpGame->m_boatSlots);
    xml::WriteArray(tempDoc, pRoot, "obeliskVisitedMasks", gpGame->m_obeliskVisitors);
    xml::WriteArray(tempDoc, pRoot, "field_637D", gpGame->m_defaultPlayerNames);
    xml::WriteArray(tempDoc, pRoot, "rumorIndices", gpGame->m_rumourEventIndices);
    xml::WriteArray(tempDoc, pRoot, "eventIndices", gpGame->m_timeEventIndices);
    xml::WriteArray(tempDoc, pRoot, "mapEventIndices", gpGame->m_mapEventIndices);

    for (i32 i = 1; i < iMaxMapExtra; i++) {
        tinyxml2::XMLElement* extraElem = tempDoc->NewElement("mapExtra");
        extraElem->SetAttribute("index", i);
        for (i32 j = 0; j < pwSizeOfMapExtra[i]; j++) {
            tinyxml2::XMLElement* ppMapExtraElem = tempDoc->NewElement("ppMapExtra");
            if (ppMapExtra[i]) {
                ppMapExtraElem->SetAttribute("value", *(static_cast<char*>(ppMapExtra[i]) + j));
            } else {
                ppMapExtraElem->SetAttribute("value", 0);
            }
            extraElem->InsertEndChild(ppMapExtraElem);
        }
        pRoot->InsertEndChild(extraElem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++) {
        playerData* player = &gpGame->m_players[i];
        tinyxml2::XMLElement* playerElem = tempDoc->NewElement("playerData");
        playerElem->SetAttribute("index", i);

        xml::PushBack(tempDoc, playerElem, "color", static_cast<i32>(player->m_color));
        xml::PushBack(tempDoc, playerElem, "numHeroes", static_cast<i32>(player->m_heroCount));
        xml::PushBack(tempDoc, playerElem, "curHeroIdx", static_cast<i32>(player->m_currentHero));
        xml::PushBack(
            tempDoc, playerElem, "relatedToSomeSortOfHeroCountOrIdx",
            static_cast<i32>(player->m_heroLocatorPage)
        );
        xml::PushBack(tempDoc, playerElem, "hasCheated", static_cast<i32>(gpGame->m_cheated));
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
        town* twn = &gpGame->m_castleRecs[i];
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
        mineRecord* m = &gpGame->m_mines[i];
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
        boatRecord* b = &gpGame->m_boats[i];
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
    fullMap* map = &gpGame->m_worldMap;
    pElement->SetAttribute("width", map->width);
    pElement->SetAttribute("height", map->height);
    pElement->SetAttribute("numCellExtras", map->extraCount);
    for (i32 i = 0; i < map->height * map->width; i++) {
        mapCell* c = &map->cells[i];
        tinyxml2::XMLElement* mapElement = tempDoc->NewElement("mapCell");
        mapElement->SetAttribute("index", i);
        mapElement->SetAttribute("groundIndex", c->m_terrainImageIndex);
        mapElement->SetAttribute("hasObject", c->m_animatedObject);
        mapElement->SetAttribute("isRoad", c->m_isRoad);
        mapElement->SetAttribute("objTileset", static_cast<i32>(c->m_objectTileset));
        mapElement->SetAttribute("objectIndex", c->m_objectIndex);
        mapElement->SetAttribute("field_4_1", c->m_objectLayerBit0);
        mapElement->SetAttribute("isShadow", c->m_objectLayerBit1);
        mapElement->SetAttribute("field_4_3", c->m_objectDrawnAsOverlay);
        mapElement->SetAttribute("extraInfo", c->m_objectMetadata);
        mapElement->SetAttribute("hasOverlay", c->m_animatedOverlay);
        mapElement->SetAttribute("hasLateOverlay", c->m_drawOverlayOnTop);
        mapElement->SetAttribute("overlayTileset", static_cast<i32>(c->m_overlayTileset));
        mapElement->SetAttribute("overlayIndex", c->m_overlayIndex);
        mapElement->SetAttribute("flags", c->m_flags);
        mapElement->SetAttribute("objType", static_cast<i32>(c->m_triggerType.value()));
        mapElement->SetAttribute("extraIdx", c->m_extraIndex);
        pElement->InsertEndChild(mapElement);
    }

    for (i32 i = 0; i < map->extraCount; i++) {
        mapCellExtra* e = &map->extras[i];
        tinyxml2::XMLElement* mapElement = tempDoc->NewElement("mapCellExtra");
        mapElement->SetAttribute("index", i);
        mapElement->SetAttribute("nextIdx", e->nextIndex);
        mapElement->SetAttribute("animatedObject", e->animatedObject);
        mapElement->SetAttribute("objTileset", static_cast<i32>(e->objectTileset));
        mapElement->SetAttribute("objectIndex", e->objectIndex);
        mapElement->SetAttribute("field_4_1", e->objectLayerBit0);
        mapElement->SetAttribute("field_4_2", e->objectLayerBit1);
        mapElement->SetAttribute("field_4_3", e->objectDrawnAsOverlay);
        mapElement->SetAttribute("field_4_4", e->objectMetadata);
        mapElement->SetAttribute("animatedLateOverlay", e->animatedOverlay);
        mapElement->SetAttribute("hasLateOverlay", e->drawOverlayOnTop);
        mapElement->SetAttribute("tileset", static_cast<i32>(e->overlayTileset));
        mapElement->SetAttribute("overlayIndex", e->overlayIndex);
        pElement->InsertEndChild(mapElement);
    }
    pRoot->InsertEndChild(pElement);

    for (i32 i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
        tinyxml2::XMLElement* elem = tempDoc->NewElement("mapRevealed");
        elem->SetAttribute("index", i);
        elem->SetAttribute("x", i % MAP_WIDTH);
        elem->SetAttribute("y", i / MAP_WIDTH);
        elem->SetAttribute("value", mapExtra[i]);
        pRoot->InsertEndChild(elem);
    }

    for (i32 i = 0; i < H2EnumIndex(GAME_HERO_COUNT); i++) {
        hero* hro = &gpGame->m_heroRecs[i];
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
            if (gpGame->m_castleRecs[j].IsBuildingDisallowed(bit)) {
                tinyxml2::XMLElement* buildElem = tempDoc->NewElement("building");
                buildElem->SetAttribute("town", j);
                buildElem->SetAttribute("building", bit);
                pElement->InsertEndChild(buildElem);
            }
        }
    }
    pRoot->InsertEndChild(pElement);

    WriteMapVariables(pRoot);
    std::string script = script::ScriptContents(gMapName);
    if (script.length())
        xml::PushBack(tempDoc, pRoot, "script", script.c_str());
    const std::string path = platform::Files().Resolve(fileName, platform::FileMode::Write);
    return tempDoc->SaveFile(path.c_str());
}

tinyxml2::XMLError XmlFile::Read(const char* fileName) {
    const std::string path = platform::Files().Resolve(fileName, platform::FileMode::Read);
    tinyxml2::XMLError eResult = tempDoc->LoadFile(path.c_str());
    if (!eResult) {
        tinyxml2::XMLNode* pRoot = tempDoc->FirstChild();
        ReadRoot(pRoot);
    }
    return eResult;
}

script::LuaTable XmlFile::ReadTable(tinyxml2::XMLNode* root) {
    script::LuaTable table;

    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        if (name == "table") {
            std::string tableId = elem->Attribute("tableId");
            script::MapVariable& variable = table[tableId];
            variable.type = script::MapVariableType::Table;
            variable.table = ReadTable(elem);
        } else if (name == "tableElement")
            ReadTableElement(elem, table);
    }
    return table;
}

void XmlFile::ReadTableElement(tinyxml2::XMLElement* elem, script::LuaTable& table) {
    script::MapVariable& variable = table[elem->Attribute("key")];
    variable.type = script::ParseMapVariableType(elem->Attribute("type"));
    variable.value = elem->Attribute("value");
}

void XmlFile::WriteMapVarTable(
    tinyxml2::XMLNode* dest, const std::string& id, const script::LuaTable& table
) {
    tinyxml2::XMLElement* tableElem = tempDoc->NewElement("table");
    tableElem->SetAttribute("tableId", id.c_str());

    for (const auto& [key, variable] : table) {
        tinyxml2::XMLElement* elem = tableElem;
        if (script::IsTable(variable.type)) {
            WriteMapVarTable(tableElem, key, variable.table);
        } else {
            elem = tempDoc->NewElement("tableElement");
            elem->SetAttribute("key", key.c_str());
            elem->SetAttribute("type", script::MapVariableTypeName(variable.type).c_str());
            elem->SetAttribute("value", variable.value.c_str());
        }
        tableElem->InsertEndChild(elem);
    }
    dest->InsertEndChild(tableElem);
}

void XmlFile::WriteMapVariables(tinyxml2::XMLNode* dest) {
    script::LuaTable mapVariables = script::LoadMapVariablesFromLua();
    if (!mapVariables.size())
        return;

    for (i32 i = 0; i != H2EnumIndex(GAME_HERO_COUNT); ++i) {
        for (i32 j = 0; j != H2EnumIndex(GAME_HERO_COUNT); ++j) {
            if (gpGame->IsHeroChaseForced(i, j)) {
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
            DisplayError("Wrong Type created by GetMapVariables", "In function SaveMapVariables");
        }
        dest->InsertEndChild(elem);
    }
}

void XmlFile::ReadCampaign(tinyxml2::XMLNode* root, i32 campaignType) {
    state::Get().campaign.savedHeroData.clear();
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (campaignType == CAMPAIGN_ORIGINAL) {
            i32 campId = elem->IntAttribute("campID");
            i32 mapId = elem->IntAttribute("mapID");
            if (name == "campID") xml::QueryCharText(elem, reinterpret_cast<u8*>(&gpGame->m_campaignType));
            else if (name == "campIDanother") xml::QueryCharText(elem, reinterpret_cast<u8*>(&gpGame->m_campaignStartingSide));
            else if (name == "campMapID") xml::QueryCharText(elem, &gpGame->m_campaignScenario);
            else if (name == "campUnknown") xml::QueryCharText(elem, &gpGame->m_unknown7d);
            else if (name == "campDaysPlayedCurrent") xml::QueryShortText(elem, &gpGame->m_campaignScore);
            else if (name == "campMaybeWon") xml::QueryCharText(elem, &gpGame->m_campaignScenarioWon);
            else if (name == "campHasCheated") xml::QueryCharText(elem, &gpGame->m_campaignCheated);
            else if (name == "campMapsWon") gpGame->m_campaignScenarioCompleted[campId][mapId] = value;
            else if (name == "campDaysPlayed") gpGame->m_campaignScenarioBonus[campId][mapId] = static_cast<i16>(value);
            else if (name == "campDaysPlayed2") gpGame->m_campaignScenarioDays[campId][mapId] = static_cast<i16>(value);
            else if (name == "campChoices") gpGame->m_campaignChoice[campId][mapId] = value;
            else if (name == "campMapsPlayed") gpGame->m_campaignMapEnabled[campId][mapId] = value;
            else if (name == "campBonuses") gpGame->m_campaignAwards[index] = value;
            else if (name == "campPlayerCreatures") gpGame->m_campaignCarryoverCreatureTypes[index] = static_cast<i16>(value);
            else if (name == "campPlayerCreatureQuantities") gpGame->m_campaignCarryoverCreatureCounts[index] = static_cast<i16>(value);
        } else if (campaignType == CAMPAIGN_EXPANSION) {
            i32 intValue;
            if (name == "campaignID") { elem->QueryIntText(&intValue); xCampaign.m_campaignId = static_cast<ExpansionCampaignId>(intValue); }
            else if (name == "currentMapID") { elem->QueryIntText(&intValue); xCampaign.m_currentMap = static_cast<ExpansionCampaignMap>(intValue); }
            else if (name == "numMaps") elem->QueryIntText(&xCampaign.m_mapCount);
            else if (name == "mightBeScenarioID") { elem->QueryIntText(&intValue); xCampaign.m_viewMap = static_cast<ExpansionCampaignMap>(intValue); }
            else if (name == "anIntVariable") elem->QueryIntText(&xCampaign.m_viewOnly);
            else if (name == "mapChoice") xCampaign.m_mapChoices[index] = value;
            else if (name == "mapsPlayed") xCampaign.m_mapsPlayed[index] = value;
            else if (name == "daysPlayed") xCampaign.m_mapDays[index] = static_cast<i16>(value);
            else if (name == "awards") xCampaign.m_awards[index] = value;
            else if (name == "bonusChoices") xCampaign.m_bonusChoices[index] = value;
            else if (name == "savedHero") ReadCampaignSavedHero(elem);
        }
    }
}

void XmlFile::ReadCampaignSavedHero(tinyxml2::XMLNode* root) {
    i32 savedHeroIdx = root->ToElement()->IntAttribute("index");
    state::CampaignState::PartialHeroData* savedHero =
        &state::Get().campaign.savedHeroData[savedHeroIdx];
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "primarySkills") savedHero->primarySkills[index] = value;
        if (name == "skillIndex") savedHero->skillIndex[index] = value;
        if (name == "secondarySkillLevel") savedHero->secondarySkillLevel[index] = value;
        if (name == "spell") savedHero->spellsLearned[elem->IntAttribute("idx")] = 1;
        else if (name == "numSecSkillsKnown") elem->QueryIntText(&savedHero->numSecSkillsKnown);
        else if (name == "experience") elem->QueryIntText(&savedHero->experience);
    }
}

void XmlFile::ReadMapHeader(tinyxml2::XMLNode* root) {
    SMapHeader* mh = &gpGame->m_mapHeader;
    i32 lossValueLow = mh->lossConditionValue & 0xff;
    i32 lossValueHigh = mh->lossConditionValue >> 8;
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
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
        else if (name == "name") xml::QueryText(elem, mh->name);
        else if (name == "description") xml::QueryText(elem, mh->description);
        else if (name == "field_1A0") xml::QueryCharText(elem, &mh->rumourCount);
        else if (name == "field_1A1") xml::QueryCharText(elem, &mh->timeEventCount);
        else if (name == "hasPlayer") mh->playerEnabled[index] = value;
        else if (name == "playerMayBeHuman") mh->playerCanHuman[index] = value;
        else if (name == "playerMayBeComp") mh->playerCanComputer[index] = value;
        else if (name == "playerFactions") mh->playerRace[index] = static_cast<i8>(value);
    }
    mh->lossConditionValue = static_cast<u16>(lossValueLow | (lossValueHigh << 8));
}

void XmlFile::ReadMap(tinyxml2::XMLNode* root) {
    gpGame->m_worldMap.width = root->ToElement()->IntAttribute("width");
    gpGame->m_worldMap.height = root->ToElement()->IntAttribute("height");
    gpGame->m_worldMap.Init(gpGame->m_worldMap.width, gpGame->m_worldMap.height);

    gpGame->m_worldMap.extraCount = root->ToElement()->IntAttribute("numCellExtras");
    if (gpGame->m_worldMap.extraCount) {
        delete[] gpGame->m_worldMap.extras;
        gpGame->m_worldMap.extras = new mapCellExtra[gpGame->m_worldMap.extraCount];
    }

    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        if (name == "mapCell") {
            mapCell* cell = &gpGame->m_worldMap.cells[index];
            cell->m_terrainImageIndex = elem->IntAttribute("groundIndex");
            cell->m_animatedObject = elem->IntAttribute("hasObject");
            cell->m_isRoad = elem->IntAttribute("isRoad");
            cell->m_objectTileset = static_cast<TilesetId>(elem->IntAttribute("objTileset"));
            cell->m_objectIndex = elem->IntAttribute("objectIndex");
            cell->m_objectLayerBit0 = elem->IntAttribute("field_4_1");
            cell->m_objectLayerBit1 = elem->IntAttribute("isShadow");
            cell->m_objectDrawnAsOverlay = elem->IntAttribute("field_4_3");
            cell->m_objectMetadata = elem->IntAttribute("extraInfo");
            cell->m_animatedOverlay = elem->IntAttribute("hasOverlay");
            cell->m_drawOverlayOnTop = elem->IntAttribute("hasLateOverlay");
            cell->m_overlayTileset = static_cast<TilesetId>(elem->IntAttribute("overlayTileset"));
            cell->m_overlayIndex = elem->IntAttribute("overlayIndex");
            cell->m_flags = elem->IntAttribute("flags");
            cell->m_triggerType = static_cast<u8>(elem->IntAttribute("objType"));
            cell->m_extraIndex = elem->IntAttribute("extraIdx");
        } else if (name == "mapCellExtra") {
            mapCellExtra* ext = &gpGame->m_worldMap.extras[index];
            ext->nextIndex = elem->IntAttribute("nextIdx");
            ext->animatedObject = elem->IntAttribute("animatedObject");
            ext->objectTileset = static_cast<TilesetId>(elem->IntAttribute("objTileset"));
            ext->objectIndex = elem->IntAttribute("objectIndex");
            ext->objectLayerBit0 = elem->IntAttribute("field_4_1");
            ext->objectLayerBit1 = elem->IntAttribute("field_4_2");
            ext->objectDrawnAsOverlay = elem->IntAttribute("field_4_3");
            ext->objectMetadata = elem->IntAttribute("field_4_4");
            ext->animatedOverlay = elem->IntAttribute("animatedLateOverlay");
            ext->drawOverlayOnTop = elem->IntAttribute("hasLateOverlay");
            ext->overlayTileset = static_cast<TilesetId>(elem->IntAttribute("tileset"));
            ext->overlayIndex = elem->IntAttribute("overlayIndex");
        }
    }
}

void XmlFile::ReadMapExtra(tinyxml2::XMLNode* root) {
    i32 size = 0;
    i32 index = root->ToElement()->IntAttribute("index");
    std::vector<i32> values;
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        if (name == "ppMapExtra") {
            values.push_back(elem->IntAttribute("value"));
            size++;
        }
    }

    pwSizeOfMapExtra[index] = static_cast<i16>(size);
    ppMapExtra[index] = H2_ALLOC(pwSizeOfMapExtra[index]);

    for (i32 j = 0; j < pwSizeOfMapExtra[index]; j++) {
        *(static_cast<char*>(ppMapExtra[index]) + j) = static_cast<char>(values.at(j));
    }
}

void XmlFile::ReadPlayerData(tinyxml2::XMLNode* root, i32 dataIndex) {
    playerData* pdata = &gpGame->m_players[dataIndex];
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "color") xml::QueryCharText(elem, &pdata->m_color);
        else if (name == "numHeroes") xml::QueryCharText(elem, &pdata->m_heroCount);
        else if (name == "curHeroIdx") xml::QueryCharText(elem, &pdata->m_currentHero);
        else if (name == "relatedToSomeSortOfHeroCountOrIdx") xml::QueryCharText(elem, &pdata->m_heroLocatorPage);
        else if (name == "hasCheated") xml::QueryCharText(elem, &gpGame->m_cheated);
        else if (name == "puzzlePieces") xml::QueryCharText(elem, &pdata->m_cheatValue);
        else if (name == "personality") {
            i32 personality;
            elem->QueryIntText(&personality);
            pdata->m_aiDifficulty = static_cast<PlayerPersonality>(personality);
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
        else if (name == "heroesOwned") pdata->m_heroIds[index] = value;
        else if (name == "heroesForPurchase") pdata->m_availableHeroIds[index] = value;
        else if (name == "castlesOwned") pdata->m_townIds[index] = value;
        else if (name == "resources") pdata->m_resources[index] = value;
        else if (name == "resourcesIncome") pdata->m_aiData.m_income[index] = value;
        else if (name == "_4_2_1") pdata->m_unknownad[index] = value;
    }
}

void XmlFile::ReadHero(tinyxml2::XMLNode* root, i32 heroIndex) {
    hero* hro = &gpGame->m_heroRecs[heroIndex];
    // Ironfist's hero::Clear: reset the identity and learned data the XML
    // only writes sparsely.
    hro->m_id = 0;
    hro->m_owner = 0;
    hro->m_x = 0;
    hro->m_y = 0;
    hro->m_cursorType = static_cast<HeroCursorType>(0);
    hro->m_portrait = static_cast<HeroPortrait>(0);
    hro->m_name[0] = '\0';
    memset(hro->m_spells, 0, sizeof(hro->m_spells));
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
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
        else if (name == "name") xml::QueryText(elem, hro->m_name);
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
        else if (name == "attack") xml::QueryCharText(elem, &hro->m_primaryStats[0]);
        else if (name == "defense") xml::QueryCharText(elem, &hro->m_primaryStats[1]);
        else if (name == "spellpower") xml::QueryCharText(elem, &hro->m_primaryStats[2]);
        else if (name == "knowledge") xml::QueryCharText(elem, &hro->m_primaryStats[3]);
        else if (name == "field_43") xml::QueryCharText(elem, &hro->m_primaryStats[4]);
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
            hro->m_eventFlags = static_cast<HeroEventFlag>(flags);
        }
        else if (name == "isCaptain") xml::QueryCharText(elem, &hro->m_isCaptain);
        else if (name == "aiParamFV") elem->QueryFloatText(&hro->m_aiFightValue);
        else if (name == "army") {
            hro->m_army.m_creatureTypes[index] = static_cast<i8>(elem->IntAttribute("type"));
            hro->m_army.m_quantities[index] = static_cast<i16>(elem->IntAttribute("quantity"));
        } else if (name == "secondarySkill") {
            hro->m_secondarySkills[index] =
                static_cast<HeroSkillLevel>(elem->IntAttribute("level"));
            hro->m_secondarySkillOrder[index] = elem->IntAttribute("idx");
        } else if (name == "numSecSkillsKnown")
            elem->QueryIntText(&hro->m_secondarySkillCount);
        else if (name == "spell") {
            index = elem->IntAttribute("idx");
            if (index >= 0 && index < KB_SPELL_TABLE_CAPACITY)
                hro->m_spells[index] = 1;
        }
        else if (name == "artifact") {
            hro->m_artifacts[index] = static_cast<i8>(elem->IntAttribute("id"));
            hro->m_artifactExtra[index] = static_cast<i8>(elem->IntAttribute("spell"));
        }
    }
}

void XmlFile::ReadTown(tinyxml2::XMLNode* root, i32 townIdx) {
    town* twn = &gpGame->m_castleRecs[townIdx];
    i32 turnsOwnedLow = twn->m_turnsOwned & 0xff;
    i32 turnsOwnedHigh = twn->m_turnsOwned >> 8;
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
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
        else if (name == "name") xml::QueryText(elem, twn->m_name);
        else if (name == "garrisonCreature") {
            twn->m_army.m_creatureTypes[index] = static_cast<i8>(elem->IntAttribute("type"));
            twn->m_army.m_quantities[index] = static_cast<i16>(elem->IntAttribute("quantity"));
        } else if (name == "mageGuildSpell") {
            i32 level = elem->IntAttribute("level");
            i32 idx = elem->IntAttribute("idx");
            i32 spell = elem->IntAttribute("spell");
            twn->m_spells[level][idx] = static_cast<i8>(spell);
        } else if (name == "numCreaturesInDwelling")
            twn->m_garrison[index] = static_cast<i16>(value);
        else if (name == "numSpellsOfLevel")
            twn->m_spellCounts[index + TOWN_MAGE_GUILD_FIRST_LEVEL] = static_cast<i8>(value);
    }
    twn->m_turnsOwned = static_cast<u16>(turnsOwnedLow | (turnsOwnedHigh << 8));
}

void XmlFile::ReadRoot(tinyxml2::XMLNode* root) {
    i32 campaignType = CAMPAIGN_NONE;
    char hasPlayer[H2EnumIndex(GAME_PLAYER_COUNT)] = {};
    std::vector<i32> xmlArtifacts;
    script::LuaTable mapVariables;
    for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        tinyxml2::XMLElement* elem = child->ToElement();
        std::string name = elem->Name();
        i32 index = elem->IntAttribute("index");
        i32 value = elem->IntAttribute("value");
        if (name == "allowAIArmySharing") {
            bool allow = true;
            elem->QueryBoolText(&allow);
            gpGame->SetAIArmySharing(allow);
        } else if (name == "disallowedBuildings") {
            for (tinyxml2::XMLNode* build = elem->FirstChild(); build;
                 build = build->NextSibling()) {
                tinyxml2::XMLElement* buildElem = build->ToElement();
                const i32 townIndex = buildElem->IntAttribute("town");
                if (townIndex >= 0 && townIndex < GAME_TOWN_COUNT) {
                    gpGame->m_castleRecs[townIndex].DisallowBuilding(
                        buildElem->IntAttribute("building")
                    );
                }
            }
        }
        else if (name == "mapWidth") elem->QueryIntText(&gpGame->m_worldMap.width);
        else if (name == "mapHeight") {
            elem->QueryIntText(&gpGame->m_worldMap.height);
            gpGame->SetMapSize(gpGame->m_worldMap.width, gpGame->m_worldMap.height);
        }
        else if (name == "gameDifficulty") xml::QueryShortText(elem, &gpGame->m_difficultyRating);
        else if (name == "monthType") {
            i32 monthType;
            elem->QueryIntText(&monthType);
            giMonthType = static_cast<CalendarPeriodType>(monthType);
        }
        else if (name == "monthTypeExtra") elem->QueryIntText(&giMonthTypeExtra);
        else if (name == "weekType") {
            i32 weekType;
            elem->QueryIntText(&weekType);
            giWeekType = static_cast<CalendarPeriodType>(weekType);
        }
        else if (name == "weekTypeExtra") elem->QueryIntText(&giWeekTypeExtra);
        else if (name == "giMapChangeCtr") {
            gpAdvManager->PurgeMapChangeQueue();
            elem->QueryIntText(&giMapChangeCtr);
        }
        else if (name == "numPlayers") xml::QueryCharText(elem, &gpGame->m_playerCount);
        else if (name == "giCurPlayer") elem->QueryIntText(&giCurPlayer);
        else if (name == "couldBeNumDefeatedPlayers") xml::QueryCharText(elem, &gpGame->m_deadPlayerCount);
        else if (name == "day") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_day));
        else if (name == "week") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_week));
        else if (name == "month") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_month));
        else if (name == "numObelisks") xml::QueryCharText(elem, &gpGame->m_obeliskCount);
        else if (name == "ultimateArtifactLocX") xml::QueryCharText(elem, &gpGame->m_ultimateArtifactX);
        else if (name == "ultimateArtifactLocY") xml::QueryCharText(elem, &gpGame->m_ultimateArtifactY);
        else if (name == "ultimateArtifactIdx") xml::QueryCharText(elem, reinterpret_cast<i8*>(&gpGame->m_ultimateArtifactId));
        else if (name == "currentRumor") xml::QueryText(elem, gpGame->m_rumour);
        else if (name == "numRumors") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_rumourEventCount));
        else if (name == "numEvents") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_timeEventCount));
        else if (name == "numMapEvents") xml::QueryShortText(elem, reinterpret_cast<i16*>(&gpGame->m_mapEventCount));
        else if (name == "iMaxMapExtra") {
            elem->QueryIntText(&iMaxMapExtra);
            ppMapExtra = static_cast<void**>(H2_ALLOC(sizeof(void*) * iMaxMapExtra));
            pwSizeOfMapExtra = static_cast<i16*>(H2_ALLOC(sizeof(i16) * iMaxMapExtra));
            memset(ppMapExtra, 0, sizeof(void*) * iMaxMapExtra);
            memset(pwSizeOfMapExtra, 0, sizeof(i16) * iMaxMapExtra);
        }
        else if (name == "difficulty") xml::QueryCharText(elem, reinterpret_cast<i8*>(&gpGame->m_difficulty));
        else if (name == "mapFilename") xml::QueryText(elem, gpGame->m_mapFilename);
        else if (name == "relatedToNewGameSelection") xml::QueryCharText(elem, &gpGame->m_selectedSetupPlayer);
        else if (name == "relatedToNewGameInit") xml::QueryCharText(elem, &gpGame->m_newGameInitialized);
        else if (name == "numHumanPlayers") xml::QueryCharText(elem, &gpGame->m_newGameHumanCount);
        else if (name == "gbIAmGreatest") elem->QueryIntText(&gbIAmGreatest);
        else if (name == "campaignType") elem->QueryIntText(&campaignType);
        else if (name == "mapHeader") ReadMapHeader(elem);
        else if (name == "playerNames") xml::QueryText(elem, cPlayerNames[index]);
        else if (name == "deadPlayers") gpGame->m_playerDead[index] = value;
        else if (name == "alivePlayers") hasPlayer[index] = value;
        else if (name == "heroHireStatus") gpGame->m_availableHeroes[index] = value;
        else if (name == "relatedToPlayerPosAndColor") gpGame->m_setupPlayerColor[index] = value;
        else if (name == "playerHandicap") gpGame->m_playerHandicap[index] = value;
        else if (name == "newGameSelectedFaction") gpGame->m_setupPlayerRace[index] = static_cast<FactionType>(value);
        else if (name == "somePlayerCodeOr10IfMayBeHuman") gpGame->m_setupPlayerNetworkId[index] = value;
        else if (name == "somePlayerNumData") gpGame->m_setupPlayerType[index] = value;
        else if (name == "field_47C") gpGame->_pad_0x47c[index] = value;
        else if (name == "field_2773") gpGame->m_castleOwners[index] = value;
        else if (name == "builtToday") gpGame->m_dailyEventFlags[index] = value;
        else if (name == "field_60A6") gpGame->m_mineOwners[index] = value;
        else if (name == "randomArtifacts") xmlArtifacts.push_back(value);
        else if (name == "boatBuilt") gpGame->m_boatSlots[index] = value;
        else if (name == "obeliskVisitedMasks") gpGame->m_obeliskVisitors[index] = value;
        else if (name == "field_637D") gpGame->m_defaultPlayerNames[index] = value;
        else if (name == "rumorIndices") gpGame->m_rumourEventIndices[index] = value;
        else if (name == "eventIndices") {
            if (index < GAME_TIME_EVENT_CAPACITY)
                gpGame->m_timeEventIndices[index] = value;
        }
        else if (name == "mapEventIndices") {
            if (index < GAME_MAP_EVENT_CAPACITY)
                gpGame->m_mapEventIndices[index] = value;
        }
        else if (name == "mapRevealed") mapExtra[index] = value;
        else if (name == "mine") {
            mineRecord* m = &gpGame->m_mines[index];
            m->x = elem->IntAttribute("x");
            m->y = elem->IntAttribute("y");
            m->id = elem->IntAttribute("field_0");
            m->owner = elem->IntAttribute("owner");
            m->resourceType = static_cast<i8>(elem->IntAttribute("type"));
            m->guardianType = static_cast<i8>(elem->IntAttribute("guardianType"));
            m->guardianCount = elem->IntAttribute("guardianQty");
        }
        else if (name == "boat") {
            boatRecord* b = &gpGame->m_boats[index];
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
            if (script) {
                std::string scriptText(script);
                script::InitializeFromSave(scriptText);
            }
        }
        else if (name == "mapExtra") ReadMapExtra(elem);
        else if (name == "playerData") ReadPlayerData(elem, index);
        else if (name == "town") ReadTown(elem, index);
        else if (name == "map") ReadMap(elem);
        else if (name == "hero") ReadHero(elem, index);
        else if (name == "campaign") ReadCampaign(elem, campaignType);
        else if (name == "campaignMetadata") ReadCampaignMetadata(elem);
        else if (name == "mapVariable") {
            std::string mapVariableId = elem->Attribute("id");

            script::MapVariableType mapVariableType = script::ParseMapVariableType(elem->Attribute("type"));
            i32 x;
            i32 y;
            if ((mapVariableType == script::MapVariableType::Boolean)
                && (std::string(elem->Attribute("value")) == "true")
                && (sscanf(mapVariableId.c_str(), "_AICHASE_%d_%d_", &x, &y) == 2)) {
                gpGame->ForceHeroChase(x, y, true);
            }
            script::MapVariable& variable = mapVariables[mapVariableId];
            variable.type = mapVariableType;
            if (script::IsTable(mapVariableType)) {
                variable.table = ReadTable(elem->FirstChild());
            } else if (script::IsScalar(mapVariableType)) {
                variable.value = elem->Attribute("value");
            } else {
                script::ErrorLoadingMapVariable(
                    mapVariableId,
                    " A map variable can only be a table, number, string or boolean."
                );
            }
        }
    }

    i32 c = 0;
    for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); i++) {
        if (hasPlayer[i] && c < iWSLastMsgNumHumanPlayers) {
            c++;
            gbHumanPlayer[i] = true;
        } else {
            gbHumanPlayer[i] = false;
        }
        if (gbHumanPlayer[i])
            gbThisNetHumanPlayer[i] = !gbRemoteOn || i == giThisGamePos;
        else
            gbThisNetHumanPlayer[i] = false;
    }
    giCurTurn = gpGame->m_day + 7 * (gpGame->m_week - 1) + 28 * (gpGame->m_month - 1);
    DeserializeGeneratedArtifacts(xmlArtifacts);
    if (mapVariables.size())
        script::WriteMapVariablesToLua(mapVariables);
}

std::string FileExtension(b32 isPickLoad) {
    if (gbInCampaign)
        return ".GMC";
    else if (xIsPlayingExpansionCampaign) {
        i32 campID = H2EnumIndex(xCampaign.m_campaignId);
        if (campID <= 3)
            return ".GXC";
        else if (campID == 4) // Ironfist campaign
            return ".GIC";
        else // Custom campaign
            return ".GCC";
    } else {
        i32 aliveHumanPlayers = 0;
        if (isPickLoad)
            aliveHumanPlayers = iWSLastMsgNumHumanPlayers;
        else {
            for (i32 i = 0; i < H2EnumIndex(GAME_PLAYER_COUNT); ++i)
                if (!gpGame->m_playerDead[i] && gbHumanPlayer[i])
                    ++aliveHumanPlayers;
        }

        if ((isPickLoad && gbRemoteOn && xNetHasOldPlayers) || !xIsExpansionMap)
            return ".GM" + std::to_string(aliveHumanPlayers);
        else
            return ".GX" + std::to_string(aliveHumanPlayers);
    }
}

i32 SaveGame(const char* saveFile, i32 autosave) {
    gpAdvManager->DemobilizeCurrHero();
    std::string filePath;
    std::string saveName = saveFile;

    if (autosave)
        filePath = saveName + FileExtension(false);
    else
        filePath = saveName;

    if (platform::CompareIgnoringCase(filePath.c_str(), "RMT", 3)) {
        filePath = ".\\GAMES\\" + filePath;
        if (platform::CompareIgnoringCase(filePath.c_str(), ".\\GAMES\\AUTOSAVE", 16)
            && platform::CompareIgnoringCase(filePath.c_str(), ".\\GAMES\\PLYREXIT", 16))
            strcpy(gpGame->m_saveName, saveName.c_str());
    } else {
        filePath = ".\\DATA\\" + filePath;
    }

    XmlFile xml;
    tinyxml2::XMLError err = xml.Save(filePath.c_str());
    if (err) {
        std::string message = "Could not save XML. " + std::string(xml.GetError());
        DisplayError(message, "Ironfist save");
        exit(1);
    }
    return 1;
}

b32 LoadGame(const char* fileName, i32 loadFromFile) {
    if (!loadFromFile) {
        // A fresh game start, not a load; the retail path handles it.
        runtime::ResetAdventureState();
        return false;
    }

    std::string filePath = ".\\GAMES\\";
    filePath += fileName;

    // Check if original save format
    i32 fd = platform::FileOpen(filePath.c_str(), platform::FileMode::Read);
    char firstByte = 0;
    if (fd != -1) {
        platform::FileReadExact(fd, &firstByte, sizeof(firstByte));
        platform::FileClose(fd);
    }

    if (firstByte != '<') {
        runtime::ResetAdventureState();
        return false;
    }

    gbGameOver = false;
    gpGame->m_gameLoaded = 1;

    gpAdvManager->PurgeMapChangeQueue();

    runtime::ResetAdventureState();

    ClearMapExtra();
    XmlFile xmlDoc;
    tinyxml2::XMLError err = xmlDoc.Read(filePath.c_str());
    if (err) {
        std::string message = "Could not load XML. " + std::string(xmlDoc.GetError());
        DisplayError(message, "Ironfist load");
        exit(1);
    }

    if (platform::CompareIgnoringCase(fileName, "RMT", 3))
        utf8::Copy(gpGame->m_saveName, sizeof(gpGame->m_saveName), fileName);

    gpAdvManager->m_heroContextLocked = false;
    gpCurPlayer = &gpGame->m_players[giCurPlayer];
    giCurPlayerBit = static_cast<u8>(1 << giCurPlayer);
    for (giCurWatchPlayer = giCurPlayer; !gbThisNetHumanPlayer[giCurWatchPlayer];
         giCurWatchPlayer = (giCurWatchPlayer + 1) % gpGame->m_playerCount) {
    }
    giCurWatchPlayerBit = static_cast<u8>(1 << giCurWatchPlayer);
    bShowIt = gbThisNetHumanPlayer[giCurPlayer];
    gpGame->SetupAdjacentMons();
    gpAdvManager->CheckSetEvilInterface(0, -1);
    return true;
}

} // namespace ironfist::save
