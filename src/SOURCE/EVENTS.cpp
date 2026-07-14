// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <_globals_model.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <BASE/heroWindow.h>
#include <BASE/heroWindowManager.h>
#include <BASE/bmap2.h>
#include <BASE/executive.h>
#include <BASE/inputManager.h>
#include <BASE/Misc.h>
#include <BASE/mouseManager.h>
#include <BASE/soundManager.h>
#include <EDITOR/fullMap.h>
#include <SOURCE/advManager.h>
#include <SOURCE/ARMY.h>
#include <SOURCE/armyGroup.h>
#include <SOURCE/CURSOR.h>
#include <SOURCE/EVENTS.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/GAME.h>
#include <SOURCE/game.h>
#include <SOURCE/hero.h>
#include <SOURCE/KB.h>
#include <SOURCE/PHILAI.h>
#include <SOURCE/philAI.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/combatManager.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/playerData.h>
#include <SOURCE/recruitUnit.h>
#include <SOURCE/swapManager.h>
#include <SOURCE/tradpost.h>
#include <SOURCE/town.h>
#include <SOURCE/townManager.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/x_arena.h>

#define EVENTS_FILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\EVENTS.CPP")
#define EVENTS_NET_LINE (*reinterpret_cast<const short *>("U\x12"))
#define EVENTS_COMBAT_LINE (*reinterpret_cast<const short *>("V\x12"))
#define EVENTS_SEND_LINE (*reinterpret_cast<const short *>("_\x17"))
#define EVENTS_RECEIVE_LINE (*reinterpret_cast<const short *>("=\x18"))
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
                eventHero->m_primaryStats[HERO_PRIMARY_ATTACK]++;
                eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE]++;
                eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE]++;
                eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER]++;
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
            eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE]++;
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
            eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER]++;
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
            eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE]++;
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
            eventHero->m_primaryStats[HERO_PRIMARY_ATTACK]++;
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
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
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
void advManager::HeroSwap(hero *firstHero, hero *secondHero)
{
    swapManager *swapWindow = new swapManager(firstHero, secondHero);
    if (swapWindow == 0)
        MemError();
    gpExec->DoDialog(swapWindow);
    delete swapWindow;
    RedrawAdvScreen(1, 0);
}

VA(0x004aea92, 0x12f)
int advManager::BarrierEvent(mapCell *cell, hero *)
{
    SAMPLE2 eventSample = NULL_SAMPLE2;
    int color = cell->w4hi;
    color &= BARRIER_COLOR_MASK;
    int passwordIndex = cell->w4hi;
    passwordIndex >>= BARRIER_PASSWORD_SHIFT;
    char response[BARRIER_INPUT_BUFFER_SIZE];

    sprintf(gText,
            "A magical %s barrier stands tall before you, blocking your way.  "
            "Runes on the arch read, \"Speak the key and you may pass.\"",
            xBarrierColor[color]);
    GetDataEntry(gText, response, BARRIER_INPUT_LENGTH, 0, 0, 1);
    if (StrEqNoCase(response, xPasswordStrings[passwordIndex]) &&
        (gpCurPlayer->m_barrierTents & (1 << color))) {
        EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, color,
                   &eventSample);
        NormalDialog(
            "As you speak the magic word, the glowing barrier dissolves into "
            "nothingness.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        return 1;
    } else {
        NormalDialog("You speak, and nothing happens.", 1, -1, -1, -1, 0,
                     -1, 0, -1, 0);
        return 0;
    }
}

VA(0x004aebc1, 0x99)
// @early-stop: all instructions and both external relocations match. The only
// raw residual is offsets 0x5d and 0x61, where MSVC reverses the two symmetric
// equality loads from the exact -0x10/-0x14 character slots.
signed char StrEqNoCase(char *firstString, char *secondString)
{
    char *firstPosition = firstString;
    char *secondPos = secondString;
    int characterCount = 0;
    char firstUpper;
    char secondUpper;

    while (1) {
        characterCount++;
        if (characterCount == GENERIC_SITE_STRING_LIMIT)
            return 1;
        firstUpper = static_cast<char>(
            toupper(static_cast<int>(*firstPosition)));
        secondUpper = static_cast<char>(
            toupper(static_cast<int>(*secondPos)));
        if (firstUpper == secondUpper) {
            if (firstUpper == 0)
                return 1;
            firstPosition++;
            secondPos++;
        } else {
            return 0;
        }
    }
}

VA(0x004aec5a, 0xde)
void advManager::PasswordEvent(mapCell *cell, hero *)
{
    SAMPLE2 eventSample = NULL_SAMPLE2;
    int color = cell->w4hi;
    color &= BARRIER_COLOR_MASK;
    int passwordIndex = cell->w4hi;
    passwordIndex >>= BARRIER_PASSWORD_SHIFT;

    EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, color,
               &eventSample);
    sprintf(
        gText,
        "You enter the tent and see an old woman gazing into a magic gem.  She "
        "looks up and says, \"In my travels, I have learned much in the way of "
        "arcane magic.  A great oracle taught me his skill.  I have the answer "
        "you seek.  The %s keyword is '%s'.\"",
        xBarrierColor[color], xPasswordStrings[passwordIndex]);
    NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
    gpCurPlayer->m_barrierTents |= 1 << color;
}

VA(0x004aed38, 0x6fe)
// @early-stop: complete semantics/CFG, 0x54-byte frame, and every source stack
// slot are recovered. Both sides have 496 non-table instructions; retail is one
// byte longer solely from the equivalent outer map-width comparison sequence.
// The monster hit-point relocation is gMonsterDatabase+8 in base and the same
// effective address const_000faeb8 in retail; other residuals are branch and
// delinked seven-entry switch-table identities.
void advManager::GenericSiteEvent(mapCell *cell, hero *eventHero)
{
    int currentSiteType;
    mapCell *currentCell5;
    int index3;
    int siteLevel6;
    int primaryStat15;
    int siteType2;
    int mapY9;
    int mapX37;
    int unusedSite;
    SAMPLE2 eventSample5;
    int cursedArtifactCount9;
    int unusedOne18;
    signed char stableResult26;
    int unusedTwo6;
    int creatureType;
    int experience11;
    int oldQuantity4;

    cursedArtifactCount9 = 0;
    eventSample5 = NULL_SAMPLE2;
    siteType2 = cell->w4hi;
    siteType2 &= GENERIC_SITE_TYPE_MASK;
    siteLevel6 = cell->w4hi;
    siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType2) {
    case GENERIC_SITE_ALCHEMIST_TOWER:
        for (index3 = 0; index3 < GENERIC_SITE_ARTIFACT_SLOT_COUNT; index3++) {
            if (IsCursedItem(eventHero->m_artifacts[index3]))
                cursedArtifactCount9++;
        }
        if (cursedArtifactCount9 != 0) {
            EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                       &eventSample5);
            if (cursedArtifactCount9 == 1) {
                sprintf(
                    gText,
                    "As you enter the Alchemist's Tower, a hobbled, graying man "
                    "in a brown cloak makes his way towards you.  He checks your "
                    "pack, and sees that you have 1 cursed item.  For 750 gold, "
                    "the alchemist will remove it for you.  Do you pay?");
            } else {
                sprintf(
                    gText,
                    "As you enter the Alchemist's Tower, a hobbled, graying man "
                    "in a brown cloak makes his way towards you.  He checks your "
                    "pack, and sees that you have %d cursed items.  For 750 gold, "
                    " the alchemist will remove them for you.  Do you pay?",
                    cursedArtifactCount9);
            }
            NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (gpCurPlayer->m_resources[RES_GOLD] >=
                    GENERIC_SITE_ALCHEMIST_COST) {
                    for (index3 = 0;
                         index3 < GENERIC_SITE_ARTIFACT_SLOT_COUNT; index3++) {
                        if (IsCursedItem(eventHero->m_artifacts[index3]))
                            eventHero->m_artifacts[index3] = -1;
                    }
                    gpCurPlayer->m_resources[RES_GOLD] -=
                        GENERIC_SITE_ALCHEMIST_COST;
                } else {
                    NormalDialog(
                        "You hear a voice from behind the locked door, \"You "
                        "don't have enough gold to pay for my services.\"",
                        1, -1, -1, -1, 0, -1, 0, -1, 0);
                }
            }
        } else {
            NormalDialog(
                "You hear a voice from high above in the tower, \"Go away! I "
                "can't help you!\"",
                1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case GENERIC_SITE_ARENA:
        if (eventHero->m_eventFlags & HERO_EVENT_ARENA) {
            NormalDialog("The Arena guards turn you away.", 1, -1, -1, -1, 0,
                         -1, 0, -1, 0);
        } else {
            EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                       &eventSample5);
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) | HERO_EVENT_ARENA;
            primaryStat15 = DoArenaDialog();
            eventHero->m_primaryStats[primaryStat15]++;
        }
        break;

    case GENERIC_SITE_MERMAIDS:
        if (eventHero->m_eventFlags & HERO_EVENT_MERMAID) {
            NormalDialog(
                "The mermaids silently entice you to return later and be "
                "blessed again.",
                1, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                       &eventSample5);
            eventHero->m_eventFlags = static_cast<int>(eventHero->m_eventFlags) |
                                      HERO_EVENT_MERMAID;
            eventHero->m_luck = eventHero->m_luck + 1;
            EventWindow(
                -1, 1,
                "The magical, soothing beauty of the Mermaids reaches you and "
                "your crew.  Just for a moment, you forget your worries and "
                "bask in the beauty of the moment.  The mermaids charms bless "
                "you with increased luck for your next combat.",
                GENERIC_SITE_MERMAID_WINDOW_ICON, 0, -1, 0, -1);
        }
        break;

    case GENERIC_SITE_HUT_OF_MAGI:
        EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                   &eventSample5);
        NormalDialog(
            "You enter a rickety hut and talk to the magician who lives there.  "
            "He tells you of places near and far which may aid you in your "
            "journeys.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        for (mapX37 = 0; mapX37 < MAP_WIDTH; mapX37++) {
            for (mapY9 = 0; mapY9 < MAP_HEIGHT; mapY9++) {
                currentCell5 = gpGame->m_worldMap.Row(mapY9) + mapX37;
                currentSiteType = currentCell5->w4hi;
                currentSiteType &= BARRIER_COLOR_MASK;
                if ((currentCell5->triggerType & MAP_EVENT_TYPE_MASK) ==
                        MAP_EVENT_EXPANSION_OBJECT &&
                    currentSiteType == GENERIC_SITE_HUT_COLOR) {
                    gpGame->SetVisibility(mapX37, mapY9, giCurPlayer,
                                          GENERIC_SITE_VISIBILITY_RADIUS);
                    CompleteDraw(0);
                    UpdateScreen(0, 0);
                }
            }
        }
        break;

    case GENERIC_SITE_EYE_OF_MAGI:
        NormalDialog("This eye seems to be intently studying its surroundings.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        break;

    case GENERIC_SITE_SIRENS:
        if (eventHero->m_eventFlags & HERO_EVENT_SIRENS) {
            NormalDialog(
                "You have your crew stop up their ears with wax before the "
                "sirens' eerie song has any chance of luring them to a watery "
                "grave.",
                1, -1, -1, -1, 0, -1, 0, -1, 0);
        } else {
            experience11 = 0;
            for (index3 = 0; index3 < GENERIC_SITE_ARMY_SLOT_COUNT; index3++) {
                creatureType = eventHero->m_army.m_creatureTypes[index3];
                if (creatureType != -1) {
                    oldQuantity4 = eventHero->m_army.m_quantities[index3];
                    if (oldQuantity4 > 1) {
                        eventHero->m_army.m_quantities[index3] =
                            static_cast<short>(
                                oldQuantity4 *
                                GENERIC_SITE_SIREN_ARMY_REMAINDER);
                        experience11 +=
                            (oldQuantity4 -
                             eventHero->m_army.m_quantities[index3]) *
                            gMonsterDatabase[creatureType].hitPoints;
                    }
                }
            }
            if (experience11 != 0) {
                EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                           &eventSample5);
                sprintf(
                    gText,
                    "An eerie wailing song emanates from the sirens perched "
                    "upon the rocks.  Many of your crew fall under its spell, "
                    "and dive into the water where they drown.  You are now "
                    "wiser for the visit, and gain %d experience.",
                    experience11);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                GiveExperience(eventHero, experience11, 1);
            } else {
                NormalDialog(
                    "As the sirens sing their eerie song, your small, "
                    "determined army manages to overcome the urge to dive "
                    "headlong into the sea.",
                    1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            eventHero->m_eventFlags = static_cast<int>(eventHero->m_eventFlags) |
                                      HERO_EVENT_SIRENS;
        }
        break;

    case GENERIC_SITE_STABLES:
        unusedOne18 = 1;
        unusedTwo6 = 2;
        stableResult26 = 0;
        if ((eventHero->m_eventFlags & HERO_EVENT_STABLES) == 0) {
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) | HERO_EVENT_STABLES;
            eventHero->m_mobility += GENERIC_SITE_STABLE_MOBILITY;
            eventHero->m_remainingMobility += GENERIC_SITE_STABLE_MOBILITY;
            stableResult26 |= 1;
        }
        if (eventHero->CreatureTypeCount(GENERIC_SITE_STABLE_CREATURE)) {
            eventHero->UpgradeCreatures(GENERIC_SITE_STABLE_CREATURE,
                                        GENERIC_SITE_STABLE_UPGRADE);
            stableResult26 |= 2;
        }
        if (stableResult26 != 0) {
            EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, siteType2,
                       &eventSample5);
        }
        sprintf(gText, xStableText[stableResult26]);
        if (stableResult26 & 2) {
            EventWindow(-1, 1, gText, GENERIC_SITE_STABLE_REWARD_TYPE,
                        GENERIC_SITE_STABLE_REWARD_VALUE, -1, 0, -1);
        } else {
            EventWindow(-1, 1, gText, -1, 0, -1, 0, -1);
        }
        break;
    }
}

// @early-stop
// Relocation-masked: all 0x191 bytes match. Objdiff's residual is the delinked
// switch-label identity and NULL_SAMPLE2's second-word symbol name.
VA(0x004af436, 0x191)
void advManager::RecruitSiteEvent(mapCell *cell, hero *eventHero)
{
    SAMPLE2 eventSample = NULL_SAMPLE2;
    unsigned int siteType2;
    short availableCount;
    int creatureType1;
    unsigned int siteIndex;
    unsigned int packedSite1;

    siteType2 = cell->w4hi;
    siteType2 &= AI_EVENT_RECRUIT_TYPE_MASK;
    availableCount = static_cast<short>(cell->w4hi);
    availableCount >>= AI_EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType2) {
    case RECRUIT_SITE_GENIE:
        creatureType1 = MONSTER_GENIE;
        break;
    case RECRUIT_SITE_EARTH_ELEMENTAL:
        creatureType1 = MONSTER_EARTH_ELEMENTAL;
        break;
    case RECRUIT_SITE_AIR_ELEMENTAL:
        creatureType1 = MONSTER_AIR_ELEMENTAL;
        break;
    case RECRUIT_SITE_FIRE_ELEMENTAL:
        creatureType1 = MONSTER_FIRE_ELEMENTAL;
        break;
    case RECRUIT_SITE_WATER_ELEMENTAL:
        creatureType1 = MONSTER_WATER_ELEMENTAL;
        break;
    }

    siteIndex = siteType2;
    if (availableCount == 0) {
        EventWindow(-1, 1, xRecruitEmpty[siteIndex], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, availableCount,
                   &eventSample);
        EventWindow(-1, 2, xRecruitBuy[siteIndex], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &availableCount);
            packedSite1 =
                (availableCount << AI_EVENT_RECRUIT_COUNT_SHIFT) | siteType2;
            cell->w4hi = packedSite1;
        }
    }
}

