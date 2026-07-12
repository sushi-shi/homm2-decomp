// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/advManager.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/playerData.h>
#include <SOURCE/tradpost.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/X_GLOBAL.h>
VA(0x004a8530, 0x5adb)
void advManager::DoEvent(mapCell *cell, int x, int y)
{
    hero *eventHero;
    int eventType;
    int eraseObject;
    int fizzleType;
    SAMPLE2 playedSample;
    SAMPLE2 eventSample;
    mapEventExtra *eventExtra;
    char sphinxAnswer[SPHINX_ANSWER_BUFFER_SIZE];
    int eventValue;
    int correctAnswer;
    int primaryReward;
    int primaryAmount;
    int secondaryReward;
    int secondaryAmount;
    int thirdUpgrade;
    int secondUpgrade;
    int firstUpgrade;
    signEventExtra *signExtra;
    int artifact;
    int artifactResourceType;
    int monsterType;
    int mineCombatResult;
    mapCell *zombieCell;
    mapCell *skeletonCell;
    int resourceType;
    int resourceAmount;
    int springSpellPoints;
    int wellSpellPoints;
    int heroLevel;
    int levelExperience;
    int dragonFactor;
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    int guardedMonster;
    int guardedCount;
    heroWindow *oracleWindow;
    tag_message oracleMessage;
    int teleportCount;
    int teleportX;
    int teleportY;
    hero *otherHero;
    town *occupiedTown;
    int heroCombatResult;
    boatRecord *boat;
    int adjacentMonster;

    eventHero = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    eventType = cell->triggerType & MAP_EVENT_TYPE_MASK;
    eraseObject = 0;
    fizzleType = 0;
    playedSample = NULL_SAMPLE2;
    gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(0);
    eventSample = NULL_SAMPLE2;

    switch (eventType) {
    case MAP_EVENT_TRADING_POST:
        EventSound(eventType, cell->w4hi, &eventSample);
        DoTradingPost(0, 0.2f);
        break;

    case MAP_EVENT_MAGIC_GARDEN:
        if (cell->w4hi == MAP_EVENT_DATA_EMPTY) {
            EventWindow(-1, 1,
                        "{Magic Garden}\n\nYou've found a magic garden, the kind of place that leprechauns and faeries like to cavort in, but there is no one here today.  Perhaps you should try again next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(-1, 1,
                        "{Magic Garden}\n\nYou catch a leprechaun foolishly sleeping amidst a cluster of magic mushrooms.  In exchange for his freedom, he guides you to a small pot filled with precious things.",
                        cell->w4hi - MAP_EVENT_RESOURCE_OFFSET,
                        cell->w4hi - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                            ? MAP_EVENT_GOLD_AMOUNT
                            : MAP_EVENT_RESOURCE_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero, cell->w4hi - MAP_EVENT_RESOURCE_OFFSET,
                         cell->w4hi - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                             ? MAP_EVENT_GOLD_AMOUNT
                             : MAP_EVENT_RESOURCE_AMOUNT);
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_EVENT_SPHINX:
        EventSound(eventType, cell->w4hi, &eventSample);
        eventExtra = reinterpret_cast<mapEventExtra *>(ppMapExtra[cell->w4hi]);
        if (!eventExtra->active) {
            NormalDialog("{Sphinx}\n\nYou come across a giant Sphinx.  The Sphinx remains strangely quiet.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            sprintf(gText,
                    "\"I have a riddle for you,\" the Sphinx says.  \"Answer correctly, and you shall be rewarded.  Answer incorrectly, and you shall be eaten.  Do you accept the challenge?\"");
            NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                sprintf(gText,
                        "The Sphinx asks you the following riddle:\n\n'%s'\n\nYour answer?",
                        eventExtra->riddle);
                GetDataEntry(gText, sphinxAnswer, SPHINX_INPUT_LENGTH, 0, 0, 1);
                correctAnswer = 0;
                for (eventValue = 0; eventValue < eventExtra->answerCount; eventValue++) {
                    if (RiddleStringsEqual(sphinxAnswer, eventExtra->answers[eventValue]))
                        correctAnswer = 1;
                }

                if (correctAnswer) {
                    primaryReward = MAP_EVENT_REWARD_NONE;
                    primaryAmount = 0;
                    secondaryReward = MAP_EVENT_REWARD_NONE;
                    secondaryAmount = 0;
                    for (eventValue = 0; eventValue < SPHINX_RESOURCE_COUNT; eventValue++) {
                        gpGame->m_players[giCurPlayer].resources[eventValue] += eventExtra->resources[eventValue];
                        if (gpGame->m_players[giCurPlayer].resources[eventValue] < 0)
                            gpGame->m_players[giCurPlayer].resources[eventValue] = 0;
                        if (eventExtra->resources[eventValue] != 0) {
                            if (primaryReward != MAP_EVENT_REWARD_NONE) {
                                secondaryReward = primaryReward;
                                secondaryAmount = primaryAmount;
                            }
                            primaryReward = eventValue;
                            primaryAmount = eventExtra->resources[eventValue];
                        }
                    }

                    if (eventExtra->artifact != MAP_EVENT_REWARD_NONE &&
                        eventHero->NumArtifacts() < 14) {
                        GiveArtifact(eventHero, eventExtra->artifact, 1, -1);
                        if (primaryReward != MAP_EVENT_REWARD_NONE) {
                            secondaryReward = primaryReward;
                            secondaryAmount = primaryAmount;
                        }
                        primaryReward = MAP_EVENT_REWARD_ARTIFACT;
                        primaryAmount = eventExtra->artifact;
                    }

                    NormalDialog("Looking somewhat disappointed, the Sphinx sighs.  You've answered my riddle so here's your reward.  Now begone.",
                                 1, -1, -1, primaryReward, primaryAmount,
                                 secondaryReward, secondaryAmount, -1, 0);
                    eventExtra->active = 0;
                }
                else {
                    NormalDialog("\"You guessed incorrectly,\" the Sphinx says, smiling.  The Sphinx swipes at you with a paw, knocking you to the ground.  Another blow makes the world go black, and you know no more.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    HeroLoses(eventHero);
                }
            }
        }
        break;

    case MAP_EVENT_OBSERVATION_TOWER:
        EventSound(eventType, cell->w4hi, &eventSample);
        NormalDialog("{Observation Tower}\n\nFrom the observation tower, you are able to see distant lands.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        gpGame->SetVisibility(x, y, giCurPlayer, OBSERVATION_TOWER_RADIUS);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_EVENT_HILL_FORT:
        thirdUpgrade = -1;
        secondUpgrade = -1;
        firstUpgrade = -1;
        if (eventHero->CreatureTypeCount(HILL_FORT_OGRE))
            firstUpgrade = HILL_FORT_OGRE;
        if (eventHero->CreatureTypeCount(HILL_FORT_ORC)) {
            if (firstUpgrade == -1)
                firstUpgrade = HILL_FORT_ORC;
            else
                secondUpgrade = HILL_FORT_ORC;
        }
        if (eventHero->CreatureTypeCount(HILL_FORT_DWARF)) {
            if (firstUpgrade == -1)
                firstUpgrade = HILL_FORT_DWARF;
            else if (secondUpgrade == -1)
                secondUpgrade = HILL_FORT_DWARF;
            else
                thirdUpgrade = HILL_FORT_DWARF;
        }

        if (firstUpgrade == -1) {
            EventWindow(-1, 1,
                        "{Hill Fort}\n\nAn unusual alliance of Orcs, Ogres, and Dwarves offer to train (upgrade) any such troops brought to them.  Unfortunately, you have none with you.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->UpgradeCreatures(HILL_FORT_OGRE, HILL_FORT_OGRE_LORD);
            eventHero->UpgradeCreatures(HILL_FORT_ORC, HILL_FORT_ORC_CHIEF);
            eventHero->UpgradeCreatures(HILL_FORT_DWARF, HILL_FORT_BATTLE_DWARF);
            if (thirdUpgrade == -1) {
                if (secondUpgrade == -1) {
                    sprintf(gText,
                            "{Hill Fort}\n\nAll of the %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s.",
                            gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[firstUpgrade + 1]);
                }
                else {
                    sprintf(gText,
                            "{Hill Fort}\n\nAll of the %s and %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s and %s.",
                            gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[secondUpgrade],
                            gArmyNamesPlural[firstUpgrade + 1], gArmyNamesPlural[secondUpgrade + 1]);
                }
            }
            else {
                sprintf(gText,
                        "{Hill Fort}\n\n All of the %s, %s and %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s, %s, and %s.",
                        gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[secondUpgrade],
                        gArmyNamesPlural[thirdUpgrade], gArmyNamesPlural[firstUpgrade + 1],
                        gArmyNamesPlural[secondUpgrade + 1], gArmyNamesPlural[thirdUpgrade + 1]);
            }
            EventWindow(-1, 1, gText, FOUNDRY_DIALOG_CREATURE, firstUpgrade + 1,
                        secondUpgrade == -1 ? -1 : FOUNDRY_DIALOG_CREATURE,
                        secondUpgrade + 1, -1);
        }
        break;

    case MAP_EVENT_FREEMANS_FOUNDRY:
        thirdUpgrade = -1;
        secondUpgrade = -1;
        firstUpgrade = -1;
        if (eventHero->CreatureTypeCount(FOUNDRY_PIKEMAN))
            firstUpgrade = FOUNDRY_PIKEMAN;
        if (eventHero->CreatureTypeCount(FOUNDRY_SWORDSMAN)) {
            if (firstUpgrade == -1)
                firstUpgrade = FOUNDRY_SWORDSMAN;
            else
                secondUpgrade = FOUNDRY_SWORDSMAN;
        }
        if (eventHero->CreatureTypeCount(FOUNDRY_IRON_GOLEM)) {
            if (firstUpgrade == -1)
                firstUpgrade = FOUNDRY_IRON_GOLEM;
            else if (secondUpgrade == -1)
                secondUpgrade = FOUNDRY_IRON_GOLEM;
            else
                thirdUpgrade = FOUNDRY_IRON_GOLEM;
        }

        if (firstUpgrade == -1) {
            EventWindow(-1, 1,
                        "{Freeman's Foundry}\n\nA blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems.  Unfortunately, you have none of these troops in your army, so he can't help you.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->UpgradeCreatures(FOUNDRY_PIKEMAN, FOUNDRY_VETERAN_PIKEMAN);
            eventHero->UpgradeCreatures(FOUNDRY_SWORDSMAN, FOUNDRY_MASTER_SWORDSMAN);
            eventHero->UpgradeCreatures(FOUNDRY_IRON_GOLEM, FOUNDRY_STEEL_GOLEM);
            if (thirdUpgrade == -1) {
                if (secondUpgrade == -1) {
                    sprintf(gText,
                            "{Freeman's Foundry}\n\nAll of your %s have been upgraded into %s.",
                            gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[firstUpgrade + 1]);
                }
                else {
                    sprintf(gText,
                            "{Freeman's Foundry}\n\nAll of your %s and %s have been upgraded into %s and %s.",
                            gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[secondUpgrade],
                            gArmyNamesPlural[firstUpgrade + 1], gArmyNamesPlural[secondUpgrade + 1]);
                }
            }
            else {
                sprintf(gText,
                        "{Freeman's Foundry}\n\nAll of your  %s, %s and %s have been upgraded into %s, %s, and %s.",
                        gArmyNamesPlural[firstUpgrade], gArmyNamesPlural[secondUpgrade],
                        gArmyNamesPlural[thirdUpgrade], gArmyNamesPlural[firstUpgrade + 1],
                        gArmyNamesPlural[secondUpgrade + 1], gArmyNamesPlural[thirdUpgrade + 1]);
            }
            EventWindow(-1, 1, gText, FOUNDRY_DIALOG_CREATURE, firstUpgrade + 1,
                        secondUpgrade == -1 ? -1 : FOUNDRY_DIALOG_CREATURE,
                        secondUpgrade + 1, -1);
        }
        break;

    case MAP_EVENT_SEA_CHEST:
        if (!(cell->w4hi & CHEST_ARTIFACT_FLAG) ||
            eventHero->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
            if (!cell->w4hi) {
                NormalDialog("{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it, only to find it empty.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            else {
                NormalDialog("{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it and find 1500 gold pieces.",
                             1, -1, -1, RES_GOLD, SEA_CHEST_GOLD, -1, 0, -1, 0);
                GiveResource(eventHero, RES_GOLD, SEA_CHEST_GOLD);
            }
        }
        else {
            eventValue = cell->w4hi & CHEST_ARTIFACT_MASK;
            sprintf(gText,
                    "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it and find 1000 gold and the %s",
                    gArtifactNames[eventValue]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, eventValue,
                         RES_GOLD, SEA_CHEST_ARTIFACT_GOLD, -1, 0);
            GiveArtifact(eventHero, eventValue, 1, -1);
            GiveResource(eventHero, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
        }
        fizzleType = 1;
        eraseObject = 1;
        break;

    case MAP_EVENT_FLOTSAM:
        switch (cell->w4hi) {
        case 0:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, but find nothing.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        case 1:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, and find some wood.",
                         1, -1, -1, RES_WOOD, 5, -1, 0, -1, 0);
            GiveResource(eventHero, RES_WOOD, 5);
            break;
        case 2:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, and find some wood and some gold.",
                         1, -1, -1, RES_WOOD, 5, RES_GOLD, 200, -1, 0);
            GiveResource(eventHero, RES_WOOD, 5);
            GiveResource(eventHero, RES_GOLD, 200);
            break;
        case 3:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam and find some wood and some gold.",
                         1, -1, -1, RES_WOOD, 10, RES_GOLD, 500, -1, 0);
            GiveResource(eventHero, RES_WOOD, 10);
            GiveResource(eventHero, RES_GOLD, 500);
            break;
        }
        fizzleType = 1;
        eraseObject = 1;
        break;

    case MAP_EVENT_SHIPWRECK_SURVIVOR:
        if (eventHero->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
            sprintf(gText,
                    "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death in an unforgiving ocean.  Grateful, he rewards you for your act of kindness by giving you the %s.",
                    gArtifactNames[cell->w4hi]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, cell->w4hi,
                         -1, 0, -1, 0);
            GiveArtifact(eventHero, cell->w4hi, 1, -1);
        }
        else {
            NormalDialog("{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death in an unforgiving ocean.  Grateful, he says, \"I would give you an artifact as a reward, but you're all full.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        fizzleType = 1;
        eraseObject = 1;
        break;

    case MAP_EVENT_MAGELLAN_MAPS:
        if (gpCurPlayer->m_resources[RES_GOLD] < MAGELLAN_MAP_COST) {
            NormalDialog("{Magellan's Maps}\n\nThe captain sighs.  \"You don't have enough money, eh?  You can't expect me to give my maps away for free!\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            NormalDialog("{Magellan's Maps}\n\nA retired captain living on this refurbished fishing platform offers to sell you maps of the sea he made in his younger days for 1,000 gold.  Do you wish to buy the maps?",
                         2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                gpCurPlayer->m_resources[RES_GOLD] -= MAGELLAN_MAP_COST;
                gpGame->MakeAllWaterVisible(giCurPlayer);
                CompleteDraw(0);
                UpdateScreen(0, 0);
            }
        }
        break;

    case MAP_EVENT_WITCH_HUT:
        if (!eventHero->m_secondarySkills[cell->w4hi]) {
            if (eventHero->m_secondarySkillCount < HERO_SECONDARY_SKILL_LIMIT) {
                EventSound(eventType, cell->w4hi, &eventSample);
                sprintf(gText,
                        "{Witch's Hut}\n\nAn ancient and immortal witch living in a hut with bird's legs for stilts teaches you %s for her own inscrutable purposes.",
                        gSecondarySkills[cell->w4hi]);
                EventWindow(-1, 1, gText, 17, cell->w4hi * 3, -1, 0, -1);
                eventHero->GiveSS(cell->w4hi, 1);
            }
            else {
                sprintf(gText,
                        "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an ancient tome on %s.  As you approach, she turns and focuses her one glass eye on you.  \"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\"",
                        gSecondarySkills[cell->w4hi]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
        else {
            sprintf(gText,
                    "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an ancient tome on %s.  As you approach, she turns and speaks.  \"You already know that which I would teach you. I can help you no further.\"",
                    gSecondarySkills[cell->w4hi]);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_ARTESIAN_SPRING:
        if (!cell->w4hi) {
            NormalDialog("{Artesian Spring}\n\nThe spring only refills once a week, and someone's already been here this week.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            cell->w4hi = 0;
            springSpellPoints = eventHero->Stats(3) * 10;
            if (eventHero->m_spellPoints < springSpellPoints * 2) {
                NormalDialog("{Artesian Spring}\n\nA drink from the spring fills your blood with magic!  You have twice your normal spell points in reserve.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero->m_spellPoints = springSpellPoints * 2;
            }
            else {
                NormalDialog("{Artesian Spring}\n\nA drink at the spring is supposed to give you twice your normal spell points, but you are already at that level.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
        break;

    case MAP_EVENT_MAGIC_WELL:
        if (!(eventHero->m_eventFlags & HERO_EVENT_MAGIC_WELL)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            cell->w4hi = 0;
            wellSpellPoints = eventHero->Stats(3) * 10;
            if (eventHero->m_spellPoints < wellSpellPoints) {
                NormalDialog("{Magic Well}\n\nA drink from the well has restored your spell points to maximum.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero->m_eventFlags |= HERO_EVENT_MAGIC_WELL;
                eventHero->m_spellPoints = wellSpellPoints;
            }
            else {
                NormalDialog("{Magic Well}\n\nA drink at the well is supposed to restore your spell points, but you are already at maximum.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
        else {
            NormalDialog("{Magic Well}\n\nA second drink at the well in one day will not help you.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_COAST:
        if (eventHero->m_eventFlags & HERO_EVENT_EMBARKED) {
            eventHero->m_eventFlags &= ~HERO_EVENT_EMBARKED;
            eventHero->m_remainingMobility = 0;
            eventHero->m_direction = static_cast<unsigned char>(m_cursorDirection);
            m_cursorType = eventHero->m_cursorType;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            playedSample = LoadPlaySample("killfade.82m");
            gpWindowManager->SaveFizzleSource(COAST_FIZZLE_X, COAST_FIZZLE_Y,
                                              COAST_FIZZLE_WIDTH, COAST_FIZZLE_HEIGHT);
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            gpWindowManager->FizzleForward(COAST_FIZZLE_X, COAST_FIZZLE_Y,
                                           COAST_FIZZLE_WIDTH, COAST_FIZZLE_HEIGHT,
                                           -1, 0, 0);
            WaitEndSample(playedSample, -1);
            CheckAdjacentMon(&adjacentMonster);
        }
        break;

    case MAP_EVENT_BOAT:
        boat = &gpGame->m_boats[cell->w4hi];
        gpGame->RestoreCell(-1, -1, boat->x, boat->y, cell, BOAT_RESTORE_MODE);
        eventHero->m_eventFlags |= HERO_EVENT_EMBARKED;
        eventHero->m_remainingMobility = 0;
        boat->heroId = eventHero->m_id;
        boat->owner = eventHero->m_owner;
        m_cursorType = BOAT_CURSOR_TYPE;
        m_cursorDirection = boat->direction;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorActive = 1;
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        break;

    case MAP_EVENT_MINE:
        if (gpGame->m_mineOwners[cell->w4hi] != giCurPlayer) {
            if (gpGame->m_mines[cell->w4hi].guardianType != -1) {
                mineCombatResult = CombatMonsterEvent(eventHero,
                                                      gpGame->m_mines[cell->w4hi].guardianType,
                                                      gpGame->m_mines[cell->w4hi].guardianCount,
                                                      cell, x, y, 0, x, y,
                                                      -1, 0, 0, -1, 0, 0);
                if (mineCombatResult != 0)
                    break;
                gpGame->m_mines[cell->w4hi].guardianType = -1;
                eventHero->CheckLevel();
            }
            EventSound(eventType, cell->w4hi, &eventSample);
            resourceType = gpGame->m_mines[cell->w4hi].resourceType;
            if (resourceType == RES_GOLD)
                resourceAmount = MINE_GOLD_INCOME;
            else if (resourceType == RES_ORE)
                resourceAmount = MINE_ORE_INCOME;
            else
                resourceAmount = 1;
            EventWindow(resourceType + MINE_RESOURCE_ICON_OFFSET, 1, "",
                        resourceType, -resourceAmount, -1, 0, -1);
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
        }
        break;

    case MAP_EVENT_ALCHEMIST_LAB:
        if (gpGame->m_mineOwners[cell->w4hi] != giCurPlayer) {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(0, 1, "", 1, -1, -1, 0, -1);
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
        }
        break;

    case MAP_EVENT_SAWMILL:
        if (gpGame->m_mineOwners[cell->w4hi] != giCurPlayer) {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(73, 1, "", 0, -2, -1, 0, -1);
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
        }
        break;

    case MAP_EVENT_LIGHTHOUSE:
        if (gpGame->m_mineOwners[cell->w4hi] != giCurPlayer) {
            EventSound(eventType, cell->w4hi, &eventSample);
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
            EventWindow(58, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_TREASURE_CHEST:
chestGold:
        if (!(cell->w4hi & CHEST_ARTIFACT_FLAG)) {
            EventWindow(-1, 2,
                        "{Chest}\n\nAfter scouring the area, you fall upon a hidden treasure cache.  You may take the gold or distribute the gold to the peasants for experience.  Do you wish to keep the gold?",
                        RES_GOLD, cell->w4hi * CHEST_GOLD_MULTIPLIER,
                        14, (cell->w4hi * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER, 1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                GiveResource(eventHero, RES_GOLD, cell->w4hi * CHEST_GOLD_MULTIPLIER);
            else
                GiveExperience(eventHero,
                               (cell->w4hi * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER, 0);
        }
        else {
            if (eventHero->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                cell->w4hi = 2;
                goto chestGold;
            }
            eventValue = cell->w4hi & CHEST_ARTIFACT_MASK;
            sprintf(gText,
                    "{Chest}\n\nAfter scouring the area, you fall upon a hidden chest, containing the ancient artifact '%s'",
                    gArtifactNames[eventValue]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, eventValue,
                         -1, 0, -1, 0);
            GiveArtifact(eventHero, eventValue, 1, -1);
        }
        eraseObject = 1;
        fizzleType = 1;
        eventHero->CheckLevel();
        break;

    case MAP_EVENT_BUOY:
        if (!(eventHero->m_eventFlags & HERO_EVENT_BUOY)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_BUOY;
            eventHero->m_morale++;
            EventWindow(3, 1, "", 12, 0, -1, 0, -1);
        }
        else {
            EventWindow(2, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_FAERIE_RING:
        if (!(eventHero->m_eventFlags & HERO_EVENT_FAERIE_RING)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_FAERIE_RING;
            eventHero->m_luck++;
            EventWindow(13, 1, "", 10, 0, -1, 0, -1);
        }
        else {
            EventWindow(12, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_IDOL:
        if (!(eventHero->m_eventFlags & HERO_EVENT_IDOL)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_IDOL;
            eventHero->m_luck++;
            EventWindow(-1, 1,
                        "{Idol}\n\nYou've found an ancient and weathered stone idol.  Kissing it is supposed to be lucky, so you do.  The stone is very cold to the touch.",
                        10, 0, -1, 0, -1);
        }
        else {
            EventWindow(-1, 1,
                        "{Idol}\n\nYou've found an ancient and weathered stone idol.  It is supposed to grant luck to visitors, but since the stars are already smiling upon you, it does nothing.",
                        -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_FOUNTAIN:
        if (!(eventHero->m_eventFlags & HERO_EVENT_FOUNTAIN)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_FOUNTAIN;
            eventHero->m_luck++;
            EventWindow(16, 1, "", 10, 0, -1, 0, -1);
        }
        else {
            EventWindow(15, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_WATERING_HOLE:
        if (!(eventHero->m_eventFlags & HERO_EVENT_WATERING_HOLE)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_WATERING_HOLE;
            eventHero->m_morale++;
            eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
            EventWindow(-1, 1,
                        "{Watering Hole}\n\nA drink at the watering hole fills your troops with strength and lifts their spirits.  You can travel a bit further today.",
                        12, 0, -1, 0, -1);
        }
        else {
            EventWindow(-1, 1,
                        "{Watering Hole}\n\nThe drink at the watering hole is refreshing, but offers no further benefit.  The watering hole might help again if you fought a battle first.",
                        -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_OASIS:
        if (!(eventHero->m_eventFlags & HERO_EVENT_OASIS)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_OASIS;
            eventHero->m_morale++;
            eventHero->m_mobility += OASIS_MOBILITY_BONUS;
            eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
            EventWindow(-1, 1,
                        "{Oasis}\n\nA drink at the oasis fills your troops with strength and lifts their spirits.  You can travel a bit further today.",
                        12, 0, -1, 0, -1);
        }
        else {
            EventWindow(-1, 1,
                        "{Oasis}\n\nThe drink at the oasis is refreshing, but offers no further benefit.  The oasis might help again if you fought a battle first.",
                        -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_TEMPLE:
        if (!(eventHero->m_eventFlags & HERO_EVENT_TEMPLE)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->m_eventFlags |= HERO_EVENT_TEMPLE;
            eventHero->m_morale += 2;
            NormalDialog("{Temple}\n\nA visit and a prayer at the temple raises the morale of your troops.",
                         1, -1, -1, 12, 0, 12, 0, -1, 0);
        }
        else {
            NormalDialog("{Temple}\n\nIt doesn't help to pray twice before a battle.  Come back after you've fought.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_LEAN_TO:
        if (!cell->w4hi) {
            NormalDialog("{Lean To}\n\nThe lean-to is long abandoned.  There is nothing of value here.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            resourceType = (cell->w4hi & CAMPFIRE_RESOURCE_MASK) - 1;
            resourceAmount = (cell->w4hi & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
            NormalDialog("{Lean To}\n\nYou've found an abandoned lean-to.  Poking about, you discover some resources hidden nearby.",
                         1, -1, -1, resourceType, resourceAmount, -1, 0, -1, 0);
            GiveResource(eventHero, resourceType, resourceAmount);
            cell->w4hi = 0;
        }
        break;

    case MAP_EVENT_WAGON:
        if (cell->w4hi) {
            if (!(cell->w4hi & WAGON_ARTIFACT_FLAG)) {
                resourceType = (cell->w4hi & CAMPFIRE_RESOURCE_MASK) - 1;
                resourceAmount = (cell->w4hi & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 1,
                            "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Inside, you find some of the wagon's cargo still intact.",
                            resourceType, resourceAmount, -1, 0, -1);
                GiveResource(eventHero, resourceType, resourceAmount);
                cell->w4hi = 0;
                break;
            }
            if (eventHero->NumArtifacts() != EVENT_ARTIFACT_CAPACITY) {
                EventSound(eventType, cell->w4hi, &eventSample);
                eventValue = cell->w4hi & WAGON_ARTIFACT_MASK;
                sprintf(gText,
                        "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Searching inside, you find the '%s'.",
                        gArtifactNames[eventValue]);
                EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue,
                            -1, 0, -1);
                GiveArtifact(eventHero, eventValue, 1, -1);
                cell->w4hi = 0;
                break;
            }
        }
        EventWindow(-1, 1,
                    "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Unfortunately, others have found it first, and the wagon is empty.",
                    -1, 0, -1, 0, -1);
        cell->w4hi = 0;
        break;

    case MAP_EVENT_SKELETON:
        if (cell->w4hi == SKELETON_EMPTY) {
            EventWindow(93, 1,
                        "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the tattered clothing, you find nothing.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            if (eventHero->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                sprintf(gText, "%s.", "Treasure");
                EventWindow(-1, 1, gText, RES_GOLD, SKELETON_GOLD, -1, 0, -1);
                GiveResource(eventHero, RES_GOLD, SKELETON_GOLD);
            }
            else {
                eventValue = cell->w4hi - SKELETON_ARTIFACT_OFFSET;
                sprintf(gText,
                        "%s %s",
                        "{Skeleton}\n\nYou come upon the remains of an unfortunate adventurer.  Searching through the tattered clothing, you find ",
                        gArtifactNames[eventValue]);
                GiveArtifact(eventHero, eventValue, 1, -1);
                EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue,
                            -1, 0, -1);
            }
            cell->w4hi = SKELETON_EMPTY;
        }
        break;

    case MAP_EVENT_CAMPFIRE:
        EventWindow(14, 1, "", RES_GOLD,
                    (cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER,
                    cell->w4hi & CAMPFIRE_RESOURCE_MASK,
                    cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT, -1);
        GiveResource(eventHero, RES_GOLD,
                     (cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER);
        GiveResource(eventHero, cell->w4hi & CAMPFIRE_RESOURCE_MASK,
                     cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT);
        eraseObject = 1;
        fizzleType = 1;
        SetEnvironmentOrigin(m_mapOriginX + ERASE_ENVIRONMENT_BORDER,
                             m_mapOriginY + ERASE_ENVIRONMENT_BORDER, 1);
        break;

    case MAP_EVENT_XANADU:
        if (!(eventHero->m_xanaduVisits & (1 << cell->w4hi))) {
            if (eventHero->m_level +
                    eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] * 2 <
                XANADU_ADMISSION_LEVEL) {
                NormalDialog("{Xanadu}\n\nThe butler opens the door and looks you up and down.  \"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs.  \"Come back when you think yourself worthy.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            else {
                EventSound(eventType, cell->w4hi, &eventSample);
                NormalDialog("{Xanadu}\n\nThe butler admits you to see the master of the house.  He trains you in the four skills a hero should know.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero->m_attack++;
                eventHero->m_defense++;
                eventHero->m_knowledge++;
                eventHero->m_spellPower++;
                eventHero->m_xanaduVisits |= 1 << cell->w4hi;
            }
        }
        else {
            NormalDialog("{Xanadu}\n\nRecognizing you, the butler refuses to admit you.  \"The master,\" he says, \"will not see the same student twice.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_FORT:
        if (!(eventHero->m_fortVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            NormalDialog("{Fort}\n\n The soldiers living in the fort teach you a few new defensive tricks.",
                         1, -1, -1, 25, 101, -1, 0, -1, 0);
            eventHero->m_defense++;
            eventHero->m_fortVisits |= 1 << cell->w4hi;
        }
        else {
            NormalDialog("{Fort}\n\n \"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_STANDING_STONES:
        if (!(eventHero->m_standingStoneVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            NormalDialog("{Standing Stones}\n\nYou've found a group of Druids worshipping at one of their strange stone edifices.  Silently, they teach you new ways to cast spells.",
                         1, -1, -1, 25, 102, -1, 0, -1, 0);
            eventHero->m_spellPower++;
            eventHero->m_standingStoneVisits |= 1 << cell->w4hi;
        }
        else {
            NormalDialog("{Standing Stones}\n\nYou've found a group of Druids worshipping at one of their strange stone edifices.  Silently, the Druids turn you away, indicating they have nothing new to teach you.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_WITCH_DOCTOR_HUT:
        if (!(eventHero->m_witchDoctorVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            NormalDialog("{Witch Doctor's Hut}\n\nAn Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails.",
                         1, -1, -1, 25, 103, -1, 0, -1, 0);
            eventHero->m_knowledge++;
            eventHero->m_witchDoctorVisits |= 1 << cell->w4hi;
        }
        else {
            NormalDialog("{Witch Doctor's Hut}\n\n\"Go 'way!\", the witch doctor barks, \"you know all I know.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_MERCENARY_CAMP:
        if (!(eventHero->m_mercenaryCampVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            NormalDialog("{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their tactics.  The mercenaries welcome you and your troops and invite you to train with them.",
                         1, -1, -1, 25, 100, -1, 0, -1, 0);
            eventHero->m_attack++;
            eventHero->m_mercenaryCampVisits |= 1 << cell->w4hi;
        }
        else {
            NormalDialog("{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their tactics.  \"You're too advanced for us,\" the mercenary captain says.  \"We can teach nothing more.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_EVENT_GAZEBO:
        if (!(eventHero->m_gazeboVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(18, 1, "", 14, GAZEBO_EXPERIENCE, -1, 0, -1);
            GiveExperience(eventHero, GAZEBO_EXPERIENCE, 0);
            eventHero->m_gazeboVisits |= 1 << cell->w4hi;
            eventHero->CheckLevel();
        }
        else {
            EventWindow(17, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_WATER_WHEEL:
        if (!cell->w4hi) {
            EventWindow(59, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(60, 1, "", RES_GOLD, cell->w4hi * MAP_EVENT_GOLD_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, cell->w4hi * MAP_EVENT_GOLD_AMOUNT);
            cell->w4hi = 0;
        }
        break;

    case MAP_EVENT_RESOURCE:
        resourceType = cell->objIndex >> 1;
        GiveResource(eventHero, resourceType,
                     resourceType == RES_GOLD
                         ? cell->w4hi * CAMPFIRE_GOLD_MULTIPLIER
                         : cell->w4hi);
        strcpy(sphinxAnswer, gResourceNames[resourceType]);
        sphinxAnswer[0] += ' ';
        sprintf(gText, "You find a small quantity of %s.", sphinxAnswer);
        BVResMsg(gText, resourceType,
                 resourceType == RES_GOLD
                     ? cell->w4hi * CAMPFIRE_GOLD_MULTIPLIER
                     : cell->w4hi);
        eraseObject = 1;
        fizzleType = 1;
        break;

    case MAP_EVENT_WINDMILL:
        if (cell->w4hi < SPHINX_RESOURCE_COUNT) {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(87, 1, "", cell->w4hi, WINDMILL_RESOURCE_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero, cell->w4hi, WINDMILL_RESOURCE_AMOUNT);
            cell->w4hi = WINDMILL_EMPTY;
        }
        else {
            EventWindow(86, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_ANCIENT_LAMP:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(19, 2, "", -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            RecruitEvent(eventHero, EVENT_RECRUIT_GENIE, cell);
            if (!cell->w4hi) {
                eraseObject = 1;
                fizzleType = 1;
            }
        }
        break;

    case MAP_EVENT_TREE_CITY:
        if (!cell->w4hi) {
            EventWindow(-1, 1,
                        "{Tree City}\n\nYou've found a Sprite Tree City.  Unfortunately, none of the Sprites living there wish to join an army.  Maybe next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(-1, 2,
                        "{Tree City}\n\nSome of the Sprites living in the tree city are willing to join your army for a price.  Do you want to recruit Sprites?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, EVENT_RECRUIT_SPRITE, cell);
        }
        break;

    case MAP_EVENT_RUINS:
        if (!cell->w4hi) {
            EventWindow(-1, 1,
                        "{Ruins}\n\nYou search the ruins, but the Medusas that used to live here are gone.  Perhaps there will be more next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(-1, 2,
                        "{Ruins}\n\nYou've found some Medusas living in the ruins.  They are willing to join your army for a price.  Do you want to recruit Medusas?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, EVENT_RECRUIT_MEDUSA, cell);
        }
        break;

    case MAP_EVENT_TROLL_BRIDGE:
        if (!cell->w4hi) {
            EventWindow(-1, 1,
                        "{Troll Bridge}\n\nYou've found one of those bridges that Trolls are so fond of living under, but there are none here.  Perhaps there will be some next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            if (!(cell->w4hi & DWELLING_GUARDED_FLAG)) {
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{Troll Bridge}\n\nSome Trolls living under a bridge are willing to join your army, but for a price.  Do you want to recruit Trolls?",
                            -1, 0, -1, 0, -1);
            }
            else {
                EventWindow(-1, 2,
                            "{Troll Bridge}\n\nTrolls living under the bridge challenge you.  Will you fight them?",
                            -1, 0, -1, 0, -1);
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                if (CombatMonsterEvent(eventHero, TROLL_BRIDGE_TROLL, 12,
                                       cell, x, y, 0, x, y,
                                       TROLL_BRIDGE_WAR_TROLL, 8, 2,
                                       -1, 0, 0) != 0)
                    break;
                eventHero->CheckLevel();
                cell->w4hi -= DWELLING_GUARDED_FLAG;
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{Troll Bridge}\n\nA few Trolls remain, cowering under the bridge.  They approach you and offer to join your forces as mercenaries.  Do you want to buy any Trolls?",
                            -1, 0, -1, 0, -1);
            }
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, TROLL_BRIDGE_TROLL, cell);
        }
        break;

    case MAP_EVENT_CITY_OF_DEAD:
        if (!cell->w4hi) {
            EventWindow(-1, 1,
                        "{City of the Dead}\n\nThe City of the Dead is empty of life, and empty of unlife as well.  Perhaps some undead will move in next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            if (!(cell->w4hi & DWELLING_GUARDED_FLAG)) {
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{City of the Dead}\n\nSome Liches living here are willing to join your army for a price.  Do you want to recruit Liches?",
                            -1, 0, -1, 0, -1);
            }
            else {
                EventWindow(-1, 2,
                            "{City of the Dead}\n\nYou've found the ruins of an ancient city, now inhabited solely by the undead.  Will you search?",
                            -1, 0, -1, 0, -1);
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                if (CombatMonsterEvent(eventHero, CITY_DEAD_PRIMARY, 10,
                                       cell, x, y, 0, x, y,
                                       CITY_DEAD_SECONDARY, 40, 2,
                                       CITY_DEAD_TERTIARY, 5, 1) != 0)
                    break;
                eventHero->CheckLevel();
                cell->w4hi -= DWELLING_GUARDED_FLAG;
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{City of the Dead}\n\nSome of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price.  Do you want to recruit Liches?",
                            -1, 0, -1, 0, -1);
            }
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, CITY_DEAD_RECRUIT, cell);
        }
        break;

    case MAP_EVENT_DRAGON_CITY:
        if (!cell->w4hi) {
            EventWindow(-1, 1,
                        "{Dragon City}\n\nThe Dragon city has no Dragons willing to join you this week.  Perhaps a Dragon will become available next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            if (!(cell->w4hi & DWELLING_GUARDED_FLAG)) {
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{Dragon City}\n\nThe Dragon city is willing to offer some Dragons for your army for a price.  Do you wish to recruit Dragons?",
                            -1, 0, -1, 0, -1);
            }
            else {
                EventWindow(-1, 2,
                            "{Dragon City}\n\nYou stand before the Dragon City, a place off-limits to mere humans.  Do you wish to violate this rule and challenge the Dragons to a fight?",
                            -1, 0, -1, 0, -1);
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                    break;
                dragonFactor = 1;
                if (gbInCampaign && gpGame->m_campaignType == 1 &&
                    gpGame->m_campaignScenario == 6)
                    dragonFactor = 2;
                if (CombatMonsterEvent(eventHero, DRAGON_CITY_GREEN_DRAGON,
                                       dragonFactor * 3, cell, x, y, 0, x, y,
                                       DRAGON_CITY_RED_DRAGON, dragonFactor, 1,
                                       DRAGON_CITY_BLACK_DRAGON, dragonFactor, 1) != 0)
                    break;
                CheckEndGame(0, 1);
                if (gbGameOver)
                    break;
                eventHero->CheckLevel();
                cell->w4hi -= DWELLING_GUARDED_FLAG;
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 2,
                            "{Dragon City}\n\nHaving defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price.  Do you wish to recruit Dragons?",
                            -1, 0, -1, 0, -1);
            }
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, DRAGON_CITY_RECRUIT, cell);
        }
        break;

    case MAP_EVENT_WAGON_CAMP:
        if (!cell->w4hi) {
            EventWindow(83, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(84, 2, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, EVENT_RECRUIT_ROGUE, cell);
        }
        break;

    case MAP_EVENT_DESERT_TENT:
        if (!cell->w4hi) {
            EventWindow(81, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            EventWindow(82, 2, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero, EVENT_RECRUIT_NOMAD, cell);
        }
        break;

    case MAP_EVENT_ARCHER_HOUSE:
    case MAP_EVENT_GOBLIN_HUT:
    case MAP_EVENT_DWARF_COTTAGE:
    case MAP_EVENT_PEASANT_HUT:
    case MAP_EVENT_LOG_CABIN:
    case MAP_EVENT_WATCH_TOWER:
    case MAP_EVENT_TREE_HOUSE:
    case MAP_EVENT_SIRENS:
    case MAP_EVENT_HALFLING_HOLE:
    case MAP_EVENT_EXCAVATION:
    case MAP_EVENT_CAVE:
        if (cell->w4hi)
            EventSound(eventType, cell->w4hi, &eventSample);
        HouseEvent(eventHero, cell);
        break;

    case MAP_EVENT_MONSTER:
        PlayerMonsterInteract(cell, cell, eventHero, &eraseObject, x, y, 0, x, y);
        break;

    case MAP_EVENT_OBELISK:
        if (!(gpGame->m_obeliskVisitors[cell->w4hi] & (1 << eventHero->m_owner))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            gpGame->m_obeliskVisitors[cell->w4hi] |= 1 << eventHero->m_owner;
            EventWindow(68, 1, "", -1, 0, -1, 0, -1);
            ViewPuzzle();
        }
        else {
            EventWindow(69, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_TREE_OF_KNOWLEDGE:
        if (!(eventHero->m_treeKnowledgeVisits & (1 << cell->w4hi))) {
            EventSound(eventType, cell->w4hi, &eventSample);
            heroLevel = eventHero->GetLevel(eventHero->m_experience);
            levelExperience = eventHero->GetExperience(heroLevel + 1) -
                              eventHero->GetExperience(heroLevel);
            switch (cell->w4hi >> TREE_KNOWLEDGE_MODE_SHIFT) {
            case TREE_KNOWLEDGE_FREE:
                NormalDialog("{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in delight.  \"Ahh, an adventurer!  Allow me to teach you a little of what I have learned over the ages.\"",
                             1, -1, -1, 14, -1, -1, 0, -1, 0);
                GiveExperience(eventHero, levelExperience, 0);
                eventHero->m_treeKnowledgeVisits |= 1 << cell->w4hi;
                break;

            case TREE_KNOWLEDGE_GOLD:
                if (gpCurPlayer->m_resources[RES_GOLD] < TREE_KNOWLEDGE_GOLD_COST) {
                    NormalDialog("{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I need 2000 gold.\" it whispers. (sniff)  \"Well, come back when you can pay me.\"",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                else {
                    NormalDialog("{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in delight.  \"Ahh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere 2000 gold.\"  (Just bury it around my roots.)",
                                 2, -1, -1, 14, -1, -1, 0, -1, 0);
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                        gpCurPlayer->m_resources[RES_GOLD] -= TREE_KNOWLEDGE_GOLD_COST;
                        GiveExperience(eventHero, levelExperience, 0);
                        eventHero->m_treeKnowledgeVisits |= 1 << cell->w4hi;
                    }
                }
                break;

            case TREE_KNOWLEDGE_GEMS:
                if (gpCurPlayer->m_resources[RES_GEMS] < TREE_KNOWLEDGE_GEM_COST) {
                    NormalDialog("{Tree of Knowledge}\n\nTears brim in the eyes of the tree.  \"I need 10 gems.\" it whispers. (sniff)  \"Well, come back when you can pay me.\"",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                else {
                    NormalDialog("{Tree of Knowledge}\n\nAhh, an adventurer! I will be happy to teach you a little of what I have learned over the ages for a mere 10 gems.\"  (Just bury it around my roots.)",
                                 2, -1, -1, 14, -1, -1, 0, -1, 0);
                    if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                        gpCurPlayer->m_resources[RES_GEMS] -= TREE_KNOWLEDGE_GEM_COST;
                        GiveExperience(eventHero, levelExperience, 0);
                        eventHero->m_treeKnowledgeVisits |= 1 << cell->w4hi;
                    }
                }
                break;
            }
        }
        else {
            NormalDialog("{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in delight.  \"It is good to see you, my student.  I hope my teachings have helped you.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        eventHero->CheckLevel();
        break;

    case MAP_EVENT_ORACLE:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(74, 1, "", -1, 0, -1, 0, -1);
        oracleWindow = new heroWindow(0, 0, "thiefwin.bin");
        if (oracleWindow == 0)
            MemError();
        SetWinText(oracleWindow, ORACLE_WINDOW_TEXT_ID);
        gpTownManager->SetupThievesGuild(oracleWindow, ORACLE_THIEVES_GUILD_RANK);
        strcpy(gText, "Shrine - Player Rankings");
        oracleMessage.type = ORACLE_MESSAGE_TYPE;
        oracleMessage.field4 = ORACLE_MESSAGE_COMMAND;
        oracleMessage.field8 = 0;
        oracleMessage.text = gText;
        oracleWindow->BroadcastMessage(oracleMessage);
        gpWindowManager->DoDialog(oracleWindow, TrueFalseDialogHandler, 0);
        delete oracleWindow;
        RedrawAdvScreen(1, 0);
        break;

    case MAP_EVENT_SHRINE_FIRST_CIRCLE:
        eventValue = cell->w4hi - 1;
        sprintf(gText, "%s %s",
                "{Shrine of the 1st Circle}\n\nYou come across a small shrine attended by a group of novice acolytes.  In exchange for your protection, they agree to teach you a simple spell - ",
                gSpellNames[eventValue]);
        goto shrineSpell;

    case MAP_EVENT_SHRINE_SECOND_CIRCLE:
        eventValue = cell->w4hi - 1;
        sprintf(gText, "%s %s",
                "{Shrine of the 2nd Circle}\n\nYou come across an ornate shrine attended by a group of rotund friars.  In exchange for your protection, they agree to teach you a spell - ",
                gSpellNames[eventValue]);
        goto shrineSpell;

    case MAP_EVENT_SHRINE_THIRD_CIRCLE:
        eventValue = cell->w4hi - 1;
        sprintf(gText, "%s %s",
                "{Shrine of the 3rd Circle}\n\nYou come across a lavish shrine attended by a group of high priests.  In exchange for your protection, they agree to teach you a sophisticated spell - ",
                gSpellNames[eventValue]);
shrineSpell:
        if (!eventHero->HasArtifact(PYRAMID_SPELLBOOK_ARTIFACT)) {
            strcat(gText, "Unfortunately, you have no Magic Book to record the spell with.");
            EventWindow(-1, 1, gText, -1, 0, -1, 0, -1);
        }
        else if (eventHero->m_secondarySkills[HERO_SKILL_WISDOM] + SHRINE_WISDOM_BONUS <
                 gsSpellInfo[eventValue].level) {
            strcat(gText,
                   "Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.  ");
            EventWindow(-1, 1, gText, -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            eventHero->AddSpell(eventValue, eventHero->Stats(3));
            EventWindow(-1, 1, gText, 8, eventValue, -1, 0, -1);
        }
        break;

    case MAP_EVENT_CASTLE:
        TownEvent(cell, x, y);
        break;

    case MAP_EVENT_WHIRLPOOL:
        EventSound(eventType, cell->w4hi, &eventSample);
        DoWhirlpool(eventHero);

findTeleportDestination:
        teleportCount = 0;
        for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
            for (teleportX = 0; teleportX < MAP_WIDTH; teleportX++) {
                if ((gpGame->m_worldMap.Row(teleportY) + teleportX)->triggerType ==
                        (eventType | TELEPORT_TRIGGER_FLAG) &&
                    (gpGame->m_worldMap.Row(teleportY) + teleportX)->objIndex == cell->objIndex &&
                    (eventType == MAP_EVENT_STONE_LITHS
                         ? STONE_LITHS_MIN_DISTANCE
                         : WHIRLPOOL_MIN_DISTANCE) <
                        abs(teleportY - y) + abs(teleportX - x)) {
                    teleportCount++;
                }
            }
        }
        if (teleportCount > 0) {
            if (teleportCount > 1)
                teleportCount = Random(1, teleportCount);
            for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                for (teleportX = 0; teleportX < MAP_WIDTH; teleportX++) {
                    if ((gpGame->m_worldMap.Row(teleportY) + teleportX)->triggerType ==
                            (eventType | TELEPORT_TRIGGER_FLAG) &&
                        (gpGame->m_worldMap.Row(teleportY) + teleportX)->objIndex == cell->objIndex &&
                        (teleportX != x || teleportY != y) &&
                        (eventType == MAP_EVENT_STONE_LITHS
                             ? STONE_LITHS_MIN_DISTANCE
                             : WHIRLPOOL_MIN_DISTANCE) <
                            abs(teleportY - y) + abs(teleportX - x) &&
                        --teleportCount < 1) {
                        goto teleportHero;
                    }
                }
            }
teleportHero:
            StopCursor(1);
            gpAdvManager->TeleportTo(eventHero, teleportX, teleportY, 1, 0);
        }
        break;

    case MAP_EVENT_STONE_LITHS:
        EventSound(eventType, cell->w4hi, &eventSample);
        goto findTeleportDestination;

    case MAP_EVENT_ARTIFACT:
        artifactResourceType = (cell->w4hi & ARTIFACT_RESOURCE_MASK) >> ARTIFACT_RESOURCE_SHIFT;
        artifact = cell->objIndex >> 1;
        guardedMonster = cell->w4hi & ARTIFACT_MONSTER_MASK;
        if (eventHero->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
            NormalDialog("You cannot pick up this artifact, you already have a full load!",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        }

        if (xIsPlayingExpansionCampaign && xCampaign.IsSpecialGoldenBow(x, y)) {
            EventSound(eventType, cell->w4hi, &eventSample);
            strcpy(gText,
                   "The fabled golden bow of the elves lies here in the dust.  You take it and journey back to the elven towns.  They shower you with their graciousness and the king promises that his people will aid you whenever you seek help.");
            EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, artifact,
                        -1, 0, -1);
            GiveArtifact(eventHero, artifact, 1, static_cast<signed char>(cell->w4hi));
            eraseObject = 1;
            fizzleType = 1;
            break;
        }

        if (artifact == ARTIFACT_SPELL_SCROLL) {
            EventSound(eventType, 1, &eventSample);
            xTheSpell = cell->w4hi;
            EventWindow(-1, 1, gArtifactEvent[artifact], MAP_EVENT_REWARD_ARTIFACT, artifact,
                        -1, 0, -1);
            GiveArtifact(eventHero, artifact, 1, static_cast<signed char>(cell->w4hi));
            eraseObject = 1;
            fizzleType = 1;
            break;
        }

        if (!(cell->w4hi & ARTIFACT_GUARDED_FLAG)) {
            switch (cell->w4hi & ARTIFACT_MODE_MASK) {
            case ARTIFACT_MODE_PICKUP:
artifactPickup:
                EventSound(eventType, cell->w4hi, &eventSample);
                EventWindow(-1, 1, gArtifactEvent[artifact], MAP_EVENT_REWARD_ARTIFACT, artifact,
                            -1, 0, -1);
                goto giveArtifact;

            case ARTIFACT_MODE_GOLD:
                EventSound(eventType, cell->w4hi, &eventSample);
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 2000 gold.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact]);
                EventWindow(-1, 2, gText, MAP_EVENT_REWARD_ARTIFACT, artifact,
                            -1, 0, -1);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] >= ARTIFACT_GOLD_COST) {
                        gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= ARTIFACT_GOLD_COST;
                        goto giveArtifact;
                    }
                    NormalDialog("You try to pay the leprechaun, but realize that you can't afford it.  The leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                else {
                    NormalDialog("Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                break;

            case ARTIFACT_MODE_WISDOM:
                if (eventHero->m_secondarySkills[HERO_SKILL_WISDOM])
                    goto artifactPickup;
                sprintf(gText,
                        "{Artifact}\n\nYou've found the humble dwelling of a withered hermit.  The hermit tells you that he is willing to give the %s to the first wise person he meets.",
                        gArtifactNames[artifact]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                break;

            case ARTIFACT_MODE_LEADERSHIP:
                if (eventHero->m_secondarySkills[HERO_SKILL_LEADERSHIP])
                    goto artifactPickup;
                sprintf(gText,
                        "{Artifact}\n\nYou've come across the spartan quarters of a retired soldier.  The soldier tells you that he is willing to pass on the %s to the first true leader he meets.",
                        gArtifactNames[artifact]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                break;

            case ARTIFACT_MODE_RESOURCE_3:
                EventSound(eventType, cell->w4hi, &eventSample);
                strcpy(sphinxAnswer, gResourceNames[artifactResourceType]);
                sphinxAnswer[0] += ' ';
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 2500 gold and 3 %s.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact], sphinxAnswer);
                NormalDialog(gText, 2, -1, -1, MAP_EVENT_REWARD_ARTIFACT, artifact,
                             -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] >= ARTIFACT_RESOURCE_3_GOLD_COST &&
                        gpGame->m_players[eventHero->m_owner].resources[artifactResourceType] >= ARTIFACT_RESOURCE_3_AMOUNT) {
                        gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= ARTIFACT_RESOURCE_3_GOLD_COST;
                        gpGame->m_players[eventHero->m_owner].resources[artifactResourceType] -= ARTIFACT_RESOURCE_3_AMOUNT;
                        goto giveArtifact;
                    }
                    NormalDialog("You try to pay the leprechaun, but realize that you can't afford it.  The leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                else {
                    NormalDialog("Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                break;

            case ARTIFACT_MODE_RESOURCE_5:
                EventSound(eventType, cell->w4hi, &eventSample);
                strcpy(sphinxAnswer, gResourceNames[artifactResourceType]);
                sphinxAnswer[0] += ' ';
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 3000 gold and 5 %s.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact], sphinxAnswer);
                NormalDialog(gText, 2, -1, -1, MAP_EVENT_REWARD_ARTIFACT, artifact,
                             -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] >= ARTIFACT_RESOURCE_5_GOLD_COST &&
                        gpGame->m_players[eventHero->m_owner].resources[artifactResourceType] >= ARTIFACT_RESOURCE_5_AMOUNT) {
                        gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= ARTIFACT_RESOURCE_5_GOLD_COST;
                        gpGame->m_players[eventHero->m_owner].resources[artifactResourceType] -= ARTIFACT_RESOURCE_5_AMOUNT;
                        goto giveArtifact;
                    }
                    NormalDialog("You try to pay the leprechaun, but realize that you can't afford it.  The leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                else {
                    NormalDialog("Insulted by your refusal of his generous offer, the leprechaun stamps his foot and ignores you.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
                break;
            }
        }
        else {
            EventSound(eventType, cell->w4hi, &eventSample);
            if (guardedMonster == ARTIFACT_ROGUE_GUARD) {
                NormalDialog("{Artifact}\n\nYou come upon an ancient artifact.  As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                guardedCount = ARTIFACT_ROGUE_COUNT;
            }
            else {
                guardedCount = 1;
                sprintf(gText,
                        "{Artifact}\n\nThrough a clearing you observe an ancient artifact.  Unfortunately, it's guarded by a nearby %s.  Do you want to fight the %s for the artifact?",
                        gArmyNames[guardedMonster], gArmyNames[guardedMonster]);
                NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES) {
                    NormalDialog("Discretion is the better part of valor, and you decide to avoid this fight for today.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    break;
                }
            }
            if (CombatMonsterEvent(eventHero, guardedMonster, guardedCount,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) != 0)
                break;
            eventHero->CheckLevel();
            sprintf(gText, "Victorious, you take your prize, the %s", gArtifactNames[artifact]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT,
                         cell->objIndex >> 1, -1, 0, -1, 0);
giveArtifact:
            GiveArtifact(eventHero, artifact, 1, -1);
            eraseObject = 1;
            fizzleType = 1;
        }
        eventHero->CheckLevel();
        break;

    case MAP_EVENT_HERO_INTERACTION:
        DemobilizeCurrHero();
        otherHero = &gpGame->m_heroRecs[cell->w4hi];
        if (otherHero->m_owner == giCurPlayer) {
            HeroSwap(eventHero, otherHero);
        }
        else {
            occupiedTown = 0;
            if (otherHero->m_locationType == HERO_TOWN_LOCATION) {
                occupiedTown = reinterpret_cast<town *>(
                    &gpGame->m_castleRecs[otherHero->m_occupiedTown]);
                occupiedTown->m_occupyingHeroId = otherHero->m_id;
            }
            heroCombatResult = DoCombat(x, y, eventHero, &eventHero->m_army,
                                        occupiedTown, otherHero, &otherHero->m_army,
                                        x, y, -1, 1);
            if (heroCombatResult == 0 && occupiedTown != 0)
                gpGame->ClaimTown(occupiedTown->m_id, giCurPlayer, 0);
        }
        break;

    case MAP_EVENT_BOTTLE:
        if (cell->w4hi) {
            signExtra = reinterpret_cast<signEventExtra *>(ppMapExtra[cell->w4hi]);
            if (strlen(signExtra->text) < 2)
                EventWindow(-1, 1, cRandomSignText[(x < 0 ? -x : x) & 3],
                            -1, 0, -1, 0, -1);
            else
                EventWindow(-1, 1, signExtra->text, -1, 0, -1, 0, -1);
        }
        fizzleType = 1;
        eraseObject = 1;
        break;

    case MAP_EVENT_SIGN:
        if (cell->w4hi) {
            signExtra = reinterpret_cast<signEventExtra *>(ppMapExtra[cell->w4hi]);
            if (strlen(signExtra->text) < 2)
                EventWindow(-1, 1, cRandomSignText[(x < 0 ? -x : x) & 3],
                            -1, 0, -1, 0, -1);
            else
                EventWindow(-1, 1, signExtra->text, -1, 0, -1, 0, -1);
        }
        break;

    case MAP_EVENT_DAEMON_CAVE:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(-1, 2,
                    "{Daemon Cave}\n\nThe entrance to the cave is dark, and a foul, sulfurous smell issues from the cave mouth.  Will you enter?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_NO)
            break;
        if (cell->w4hi == DAEMON_CAVE_EMPTY) {
            EventWindow(-1, 1,
                        "Except for evidence of a terrible battle, the cave is empty.",
                        -1, 0, -1, 0, -1);
            break;
        }

        monsterType = ((cell->w4hi & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT) + DAEMON_SERVANT_BASE;
        sprintf(gText,
                "You find a powerful and grotesque Demon in the cave.  \"Today,\" it rasps, \"you will fight and surely die.  But I will give you a choice of deaths.  You may fight me, or you may fight my servants.  Do you prefer to fight my servants?\"");
        EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (CombatMonsterEvent(eventHero, monsterType, DAEMON_SERVANT_COUNT,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) == 0) {
                eventHero->CheckLevel();
                NormalDialog("Upon defeating the daemon's servants, you find a hidden cache with 2500 gold.",
                             1, -1, -1, RES_GOLD, DAEMON_GOLD, -1, 0, -1, 0);
                GiveResource(eventHero, RES_GOLD, DAEMON_GOLD);
                cell->w4hi = DAEMON_CAVE_EMPTY;
            }
            break;
        }

        switch (cell->w4hi & DAEMON_REWARD_MASK) {
        case DAEMON_REWARD_EXPERIENCE:
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
            EventWindow(-1, 1,
                        "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and receive 1,000 experience points.",
                        14, DAEMON_EXPERIENCE, -1, 0, -1);
            cell->w4hi = DAEMON_CAVE_EMPTY;
            eventHero->CheckLevel();
            break;

        case DAEMON_REWARD_ARTIFACT:
            if (eventHero->NumArtifacts() == EVENT_ARTIFACT_CAPACITY ||
                gpGame->GetRandomArtifactId(EVENT_RANDOM_ARTIFACT_ANY, 1) == -1)
                goto daemonExperienceGold;
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
            eventValue = GiveRandomArtifact(eventHero);
            sprintf(gText,
                    "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and find the %s in the back of the cave.",
                    gArtifactNames[eventValue]);
            EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue,
                        14, DAEMON_EXPERIENCE, -1);
            cell->w4hi = DAEMON_CAVE_EMPTY;
            eventHero->CheckLevel();
            break;

        case DAEMON_REWARD_EXPERIENCE_GOLD:
daemonExperienceGold:
            EventWindow(-1, 1,
                        "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and receive 1,000 experience points and 2,500 gold.",
                        RES_GOLD, DAEMON_GOLD, 14, DAEMON_EXPERIENCE, -1);
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 0);
            GiveResource(eventHero, RES_GOLD, DAEMON_GOLD);
            cell->w4hi = DAEMON_CAVE_EMPTY;
            eventHero->CheckLevel();
            break;

        case DAEMON_REWARD_RANSOM:
            EventWindow(-1, 2,
                        "The Demon leaps upon you and has its claws at your throat before you can even draw your sword.  \"Your life is mine,\" it says.  \"I will sell it back to you for 2,500 gold.\"",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] < DAEMON_GOLD) {
                    EventWindow(-1, 1,
                                "Seeing that you do not have 2500 gold, the demon slashes you with its claws, and the last thing you see is a red haze.",
                                -1, 0, -1, 0, -1);
                    HeroLoses(eventHero);
                }
                else {
                    gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= DAEMON_GOLD;
                }
            }
            else {
                HeroLoses(eventHero);
            }
            break;
        }
        cell->w4hi = DAEMON_CAVE_EMPTY;
        break;

    case MAP_EVENT_SHIPWRECK:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(-1, 2,
                    "{Shipwreck}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.  Do you wish to search the shipwreck?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (cell->w4hi == SKELETON_EMPTY) {
                EventWindow(-1, 1,
                            "Upon defeating the Ghosts you spend several hours sifting through the debris and find nothing.  Such a despicable act reduces your army's morale.",
                            13, 0, -1, 0, -1);
                if (!(eventHero->m_eventFlags & HERO_EVENT_SHIPWRECK)) {
                    eventHero->m_eventFlags |= HERO_EVENT_SHIPWRECK;
                    eventHero->m_morale--;
                }
            }
            else if (GhostEvent(eventHero, cell,
                                "Upon defeating the Ghosts you sift through the debris and find something!",
                                x, y)) {
                cell->w4hi = SKELETON_EMPTY;
            }
        }
        break;

    case MAP_EVENT_GRAVEYARD:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(20, 2,
                    "{Graveyard}\n\nYou tentatively approach the burial ground of ancient warriors.  Do you want to search the graves?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (cell->w4hi == SKELETON_EMPTY) {
                EventWindow(21, 1,
                            "Upon defeating the Zombies you spend several hours searching the graves and find nothing.  Such a despicable act reduces your army's morale.",
                            13, 0, -1, 0, -1);
                if (!(eventHero->m_eventFlags & HERO_EVENT_GRAVEYARD)) {
                    eventHero->m_eventFlags |= HERO_EVENT_GRAVEYARD;
                    eventHero->m_morale--;
                }
            }
            else {
                zombieCell = GetCell(x - normalDirTable[eventHero->m_direction].x,
                                     y - normalDirTable[eventHero->m_direction]._1);
                if (ZombieEvent(eventHero, zombieCell,
                                "Upon defeating the zomies you search the graves and find something!",
                                x, y))
                    cell->w4hi = SKELETON_EMPTY;
            }
        }
        break;

    case MAP_EVENT_DERELICT_SHIP:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(-1, 2,
                    "{Derelict Ship}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.  Do you wish to search the ship?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (cell->w4hi == SKELETON_EMPTY) {
                EventWindow(-1, 1,
                            "Upon defeating the Skeletons you spend several hours sifting through the debris and find nothing.  Such a despicable act reduces your army's morale.",
                            13, 0, -1, 0, -1);
                if (!(eventHero->m_eventFlags & HERO_EVENT_DERELICT_SHIP)) {
                    eventHero->m_eventFlags |= HERO_EVENT_DERELICT_SHIP;
                    eventHero->m_morale--;
                }
            }
            else {
                skeletonCell = GetCell(x - normalDirTable[eventHero->m_direction].x,
                                       y - normalDirTable[eventHero->m_direction]._1);
                if (SkeletonEvent(eventHero, skeletonCell,
                                  "Upon defeating the Skeletons you sift through the debris and find something!",
                                  x, y))
                    cell->w4hi = SKELETON_EMPTY;
            }
        }
        break;

    case MAP_EVENT_PYRAMID:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(-1, 2,
                    "You come upon the pyramid of a great and ancient king.  You are tempted to search it for treasure, but all the old stories warn of fearful curses and undead guardians.  Will you search?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (!cell->w4hi) {
                NormalDialog("You come upon the pyramid of a great and ancient king.  Routine exploration reveals that the pyramid is completely empty.",
                             1, -1, -1, 11, 0, 11, 0, -1, 0);
                if (!(eventHero->m_eventFlags & HERO_EVENT_PYRAMID)) {
                    eventHero->m_eventFlags |= HERO_EVENT_PYRAMID;
                    eventHero->m_luck -= 2;
                }
            }
            else {
                if (CombatMonsterEvent(eventHero, PYRAMID_PRIMARY_MONSTER, 30,
                                       cell, x, y, 0, x, y,
                                       PYRAMID_SECONDARY_MONSTER, 20, 2,
                                       -1, 0, 0) == 0) {
                    eventHero->CheckLevel();
                    eventValue = cell->w4hi - 1;
                    sprintf(eventText, "%s %s",
                            "Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell - ",
                            gSpellNames[eventValue]);
                    if (!eventHero->HasArtifact(PYRAMID_SPELLBOOK_ARTIFACT)) {
                        strcat(eventText,
                               "  Unfortunately, you have no Magic Book to record the spell with.");
                        EventWindow(-1, 1, eventText, -1, 0, -1, 0, -1);
                    }
                    else if (eventHero->m_secondarySkills[HERO_SKILL_WISDOM] <
                             PYRAMID_WISDOM_REQUIRED) {
                        strcat(eventText,
                               "  Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.  ");
                        EventWindow(-1, 1, eventText, -1, 0, -1, 0, -1);
                    }
                    else {
                        eventHero->AddSpell(eventValue, eventHero->Stats(3));
                        EventWindow(-1, 1, eventText, 8, eventValue, -1, 0, -1);
                    }
                    cell->w4hi = 0;
                }
            }
        }
        break;

    case MAP_EVENT_ABANDONED_MINE:
        EventSound(eventType, cell->w4hi, &eventSample);
        EventWindow(-1, 2,
                    "You come upon an abandoned gold mine.  The mine appears to be haunted.  Do you wish to enter?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (CombatMonsterEvent(eventHero,
                                   gpGame->m_mines[cell->w4hi].guardianType,
                                   gpGame->m_mines[cell->w4hi].guardianCount,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) == 0) {
                eventHero->CheckLevel();
                EventWindow(-1, 1,
                            "You beat the Ghosts and are able to restore the mine to production.",
                            -1, 0, -1, 0, -1);
                gpGame->ConvertObject(x + ABANDONED_MINE_X_MIN, y + ABANDONED_MINE_Y_TOP,
                                      x + ABANDONED_MINE_X_MAX, y + ABANDONED_MINE_Y_TOP,
                                      ABANDONED_MINE_TILESET_TOP_A, 0, 4,
                                      ABANDONED_MINE_TILESET_BOTTOM_A, 104,
                                      ABANDONED_MINE_OBJECT, ABANDONED_MINE_EVENT);
                gpGame->ConvertObject(x + ABANDONED_MINE_X_MIN, y,
                                      x + ABANDONED_MINE_X_MAX, y,
                                      ABANDONED_MINE_TILESET_TOP_A, 5, 9,
                                      ABANDONED_MINE_TILESET_BOTTOM_A, 109,
                                      ABANDONED_MINE_OBJECT, ABANDONED_MINE_EVENT);
                gpGame->ConvertObject(x + ABANDONED_MINE_X_MIN, y + ABANDONED_MINE_Y_TOP,
                                      x + ABANDONED_MINE_X_MAX, y + ABANDONED_MINE_Y_TOP,
                                      ABANDONED_MINE_TILESET_TOP_B, 0, 3,
                                      ABANDONED_MINE_TILESET_BOTTOM_B, 75,
                                      ABANDONED_MINE_OBJECT, ABANDONED_MINE_EVENT);
                gpGame->ConvertObject(x + ABANDONED_MINE_X_MIN, y,
                                      x + ABANDONED_MINE_X_MAX, y,
                                      ABANDONED_MINE_TILESET_TOP_B, 4, 7,
                                      ABANDONED_MINE_TILESET_BOTTOM_B, 80,
                                      ABANDONED_MINE_OBJECT, ABANDONED_MINE_EVENT);
                gpGame->ConvertObject(x, y, x, y,
                                      ABANDONED_MINE_TILESET_CENTER, 5, 5,
                                      ABANDONED_MINE_TILESET_CENTER, 4,
                                      ABANDONED_MINE_OBJECT, ABANDONED_MINE_EVENT);
                gpGame->m_mines[cell->w4hi].resourceType = RES_GOLD;
                gpGame->m_mines[cell->w4hi].guardianType = -1;
                gpGame->m_mines[cell->w4hi].guardianCount = 0;
                gpGame->ClaimMine(cell->w4hi, giCurPlayer);
            }
        }
        break;

    case MAP_EVENT_BARRIER:
        eraseObject = BarrierEvent(cell, eventHero);
        break;

    case MAP_EVENT_TRAVELER_TENT:
        PasswordEvent(cell, eventHero);
        break;

    case MAP_EVENT_EXPANSION_OBJECT:
        GenericSiteEvent(cell, eventHero);
        break;

    case MAP_EVENT_EXPANSION_DWELLING:
        RecruitSiteEvent(cell, eventHero);
        break;

    case MAP_EVENT_JAIL:
        JailEvent(cell, eventHero, x, y);
    }

    playedSample = eventSample;
    UpdateRadar(1, 0);
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdBottomView(1, 1, 1);
    if (eraseObject) {
        EraseObj(cell, x, y);
        FizzleCenter(fizzleType);
    }
    else {
        CompleteDraw(0);
    }
    UpdateScreen(0, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[*m_currentTerrain]);
    WaitEndSample(playedSample, -1);
    CheckEndGame(0, 0);
}

// @early-stop
// reloc-masked: all 0x9f7 code bytes identical; residual is delinked local-label naming
VA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell *cell, int x, int y)
{
    int erased_a = 0;
    int frame_k = ERASE_NO_FRAME;
    signed char isWide_d = 0;
    mapCell *cells_h[ERASE_NEIGHBOR_COUNT];
    mapCellExtra *extras_b[ERASE_NEIGHBOR_COUNT];
    int i_e;
    int cellX_b[2];
    int cellY_e[2];
    mapCell *currentCell_k;
    mapCellExtra *extra_i;
    volatile int changed_i;

    for (i_e = 0; i_e < ERASE_NEIGHBOR_COUNT; i_e++) {
        cells_h[i_e] = 0;
        extras_b[i_e] = 0;
    }

    erased_a = 1;
    if (cell->objTileset == ERASE_TILESET_11)
        frame_k = cell->objIndex - 1;
    if (cell->objTileset == ERASE_TILESET_63)
        frame_k = cell->objIndex - 1;
    if (cell->objTileset == ERASE_TILESET_62 && cell->objIndex == 9) {
        frame_k = 9;
        isWide_d = 1;
    }
    if (cell->objTileset == ERASE_TILESET_59 && cell->objIndex == 131)
        frame_k = 124;
    if (cell->objTileset == ERASE_TILESET_55 && cell->objIndex == 61)
        frame_k = 54;
    if (cell->objTileset == ERASE_TILESET_50 && cell->objIndex == 45)
        frame_k = 38;
    if (cell->objTileset == ERASE_TILESET_50 && cell->objIndex == 19)
        frame_k = 12;
    if (cell->objTileset == ERASE_TILESET_46) {
        switch (cell->objIndex) {
        case 1: frame_k = 0; break;
        case 3: frame_k = 2; break;
        case 5: frame_k = 4; break;
        case 7: frame_k = 6; break;
        case 9: frame_k = 8; break;
        case 11: frame_k = 10; break;
        case 13: frame_k = 12; break;
        case 15: frame_k = 14; break;
        case 19: frame_k = 18; break;
        }
    }

    for (i_e = 0; i_e < ERASE_NEIGHBOR_COUNT; i_e++) {
        if (isWide_d)
            frame_k--;
        else if (i_e > 0)
            break;

        if (frame_k != ERASE_NO_FRAME) {
        if (isWide_d) {
            if (frame_k > 6)
                cellX_b[0] = frame_k - 9 + x;
            else
                cellX_b[0] = frame_k - 6 + x;
            if (frame_k > 6)
                cellY_e[0] = y;
            else
                cellY_e[0] = y - 1;
        }
        else {
            cellX_b[0] = x - 1;
            cellY_e[0] = y;
        }

        if (cellX_b[0] >= 0) {
        if (cellY_e[0] >= 0) {
        cells_h[i_e] = gpGame->m_worldMap.Row(cellY_e[0]) + cellX_b[0];
        if (i_e > 1) {
            cells_h[i_e]->ovlTileset = 0;
            cells_h[i_e]->ovlIndex = ERASE_EMPTY_INDEX;
        }
        else if (cells_h[i_e]->objIndex != ERASE_EMPTY_INDEX) {
        if (cells_h[i_e]->objIndex == frame_k && cells_h[i_e]->objTileset == cell->objTileset) {
            cells_h[i_e]->objIndex = 0;
            cells_h[i_e]->objTileset = ERASE_CLEARED_TILESET;
            cells_h[i_e]->objFlag0 = 0;
        }

        if (cells_h[i_e]->extra && m_mapData->Extra(cells_h[i_e]->extra)->objIndex != ERASE_EMPTY_INDEX)
            extras_b[i_e] = m_mapData->Extra(cells_h[i_e]->extra);
        else
            extras_b[i_e] = 0;

        while (extras_b[i_e]) {
            if (extras_b[i_e]->objIndex == frame_k && extras_b[i_e]->objTileset == cell->objTileset) {
                extras_b[i_e]->objIndex = 0;
                extras_b[i_e]->objTileset = ERASE_CLEARED_TILESET;
                extras_b[i_e]->objFlag = 0;
            }

            if (extras_b[i_e]->index && m_mapData->Extra(extras_b[i_e]->index)->objIndex != ERASE_EMPTY_INDEX)
                extras_b[i_e] = m_mapData->Extra(extras_b[i_e]->index);
            else
                extras_b[i_e] = 0;
        }
        }
        }
        }
        }
    }

    cell->triggerType = 0;
    cell->objIndex = 0;
    cell->objTileset = ERASE_CLEARED_TILESET;
    cell->objFlag0 = 0;

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;
        if (currentCell_k->objTileset != ERASE_CLEARED_TILESET)
            continue;

        if (currentCell_k->extra && m_mapData->Extra(currentCell_k->extra)->objIndex != ERASE_EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->extra);
        else
            continue;

        if (extra_i->objTileset == ERASE_CLEARED_TILESET || extra_i->objIndex == ERASE_EMPTY_INDEX)
            continue;

            currentCell_k->objIndex = extra_i->objIndex;
            currentCell_k->objTileset = extra_i->objTileset;
            currentCell_k->objFlag0 = extra_i->objFlag;
            currentCell_k->w4a = extra_i->f4a;
            currentCell_k->w4b = extra_i->f4b;
            extra_i->objIndex = 0;
            extra_i->objTileset = ERASE_CLEARED_TILESET;
            extra_i->objFlag = 0;
    }

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;

        if (currentCell_k->objTileset != ERASE_CLEARED_TILESET &&
            currentCell_k->objIndex != ERASE_EMPTY_INDEX && !currentCell_k->w4b)
            goto cellDone;

        if (currentCell_k->extra && m_mapData->Extra(currentCell_k->extra)->objIndex != ERASE_EMPTY_INDEX)
            extra_i = m_mapData->Extra(currentCell_k->extra);
        else
            extra_i = 0;

        while (extra_i) {
            if (extra_i->objTileset != ERASE_CLEARED_TILESET &&
                extra_i->objIndex != ERASE_EMPTY_INDEX && !extra_i->f4b)
                goto cellDone;

            if (extra_i->index && m_mapData->Extra(extra_i->index)->objIndex != ERASE_EMPTY_INDEX)
                extra_i = m_mapData->Extra(extra_i->index);
            else
                extra_i = 0;
        }

        currentCell_k->field8 |= 0x80;
cellDone:
        changed_i = 0;
    }

    SendMapChange(ERASE_MAP_CHANGE_OBJECT, 0, x, y, ERASE_MAP_CHANGE_VALUE, 0, 0);
    SetEnvironmentOrigin(m_mapOriginX + ERASE_ENVIRONMENT_BORDER,
                         m_mapOriginY + ERASE_ENVIRONMENT_BORDER, 1);
    gpGame->SetupAdjacentMons();
}

VA(0x004aea02, 0x90)
void advManager::HeroSwap(class hero *, class hero *) {}

VA(0x004aea92, 0x12f)
int advManager::BarrierEvent(class mapCell *, class hero *) { return 0; }

VA(0x004aebc1, 0x99)
signed char StrEqNoCase(char *, char *) { return 0; }

VA(0x004aec5a, 0xde)
void advManager::PasswordEvent(class mapCell *, class hero *) {}

VA(0x004aed38, 0x6fe)
void advManager::GenericSiteEvent(class mapCell *, class hero *) {}

VA(0x004af436, 0x191)
void advManager::RecruitSiteEvent(class mapCell *, class hero *) {}

VA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(class hero *, int, short int *) {}

VA(0x004af652, 0x22a)
void advManager::JailEvent(class mapCell *, class hero *, int, int) {}

VA(0x004af87c, 0x1da)
void advManager::TownEvent(class mapCell *, int, int) {}

// @early-stop
// reloc-masked: all 0x516 code bytes identical; residual is delinked jump-table/local-label and empty-string symbol naming
VA(0x004afa56, 0x516)
void advManager::EventSound(int eventType, int eventData, struct SAMPLE2 *outSample)
{
    const int treasureSound_a = EVENT_SOUND_TREASURE;
    const int experienceSound_o = EVENT_SOUND_EXPERIENCE;
    const int moraleSound_e = EVENT_SOUND_MORALE;
    const int luckSound_g = EVENT_SOUND_LUCK;
    const int pickupSound_o = EVENT_SOUND_PICKUP;
    const int mineSound_c = EVENT_SOUND_MINE;
    char sampleFile_m[EVENT_SOUND_FILENAME_LENGTH];
    strcpy(sampleFile_m, "");
    int musicTrack_e = EVENT_SOUND_NONE;

    switch (eventType) {
    case MAP_EVENT_ARTESIAN_SPRING:
    case MAP_EVENT_FLOTSAM:
        musicTrack_e = pickupSound_o;
    case MAP_EVENT_ARCHER_HOUSE:
    case MAP_EVENT_GOBLIN_HUT:
    case MAP_EVENT_DWARF_COTTAGE:
    case MAP_EVENT_PEASANT_HUT:
    case MAP_EVENT_LOG_CABIN:
    case MAP_EVENT_TREE_CITY:
    case MAP_EVENT_RUINS:
    case MAP_EVENT_IDOL:
    case MAP_EVENT_SHRINE_SECOND_CIRCLE:
    case MAP_EVENT_WAGON:
    case MAP_EVENT_MAGELLAN_MAPS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_WATER_WHEEL:
    case MAP_EVENT_LEAN_TO:
        musicTrack_e = EVENT_SOUND_TRACK_40;
        break;
    case MAP_EVENT_SEA_CHEST:
    case MAP_EVENT_PYRAMID:
    case MAP_EVENT_CITY_OF_DEAD:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_BOAT:
        if (eventData == EVENT_SOUND_VARIANT_1)
            musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_SKELETON:
    case MAP_EVENT_MINE:
    case MAP_EVENT_MERMAID:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_EVENT_DAEMON_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_36;
        break;
    case MAP_EVENT_SHIPWRECK:
    case MAP_EVENT_WATERING_HOLE:
    case MAP_EVENT_OBSERVATION_TOWER:
        musicTrack_e = EVENT_SOUND_TRACK_32;
        break;
    case MAP_EVENT_FAERIE_RING:
    case MAP_EVENT_FOUNTAIN:
    case MAP_EVENT_WITCH_DOCTOR_HUT:
        musicTrack_e = luckSound_g;
        break;
    case MAP_EVENT_BUOY:
    case MAP_EVENT_RESOURCE:
    case MAP_EVENT_HALFLING_HOLE:
    case MAP_EVENT_XANADU:
        musicTrack_e = moraleSound_e;
        break;
    case MAP_EVENT_GAZEBO:
    case MAP_EVENT_TEMPLE:
    case MAP_EVENT_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_37;
        break;
    case MAP_EVENT_ANCIENT_LAMP:
    case MAP_EVENT_TAR_PIT:
        musicTrack_e = EVENT_SOUND_TRACK_33;
        break;
    case MAP_EVENT_GRAVEYARD:
    case MAP_EVENT_DESERT_TENT:
    case MAP_EVENT_STANDING_STONES:
    case MAP_EVENT_DERELICT_SHIP:
    case MAP_EVENT_BOTTLE:
    case MAP_EVENT_REEFS:
        musicTrack_e = EVENT_SOUND_TRACK_39;
        break;
    case MAP_EVENT_LIGHTHOUSE:
    case MAP_EVENT_EXCAVATION:
    case MAP_EVENT_SPHINX:
    case MAP_EVENT_WITCH_HUT:
        musicTrack_e = EVENT_SOUND_TRACK_31;
        break;
    case MAP_EVENT_OASIS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_ABANDONED_MINE:
    case MAP_EVENT_TREE_OF_KNOWLEDGE:
    case MAP_EVENT_HILL_FORT:
    case MAP_EVENT_SHRINE_THIRD_CIRCLE:
        musicTrack_e = EVENT_SOUND_TRACK_38;
        break;
    case MAP_EVENT_STONE_LITHS:
    case MAP_EVENT_MERCENARY_CAMP:
        musicTrack_e = EVENT_SOUND_TRACK_34;
        break;
    case MAP_EVENT_HUT_OF_MAGI:
        musicTrack_e = EVENT_SOUND_TRACK_1;
        break;
    case MAP_EVENT_WHIRLPOOL:
    case MAP_EVENT_FORT:
    case MAP_EVENT_FREEMANS_FOUNDRY:
        musicTrack_e = EVENT_SOUND_TRACK_35;
        break;
    case MAP_EVENT_BARRIER:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_TRAVELER_TENT:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_EXPANSION_OBJECT:
        switch (eventData) {
        case EVENT_SOUND_VARIANT_0:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_1:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_5:
            musicTrack_e = luckSound_g;
            break;
        case EVENT_SOUND_VARIANT_2:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_3:
            break;
        case EVENT_SOUND_VARIANT_6:
            musicTrack_e = experienceSound_o;
            break;
        case EVENT_SOUND_VARIANT_4:
            musicTrack_e = experienceSound_o;
            break;
        default:
            ;
        }
        break;
    case MAP_EVENT_EXPANSION_DWELLING:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_EVENT_JAIL:
        musicTrack_e = experienceSound_o;
        break;
    default:
        musicTrack_e = EVENT_SOUND_NONE;
        break;
    }

    if (musicTrack_e != EVENT_SOUND_NONE) {
    switch (musicTrack_e) {
    case EVENT_SOUND_TREASURE:
        strcpy(sampleFile_m, "treasure.82m");
        break;
    case EVENT_SOUND_EXPERIENCE:
        strcpy(sampleFile_m, "expernce.82m");
        break;
    case EVENT_SOUND_MORALE:
        strcpy(sampleFile_m, "goodmrle.82m");
        break;
    case EVENT_SOUND_LUCK:
        strcpy(sampleFile_m, "goodluck.82m");
        break;
    case EVENT_SOUND_PICKUP:
        strcpy(sampleFile_m, "pickup01.82m");
        break;
    case EVENT_SOUND_MINE:
        strcpy(sampleFile_m, "h2mine.82m");
        break;
    default:
        gpSoundManager->SwitchAmbientMusic(musicTrack_e);
        break;
    }

    if (strlen(sampleFile_m) > EVENT_SOUND_SAMPLE_NAME_EMPTY_LIMIT)
        *outSample = LoadPlaySample(sampleFile_m);
    }
}

VA(0x004aff6c, 0xc7)
void advManager::EventWindow(int, int, char *, int, int, int, int, int) {}

VA(0x004b0033, 0xb6)
int GiveArtifact(class hero *, int, int, signed char) { return 0; }

VA(0x004b00e9, 0x5e)
int advManager::GiveRandomArtifact(class hero *) { return 0; }

VA(0x004b0147, 0x67)
int advManager::GiveExperience(class hero *, int, int) { return 0; }

VA(0x004b01ae, 0x80)
void advManager::GiveResource(class hero *, int, int) {}

VA(0x004b022e, 0xbb)
void advManager::RecruitEvent(class hero *, int, class mapCell *) {}

VA(0x004b02e9, 0x261)
int advManager::SkeletonEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b054a, 0x29b)
int advManager::ZombieEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b07e5, 0x2f8)
int advManager::GhostEvent(class hero *, class mapCell *, char *, int, int) { return 0; }

VA(0x004b0add, 0x274)
void advManager::HouseEvent(class hero *, class mapCell *) {}

VA(0x004b0d51, 0x62f)
int advManager::CombatMonsterEvent(class hero *, int, int, class mapCell *, int, int, int, int, int, int, int, int, int, int, int) { return 0; }

VA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(class hero *, int, int) {}

VA(0x004b1973, 0x1dd)
void advManager::TransferArtifacts(class hero *, class hero *) {}

VA(0x004b1b50, 0x7f)
void advManager::HeroLoses(class hero *) {}

VA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(class hero *) {}

VA(0x004b1d01, 0x142)
void advManager::FizzleCenter(int) {}

VA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(class mapCell *, class hero *, int, int) {}

VA(0x004b4883, 0x65)
int advManager::BarrierAIEvent(class mapCell *, class hero *) { return 0; }

VA(0x004b48e8, 0x53)
void advManager::PasswordAIEvent(class mapCell *, class hero *) {}

VA(0x004b493b, 0x369)
void advManager::GenericSiteAIEvent(class mapCell *, class hero *) {}

VA(0x004b4ca4, 0x18f)
void advManager::RecruitSiteAIEvent(class mapCell *, class hero *) {}

VA(0x004b4e33, 0x1a2)
void advManager::JailAIEvent(class mapCell *, class hero *, int, int) {}

VA(0x004b4fd5, 0x82b)
void advManager::PlayerMonsterInteract(mapCell *cell, mapCell *combatCell, hero *eventHero,
                                       int *handled, int x, int y, int unused,
                                       int combatX, int combatY)
{
    static char *followerText =
        "{Followers}\n\nA group of %s with a desire for greater glory wish to join you. Do you accept? ";
    int monster_n;
    float strengthRatio_p;
    int combatResult_f;
    int forcedJoin_f;
    int joining;
    int monsterCount_n;
    int joiningCost_i;
    char offerText_g[MONSTER_OFFER_BUFFER_SIZE];

    unused = 0;
    gpMouseManager->ShowColorPointer();
    monster_n = cell->objIndex;
    forcedJoin_f = cell->w4hi & MONSTER_JOIN_FORCED;
    monsterCount_n = cell->w4hi & MONSTER_COUNT_MASK;
    strengthRatio_p = static_cast<float>(gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)) /
                      static_cast<float>(gMonsterDatabase[monster_n].fightValue * monsterCount_n);

    if (gbInCampaign &&
        ((gpGame->m_dwarfAlliance && (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)) ||
         (gpGame->m_ogreAlliance && (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)) ||
         (gpGame->m_dragonAlliance && (monster_n == MONSTER_GREEN_DRAGON || monster_n == MONSTER_RED_DRAGON ||
                                       monster_n == MONSTER_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monster_n)) {
            if (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)
                NormalDialog("The dwarves hail you, \"Any friend of Roland is a friend of ours.  You may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)
                NormalDialog("The ogres give you a grunt of recognition, \"Archibald's allies may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog("The dragons see you and call out.  \"Our alliance with Archibald compels us to join you.  Unfortunately you have no room.  A pity!\"  They quickly scatter.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            *handled = 1;
        }
        else {
            if (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)
                NormalDialog("The dwarves recognize their allies and gladly join your forces.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == MONSTER_OGRE || monster_n == MONSTER_OGRE_LORD)
                NormalDialog("The ogres recognize you as the Dwarfbane and lumber over to join you.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog("The dragons, snarling and growling, agree to join forces with you, their 'Ally'.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
            *handled = 1;
        }
        return;
    }

    if (gbInCampaign && gpGame->m_dwarfbane &&
        (monster_n == MONSTER_DWARF || monster_n == MONSTER_BATTLE_DWARF)) {
        NormalDialog("\"The Dwarfbane!!!!, run for your lives.\"", 1, -1, -1, -1, 0, -1, 0, -1, 0);
        *handled = 1;
        return;
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(0) &&
        (monster_n == MONSTER_ELF || monster_n == MONSTER_GRAND_ELF)) {
        *handled = 1;
        if (eventHero->m_army.CanJoin(monster_n)) {
            NormalDialog("As you approach the group of elves, their leader calls them all to attention.  He shouts to them, \"Who of you is brave enough to join this fearless ally of ours?\"  The group explodes with cheers as they run to join your ranks.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            eventHero->m_army.Add(monster_n, monsterCount_n, -1);
        }
        else {
            NormalDialog("The elves stand at attention as you approach.  Their leader calls to you and says, \"Let us not impede your progress, ally!  Move on, and may victory be yours.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        return;
    }

    if (eventHero->m_army.CanJoin(monster_n) && strengthRatio_p > MONSTER_STRENGTH_JOIN &&
        !eventHero->HasArtifact(MONSTER_NO_JOIN_ARTIFACT) &&
        monster_n != MONSTER_GENIE && monster_n != MONSTER_EARTH_ELEMENTAL &&
        monster_n != MONSTER_AIR_ELEMENTAL && monster_n != MONSTER_FIRE_ELEMENTAL &&
        monster_n != MONSTER_WATER_ELEMENTAL) {
        if (forcedJoin_f) {
            sprintf(gText, followerText, gArmyNamesPlural[monster_n]);
            EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, monsterCount_n, -1);
                *handled = 1;
                return;
            }
            else {
                EventWindow(0x43, 1, "Insulted by your refusal of their offer, the monsters attack!",
                            -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
        else if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] != MONSTER_DIPLOMACY_NONE) {
            if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] == MONSTER_DIPLOMACY_EXPERT)
                joining = monsterCount_n;
            else if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] == MONSTER_DIPLOMACY_ADVANCED)
                joining = monsterCount_n / 2;
            else
                joining = monsterCount_n / 4;
            if (!joining)
                joining = 1;

            joiningCost_i = gMonsterDatabase[monster_n].cost * monsterCount_n;
            if (joiningCost_i > gpGame->m_players[eventHero->m_owner].resources[RES_GOLD]) {
                if (strengthRatio_p > MONSTER_STRENGTH_FLEE)
                    goto monstersFlee;
                else
                    goto fightMonsters;
            }

            if (monsterCount_n == 1) {
                sprintf(gText,
                        "The %s is swayed by your diplomatic tongue, and offers to join your army for the sum of %d gold.  Do you accept?",
                        gArmyNames[monster_n], joiningCost_i);
            }
            else {
                sprintf(gText,
                        "The creatures are swayed by your diplomatic tongue, and make you an offer:\n\n");
                if (monsterCount_n == joining)
                    sprintf(offerText_g,
                            "All %d of the %s will join your army for the sum of %d gold.  Do you accept?",
                            monsterCount_n, gArmyNamesPlural[monster_n], joiningCost_i);
                else
                    sprintf(offerText_g,
                            "%d of the %d %s will join your army, and the rest will leave you alone, for the sum of %d gold.  Do you accept?",
                            joining, monsterCount_n, gArmyNamesPlural[monster_n], joiningCost_i);
                strcat(gText, offerText_g);
            }

            NormalDialog(gText, 2, -1, -1, RES_GOLD, joiningCost_i, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, joining, -1);
                *handled = 1;
                gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -= joiningCost_i;
                return;
            }
            else {
                EventWindow(0x43, 1, "Insulted by your refusal of their offer, the monsters attack!",
                            -1, 0, -1, 0, -1);
                goto fightMonsters;
            }
        }
    }

    if (strengthRatio_p > MONSTER_STRENGTH_FLEE) {
monstersFlee:
        sprintf(gText,
                "The %s, awed by the power of your forces, begin to scatter.  Do you wish to pursue and engage them?",
                gArmyNamesPlural[monster_n]);
        EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
            goto fightMonsters;
        *handled = 1;
        return;
    }

fightMonsters:
    combatResult_f = CombatMonsterEvent(eventHero, monster_n, monsterCount_n, combatCell, x, y, unused,
                                        combatX, combatY, -1, 0, 0, -1, 0, 0);
    if (combatResult_f == 0 || combatResult_f == -1)
        *handled = 1;
}

VA(0x004b5800, 0x440)
void advManager::ComputerMonsterInteract(class mapCell *, class hero *, int *) {}

VA(0x004b5c40, 0x1d0)
int advManager::DoNetCombat(char *) { return 0; }

VA(0x004b5e10, 0x64e)
int advManager::DoCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int) { return 0; }

VA(0x004b645e, 0x36f)
void advManager::SendHeroTownData(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int, int, int, int) {}

VA(0x004b67cd, 0x462)
void advManager::ReceiveHeroTownData(char *, int *, int *, int *, class hero * *, class armyGroup * *, class town * *, class hero * *, class armyGroup * *, int *, int *, int *, signed char *, signed char *, signed char *) {}

VA(0x004b6c2f, 0x254)
int advManager::AutoResolveCombat(int, int, class hero *, class armyGroup *, class town *, class hero *, class armyGroup *, int, int, int, int) { return 0; }

VA(0x004b6e83, 0xb8)
int RiddleStringsEqual(char *, char *) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x0051cc0c) int gbNoShowCombat;