VA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(hero *eventHero, int creatureType,
                                       short *availableCount)
{
    // These unused dialog locals account for the retail /Od frame layout.
    tag_message dialogMessage2;
    baseManager *dialogManager =
        new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    int dialogResult;
    if (dialogManager == 0)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
}

// @early-stop
// Relocation-masked: only rel32 bytes +0x7e and +0xb4 differ. The two early
// exits target the epilogue instead of the identical final continuation;
// every non-branch byte and external relocation target matches.
VA(0x004af652, 0x22a)
void advManager::JailEvent(mapCell *cell, hero *eventHero, int x, int y)
{
    SAMPLE2 eventSample1 = NULL_SAMPLE2;
    int heroId9;
    hero *releasedHero1;

    heroId9 = cell->w4hi;
    if (gpGame->m_availableHeroes[heroId9] != AI_EVENT_JAILED_HERO) {
        NormalDialog(
            "The jailer tells you that the hero who was imprisoned here has been released by the king who imprisoned him.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        EraseObj(cell, x, y);
    } else if (gpCurPlayer->m_heroCount >= AI_EVENT_HERO_LIMIT) {
        NormalDialog(
            "You already have 8 heroes, and regretfully must leave the prisoner in this jail to languish in agony for untold days.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
    } else {
        EventSound(cell->triggerType & MAP_EVENT_TYPE_MASK, 0, &eventSample1);
        NormalDialog(
            "In a dazzling display of daring, you break into the local jail and free the hero imprisoned there, who, in return, pledges loyalty to your cause.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        gpGame->m_heroRecs[heroId9].m_owner = eventHero->m_owner;
        gpGame->m_availableHeroes[heroId9] = eventHero->m_owner;
        releasedHero1 = &gpGame->m_heroRecs[heroId9];
        EraseObj(cell, x, y);
        gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
            static_cast<signed char>(heroId9);
        gpCurPlayer->m_heroCount++;
        releasedHero1->m_x = x;
        releasedHero1->m_y = y;
        releasedHero1->m_eventFlags = 0;
        releasedHero1->m_direction = AI_EVENT_HERO_DIRECTION;
        releasedHero1->m_remainingMobility = releasedHero1->CalcMobility();
        releasedHero1->m_mobility = releasedHero1->m_remainingMobility;
        releasedHero1->m_locationType = cell->triggerType;
        releasedHero1->m_occupiedTown = cell->w4hi;
        cell->triggerType = MAP_EVENT_ACTION_FLAG | MAP_EVENT_HERO_INTERACTION;
        cell->w4hi = heroId9;
        SendMapChange(AI_EVENT_HERO_MAP_CHANGE,
                      static_cast<signed char>(heroId9), x, y,
                      AI_EVENT_HERO_MAP_CHANGE_VALUE, 0, 0);
    }
}

VA(0x004af87c, 0x1da)
void advManager::TownEvent(mapCell *cell, int x, int y)
{
    hero *eventHero1;
    int combatResult1;
    hero *defendingHero;
    town *eventTown1;

    eventTown1 = gpGame->GetTown(cell->w4hi);
    eventHero1 = gpGame->GetHero(gpCurPlayer->m_currentHero);
    DemobilizeCurrHero();
    if (eventTown1->m_owner == giCurPlayer) {
        eventTown1->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown1->View(0);
    } else if (eventTown1->HasGarrison()) {
        defendingHero = eventTown1->m_occupyingHeroId == -1
                            ? 0
                            : gpGame->GetHero(eventTown1->m_occupyingHeroId);
        combatResult1 =
            DoCombat(x, y, eventHero1, &eventHero1->m_army, eventTown1,
                     defendingHero, &eventTown1->m_army, x, y, -1, 1);
        if (combatResult1 == 0)
            gpGame->ClaimTown(eventTown1->m_id, giCurPlayer, 0);
    } else {
        gpGame->ClaimTown(eventTown1->m_id, giCurPlayer, 0);
        if (gbGameOver)
            return;
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
        UpdateTownLocators(1, 1);
        eventTown1->m_occupyingHeroId = gpCurPlayer->CurrentHero();
        eventTown1->View(0);
    }
    eventTown1->GiveSpells(0);
    eventHero1->CheckLevel();
}

// @early-stop
// Excluding address tables +0x1cb..+0x1e7, +0x235..+0x31d, and
// +0x468..+0x484 plus byte tables +0x31d..+0x396 and +0x484..+0x4d9,
// all 176 normalized instructions match. Residuals are delinked local-table
// relocations and string symbol names; external relocation targets match.
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

// @early-stop
// Relocation-masked raw bytes are identical; the only residual is the delinked
// constant-pool identity for the identical "Event ID %d" literal.
VA(0x004aff6c, 0xc7)
void advManager::EventWindow(int eventId, int buttons, char *text, int type1,
                             int value1, int type2, int value2, int type3)
{
    // Unused retail locals retain the original /Od frame around the text buffer.
    int dialogState7;
    int eventWindowUnused12;
    int windowGap1;
    int unusedStyle9;
    int unusedResult;
    int dialogState8;
    int eventWindowUnused11;
    int unusedStyle;
    char eventText[EVENT_TEXT_BUFFER_SIZE];

    unusedResult = 0;
    unusedStyle9 = 1;

    if (eventId >= 0 && eventId < EVENT_TEXT_COUNT)
        sprintf(eventText, gEventText[eventId]);
    else if (eventId == MAP_EVENT_REWARD_NONE)
        sprintf(eventText, text);
    else
        sprintf(eventText, "Event ID %d", eventId);

    NormalDialog(eventText, buttons, -1, -1, type1, value1, type2, value2,
                 type3, 0);
}

// @early-stop
// The base is five bytes larger solely because of the unreachable jump at
// +0x44. With branch destinations normalized, every other opcode and operand
// matches; all three external relocation targets also agree.
VA(0x004b0033, 0xb6)
int GiveArtifact(hero *eventHero, int artifact, int checkEndGame,
                 signed char artifactExtra)
{
    int artifactSlot;

    for (artifactSlot = 0; artifactSlot < EVENT_ARTIFACT_SLOT_COUNT;
         artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == EVENT_ARTIFACT_NONE) {
            break;
        } else {
        }
    }

    if (artifactSlot == EVENT_ARTIFACT_SLOT_COUNT) {
        return EVENT_ARTIFACT_NONE;
    }

    eventHero->m_artifacts[artifactSlot] = artifact;
    eventHero->m_artifactExtra[artifactSlot] = artifactExtra;
    GiveTakeArtifactStat(eventHero, artifact, 0);
    eventHero->CheckAnduranPieces(0);
    if (checkEndGame)
        CheckEndGame(0, 0);
    return artifactSlot;
}

VA(0x004b00e9, 0x5e)
int advManager::GiveRandomArtifact(hero *eventHero)
{
    int artifactId = gpGame->GetRandomArtifactId(EVENT_RANDOM_ARTIFACT_ANY, 1);

    if (artifactId == EVENT_ARTIFACT_NONE)
        GiveResource(eventHero, RES_GOLD, EVENT_RANDOM_ARTIFACT_GOLD);
    else
        GiveArtifact(eventHero, artifactId, 1, EVENT_ARTIFACT_NONE);
    return artifactId;
}

VA(0x004b0147, 0x67)
int advManager::GiveExperience(hero *eventHero, int experience, int checkLevel)
{
    // These unused level locals account for the retail /Od stack slots.
    int oldLevel;
    int unusedLevel2;
    int unusedLevel1;
    int newLevel1;
    int levelGap1;

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = static_cast<short>(oldLevel);
    eventHero->m_experience += experience;
    newLevel1 = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel1 - oldLevel;
}

VA(0x004b01ae, 0x80)
void advManager::GiveResource(hero *eventHero, int resourceType, int amount)
{
    if (resourceType >= 0 && resourceType <= RES_GOLD)
        gpGame->m_players[eventHero->m_owner].resources[resourceType] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[eventHero->m_owner])
        CheckEndGame(0, 0);
}

VA(0x004b022e, 0xbb)
void advManager::RecruitEvent(hero *eventHero, int creatureType, mapCell *cell)
{
    tag_message dialogMessage2;
    short availableCount15 = static_cast<short>(cell->w4hi);
    baseManager *dialogManager =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount15);
    int dialogResult;

    if (dialogManager == 0)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
    cell->w4hi = static_cast<unsigned short>(availableCount15 + 0);
}

VA(0x004b02e9, 0x261)
int advManager::SkeletonEvent(hero *eventHero, mapCell *cell, char *text,
                              int x, int y)
{
    int artifactId;

    switch (cell->w4hi) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_SKELETON,
                               SKELETON_EVENT_SMALL_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, SKELETON_EVENT_SMALL_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_SMALL_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_MEDIUM:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_SKELETON,
                               SKELETON_EVENT_MEDIUM_COUNT, cell, x, y, 0, x,
                               y, -1, 0, 0, -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, SKELETON_EVENT_MEDIUM_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_MEDIUM_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_LARGE:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_SKELETON,
                               SKELETON_EVENT_LARGE_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, SKELETON_EVENT_LARGE_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_LARGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    default:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_SKELETON,
                               SKELETON_EVENT_HUGE_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, SKELETON_EVENT_HUGE_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, SKELETON_EVENT_HUGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    }
    return 0;
}

VA(0x004b054a, 0x29b)
int advManager::ZombieEvent(hero *eventHero, mapCell *cell, char *text,
                            int x, int y)
{
    int artifactId;
    switch (cell->w4hi) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(
                eventHero, ARMY_CREATURE_ZOMBIE, ZOMBIE_EVENT_SMALL_COUNT,
                cell, x, y, 0, x, y, ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_SMALL_SUPPORT_COUNT, ZOMBIE_EVENT_SUPPORT_STACKS,
                -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, ZOMBIE_EVENT_SMALL_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_SMALL_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_MEDIUM:
        if (CombatMonsterEvent(
                eventHero, ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_MEDIUM_COUNT, cell, x, y, 0, x, y,
                ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_MEDIUM_SUPPORT_COUNT, ZOMBIE_EVENT_SUPPORT_STACKS,
                -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, ZOMBIE_EVENT_MEDIUM_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_MEDIUM_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_LARGE:
        if (CombatMonsterEvent(
                eventHero, ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_LARGE_COUNT, cell, x, y, 0, x, y,
                ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_LARGE_SUPPORT_COUNT, ZOMBIE_EVENT_SUPPORT_STACKS,
                -1, 0, 0) == 0) {
            EventWindow(-1, 1, text, RES_GOLD, ZOMBIE_EVENT_LARGE_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_LARGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    default:
        if (CombatMonsterEvent(
                eventHero, ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_HUGE_COUNT, cell, x, y, 0, x, y,
                ARMY_CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_HUGE_SUPPORT_COUNT, ZOMBIE_EVENT_SUPPORT_STACKS,
                -1, 0, 0) == 0) {
            artifactId = GiveRandomArtifact(eventHero);
            if (artifactId != EVENT_ARTIFACT_NONE)
                EventWindow(-1, 1, text, RES_GOLD, ZOMBIE_EVENT_HUGE_GOLD,
                            MAP_EVENT_REWARD_ARTIFACT, artifactId, -1);
            else
                EventWindow(-1, 1, text, RES_GOLD, ZOMBIE_EVENT_HUGE_GOLD,
                            -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, ZOMBIE_EVENT_HUGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    }
    return 0;
}

VA(0x004b07e5, 0x2f8)
int advManager::GhostEvent(hero *eventHero, mapCell *cell, char *text,
                           int x, int y)
{
    int artifactId;
    switch (cell->w4hi) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_GHOST,
                               GHOST_EVENT_SMALL_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            sprintf(gText, "%s", text);
            EventWindow(-1, 1, gText, RES_GOLD, GHOST_EVENT_SMALL_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, GHOST_EVENT_SMALL_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_MEDIUM:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_GHOST,
                               GHOST_EVENT_MEDIUM_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            sprintf(gText, "%s", text);
            EventWindow(-1, 1, gText, RES_GOLD, GHOST_EVENT_MEDIUM_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, GHOST_EVENT_MEDIUM_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    case UNDEAD_EVENT_LEVEL_LARGE:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_GHOST,
                               GHOST_EVENT_LARGE_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            sprintf(gText, "%s", text);
            EventWindow(-1, 1, gText, RES_GOLD, GHOST_EVENT_LARGE_GOLD,
                        -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, GHOST_EVENT_LARGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    default:
        if (CombatMonsterEvent(eventHero, ARMY_CREATURE_GHOST,
                               GHOST_EVENT_HUGE_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            artifactId = GiveRandomArtifact(eventHero);
            sprintf(gText, "%s", text);
            if (artifactId != EVENT_ARTIFACT_NONE)
                EventWindow(-1, 1, gText, RES_GOLD, GHOST_EVENT_HUGE_GOLD,
                            MAP_EVENT_REWARD_ARTIFACT, artifactId, -1);
            else
                EventWindow(-1, 1, gText, RES_GOLD, GHOST_EVENT_HUGE_GOLD,
                            -1, 0, -1);
            GiveResource(eventHero, RES_GOLD, GHOST_EVENT_HUGE_GOLD);
            eventHero->CheckLevel();
            return 1;
        }
        break;
    }
    return 0;
}

VA(0x004b0add, 0x274)
void advManager::HouseEvent(class hero *, class mapCell *) {}

// @early-stop
// raw instructions/slots identical; gMonsterDatabase+0x16 and const_000faec6 resolve to the same address
VA(0x004b0d51, 0x62f)
int advManager::CombatMonsterEvent(hero *eventHero, int monsterType,
                                   int monsterCount, mapCell *, int mapX,
                                   int mapY, int defender, int combatX,
                                   int combatY, int secondaryType,
                                   int secondaryCount, int secondaryStacks,
                                   int tertiaryType, int tertiaryCount,
                                   int tertiaryStacks)
{
    int stackCount;
    int stackIndex9;
    int combatResult7;
    int lastStackCount;
    int temporaryTypes7[COMBAT_MONSTER_ARMY_SLOTS];
    int temporaryCounts[COMBAT_MONSTER_ARMY_SLOTS];
    int placement4[COMBAT_MONSTER_ARMY_SLOTS + 1];
    int groupCount;
    int stackTotal;

    DemobilizeCurrHero();
    if (combatX == -1) {
        combatX = mapX;
        combatY = mapY;
    } else {
        m_lastQuickViewX = combatX;
        m_lastQuickViewY = combatY;
        if (eventHero->m_x >= combatX)
            m_field_0x2ba = 0;
        else
            m_field_0x2ba = 1;
        if (ComboDraw(0))
            UpdateScreen(0, 0);
        m_lastQuickViewX = -1;
    }

    memset(gpMonGroup->m_creatureTypes, COMBAT_MONSTER_INVALID_TYPE,
           COMBAT_MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0,
           COMBAT_MONSTER_ARMY_SLOTS * sizeof(short));
    stackCount = COMBAT_MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    placement4[COMBAT_MONSTER_ARMY_SLOTS] = 0;
    SRand(combatX + combatY);
    if (stackCount == COMBAT_MONSTER_ARMY_SLOTS &&
        (gMonsterDatabase[monsterType].flags.all & MONSTER_FLAGS_SHOOTER) == 0) {
        int roll = SRandom(0, COMBAT_MONSTER_RANDOM_MAX);
        if (roll < COMBAT_MONSTER_REDUCED_STACK_CHANCE)
            stackCount = 3;
        else if (roll < COMBAT_MONSTER_FOUR_STACK_THRESHOLD)
            stackCount = 4;
    }

    for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
        if (stackIndex9 == (stackCount >> 1) &&
            (monsterType == ARMY_CREATURE_ARCHER || monsterType == ARMY_CREATURE_PIKEMAN ||
             monsterType == ARMY_CREATURE_SWORDSMAN || monsterType == ARMY_CREATURE_CAVALRY ||
             monsterType == ARMY_CREATURE_PALADIN || monsterType == ARMY_CREATURE_ORC ||
             monsterType == ARMY_CREATURE_OGRE || monsterType == ARMY_CREATURE_TROLL ||
             monsterType == ARMY_CREATURE_DWARF || monsterType == ARMY_CREATURE_ELF ||
             monsterType == ARMY_CREATURE_DRUID || monsterType == ARMY_CREATURE_MINOTAUR ||
             monsterType == ARMY_CREATURE_GREEN_DRAGON || monsterType == ARMY_CREATURE_RED_DRAGON ||
             monsterType == ARMY_CREATURE_IRON_GOLEM || monsterType == ARMY_CREATURE_MAGE ||
             monsterType == ARMY_CREATURE_GIANT || monsterType == ARMY_CREATURE_ZOMBIE ||
             monsterType == ARMY_CREATURE_MUMMY || monsterType == ARMY_CREATURE_VAMPIRE ||
             monsterType == ARMY_CREATURE_LICH) &&
            SRandom(0, COMBAT_MONSTER_RANDOM_MAX) < COMBAT_MONSTER_UPGRADE_CHANCE &&
            secondaryCount == 0 && tertiaryCount == 0)
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<signed char>(monsterType + 1);
        else
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<signed char>(monsterType);
        gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
            static_cast<short>((stackIndex9 < monsterCount % stackCount) +
                               monsterCount / stackCount);
    }

    placement4[COMBAT_MONSTER_ARMY_SLOTS] += stackCount;
    if (secondaryStacks != 0) {
        stackCount = secondaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<signed char>(secondaryType);
            gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<short>((stackIndex9 < secondaryCount % stackCount) +
                                   secondaryCount / stackCount);
        }
    }

    placement4[COMBAT_MONSTER_ARMY_SLOTS] += stackCount;
    if (tertiaryStacks != 0) {
        stackCount = tertiaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<signed char>(tertiaryType);
            gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<short>((stackIndex9 < secondaryCount % stackCount) +
                                   tertiaryCount / stackCount);
        }
    }
    lastStackCount = stackCount;

    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++) {
        if (gpMonGroup->m_creatureCounts[stackIndex9] <= 0)
            gpMonGroup->m_creatureTypes[stackIndex9] = COMBAT_MONSTER_INVALID_TYPE;
    }
    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++)
        placement4[stackIndex9] = stackIndex9;

    if (stackCount == 1) {
        placement4[2] = 0;
        placement4[0] = 2;
    } else if (stackCount == 2) {
        placement4[1] = 1;
        placement4[3] = 0;
        placement4[0] = 3;
    } else if (stackCount == 3) {
        placement4[0] = 3;
        placement4[1] = 0;
        placement4[2] = 1;
        placement4[3] = 2;
    } else if (tertiaryStacks == 1 && secondaryStacks == 1) {
        placement4[1] = 4;
        placement4[4] = 1;
    } else if (tertiaryStacks == 1 && secondaryStacks == 2) {
        placement4[1] = 2;
        placement4[2] = 4;
        placement4[3] = 3;
        placement4[4] = 1;
    } else if (secondaryStacks == 2) {
        placement4[1] = 4;
        placement4[4] = 1;
    }

    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++) {
        temporaryTypes7[stackIndex9] = gpMonGroup->m_creatureTypes[stackIndex9];
        temporaryCounts[stackIndex9] = gpMonGroup->m_creatureCounts[stackIndex9];
    }
    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++) {
        gpMonGroup->m_creatureTypes[stackIndex9] =
            static_cast<signed char>(temporaryTypes7[placement4[stackIndex9]]);
        gpMonGroup->m_creatureCounts[stackIndex9] =
            static_cast<short>(temporaryCounts[placement4[stackIndex9]]);
    }

    if (defender != 0)
        combatResult7 = DoCombat(combatX, combatY, 0, gpMonGroup,
                                0, eventHero, &eventHero->m_army, mapX, mapY,
                                combatX + combatY, 1);
    else
        combatResult7 = DoCombat(combatX, combatY, eventHero, &eventHero->m_army,
                                0, 0, gpMonGroup, mapX, mapY,
                                combatX + combatY, 1);
    MobilizeCurrHero(0);
    return combatResult7;
}

// @early-stop
// non-table instructions/slots identical; 103 table entries differ only by delinked local-label identity
VA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(hero *targetHero, int artifact, int take)
{
    int statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT + 1];
    int maxSpellPoints;

    if (artifact == EVENT_ARTIFACT_NONE)
        return;
    statChanges[HERO_PRIMARY_ATTACK] = 0;
    statChanges[HERO_PRIMARY_DEFENSE] = 0;
    statChanges[HERO_PRIMARY_SPELL_POWER] = 0;
    statChanges[HERO_PRIMARY_KNOWLEDGE] = 0;

    switch (artifact) {
    case EVENT_ARTIFACT_ULTIMATE_BOOK: statChanges[HERO_PRIMARY_KNOWLEDGE] = 12; break;
    case EVENT_ARTIFACT_ULTIMATE_SWORD: statChanges[HERO_PRIMARY_ATTACK] = 12; break;
    case EVENT_ARTIFACT_ULTIMATE_CLOAK: statChanges[HERO_PRIMARY_DEFENSE] = 12; break;
    case EVENT_ARTIFACT_ULTIMATE_WAND: statChanges[HERO_PRIMARY_SPELL_POWER] = 12; break;
    case EVENT_ARTIFACT_ULTIMATE_SHIELD:
        statChanges[HERO_PRIMARY_ATTACK] = 6;
        statChanges[HERO_PRIMARY_DEFENSE] = 6;
        break;
    case EVENT_ARTIFACT_ULTIMATE_STAFF:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 6;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 6;
        break;
    case EVENT_ARTIFACT_ULTIMATE_CROWN:
        statChanges[HERO_PRIMARY_ATTACK] = 4;
        statChanges[HERO_PRIMARY_DEFENSE] = 4;
        statChanges[HERO_PRIMARY_SPELL_POWER] = 4;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 4;
        break;
    case EVENT_ARTIFACT_GOLDEN_GOOSE: break;
    case EVENT_ARTIFACT_ARCANE_NECKLACE: statChanges[HERO_PRIMARY_SPELL_POWER] = 4; break;
    case EVENT_ARTIFACT_CASTER_BRACELET: statChanges[HERO_PRIMARY_SPELL_POWER] = 2; break;
    case EVENT_ARTIFACT_MAGE_RING: statChanges[HERO_PRIMARY_SPELL_POWER] = 2; break;
    case EVENT_ARTIFACT_WITCHES_BROACH: statChanges[HERO_PRIMARY_SPELL_POWER] = 3; break;
    case EVENT_ARTIFACT_MEDAL_VALOR: break;
    case EVENT_ARTIFACT_MEDAL_COURAGE: break;
    case EVENT_ARTIFACT_MEDAL_HONOR: break;
    case EVENT_ARTIFACT_MEDAL_DISTINCTION: break;
    case EVENT_ARTIFACT_FIZBIN_MISFORTUNE: break;
    case EVENT_ARTIFACT_THUNDER_MACE: statChanges[HERO_PRIMARY_ATTACK] = 1; break;
    case EVENT_ARTIFACT_ARMORED_GAUNTLETS: statChanges[HERO_PRIMARY_DEFENSE] = 1; break;
    case EVENT_ARTIFACT_DEFENDER_HELM: statChanges[HERO_PRIMARY_DEFENSE] = 1; break;
    case EVENT_ARTIFACT_GIANT_FLAIL: statChanges[HERO_PRIMARY_ATTACK] = 1; break;
    case EVENT_ARTIFACT_BALLISTA: break;
    case EVENT_ARTIFACT_STEALTH_SHIELD: statChanges[HERO_PRIMARY_DEFENSE] = 2; break;
    case EVENT_ARTIFACT_DRAGON_SWORD: statChanges[HERO_PRIMARY_ATTACK] = 3; break;
    case EVENT_ARTIFACT_POWER_AXE: statChanges[HERO_PRIMARY_ATTACK] = 2; break;
    case EVENT_ARTIFACT_DIVINE_BREASTPLATE: statChanges[HERO_PRIMARY_DEFENSE] = 3; break;
    case EVENT_ARTIFACT_MINOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 2; break;
    case EVENT_ARTIFACT_MAJOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 3; break;
    case EVENT_ARTIFACT_SUPERIOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 4; break;
    case EVENT_ARTIFACT_FOREMOST_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 5; break;
    case EVENT_ARTIFACT_ENDLESS_SACK_GOLD: break;
    case EVENT_ARTIFACT_ENDLESS_BAG_GOLD: break;
    case EVENT_ARTIFACT_ENDLESS_PURSE_GOLD: break;
    case EVENT_ARTIFACT_NOMAD_BOOTS: break;
    case EVENT_ARTIFACT_TRAVELER_BOOTS: break;
    case EVENT_ARTIFACT_RABBIT_FOOT: break;
    case EVENT_ARTIFACT_GOLDEN_HORSESHOE: break;
    case EVENT_ARTIFACT_GAMBLERS_COIN: break;
    case EVENT_ARTIFACT_FOUR_LEAF_CLOVER: break;
    case EVENT_ARTIFACT_TRUE_COMPASS: break;
    case EVENT_ARTIFACT_SAILORS_ASTROLABE: break;
    case EVENT_ARTIFACT_EVIL_EYE: break;
    case EVENT_ARTIFACT_ENCHANTED_HOURGLASS: break;
    case EVENT_ARTIFACT_GOLD_WATCH: break;
    case EVENT_ARTIFACT_SKULLCAP: break;
    case EVENT_ARTIFACT_ICE_CLOAK: break;
    case EVENT_ARTIFACT_FIRE_CLOAK: break;
    case EVENT_ARTIFACT_LIGHTNING_HELM: break;
    case EVENT_ARTIFACT_EVERCOLD_ICICLE: break;
    case EVENT_ARTIFACT_EVERHOT_LAVA_ROCK: break;
    case EVENT_ARTIFACT_LIGHTNING_ROD: break;
    case EVENT_ARTIFACT_SNAKE_RING: break;
    case EVENT_ARTIFACT_ANKH: break;
    case EVENT_ARTIFACT_BOOK_ELEMENTS: break;
    case EVENT_ARTIFACT_ELEMENTAL_RING: break;
    case EVENT_ARTIFACT_HOLY_PENDANT: break;
    case EVENT_ARTIFACT_PENDANT_FREE_WILL: break;
    case EVENT_ARTIFACT_PENDANT_LIFE: break;
    case EVENT_ARTIFACT_SERENITY_PENDANT: break;
    case EVENT_ARTIFACT_SEEING_EYE_PENDANT: break;
    case EVENT_ARTIFACT_KINETIC_PENDANT: break;
    case EVENT_ARTIFACT_PENDANT_DEATH: break;
    case EVENT_ARTIFACT_WAND_NEGATION: break;
    case EVENT_ARTIFACT_GOLDEN_BOW: break;
    case EVENT_ARTIFACT_TELESCOPE: break;
    case EVENT_ARTIFACT_STATESMAN_QUILL: break;
    case EVENT_ARTIFACT_WIZARD_HAT: break;
    case EVENT_ARTIFACT_POWER_RING: break;
    case EVENT_ARTIFACT_AMMO_CART: break;
    case EVENT_ARTIFACT_TAX_LIEN: break;
    case EVENT_ARTIFACT_HIDEOUS_MASK: break;
    case EVENT_ARTIFACT_ENDLESS_POUCH_SULFUR: break;
    case EVENT_ARTIFACT_ENDLESS_VIAL_MERCURY: break;
    case EVENT_ARTIFACT_ENDLESS_POUCH_GEMS: break;
    case EVENT_ARTIFACT_ENDLESS_CORD_WOOD: break;
    case EVENT_ARTIFACT_ENDLESS_CART_ORE: break;
    case EVENT_ARTIFACT_ENDLESS_POUCH_CRYSTAL: break;
    case EVENT_ARTIFACT_SPIKED_HELM:
        statChanges[HERO_PRIMARY_ATTACK] = 1;
        statChanges[HERO_PRIMARY_DEFENSE] = 1;
        break;
    case EVENT_ARTIFACT_SPIKED_SHIELD:
        statChanges[HERO_PRIMARY_ATTACK] = 2;
        statChanges[HERO_PRIMARY_DEFENSE] = 2;
        break;
    case EVENT_ARTIFACT_WHITE_PEARL:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 1;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 1;
        break;
    case EVENT_ARTIFACT_BLACK_PEARL:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 2;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 2;
        break;
    case EVENT_ARTIFACT_MAGIC_BOOK: break;
    case EVENT_ARTIFACT_EDITOR_ANY_ULTIMATE:
    case EVENT_ARTIFACT_EDITOR_UNUSED_84:
    case EVENT_ARTIFACT_EDITOR_UNUSED_85:
    case EVENT_ARTIFACT_EDITOR_UNUSED_86:
    case EVENT_ARTIFACT_SPELL_SCROLL:
        break;
    case EVENT_ARTIFACT_ARM_OF_MARTYR: statChanges[HERO_PRIMARY_SPELL_POWER] = 3; break;
    case EVENT_ARTIFACT_BREASTPLATE_ANDURAN: statChanges[HERO_PRIMARY_DEFENSE] = 5; break;
    case EVENT_ARTIFACT_BROACH_SHIELDING: statChanges[HERO_PRIMARY_SPELL_POWER] = -2; break;
    case EVENT_ARTIFACT_BATTLE_GARB:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 5;
        statChanges[HERO_PRIMARY_DEFENSE] = 5;
        statChanges[HERO_PRIMARY_ATTACK] = 5;
        break;
    case EVENT_ARTIFACT_CRYSTAL_BALL: break;
    case EVENT_ARTIFACT_HEART_FIRE: break;
    case EVENT_ARTIFACT_HEART_ICE: break;
    case EVENT_ARTIFACT_HELMET_ANDURAN: statChanges[HERO_PRIMARY_SPELL_POWER] = 5; break;
    case EVENT_ARTIFACT_HOLY_HAMMER: statChanges[HERO_PRIMARY_ATTACK] = 5; break;
    case EVENT_ARTIFACT_LEGENDARY_SCEPTER:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 2;
        statChanges[HERO_PRIMARY_ATTACK] = 2;
        statChanges[HERO_PRIMARY_DEFENSE] = 2;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 2;
        break;
    case EVENT_ARTIFACT_MASTHEAD: break;
    case EVENT_ARTIFACT_SPHERE_NEGATION: break;
    case EVENT_ARTIFACT_STAFF_WIZARDRY: statChanges[HERO_PRIMARY_SPELL_POWER] = 5; break;
    case EVENT_ARTIFACT_SWORD_BREAKER:
        statChanges[HERO_PRIMARY_DEFENSE] = 4;
        statChanges[HERO_PRIMARY_ATTACK] = 1;
        break;
    case EVENT_ARTIFACT_SWORD_ANDURAN: statChanges[HERO_PRIMARY_ATTACK] = 5; break;
    case EVENT_ARTIFACT_SPADE_NECROMANCY: break;
    default: break;
    }

    for (statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] = 0;
         statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] < EVENT_ARTIFACT_PRIMARY_STAT_COUNT;
         statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]++) {
        targetHero->m_primaryStats[
            statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]] +=
            statChanges[statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT]] *
            (take == EVENT_ARTIFACT_TAKE ? -1 : 1);
        if (statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT] == HERO_PRIMARY_KNOWLEDGE &&
            take == EVENT_ARTIFACT_TAKE) {
            maxSpellPoints = targetHero->Stats(HERO_PRIMARY_KNOWLEDGE) *
                             EVENT_ARTIFACT_SPELL_POINT_MULTIPLIER;
            if (targetHero->m_spellPoints > maxSpellPoints)
                targetHero->m_spellPoints = static_cast<short>(maxSpellPoints);
        }
    }
}

// @early-stop
// reloc-masked instructions/slots identical; only the string-literal symbol identity differs
VA(0x004b1973, 0x1dd)
void advManager::TransferArtifacts(hero *sourceHero, hero *destinationHero)
{
    int targetSlot;
    int sourceArtifactSlot;

    if (sourceHero != 0) {
        if (destinationHero == 0) {
        } else {
            for (targetSlot = 0; targetSlot < EVENT_ARTIFACT_SLOT_COUNT;
                 targetSlot++) {
                if (destinationHero->m_artifacts[targetSlot] == EVENT_ARTIFACT_NONE) {
                    for (sourceArtifactSlot = 0;
                         sourceArtifactSlot < EVENT_ARTIFACT_SLOT_COUNT;
                         sourceArtifactSlot++) {
                        if (sourceHero->m_artifacts[sourceArtifactSlot] != EVENT_ARTIFACT_NONE &&
                            sourceHero->m_artifacts[sourceArtifactSlot] != EVENT_ARTIFACT_MAGIC_BOOK) {
                            if (sourceHero->m_artifacts[sourceArtifactSlot] <=
                                EVENT_ARTIFACT_NON_TRANSFERABLE_LAST) {
                                if (gbThisNetHumanPlayer[sourceHero->m_owner] ||
                                    gbThisNetHumanPlayer[destinationHero->m_owner]) {
                                    sprintf(gText,
                                            "As you reach for the %s, it mysteriously disappears.",
                                            gArtifactNames[sourceHero->m_artifacts[sourceArtifactSlot]]);
                                    NormalDialog(gText, 1, -1, -1, 7,
                                                 sourceHero->m_artifacts[sourceArtifactSlot],
                                                 -1, 0, -1, 0);
                                }
                            } else {
                                GiveTakeArtifactStat(
                                    destinationHero,
                                    sourceHero->m_artifacts[sourceArtifactSlot], 0);
                                destinationHero->m_artifacts[targetSlot] =
                                    sourceHero->m_artifacts[sourceArtifactSlot];
                                destinationHero->m_artifactExtra[targetSlot] =
                                    sourceHero->m_artifactExtra[sourceArtifactSlot];
                            }
                            GiveTakeArtifactStat(sourceHero,
                                                 sourceHero->m_artifacts[sourceArtifactSlot], 1);
                            sourceHero->m_artifacts[sourceArtifactSlot] = EVENT_ARTIFACT_NONE;
                            sourceHero->m_artifactExtra[sourceArtifactSlot] = EVENT_ARTIFACT_NONE;
                            break;
                        }
                    }
                }
            }
            destinationHero->CheckAnduranPieces(0);
        }
    }
}

// @early-stop
// raw instructions identical; retail carries one trailing function-boundary nop
VA(0x004b1b50, 0x7f)
void advManager::HeroLoses(hero *lostHero)
{
    if (lostHero == 0) {
    } else {
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        lostHero->Deallocate(1);
        FizzleCenter(EVENT_FIZZLE_HERO_LOSS);
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
    }
}

// @early-stop
// raw instructions/slots identical; the fight-value relocation names the same interior database address
VA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(hero *eventHero)
{
    int armyValue;
    int weakestValue;
    int selectedSlot;
    int slotIndex;
    int groupValues[COMBAT_MONSTER_ARMY_SLOTS];

    if (gbHumanPlayer[eventHero->m_owner] == 0) {
    } else {
        if (Random(EVENT_WHIRLPOOL_TRIGGER_ROLL, EVENT_WHIRLPOOL_TRIGGER_MAX) !=
            EVENT_WHIRLPOOL_TRIGGER_ROLL) {
        } else {
            weakestValue = EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT;
            selectedSlot = -1;
            for (slotIndex = 0; slotIndex < COMBAT_MONSTER_ARMY_SLOTS; slotIndex++) {
                if (eventHero->m_army.m_creatureCounts[slotIndex] > 0) {
                    armyValue =
                        gMonsterDatabase[eventHero->m_army.m_creatureTypes[slotIndex]].fightValue *
                        eventHero->m_army.m_creatureCounts[slotIndex];
                    if (weakestValue > armyValue) {
                        weakestValue = armyValue;
                        selectedSlot = slotIndex;
                    }
                }
            }
            if (eventHero->m_army.GetNumArmies() > 1) {
                eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
                if (eventHero->m_army.m_creatureCounts[selectedSlot] == 0)
                    eventHero->m_army.m_creatureTypes[selectedSlot] = COMBAT_MONSTER_INVALID_TYPE;
            } else if (eventHero->m_army.m_creatureCounts[selectedSlot] > 1) {
                eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
            }
        }
    }
}

// @early-stop
// reloc-masked instructions/slots identical; only literal and interior aggregate relocation identities differ
VA(0x004b1d01, 0x142)
void advManager::FizzleCenter(int fizzleType)
{
    SAMPLE2 playedSample;
    int fizzleSteps;

    if (bShowIt == 0) {
    } else {
        switch (fizzleType) {
        case EVENT_FIZZLE_HERO_LOSS:
            sprintf(gText, "killfade.82M");
            break;
        case EVENT_FIZZLE_ARTIFACT:
            sprintf(gText, "pickup%02d.82M", Random(1, 7));
            break;
        default:
            return;
        }
        playedSample = NULL_SAMPLE2;
        playedSample = LoadPlaySample(gText);
        gpMouseManager->HideColorPointer();
        gpWindowManager->SaveFizzleSource(EVENT_FIZZLE_X, EVENT_FIZZLE_Y,
                                          EVENT_FIZZLE_WIDTH, EVENT_FIZZLE_HEIGHT);
        CompleteDraw(0);
        fizzleSteps = EVENT_FIZZLE_STEPS;
        gpWindowManager->FizzleForward(EVENT_FIZZLE_X, EVENT_FIZZLE_Y,
                                       EVENT_FIZZLE_WIDTH, EVENT_FIZZLE_HEIGHT,
                                       fizzleSteps, 0, 0);
        gpMouseManager->ShowColorPointer();
        WaitEndSample(playedSample, -1);
    }
}

// @early-stop
// Pre-95% structural wall: exact 0x150 frame, four jump tables, and 356/356
// relocations; 68 localized non-table lowering hunks remain.
VA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(mapCell *cell, hero *eventHero, int x, int y)
{
    float battleStatValue_o;
    float spellValueFactor_i;
    unsigned int resourceAmount_o;
    int unusedEventResult_e;
    int unusedEventValue_o;
    mapCell *exitCell_d;
    int teleportDistance_o;
    int spell_g;
    unsigned int guardianCount_i;
    float attackerLoss_c;
    float defenderLoss_k;
    int levelExperience_g;
    int heroLevel_e;
    int creatureCosts_a[AI_EVENT_RESOURCE_COUNT];
    int spellPower_j;
    int adjacentMonster_j;
    int artifactResource_p;
    hero *otherHero_e;
    int artifactGuardResult_e;
    int heroCombatResult_h;
    int exitY_d;
    int artifact_g;
    int heroInteractionResult;
    int resourceType_a;
    int exitX;
    int exitCount;
    mapEventExtra *eventExtra_o;
    unsigned int artifactGuardCount_b;
    int survivingCount_a;
    boatRecord *boat_k;
    int savedShowIt_e;
    int mineId_j;
    int rewardWork_e[AI_EVENT_ARMY_STACK_COUNT];
    int resourceWork_p[AI_EVENT_RESOURCE_COUNT];
    int eventResults[3];
    int abandonedMineValue_f;
    int index_h;
    town *occupiedTown_b;
    int combatResult_d;
    int eventWork_o[AI_EVENT_RESOURCE_COUNT];
    int battleWon_j;
    int pyramidBattleValue_l;
    int oldPlayer_o;
    int eventType_g;
    playerData *oldPlayerData_h;
    int battleResult_l;
    int purchaseCost_i;
    int purchaseValue_a;
    int creatureType_i;
    int creatureFlag_l;

    occupiedTown_b = 0;
    eventType_g = cell->triggerType & MAP_EVENT_TYPE_MASK;
    eventResults[0] = 0;
    eventResults[1] = 0;
    oldPlayer_o = giCurPlayer;
    oldPlayerData_h = gpCurPlayer;

    if (eventHero->m_destinationX == x && eventHero->m_destinationY == y) {
        eventHero->m_destinationY = -1;
        eventHero->m_destinationX = eventHero->m_destinationY;
    }
    --eventHero->m_remainingMobility;
    switch (eventType_g) {
    case MAP_EVENT_COAST:
        if (eventHero->m_eventFlags & HERO_EVENT_EMBARKED) {
            eventHero->m_eventFlags &= ~HERO_EVENT_EMBARKED;
            eventHero->m_remainingMobility = 0;
            eventHero->m_direction = static_cast<unsigned char>(m_cursorDirection);
            m_cursorType = eventHero->m_cursorType;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            CheckAdjacentMon(&adjacentMonster_j);
        }
        break;

    case MAP_EVENT_BOAT:
        boat_k = &gpGame->m_boats[cell->w4hi];
        gpGame->RestoreCell(-1, -1, boat_k->savedTriggerType,
                            boat_k->savedEventData, cell,
                            AI_EVENT_BOAT_RESTORE_MODE);
        eventHero->m_eventFlags |= HERO_EVENT_EMBARKED;
        eventHero->m_remainingMobility = 0;
        boat_k->heroId = eventHero->m_id;
        boat_k->owner = eventHero->m_owner;
        m_cursorType = AI_EVENT_BOAT_CURSOR;
        m_cursorDirection = boat_k->direction;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorActive = 1;
        break;

    case MAP_EVENT_ALCHEMIST_LAB:
    case MAP_EVENT_MINE:
    case MAP_EVENT_SAWMILL:
        if (gpGame->m_mineOwners[cell->w4hi] != giCurPlayer) {
            if (gpGame->m_mines[cell->w4hi].guardianType != AI_EVENT_NO_CREATURE) {
                index_h = gpGame->m_mines[cell->w4hi].guardianCount;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero, gpGame->m_mines[cell->w4hi].guardianType,
                    &index_h, cell);
                if (combatResult_d != 0) {
                    gpGame->m_mines[cell->w4hi].guardianType =
                        AI_EVENT_NO_CREATURE;
                    gpGame->m_mines[cell->w4hi].guardianCount = 0;
                    eventHero->CheckLevel();
                } else {
                    break;
                }
            }
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
            if (eventType_g == MAP_EVENT_MINE) {
                for (index_h = AI_EVENT_MINE_SPELL_FIRST;
                     index_h < AI_EVENT_MINE_SPELL_LAST; ++index_h) {
                    if (eventHero->HasSpell(index_h) &&
                        GetManaCost(index_h, eventHero) < eventHero->m_spellPoints) {
                        eventHero->m_spellPoints = static_cast<short>(
                            eventHero->m_spellPoints -
                            GetManaCost(index_h, eventHero));
                        gpGame->m_mines[cell->w4hi].guardianType =
                            static_cast<signed char>(index_h + 1);
                        spellPower_j = eventHero->Stats(HERO_PRIMARY_SPELL_POWER);
                        if (spellPower_j > AI_EVENT_MINE_SPELL_POWER_MAX)
                            spellPower_j = AI_EVENT_MINE_SPELL_POWER_MAX;
                        gpGame->m_mines[cell->w4hi].guardianCount =
                            static_cast<unsigned char>(
                                spellPower_j * AI_EVENT_MINE_SPELL_COUNT_SCALE);
                        index_h = 999;
                    }
                }
            }
        }
        break;

    case MAP_EVENT_LIGHTHOUSE:
        if (gpGame->m_mineOwners[cell->w4hi] == giCurPlayer) {
        } else {
            gpGame->ClaimMine(cell->w4hi, giCurPlayer);
        }
        break;

    case MAP_EVENT_TREASURE_CHEST:
        if (cell->w4hi & CHEST_ARTIFACT_FLAG) {
            if (eventHero->NumArtifacts() >= AI_EVENT_ARTIFACT_LIMIT) {
                cell->w4hi = CHEST_GOLD_ONLY;
                goto chestGoldOrExperience;
            }
            GiveArtifact(eventHero, cell->w4hi & CHEST_ARTIFACT_MASK, 1, -1);
        } else {
chestGoldOrExperience:
            if (gpPhilAI->ChooseGoldOrExperience(
                    cell->w4hi * CHEST_GOLD_MULTIPLIER,
                    (cell->w4hi * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER)) {
                GiveResource(eventHero, RES_GOLD,
                             cell->w4hi * CHEST_GOLD_MULTIPLIER);
            } else {
                GiveExperience(eventHero,
                               (cell->w4hi * 4 - 4) *
                                   CHEST_EXPERIENCE_MULTIPLIER,
                               1);
                eventHero->CheckLevel();
            }
        }
        eventResults[0] = 1;
        break;

    case MAP_EVENT_WATERING_HOLE:
        if ((eventHero->m_eventFlags & HERO_EVENT_WATERING_HOLE) == 0) {
            eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero->m_eventFlags |= HERO_EVENT_WATERING_HOLE;
            ++eventHero->m_morale;
        }
        break;

    case MAP_EVENT_BUOY:
        if ((eventHero->m_eventFlags & HERO_EVENT_BUOY) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_BUOY;
            ++eventHero->m_morale;
        }
        break;

    case MAP_EVENT_FAERIE_RING:
        if ((eventHero->m_eventFlags & HERO_EVENT_FAERIE_RING) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_FAERIE_RING;
            ++eventHero->m_luck;
        }
        break;

    case MAP_EVENT_IDOL:
        if ((eventHero->m_eventFlags & HERO_EVENT_IDOL) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_IDOL;
            ++eventHero->m_luck;
        }
        break;

    case MAP_EVENT_FOUNTAIN:
        if ((eventHero->m_eventFlags & HERO_EVENT_FOUNTAIN) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_FOUNTAIN;
            ++eventHero->m_luck;
        }
        break;

    case MAP_EVENT_OASIS:
        if ((eventHero->m_eventFlags & HERO_EVENT_OASIS) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_OASIS;
            ++eventHero->m_morale;
            eventHero->m_mobility += OASIS_MOBILITY_BONUS;
            eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
        }
        break;

    case MAP_EVENT_TEMPLE:
        if ((eventHero->m_eventFlags & HERO_EVENT_TEMPLE) == 0) {
            eventHero->m_eventFlags |= HERO_EVENT_TEMPLE;
            eventHero->m_morale += 2;
        }
        break;

    case MAP_EVENT_SKELETON:
        switch (cell->w4hi) {
        case SKELETON_EMPTY:
            break;
        default:
            GiveArtifact(eventHero, cell->w4hi - SKELETON_ARTIFACT_OFFSET,
                         1, -1);
            cell->w4hi = SKELETON_EMPTY;
            break;
        }
        break;

    case MAP_EVENT_MAGIC_GARDEN:
        if (cell->w4hi != MAP_EVENT_DATA_EMPTY) {
            GiveResource(eventHero, cell->w4hi - MAP_EVENT_RESOURCE_OFFSET,
                         cell->w4hi - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                             ? MAP_EVENT_GOLD_AMOUNT
                             : MAP_EVENT_RESOURCE_AMOUNT);
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_EVENT_LEAN_TO:
        if (cell->w4hi != MAP_EVENT_DATA_EMPTY) {
            GiveResource(eventHero,
                         (cell->w4hi & ARTIFACT_MODE_MASK) - 1,
                         (cell->w4hi & ARTIFACT_RESOURCE_MASK) >>
                             ARTIFACT_RESOURCE_SHIFT);
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_EVENT_WAGON:
        if (cell->w4hi != MAP_EVENT_DATA_EMPTY) {
            if (cell->w4hi & WAGON_ARTIFACT_FLAG) {
                if (eventHero->NumArtifacts() != AI_EVENT_ARTIFACT_LIMIT)
                    GiveArtifact(eventHero,
                                 cell->w4hi & WAGON_ARTIFACT_MASK, 1, -1);
                cell->w4hi = MAP_EVENT_DATA_EMPTY;
            } else {
                GiveResource(eventHero,
                             (cell->w4hi & ARTIFACT_MODE_MASK) - 1,
                             (cell->w4hi & ARTIFACT_RESOURCE_MASK) >>
                                 ARTIFACT_RESOURCE_SHIFT);
                cell->w4hi = MAP_EVENT_DATA_EMPTY;
            }
        }
        break;

    case MAP_EVENT_SEA_CHEST:
        if ((cell->w4hi & CHEST_ARTIFACT_FLAG) != 0 &&
            eventHero->NumArtifacts() < AI_EVENT_ARTIFACT_LIMIT) {
            GiveArtifact(eventHero, cell->w4hi & CHEST_ARTIFACT_MASK, 1, -1);
            GiveResource(eventHero, RES_GOLD,
                         AI_EVENT_SEA_CHEST_ARTIFACT_GOLD);
        } else if (cell->w4hi != 0) {
            GiveResource(eventHero, RES_GOLD, AI_EVENT_SEA_CHEST_GOLD);
        }
        eventResults[0] = 1;
        break;

    case MAP_EVENT_FLOTSAM:
        switch (cell->w4hi) {
        case 0:
            break;
        case 1:
            GiveResource(eventHero, RES_WOOD, 5);
            break;
        case 2:
            GiveResource(eventHero, RES_WOOD, 5);
            GiveResource(eventHero, RES_GOLD, 200);
            break;
        case 3:
            GiveResource(eventHero, RES_WOOD, 10);
            GiveResource(eventHero, RES_GOLD, 500);
            break;
        }
        eventResults[0] = 1;
        break;

    case MAP_EVENT_CAMPFIRE:
        GiveResource(eventHero, RES_GOLD,
                     (cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT) *
                         CAMPFIRE_GOLD_MULTIPLIER);
        GiveResource(eventHero, cell->w4hi & CAMPFIRE_RESOURCE_MASK,
                     cell->w4hi >> CAMPFIRE_AMOUNT_SHIFT);
        eventResults[0] = 1;
        break;

    case MAP_EVENT_FORT:
        if ((eventHero->m_fortVisits & (1U << cell->w4hi)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE];
            eventHero->m_fortVisits |= 1U << cell->w4hi;
        }
        break;

    case MAP_EVENT_XANADU:
        if ((eventHero->m_xanaduVisits & (1U << cell->w4hi)) == 0 &&
            eventHero->m_level +
                    eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] * 2 >=
                XANADU_ADMISSION_LEVEL) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_ATTACK];
            ++eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE];
            ++eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE];
            ++eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER];
            eventHero->m_xanaduVisits |= 1U << cell->w4hi;
        }
        break;

    case MAP_EVENT_STANDING_STONES:
        if ((eventHero->m_standingStoneVisits & (1U << cell->w4hi)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER];
            eventHero->m_standingStoneVisits |= 1U << cell->w4hi;
        }
        break;

    case MAP_EVENT_WITCH_DOCTOR_HUT:
        if ((eventHero->m_witchDoctorVisits & (1U << cell->w4hi)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE];
            eventHero->m_witchDoctorVisits |= 1U << cell->w4hi;
        }
        break;

    case MAP_EVENT_MERCENARY_CAMP:
        if ((eventHero->m_mercenaryCampVisits & (1U << cell->w4hi)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_ATTACK];
            eventHero->m_mercenaryCampVisits |= 1U << cell->w4hi;
        }
        break;

    case MAP_EVENT_GAZEBO:
        if ((eventHero->m_gazeboVisits & (1U << cell->w4hi)) == 0) {
            GiveExperience(eventHero, GAZEBO_EXPERIENCE, 1);
            eventHero->m_gazeboVisits |= 1U << cell->w4hi;
            eventHero->CheckLevel();
        }
        break;

    case MAP_EVENT_WATER_WHEEL:
        if (cell->w4hi != 0) {
            GiveResource(eventHero, RES_GOLD,
                         cell->w4hi * CHEST_GOLD_MULTIPLIER);
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_EVENT_RESOURCE:
        resourceType_a = cell->objIndex >> 1;
        resourceAmount_o = resourceType_a == RES_GOLD
            ? cell->w4hi * CAMPFIRE_GOLD_MULTIPLIER
            : cell->w4hi;
        GiveResource(eventHero, resourceType_a, resourceAmount_o);
        eventResults[0] = 1;
        break;

    case MAP_EVENT_WINDMILL:
        if (cell->w4hi != AI_EVENT_WINDMILL_EMPTY) {
            GiveResource(eventHero, cell->w4hi, WINDMILL_RESOURCE_AMOUNT);
            cell->w4hi = AI_EVENT_WINDMILL_EMPTY;
        }
        break;

    case MAP_EVENT_HILL_FORT:
        eventHero->UpgradeCreatures(HILL_FORT_OGRE, HILL_FORT_OGRE_LORD);
        eventHero->UpgradeCreatures(HILL_FORT_ORC, HILL_FORT_ORC_CHIEF);
        eventHero->UpgradeCreatures(HILL_FORT_DWARF,
                                    HILL_FORT_BATTLE_DWARF);
        break;

    case MAP_EVENT_FREEMANS_FOUNDRY:
        eventHero->UpgradeCreatures(FOUNDRY_IRON_GOLEM,
                                    FOUNDRY_STEEL_GOLEM);
        eventHero->UpgradeCreatures(FOUNDRY_PIKEMAN,
                                    FOUNDRY_VETERAN_PIKEMAN);
        eventHero->UpgradeCreatures(FOUNDRY_SWORDSMAN,
                                    FOUNDRY_MASTER_SWORDSMAN);
        break;

    case MAP_EVENT_TREE_CITY:
        creatureType_i = EVENT_RECRUIT_SPRITE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_RUINS:
        creatureType_i = EVENT_RECRUIT_MEDUSA;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_TROLL_BRIDGE:
        if (cell->w4hi & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = TROLL_BRIDGE_TROLL;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_CITY_OF_DEAD:
        if (cell->w4hi & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = CITY_DEAD_RECRUIT;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_DRAGON_CITY:
        if (cell->w4hi & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = DRAGON_CITY_RECRUIT;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_HALFLING_HOLE:
        creatureType_i = AI_CREATURE_HALFLING;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_ANCIENT_LAMP:
        creatureType_i = EVENT_RECRUIT_GENIE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_WAGON_CAMP:
        creatureType_i = EVENT_RECRUIT_ROGUE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_DESERT_TENT:
        creatureType_i = EVENT_RECRUIT_NOMAD;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_EVENT_WATCH_TOWER:
        creatureType_i = AI_CREATURE_ORC;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_TREE_HOUSE:
        creatureType_i = EVENT_RECRUIT_SPRITE;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_ARCHER_HOUSE:
        creatureType_i = AI_CREATURE_ARCHER;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_GOBLIN_HUT:
        creatureType_i = AI_CREATURE_GOBLIN;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_PEASANT_HUT:
        creatureType_i = AI_CREATURE_PEASANT;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_DWARF_COTTAGE:
    case MAP_EVENT_SIRENS:
        creatureType_i = AI_CREATURE_DWARF;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_CAVE:
        creatureType_i = AI_CREATURE_CENTAUR;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_EVENT_EXCAVATION:
        creatureType_i = AI_CREATURE_SKELETON;
        creatureFlag_l = 1;
creaturePurchase:
        if (cell->w4hi != 0) {
            gpPhilAI->EvaluateOneTimeCreaturePurchase(
                creatureType_i, cell->w4hi, creatureFlag_l, eventResults[2],
                purchaseCost_i, purchaseValue_a);
            if (eventResults[2] > 0) {
                gpGame->GiveArmy(&eventHero->m_army, creatureType_i,
                                 eventResults[2], purchaseValue_a);
                cell->m_objectData = static_cast<unsigned short>(
                    (cell->w4hi - eventResults[2]) * 8 |
                    (cell->m_objectData & 7));
                if (creatureFlag_l == 0) {
                    GetMonsterCost(creatureType_i, creatureCosts_a);
                    for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h)
                        gpCurPlayer->m_resources[index_h] -=
                            creatureCosts_a[index_h] * eventResults[2];
                }
            }
        }
        if (cell->w4hi == 0 && eventType_g == MAP_EVENT_ANCIENT_LAMP)
            eventResults[0] = 1;
        break;

    case MAP_EVENT_MONSTER:
        ComputerMonsterInteract(cell, eventHero, eventResults);
        break;

    case MAP_EVENT_TREE_OF_KNOWLEDGE:
        if ((eventHero->m_treeKnowledgeVisits &
             (1U << (cell->w4hi & 31))) == 0) {
            heroLevel_e = eventHero->GetLevel(eventHero->m_experience);
            levelExperience_g = eventHero->GetExperience(heroLevel_e + 1) -
                              eventHero->GetExperience(heroLevel_e);
            switch (cell->w4hi >> TREE_KNOWLEDGE_MODE_SHIFT) {
            case TREE_KNOWLEDGE_FREE:
                GiveExperience(eventHero, levelExperience_g, 1);
                eventHero->m_treeKnowledgeVisits |=
                    1U << (cell->w4hi & 31);
                eventHero->CheckLevel();
                break;
            case TREE_KNOWLEDGE_GOLD:
                if (gpCurPlayer->m_resources[RES_GOLD] >
                    AI_EVENT_TREE_GOLD) {
                    gpCurPlayer->m_resources[RES_GOLD] -= AI_EVENT_TREE_GOLD;
                    GiveExperience(eventHero, levelExperience_g, 1);
                    eventHero->m_treeKnowledgeVisits |=
                        1U << (cell->w4hi & 31);
                    eventHero->CheckLevel();
                }
                break;
            case TREE_KNOWLEDGE_GEMS:
                if (gpCurPlayer->m_resources[RES_GEMS] >
                    AI_EVENT_TREE_GEMS) {
                    gpCurPlayer->m_resources[RES_GEMS] -= AI_EVENT_TREE_GEMS;
                    GiveExperience(eventHero, levelExperience_g, 1);
                    eventHero->m_treeKnowledgeVisits |=
                        1U << (cell->w4hi & 31);
                    eventHero->CheckLevel();
                }
                break;
            }
        }
        break;

    case MAP_EVENT_OBELISK:
        if ((giCurPlayerBit &
             gpGame->m_obeliskVisitors[cell->w4hi - 1]) == 0) {
            gpGame->m_obeliskVisitors[cell->w4hi - 1] |= giCurPlayerBit;
            ComputeUALoc(giCurPlayer);
        }
        break;

    case MAP_EVENT_ORACLE:
        break;

    case MAP_EVENT_SHRINE_FIRST_CIRCLE:
    case MAP_EVENT_SHRINE_SECOND_CIRCLE:
    case MAP_EVENT_SHRINE_THIRD_CIRCLE:
        if (eventHero->HasArtifact(AI_EVENT_MAGIC_BOOK) &&
            gsSpellInfo[cell->w4hi - 1].level <=
                eventHero->m_secondarySkills[HERO_SKILL_WISDOM] + 2) {
            eventHero->AddSpell(cell->w4hi - 1,
                                eventHero->Stats(HERO_PRIMARY_KNOWLEDGE));
        }
        break;

    case MAP_EVENT_CASTLE:
        gpPhilAI->TownEvent(cell, eventHero, x, y);
        break;

    case MAP_EVENT_WHIRLPOOL:
        DoWhirlpool(eventHero);
        goto teleportEvent;
    case MAP_EVENT_STONE_LITHS:
teleportEvent:
        exitCount = 0;
        for (exitY_d = 0; MAP_HEIGHT > exitY_d; ++exitY_d) {
            for (exitX = 0; MAP_WIDTH > exitX; ++exitX) {
                if (gpGame->m_worldMap.Row(exitY_d)[exitX].triggerType ==
                        static_cast<unsigned char>(eventType_g |
                                                   MAP_EVENT_ACTION_FLAG) &&
                    gpGame->m_worldMap.Row(exitY_d)[exitX].objIndex ==
                        cell->objIndex &&
                    abs(exitY_d - y) + abs(exitX - x) >
                        (eventType_g == MAP_EVENT_STONE_LITHS
                             ? AI_EVENT_TELEPORT_STONE_DISTANCE
                             : AI_EVENT_TELEPORT_WHIRLPOOL_DISTANCE)) {
                    ++exitCount;
                }
            }
        }
        if (exitCount > 0) {
            if (exitCount > 1)
                exitCount = Random(1, exitCount);
            for (exitY_d = 0; MAP_HEIGHT > exitY_d; ++exitY_d) {
                for (exitX = 0; MAP_WIDTH > exitX; ++exitX) {
                    if (gpGame->m_worldMap.Row(exitY_d)[exitX].triggerType ==
                            static_cast<unsigned char>(eventType_g |
                                                       MAP_EVENT_ACTION_FLAG) &&
                        gpGame->m_worldMap.Row(exitY_d)[exitX].objIndex ==
                            cell->objIndex &&
                        abs(exitY_d - y) + abs(exitX - x) >
                            (eventType_g == MAP_EVENT_STONE_LITHS
                                 ? AI_EVENT_TELEPORT_STONE_DISTANCE
                                 : AI_EVENT_TELEPORT_WHIRLPOOL_DISTANCE) &&
                        --exitCount < 1) {
                        goto teleportDestination;
                    }
                }
            }
teleportDestination:
            StopCursor(1);
            gpAdvManager->TeleportTo(eventHero, exitX, exitY_d, 0, 0);
        }
        break;

    case MAP_EVENT_ARTIFACT:
        artifactResource_p = (cell->w4hi & ARTIFACT_RESOURCE_MASK) >>
                           ARTIFACT_RESOURCE_SHIFT;
        artifact_g = cell->objIndex >> 1;
        artifactGuardCount_b = cell->w4hi & ARTIFACT_MONSTER_MASK;
        if (eventHero->NumArtifacts() == AI_EVENT_ARTIFACT_LIMIT)
            break;
        if (artifact_g == AI_EVENT_SPELL_SCROLL) {
            GiveArtifact(eventHero, artifact_g, 1,
                         static_cast<signed char>(cell->w4hi));
            eventResults[0] = 1;
            break;
        }
        if (cell->w4hi & ARTIFACT_GUARDED_FLAG) {
            if (artifactGuardCount_b == AI_EVENT_ROGUE_GUARD) {
                artifactGuardResult_e = AI_EVENT_ROGUE_COUNT;
            } else {
                artifactGuardResult_e = 1;
                if (gpPhilAI->ChooseToFightForArtifact(
                        artifact_g, artifactGuardCount_b, 1) == 0)
                    break;
            }
            if (gpPhilAI->CombatMonsterEvent(
                    eventHero, artifactGuardCount_b, &artifactGuardResult_e,
                    cell) == 0)
                break;
            goto artifactPickup;
        }
        switch (cell->w4hi & ARTIFACT_MODE_MASK) {
        case ARTIFACT_MODE_PICKUP:
artifactPickup:
            for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h) {
                if (gpCurPlayer->m_resources[index_h] < 0)
                    gpCurPlayer->m_resources[index_h] = 0;
            }
            GiveArtifact(eventHero, artifact_g, 1, -1);
            eventResults[0] = 1;
            break;
        case ARTIFACT_MODE_GOLD:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_GOLD, 0, 0)) {
                gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_GOLD;
                goto artifactPickup;
            }
            break;
        case ARTIFACT_MODE_WISDOM:
            if (eventHero->m_secondarySkills[HERO_SKILL_WISDOM] != 0)
                goto artifactPickup;
            break;
        case ARTIFACT_MODE_LEADERSHIP:
            if (eventHero->m_secondarySkills[HERO_SKILL_LEADERSHIP] != 0)
                goto artifactPickup;
            break;
        case ARTIFACT_MODE_RESOURCE_3:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_RESOURCE_3_GOLD,
                    artifactResource_p, AI_EVENT_ARTIFACT_RESOURCE_3)) {
                gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_RESOURCE_3_GOLD;
                gpGame->m_players[eventHero->m_owner]
                    .resources[artifactResource_p] -=
                    AI_EVENT_ARTIFACT_RESOURCE_3;
                goto artifactPickup;
            }
            break;
        case ARTIFACT_MODE_RESOURCE_5:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_RESOURCE_5_GOLD,
                    artifactResource_p, AI_EVENT_ARTIFACT_RESOURCE_5)) {
                gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_RESOURCE_5_GOLD;
                gpGame->m_players[eventHero->m_owner]
                    .resources[artifactResource_p] -=
                    AI_EVENT_ARTIFACT_RESOURCE_5;
                goto artifactPickup;
            }
            break;
        }
        break;

    case MAP_EVENT_HERO_INTERACTION:
        otherHero_e = &gpGame->m_heroRecs[cell->w4hi];
        savedShowIt_e = bShowIt;
        if (otherHero_e->m_owner == giCurPlayer) {
            gpPhilAI->HeroInteractionAtHero(eventHero, otherHero_e, 0,
                                            &heroInteractionResult);
            return;
        }
        if (otherHero_e->m_locationType == AI_EVENT_HERO_TOWN_LOCATION)
            occupiedTown_b = reinterpret_cast<town *>(
                &gpGame->m_castleRecs[otherHero_e->m_occupiedTown]);

        if (gbHumanPlayer[otherHero_e->m_owner] == 0) {
            combatResult_d = gpPhilAI->QuickCombat(
                &eventHero->m_army, eventHero, &otherHero_e->m_army, otherHero_e,
                0, 0, attackerLoss_c, defenderLoss_k);
            if (combatResult_d != 0 && occupiedTown_b != 0) {
                combatResult_d = gpPhilAI->QuickCombat(
                    &eventHero->m_army, eventHero, &occupiedTown_b->m_army, 0,
                    1, occupiedTown_b->m_id, attackerLoss_c, defenderLoss_k);
            }
        } else {
            if (occupiedTown_b != 0)
                occupiedTown_b->m_occupyingHeroId = otherHero_e->m_id;
            heroCombatResult_h = DoCombat(
                x, y, eventHero, &eventHero->m_army, occupiedTown_b, otherHero_e,
                &otherHero_e->m_army, x, y, -1, 1);
            if (heroCombatResult_h == 0 && occupiedTown_b != 0)
                gpGame->ClaimTown(occupiedTown_b->m_id, giCurPlayer, 0);
        }
        CompleteDraw(0);
        break;

    case MAP_EVENT_SIGN:
    case MAP_EVENT_BOTTLE:
        break;

    case MAP_EVENT_DAEMON_CAVE:
        switch (cell->w4hi) {
        case DAEMON_CAVE_EMPTY:
            break;
        case DAEMON_REWARD_EXPERIENCE:
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
            break;
        case DAEMON_REWARD_ARTIFACT:
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
            GiveRandomArtifact(eventHero);
            break;
        case DAEMON_REWARD_EXPERIENCE_GOLD:
            GiveExperience(eventHero, DAEMON_EXPERIENCE, 1);
            GiveResource(eventHero, RES_GOLD, AI_EVENT_DAEMON_GOLD);
            break;
        case DAEMON_REWARD_RANSOM:
            if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] >=
                AI_EVENT_DAEMON_GOLD) {
                if (gpPhilAI->ChooseToPayRansomOnHero(AI_EVENT_DAEMON_GOLD)) {
                    gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -=
                        AI_EVENT_DAEMON_GOLD;
                } else {
                    HeroLoses(eventHero);
                }
            } else {
                HeroLoses(eventHero);
            }
            break;
        }
        cell->w4hi = DAEMON_CAVE_EMPTY;
        break;

    case MAP_EVENT_PYRAMID:
        if (cell->w4hi != 0 && eventHero->HasSpell(cell->w4hi - 1) == 0) {
            for (index_h = 0; index_h < AI_EVENT_ARMY_STACK_COUNT; ++index_h) {
                gpMonGroup->m_creatureTypes[index_h] =
                    PYRAMID_PRIMARY_MONSTER;
                gpMonGroup->m_creatureCounts[index_h] = 10;
            }
            index_h = cell->w4hi - 1;
            if (gsSpellInfo[index_h].m_e & 1) {
                battleStatValue_o =
                    eventHero->Stats(HERO_PRIMARY_SPELL_POWER) > 40
                        ? gfBattleStat[40]
                        : gfBattleStat[eventHero->Stats(
                              HERO_PRIMARY_SPELL_POWER)];
                spellValueFactor_i = battleStatValue_o;
            } else {
                spellValueFactor_i = 1.0f;
            }
            pyramidBattleValue_l = static_cast<int>(
                gsSpellInfo[index_h].aiValue *
                gpCurPlayer->m_aiSpellValueMultiplier * spellValueFactor_i);
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army, eventHero, gpMonGroup, 0, 0, 0,
                pyramidBattleValue_l, battleWon_j, battleResult_l);
            if (battleWon_j == 0) {
            } else {
                index_h = PYRAMID_GUARD_COUNT;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero, PYRAMID_PRIMARY_MONSTER,
                    &index_h, cell);
                if (combatResult_d == 0) {
                } else {
                    eventHero->AddSpell(
                        cell->w4hi - 1,
                        eventHero->Stats(HERO_PRIMARY_KNOWLEDGE));
                    cell->w4hi = MAP_EVENT_DATA_EMPTY;
                }
            }
        }
        break;

    case MAP_EVENT_GRAVEYARD:
    case MAP_EVENT_SHIPWRECK:
    case MAP_EVENT_DERELICT_SHIP:
        gpPhilAI->FightEvent(eventHero, cell, 0);
        break;

    case MAP_EVENT_ABANDONED_MINE:
        for (index_h = 0; index_h < AI_EVENT_ARMY_STACK_COUNT; ++index_h) {
            gpMonGroup->m_creatureTypes[index_h] =
                gpGame->m_mines[cell->w4hi].guardianType;
            gpMonGroup->m_creatureCounts[index_h] = static_cast<short>(
                gpGame->m_mines[cell->w4hi].guardianCount /
                AI_EVENT_ABANDONED_MINE_ARMY_DIVISOR);
        }
        abandonedMineValue_f = static_cast<int>(
            static_cast<float>(gaiTurnValueOfMine[y * MAP_WIDTH + x]) *
            gMineCharacteristics[RES_GOLD] *
            gafAITurnCostResource[RES_GOLD]);
        gpPhilAI->ChooseEvaluateBattle(
            &eventHero->m_army, eventHero, gpMonGroup, 0, 0, 0,
            abandonedMineValue_f, battleWon_j, battleResult_l);
        if (battleWon_j != 0) {
            survivingCount_a = gpGame->m_mines[cell->w4hi].guardianCount;
            combatResult_d = gpPhilAI->CombatMonsterEvent(
                eventHero, gpGame->m_mines[cell->w4hi].guardianType,
                &survivingCount_a, cell);
            if (survivingCount_a > AI_EVENT_GUARD_COUNT_MAX)
                survivingCount_a = AI_EVENT_GUARD_COUNT_MAX;
            gpGame->m_mines[cell->w4hi].guardianCount =
                static_cast<unsigned char>(survivingCount_a);
            if (combatResult_d == 0) {
            } else {
                eventHero->CheckLevel();
                gpGame->ConvertObject(x - 2, y - 1, x + 1, y - 1,
                                      56, 0, 4, 26, 104, 64, 23);
                gpGame->ConvertObject(x - 2, y, x + 1, y,
                                      56, 5, 9, 26, 109, 64, 23);
                gpGame->ConvertObject(x - 2, y - 1, x + 1, y - 1,
                                      51, 0, 3, 32, 75, 64, 23);
                gpGame->ConvertObject(x - 2, y, x + 1, y,
                                      51, 4, 7, 32, 80, 64, 23);
                gpGame->ConvertObject(x, y, x, y,
                                      29, 5, 5, 29, 4, 64, 23);
                gpGame->m_mines[cell->w4hi].resourceType = RES_GOLD;
                gpGame->m_mines[cell->w4hi].guardianType = AI_EVENT_NO_CREATURE;
                gpGame->m_mines[cell->w4hi].guardianCount = 0;
                gpGame->ClaimMine(cell->w4hi, giCurPlayer);
            }
        }
        break;

    case MAP_EVENT_TAR_PIT:
        break;

    case MAP_EVENT_OBSERVATION_TOWER:
        gpGame->SetVisibility(x, y, giCurPlayer,
                              AI_EVENT_OBSERVATION_RADIUS);
        break;

    case MAP_EVENT_SHIPWRECK_SURVIVOR:
        if (eventHero->NumArtifacts() < AI_EVENT_ARTIFACT_LIMIT)
            GiveArtifact(eventHero, cell->w4hi, 1, -1);
        eventResults[0] = 1;
        break;

    case MAP_EVENT_ARTESIAN_SPRING:
        if (cell->w4hi != 0) {
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
            spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
            if (eventHero->m_spellPoints < spellPower_j * 20)
                eventHero->m_spellPoints = static_cast<short>(spellPower_j * 20);
        }
        break;

    case MAP_EVENT_MAGIC_WELL:
        if ((eventHero->m_eventFlags & HERO_EVENT_MAGIC_WELL) == 0) {
            cell->w4hi = MAP_EVENT_DATA_EMPTY;
            spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
            if (eventHero->m_spellPoints < spellPower_j * 10) {
                eventHero->m_eventFlags |= HERO_EVENT_MAGIC_WELL;
                eventHero->m_spellPoints = static_cast<short>(spellPower_j * 10);
            }
        }
        break;

    case MAP_EVENT_WITCH_HUT:
        if (eventHero->m_secondarySkills[cell->w4hi] == 0)
            eventHero->GiveSS(cell->w4hi, HERO_SKILL_LEVEL_BASIC);
        break;

    case MAP_EVENT_MAGELLAN_MAPS:
        break;

    case MAP_EVENT_SPHINX:
        eventExtra_o = reinterpret_cast<mapEventExtra *>(ppMapExtra[cell->w4hi]);
        if (eventExtra_o->active != 0) {
            if (Random(0, AI_EVENT_RANDOM_PERCENT_MAX) <
                AI_EVENT_RANDOM_EVENT_SUCCESS) {
                for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h) {
                    gpGame->m_players[giCurPlayer].resources[index_h] +=
                        eventExtra_o->resources[index_h];
                    if (gpGame->m_players[giCurPlayer].resources[index_h] < 0)
                        gpGame->m_players[giCurPlayer].resources[index_h] = 0;
                }
                if (eventExtra_o->artifact != -1 &&
                    eventHero->NumArtifacts() < AI_EVENT_ARTIFACT_LIMIT) {
                    GiveArtifact(eventHero, eventExtra_o->artifact, 1, -1);
                }
                eventExtra_o->active = 0;
            } else {
                HeroLoses(eventHero);
            }
        }
        break;

    case MAP_EVENT_TRADING_POST:
        break;

    case MAP_EVENT_BARRIER:
        eventResults[0] = BarrierAIEvent(cell, eventHero);
        break;
    case MAP_EVENT_TRAVELER_TENT:
        PasswordAIEvent(cell, eventHero);
        break;
    case MAP_EVENT_EXPANSION_OBJECT:
        GenericSiteAIEvent(cell, eventHero);
        break;
    case MAP_EVENT_EXPANSION_DWELLING:
        RecruitSiteAIEvent(cell, eventHero);
        break;
    case MAP_EVENT_JAIL:
        JailAIEvent(cell, eventHero, x, y);
        break;

    default:
        break;
    }

    if (eventResults[0] != 0)
        EraseObj(cell, x, y);
    giCurPlayer = oldPlayer_o;
    gpCurPlayer = oldPlayerData_h;
    if (eventHero->m_owner != AI_EVENT_NO_OWNER)
        eventHero->CheckLevel();
    CheckEndGame(0, 0);
}

VA(0x004b4883, 0x65)
int advManager::BarrierAIEvent(mapCell *cell, hero *)
{
    int unusedBarrier15[5];
    int color = cell->w4hi;
    color &= AI_EVENT_BARRIER_COLOR_MASK;
    if (gpCurPlayer->m_barrierTents & (1 << color))
        return 1;
    else
        return 0;
    return 0;
}

VA(0x004b48e8, 0x53)
void advManager::PasswordAIEvent(mapCell *cell, hero *)
{
    int unusedPassword6[1];
    int color = cell->w4hi;
    color &= AI_EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

VA(0x004b493b, 0x369)
// @early-stop: all 0x369 code bytes match after masking 25 relocations. The
// residual is the two switch tables' delinked local-label identities plus the
// graveyard multiplier/monster-table folded symbol identities.
void advManager::GenericSiteAIEvent(mapCell *cell, hero *eventHero)
{
    int unusedPair27[2];
    int artifactIndex1;
    int siteLevel5;
    int primaryStat16;
    int siteType3;
    int unusedTriple26[3];
    int cursedArtifactCount5;
    int creatureType3;
    int quantity6;
    int armyValue7;

    cursedArtifactCount5 = 0;
    siteType3 = cell->w4hi;
    siteType3 &= AI_EVENT_SITE_TYPE_MASK;
    siteLevel5 = cell->w4hi;
    siteLevel5 >>= AI_EVENT_SITE_LEVEL_SHIFT;

    switch (siteType3) {
    case AI_GENERIC_SITE_CURSED_ARTIFACTS:
        for (artifactIndex1 = 0; artifactIndex1 < AI_EVENT_ARTIFACT_LIMIT;
             artifactIndex1++) {
            if (IsCursedItem(eventHero->m_artifacts[artifactIndex1]))
                cursedArtifactCount5++;
        }
        if (cursedArtifactCount5 != 0 &&
            gpCurPlayer->m_resources[RES_GOLD] >=
                AI_EVENT_CURSED_ARTIFACT_GOLD_THRESHOLD) {
            for (artifactIndex1 = 0; artifactIndex1 < AI_EVENT_ARTIFACT_LIMIT;
                 artifactIndex1++) {
                if (IsCursedItem(eventHero->m_artifacts[artifactIndex1]))
                    eventHero->m_artifacts[artifactIndex1] = -1;
            }
            gpCurPlayer->m_resources[RES_GOLD] -= AI_EVENT_CURSED_ARTIFACT_COST;
        }
        break;
    case AI_GENERIC_SITE_SHIPWRECK:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_SHIPWRECK_FLAG)) {
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_SHIPWRECK_FLAG;
            switch (eventHero->m_cursorType) {
            case HERO_CLASS_SORCERESS:
            case HERO_CLASS_WARLOCK:
            case HERO_CLASS_WIZARD:
            case HERO_CLASS_NECROMANCER:
                primaryStat16 = HERO_PRIMARY_SPELL_POWER;
                break;
            case HERO_CLASS_BARBARIAN:
                primaryStat16 = HERO_PRIMARY_ATTACK;
                break;
            case HERO_CLASS_KNIGHT:
                primaryStat16 = HERO_PRIMARY_DEFENSE;
                break;
            default:
                primaryStat16 = HERO_PRIMARY_ATTACK;
            }
            eventHero->m_primaryStats[primaryStat16]++;
        }
        break;
    case AI_GENERIC_SITE_FAERIE_RING:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_FAERIE_RING_FLAG)) {
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_FAERIE_RING_FLAG;
            eventHero->m_luck = eventHero->m_luck + 1;
        }
        break;
    case AI_GENERIC_SITE_UNUSED_2:
    case AI_GENERIC_SITE_UNUSED_3:
        break;
    case AI_GENERIC_SITE_GRAVEYARD:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_GRAVEYARD_FLAG)) {
            armyValue7 = 0;
            for (artifactIndex1 = 0;
                 artifactIndex1 < AI_EVENT_ARMY_STACK_COUNT;
                 artifactIndex1++) {
                creatureType3 =
                    eventHero->m_army.m_creatureTypes[artifactIndex1];
                if (creatureType3 != AI_EVENT_NO_CREATURE) {
                    quantity6 = eventHero->m_army.m_quantities[artifactIndex1];
                    if (quantity6 > AI_EVENT_GRAVEYARD_ARMY_REMAINDER) {
                        eventHero->m_army.m_quantities[artifactIndex1] =
                            static_cast<short>(
                                quantity6 * AI_GENERIC_SITE_GRAVEYARD_REMAINING);
                        armyValue7 +=
                            (quantity6 -
                             eventHero->m_army.m_quantities[artifactIndex1]) *
                            gMonsterDatabase[creatureType3].hitPoints;
                    }
                }
            }
            if (armyValue7 != 0)
                GiveExperience(eventHero, armyValue7, 1);
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_GRAVEYARD_FLAG;
        }
        break;
    case AI_GENERIC_SITE_CREATURE_UPGRADE:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG)) {
            eventHero->m_eventFlags =
                static_cast<int>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG;
            eventHero->m_mobility += AI_EVENT_CREATURE_UPGRADE_MOBILITY;
            eventHero->m_remainingMobility += AI_EVENT_CREATURE_UPGRADE_MOBILITY;
        }
        if (eventHero->CreatureTypeCount(AI_EVENT_CREATURE_UPGRADE_FROM))
            eventHero->UpgradeCreatures(AI_EVENT_CREATURE_UPGRADE_FROM,
                                        AI_EVENT_CREATURE_UPGRADE_TO);
        break;
    }
}

VA(0x004b4ca4, 0x18f)
// @early-stop: complete semantics, frame, slots, CFG, and relocation targets.
// The switch table differs only by delinked local-label identity; the only
// opcode residual is the packed w4hi assignment's equivalent operand order.
void advManager::RecruitSiteAIEvent(mapCell *cell, hero *eventHero)
{
    int cost16[AI_EVENT_RESOURCE_COUNT];
    int purchaseCount17;
    unsigned int packedSite17;
    short availableCount1;
    unsigned int siteType3;
    int creatureType13;
    int resourceIndex27;
    int purchaseValue5;
    int replacementSlot26;

    siteType3 = cell->w4hi;
    siteType3 &= AI_EVENT_RECRUIT_TYPE_MASK;
    availableCount1 = static_cast<short>(cell->w4hi);
    availableCount1 >>= AI_EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType3) {
    case 0:
        creatureType13 = MONSTER_GENIE;
        break;
    case 1:
        creatureType13 = MONSTER_EARTH_ELEMENTAL;
        break;
    case 2:
        creatureType13 = MONSTER_AIR_ELEMENTAL;
        break;
    case 3:
        creatureType13 = MONSTER_FIRE_ELEMENTAL;
        break;
    case 4:
        creatureType13 = MONSTER_WATER_ELEMENTAL;
        break;
    }

    if (availableCount1 != 0) {
        gpPhilAI->EvaluateOneTimeCreaturePurchase(
            creatureType13, availableCount1, 0, purchaseCount17, purchaseValue5,
            replacementSlot26);
        if (purchaseCount17 > 0) {
            gpGame->GiveArmy(&eventHero->m_army, creatureType13, purchaseCount17,
                             replacementSlot26);
            availableCount1 =
                static_cast<short>(availableCount1 - purchaseCount17);
            packedSite17 =
                (availableCount1 << AI_EVENT_RECRUIT_COUNT_SHIFT) | siteType3;
            cell->w4hi = packedSite17 | 0;
            GetMonsterCost(creatureType13, cost16);
            for (resourceIndex27 = 0;
                 resourceIndex27 < AI_EVENT_RESOURCE_COUNT;
                 resourceIndex27++) {
                gpCurPlayer->m_resources[resourceIndex27] -=
                    cost16[resourceIndex27] * purchaseCount17;
            }
        }
    }
}

VA(0x004b4e33, 0x1a2)
// @early-stop: complete semantics, frame, slots, CFG, and relocation targets.
// The only opcode residual is the final w4hi assignment's equivalent operand
// evaluation order; two earlier exits also target the identical final five-byte
// continuation rather than its epilogue destination.
void advManager::JailAIEvent(mapCell *cell, hero *eventHero, int x, int y)
{
    int heroId9;
    hero *releasedHero13;

    heroId9 = cell->w4hi;
    if (gpGame->m_availableHeroes[heroId9] != AI_EVENT_JAILED_HERO) {
        EraseObj(cell, x, y);
    } else {
        if (gpCurPlayer->m_heroCount >= AI_EVENT_HERO_LIMIT) {
        } else {
            gpGame->m_heroRecs[heroId9].m_owner = eventHero->m_owner;
            gpGame->m_availableHeroes[heroId9] = eventHero->m_owner;
            releasedHero13 = &gpGame->m_heroRecs[heroId9];
            EraseObj(cell, x, y);
            gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
                static_cast<signed char>(heroId9);
            gpCurPlayer->m_heroCount++;
            releasedHero13->m_x = x;
            releasedHero13->m_y = y;
            releasedHero13->m_eventFlags = 0;
            releasedHero13->m_direction = AI_EVENT_HERO_DIRECTION;
            releasedHero13->m_remainingMobility = releasedHero13->CalcMobility();
            releasedHero13->m_mobility = releasedHero13->m_remainingMobility;
            releasedHero13->m_locationType = cell->triggerType;
            releasedHero13->m_occupiedTown = cell->w4hi;
            cell->triggerType = MAP_EVENT_ACTION_FLAG | MAP_EVENT_HERO_INTERACTION;
            cell->w4hi = heroId9;
            SendMapChange(AI_EVENT_HERO_MAP_CHANGE,
                          static_cast<signed char>(heroId9), x, y,
                          AI_EVENT_HERO_MAP_CHANGE_VALUE, 0, 0);
        }
    }
}

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
// @early-stop: complete semantics, 0x40-byte frame, slots, CFG, and all 36
// relocation sites are recovered. Retail is 25 bytes larger solely because it
// retains five five-byte continuation jumps; the remaining relocation names
// are delinked aliases of the same monster fields and floating constants.
void advManager::ComputerMonsterInteract(mapCell *cell, hero *eventHero,
                                         int *handled)
{
    int joiningCount;
    int purchaseCount;
    int monsterCount[2];
    float strengthRatio;
    int purchaseValue;
    int replacementSlot;
    unsigned int forcedJoin;
    int monsterType;

    monsterType = cell->objIndex;
    monsterCount[0] = cell->w4hi & MONSTER_COUNT_MASK;
    forcedJoin = cell->w4hi & MONSTER_JOIN_FORCED;
    strengthRatio = static_cast<float>(gpPhilAI->FightValueOfStack(
                        &eventHero->m_army, eventHero, 0, 0, 0, 0)) /
                    static_cast<float>(gMonsterDatabase[monsterType].fightValue *
                                       monsterCount[0]);

    if (eventHero->m_army.CanJoin(monsterType) &&
        !eventHero->HasArtifact(MONSTER_NO_JOIN_ARTIFACT) &&
        strengthRatio > MONSTER_STRENGTH_JOIN &&
        monsterType != MONSTER_GENIE &&
        monsterType != MONSTER_EARTH_ELEMENTAL &&
        monsterType != MONSTER_AIR_ELEMENTAL &&
        monsterType != MONSTER_FIRE_ELEMENTAL &&
        monsterType != MONSTER_WATER_ELEMENTAL) {
        if (forcedJoin) {
            gpPhilAI->EvaluateOneTimeCreaturePurchase(
                monsterType, monsterCount[0], 1, purchaseCount, purchaseValue,
                replacementSlot);
            if (purchaseCount > 0) {
                gpGame->GiveArmy(&eventHero->m_army, monsterType,
                                 monsterCount[0], replacementSlot);
                *handled = 1;
            } else {
                *handled = 1;
            }
        } else {
            if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY]) {
                if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] ==
                    MONSTER_DIPLOMACY_EXPERT)
                    joiningCount = monsterCount[0];
                else if (eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] ==
                         MONSTER_DIPLOMACY_ADVANCED)
                    joiningCount = monsterCount[0] / 2;
                else
                    joiningCount = monsterCount[0] / 4;
                if (!joiningCount)
                    joiningCount = 1;

                int joiningCost = static_cast<int>(
                    gMonsterDatabase[monsterType].cost * joiningCount *
                    MONSTER_AI_JOIN_COST_FRACTION);
                if (gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] <
                    joiningCost) {
                    if (strengthRatio > MONSTER_STRENGTH_FLEE)
                        goto computerMonstersFlee;
                    goto fightComputerMonsters;
                }
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    monsterType, monsterCount[0], 1, purchaseCount,
                    purchaseValue, replacementSlot);
                if (purchaseCount > 0) {
                    gpGame->m_players[eventHero->m_owner].resources[RES_GOLD] -=
                        joiningCost;
                    gpGame->GiveArmy(&eventHero->m_army, monsterType,
                                     joiningCount, replacementSlot);
                    *handled = 1;
                }
            }
        }
    }

    if (strengthRatio > MONSTER_STRENGTH_FLEE) {
computerMonstersFlee:
        gpAdvManager->GiveExperience(
            eventHero,
            gMonsterDatabase[monsterType].hitPoints * monsterCount[0], 1);
        eventHero->CheckLevel();
        if (eventHero->GetSSLevel(HERO_SKILL_NECROMANCY) &&
            eventHero->m_army.CanJoin(MONSTER_SKELETON)) {
            gpGame->GiveArmy(
                &eventHero->m_army, MONSTER_SKELETON,
                static_cast<int>(static_cast<double>(monsterCount[0]) *
                                 eventHero->GetSSLevel(HERO_SKILL_NECROMANCY) *
                                 MONSTER_NECROMANCY_FRACTION),
                -1);
        }
        *handled = 1;
    } else {
fightComputerMonsters:
        monsterCount[1] = gpPhilAI->CombatMonsterEvent(
            eventHero, monsterType, monsterCount, cell);
        if (monsterCount[1] != 0) {
            *handled = 1;
            return;
        }
        cell->w4hi = ((cell->w4hi & MONSTER_JOIN_FORCED) +
                      (static_cast<unsigned short>(monsterCount[0]) &
                       MONSTER_COUNT_MASK)) |
                     0;
    }
}

VA(0x004b5c40, 0x1d0)
int advManager::DoNetCombat(char *packet)
{
    hero *secondHero9;
    int secondSide15;
    int combatX1;
    int combatY8;
    int randomSeed1;
    signed char combatResult16[4];
    int firstSide2;
    hero *firstHero29;
    int remotePlayer8;
    int firstPlayer3;
    armyGroup *secondArmy10;
    armyGroup *firstArmy6;
    town *combatTown[3];

    firstHero29 = 0;
    firstArmy6 = 0;
    combatTown[0] = 0;
    secondHero9 = 0;
    secondArmy10 = 0;
    ReceiveHeroTownData(packet, &remotePlayer8, &combatX1, &combatY8,
                        &firstHero29, &firstArmy6, combatTown, &secondHero9,
                        &secondArmy10, &firstSide2, &secondSide15, &randomSeed1,
                        combatResult16, &gbRetreatWin, &gbCombatSurrender);
    firstPlayer3 = firstHero29->m_owner;
    combatResult16[0] = static_cast<signed char>(
        DoCombat(combatX1, combatY8, firstHero29, firstArmy6, combatTown[0],
                 secondHero9, secondArmy10, firstSide2, secondSide15,
                 randomSeed1, 0));
    if (!gbHumanPlayer[firstPlayer3]) {
        SendHeroTownData(combatX1, combatY8, firstHero29, firstArmy6,
                         combatTown[0], secondHero9, secondArmy10, firstSide2,
                         secondSide15, randomSeed1, remotePlayer8,
                         combatResult16[0], gbRetreatWin,
                         gbCombatSurrender);
    }
    if (firstArmy6)
        BaseFree(firstArmy6, EVENTS_FILE, EVENTS_NET_LINE + 0x46);
    if (secondArmy10)
        BaseFree(secondArmy10, EVENTS_FILE, EVENTS_NET_LINE + 0x49);
    if (combatTown[0])
        BaseFree(combatTown[0], EVENTS_FILE, EVENTS_NET_LINE + 0x4c);
    if (secondHero9)
        BaseFree(secondHero9, EVENTS_FILE, EVENTS_NET_LINE + 0x4f);
    if (firstHero29)
        BaseFree(firstHero29, EVENTS_FILE, EVENTS_NET_LINE + 0x52);
    gbRetreatWin = 0;
    return 1;
}

VA(0x004b5e10, 0x64e)
// Complete semantics, 0x78-byte frame, network receive/copy path, combat-result
// switch, and external relocation targets are recovered. The pre-95 residual
// is confined to the packet-command spill/trampoline cluster, local switch
// labels, and compiler-generated continuation jumps.
int advManager::DoCombat(int x, int y, hero *firstHero, armyGroup *firstArmy,
                         town *combatTown, hero *secondHero,
                         armyGroup *secondArmy, int firstSide, int secondSide,
                         int randomSeed, int processLosses)
{
    int unusedCombat[2];
    armyGroup *receivedSecondArmy;
    hero *receivedSecondHero;
    hero *receivedFirstHero;
    armyGroup *receivedFirstArmy;
    town *receivedTown;
    int remotePlayer;
    char *packet;
    signed char combatResult[4];
    tag_message message;
    int secondPlayer;
    int firstPlayer;
    int savedPlayer;
    int savedShowIt;

    if (giDebugLevel == 4)
        return AutoResolveCombat(x, y, firstHero, firstArmy, combatTown,
                                 secondHero, secondArmy, firstSide, secondSide,
                                 randomSeed, processLosses);
    gbInCombat = 1;
    firstPlayer = firstHero ? firstHero->m_owner : -1;
    secondPlayer = secondHero ? secondHero->m_owner
                              : (combatTown ? combatTown->m_owner : -1);
    if (randomSeed == -1)
        randomSeed = Random(1, 1000);
    DemobilizeCurrHero();
    savedPlayer = giCurPlayer;
    savedShowIt = bShowIt;

    if (firstPlayer >= 0 && secondPlayer >= 0 &&
        gbHumanPlayer[secondPlayer]) {
        if (!gbThisNetHumanPlayer[secondPlayer]) {
            iCombatControlNetPos = reinterpret_cast<int *>(giThisNetPos);
            iCombatControlGamePos = gbGamePosToNetPos[secondPlayer];
            SendHeroTownData(x, y, firstHero, firstArmy, combatTown, secondHero,
                             secondArmy, firstSide, secondSide, randomSeed,
                             gbGamePosToNetPos[secondPlayer], 0, 0, 0);
            if (!gbHumanPlayer[firstPlayer]) {
                for (;;) {
                    PollSound();
                    FillBitmapArea(gpWindowManager->m_screen, 30, 30, 4, 4, 0);
                    packet = CheckHandleNet();
                    if (packet && packet[6] == COMBAT_REMOTE_COMMAND) {
                        ReceiveHeroTownData(
                            packet, &remotePlayer, &x, &y, &receivedFirstHero,
                            &receivedFirstArmy, &receivedTown,
                            &receivedSecondHero, &receivedSecondArmy, &firstSide,
                            &secondSide, &randomSeed, combatResult,
                            &gbRetreatWin, &gbCombatSurrender);
                        if (receivedFirstArmy) {
                            memcpy(firstArmy, receivedFirstArmy,
                                   sizeof(armyGroup));
                            BaseFree(receivedFirstArmy, EVENTS_FILE,
                                     EVENTS_COMBAT_LINE + 0x71);
                        }
                        if (receivedSecondArmy) {
                            memcpy(secondArmy, receivedSecondArmy,
                                   sizeof(armyGroup));
                            BaseFree(receivedSecondArmy, EVENTS_FILE,
                                     EVENTS_COMBAT_LINE + 0x77);
                        }
                        if (receivedTown) {
                            memcpy(combatTown, receivedTown, sizeof(town));
                            BaseFree(receivedTown, EVENTS_FILE,
                                     EVENTS_COMBAT_LINE + 0x7d);
                        }
                        if (receivedSecondHero) {
                            memcpy(secondHero, receivedSecondHero, sizeof(hero));
                            BaseFree(receivedSecondHero, EVENTS_FILE,
                                     EVENTS_COMBAT_LINE + 0x83);
                        }
                        if (receivedFirstHero) {
                            memcpy(firstHero, receivedFirstHero, sizeof(hero));
                            BaseFree(receivedFirstHero, EVENTS_FILE,
                                     EVENTS_COMBAT_LINE + 0x89);
                        }
                        gpCombatManager->m_combatResult = combatResult[0];
                        goto combatFinished;
                    }
                    Process1WindowsMessage();
                    message = gpInputManager->GetEvent();
                    CheckHandleNetPlayerWait(message, 1);
                }
            }
        } else if (!gbThisNetHumanPlayer[firstPlayer]) {
            bShowIt = 1;
            gpGame->TurnOffAIMusic();
            sprintf(gText, "%s's %s is under attack!",
                    cPlayerNames[secondPlayer], combatTown ? "Town" : "Hero");
            gpGame->WaitForPlayer(gText, secondPlayer);
        }
    }

    bShowIt = 1;
    gpCombatManager->SetupCombat(x, y, firstHero, firstArmy, combatTown,
                                 secondHero, secondArmy, x, y, randomSeed);
    if (giHighMemBuffer > 2900) {
        gAdvDisposeLevel = 2;
    } else if (giHighMemBuffer > 900)
        gAdvDisposeLevel = 1;
    gpExec->CallManager(gpCombatManager);
    gpMouseManager->SetPointer(const_cast<char *>("advmice.mse"), 0, -999);
    gAdvDisposeLevel = 0;

combatFinished:
    if (firstHero)
        firstHero->CheckLevel();
    if (secondHero)
        secondHero->CheckLevel();
    if (processLosses) {
        switch (gpCombatManager->m_combatResult) {
        case 0:
            if (!gbRetreatWin)
                TransferArtifacts(secondHero, firstHero);
            HeroLoses(secondHero);
            break;
        case 1:
            if (!gbRetreatWin)
                TransferArtifacts(firstHero, secondHero);
            HeroLoses(firstHero);
            break;
        case 3:
            break;
        case -1:
            HeroLoses(firstHero);
            HeroLoses(secondHero);
        }
    }
    bShowIt = savedShowIt;
    giCurPlayer = savedPlayer;
    if (!gbHumanPlayer[savedPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = 0;
    gbInCombat = 0;
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    return gpCombatManager->m_combatResult;
}

VA(0x004b645e, 0x36f)
void advManager::SendHeroTownData(
    int x, int y, hero *firstHero, armyGroup *firstArmy, town *combatTown,
    hero *secondHero, armyGroup *secondArmy, int firstSide, int secondSide,
    int randomSeed, int remotePlayer, int combatResult, int retreatWin,
    int combatSurrender)
{
    char *reply;
    int result;
    combatRemoteData *buffer = 0;

    buffer = static_cast<combatRemoteData *>(BaseAlloc(
        COMBAT_REMOTE_BUFFER_SIZE, EVENTS_FILE, EVENTS_SEND_LINE + 3));
    reply = 0;
    buffer->fragment = 0;
    buffer->x = static_cast<signed char>(x);
    buffer->y = static_cast<signed char>(y);
    buffer->hasFirstHero = firstHero != 0;
    buffer->hasTown = combatTown != 0;
    buffer->hasSecondHero = secondHero != 0;
    buffer->firstSide = static_cast<signed char>(firstSide);
    buffer->secondSide = static_cast<signed char>(secondSide);
    buffer->randomSeed = randomSeed;
    buffer->combatResult = static_cast<signed char>(combatResult);
    buffer->retreatWin = static_cast<signed char>(retreatWin);
    buffer->combatSurrender = static_cast<signed char>(combatSurrender);
    if (firstHero) {
        buffer->firstOwner = firstHero->m_owner;
    } else {
        buffer->firstOwner = -1;
    }
    if (firstHero) {
        buffer->firstGold =
            gpGame->m_players[firstHero->m_owner].resources[RES_GOLD];
    } else {
        buffer->firstGold = 0;
    }
    if (secondHero) {
        buffer->secondOwner = secondHero->m_owner;
    } else {
        buffer->secondOwner = -1;
    }
    if (secondHero) {
        buffer->secondGold =
            gpGame->m_players[secondHero->m_owner].resources[RES_GOLD];
    } else {
        buffer->secondGold = 0;
    }
    memcpy(buffer->firstArmy, firstArmy, sizeof(armyGroup));
    memcpy(buffer->secondArmy, secondArmy, sizeof(armyGroup));
    if (combatTown)
        memcpy(buffer->townData, combatTown, sizeof(town));

    result = TransmitAndWait(reinterpret_cast<char *>(buffer), remotePlayer,
                             COMBAT_REMOTE_HEADER_SIZE, COMBAT_REMOTE_COMMAND,
                             COMBAT_REMOTE_CONFIRM_COMMAND, &reply);
    if (!result)
        ShutDown(0);

    if (firstHero) {
        buffer->fragment = COMBAT_REMOTE_FIRST_HERO_FIRST;
        memcpy(reinterpret_cast<char *>(buffer) + 1, firstHero,
               COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
        buffer->fragment = COMBAT_REMOTE_FIRST_HERO_SECOND;
        memcpy(reinterpret_cast<char *>(buffer) + 1,
               reinterpret_cast<char *>(firstHero) +
                   COMBAT_REMOTE_HERO_FIRST_SIZE,
               COMBAT_REMOTE_HERO_SECOND_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
    }
    if (secondHero) {
        buffer->fragment = COMBAT_REMOTE_SECOND_HERO_FIRST;
        memcpy(reinterpret_cast<char *>(buffer) + 1, secondHero,
               COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
        buffer->fragment = COMBAT_REMOTE_SECOND_HERO_SECOND;
        memcpy(reinterpret_cast<char *>(buffer) + 1,
               reinterpret_cast<char *>(secondHero) +
                   COMBAT_REMOTE_HERO_FIRST_SIZE,
               COMBAT_REMOTE_HERO_SECOND_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
    }
    BaseFree(buffer, EVENTS_FILE, EVENTS_SEND_LINE + 0x5c);
}

VA(0x004b67cd, 0x462)
void advManager::ReceiveHeroTownData(
    char *packet, int *remotePlayer, int *x, int *y, hero **firstHero,
    armyGroup **firstArmy, town **combatTown, hero **secondHero,
    armyGroup **secondArmy, int *firstSide, int *secondSide, int *randomSeed,
    signed char *combatResult, signed char *retreatWin,
    signed char *combatSurrender)
{
    int hasFirstHero7;
    int hasTown0;
    int hasSecondHero8;
    long lastPacketTime36;
    int result7;
    int gotFirstHeroFirst3;
    int gotFirstHeroSecond9;
    int gotSecondHeroFirst13;
    int gotSecondHeroSecond6;
    int firstOwner29;
    int secondOwner28;

    *firstHero = 0;
    *firstArmy = 0;
    *combatTown = 0;
    *secondHero = 0;
    *secondArmy = 0;
    hasFirstHero7 = hasSecondHero8 = hasTown0 = 0;
    *remotePlayer = packet[0];
    *x = packet[10];
    *y = packet[11];
    hasFirstHero7 = packet[12];
    hasTown0 = packet[13];
    hasSecondHero8 = packet[14];
    *firstSide = packet[15];
    *secondSide = packet[16];
    *randomSeed = *reinterpret_cast<int *>(packet + 17);
    *combatResult = packet[21];
    *retreatWin = packet[22];
    *combatSurrender = packet[23];
    firstOwner29 = packet[24];
    if (firstOwner29 > 0)
        gpGame->m_players[firstOwner29].resources[RES_GOLD] =
            *reinterpret_cast<int *>(packet + 25);
    secondOwner28 = packet[29];
    if (secondOwner28 > 0)
        gpGame->m_players[secondOwner28].resources[RES_GOLD] =
            *reinterpret_cast<int *>(packet + 30);

    *firstArmy = static_cast<armyGroup *>(
        BaseAlloc(sizeof(armyGroup), EVENTS_FILE, EVENTS_RECEIVE_LINE + 0x26));
    memcpy(*firstArmy, packet + 34, sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup *>(
        BaseAlloc(sizeof(armyGroup), EVENTS_FILE, EVENTS_RECEIVE_LINE + 0x29));
    memcpy(*secondArmy, packet + 49, sizeof(armyGroup));
    if (hasTown0) {
        *combatTown = static_cast<town *>(
            BaseAlloc(sizeof(town), EVENTS_FILE,
                      EVENTS_RECEIVE_LINE + 0x2e));
        memcpy(*combatTown, packet + 64, sizeof(town));
    }

    iCombatControlNetPos = reinterpret_cast<int *>(*remotePlayer);
    iCombatControlGamePos = giThisNetPos;
    result7 = TransmitRemoteData(0, *remotePlayer, 0,
                                 COMBAT_REMOTE_CONFIRM_COMMAND,
                                 COMBAT_REMOTE_FRAGMENT_TYPE,
                                 COMBAT_REMOTE_FRAGMENT_TYPE, -1);
    if (!result7)
        ShutDown(0);

    lastPacketTime36 = KBTickCount();
    gotFirstHeroFirst3 = 1;
    gotFirstHeroSecond9 = 1;
    gotSecondHeroFirst13 = 1;
    gotSecondHeroSecond6 = 1;
    if (hasFirstHero7) {
        *firstHero = static_cast<hero *>(
            BaseAlloc(sizeof(hero), EVENTS_FILE,
                      EVENTS_RECEIVE_LINE + 0x47));
        gotFirstHeroFirst3 = 0;
        gotFirstHeroSecond9 = 0;
    }
    if (hasSecondHero8) {
        *secondHero = static_cast<hero *>(
            BaseAlloc(sizeof(hero), EVENTS_FILE,
                      EVENTS_RECEIVE_LINE + 0x4d));
        gotSecondHeroFirst13 = 0;
        gotSecondHeroSecond6 = 0;
    }

    while (!gotFirstHeroFirst3 || !gotFirstHeroSecond9 ||
           !gotSecondHeroFirst13 || !gotSecondHeroSecond6) {
        PollSound();
        if (lastPacketTime36 + COMBAT_REMOTE_TIMEOUT < KBTickCount()) {
            NormalDialog(const_cast<char *>(
                             "Error receiving data.  Keep trying?"),
                         2, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                lastPacketTime36 = KBTickCount();
            else
                ShutDown(const_cast<char *>("Game canceled."));
        }
        packet = GetRemoteData(1);
        if (packet && packet[5] == 2 &&
            packet[6] == COMBAT_REMOTE_COMMAND) {
            lastPacketTime36 = KBTickCount();
            if (packet[9] == COMBAT_REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, packet + 10,
                       COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst3 = 1;
            }
            if (packet[9] == COMBAT_REMOTE_FIRST_HERO_SECOND) {
                memcpy(reinterpret_cast<char *>(*firstHero) +
                           COMBAT_REMOTE_HERO_FIRST_SIZE,
                       packet + 10, COMBAT_REMOTE_HERO_SECOND_SIZE);
                gotFirstHeroSecond9 = 1;
            }
            if (packet[9] == COMBAT_REMOTE_SECOND_HERO_FIRST) {
                memcpy(*secondHero, packet + 10,
                       COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotSecondHeroFirst13 = 1;
            }
            if (packet[9] == COMBAT_REMOTE_SECOND_HERO_SECOND) {
                memcpy(reinterpret_cast<char *>(*secondHero) +
                           COMBAT_REMOTE_HERO_FIRST_SIZE,
                       packet + 10, COMBAT_REMOTE_HERO_SECOND_SIZE);
                gotSecondHeroSecond6 = 1;
            }
        }
    }
}

VA(0x004b6c2f, 0x254)
// @early-stop: complete semantics, frame, slots, CFG, and external relocation
// targets. The retail body is exactly one five-byte continuation jump longer;
// the apparent relocation excess is the delinked six-entry local switch table.
int advManager::AutoResolveCombat(
    int x, int y, hero *firstHero, armyGroup *firstArmy, town *combatTown,
    hero *secondHero, armyGroup *secondArmy, int firstSide, int secondSide,
    int randomSeed, int processLosses)
{
    tag_message message;
    int savedShowIt37;

    gbNoShowCombat = 1;
    savedShowIt37 = bShowIt;
    bShowIt = 0;
    gpMouseManager->SetPointer(0);
    gpMouseManager->m_forcePointerUpdate = 1;
    message.type = 0;
    DemobilizeCurrHero();
    gpCombatManager->SetupCombat(x, y, firstHero, firstArmy, combatTown,
                                 secondHero, secondArmy, x, y, randomSeed);
    gpCombatManager->InitNonVisualVars();
    gpCombatManager->m_gridSelectionDisabled = 1;
    while (!gpCombatManager->m_nonVisualCombat)
        gpCombatManager->Main(message);
    gbNoShowCombat = 0;
    if (firstHero)
        firstHero->CheckLevel();
    if (secondHero)
        secondHero->CheckLevel();
    if (processLosses) {
        switch (gpCombatManager->m_combatResult) {
        case 0:
            if (!gbRetreatWin)
                TransferArtifacts(secondHero, firstHero);
            HeroLoses(secondHero);
            break;
        case 1:
            if (!gbRetreatWin)
                TransferArtifacts(firstHero, secondHero);
            HeroLoses(firstHero);
            break;
        case 3:
            break;
        case -1:
            HeroLoses(firstHero);
            HeroLoses(secondHero);
        }
    }
    bShowIt = savedShowIt37;
    if (!gbHumanPlayer[giCurPlayer]) {
        gpGame->ShowComputerScreen();
        gpGame->TurnOnAIMusic();
        SetNoDialogMenus(0);
    } else {
        SetNoDialogMenus(1);
    }
    MobilizeCurrHero(0);
    if (processLosses)
        gbRetreatWin = 0;
    gbInCombat = 0;
    gpMouseManager->m_forcePointerUpdate = 0;
    return gpCombatManager->m_combatResult;
}

VA(0x004b6e83, 0xb8)
int RiddleStringsEqual(char *answer, char *expected)
{
    int index;
    char expectedPrefix[5];
    char answerPrefix[8];

    strncpy(expectedPrefix, expected, 4);
    expectedPrefix[4] = 0;
    for (index = 3; index >= 0; index--) {
        if (expectedPrefix[index] != ' ') {
            expectedPrefix[index + 1] = 0;
            break;
        }
    }
    strncpy(answerPrefix, answer, 4);
    answerPrefix[strlen(expectedPrefix)] = 0;
    return stricmp(expectedPrefix, answerPrefix) == 0;
}

// ---- globals (definitions, RVA order) ----
DATA(0x0051cc0c) int gbNoShowCombat;
