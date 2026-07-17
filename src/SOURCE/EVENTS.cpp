// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\EVENTS.OBJ   from: (directly linked into exe)
// functions: 42   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
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
#include <SOURCE/Campaign.h>
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
#include <SOURCE/EVENTS_TYPES.h>
#include <SOURCE/townManager.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/x_arena.h>

#define EVENTS_FILE const_cast<char *>("I:\\Projects\\Heroes\\Prog\\SOURCE\\EVENTS.CPP")
#define EVENTS_REMOTE_MESSAGE(buffer) \
    (reinterpret_cast<RemoteMessage *>(buffer))
#define EVENTS_REMOTE_COMBAT(buffer) \
    (reinterpret_cast<combatRemoteData *>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_REMOTE_HERO(buffer) \
    (reinterpret_cast<combatRemoteHeroFragment *>(EVENTS_REMOTE_MESSAGE(buffer)->payload))
#define EVENTS_HERO_BUFFER(buffer) \
    (reinterpret_cast<combatRemoteHeroFragment *>(buffer))

DATA(0x005190a4) static char s_twoStringFormat[] = "%s %s";

// @semantic
// Complete semantics/CFG with the retail 0x350 frame, all source slots, and the
// main plus eight nested-switch spills at -0x330..-0x350. All 972 relocation
// sites agree: gpGame is 62/62 and gpWindowManager is 34/34. The mine uses four
// direct resource-type loads; Troll Bridge, City of the Dead, and Dragon City
// put the guarded arm first and share each recruit call through a label. The
// four base gSpellNames relocations use 0xff778+4*(n-1), while retail uses
// 0xff774+4*n; raw instructions prove the same effective address. First
// non-symbol divergence is the Sphinx resource loop: retail loads giCurPlayer
// before eventValue, while base loads eventValue first. Reversed array indexing,
// explicit pointer/additive forms, and inline giCurPlayer reads emitted the same
// base order or regressed it; bounded TU-state probes did not improve the wall.
// The repeated event-flag updates match retail load/op/store form via
// static_cast<int>. Identifier/declaration ordering, the unused tag_message,
// one-case switches/scopes, shared ClaimMine, resource sprintf calls, and
// duplicate artifact CheckLevel were recovered. GetHero/GetTown now reproduce
// the retail /Ob1 continuation jumps at +0x47bd and +0x483e. The retained
// source-hash max is 97.9046%; revisit the Sphinx register-order wall only with
// a new structural lead.
VA(0x004a8530, 0x5adb)
void advManager::DoEvent(mapCell *cell, i32 x, i32 y)
{
    i32 secondUpgrade1;
    i32 firstUpgrade_e;
    SAMPLE2 eventSample_f;
    i32 eventValue1;
    signEventExtra *signExtra_k;
    SAMPLE2 playedSample3;
    tag_message unusedEventMessage;
    i32 eventType_g;
    hero *eventHero2;
    char sphinxAnswer_a[SPHINX_ANSWER_BUFFER_SIZE];
    i32 eraseObject;
    i32 fizzleType3;
    i32 thirdUpgrade;
    i32 monsterType6;
    i32 guardedCount;
    i32 heroCombatResult3;
    i32 teleportY;
    i32 artifact8;
    mapEventExtra *eventExtra1;
    i32 resourceType;
    i32 teleportX3;
    i32 teleportCount;
    i32 guardedMonster5;
    boatRecord *boat_j;
    heroWindow *oracleWindow;
    tag_message oracleMessage1;
    hero *otherHero6;
    i32 resourceAmount6;
    i32 artifactResourceType;
    i32 secondaryAmount;
    i32 primaryAmount1;
    i32 secondaryReward_f;
    i32 primaryReward3;
    i32 correctAnswer1;
    char eventText[EVENT_TEXT_BUFFER_SIZE];
    mapCell *skeletonCell1;
    mapCell *zombieCell6;
    town *occupiedTown4;
    i32 levelExperience1;
    i32 heroLevel2;
    i32 dragonFactor_d;
    i32 mineCombatResult7;
    i32 adjacentMonster8;
    i32 wellSpellPoints5;
    i32 springSpellPoints;

    eventHero2 = &gpGame->m_heroRecs[gpCurPlayer->CurrentHero()];
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
    eraseObject = 0;
    fizzleType3 = 0;
    playedSample3 = NULL_SAMPLE2;
    gpMouseManager->ShowColorPointer();
    gpMouseManager->SetPointer(0);
    eventSample_f = NULL_SAMPLE2;

    switch (eventType_g) {
    case MAP_OBJECT_TRADING_POST:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        DoTradingPost(0, 0.2f);
        break;

    case MAP_OBJECT_MAGIC_GARDEN:
        if (cell->m_objectMetadata == MAP_EVENT_DATA_EMPTY) {
            EventWindow(-1, 1,
                        "{Magic Garden}\n\nYou've found a magic garden, the kind of place that leprechauns and faeries like to cavort in, but there is no one here today.  Perhaps you should try again next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 1,
                        "{Magic Garden}\n\nYou catch a leprechaun foolishly sleeping amidst a cluster of magic mushrooms.  In exchange for his freedom, he guides you to a small pot filled with precious things.",
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET,
                        cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                            ? MAP_EVENT_GOLD_AMOUNT
                            : MAP_EVENT_RESOURCE_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero2, cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET,
                         cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                             ? MAP_EVENT_GOLD_AMOUNT
                             : MAP_EVENT_RESOURCE_AMOUNT);
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_OBJECT_SPHINX:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        eventExtra1 = reinterpret_cast<mapEventExtra *>(ppMapExtra[cell->m_objectMetadata]);
        if (!eventExtra1->active) {
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
                        eventExtra1->riddle);
                GetDataEntry(gText, sphinxAnswer_a, SPHINX_INPUT_LENGTH, 0, 0, 1);
                correctAnswer1 = 0;
                for (eventValue1 = 0; eventValue1 < eventExtra1->answerCount; eventValue1++) {
                    if (RiddleStringsEqual(sphinxAnswer_a, eventExtra1->answers[eventValue1]))
                        correctAnswer1 = 1;
                }

                if (correctAnswer1) {
                    primaryReward3 = MAP_EVENT_REWARD_NONE;
                    primaryAmount1 = 0;
                    secondaryReward_f = MAP_EVENT_REWARD_NONE;
                    secondaryAmount = 0;
                    for (eventValue1 = 0; eventValue1 < SPHINX_RESOURCE_COUNT; eventValue1++) {
                        gpGame->m_players[giCurPlayer].m_resources[eventValue1] += eventExtra1->resources[eventValue1];
                        if (gpGame->m_players[giCurPlayer].m_resources[eventValue1] < 0)
                            gpGame->m_players[giCurPlayer].m_resources[eventValue1] = 0;
                        if (eventExtra1->resources[eventValue1] != 0) {
                            if (primaryReward3 != MAP_EVENT_REWARD_NONE) {
                                secondaryReward_f = primaryReward3;
                                secondaryAmount = primaryAmount1;
                            }
                            primaryReward3 = eventValue1;
                            primaryAmount1 = eventExtra1->resources[eventValue1];
                        }
                    }

                    if (eventExtra1->artifact != MAP_EVENT_REWARD_NONE &&
                        eventHero2->NumArtifacts() < 14) {
                        GiveArtifact(eventHero2, eventExtra1->artifact, 1, -1);
                        if (primaryReward3 != MAP_EVENT_REWARD_NONE) {
                            secondaryReward_f = primaryReward3;
                            secondaryAmount = primaryAmount1;
                        }
                        primaryReward3 = MAP_EVENT_REWARD_ARTIFACT;
                        primaryAmount1 = eventExtra1->artifact;
                    }

                    NormalDialog("Looking somewhat disappointed, the Sphinx sighs.  You've answered my riddle so here's your reward.  Now begone.",
                                 1, -1, -1, primaryReward3, primaryAmount1,
                                 secondaryReward_f, secondaryAmount, -1, 0);
                    eventExtra1->active = 0;
                }
                else {
                    NormalDialog("\"You guessed incorrectly,\" the Sphinx says, smiling.  The Sphinx swipes at you with a paw, knocking you to the ground.  Another blow makes the world go black, and you know no more.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    HeroLoses(eventHero2);
                }
            }
        }
        break;

    case MAP_OBJECT_OBSERVATION_TOWER:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        NormalDialog("{Observation Tower}\n\nFrom the observation tower, you are able to see distant lands.",
                     1, -1, -1, -1, 0, -1, 0, -1, 0);
        gpGame->SetVisibility(x, y, giCurPlayer, OBSERVATION_TOWER_RADIUS);
        CompleteDraw(0);
        UpdateScreen(0, 0);
        break;

    case MAP_OBJECT_HILL_FORT:
        thirdUpgrade = -1;
        secondUpgrade1 = -1;
        firstUpgrade_e = -1;
        if (eventHero2->CreatureTypeCount(CREATURE_OGRE))
            firstUpgrade_e = CREATURE_OGRE;
        if (eventHero2->CreatureTypeCount(CREATURE_ORC)) {
            if (firstUpgrade_e == -1)
                firstUpgrade_e = CREATURE_ORC;
            else
                secondUpgrade1 = CREATURE_ORC;
        }
        if (eventHero2->CreatureTypeCount(CREATURE_DWARF)) {
            if (firstUpgrade_e == -1)
                firstUpgrade_e = CREATURE_DWARF;
            else if (secondUpgrade1 == -1)
                secondUpgrade1 = CREATURE_DWARF;
            else
                thirdUpgrade = CREATURE_DWARF;
        }

        if (firstUpgrade_e == -1) {
            EventWindow(-1, 1,
                        "{Hill Fort}\n\nAn unusual alliance of Orcs, Ogres, and Dwarves offer to train (upgrade) any such troops brought to them.  Unfortunately, you have none with you.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
            eventHero2->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
            eventHero2->UpgradeCreatures(CREATURE_DWARF, CREATURE_BATTLE_DWARF);
            if (thirdUpgrade == -1) {
                if (secondUpgrade1 == -1) {
                    sprintf(gText,
                            "{Hill Fort}\n\nAll of the %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s.",
                            gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[firstUpgrade_e + 1]);
                }
                else {
                    sprintf(gText,
                            "{Hill Fort}\n\nAll of the %s and %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s and %s.",
                            gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[secondUpgrade1],
                            gArmyNamesPlural[firstUpgrade_e + 1], gArmyNamesPlural[secondUpgrade1 + 1]);
                }
            }
            else {
                sprintf(gText,
                        "{Hill Fort}\n\n All of the %s, %s and %s you have in your army have been trained by the battle masters of the fort.  Your army now contains %s, %s, and %s.",
                        gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[secondUpgrade1],
                        gArmyNamesPlural[thirdUpgrade], gArmyNamesPlural[firstUpgrade_e + 1],
                        gArmyNamesPlural[secondUpgrade1 + 1], gArmyNamesPlural[thirdUpgrade + 1]);
            }
            EventWindow(-1, 1, gText, EVENT_WINDOW_REWARD_CREATURE, firstUpgrade_e + 1,
                        secondUpgrade1 == -1 ? -1 : EVENT_WINDOW_REWARD_CREATURE,
                        secondUpgrade1 + 1, -1);
        }
        break;

    case MAP_OBJECT_FREEMANS_FOUNDRY:
        thirdUpgrade = -1;
        secondUpgrade1 = -1;
        firstUpgrade_e = -1;
        if (eventHero2->CreatureTypeCount(CREATURE_PIKEMAN))
            firstUpgrade_e = CREATURE_PIKEMAN;
        if (eventHero2->CreatureTypeCount(CREATURE_SWORDSMAN)) {
            if (firstUpgrade_e == -1)
                firstUpgrade_e = CREATURE_SWORDSMAN;
            else
                secondUpgrade1 = CREATURE_SWORDSMAN;
        }
        if (eventHero2->CreatureTypeCount(CREATURE_IRON_GOLEM)) {
            if (firstUpgrade_e == -1)
                firstUpgrade_e = CREATURE_IRON_GOLEM;
            else if (secondUpgrade1 == -1)
                secondUpgrade1 = CREATURE_IRON_GOLEM;
            else
                thirdUpgrade = CREATURE_IRON_GOLEM;
        }

        if (firstUpgrade_e == -1) {
            EventWindow(-1, 1,
                        "{Freeman's Foundry}\n\nA blacksmith working at the foundry offers to convert all Pikemen and Swordsmen's weapons brought to him from iron to steel. He also says that he knows a process that will convert Iron Golems into Steel Golems.  Unfortunately, you have none of these troops in your army, so he can't help you.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->UpgradeCreatures(CREATURE_PIKEMAN, CREATURE_VETERAN_PIKEMAN);
            eventHero2->UpgradeCreatures(CREATURE_SWORDSMAN, CREATURE_MASTER_SWORDSMAN);
            eventHero2->UpgradeCreatures(CREATURE_IRON_GOLEM, CREATURE_STEEL_GOLEM);
            if (thirdUpgrade == -1) {
                if (secondUpgrade1 == -1) {
                    sprintf(gText,
                            "{Freeman's Foundry}\n\nAll of your %s have been upgraded into %s.",
                            gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[firstUpgrade_e + 1]);
                }
                else {
                    sprintf(gText,
                            "{Freeman's Foundry}\n\nAll of your %s and %s have been upgraded into %s and %s.",
                            gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[secondUpgrade1],
                            gArmyNamesPlural[firstUpgrade_e + 1], gArmyNamesPlural[secondUpgrade1 + 1]);
                }
            }
            else {
                sprintf(gText,
                        "{Freeman's Foundry}\n\nAll of your  %s, %s and %s have been upgraded into %s, %s, and %s.",
                        gArmyNamesPlural[firstUpgrade_e], gArmyNamesPlural[secondUpgrade1],
                        gArmyNamesPlural[thirdUpgrade], gArmyNamesPlural[firstUpgrade_e + 1],
                        gArmyNamesPlural[secondUpgrade1 + 1], gArmyNamesPlural[thirdUpgrade + 1]);
            }
            EventWindow(-1, 1, gText, EVENT_WINDOW_REWARD_CREATURE, firstUpgrade_e + 1,
                        secondUpgrade1 == -1 ? -1 : EVENT_WINDOW_REWARD_CREATURE,
                        secondUpgrade1 + 1, -1);
        }
        break;

    case MAP_OBJECT_SEA_CHEST:
        if (!(cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) ||
            eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
            if (!cell->m_objectMetadata) {
                NormalDialog("{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it, only to find it empty.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            else {
                NormalDialog("{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it and find 1500 gold pieces.",
                             1, -1, -1, RES_GOLD, SEA_CHEST_GOLD, -1, 0, -1, 0);
                GiveResource(eventHero2, RES_GOLD, SEA_CHEST_GOLD);
            }
        }
        else {
            eventValue1 = cell->m_objectMetadata & CHEST_ARTIFACT_MASK;
            sprintf(gText,
                    "{Chest}\n\nAfter spending hours trying to fish the chest out of the sea, you open it and find 1000 gold and the %s",
                    gArtifactNames[eventValue1]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, eventValue1,
                         RES_GOLD, SEA_CHEST_ARTIFACT_GOLD, -1, 0);
            GiveArtifact(eventHero2, eventValue1, 1, -1);
            GiveResource(eventHero2, RES_GOLD, SEA_CHEST_ARTIFACT_GOLD);
        }
        fizzleType3 = 1;
        eraseObject = 1;
        break;

    case MAP_OBJECT_FLOTSAM: {
        switch (cell->m_objectMetadata) {
        case 0:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, but find nothing.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        case 1:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, and find some wood.",
                         1, -1, -1, RES_WOOD, 5, -1, 0, -1, 0);
            GiveResource(eventHero2, RES_WOOD, 5);
            break;
        case 2:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam, and find some wood and some gold.",
                         1, -1, -1, RES_WOOD, 5, RES_GOLD, 200, -1, 0);
            GiveResource(eventHero2, RES_WOOD, 5);
            GiveResource(eventHero2, RES_GOLD, 200);
            break;
        case 3:
            NormalDialog("{Flotsam}\n\nYou search through the flotsam and find some wood and some gold.",
                         1, -1, -1, RES_WOOD, 10, RES_GOLD, 500, -1, 0);
            GiveResource(eventHero2, RES_WOOD, 10);
            GiveResource(eventHero2, RES_GOLD, 500);
            break;
        }
        fizzleType3 = 1;
        eraseObject = 1;
        break;
    }

    case MAP_OBJECT_SHIPWRECK_SURVIVOR:
        if (eventHero2->NumArtifacts() < EVENT_ARTIFACT_CAPACITY) {
            sprintf(gText,
                    "{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death in an unforgiving ocean.  Grateful, he rewards you for your act of kindness by giving you the %s.",
                    gArtifactNames[cell->m_objectMetadata]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, cell->m_objectMetadata,
                         -1, 0, -1, 0);
            GiveArtifact(eventHero2, cell->m_objectMetadata, 1, -1);
        }
        else {
            NormalDialog("{Shipwreck Survivor}\n\nYou've pulled a shipwreck survivor from certain death in an unforgiving ocean.  Grateful, he says, \"I would give you an artifact as a reward, but you're all full.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        fizzleType3 = 1;
        eraseObject = 1;
        break;

    case MAP_OBJECT_MAGELLAN_MAPS:
        if (gpCurPlayer->m_resources[RES_GOLD] < MAGELLAN_MAP_COST) {
            NormalDialog("{Magellan's Maps}\n\nThe captain sighs.  \"You don't have enough money, eh?  You can't expect me to give my maps away for free!\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
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

    case MAP_OBJECT_WITCH_HUT:
        if (!eventHero2->m_secondarySkills[cell->m_objectMetadata]) {
            if (eventHero2->m_secondarySkillCount < HERO_SECONDARY_SKILL_LIMIT) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                sprintf(gText,
                        "{Witch's Hut}\n\nAn ancient and immortal witch living in a hut with bird's legs for stilts teaches you %s for her own inscrutable purposes.",
                        gSecondarySkills[cell->m_objectMetadata]);
                EventWindow(-1, 1, gText, 17, cell->m_objectMetadata * 3, -1, 0, -1);
                eventHero2->GiveSS(cell->m_objectMetadata, 1);
            }
            else {
                sprintf(gText,
                        "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an ancient tome on %s.  As you approach, she turns and focuses her one glass eye on you.  \"You already know everything you deserve to learn!\" the witch screeches. \"NOW GET OUT OF MY HOUSE!\"",
                        gSecondarySkills[cell->m_objectMetadata]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
        else {
            sprintf(gText,
                    "{Witch's Hut}\n\nYou approach the hut and observe a witch inside studying an ancient tome on %s.  As you approach, she turns and speaks.  \"You already know that which I would teach you. I can help you no further.\"",
                    gSecondarySkills[cell->m_objectMetadata]);
            NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_ARTESIAN_SPRING:
        if (!cell->m_objectMetadata) {
            NormalDialog("{Artesian Spring}\n\nThe spring only refills once a week, and someone's already been here this week.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            cell->m_objectMetadata = 0;
            springSpellPoints = eventHero2->Stats(3) * 10;
            if (eventHero2->m_spellPoints < springSpellPoints * 2) {
                NormalDialog("{Artesian Spring}\n\nA drink from the spring fills your blood with magic!  You have twice your normal spell points in reserve.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero2->m_spellPoints = springSpellPoints * 2;
            }
            else {
                NormalDialog("{Artesian Spring}\n\nA drink at the spring is supposed to give you twice your normal spell points, but you are already at that level.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
        }
        break;

    case MAP_OBJECT_MAGIC_WELL:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_MAGIC_WELL)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            cell->m_objectMetadata = 0;
            wellSpellPoints5 = eventHero2->Stats(3) * 10;
            if (eventHero2->m_spellPoints < wellSpellPoints5) {
                NormalDialog("{Magic Well}\n\nA drink from the well has restored your spell points to maximum.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero2->m_eventFlags =
                    static_cast<i32>(eventHero2->m_eventFlags) |
                    HERO_EVENT_MAGIC_WELL;
                eventHero2->m_spellPoints = wellSpellPoints5;
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

    case MAP_OBJECT_COAST:
        if (eventHero2->m_eventFlags & HERO_EVENT_EMBARKED) {
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) &
                ~HERO_EVENT_EMBARKED;
            eventHero2->m_remainingMobility = 0;
            eventHero2->m_direction = static_cast<u8>(m_cursorDirection);
            m_cursorType = eventHero2->m_cursorType;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            playedSample3 = LoadPlaySample("killfade.82m");
            gpWindowManager->SaveFizzleSource(COAST_FIZZLE_X, COAST_FIZZLE_Y,
                                              COAST_FIZZLE_WIDTH, COAST_FIZZLE_HEIGHT);
            CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
            gpWindowManager->FizzleForward(COAST_FIZZLE_X, COAST_FIZZLE_Y,
                                           COAST_FIZZLE_WIDTH, COAST_FIZZLE_HEIGHT,
                                           -1, 0, 0);
            WaitEndSample(playedSample3, -1);
            CheckAdjacentMon(&adjacentMonster8);
        }
        break;

    case MAP_OBJECT_BOAT:
        boat_j = &gpGame->m_boats[cell->m_objectMetadata];
        gpGame->RestoreCell(-1, -1, boat_j->x, boat_j->y, cell, BOAT_RESTORE_MODE);
        eventHero2->m_eventFlags =
            static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_EMBARKED;
        eventHero2->m_remainingMobility = 0;
        boat_j->heroId = eventHero2->m_id;
        boat_j->owner = eventHero2->m_owner;
        m_cursorType = BOAT_CURSOR_TYPE;
        m_cursorDirection = boat_j->direction;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorActive = 1;
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        break;

    case MAP_OBJECT_MINE:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
            if (gpGame->m_mines[cell->m_objectMetadata].guardianType != -1) {
                mineCombatResult7 = CombatMonsterEvent(eventHero2,
                                                      gpGame->m_mines[cell->m_objectMetadata].guardianType,
                                                      gpGame->m_mines[cell->m_objectMetadata].guardianCount,
                                                      cell, x, y, 0, x, y,
                                                      -1, 0, 0, -1, 0, 0);
                if (mineCombatResult7 != 0)
                    break;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = -1;
                eventHero2->CheckLevel();
            }
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            if (gpGame->m_mines[cell->m_objectMetadata].resourceType == RES_GOLD)
                resourceAmount6 = MINE_GOLD_INCOME;
            else if (gpGame->m_mines[cell->m_objectMetadata].resourceType == RES_ORE)
                resourceAmount6 = MINE_ORE_INCOME;
            else
                resourceAmount6 = 1;
            EventWindow(gpGame->m_mines[cell->m_objectMetadata].resourceType +
                            MINE_RESOURCE_ICON_OFFSET,
                        1, "", gpGame->m_mines[cell->m_objectMetadata].resourceType,
                        -resourceAmount6, -1, 0, -1);
claimMine:
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
        }
        break;

    case MAP_OBJECT_ALCHEMIST_LAB:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(0, 1, "", 1, -1, -1, 0, -1);
            goto claimMine;
        }
        break;

    case MAP_OBJECT_SAWMILL:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(73, 1, "", 0, -2, -1, 0, -1);
            goto claimMine;
        }
        break;

    case MAP_OBJECT_LIGHTHOUSE:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] != giCurPlayer) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            EventWindow(58, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_TREASURE_CHEST:
chestGold:
        if (!(cell->m_objectMetadata & CHEST_ARTIFACT_FLAG)) {
            EventWindow(-1, 2,
                        "{Chest}\n\nAfter scouring the area, you fall upon a hidden treasure cache.  You may take the gold or distribute the gold to the peasants for experience.  Do you wish to keep the gold?",
                        RES_GOLD, cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                        14, (cell->m_objectMetadata * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER, 1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                GiveResource(eventHero2, RES_GOLD, cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER);
            else
                GiveExperience(eventHero2,
                               (cell->m_objectMetadata * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER, 0);
        }
        else {
            if (eventHero2->NumArtifacts() >= EVENT_ARTIFACT_CAPACITY) {
                cell->m_objectMetadata = 2;
                goto chestGold;
            }
            eventValue1 = cell->m_objectMetadata & CHEST_ARTIFACT_MASK;
            sprintf(gText,
                    "{Chest}\n\nAfter scouring the area, you fall upon a hidden chest, containing the ancient artifact '%s'",
                    gArtifactNames[eventValue1]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT, eventValue1,
                         -1, 0, -1, 0);
            GiveArtifact(eventHero2, eventValue1, 1, -1);
        }
        eraseObject = 1;
        fizzleType3 = 1;
        eventHero2->CheckLevel();
        break;

    case MAP_OBJECT_BUOY:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_BUOY)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_BUOY;
            eventHero2->m_morale++;
            EventWindow(3, 1, "", 12, 0, -1, 0, -1);
        }
        else {
            EventWindow(2, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_FAERIE_RING:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_FAERIE_RING)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) |
                HERO_EVENT_FAERIE_RING;
            eventHero2->m_luck++;
            EventWindow(13, 1, "", 10, 0, -1, 0, -1);
        }
        else {
            EventWindow(12, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_IDOL:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_IDOL)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_IDOL;
            eventHero2->m_luck++;
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

    case MAP_OBJECT_FOUNTAIN:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_FOUNTAIN)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_FOUNTAIN;
            eventHero2->m_luck++;
            EventWindow(16, 1, "", 10, 0, -1, 0, -1);
        }
        else {
            EventWindow(15, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_WATERING_HOLE:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_WATERING_HOLE)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) |
                HERO_EVENT_WATERING_HOLE;
            eventHero2->m_morale++;
            eventHero2->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero2->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
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

    case MAP_OBJECT_OASIS:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_OASIS)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_OASIS;
            eventHero2->m_morale++;
            eventHero2->m_mobility += OASIS_MOBILITY_BONUS;
            eventHero2->m_remainingMobility += OASIS_MOBILITY_BONUS;
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

    case MAP_OBJECT_TEMPLE:
        if (!(eventHero2->m_eventFlags & HERO_EVENT_TEMPLE)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->m_eventFlags =
                static_cast<i32>(eventHero2->m_eventFlags) | HERO_EVENT_TEMPLE;
            eventHero2->m_morale += 2;
            NormalDialog("{Temple}\n\nA visit and a prayer at the temple raises the morale of your troops.",
                         1, -1, -1, 12, 0, 12, 0, -1, 0);
        }
        else {
            NormalDialog("{Temple}\n\nIt doesn't help to pray twice before a battle.  Come back after you've fought.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_LEAN_TO:
        if (!cell->m_objectMetadata) {
            NormalDialog("{Lean To}\n\nThe lean-to is long abandoned.  There is nothing of value here.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            resourceType = (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1;
            resourceAmount6 = (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
            NormalDialog("{Lean To}\n\nYou've found an abandoned lean-to.  Poking about, you discover some resources hidden nearby.",
                         1, -1, -1, resourceType, resourceAmount6, -1, 0, -1, 0);
            GiveResource(eventHero2, resourceType, resourceAmount6);
            cell->m_objectMetadata = 0;
        }
        break;

    case MAP_OBJECT_WAGON:
        if (cell->m_objectMetadata) {
            if (!(cell->m_objectMetadata & WAGON_ARTIFACT_FLAG)) {
                resourceType = (cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK) - 1;
                resourceAmount6 = (cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT;
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(-1, 1,
                            "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Inside, you find some of the wagon's cargo still intact.",
                            resourceType, resourceAmount6, -1, 0, -1);
                GiveResource(eventHero2, resourceType, resourceAmount6);
                cell->m_objectMetadata = 0;
                break;
            }
            if (eventHero2->NumArtifacts() != EVENT_ARTIFACT_CAPACITY) {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                eventValue1 = cell->m_objectMetadata & WAGON_ARTIFACT_MASK;
                sprintf(gText,
                        "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Searching inside, you find the '%s'.",
                        gArtifactNames[eventValue1]);
                EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue1,
                            -1, 0, -1);
                GiveArtifact(eventHero2, eventValue1, 1, -1);
                cell->m_objectMetadata = 0;
                break;
            }
        }
        EventWindow(-1, 1,
                    "{Wagon}\n\nYou come across an old wagon left by a trader who didn't quite make it to safe terrain.  Unfortunately, others have found it first, and the wagon is empty.",
                    -1, 0, -1, 0, -1);
        cell->m_objectMetadata = 0;
        break;

    case MAP_OBJECT_SKELETON: {
        switch (cell->m_objectMetadata) {
        case SKELETON_EMPTY:
            EventWindow(EVENT_TEXT_SKELETON_EMPTY, 1, "",
                        -1, 0, -1, 0, -1);
            break;
        default:
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
                sprintf(gText, "%s.", "Treasure");
                EventWindow(-1, 1, gText, RES_GOLD, SKELETON_GOLD, -1, 0, -1);
                GiveResource(eventHero2, RES_GOLD, SKELETON_GOLD);
            }
            else {
                eventValue1 = cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET;
                sprintf(gText, s_twoStringFormat,
                        gEventText[EVENT_TEXT_SKELETON_REWARD],
                        gArtifactNames[eventValue1]);
                GiveArtifact(eventHero2, eventValue1, 1, -1);
                EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue1,
                            -1, 0, -1);
            }
            cell->m_objectMetadata = SKELETON_EMPTY;
            break;
        }
        break;
    }

    case MAP_OBJECT_CAMPFIRE:
        EventWindow(14, 1, "", RES_GOLD,
                    (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER,
                    cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK,
                    cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT, -1);
        GiveResource(eventHero2, RES_GOLD,
                     (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) * CAMPFIRE_GOLD_MULTIPLIER);
        GiveResource(eventHero2, cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK,
                     cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT);
        eraseObject = 1;
        fizzleType3 = 1;
        SetEnvironmentOrigin(m_mapOriginX + ERASE_ENVIRONMENT_BORDER,
                             m_mapOriginY + ERASE_ENVIRONMENT_BORDER, 1);
        break;

    case MAP_OBJECT_XANADU:
        if (!(eventHero2->m_xanaduVisits & (1 << cell->m_objectMetadata))) {
            if (eventHero2->m_level +
                    eventHero2->m_secondarySkills[HERO_SKILL_DIPLOMACY] * 2 <
                XANADU_ADMISSION_LEVEL) {
                NormalDialog("{Xanadu}\n\nThe butler opens the door and looks you up and down.  \"You are neither famous nor diplomatic enough to be admitted to see my master,\" he sniffs.  \"Come back when you think yourself worthy.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            }
            else {
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                NormalDialog("{Xanadu}\n\nThe butler admits you to see the master of the house.  He trains you in the four skills a hero should know.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                eventHero2->m_primaryStats[HERO_PRIMARY_ATTACK]++;
                eventHero2->m_primaryStats[HERO_PRIMARY_DEFENSE]++;
                eventHero2->m_primaryStats[HERO_PRIMARY_KNOWLEDGE]++;
                eventHero2->m_primaryStats[HERO_PRIMARY_SPELL_POWER]++;
                eventHero2->m_xanaduVisits |= 1 << cell->m_objectMetadata;
            }
        }
        else {
            NormalDialog("{Xanadu}\n\nRecognizing you, the butler refuses to admit you.  \"The master,\" he says, \"will not see the same student twice.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_FORT:
        if (!(eventHero2->m_fortVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog("{Fort}\n\n The soldiers living in the fort teach you a few new defensive tricks.",
                         1, -1, -1, 25, 101, -1, 0, -1, 0);
            eventHero2->m_primaryStats[HERO_PRIMARY_DEFENSE]++;
            eventHero2->m_fortVisits |= 1 << cell->m_objectMetadata;
        }
        else {
            NormalDialog("{Fort}\n\n \"I'm sorry sir,\" The leader of the soldiers says, \"but you already know everything we have to teach.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_STANDING_STONES:
        if (!(eventHero2->m_standingStoneVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog("{Standing Stones}\n\nYou've found a group of Druids worshipping at one of their strange stone edifices.  Silently, they teach you new ways to cast spells.",
                         1, -1, -1, 25, 102, -1, 0, -1, 0);
            eventHero2->m_primaryStats[HERO_PRIMARY_SPELL_POWER]++;
            eventHero2->m_standingStoneVisits |= 1 << cell->m_objectMetadata;
        }
        else {
            NormalDialog("{Standing Stones}\n\nYou've found a group of Druids worshipping at one of their strange stone edifices.  Silently, the Druids turn you away, indicating they have nothing new to teach you.",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_WITCH_DOCTOR_HUT:
        if (!(eventHero2->m_witchDoctorVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog("{Witch Doctor's Hut}\n\nAn Orcish witch doctor living in the hut deepens your knowledge of magic by showing you how to cast stones, read portents, and decipher the intricacies of chicken entrails.",
                         1, -1, -1, 25, 103, -1, 0, -1, 0);
            eventHero2->m_primaryStats[HERO_PRIMARY_KNOWLEDGE]++;
            eventHero2->m_witchDoctorVisits |= 1 << cell->m_objectMetadata;
        }
        else {
            NormalDialog("{Witch Doctor's Hut}\n\n\"Go 'way!\", the witch doctor barks, \"you know all I know.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_MERCENARY_CAMP:
        if (!(eventHero2->m_mercenaryCampVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            NormalDialog("{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their tactics.  The mercenaries welcome you and your troops and invite you to train with them.",
                         1, -1, -1, 25, 100, -1, 0, -1, 0);
            eventHero2->m_primaryStats[HERO_PRIMARY_ATTACK]++;
            eventHero2->m_mercenaryCampVisits |= 1 << cell->m_objectMetadata;
        }
        else {
            NormalDialog("{Mercenary Camp}\n\nYou've come upon a mercenary camp practicing their tactics.  \"You're too advanced for us,\" the mercenary captain says.  \"We can teach nothing more.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        break;

    case MAP_OBJECT_GAZEBO:
        if (!(eventHero2->m_gazeboVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(18, 1, "", 14, GAZEBO_EXPERIENCE, -1, 0, -1);
            GiveExperience(eventHero2, GAZEBO_EXPERIENCE, 0);
            eventHero2->m_gazeboVisits |= 1 << cell->m_objectMetadata;
            eventHero2->CheckLevel();
        }
        else {
            EventWindow(17, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_WATER_WHEEL:
        if (!cell->m_objectMetadata) {
            EventWindow(59, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(60, 1, "", RES_GOLD, cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero2, RES_GOLD, cell->m_objectMetadata * MAP_EVENT_GOLD_AMOUNT);
            cell->m_objectMetadata = 0;
        }
        break;

    case MAP_OBJECT_RESOURCE:
        resourceType = cell->m_objectIndex >> 1;
        GiveResource(eventHero2, resourceType,
                     resourceType == RES_GOLD
                         ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                         : cell->m_objectMetadata);
        strcpy(sphinxAnswer_a, gResourceNames[resourceType]);
        sphinxAnswer_a[0] += ' ';
        sprintf(gText, gEventText[EVENT_TEXT_RESOURCE_PICKUP], sphinxAnswer_a);
        BVResMsg(gText, resourceType,
                 resourceType == RES_GOLD
                     ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
                     : cell->m_objectMetadata);
        eraseObject = 1;
        fizzleType3 = 1;
        break;

    case MAP_OBJECT_WINDMILL:
        if (cell->m_objectMetadata < SPHINX_RESOURCE_COUNT) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(87, 1, "", cell->m_objectMetadata, WINDMILL_RESOURCE_AMOUNT,
                        -1, 0, -1);
            GiveResource(eventHero2, cell->m_objectMetadata, WINDMILL_RESOURCE_AMOUNT);
            cell->m_objectMetadata = WINDMILL_EMPTY;
        }
        else {
            EventWindow(86, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_ANCIENT_LAMP:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(19, 2, "", -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            RecruitEvent(eventHero2, CREATURE_GENIE, cell);
            if (!cell->m_objectMetadata) {
                eraseObject = 1;
                fizzleType3 = 1;
            }
        }
        break;

    case MAP_OBJECT_TREE_CITY:
        if (!cell->m_objectMetadata) {
            EventWindow(-1, 1,
                        "{Tree City}\n\nYou've found a Sprite Tree City.  Unfortunately, none of the Sprites living there wish to join an army.  Maybe next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Tree City}\n\nSome of the Sprites living in the tree city are willing to join your army for a price.  Do you want to recruit Sprites?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero2, CREATURE_SPRITE, cell);
        }
        break;

    case MAP_OBJECT_RUINS:
        if (!cell->m_objectMetadata) {
            EventWindow(-1, 1,
                        "{Ruins}\n\nYou search the ruins, but the Medusas that used to live here are gone.  Perhaps there will be more next week.",
                        -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Ruins}\n\nYou've found some Medusas living in the ruins.  They are willing to join your army for a price.  Do you want to recruit Medusas?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero2, CREATURE_MEDUSA, cell);
        }
        break;

    case MAP_OBJECT_TROLL_BRIDGE:
        if (!cell->m_objectMetadata) {
            EventWindow(-1, 1,
                        "{Troll Bridge}\n\nYou've found one of those bridges that Trolls are so fond of living under, but there are none here.  Perhaps there will be some next week.",
                        -1, 0, -1, 0, -1);
        }
        else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
            EventWindow(-1, 2,
                        "{Troll Bridge}\n\nTrolls living under the bridge challenge you.  Will you fight them?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
            if (CombatMonsterEvent(eventHero2, CREATURE_TROLL, 12,
                                   cell, x, y, 0, x, y,
                                   CREATURE_WAR_TROLL, 8, 2,
                                   -1, 0, 0) != 0)
                break;
            eventHero2->CheckLevel();
            cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Troll Bridge}\n\nA few Trolls remain, cowering under the bridge.  They approach you and offer to join your forces as mercenaries.  Do you want to buy any Trolls?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                goto recruitTroll;
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Troll Bridge}\n\nSome Trolls living under a bridge are willing to join your army, but for a price.  Do you want to recruit Trolls?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
recruitTroll:
            RecruitEvent(eventHero2, CREATURE_TROLL, cell);
        }
        break;

    case MAP_OBJECT_CITY_OF_DEAD:
        if (!cell->m_objectMetadata) {
            EventWindow(-1, 1,
                        "{City of the Dead}\n\nThe City of the Dead is empty of life, and empty of unlife as well.  Perhaps some undead will move in next week.",
                        -1, 0, -1, 0, -1);
        }
        else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
            EventWindow(-1, 2,
                        "{City of the Dead}\n\nYou've found the ruins of an ancient city, now inhabited solely by the undead.  Will you search?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
            if (CombatMonsterEvent(eventHero2, CREATURE_VAMPIRE_LORD, 10,
                                   cell, x, y, 0, x, y,
                                   CREATURE_ZOMBIE, 40, 2,
                                   CREATURE_POWER_LICH, 5, 1) != 0)
                break;
            eventHero2->CheckLevel();
            cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{City of the Dead}\n\nSome of the surviving Liches are impressed by your victory over their fellows, and offer to join you for a price.  Do you want to recruit Liches?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                goto recruitLich;
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{City of the Dead}\n\nSome Liches living here are willing to join your army for a price.  Do you want to recruit Liches?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
recruitLich:
            RecruitEvent(eventHero2, CREATURE_POWER_LICH, cell);
        }
        break;

    case MAP_OBJECT_DRAGON_CITY:
        if (!cell->m_objectMetadata) {
            EventWindow(-1, 1,
                        "{Dragon City}\n\nThe Dragon city has no Dragons willing to join you this week.  Perhaps a Dragon will become available next week.",
                        -1, 0, -1, 0, -1);
        }
        else if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG) {
            EventWindow(-1, 2,
                        "{Dragon City}\n\nYou stand before the Dragon City, a place off-limits to mere humans.  Do you wish to violate this rule and challenge the Dragons to a fight?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
            dragonFactor_d = 1;
            if (gbInCampaign && gpGame->m_campaignType == 1 &&
                gpGame->m_campaignScenario == 6)
                dragonFactor_d = 2;
            if (CombatMonsterEvent(eventHero2, CREATURE_GREEN_DRAGON,
                                   dragonFactor_d * 3, cell, x, y, 0, x, y,
                                   CREATURE_RED_DRAGON, dragonFactor_d, 1,
                                   CREATURE_BLACK_DRAGON, dragonFactor_d, 1) != 0)
                break;
            CheckEndGame(0, 1);
            if (gbGameOver)
                break;
            eventHero2->CheckLevel();
            cell->m_objectMetadata -= DWELLING_GUARDED_FLAG;
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Dragon City}\n\nHaving defeated the Dragon champions, the city's leaders agree to supply some Dragons to your army for a price.  Do you wish to recruit Dragons?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                goto recruitDragon;
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(-1, 2,
                        "{Dragon City}\n\nThe Dragon city is willing to offer some Dragons for your army for a price.  Do you wish to recruit Dragons?",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES)
                break;
recruitDragon:
            RecruitEvent(eventHero2, CREATURE_RED_DRAGON, cell);
        }
        break;

    case MAP_OBJECT_WAGON_CAMP:
        if (!cell->m_objectMetadata) {
            EventWindow(83, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(84, 2, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero2, CREATURE_ROGUE, cell);
        }
        break;

    case MAP_OBJECT_DESERT_TENT:
        if (!cell->m_objectMetadata) {
            EventWindow(81, 1, "", -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            EventWindow(82, 2, "", -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES)
                RecruitEvent(eventHero2, CREATURE_NOMAD, cell);
        }
        break;

    case MAP_OBJECT_ARCHER_HOUSE:
    case MAP_OBJECT_GOBLIN_HUT:
    case MAP_OBJECT_DWARF_COTTAGE:
    case MAP_OBJECT_PEASANT_HUT:
    case MAP_OBJECT_LOG_CABIN:
    case MAP_OBJECT_WATCH_TOWER:
    case MAP_OBJECT_TREE_HOUSE:
    case MAP_OBJECT_SIRENS:
    case MAP_OBJECT_HALFLING_HOLE:
    case MAP_OBJECT_EXCAVATION:
    case MAP_OBJECT_CAVE:
        if (cell->m_objectMetadata)
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        HouseEvent(eventHero2, cell);
        break;

    case MAP_OBJECT_MONSTER:
        PlayerMonsterInteract(cell, cell, eventHero2, &eraseObject, x, y, 0, x, y);
        break;

    case MAP_OBJECT_OBELISK:
        if (!(gpGame->m_obeliskVisitors[cell->m_objectMetadata] & (1 << eventHero2->m_owner))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            gpGame->m_obeliskVisitors[cell->m_objectMetadata] |= 1 << eventHero2->m_owner;
            EventWindow(68, 1, "", -1, 0, -1, 0, -1);
            ViewPuzzle();
        }
        else {
            EventWindow(69, 1, "", -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_TREE_OF_KNOWLEDGE: {
        if (!(eventHero2->m_treeKnowledgeVisits & (1 << cell->m_objectMetadata))) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            heroLevel2 = eventHero2->GetLevel(eventHero2->m_experience);
            levelExperience1 = eventHero2->GetExperience(heroLevel2 + 1) -
                              eventHero2->GetExperience(heroLevel2);
            switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
            case TREE_KNOWLEDGE_FREE:
                NormalDialog("{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in delight.  \"Ahh, an adventurer!  Allow me to teach you a little of what I have learned over the ages.\"",
                             1, -1, -1, 14, -1, -1, 0, -1, 0);
                GiveExperience(eventHero2, levelExperience1, 0);
                eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
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
                        GiveExperience(eventHero2, levelExperience1, 0);
                        eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
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
                        GiveExperience(eventHero2, levelExperience1, 0);
                        eventHero2->m_treeKnowledgeVisits |= 1 << cell->m_objectMetadata;
                    }
                }
                break;
            }
        }
        else {
            NormalDialog("{Tree of Knowledge}\n\nUpon your approach, the tree opens its eyes in delight.  \"It is good to see you, my student.  I hope my teachings have helped you.\"",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
        }
        eventHero2->CheckLevel();
        break;
    }

    case MAP_OBJECT_ORACLE:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(74, 1, "", -1, 0, -1, 0, -1);
        oracleWindow = new heroWindow(0, 0, "thiefwin.bin");
        if (oracleWindow == 0)
            MemError();
        SetWinText(oracleWindow, ORACLE_WINDOW_TEXT_ID);
        gpTownManager->SetupThievesGuild(oracleWindow, ORACLE_THIEVES_GUILD_RANK);
        strcpy(gText, "Shrine - Player Rankings");
        oracleMessage1.type = ORACLE_MESSAGE_TYPE;
        oracleMessage1.payload.widget.command = ORACLE_MESSAGE_COMMAND;
        oracleMessage1.payload.widget.id = 0;
        oracleMessage1.payload.widget.data.text = gText;
        oracleWindow->BroadcastMessage(oracleMessage1);
        gpWindowManager->DoDialog(oracleWindow, TrueFalseDialogHandler, 0);
        delete oracleWindow;
        RedrawAdvScreen(1, 0);
        break;

    case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
        eventValue1 = cell->m_objectMetadata - 1;
        sprintf(gText, "%s'%s'.  ",
                "{Shrine of the 1st Circle}\n\nYou come across a small shrine attended by a group of novice acolytes.  In exchange for your protection, they agree to teach you a simple spell - ",
                gSpellNames[eventValue1]);
        goto shrineSpell;

    case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
        eventValue1 = cell->m_objectMetadata - 1;
        sprintf(gText, "%s'%s'.  ",
                "{Shrine of the 2nd Circle}\n\nYou come across an ornate shrine attended by a group of rotund friars.  In exchange for your protection, they agree to teach you a spell - ",
                gSpellNames[eventValue1]);
        goto shrineSpell;

    case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
        eventValue1 = cell->m_objectMetadata - 1;
        sprintf(gText, "%s'%s'.  ",
                "{Shrine of the 3rd Circle}\n\nYou come across a lavish shrine attended by a group of high priests.  In exchange for your protection, they agree to teach you a sophisticated spell - ",
                gSpellNames[eventValue1]);
shrineSpell:
        if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
            strcat(gText, "Unfortunately, you have no Magic Book to record the spell with.");
            EventWindow(-1, 1, gText, -1, 0, -1, 0, -1);
        }
        else if (eventHero2->m_secondarySkills[HERO_SKILL_WISDOM] + SHRINE_WISDOM_BONUS <
                 gsSpellInfo[eventValue1].level) {
            strcat(gText,
                   "Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.  ");
            EventWindow(-1, 1, gText, -1, 0, -1, 0, -1);
        }
        else {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            eventHero2->AddSpell(eventValue1, eventHero2->Stats(3));
            EventWindow(-1, 1, gText, 8, eventValue1, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_CASTLE:
        TownEvent(cell, x, y);
        break;

    case MAP_OBJECT_WHIRLPOOL:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        DoWhirlpool(eventHero2);

findTeleportDestination:
        teleportCount = 0;
        for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
            for (teleportX3 = 0; teleportX3 < MAP_WIDTH; teleportX3++) {
                if ((gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_triggerType ==
                        (eventType_g | TELEPORT_TRIGGER_FLAG) &&
                    (gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_objectIndex == cell->m_objectIndex &&
                    (eventType_g == MAP_OBJECT_STONE_LITHS
                         ? STONE_LITHS_MIN_DISTANCE
                         : WHIRLPOOL_MIN_DISTANCE) <
                        abs(teleportY - y) + abs(teleportX3 - x)) {
                    teleportCount++;
                }
            }
        }
        if (teleportCount > 0) {
            if (teleportCount > 1)
                teleportCount = Random(1, teleportCount);
            for (teleportY = 0; teleportY < MAP_HEIGHT; teleportY++) {
                for (teleportX3 = 0; teleportX3 < MAP_WIDTH; teleportX3++) {
                    if ((gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_triggerType ==
                            (eventType_g | TELEPORT_TRIGGER_FLAG) &&
                        (gpGame->m_worldMap.Row(teleportY) + teleportX3)->m_objectIndex == cell->m_objectIndex &&
                        (teleportX3 != x || teleportY != y) &&
                        (eventType_g == MAP_OBJECT_STONE_LITHS
                             ? STONE_LITHS_MIN_DISTANCE
                             : WHIRLPOOL_MIN_DISTANCE) <
                            abs(teleportY - y) + abs(teleportX3 - x) &&
                        --teleportCount < 1) {
                        goto teleportHero;
                    }
                }
            }
teleportHero:
            StopCursor(1);
            gpAdvManager->TeleportTo(eventHero2, teleportX3, teleportY, 1, 0);
        }
        break;

    case MAP_OBJECT_STONE_LITHS:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        goto findTeleportDestination;

    case MAP_OBJECT_ARTIFACT: {
        artifactResourceType = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK) >> ARTIFACT_EVENT_RESOURCE_SHIFT;
        artifact8 = cell->m_objectIndex >> 1;
        guardedMonster5 = cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK;
        if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY) {
            NormalDialog("You cannot pick up this artifact, you already have a full load!",
                         1, -1, -1, -1, 0, -1, 0, -1, 0);
            break;
        }

        if (xIsPlayingExpansionCampaign && xCampaign.IsSpecialGoldenBow(x, y)) {
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            strcpy(gText,
                   "The fabled golden bow of the elves lies here in the dust.  You take it and journey back to the elven towns.  They shower you with their graciousness and the king promises that his people will aid you whenever you seek help.");
            EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, artifact8,
                        -1, 0, -1);
            GiveArtifact(eventHero2, artifact8, 1, static_cast<i8>(cell->m_objectMetadata));
            eraseObject = 1;
            fizzleType3 = 1;
            break;
        }

        if (artifact8 == ARTIFACT_SPELL_SCROLL) {
            EventSound(eventType_g, 1, &eventSample_f);
            xTheSpell = cell->m_objectMetadata;
            EventWindow(-1, 1, gArtifactEvent[artifact8], MAP_EVENT_REWARD_ARTIFACT, artifact8,
                        -1, 0, -1);
            GiveArtifact(eventHero2, artifact8, 1, static_cast<i8>(cell->m_objectMetadata));
            eraseObject = 1;
            fizzleType3 = 1;
            break;
        }

        if (!(cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG)) {
            switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
            case ARTIFACT_EVENT_MODE_PICKUP:
artifactPickup:
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                EventWindow(-1, 1, gArtifactEvent[artifact8], MAP_EVENT_REWARD_ARTIFACT, artifact8,
                            -1, 0, -1);
                goto giveArtifact;

            case ARTIFACT_EVENT_MODE_GOLD:
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 2000 gold.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact8]);
                EventWindow(-1, 2, gText, MAP_EVENT_REWARD_ARTIFACT, artifact8,
                            -1, 0, -1);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] >= ARTIFACT_EVENT_GOLD_COST) {
                        gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] -= ARTIFACT_EVENT_GOLD_COST;
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

            case ARTIFACT_EVENT_MODE_WISDOM:
                if (eventHero2->m_secondarySkills[HERO_SKILL_WISDOM])
                    goto artifactPickup;
                sprintf(gText,
                        "{Artifact}\n\nYou've found the humble dwelling of a withered hermit.  The hermit tells you that he is willing to give the %s to the first wise person he meets.",
                        gArtifactNames[artifact8]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                break;

            case ARTIFACT_EVENT_MODE_LEADERSHIP:
                if (eventHero2->m_secondarySkills[HERO_SKILL_LEADERSHIP])
                    goto artifactPickup;
                sprintf(gText,
                        "{Artifact}\n\nYou've come across the spartan quarters of a retired soldier.  The soldier tells you that he is willing to pass on the %s to the first true leader he meets.",
                        gArtifactNames[artifact8]);
                NormalDialog(gText, 1, -1, -1, -1, 0, -1, 0, -1, 0);
                break;

            case ARTIFACT_EVENT_MODE_RESOURCE_3:
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                sprintf(sphinxAnswer_a, gResourceNames[artifactResourceType]);
                sphinxAnswer_a[0] += ' ';
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 2500 gold and 3 %s.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact8], sphinxAnswer_a);
                NormalDialog(gText, 2, -1, -1, MAP_EVENT_REWARD_ARTIFACT, artifact8,
                             -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] >= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST &&
                        gpGame->m_players[eventHero2->m_owner].m_resources[artifactResourceType] >= ARTIFACT_EVENT_RESOURCE_3_AMOUNT) {
                        gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] -= ARTIFACT_EVENT_RESOURCE_3_GOLD_COST;
                        gpGame->m_players[eventHero2->m_owner].m_resources[artifactResourceType] -= ARTIFACT_EVENT_RESOURCE_3_AMOUNT;
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

            case ARTIFACT_EVENT_MODE_RESOURCE_5:
                EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
                sprintf(sphinxAnswer_a, gResourceNames[artifactResourceType]);
                sphinxAnswer_a[0] += ' ';
                sprintf(gText,
                        "{Artifact}\n\nA leprechaun offers you the %s for the small price of 3000 gold and 5 %s.  Do you wish to buy this artifact?",
                        gArtifactNames[artifact8], sphinxAnswer_a);
                NormalDialog(gText, 2, -1, -1, MAP_EVENT_REWARD_ARTIFACT, artifact8,
                             -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                    if (gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] >= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST &&
                        gpGame->m_players[eventHero2->m_owner].m_resources[artifactResourceType] >= ARTIFACT_EVENT_RESOURCE_5_AMOUNT) {
                        gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] -= ARTIFACT_EVENT_RESOURCE_5_GOLD_COST;
                        gpGame->m_players[eventHero2->m_owner].m_resources[artifactResourceType] -= ARTIFACT_EVENT_RESOURCE_5_AMOUNT;
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
            EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
            if (guardedMonster5 == CREATURE_ROGUE) {
                NormalDialog("{Artifact}\n\nYou come upon an ancient artifact.  As you reach for it, a pack of Rogues leap out of the brush to guard their stolen loot.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
                guardedCount = ARTIFACT_EVENT_GUARD_ROGUE_COUNT;
            }
            else {
                guardedCount = 1;
                sprintf(gText,
                        "{Artifact}\n\nThrough a clearing you observe an ancient artifact.  Unfortunately, it's guarded by a nearby %s.  Do you want to fight the %s for the artifact?",
                        gArmyNames[guardedMonster5], gArmyNames[guardedMonster5]);
                NormalDialog(gText, 2, -1, -1, -1, 0, -1, 0, -1, 0);
                if (gpWindowManager->m_dialogResult != MONSTER_DIALOG_YES) {
                    NormalDialog("Discretion is the better part of valor, and you decide to avoid this fight for today.",
                                 1, -1, -1, -1, 0, -1, 0, -1, 0);
                    break;
                }
            }
            if (CombatMonsterEvent(eventHero2, guardedMonster5, guardedCount,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) != 0)
                break;
            eventHero2->CheckLevel();
            sprintf(gText, "Victorious, you take your prize, the %s", gArtifactNames[artifact8]);
            NormalDialog(gText, 1, -1, -1, MAP_EVENT_REWARD_ARTIFACT,
                         cell->m_objectIndex >> 1, -1, 0, -1, 0);
giveArtifact:
            GiveArtifact(eventHero2, artifact8, 1, -1);
            eraseObject = 1;
            fizzleType3 = 1;
        }
        break;
    }

    case MAP_OBJECT_HERO_INTERACTION:
        DemobilizeCurrHero();
        otherHero6 = gpGame->GetHero(cell->m_objectMetadata);
        if (otherHero6->m_owner == giCurPlayer) {
            HeroSwap(eventHero2, otherHero6);
        }
        else {
            occupiedTown4 = 0;
            if (otherHero6->m_locationType == HERO_TOWN_LOCATION) {
                occupiedTown4 = gpGame->GetTown(otherHero6->m_occupiedTown);
                occupiedTown4->m_occupyingHeroId = otherHero6->m_id;
            }
            heroCombatResult3 = DoCombat(x, y, eventHero2, &eventHero2->m_army,
                                        occupiedTown4, otherHero6, &otherHero6->m_army,
                                        x, y, -1, 1);
            if (heroCombatResult3 == 0 && occupiedTown4 != 0)
                gpGame->ClaimTown(occupiedTown4->m_id, giCurPlayer, 0);
        }
        break;

    case MAP_OBJECT_BOTTLE:
        if (cell->m_objectMetadata) {
            signExtra_k = reinterpret_cast<signEventExtra *>(ppMapExtra[cell->m_objectMetadata]);
            if (strlen(signExtra_k->text) < 2)
                EventWindow(-1, 1, cRandomSignText[(x < 0 ? -x : x) & 3],
                            -1, 0, -1, 0, -1);
            else
                EventWindow(-1, 1, signExtra_k->text, -1, 0, -1, 0, -1);
        }
        fizzleType3 = 1;
        eraseObject = 1;
        break;

    case MAP_OBJECT_SIGN:
        if (cell->m_objectMetadata) {
            signExtra_k = reinterpret_cast<signEventExtra *>(ppMapExtra[cell->m_objectMetadata]);
            if (strlen(signExtra_k->text) < 2)
                EventWindow(-1, 1, cRandomSignText[(x < 0 ? -x : x) & 3],
                            -1, 0, -1, 0, -1);
            else
                EventWindow(-1, 1, signExtra_k->text, -1, 0, -1, 0, -1);
        }
        break;

    case MAP_OBJECT_DAEMON_CAVE: {
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(-1, 2,
                    "{Daemon Cave}\n\nThe entrance to the cave is dark, and a foul, sulfurous smell issues from the cave mouth.  Will you enter?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_NO)
            break;
        if (cell->m_objectMetadata == DAEMON_CAVE_EMPTY) {
            EventWindow(-1, 1,
                        "Except for evidence of a terrible battle, the cave is empty.",
                        -1, 0, -1, 0, -1);
            break;
        }

        monsterType6 = ((cell->m_objectMetadata & DAEMON_SERVANT_MASK) >> DAEMON_SERVANT_SHIFT) + DAEMON_SERVANT_BASE;
        sprintf(gText,
                "You find a powerful and grotesque Demon in the cave.  \"Today,\" it rasps, \"you will fight and surely die.  But I will give you a choice of deaths.  You may fight me, or you may fight my servants.  Do you prefer to fight my servants?\"");
        EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (CombatMonsterEvent(eventHero2, monsterType6, DAEMON_SERVANT_COUNT,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) == 0) {
                eventHero2->CheckLevel();
                NormalDialog("Upon defeating the daemon's servants, you find a hidden cache with 2500 gold.",
                             1, -1, -1, RES_GOLD, DAEMON_GOLD, -1, 0, -1, 0);
                GiveResource(eventHero2, RES_GOLD, DAEMON_GOLD);
                cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            }
            break;
        }

        switch (cell->m_objectMetadata & DAEMON_REWARD_MASK) {
        case DAEMON_REWARD_EXPERIENCE:
            GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
            EventWindow(-1, 1,
                        "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and receive 1,000 experience points.",
                        14, DAEMON_EXPERIENCE, -1, 0, -1);
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            eventHero2->CheckLevel();
            break;

        case DAEMON_REWARD_ARTIFACT:
            if (eventHero2->NumArtifacts() == EVENT_ARTIFACT_CAPACITY ||
                gpGame->GetRandomArtifactId(EVENT_RANDOM_ARTIFACT_ANY, 1) == -1)
                goto daemonExperienceGold;
            GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
            eventValue1 = GiveRandomArtifact(eventHero2);
            sprintf(gText,
                    "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and find the %s in the back of the cave.",
                    gArtifactNames[eventValue1]);
            EventWindow(-1, 1, gText, MAP_EVENT_REWARD_ARTIFACT, eventValue1,
                        14, DAEMON_EXPERIENCE, -1);
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            eventHero2->CheckLevel();
            break;

        case DAEMON_REWARD_EXPERIENCE_GOLD:
daemonExperienceGold:
            EventWindow(-1, 1,
                        "The Demon screams its challenge and attacks!  After a short, desperate battle, you slay the monster and receive 1,000 experience points and 2,500 gold.",
                        RES_GOLD, DAEMON_GOLD, 14, DAEMON_EXPERIENCE, -1);
            GiveExperience(eventHero2, DAEMON_EXPERIENCE, 0);
            GiveResource(eventHero2, RES_GOLD, DAEMON_GOLD);
            cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
            eventHero2->CheckLevel();
            break;

        case DAEMON_REWARD_RANSOM:
            EventWindow(-1, 2,
                        "The Demon leaps upon you and has its claws at your throat before you can even draw your sword.  \"Your life is mine,\" it says.  \"I will sell it back to you for 2,500 gold.\"",
                        -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                if (gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] < DAEMON_GOLD) {
                    EventWindow(-1, 1,
                                "Seeing that you do not have 2500 gold, the demon slashes you with its claws, and the last thing you see is a red haze.",
                                -1, 0, -1, 0, -1);
                    HeroLoses(eventHero2);
                }
                else {
                    gpGame->m_players[eventHero2->m_owner].m_resources[RES_GOLD] -= DAEMON_GOLD;
                }
            }
            else {
                HeroLoses(eventHero2);
            }
            break;
        }
        cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
        break;
    }

    case MAP_OBJECT_SHIPWRECK: {
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(-1, 2,
                    "{Shipwreck}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.  Do you wish to search the shipwreck?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            switch (cell->m_objectMetadata) {
            case SKELETON_EMPTY:
                EventWindow(-1, 1,
                            "Upon defeating the Ghosts you spend several hours sifting through the debris and find nothing.  Such a despicable act reduces your army's morale.",
                            13, 0, -1, 0, -1);
                if (!(eventHero2->m_eventFlags & HERO_EVENT_SHIPWRECK)) {
                    eventHero2->m_eventFlags =
                        static_cast<i32>(eventHero2->m_eventFlags) |
                        HERO_EVENT_SHIPWRECK;
                    eventHero2->m_morale--;
                }
                break;
            default:
                if (GhostEvent(eventHero2, cell,
                               "Upon defeating the Ghosts you sift through the debris and find something!",
                               x, y))
                    cell->m_objectMetadata = SKELETON_EMPTY;
                break;
            }
        }
        break;
    }

    case MAP_OBJECT_GRAVEYARD: {
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(EVENT_TEXT_GRAVEYARD_PROMPT, 2, "",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            switch (cell->m_objectMetadata) {
            case SKELETON_EMPTY:
                EventWindow(EVENT_TEXT_GRAVEYARD_EMPTY, 1, "",
                            13, 0, -1, 0, -1);
                if (!(eventHero2->m_eventFlags & HERO_EVENT_GRAVEYARD)) {
                    eventHero2->m_eventFlags =
                        static_cast<i32>(eventHero2->m_eventFlags) |
                        HERO_EVENT_GRAVEYARD;
                    eventHero2->m_morale--;
                }
                break;
            default:
                zombieCell6 = GetCell(x - normalDirTable[eventHero2->m_direction].x,
                                     y - normalDirTable[eventHero2->m_direction].y);
                if (ZombieEvent(eventHero2, zombieCell6,
                                gEventText[EVENT_TEXT_GRAVEYARD_REWARD],
                                x, y))
                    cell->m_objectMetadata = SKELETON_EMPTY;
                break;
            }
        }
        break;
    }

    case MAP_OBJECT_DERELICT_SHIP: {
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(-1, 2,
                    "{Derelict Ship}\n\nThe rotting hulk of a great pirate ship creaks eerily as it is pushed against the rocks.  Do you wish to search the ship?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            switch (cell->m_objectMetadata) {
            case SKELETON_EMPTY:
                EventWindow(-1, 1,
                            "Upon defeating the Skeletons you spend several hours sifting through the debris and find nothing.  Such a despicable act reduces your army's morale.",
                            13, 0, -1, 0, -1);
                if (!(eventHero2->m_eventFlags & HERO_EVENT_DERELICT_SHIP)) {
                    eventHero2->m_eventFlags =
                        static_cast<i32>(eventHero2->m_eventFlags) |
                        HERO_EVENT_DERELICT_SHIP;
                    eventHero2->m_morale--;
                }
                break;
            default:
                skeletonCell1 = GetCell(x - normalDirTable[eventHero2->m_direction].x,
                                       y - normalDirTable[eventHero2->m_direction].y);
                if (SkeletonEvent(eventHero2, skeletonCell1,
                                  "Upon defeating the Skeletons you sift through the debris and find something!",
                                  x, y))
                    cell->m_objectMetadata = SKELETON_EMPTY;
                break;
            }
        }
        break;
    }

    case MAP_OBJECT_PYRAMID:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(-1, 2,
                    "You come upon the pyramid of a great and ancient king.  You are tempted to search it for treasure, but all the old stories warn of fearful curses and undead guardians.  Will you search?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (!cell->m_objectMetadata) {
                NormalDialog("You come upon the pyramid of a great and ancient king.  Routine exploration reveals that the pyramid is completely empty.",
                             1, -1, -1, 11, 0, 11, 0, -1, 0);
                if (!(eventHero2->m_eventFlags & HERO_EVENT_PYRAMID)) {
                    eventHero2->m_eventFlags =
                        static_cast<i32>(eventHero2->m_eventFlags) |
                        HERO_EVENT_PYRAMID;
                    eventHero2->m_luck -= 2;
                }
            }
            else {
                if (CombatMonsterEvent(eventHero2, CREATURE_ROYAL_MUMMY, 30,
                                       cell, x, y, 0, x, y,
                                       CREATURE_VAMPIRE_LORD, 20, 2,
                                       -1, 0, 0) == 0) {
                    eventHero2->CheckLevel();
                    eventValue1 = cell->m_objectMetadata - 1;
                    sprintf(eventText, "%s'%s'.  ",
                            "Upon defeating the monsters, you decipher an ancient glyph on the wall, telling the secret of the spell - ",
                            gSpellNames[eventValue1]);
                    if (!eventHero2->HasArtifact(ARTIFACT_MAGIC_BOOK)) {
                        strcat(eventText,
                               "  Unfortunately, you have no Magic Book to record the spell with.");
                        EventWindow(-1, 1, eventText, -1, 0, -1, 0, -1);
                    }
                    else if (eventHero2->m_secondarySkills[HERO_SKILL_WISDOM] <
                             PYRAMID_WISDOM_REQUIRED) {
                        strcat(eventText,
                               "  Unfortunately, you do not have the wisdom to understand the spell, and you are unable to learn it.  ");
                        EventWindow(-1, 1, eventText, -1, 0, -1, 0, -1);
                    }
                    else {
                        eventHero2->AddSpell(eventValue1, eventHero2->Stats(3));
                        EventWindow(-1, 1, eventText, 8, eventValue1, -1, 0, -1);
                    }
                    cell->m_objectMetadata = 0;
                }
            }
        }
        break;

    case MAP_OBJECT_ABANDONED_MINE:
        EventSound(eventType_g, cell->m_objectMetadata, &eventSample_f);
        EventWindow(-1, 2,
                    "You come upon an abandoned gold mine.  The mine appears to be haunted.  Do you wish to enter?",
                    -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (CombatMonsterEvent(eventHero2,
                                   gpGame->m_mines[cell->m_objectMetadata].guardianType,
                                   gpGame->m_mines[cell->m_objectMetadata].guardianCount,
                                   cell, x, y, 0, x, y,
                                   -1, 0, 0, -1, 0, 0) == 0) {
                eventHero2->CheckLevel();
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
                gpGame->m_mines[cell->m_objectMetadata].resourceType = RES_GOLD;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = -1;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            }
        }
        break;

    case MAP_OBJECT_BARRIER:
        eraseObject = BarrierEvent(cell, eventHero2);
        break;

    case MAP_OBJECT_TRAVELER_TENT:
        PasswordEvent(cell, eventHero2);
        break;

    case MAP_OBJECT_EXPANSION_OBJECT:
        GenericSiteEvent(cell, eventHero2);
        break;

    case MAP_OBJECT_EXPANSION_DWELLING:
        RecruitSiteEvent(cell, eventHero2);
        break;

    case MAP_OBJECT_JAIL:
        JailEvent(cell, eventHero2, x, y);
    }

    playedSample3 = eventSample_f;
    UpdateRadar(1, 0);
    UpdateHeroLocators(1, 1);
    UpdateTownLocators(1, 1);
    UpdBottomView(1, 1, 1);
    if (eraseObject) {
        EraseObj(cell, x, y);
        FizzleCenter(fizzleType3);
    }
    else {
        CompleteDraw(0);
    }
    UpdateScreen(0, 0);
    gpSoundManager->SwitchAmbientMusic(giTerrainToMusicTrack[m_currentTerrain]);
    WaitEndSample(playedSample3, -1);
    CheckEndGame(0, 0);
}

VA(0x004ae00b, 0x9f7)
void advManager::EraseObj(class mapCell *cell, i32 x, i32 y)
{
    i32 erased_a = 0;
    i32 frame_k = ERASE_NO_FRAME;
    i8 isWide_d = 0;
    mapCell *cells_h[ERASE_NEIGHBOR_COUNT];
    mapCellExtra *extras_b[ERASE_NEIGHBOR_COUNT];
    i32 i_e;
    i32 cellX_b[2];
    i32 cellY_e[2];
    mapCell *currentCell_k;
    mapCellExtra *extra_i;
    volatile i32 changed_i;

    for (i_e = 0; i_e < ERASE_NEIGHBOR_COUNT; i_e++) {
        cells_h[i_e] = 0;
        extras_b[i_e] = 0;
    }

    erased_a = 1;
    if (cell->m_objectTileset == ERASE_TILESET_11)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == ERASE_TILESET_63)
        frame_k = cell->m_objectIndex - 1;
    if (cell->m_objectTileset == ERASE_TILESET_62 && cell->m_objectIndex == 9) {
        frame_k = 9;
        isWide_d = 1;
    }
    if (cell->m_objectTileset == ERASE_TILESET_59 && cell->m_objectIndex == 131)
        frame_k = 124;
    if (cell->m_objectTileset == ERASE_TILESET_55 && cell->m_objectIndex == 61)
        frame_k = 54;
    if (cell->m_objectTileset == ERASE_TILESET_50 && cell->m_objectIndex == 45)
        frame_k = 38;
    if (cell->m_objectTileset == ERASE_TILESET_50 && cell->m_objectIndex == 19)
        frame_k = 12;
    if (cell->m_objectTileset == ERASE_TILESET_46) {
        switch (cell->m_objectIndex) {
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
            cells_h[i_e]->m_overlayTileset = 0;
            cells_h[i_e]->m_overlayIndex = ERASE_EMPTY_INDEX;
        }
        else if (cells_h[i_e]->m_objectIndex != ERASE_EMPTY_INDEX) {
        if (cells_h[i_e]->m_objectIndex == frame_k && cells_h[i_e]->m_objectTileset == cell->m_objectTileset) {
            cells_h[i_e]->m_objectIndex = 0;
            cells_h[i_e]->m_objectTileset = ERASE_CLEARED_TILESET;
            cells_h[i_e]->m_animatedObject = 0;
        }

        if (cells_h[i_e]->m_extraIndex && m_mapData->Extra(cells_h[i_e]->m_extraIndex)->objectIndex != ERASE_EMPTY_INDEX)
            extras_b[i_e] = m_mapData->Extra(cells_h[i_e]->m_extraIndex);
        else
            extras_b[i_e] = 0;

        while (extras_b[i_e]) {
            if (extras_b[i_e]->objectIndex == frame_k && extras_b[i_e]->objectTileset == cell->m_objectTileset) {
                extras_b[i_e]->objectIndex = 0;
                extras_b[i_e]->objectTileset = ERASE_CLEARED_TILESET;
                extras_b[i_e]->animatedObject = 0;
            }

            if (extras_b[i_e]->nextIndex && m_mapData->Extra(extras_b[i_e]->nextIndex)->objectIndex != ERASE_EMPTY_INDEX)
                extras_b[i_e] = m_mapData->Extra(extras_b[i_e]->nextIndex);
            else
                extras_b[i_e] = 0;
        }
        }
        }
        }
        }
    }

    cell->m_triggerType = 0;
    cell->m_objectIndex = 0;
    cell->m_objectTileset = ERASE_CLEARED_TILESET;
    cell->m_animatedObject = 0;

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;
        if (currentCell_k->m_objectTileset != ERASE_CLEARED_TILESET)
            continue;

        if (currentCell_k->m_extraIndex &&
            OD_STEER(m_mapData)->Extra(currentCell_k->m_extraIndex)->objectIndex !=
                ERASE_EMPTY_INDEX)
            extra_i = OD_STEER(m_mapData)->Extra(currentCell_k->m_extraIndex);
        else
            continue;

        if (extra_i->objectTileset == ERASE_CLEARED_TILESET || extra_i->objectIndex == ERASE_EMPTY_INDEX)
            continue;

            currentCell_k->m_objectIndex = extra_i->objectIndex;
            currentCell_k->m_objectTileset = extra_i->objectTileset;
            currentCell_k->m_animatedObject = extra_i->animatedObject;
            currentCell_k->m_objectLayerBit0 = extra_i->objectLayerBit0;
            currentCell_k->m_objectLayerBit1 = extra_i->objectLayerBit1;
            extra_i->objectIndex = 0;
            extra_i->objectTileset = ERASE_CLEARED_TILESET;
            extra_i->animatedObject = 0;
    }

    for (i_e = 0; i_e < ERASE_CELL_COUNT; i_e++) {
        currentCell_k = i_e == 0 ? cell : cells_h[i_e - 1];
        if (!currentCell_k)
            continue;

        if (currentCell_k->m_objectTileset != ERASE_CLEARED_TILESET &&
            currentCell_k->m_objectIndex != ERASE_EMPTY_INDEX && !currentCell_k->m_objectLayerBit1)
            goto cellDone;

        if (currentCell_k->m_extraIndex &&
            OD_STEER(m_mapData)->Extra(currentCell_k->m_extraIndex)->objectIndex !=
                ERASE_EMPTY_INDEX)
            extra_i = OD_STEER(m_mapData)->Extra(currentCell_k->m_extraIndex);
        else
            extra_i = 0;

        while (extra_i) {
            if (extra_i->objectTileset != ERASE_CLEARED_TILESET &&
                extra_i->objectIndex != ERASE_EMPTY_INDEX && !extra_i->objectLayerBit1)
                goto cellDone;

            if (extra_i->nextIndex &&
                OD_STEER(m_mapData)->Extra(extra_i->nextIndex)->objectIndex !=
                    ERASE_EMPTY_INDEX)
                extra_i = OD_STEER(m_mapData)->Extra(extra_i->nextIndex);
            else
                extra_i = 0;
        }

        currentCell_k->m_flags |= 0x80;
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
i32 advManager::BarrierEvent(mapCell *cell, hero *)
{
    SAMPLE2 eventSample = NULL_SAMPLE2;
    i32 color = cell->m_objectMetadata;
    color &= BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= BARRIER_PASSWORD_SHIFT;
    char response[BARRIER_INPUT_BUFFER_SIZE];

    sprintf(gText,
            "A magical %s barrier stands tall before you, blocking your way.  "
            "Runes on the arch read, \"Speak the key and you may pass.\"",
            xBarrierColor[color]);
    GetDataEntry(gText, response, BARRIER_INPUT_LENGTH, 0, 0, 1);
    if (StrEqNoCase(response, xPasswordStrings[passwordIndex]) &&
        (gpCurPlayer->m_barrierTents & (1 << color))) {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color,
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
i8 StrEqNoCase(char *firstString, char *secondString)
{
    char *firstPosition = firstString;
    char *secondPos = secondString;
    i32 characterCount = 0;
    char firstUpper;
    char secondUpper;

    while (1) {
        characterCount++;
        if (characterCount == GENERIC_SITE_STRING_LIMIT)
            return 1;
        firstUpper = static_cast<char>(
            toupper(static_cast<i32>(*firstPosition)));
        secondUpper = static_cast<char>(
            toupper(static_cast<i32>(*secondPos)));
        if (OD_STEER(firstUpper) == secondUpper) {
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
    i32 color = cell->m_objectMetadata;
    color &= BARRIER_COLOR_MASK;
    i32 passwordIndex = cell->m_objectMetadata;
    passwordIndex >>= BARRIER_PASSWORD_SHIFT;

    EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, color,
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

// @semantic
// Complete 0x6fe body, 0x54 frame/slots, CFG, and all 75 ordered relocation
// sites/effective targets align. OD_STEER(mapY9) restores retail's inner height compare
// and closes the former one-byte body/shifted-relocation residual. The only
// unmasked bytes left are +0x250/+0x253: retail loads primaryStat15 before
// eventHero for the arena stat increment, while ours loads eventHero first.
// Equivalent prefix/postfix/index spellings are byte-neutral in the identical
// GenericSiteAIEvent shape. Revisit only if the hero primary-stat representation
// or accessor changes, an earlier EVENTS source edit changes emitted evaluation
// order, or comparison gains a proved commutative-load normalization. Ten bounded
// TU-state declaration families produced no canonical exact closure.
VA(0x004aed38, 0x6fe)
void advManager::GenericSiteEvent(mapCell *cell, hero *eventHero)
{
    i32 currentSiteType;
    mapCell *currentCell5;
    i32 index3;
    i32 siteLevel6;
    i32 primaryStat15;
    i32 siteType2;
    i32 mapY9;
    i32 mapX37;
    i32 unusedSite;
    SAMPLE2 eventSample5;
    i32 cursedArtifactCount9;
    i32 unusedOne18;
    i8 stableResult26;
    i32 unusedTwo6;
    i32 creatureType;
    i32 experience11;
    i32 oldQuantity4;

    cursedArtifactCount9 = 0;
    eventSample5 = NULL_SAMPLE2;
    siteType2 = cell->m_objectMetadata;
    siteType2 &= GENERIC_SITE_TYPE_MASK;
    siteLevel6 = cell->m_objectMetadata;
    siteLevel6 >>= GENERIC_SITE_LEVEL_SHIFT;

    switch (siteType2) {
    case GENERIC_SITE_ALCHEMIST_TOWER:
        for (index3 = 0; index3 < GENERIC_SITE_ARTIFACT_SLOT_COUNT; index3++) {
            if (IsCursedItem(eventHero->m_artifacts[index3]))
                cursedArtifactCount9++;
        }
        if (cursedArtifactCount9 != 0) {
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
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
                            eventHero->m_artifacts[index3] = ARTIFACT_NONE;
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
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
                       &eventSample5);
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_ARENA;
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
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
                       &eventSample5);
            eventHero->m_eventFlags = static_cast<i32>(eventHero->m_eventFlags) |
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
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
                   &eventSample5);
        NormalDialog(
            "You enter a rickety hut and talk to the magician who lives there.  "
            "He tells you of places near and far which may aid you in your "
            "journeys.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        for (mapX37 = 0; mapX37 < MAP_WIDTH; mapX37++) {
            for (mapY9 = 0; OD_STEER(mapY9) < MAP_HEIGHT; mapY9++) {
                currentCell5 = gpGame->m_worldMap.Row(mapY9) + mapX37;
                currentSiteType = currentCell5->m_objectMetadata;
                currentSiteType &= BARRIER_COLOR_MASK;
                if ((currentCell5->m_triggerType & MAP_TRIGGER_TYPE_MASK) ==
                        MAP_OBJECT_EXPANSION_OBJECT &&
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
                            static_cast<i16>(
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
                EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
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
            eventHero->m_eventFlags = static_cast<i32>(eventHero->m_eventFlags) |
                                      HERO_EVENT_SIRENS;
        }
        break;

    case GENERIC_SITE_STABLES:
        unusedOne18 = 1;
        unusedTwo6 = 2;
        stableResult26 = 0;
        if ((eventHero->m_eventFlags & HERO_EVENT_STABLES) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_STABLES;
            eventHero->m_mobility += GENERIC_SITE_STABLE_MOBILITY;
            eventHero->m_remainingMobility += GENERIC_SITE_STABLE_MOBILITY;
            stableResult26 |= 1;
        }
        if (eventHero->CreatureTypeCount(CREATURE_CAVALRY)) {
            eventHero->UpgradeCreatures(CREATURE_CAVALRY,
                                        CREATURE_CHAMPION);
            stableResult26 |= 2;
        }
        if (stableResult26 != 0) {
            EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, siteType2,
                       &eventSample5);
        }
        sprintf(gText, xStableText[stableResult26]);
        if (stableResult26 & 2) {
            EventWindow(-1, 1, gText, EVENT_WINDOW_REWARD_CREATURE,
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
    u32 siteType2;
    i16 availableCount;
    i32 creatureType1;
    u32 siteIndex;
    u32 packedSite1;

    siteType2 = cell->m_objectMetadata;
    siteType2 &= AI_EVENT_RECRUIT_TYPE_MASK;
    availableCount = static_cast<i16>(cell->m_objectMetadata);
    availableCount >>= AI_EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType2) {
    case RECRUIT_SITE_GHOST:
        creatureType1 = CREATURE_GHOST;
        break;
    case RECRUIT_SITE_EARTH_ELEMENTAL:
        creatureType1 = CREATURE_EARTH_ELEMENTAL;
        break;
    case RECRUIT_SITE_AIR_ELEMENTAL:
        creatureType1 = CREATURE_AIR_ELEMENTAL;
        break;
    case RECRUIT_SITE_FIRE_ELEMENTAL:
        creatureType1 = CREATURE_FIRE_ELEMENTAL;
        break;
    case RECRUIT_SITE_WATER_ELEMENTAL:
        creatureType1 = CREATURE_WATER_ELEMENTAL;
        break;
    }

    siteIndex = siteType2;
    if (availableCount == 0) {
        EventWindow(-1, 1, xRecruitEmpty[siteIndex], -1, 0, -1, 0, -1);
    } else {
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, availableCount,
                   &eventSample);
        EventWindow(-1, 2, xRecruitBuy[siteIndex], -1, 0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            ExpansionRecruitEvent(eventHero, creatureType1, &availableCount);
            packedSite1 =
                (availableCount << AI_EVENT_RECRUIT_COUNT_SHIFT) | siteType2;
            cell->m_objectMetadata = packedSite1;
        }
    }
}

VA(0x004af5c7, 0x8b)
void advManager::ExpansionRecruitEvent(hero *eventHero, i32 creatureType,
                                       i16 *availableCount)
{
    // These unused dialog locals account for the retail /Od frame layout.
    tag_message dialogMessage2;
    baseManager *dialogManager =
        new recruitUnit(&eventHero->m_army, creatureType, availableCount);
    i32 dialogResult;
    if (dialogManager == 0)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
}

// @semantic
// Complete 0x22a body, frame/slots, CFG, and all 21 ordered relocation sites and
// effective targets align. Only rel32 bytes +0x7e/+0xb4 differ: retail routes
// the two dialog-complete arms through the final continuation, while ours targets
// the epilogue directly. An explicit early-return family removed both retail
// jumps and shifted the remainder by 10 bytes, so it was rejected. Revisit only
// if an earlier EVENTS edit changes this function's branch-target lowering or
// comparison normalizes proven continuation equivalence.
VA(0x004af652, 0x22a)
void advManager::JailEvent(mapCell *cell, hero *eventHero, i32 x, i32 y)
{
    SAMPLE2 eventSample1 = NULL_SAMPLE2;
    i32 heroId9;
    hero *releasedHero1;

    heroId9 = cell->m_objectMetadata;
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
        EventSound(cell->m_triggerType & MAP_TRIGGER_TYPE_MASK, 0, &eventSample1);
        NormalDialog(
            "In a dazzling display of daring, you break into the local jail and free the hero imprisoned there, who, in return, pledges loyalty to your cause.",
            1, -1, -1, -1, 0, -1, 0, -1, 0);
        gpGame->m_heroRecs[heroId9].m_owner = eventHero->m_owner;
        gpGame->m_availableHeroes[heroId9] = eventHero->m_owner;
        releasedHero1 = &gpGame->m_heroRecs[heroId9];
        EraseObj(cell, x, y);
        gpCurPlayer->m_heroIds[gpCurPlayer->m_heroCount] =
            static_cast<i8>(heroId9);
        gpCurPlayer->m_heroCount++;
        releasedHero1->m_x = x;
        releasedHero1->m_y = y;
        releasedHero1->m_eventFlags = 0;
        releasedHero1->m_direction = AI_EVENT_HERO_DIRECTION;
        releasedHero1->m_remainingMobility = releasedHero1->CalcMobility();
        releasedHero1->m_mobility = releasedHero1->m_remainingMobility;
        releasedHero1->m_locationType = cell->m_triggerType;
        releasedHero1->m_occupiedTown = cell->m_objectMetadata;
        cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
        cell->m_objectMetadata = heroId9;
        SendMapChange(AI_EVENT_HERO_MAP_CHANGE,
                      static_cast<i8>(heroId9), x, y,
                      AI_EVENT_HERO_MAP_CHANGE_VALUE, 0, 0);
    }
}

VA(0x004af87c, 0x1da)
void advManager::TownEvent(mapCell *cell, i32 x, i32 y)
{
    hero *eventHero1;
    i32 combatResult1;
    hero *defendingHero;
    town *eventTown1;

    eventTown1 = gpGame->GetTown(cell->m_objectMetadata);
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
void advManager::EventSound(i32 eventType, i32 eventData, struct SAMPLE2 *outSample)
{
    const i32 treasureSound_a = EVENT_SOUND_TREASURE;
    const i32 experienceSound_o = EVENT_SOUND_EXPERIENCE;
    const i32 moraleSound_e = EVENT_SOUND_MORALE;
    const i32 luckSound_g = EVENT_SOUND_LUCK;
    const i32 pickupSound_o = EVENT_SOUND_PICKUP;
    const i32 mineSound_c = EVENT_SOUND_MINE;
    char sampleFile_m[EVENT_SOUND_FILENAME_LENGTH];
    strcpy(sampleFile_m, "");
    i32 musicTrack_e = EVENT_SOUND_NONE;

    switch (eventType) {
    case MAP_OBJECT_ARTESIAN_SPRING:
    case MAP_OBJECT_FLOTSAM:
        musicTrack_e = pickupSound_o;
    case MAP_OBJECT_ARCHER_HOUSE:
    case MAP_OBJECT_GOBLIN_HUT:
    case MAP_OBJECT_DWARF_COTTAGE:
    case MAP_OBJECT_PEASANT_HUT:
    case MAP_OBJECT_LOG_CABIN:
    case MAP_OBJECT_TREE_CITY:
    case MAP_OBJECT_RUINS:
    case MAP_OBJECT_IDOL:
    case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
    case MAP_OBJECT_WAGON:
    case MAP_OBJECT_MAGELLAN_MAPS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_OBJECT_WATER_WHEEL:
    case MAP_OBJECT_LEAN_TO:
        musicTrack_e = EVENT_SOUND_TRACK_40;
        break;
    case MAP_OBJECT_SEA_CHEST:
    case MAP_OBJECT_PYRAMID:
    case MAP_OBJECT_CITY_OF_DEAD:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_OBJECT_BOAT:
        if (eventData == EVENT_SOUND_VARIANT_1)
            musicTrack_e = treasureSound_a;
        break;
    case MAP_OBJECT_SKELETON:
    case MAP_OBJECT_MINE:
    case MAP_OBJECT_MERMAID:
        musicTrack_e = treasureSound_a;
        break;
    case MAP_OBJECT_DAEMON_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_36;
        break;
    case MAP_OBJECT_SHIPWRECK:
    case MAP_OBJECT_WATERING_HOLE:
    case MAP_OBJECT_OBSERVATION_TOWER:
        musicTrack_e = EVENT_SOUND_TRACK_32;
        break;
    case MAP_OBJECT_FAERIE_RING:
    case MAP_OBJECT_FOUNTAIN:
    case MAP_OBJECT_WITCH_DOCTOR_HUT:
        musicTrack_e = luckSound_g;
        break;
    case MAP_OBJECT_BUOY:
    case MAP_OBJECT_RESOURCE:
    case MAP_OBJECT_HALFLING_HOLE:
    case MAP_OBJECT_XANADU:
        musicTrack_e = moraleSound_e;
        break;
    case MAP_OBJECT_GAZEBO:
    case MAP_OBJECT_TEMPLE:
    case MAP_OBJECT_CAVE:
        musicTrack_e = EVENT_SOUND_TRACK_37;
        break;
    case MAP_OBJECT_ANCIENT_LAMP:
    case MAP_OBJECT_TAR_PIT:
        musicTrack_e = EVENT_SOUND_TRACK_33;
        break;
    case MAP_OBJECT_GRAVEYARD:
    case MAP_OBJECT_DESERT_TENT:
    case MAP_OBJECT_STANDING_STONES:
    case MAP_OBJECT_DERELICT_SHIP:
    case MAP_OBJECT_BOTTLE:
    case MAP_OBJECT_REEFS:
        musicTrack_e = EVENT_SOUND_TRACK_39;
        break;
    case MAP_OBJECT_LIGHTHOUSE:
    case MAP_OBJECT_EXCAVATION:
    case MAP_OBJECT_SPHINX:
    case MAP_OBJECT_WITCH_HUT:
        musicTrack_e = EVENT_SOUND_TRACK_31;
        break;
    case MAP_OBJECT_OASIS:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_OBJECT_ABANDONED_MINE:
    case MAP_OBJECT_TREE_OF_KNOWLEDGE:
    case MAP_OBJECT_HILL_FORT:
    case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
        musicTrack_e = EVENT_SOUND_TRACK_38;
        break;
    case MAP_OBJECT_STONE_LITHS:
    case MAP_OBJECT_MERCENARY_CAMP:
        musicTrack_e = EVENT_SOUND_TRACK_34;
        break;
    case MAP_OBJECT_HUT_OF_MAGI:
        musicTrack_e = EVENT_SOUND_TRACK_1;
        break;
    case MAP_OBJECT_WHIRLPOOL:
    case MAP_OBJECT_FORT:
    case MAP_OBJECT_FREEMANS_FOUNDRY:
        musicTrack_e = EVENT_SOUND_TRACK_35;
        break;
    case MAP_OBJECT_BARRIER:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_OBJECT_TRAVELER_TENT:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_OBJECT_EXPANSION_OBJECT:
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
    case MAP_OBJECT_EXPANSION_DWELLING:
        musicTrack_e = experienceSound_o;
        break;
    case MAP_OBJECT_JAIL:
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
void advManager::EventWindow(i32 eventId, i32 buttons, char *text, i32 type1,
                             i32 value1, i32 type2, i32 value2, i32 type3)
{
    // Unused retail locals retain the original /Od frame around the text buffer.
    i32 dialogState7;
    i32 eventWindowUnused12;
    i32 windowGap1;
    i32 unusedStyle9;
    i32 unusedResult;
    i32 dialogState8;
    i32 eventWindowUnused11;
    i32 unusedStyle;
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
// @early-stop-reloc-only
// All 0xb6 relocation-masked bytes, 54 normalized instructions, frame/slots,
// CFG, and three ordered relocation sites/effective targets match after removing
// the empty loop else arm.
VA(0x004b0033, 0xb6)
i32 GiveArtifact(hero *eventHero, i32 artifact, i32 checkEndGame,
                 i8 artifactExtra)
{
    i32 artifactSlot;

    for (artifactSlot = 0; artifactSlot < EVENT_ARTIFACT_SLOT_COUNT;
         artifactSlot++) {
        if (eventHero->m_artifacts[artifactSlot] == ARTIFACT_NONE) {
            break;
        }
    }

    if (artifactSlot == EVENT_ARTIFACT_SLOT_COUNT) {
        return ARTIFACT_NONE;
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
i32 advManager::GiveRandomArtifact(hero *eventHero)
{
    i32 artifactId = gpGame->GetRandomArtifactId(EVENT_RANDOM_ARTIFACT_ANY, 1);

    if (artifactId == ARTIFACT_NONE)
        GiveResource(eventHero, RES_GOLD, EVENT_RANDOM_ARTIFACT_GOLD);
    else
        GiveArtifact(eventHero, artifactId, 1, ARTIFACT_NONE);
    return artifactId;
}

VA(0x004b0147, 0x67)
i32 advManager::GiveExperience(hero *eventHero, i32 experience, i32 checkLevel)
{
    // These unused level locals account for the retail /Od stack slots.
    i32 oldLevel;
    i32 unusedLevel2;
    i32 unusedLevel1;
    i32 newLevel1;
    i32 levelGap1;

    oldLevel = eventHero->GetLevel(eventHero->m_experience);
    eventHero->m_level = static_cast<i16>(oldLevel);
    eventHero->m_experience += experience;
    newLevel1 = eventHero->GetLevel(eventHero->m_experience);
    if (checkLevel)
        eventHero->CheckLevel();
    return newLevel1 - oldLevel;
}

VA(0x004b01ae, 0x80)
void advManager::GiveResource(hero *eventHero, i32 resourceType, i32 amount)
{
    if (resourceType >= 0 && resourceType <= RES_GOLD)
        gpGame->m_players[eventHero->m_owner].m_resources[resourceType] += amount;
    if (resourceType == RES_GOLD && gbHumanPlayer[eventHero->m_owner])
        CheckEndGame(0, 0);
}

VA(0x004b022e, 0xbb)
void advManager::RecruitEvent(hero *eventHero, i32 creatureType, mapCell *cell)
{
    tag_message dialogMessage2;
    i16 availableCount15 = static_cast<i16>(cell->m_objectMetadata);
    baseManager *dialogManager =
        new recruitUnit(&eventHero->m_army, creatureType, &availableCount15);
    i32 dialogResult;

    if (dialogManager == 0)
        MemError();
    gpExec->DoDialog(dialogManager);
    delete dialogManager;
    cell->m_objectMetadata = static_cast<u16>(availableCount15 + 0);
}

VA(0x004b02e9, 0x261)
i32 advManager::SkeletonEvent(hero *eventHero, mapCell *cell, char *text,
                              i32 x, i32 y)
{
    i32 artifactId;

    switch (cell->m_objectMetadata) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(eventHero, CREATURE_SKELETON,
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
        if (CombatMonsterEvent(eventHero, CREATURE_SKELETON,
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
        if (CombatMonsterEvent(eventHero, CREATURE_SKELETON,
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
        if (CombatMonsterEvent(eventHero, CREATURE_SKELETON,
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
i32 advManager::ZombieEvent(hero *eventHero, mapCell *cell, char *text,
                            i32 x, i32 y)
{
    i32 artifactId;
    switch (cell->m_objectMetadata) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(
                eventHero, CREATURE_ZOMBIE, ZOMBIE_EVENT_SMALL_COUNT,
                cell, x, y, 0, x, y, CREATURE_MUTANT_ZOMBIE,
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
                eventHero, CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_MEDIUM_COUNT, cell, x, y, 0, x, y,
                CREATURE_MUTANT_ZOMBIE,
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
                eventHero, CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_LARGE_COUNT, cell, x, y, 0, x, y,
                CREATURE_MUTANT_ZOMBIE,
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
                eventHero, CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_HUGE_COUNT, cell, x, y, 0, x, y,
                CREATURE_MUTANT_ZOMBIE,
                ZOMBIE_EVENT_HUGE_SUPPORT_COUNT, ZOMBIE_EVENT_SUPPORT_STACKS,
                -1, 0, 0) == 0) {
            artifactId = GiveRandomArtifact(eventHero);
            if (artifactId != ARTIFACT_NONE)
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
i32 advManager::GhostEvent(hero *eventHero, mapCell *cell, char *text,
                           i32 x, i32 y)
{
    i32 artifactId;
    switch (cell->m_objectMetadata) {
    case UNDEAD_EVENT_LEVEL_SMALL:
        if (CombatMonsterEvent(eventHero, CREATURE_GHOST,
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
        if (CombatMonsterEvent(eventHero, CREATURE_GHOST,
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
        if (CombatMonsterEvent(eventHero, CREATURE_GHOST,
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
        if (CombatMonsterEvent(eventHero, CREATURE_GHOST,
                               GHOST_EVENT_HUGE_COUNT, cell, x, y, 0, x, y,
                               -1, 0, 0, -1, 0, 0) == 0) {
            artifactId = GiveRandomArtifact(eventHero);
            sprintf(gText, "%s", text);
            if (artifactId != ARTIFACT_NONE)
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

// @early-stop
// Relocation-masked raw bytes are identical across all 0x274 bytes, including
// the address table at +0xc3..+0xf2 and byte table at +0xf3..+0x13d. The only
// residuals are delinked local-label and three equivalent empty-string symbols;
// all six external relocation targets agree.
VA(0x004b0add, 0x274)
void advManager::HouseEvent(hero *eventHero, mapCell *cell)
{
    i32 siteIndex = HOUSE_RECRUIT_ARCHER;
    i32 creatureTypes[HOUSE_RECRUIT_SITE_COUNT];

    switch (cell->m_triggerType & MAP_TRIGGER_TYPE_MASK) {
    case MAP_OBJECT_ARCHER_HOUSE:
        siteIndex = HOUSE_RECRUIT_ARCHER;
        break;
    case MAP_OBJECT_GOBLIN_HUT:
        siteIndex = HOUSE_RECRUIT_GOBLIN;
        break;
    case MAP_OBJECT_PEASANT_HUT:
        siteIndex = HOUSE_RECRUIT_PEASANT;
        break;
    case MAP_OBJECT_DWARF_COTTAGE:
    case MAP_OBJECT_SIRENS:
        siteIndex = HOUSE_RECRUIT_DWARF;
        break;
    case MAP_OBJECT_LOG_CABIN:
        siteIndex = HOUSE_RECRUIT_LOG_CABIN;
        break;
    case MAP_OBJECT_TREE_HOUSE:
        siteIndex = HOUSE_RECRUIT_TREE_HOUSE;
        break;
    case MAP_OBJECT_HALFLING_HOLE:
        siteIndex = HOUSE_RECRUIT_HALFLING;
        break;
    case MAP_OBJECT_WATCH_TOWER:
        siteIndex = HOUSE_RECRUIT_WATCH_TOWER;
        break;
    case MAP_OBJECT_CAVE:
        siteIndex = HOUSE_RECRUIT_CAVE;
        break;
    case MAP_OBJECT_EXCAVATION:
        siteIndex = HOUSE_RECRUIT_EXCAVATION;
        break;
    }

    if (cell->m_objectMetadata == 0) {
        EventWindow(siteIndex * 3 + HOUSE_EVENT_EMPTY_DIALOG_BASE, 1, "", -1,
                    0, -1, 0, -1);
    } else {
        creatureTypes[HOUSE_RECRUIT_ARCHER] = CREATURE_ARCHER;
        creatureTypes[HOUSE_RECRUIT_GOBLIN] = CREATURE_GOBLIN;
        creatureTypes[HOUSE_RECRUIT_PEASANT] = CREATURE_PEASANT;
        creatureTypes[HOUSE_RECRUIT_DWARF] = CREATURE_DWARF;
        creatureTypes[HOUSE_RECRUIT_LOG_CABIN] = CREATURE_DWARF;
        creatureTypes[HOUSE_RECRUIT_TREE_HOUSE] = CREATURE_SPRITE;
        creatureTypes[HOUSE_RECRUIT_HALFLING] = CREATURE_HALFLING;
        creatureTypes[HOUSE_RECRUIT_WATCH_TOWER] = CREATURE_ORC;
        creatureTypes[HOUSE_RECRUIT_CAVE] = CREATURE_CENTAUR;
        creatureTypes[HOUSE_RECRUIT_EXCAVATION] = CREATURE_SKELETON;

        EventWindow(siteIndex * 3 + HOUSE_EVENT_RECRUIT_DIALOG_BASE, 2, "", -1,
                    0, -1, 0, -1);
        if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
            if (eventHero->m_army.CanJoin(creatureTypes[siteIndex])) {
                eventHero->m_army.Add(creatureTypes[siteIndex], cell->m_objectMetadata,
                                      -1);
                cell->m_objectMetadata = 0;
            } else {
                EventWindow(siteIndex * 3 + HOUSE_EVENT_ARMY_FULL_DIALOG_BASE,
                            1, "", -1, 0, -1, 0, -1);
            }
        }
    }
}

// @semantic
// Shared enum declaration state reversed equivalent compare operand/polarity
// pairs at +0x164, +0x2f6, and +0x367. Frame, slots, CFG, and all 29 external
// relocation sites remain aligned; revisit during the byte-last-mile pass.
VA(0x004b0d51, 0x62f)
i32 advManager::CombatMonsterEvent(hero *eventHero, i32 monsterType,
                                   i32 monsterCount, mapCell *, i32 mapX,
                                   i32 mapY, i32 defender, i32 combatX,
                                   i32 combatY, i32 secondaryType,
                                   i32 secondaryCount, i32 secondaryStacks,
                                   i32 tertiaryType, i32 tertiaryCount,
                                   i32 tertiaryStacks)
{
    i32 stackCount;
    i32 stackIndex9;
    i32 combatResult7;
    i32 lastStackCount;
    i32 temporaryTypes7[COMBAT_MONSTER_ARMY_SLOTS];
    i32 temporaryCounts[COMBAT_MONSTER_ARMY_SLOTS];
    i32 placement4[COMBAT_MONSTER_ARMY_SLOTS + 1];
    i32 groupCount;
    i32 stackTotal;

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

    memset(gpMonGroup->m_creatureTypes, CREATURE_NONE,
           COMBAT_MONSTER_ARMY_SLOTS);
    memset(gpMonGroup->m_creatureCounts, 0,
           COMBAT_MONSTER_ARMY_SLOTS * sizeof(i16));
    stackCount = COMBAT_MONSTER_ARMY_SLOTS - secondaryStacks - tertiaryStacks;
    if (stackCount < 1)
        stackCount = 1;
    placement4[COMBAT_MONSTER_ARMY_SLOTS] = 0;
    SRand(combatY + combatX);
    if (stackCount == COMBAT_MONSTER_ARMY_SLOTS &&
        (gMonsterDatabase[monsterType].attributes & MONSTER_ATTRIBUTE_RANGED) == 0) {
        i32 roll = SRandom(0, COMBAT_MONSTER_RANDOM_MAX);
        if (roll < COMBAT_MONSTER_REDUCED_STACK_CHANCE)
            stackCount = 3;
        else if (roll < COMBAT_MONSTER_FOUR_STACK_THRESHOLD)
            stackCount = 4;
    }

    for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
        if (stackIndex9 == (stackCount >> 1) &&
            (monsterType == CREATURE_ARCHER || monsterType == CREATURE_PIKEMAN ||
             monsterType == CREATURE_SWORDSMAN || monsterType == CREATURE_CAVALRY ||
             monsterType == CREATURE_PALADIN || monsterType == CREATURE_ORC ||
             monsterType == CREATURE_OGRE || monsterType == CREATURE_TROLL ||
             monsterType == CREATURE_DWARF || monsterType == CREATURE_ELF ||
             monsterType == CREATURE_DRUID || monsterType == CREATURE_MINOTAUR ||
             monsterType == CREATURE_GREEN_DRAGON || monsterType == CREATURE_RED_DRAGON ||
             monsterType == CREATURE_IRON_GOLEM || monsterType == CREATURE_MAGE ||
             monsterType == CREATURE_GIANT || monsterType == CREATURE_ZOMBIE ||
             monsterType == CREATURE_MUMMY || monsterType == CREATURE_VAMPIRE ||
             monsterType == CREATURE_LICH) &&
            SRandom(0, COMBAT_MONSTER_RANDOM_MAX) < COMBAT_MONSTER_UPGRADE_CHANCE &&
            secondaryCount == 0 && tertiaryCount == 0)
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i8>(monsterType + 1);
        else
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i8>(monsterType);
        gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
            static_cast<i16>((stackIndex9 < monsterCount % stackCount) +
                               monsterCount / stackCount);
    }

    placement4[COMBAT_MONSTER_ARMY_SLOTS] += stackCount;
    if (secondaryStacks != 0) {
        stackCount = secondaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i8>(secondaryType);
            gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i16>((stackIndex9 < secondaryCount % stackCount) +
                                   secondaryCount / stackCount);
        }
    }

    placement4[COMBAT_MONSTER_ARMY_SLOTS] += stackCount;
    if (tertiaryStacks != 0) {
        stackCount = tertiaryStacks;
        for (stackIndex9 = 0; stackIndex9 < stackCount; stackIndex9++) {
            gpMonGroup->m_creatureTypes[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i8>(tertiaryType);
            gpMonGroup->m_creatureCounts[placement4[COMBAT_MONSTER_ARMY_SLOTS] + stackIndex9] =
                static_cast<i16>((stackIndex9 < secondaryCount % stackCount) +
                                   tertiaryCount / stackCount);
        }
    }
    lastStackCount = stackCount;

    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++) {
        if (gpMonGroup->m_creatureCounts[stackIndex9] <= 0)
            gpMonGroup->m_creatureTypes[stackIndex9] = CREATURE_NONE;
    }
    for (stackIndex9 = 0; stackIndex9 < COMBAT_MONSTER_ARMY_SLOTS; stackIndex9++)
        placement4[stackIndex9] = stackIndex9;

    if (lastStackCount == 1) {
        placement4[2] = 0;
        placement4[0] = 2;
    } else if (lastStackCount == 2) {
        placement4[1] = 1;
        placement4[3] = 0;
        placement4[0] = 3;
    } else if (lastStackCount == 3) {
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
            static_cast<i8>(temporaryTypes7[placement4[stackIndex9]]);
        gpMonGroup->m_creatureCounts[stackIndex9] =
            static_cast<i16>(temporaryCounts[placement4[stackIndex9]]);
    }

    if (defender != 0)
        combatResult7 = DoCombat(combatX, combatY, 0, gpMonGroup,
                                0, eventHero, &eventHero->m_army, mapX, mapY,
                                combatY + combatX, 1);
    else
        combatResult7 = DoCombat(combatX, combatY, eventHero, &eventHero->m_army,
                                0, 0, gpMonGroup, mapX, mapY,
                                combatY + combatX, 1);
    MobilizeCurrHero(0);
    return combatResult7;
}

VA(0x004b1380, 0x5f3)
void GiveTakeArtifactStat(hero *targetHero, i32 artifact, i32 take)
{
    i32 statChanges[EVENT_ARTIFACT_PRIMARY_STAT_COUNT + 1];
    i32 maxSpellPoints;

    if (artifact == ARTIFACT_NONE)
        return;
    statChanges[HERO_PRIMARY_ATTACK] = 0;
    statChanges[HERO_PRIMARY_DEFENSE] = 0;
    statChanges[HERO_PRIMARY_SPELL_POWER] = 0;
    statChanges[HERO_PRIMARY_KNOWLEDGE] = 0;

    switch (artifact) {
    case ARTIFACT_ULTIMATE_BOOK: statChanges[HERO_PRIMARY_KNOWLEDGE] = 12; break;
    case ARTIFACT_ULTIMATE_SWORD: statChanges[HERO_PRIMARY_ATTACK] = 12; break;
    case ARTIFACT_ULTIMATE_CLOAK: statChanges[HERO_PRIMARY_DEFENSE] = 12; break;
    case ARTIFACT_ULTIMATE_WAND: statChanges[HERO_PRIMARY_SPELL_POWER] = 12; break;
    case ARTIFACT_ULTIMATE_SHIELD:
        statChanges[HERO_PRIMARY_ATTACK] = 6;
        statChanges[HERO_PRIMARY_DEFENSE] = 6;
        break;
    case ARTIFACT_ULTIMATE_STAFF:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 6;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 6;
        break;
    case ARTIFACT_ULTIMATE_CROWN:
        statChanges[HERO_PRIMARY_ATTACK] = 4;
        statChanges[HERO_PRIMARY_DEFENSE] = 4;
        statChanges[HERO_PRIMARY_SPELL_POWER] = 4;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 4;
        break;
    case ARTIFACT_GOLDEN_GOOSE: break;
    case ARTIFACT_ARCANE_NECKLACE: statChanges[HERO_PRIMARY_SPELL_POWER] = 4; break;
    case ARTIFACT_CASTER_BRACELET: statChanges[HERO_PRIMARY_SPELL_POWER] = 2; break;
    case ARTIFACT_MAGE_RING: statChanges[HERO_PRIMARY_SPELL_POWER] = 2; break;
    case ARTIFACT_WITCHES_BROACH: statChanges[HERO_PRIMARY_SPELL_POWER] = 3; break;
    case ARTIFACT_MEDAL_OF_VALOR: break;
    case ARTIFACT_MEDAL_OF_COURAGE: break;
    case ARTIFACT_MEDAL_OF_HONOR: break;
    case ARTIFACT_MEDAL_OF_DISTINCTION: break;
    case ARTIFACT_FIZBIN_OF_MISFORTUNE: break;
    case ARTIFACT_THUNDER_MACE: statChanges[HERO_PRIMARY_ATTACK] = 1; break;
    case ARTIFACT_ARMORED_GAUNTLETS: statChanges[HERO_PRIMARY_DEFENSE] = 1; break;
    case ARTIFACT_DEFENDER_HELM: statChanges[HERO_PRIMARY_DEFENSE] = 1; break;
    case ARTIFACT_GIANT_FLAIL: statChanges[HERO_PRIMARY_ATTACK] = 1; break;
    case ARTIFACT_BALLISTA: break;
    case ARTIFACT_STEALTH_SHIELD: statChanges[HERO_PRIMARY_DEFENSE] = 2; break;
    case ARTIFACT_DRAGON_SWORD: statChanges[HERO_PRIMARY_ATTACK] = 3; break;
    case ARTIFACT_POWER_AXE: statChanges[HERO_PRIMARY_ATTACK] = 2; break;
    case ARTIFACT_DIVINE_BREASTPLATE: statChanges[HERO_PRIMARY_DEFENSE] = 3; break;
    case ARTIFACT_MINOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 2; break;
    case ARTIFACT_MAJOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 3; break;
    case ARTIFACT_SUPERIOR_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 4; break;
    case ARTIFACT_FOREMOST_SCROLL: statChanges[HERO_PRIMARY_KNOWLEDGE] = 5; break;
    case ARTIFACT_ENDLESS_SACK_GOLD: break;
    case ARTIFACT_ENDLESS_BAG_GOLD: break;
    case ARTIFACT_ENDLESS_PURSE_GOLD: break;
    case ARTIFACT_NOMAD_BOOTS: break;
    case ARTIFACT_TRAVELER_BOOTS: break;
    case ARTIFACT_RABBIT_FOOT: break;
    case ARTIFACT_GOLDEN_HORSESHOE: break;
    case ARTIFACT_GAMBLERS_COIN: break;
    case ARTIFACT_FOUR_LEAF_CLOVER: break;
    case ARTIFACT_TRUE_COMPASS: break;
    case ARTIFACT_SAILORS_ASTROLABE: break;
    case ARTIFACT_EVIL_EYE: break;
    case ARTIFACT_ENCHANTED_HOURGLASS: break;
    case ARTIFACT_GOLD_WATCH: break;
    case ARTIFACT_SKULLCAP: break;
    case ARTIFACT_ICE_CLOAK: break;
    case ARTIFACT_FIRE_CLOAK: break;
    case ARTIFACT_LIGHTNING_HELM: break;
    case ARTIFACT_EVERCOLD_ICICLE: break;
    case ARTIFACT_EVERHOT_LAVA_ROCK: break;
    case ARTIFACT_LIGHTNING_ROD: break;
    case ARTIFACT_SNAKE_RING: break;
    case ARTIFACT_ANKH: break;
    case ARTIFACT_BOOK_ELEMENTS: break;
    case ARTIFACT_ELEMENTAL_RING: break;
    case ARTIFACT_HOLY_PENDANT: break;
    case ARTIFACT_PENDANT_FREE_WILL: break;
    case ARTIFACT_PENDANT_LIFE: break;
    case ARTIFACT_SERENITY_PENDANT: break;
    case ARTIFACT_SEEING_EYE_PENDANT: break;
    case ARTIFACT_KINETIC_PENDANT: break;
    case ARTIFACT_PENDANT_DEATH: break;
    case ARTIFACT_WAND_NEGATION: break;
    case ARTIFACT_GOLDEN_BOW: break;
    case ARTIFACT_TELESCOPE: break;
    case ARTIFACT_STATESMANS_QUILL: break;
    case ARTIFACT_WIZARD_HAT: break;
    case ARTIFACT_POWER_RING: break;
    case ARTIFACT_AMMO_CART: break;
    case ARTIFACT_TAX_LIEN: break;
    case ARTIFACT_HIDEOUS_MASK: break;
    case ARTIFACT_ENDLESS_POUCH_SULFUR: break;
    case ARTIFACT_ENDLESS_VIAL_MERCURY: break;
    case ARTIFACT_ENDLESS_POUCH_GEMS: break;
    case ARTIFACT_ENDLESS_CORD_WOOD: break;
    case ARTIFACT_ENDLESS_CART_ORE: break;
    case ARTIFACT_ENDLESS_POUCH_CRYSTAL: break;
    case ARTIFACT_SPIKED_HELM:
        statChanges[HERO_PRIMARY_ATTACK] = 1;
        statChanges[HERO_PRIMARY_DEFENSE] = 1;
        break;
    case ARTIFACT_SPIKED_SHIELD:
        statChanges[HERO_PRIMARY_ATTACK] = 2;
        statChanges[HERO_PRIMARY_DEFENSE] = 2;
        break;
    case ARTIFACT_WHITE_PEARL:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 1;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 1;
        break;
    case ARTIFACT_BLACK_PEARL:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 2;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 2;
        break;
    case ARTIFACT_MAGIC_BOOK: break;
    case ARTIFACT_SPELL_SCROLL:
        break;
    case ARTIFACT_ARM_OF_MARTYR: statChanges[HERO_PRIMARY_SPELL_POWER] = 3; break;
    case ARTIFACT_BREASTPLATE_ANDURAN: statChanges[HERO_PRIMARY_DEFENSE] = 5; break;
    case ARTIFACT_BROACH_SHIELDING: statChanges[HERO_PRIMARY_SPELL_POWER] = -2; break;
    case ARTIFACT_BATTLE_GARB:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 5;
        statChanges[HERO_PRIMARY_DEFENSE] = 5;
        statChanges[HERO_PRIMARY_ATTACK] = 5;
        break;
    case ARTIFACT_CRYSTAL_BALL: break;
    case ARTIFACT_HEART_FIRE: break;
    case ARTIFACT_HEART_ICE: break;
    case ARTIFACT_HELMET_ANDURAN: statChanges[HERO_PRIMARY_SPELL_POWER] = 5; break;
    case ARTIFACT_HOLY_HAMMER: statChanges[HERO_PRIMARY_ATTACK] = 5; break;
    case ARTIFACT_LEGENDARY_SCEPTER:
        statChanges[HERO_PRIMARY_SPELL_POWER] = 2;
        statChanges[HERO_PRIMARY_ATTACK] = 2;
        statChanges[HERO_PRIMARY_DEFENSE] = 2;
        statChanges[HERO_PRIMARY_KNOWLEDGE] = 2;
        break;
    case ARTIFACT_MASTHEAD: break;
    case ARTIFACT_SPHERE_NEGATION: break;
    case ARTIFACT_STAFF_WIZARDRY: statChanges[HERO_PRIMARY_SPELL_POWER] = 5; break;
    case ARTIFACT_SWORD_BREAKER:
        statChanges[HERO_PRIMARY_DEFENSE] = 4;
        statChanges[HERO_PRIMARY_ATTACK] = 1;
        break;
    case ARTIFACT_SWORD_ANDURAN: statChanges[HERO_PRIMARY_ATTACK] = 5; break;
    case ARTIFACT_SPADE_NECROMANCY: break;
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
                targetHero->m_spellPoints = static_cast<i16>(maxSpellPoints);
        }
    }
}

// @semantic
// The 0xc frame/slots, CFG, and all 11 ordered relocation sites/effective targets
// align. After relocation masking, only branch bytes +0x12/+0x13/+0x21 differ:
// retail routes the sourceHero-null arm through the destinationHero-null jump,
// while ours targets the same epilogue directly. Four bounded CFG families were
// tried: nested empty arms, positive conjunction, early returns, and shared-label
// variants; all alternatives added 5-10 bytes. Revisit only if an earlier EVENTS
// source edit changes this function's emitted CFG or branch comparison normalizes
// continuation targets. The remaining objdiff residual is pooled string identity.
VA(0x004b1973, 0x1dd)
void advManager::TransferArtifacts(hero *sourceHero, hero *destinationHero)
{
    i32 targetSlot;
    i32 sourceArtifactSlot;

    if (sourceHero != 0) {
        if (destinationHero == 0) {
        } else {
            for (targetSlot = 0; targetSlot < EVENT_ARTIFACT_SLOT_COUNT;
                 targetSlot++) {
                if (destinationHero->m_artifacts[targetSlot] == ARTIFACT_NONE) {
                    for (sourceArtifactSlot = 0;
                         sourceArtifactSlot < EVENT_ARTIFACT_SLOT_COUNT;
                         sourceArtifactSlot++) {
                        if (sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_NONE &&
                            sourceHero->m_artifacts[sourceArtifactSlot] != ARTIFACT_MAGIC_BOOK) {
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
                            sourceHero->m_artifacts[sourceArtifactSlot] = ARTIFACT_NONE;
                            sourceHero->m_artifactExtra[sourceArtifactSlot] = ARTIFACT_NONE;
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
// @early-stop-reloc-only
// All 0x7f relocation-masked bytes, 50 normalized instructions, the 0x4 frame,
// slots, CFG, and six ordered relocation sites/effective targets match after an
// explicit null early return. Retail carries one nop outside the declared span.
VA(0x004b1b50, 0x7f)
void advManager::HeroLoses(hero *lostHero)
{
    if (lostHero == 0)
        return;
    {
        CompleteDraw(m_mapOriginX, m_mapOriginY, 0, 1);
        UpdateScreen(0, 0);
        lostHero->Deallocate(1);
        FizzleCenter(EVENT_FIZZLE_HERO_LOSS);
        UpdateRadar(1, 0);
        UpdateHeroLocators(1, 1);
    }
}

// @early-stop
// @early-stop-reloc-only
// All 0x132 relocation-masked bytes, 85 normalized instructions, frame/slots,
// CFG, and four ordered relocation sites/effective targets match after explicit
// early returns and restoring retail's weakest-value operand order. Objdiff only
// names the same interior fight-value address differently.
VA(0x004b1bcf, 0x132)
void advManager::DoWhirlpool(hero *eventHero)
{
    i32 armyValue;
    i32 weakestValue;
    i32 selectedSlot;
    i32 slotIndex;
    i32 groupValues[COMBAT_MONSTER_ARMY_SLOTS];

    if (gbHumanPlayer[eventHero->m_owner] == 0)
        return;
    if (Random(EVENT_WHIRLPOOL_TRIGGER_ROLL, EVENT_WHIRLPOOL_TRIGGER_MAX) !=
        EVENT_WHIRLPOOL_TRIGGER_ROLL)
        return;
    {
            weakestValue = EVENT_WHIRLPOOL_ARMY_VALUE_LIMIT;
            selectedSlot = -1;
            for (slotIndex = 0; slotIndex < COMBAT_MONSTER_ARMY_SLOTS; slotIndex++) {
                if (eventHero->m_army.m_creatureCounts[slotIndex] > 0) {
                    armyValue =
                        gMonsterDatabase[eventHero->m_army.m_creatureTypes[slotIndex]].fightValue *
                        eventHero->m_army.m_creatureCounts[slotIndex];
                    if (armyValue < OD_STEER(weakestValue)) {
                        weakestValue = armyValue;
                        selectedSlot = slotIndex;
                    }
                }
            }
            if (eventHero->m_army.GetNumArmies() > 1) {
                eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
                if (eventHero->m_army.m_creatureCounts[selectedSlot] == 0)
                    eventHero->m_army.m_creatureTypes[selectedSlot] = CREATURE_NONE;
            } else if (eventHero->m_army.m_creatureCounts[selectedSlot] > 1) {
                eventHero->m_army.m_creatureCounts[selectedSlot] >>= 1;
            }
    }
}

// @early-stop
// @early-stop-reloc-only
// All 0x142 relocation-masked bytes, normalized instructions, the 0x1c frame,
// retail slots, CFG, and 22 ordered relocation sites/effective targets match
// after an explicit hidden-state early return and hash-derived local renaming.
// Remaining objdiff noise is pooled literal/interior aggregate identity.
VA(0x004b1d01, 0x142)
void advManager::FizzleCenter(i32 fizzleType)
{
    SAMPLE2 playedSample;
    i32 fizzleStepCount;

    if (bShowIt == 0)
        return;
    {
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
        fizzleStepCount = EVENT_FIZZLE_STEPS;
        gpWindowManager->FizzleForward(EVENT_FIZZLE_X, EVENT_FIZZLE_Y,
                                       EVENT_FIZZLE_WIDTH, EVENT_FIZZLE_HEIGHT,
                                       fizzleStepCount, 0, 0);
        gpMouseManager->ShowColorPointer();
        WaitEndSample(playedSample, -1);
    }
}

// @semantic
// Complete body and CFG with the retail 0x150 frame, four jump tables, and
// 356/356 relocation sites. All event-flag updates match retail load/op/store
// form via static_cast<int>, and the mine-owner and combat-result early breaks
// now match retail polarity and block order. First non-table divergence is
// +0x53c: for (metadata * 4 - 4) * 125 retail emits lea [4*eax-4] then
// three multiply-by-five LEAs, while base emits shl eax,2, folds -20 into the
// first LEA, then emits two more LEAs. Commuted/parenthesized AST spellings and
// 30 bounded TU-state probes did not fix it. A 137-site member-access sweep only
// improved the score by changing the proven frame to 0x154 and was rejected.
// GetHero/GetTown now reproduce the retail /Ob1 continuation jumps at +0x1b26
// and +0x1bb1. Revisit the multiply shape only with a new structural lead.
VA(0x004b1e43, 0x2a40)
void advManager::DoAIEvent(mapCell *cell, hero *eventHero, i32 x, i32 y)
{
    float battleStatValue_o;
    float spellValueFactor_i;
    u32 resourceAmount_o;
    i32 unusedEventResult_e;
    i32 unusedEventValue_o;
    mapCell *exitCell_d;
    i32 teleportDistance_o;
    i32 spell_g;
    u32 guardianCount_i;
    float attackerLoss_c;
    float defenderLoss_k;
    i32 levelExperience_g;
    i32 heroLevel_e;
    i32 creatureCosts_a[AI_EVENT_RESOURCE_COUNT];
    i32 spellPower_j;
    i32 adjacentMonster_j;
    i32 artifactResource_p;
    hero *otherHero_e;
    i32 artifactGuardResult_e;
    i32 heroCombatResult_h;
    i32 exitY_d;
    i32 artifact_g;
    i32 heroInteractionResult;
    i32 resourceType_a;
    i32 exitX;
    i32 exitCount;
    mapEventExtra *eventExtra_o;
    u32 artifactGuardCount_b;
    i32 survivingCount_a;
    boatRecord *boat_k;
    i32 savedShowIt_e;
    i32 mineId_j;
    i32 rewardWork_e[AI_EVENT_ARMY_STACK_COUNT];
    i32 resourceWork_p[AI_EVENT_RESOURCE_COUNT];
    i32 eventResults[3];
    i32 abandonedMineValue_f;
    i32 index_h;
    town *occupiedTown_b;
    i32 combatResult_d;
    i32 eventWork_o[AI_EVENT_RESOURCE_COUNT];
    i32 battleWon_j;
    i32 pyramidBattleValue_l;
    i32 oldPlayer_o;
    i32 eventType_g;
    playerData *oldPlayerData_h;
    i32 battleResult_l;
    i32 purchaseCost_i;
    i32 purchaseValue_a;
    i32 creatureType_i;
    i32 creatureFlag_l;

    occupiedTown_b = 0;
    eventType_g = cell->m_triggerType & MAP_TRIGGER_TYPE_MASK;
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
    case MAP_OBJECT_COAST:
        if (eventHero->m_eventFlags & HERO_EVENT_EMBARKED) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) & ~HERO_EVENT_EMBARKED;
            eventHero->m_remainingMobility = 0;
            eventHero->m_direction = static_cast<u8>(m_cursorDirection);
            m_cursorType = eventHero->m_cursorType;
            m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
            m_cursorActive = 1;
            CheckAdjacentMon(&adjacentMonster_j);
        }
        break;

    case MAP_OBJECT_BOAT:
        boat_k = &gpGame->m_boats[cell->m_objectMetadata];
        gpGame->RestoreCell(-1, -1, boat_k->savedTriggerType,
                            boat_k->savedEventData, cell,
                            AI_EVENT_BOAT_RESTORE_MODE);
        eventHero->m_eventFlags =
            static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_EMBARKED;
        eventHero->m_remainingMobility = 0;
        boat_k->heroId = eventHero->m_id;
        boat_k->owner = eventHero->m_owner;
        m_cursorType = AI_EVENT_BOAT_CURSOR;
        m_cursorDirection = boat_k->direction;
        m_cursorFrame = GetCursorBaseFrame(m_cursorDirection);
        m_cursorActive = 1;
        break;

    case MAP_OBJECT_ALCHEMIST_LAB:
    case MAP_OBJECT_MINE:
    case MAP_OBJECT_SAWMILL:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer)
            break;
        if (gpGame->m_mines[cell->m_objectMetadata].guardianType != CREATURE_NONE) {
            index_h = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
            combatResult_d = gpPhilAI->CombatMonsterEvent(
                eventHero, gpGame->m_mines[cell->m_objectMetadata].guardianType,
                &index_h, cell);
            if (combatResult_d == 0)
                break;
            gpGame->m_mines[cell->m_objectMetadata].guardianType =
                CREATURE_NONE;
            gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
            eventHero->CheckLevel();
        }
        gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
        if (eventType_g == MAP_OBJECT_MINE) {
            for (index_h = SPELL_SET_EARTH_GUARDIAN;
                 index_h < SPELL_SET_WATER_GUARDIAN; ++index_h) {
                if (eventHero->HasSpell(index_h) &&
                    GetManaCost(index_h, eventHero) < eventHero->m_spellPoints) {
                    eventHero->m_spellPoints = static_cast<i16>(
                        eventHero->m_spellPoints -
                        GetManaCost(index_h, eventHero));
                    gpGame->m_mines[cell->m_objectMetadata].guardianType =
                        static_cast<i8>(index_h + 1);
                    spellPower_j = eventHero->Stats(HERO_PRIMARY_SPELL_POWER);
                    if (spellPower_j > AI_EVENT_MINE_SPELL_POWER_MAX)
                        spellPower_j = AI_EVENT_MINE_SPELL_POWER_MAX;
                    gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                        static_cast<u8>(
                            spellPower_j * AI_EVENT_MINE_SPELL_COUNT_SCALE);
                    index_h = AI_EVENT_MINE_SPELL_LOOP_END;
                }
            }
        }
        break;

    case MAP_OBJECT_LIGHTHOUSE:
        if (gpGame->m_mineOwners[cell->m_objectMetadata] == giCurPlayer) {
        } else {
            gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
        }
        break;

    case MAP_OBJECT_TREASURE_CHEST:
        if (cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) {
            if (eventHero->NumArtifacts() >= AI_EVENT_ARTIFACT_LIMIT) {
                cell->m_objectMetadata = CHEST_GOLD_ONLY;
                goto chestGoldOrExperience;
            }
            GiveArtifact(eventHero, cell->m_objectMetadata & CHEST_ARTIFACT_MASK, 1, -1);
        } else {
chestGoldOrExperience:
            if (gpPhilAI->ChooseGoldOrExperience(
                    cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER,
                    (cell->m_objectMetadata * 4 - 4) * CHEST_EXPERIENCE_MULTIPLIER)) {
                GiveResource(eventHero, RES_GOLD,
                             cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER);
            } else {
                GiveExperience(eventHero,
                               (cell->m_objectMetadata * 4 - 4) *
                                   CHEST_EXPERIENCE_MULTIPLIER,
                               1);
                eventHero->CheckLevel();
            }
        }
        eventResults[0] = 1;
        break;

    case MAP_OBJECT_WATERING_HOLE:
        if ((eventHero->m_eventFlags & HERO_EVENT_WATERING_HOLE) == 0) {
            eventHero->m_mobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero->m_remainingMobility += WATERING_HOLE_MOBILITY_BONUS;
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) |
                HERO_EVENT_WATERING_HOLE;
            ++eventHero->m_morale;
        }
        break;

    case MAP_OBJECT_BUOY:
        if ((eventHero->m_eventFlags & HERO_EVENT_BUOY) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_BUOY;
            ++eventHero->m_morale;
        }
        break;

    case MAP_OBJECT_FAERIE_RING:
        if ((eventHero->m_eventFlags & HERO_EVENT_FAERIE_RING) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) |
                HERO_EVENT_FAERIE_RING;
            ++eventHero->m_luck;
        }
        break;

    case MAP_OBJECT_IDOL:
        if ((eventHero->m_eventFlags & HERO_EVENT_IDOL) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_IDOL;
            ++eventHero->m_luck;
        }
        break;

    case MAP_OBJECT_FOUNTAIN:
        if ((eventHero->m_eventFlags & HERO_EVENT_FOUNTAIN) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_FOUNTAIN;
            ++eventHero->m_luck;
        }
        break;

    case MAP_OBJECT_OASIS:
        if ((eventHero->m_eventFlags & HERO_EVENT_OASIS) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_OASIS;
            ++eventHero->m_morale;
            eventHero->m_mobility += OASIS_MOBILITY_BONUS;
            eventHero->m_remainingMobility += OASIS_MOBILITY_BONUS;
        }
        break;

    case MAP_OBJECT_TEMPLE:
        if ((eventHero->m_eventFlags & HERO_EVENT_TEMPLE) == 0) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) | HERO_EVENT_TEMPLE;
            eventHero->m_morale += 2;
        }
        break;

    case MAP_OBJECT_SKELETON:
        switch (cell->m_objectMetadata) {
        case SKELETON_EMPTY:
            break;
        default:
            GiveArtifact(eventHero, cell->m_objectMetadata - SKELETON_ARTIFACT_OFFSET,
                         1, -1);
            cell->m_objectMetadata = SKELETON_EMPTY;
            break;
        }
        break;

    case MAP_OBJECT_MAGIC_GARDEN:
        if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
            GiveResource(eventHero, cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET,
                         cell->m_objectMetadata - MAP_EVENT_RESOURCE_OFFSET == RES_GOLD
                             ? MAP_EVENT_GOLD_AMOUNT
                             : MAP_EVENT_RESOURCE_AMOUNT);
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_OBJECT_LEAN_TO:
        if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
            GiveResource(eventHero,
                         (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) - 1,
                         (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK) >>
                             ARTIFACT_EVENT_RESOURCE_SHIFT);
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_OBJECT_WAGON:
        if (cell->m_objectMetadata != MAP_EVENT_DATA_EMPTY) {
            if (cell->m_objectMetadata & WAGON_ARTIFACT_FLAG) {
                if (eventHero->NumArtifacts() != AI_EVENT_ARTIFACT_LIMIT)
                    GiveArtifact(eventHero,
                                 cell->m_objectMetadata & WAGON_ARTIFACT_MASK, 1, -1);
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            } else {
                GiveResource(eventHero,
                             (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) - 1,
                             (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK) >>
                                 ARTIFACT_EVENT_RESOURCE_SHIFT);
                cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            }
        }
        break;

    case MAP_OBJECT_SEA_CHEST:
        if ((cell->m_objectMetadata & CHEST_ARTIFACT_FLAG) != 0 &&
            eventHero->NumArtifacts() < AI_EVENT_ARTIFACT_LIMIT) {
            GiveArtifact(eventHero, cell->m_objectMetadata & CHEST_ARTIFACT_MASK, 1, -1);
            GiveResource(eventHero, RES_GOLD,
                         AI_EVENT_SEA_CHEST_ARTIFACT_GOLD);
        } else if (cell->m_objectMetadata != 0) {
            GiveResource(eventHero, RES_GOLD, AI_EVENT_SEA_CHEST_GOLD);
        }
        eventResults[0] = 1;
        break;

    case MAP_OBJECT_FLOTSAM:
        switch (cell->m_objectMetadata) {
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

    case MAP_OBJECT_CAMPFIRE:
        GiveResource(eventHero, RES_GOLD,
                     (cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT) *
                         CAMPFIRE_GOLD_MULTIPLIER);
        GiveResource(eventHero, cell->m_objectMetadata & CAMPFIRE_RESOURCE_MASK,
                     cell->m_objectMetadata >> CAMPFIRE_AMOUNT_SHIFT);
        eventResults[0] = 1;
        break;

    case MAP_OBJECT_FORT:
        if ((eventHero->m_fortVisits & (1U << cell->m_objectMetadata)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE];
            eventHero->m_fortVisits |= 1U << cell->m_objectMetadata;
        }
        break;

    case MAP_OBJECT_XANADU:
        if ((eventHero->m_xanaduVisits & (1U << cell->m_objectMetadata)) == 0 &&
            eventHero->m_level +
                    eventHero->m_secondarySkills[HERO_SKILL_DIPLOMACY] * 2 >=
                XANADU_ADMISSION_LEVEL) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_ATTACK];
            ++eventHero->m_primaryStats[HERO_PRIMARY_DEFENSE];
            ++eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE];
            ++eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER];
            eventHero->m_xanaduVisits |= 1U << cell->m_objectMetadata;
        }
        break;

    case MAP_OBJECT_STANDING_STONES:
        if ((eventHero->m_standingStoneVisits & (1U << cell->m_objectMetadata)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_SPELL_POWER];
            eventHero->m_standingStoneVisits |= 1U << cell->m_objectMetadata;
        }
        break;

    case MAP_OBJECT_WITCH_DOCTOR_HUT:
        if ((eventHero->m_witchDoctorVisits & (1U << cell->m_objectMetadata)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_KNOWLEDGE];
            eventHero->m_witchDoctorVisits |= 1U << cell->m_objectMetadata;
        }
        break;

    case MAP_OBJECT_MERCENARY_CAMP:
        if ((eventHero->m_mercenaryCampVisits & (1U << cell->m_objectMetadata)) == 0) {
            ++eventHero->m_primaryStats[HERO_PRIMARY_ATTACK];
            eventHero->m_mercenaryCampVisits |= 1U << cell->m_objectMetadata;
        }
        break;

    case MAP_OBJECT_GAZEBO:
        if ((eventHero->m_gazeboVisits & (1U << cell->m_objectMetadata)) == 0) {
            GiveExperience(eventHero, GAZEBO_EXPERIENCE, 1);
            eventHero->m_gazeboVisits |= 1U << cell->m_objectMetadata;
            eventHero->CheckLevel();
        }
        break;

    case MAP_OBJECT_WATER_WHEEL:
        if (cell->m_objectMetadata != 0) {
            GiveResource(eventHero, RES_GOLD,
                         cell->m_objectMetadata * CHEST_GOLD_MULTIPLIER);
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
        }
        break;

    case MAP_OBJECT_RESOURCE:
        resourceType_a = cell->m_objectIndex >> 1;
        resourceAmount_o = resourceType_a == RES_GOLD
            ? cell->m_objectMetadata * CAMPFIRE_GOLD_MULTIPLIER
            : cell->m_objectMetadata;
        GiveResource(eventHero, resourceType_a, resourceAmount_o);
        eventResults[0] = 1;
        break;

    case MAP_OBJECT_WINDMILL:
        if (cell->m_objectMetadata != AI_EVENT_WINDMILL_EMPTY) {
            GiveResource(eventHero, cell->m_objectMetadata, WINDMILL_RESOURCE_AMOUNT);
            cell->m_objectMetadata = AI_EVENT_WINDMILL_EMPTY;
        }
        break;

    case MAP_OBJECT_HILL_FORT:
        eventHero->UpgradeCreatures(CREATURE_OGRE, CREATURE_OGRE_LORD);
        eventHero->UpgradeCreatures(CREATURE_ORC, CREATURE_ORC_CHIEF);
        eventHero->UpgradeCreatures(CREATURE_DWARF,
                                    CREATURE_BATTLE_DWARF);
        break;

    case MAP_OBJECT_FREEMANS_FOUNDRY:
        eventHero->UpgradeCreatures(CREATURE_IRON_GOLEM,
                                    CREATURE_STEEL_GOLEM);
        eventHero->UpgradeCreatures(CREATURE_PIKEMAN,
                                    CREATURE_VETERAN_PIKEMAN);
        eventHero->UpgradeCreatures(CREATURE_SWORDSMAN,
                                    CREATURE_MASTER_SWORDSMAN);
        break;

    case MAP_OBJECT_TREE_CITY:
        creatureType_i = CREATURE_SPRITE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_RUINS:
        creatureType_i = CREATURE_MEDUSA;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_TROLL_BRIDGE:
        if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = CREATURE_TROLL;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_CITY_OF_DEAD:
        if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = CREATURE_POWER_LICH;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_DRAGON_CITY:
        if (cell->m_objectMetadata & DWELLING_GUARDED_FLAG)
            break;
        creatureType_i = CREATURE_RED_DRAGON;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_HALFLING_HOLE:
        creatureType_i = CREATURE_HALFLING;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_ANCIENT_LAMP:
        creatureType_i = CREATURE_GENIE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_WAGON_CAMP:
        creatureType_i = CREATURE_ROGUE;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_DESERT_TENT:
        creatureType_i = CREATURE_NOMAD;
        creatureFlag_l = 0;
        goto creaturePurchase;
    case MAP_OBJECT_WATCH_TOWER:
        creatureType_i = CREATURE_ORC;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_TREE_HOUSE:
        creatureType_i = CREATURE_SPRITE;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_ARCHER_HOUSE:
        creatureType_i = CREATURE_ARCHER;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_GOBLIN_HUT:
        creatureType_i = CREATURE_GOBLIN;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_PEASANT_HUT:
        creatureType_i = CREATURE_PEASANT;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_DWARF_COTTAGE:
    case MAP_OBJECT_SIRENS:
        creatureType_i = CREATURE_DWARF;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_CAVE:
        creatureType_i = CREATURE_CENTAUR;
        creatureFlag_l = 1;
        goto creaturePurchase;
    case MAP_OBJECT_EXCAVATION:
        creatureType_i = CREATURE_SKELETON;
        creatureFlag_l = 1;
creaturePurchase:
        if (cell->m_objectMetadata != 0) {
            gpPhilAI->EvaluateOneTimeCreaturePurchase(
                creatureType_i, cell->m_objectMetadata, creatureFlag_l, eventResults[2],
                purchaseCost_i, purchaseValue_a);
            if (eventResults[2] > 0) {
                gpGame->GiveArmy(&eventHero->m_army, creatureType_i,
                                 eventResults[2], purchaseValue_a);
                cell->m_objectData = static_cast<u16>(
                    (cell->m_objectMetadata - eventResults[2]) * 8 |
                    (cell->m_objectData & 7));
                if (creatureFlag_l == 0) {
                    GetMonsterCost(creatureType_i, creatureCosts_a);
                    for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h)
                        gpCurPlayer->m_resources[index_h] -=
                            creatureCosts_a[index_h] * eventResults[2];
                }
            }
        }
        if (cell->m_objectMetadata == 0 && eventType_g == MAP_OBJECT_ANCIENT_LAMP)
            eventResults[0] = 1;
        break;

    case MAP_OBJECT_MONSTER:
        ComputerMonsterInteract(cell, eventHero, eventResults);
        break;

    case MAP_OBJECT_TREE_OF_KNOWLEDGE:
        if ((eventHero->m_treeKnowledgeVisits &
             (1U << (cell->m_objectMetadata & 31))) == 0) {
            heroLevel_e = eventHero->GetLevel(eventHero->m_experience);
            levelExperience_g = eventHero->GetExperience(heroLevel_e + 1) -
                              eventHero->GetExperience(heroLevel_e);
            switch (cell->m_objectMetadata >> TREE_KNOWLEDGE_MODE_SHIFT) {
            case TREE_KNOWLEDGE_FREE:
                GiveExperience(eventHero, levelExperience_g, 1);
                eventHero->m_treeKnowledgeVisits |=
                    1U << (cell->m_objectMetadata & 31);
                eventHero->CheckLevel();
                break;
            case TREE_KNOWLEDGE_GOLD:
                if (gpCurPlayer->m_resources[RES_GOLD] >
                    AI_EVENT_TREE_GOLD) {
                    gpCurPlayer->m_resources[RES_GOLD] -= AI_EVENT_TREE_GOLD;
                    GiveExperience(eventHero, levelExperience_g, 1);
                    eventHero->m_treeKnowledgeVisits |=
                        1U << (cell->m_objectMetadata & 31);
                    eventHero->CheckLevel();
                }
                break;
            case TREE_KNOWLEDGE_GEMS:
                if (gpCurPlayer->m_resources[RES_GEMS] >
                    AI_EVENT_TREE_GEMS) {
                    gpCurPlayer->m_resources[RES_GEMS] -= AI_EVENT_TREE_GEMS;
                    GiveExperience(eventHero, levelExperience_g, 1);
                    eventHero->m_treeKnowledgeVisits |=
                        1U << (cell->m_objectMetadata & 31);
                    eventHero->CheckLevel();
                }
                break;
            }
        }
        break;

    case MAP_OBJECT_OBELISK:
        if ((giCurPlayerBit &
             gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1]) == 0) {
            gpGame->m_obeliskVisitors[cell->m_objectMetadata - 1] |= giCurPlayerBit;
            ComputeUALoc(giCurPlayer);
        }
        break;

    case MAP_OBJECT_ORACLE:
        break;

    case MAP_OBJECT_SHRINE_FIRST_CIRCLE:
    case MAP_OBJECT_SHRINE_SECOND_CIRCLE:
    case MAP_OBJECT_SHRINE_THIRD_CIRCLE:
        if (eventHero->HasArtifact(ARTIFACT_MAGIC_BOOK) &&
            gsSpellInfo[cell->m_objectMetadata - 1].level <=
                eventHero->m_secondarySkills[HERO_SKILL_WISDOM] + 2) {
            eventHero->AddSpell(cell->m_objectMetadata - 1,
                                eventHero->Stats(HERO_PRIMARY_KNOWLEDGE));
        }
        break;

    case MAP_OBJECT_CASTLE:
        gpPhilAI->TownEvent(cell, eventHero, x, y);
        break;

    case MAP_OBJECT_WHIRLPOOL:
        DoWhirlpool(eventHero);
        goto teleportEvent;
    case MAP_OBJECT_STONE_LITHS:
teleportEvent:
        exitCount = 0;
        for (exitY_d = 0; MAP_HEIGHT > exitY_d; ++exitY_d) {
            for (exitX = 0; MAP_WIDTH > exitX; ++exitX) {
                if (gpGame->m_worldMap.Row(exitY_d)[exitX].m_triggerType ==
                        static_cast<u8>(eventType_g |
                                                   MAP_TRIGGER_ACTION_FLAG) &&
                    gpGame->m_worldMap.Row(exitY_d)[exitX].m_objectIndex ==
                        cell->m_objectIndex &&
                    abs(exitY_d - y) + abs(exitX - x) >
                        (eventType_g == MAP_OBJECT_STONE_LITHS
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
                    if (gpGame->m_worldMap.Row(exitY_d)[exitX].m_triggerType ==
                            static_cast<u8>(eventType_g |
                                                       MAP_TRIGGER_ACTION_FLAG) &&
                        gpGame->m_worldMap.Row(exitY_d)[exitX].m_objectIndex ==
                            cell->m_objectIndex &&
                        abs(exitY_d - y) + abs(exitX - x) >
                            (eventType_g == MAP_OBJECT_STONE_LITHS
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

    case MAP_OBJECT_ARTIFACT:
        artifactResource_p = (cell->m_objectMetadata & ARTIFACT_EVENT_RESOURCE_MASK) >>
                           ARTIFACT_EVENT_RESOURCE_SHIFT;
        artifact_g = cell->m_objectIndex >> 1;
        artifactGuardCount_b = cell->m_objectMetadata & ARTIFACT_EVENT_MONSTER_MASK;
        if (eventHero->NumArtifacts() == AI_EVENT_ARTIFACT_LIMIT)
            break;
        if (artifact_g == ARTIFACT_SPELL_SCROLL) {
            GiveArtifact(eventHero, artifact_g, 1,
                         static_cast<i8>(cell->m_objectMetadata));
            eventResults[0] = 1;
            break;
        }
        if (cell->m_objectMetadata & ARTIFACT_EVENT_GUARDED_FLAG) {
            if (artifactGuardCount_b == CREATURE_ROGUE) {
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
        switch (cell->m_objectMetadata & ARTIFACT_EVENT_MODE_MASK) {
        case ARTIFACT_EVENT_MODE_PICKUP:
artifactPickup:
            for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h) {
                if (gpCurPlayer->m_resources[index_h] < 0)
                    gpCurPlayer->m_resources[index_h] = 0;
            }
            GiveArtifact(eventHero, artifact_g, 1, -1);
            eventResults[0] = 1;
            break;
        case ARTIFACT_EVENT_MODE_GOLD:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_GOLD, 0, 0)) {
                gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_GOLD;
                goto artifactPickup;
            }
            break;
        case ARTIFACT_EVENT_MODE_WISDOM:
            if (eventHero->m_secondarySkills[HERO_SKILL_WISDOM] != 0)
                goto artifactPickup;
            break;
        case ARTIFACT_EVENT_MODE_LEADERSHIP:
            if (eventHero->m_secondarySkills[HERO_SKILL_LEADERSHIP] != 0)
                goto artifactPickup;
            break;
        case ARTIFACT_EVENT_MODE_RESOURCE_3:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_RESOURCE_3_GOLD,
                    artifactResource_p, AI_EVENT_ARTIFACT_RESOURCE_3)) {
                gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_RESOURCE_3_GOLD;
                gpGame->m_players[eventHero->m_owner]
                    .m_resources[artifactResource_p] -=
                    AI_EVENT_ARTIFACT_RESOURCE_3;
                goto artifactPickup;
            }
            break;
        case ARTIFACT_EVENT_MODE_RESOURCE_5:
            if (gpPhilAI->NetValueOfArtifact(
                    artifact_g, AI_EVENT_ARTIFACT_RESOURCE_5_GOLD,
                    artifactResource_p, AI_EVENT_ARTIFACT_RESOURCE_5)) {
                gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -=
                    AI_EVENT_ARTIFACT_RESOURCE_5_GOLD;
                gpGame->m_players[eventHero->m_owner]
                    .m_resources[artifactResource_p] -=
                    AI_EVENT_ARTIFACT_RESOURCE_5;
                goto artifactPickup;
            }
            break;
        }
        break;

    case MAP_OBJECT_HERO_INTERACTION:
        otherHero_e = gpGame->GetHero(cell->m_objectMetadata);
        savedShowIt_e = bShowIt;
        if (otherHero_e->m_owner == giCurPlayer) {
            gpPhilAI->HeroInteractionAtHero(eventHero, otherHero_e, 0,
                                            &heroInteractionResult);
            return;
        }
        if (otherHero_e->m_locationType == AI_EVENT_HERO_TOWN_LOCATION)
            occupiedTown_b = gpGame->GetTown(otherHero_e->m_occupiedTown);

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

    case MAP_OBJECT_SIGN:
    case MAP_OBJECT_BOTTLE:
        break;

    case MAP_OBJECT_DAEMON_CAVE:
        switch (cell->m_objectMetadata) {
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
            if (gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] >=
                AI_EVENT_DAEMON_GOLD) {
                if (gpPhilAI->ChooseToPayRansomOnHero(AI_EVENT_DAEMON_GOLD)) {
                    gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -=
                        AI_EVENT_DAEMON_GOLD;
                } else {
                    HeroLoses(eventHero);
                }
            } else {
                HeroLoses(eventHero);
            }
            break;
        }
        cell->m_objectMetadata = DAEMON_CAVE_EMPTY;
        break;

    case MAP_OBJECT_PYRAMID:
        if (cell->m_objectMetadata != 0 && eventHero->HasSpell(cell->m_objectMetadata - 1) == 0) {
            for (index_h = 0; index_h < AI_EVENT_ARMY_STACK_COUNT; ++index_h) {
                gpMonGroup->m_creatureTypes[index_h] =
                    CREATURE_ROYAL_MUMMY;
                gpMonGroup->m_creatureCounts[index_h] = 10;
            }
            index_h = cell->m_objectMetadata - 1;
            if (gsSpellInfo[index_h].attributes & 1) {
                battleStatValue_o =
                    eventHero->Stats(HERO_PRIMARY_SPELL_POWER) > 40
                        ? gfBattleStat[40]
                        : gfBattleStat[eventHero->Stats(
                              HERO_PRIMARY_SPELL_POWER)];
                spellValueFactor_i = battleStatValue_o;
            } else {
                spellValueFactor_i = 1.0f;
            }
            pyramidBattleValue_l = static_cast<i32>(
                gsSpellInfo[index_h].aiValue *
                gpCurPlayer->m_upgradeValueWeight * spellValueFactor_i);
            gpPhilAI->ChooseEvaluateBattle(
                &eventHero->m_army, eventHero, gpMonGroup, 0, 0, 0,
                pyramidBattleValue_l, battleWon_j, battleResult_l);
            if (battleWon_j == 0) {
            } else {
                index_h = PYRAMID_GUARD_COUNT;
                combatResult_d = gpPhilAI->CombatMonsterEvent(
                    eventHero, CREATURE_ROYAL_MUMMY,
                    &index_h, cell);
                if (combatResult_d == 0) {
                } else {
                    eventHero->AddSpell(
                        cell->m_objectMetadata - 1,
                        eventHero->Stats(HERO_PRIMARY_KNOWLEDGE));
                    cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
                }
            }
        }
        break;

    case MAP_OBJECT_GRAVEYARD:
    case MAP_OBJECT_SHIPWRECK:
    case MAP_OBJECT_DERELICT_SHIP:
        gpPhilAI->FightEvent(eventHero, cell, 0);
        break;

    case MAP_OBJECT_ABANDONED_MINE:
        for (index_h = 0; index_h < AI_EVENT_ARMY_STACK_COUNT; ++index_h) {
            gpMonGroup->m_creatureTypes[index_h] =
                gpGame->m_mines[cell->m_objectMetadata].guardianType;
            gpMonGroup->m_creatureCounts[index_h] = static_cast<i16>(
                gpGame->m_mines[cell->m_objectMetadata].guardianCount /
                AI_EVENT_ABANDONED_MINE_ARMY_DIVISOR);
        }
        abandonedMineValue_f = static_cast<i32>(
            static_cast<float>(gaiTurnValueOfMine[y * MAP_WIDTH + x]) *
            gMineCharacteristics[RES_GOLD] *
            gafAITurnCostResource[RES_GOLD]);
        gpPhilAI->ChooseEvaluateBattle(
            &eventHero->m_army, eventHero, gpMonGroup, 0, 0, 0,
            abandonedMineValue_f, battleWon_j, battleResult_l);
        if (battleWon_j != 0) {
            survivingCount_a = gpGame->m_mines[cell->m_objectMetadata].guardianCount;
            combatResult_d = gpPhilAI->CombatMonsterEvent(
                eventHero, gpGame->m_mines[cell->m_objectMetadata].guardianType,
                &survivingCount_a, cell);
            if (survivingCount_a > AI_EVENT_GUARD_COUNT_MAX)
                survivingCount_a = AI_EVENT_GUARD_COUNT_MAX;
            gpGame->m_mines[cell->m_objectMetadata].guardianCount =
                static_cast<u8>(survivingCount_a);
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
                gpGame->m_mines[cell->m_objectMetadata].resourceType = RES_GOLD;
                gpGame->m_mines[cell->m_objectMetadata].guardianType = CREATURE_NONE;
                gpGame->m_mines[cell->m_objectMetadata].guardianCount = 0;
                gpGame->ClaimMine(cell->m_objectMetadata, giCurPlayer);
            }
        }
        break;

    case MAP_OBJECT_TAR_PIT:
        break;

    case MAP_OBJECT_OBSERVATION_TOWER:
        gpGame->SetVisibility(x, y, giCurPlayer,
                              AI_EVENT_OBSERVATION_RADIUS);
        break;

    case MAP_OBJECT_SHIPWRECK_SURVIVOR:
        if (eventHero->NumArtifacts() < AI_EVENT_ARTIFACT_LIMIT)
            GiveArtifact(eventHero, cell->m_objectMetadata, 1, -1);
        eventResults[0] = 1;
        break;

    case MAP_OBJECT_ARTESIAN_SPRING:
        if (cell->m_objectMetadata != 0) {
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
            if (eventHero->m_spellPoints < spellPower_j * 20)
                eventHero->m_spellPoints = static_cast<i16>(spellPower_j * 20);
        }
        break;

    case MAP_OBJECT_MAGIC_WELL:
        if ((eventHero->m_eventFlags & HERO_EVENT_MAGIC_WELL) == 0) {
            cell->m_objectMetadata = MAP_EVENT_DATA_EMPTY;
            spellPower_j = eventHero->Stats(HERO_PRIMARY_KNOWLEDGE);
            if (eventHero->m_spellPoints < spellPower_j * 10) {
                eventHero->m_eventFlags =
                    static_cast<i32>(eventHero->m_eventFlags) |
                    HERO_EVENT_MAGIC_WELL;
                eventHero->m_spellPoints = static_cast<i16>(spellPower_j * 10);
            }
        }
        break;

    case MAP_OBJECT_WITCH_HUT:
        if (eventHero->m_secondarySkills[cell->m_objectMetadata] == 0)
            eventHero->GiveSS(cell->m_objectMetadata, HERO_SKILL_LEVEL_BASIC);
        break;

    case MAP_OBJECT_MAGELLAN_MAPS:
        break;

    case MAP_OBJECT_SPHINX:
        eventExtra_o = reinterpret_cast<mapEventExtra *>(ppMapExtra[cell->m_objectMetadata]);
        if (eventExtra_o->active != 0) {
            if (Random(0, AI_EVENT_RANDOM_PERCENT_MAX) <
                AI_EVENT_RANDOM_EVENT_SUCCESS) {
                for (index_h = 0; index_h < AI_EVENT_RESOURCE_COUNT; ++index_h) {
                    gpGame->m_players[giCurPlayer].m_resources[index_h] +=
                        eventExtra_o->resources[index_h];
                    if (gpGame->m_players[giCurPlayer].m_resources[index_h] < 0)
                        gpGame->m_players[giCurPlayer].m_resources[index_h] = 0;
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

    case MAP_OBJECT_TRADING_POST:
        break;

    case MAP_OBJECT_BARRIER:
        eventResults[0] = BarrierAIEvent(cell, eventHero);
        break;
    case MAP_OBJECT_TRAVELER_TENT:
        PasswordAIEvent(cell, eventHero);
        break;
    case MAP_OBJECT_EXPANSION_OBJECT:
        GenericSiteAIEvent(cell, eventHero);
        break;
    case MAP_OBJECT_EXPANSION_DWELLING:
        RecruitSiteAIEvent(cell, eventHero);
        break;
    case MAP_OBJECT_JAIL:
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
i32 advManager::BarrierAIEvent(mapCell *cell, hero *)
{
    i32 unusedBarrier15[5];
    i32 color = cell->m_objectMetadata;
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
    i32 unusedPassword6[1];
    i32 color = cell->m_objectMetadata;
    color &= AI_EVENT_BARRIER_COLOR_MASK;
    gpCurPlayer->m_barrierTents |= (1 << color);
}

// @semantic
// Complete 0x369 body, 0x44 frame/slots, CFG, and all 25 ordered relocation
// sites/effective targets align. After masking those relocations, the only raw
// differences are +0x197/+0x19a: retail loads primaryStat16 before eventHero for
// the primary-stat increment, while ours loads eventHero first. Prefix/postfix,
// OD_STEER(index), and commuted subscript spellings were byte-neutral; an explicit
// read/add/write grew the body by 12 bytes. Revisit only if the hero primary-stat
// representation/accessor changes, an earlier EVENTS source edit changes emitted
// evaluation order, or comparison gains a proved commutative-load normalization.
// Ten bounded TU-state declaration families produced no canonical exact closure.
VA(0x004b493b, 0x369)
void advManager::GenericSiteAIEvent(mapCell *cell, hero *eventHero)
{
    i32 unusedPair27[2];
    i32 artifactIndex1;
    i32 siteLevel5;
    i32 primaryStat16;
    i32 siteType3;
    i32 unusedTriple26[3];
    i32 cursedArtifactCount5;
    i32 creatureType3;
    i32 quantity6;
    i32 armyValue7;

    cursedArtifactCount5 = 0;
    siteType3 = cell->m_objectMetadata;
    siteType3 &= AI_EVENT_SITE_TYPE_MASK;
    siteLevel5 = cell->m_objectMetadata;
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
                    eventHero->m_artifacts[artifactIndex1] = ARTIFACT_NONE;
            }
            gpCurPlayer->m_resources[RES_GOLD] -= AI_EVENT_CURSED_ARTIFACT_COST;
        }
        break;
    case AI_GENERIC_SITE_SHIPWRECK:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_SHIPWRECK_FLAG)) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_SHIPWRECK_FLAG;
            switch (eventHero->m_cursorType) {
            case FACTION_SORCERESS:
            case FACTION_WARLOCK:
            case FACTION_WIZARD:
            case FACTION_NECROMANCER:
                primaryStat16 = HERO_PRIMARY_SPELL_POWER;
                break;
            case FACTION_BARBARIAN:
                primaryStat16 = HERO_PRIMARY_ATTACK;
                break;
            case FACTION_KNIGHT:
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
                static_cast<i32>(eventHero->m_eventFlags) |
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
                if (creatureType3 != CREATURE_NONE) {
                    quantity6 = eventHero->m_army.m_quantities[artifactIndex1];
                    if (quantity6 > AI_EVENT_GRAVEYARD_ARMY_REMAINDER) {
                        eventHero->m_army.m_quantities[artifactIndex1] =
                            static_cast<i16>(
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
                static_cast<i32>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_GRAVEYARD_FLAG;
        }
        break;
    case AI_GENERIC_SITE_CREATURE_UPGRADE:
        if (!(eventHero->m_eventFlags & AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG)) {
            eventHero->m_eventFlags =
                static_cast<i32>(eventHero->m_eventFlags) |
                AI_GENERIC_SITE_CREATURE_UPGRADE_FLAG;
            eventHero->m_mobility += AI_EVENT_CREATURE_UPGRADE_MOBILITY;
            eventHero->m_remainingMobility += AI_EVENT_CREATURE_UPGRADE_MOBILITY;
        }
        if (eventHero->CreatureTypeCount(CREATURE_CAVALRY))
            eventHero->UpgradeCreatures(CREATURE_CAVALRY,
                                        CREATURE_CHAMPION);
        break;
    }
}

// @semantic
// Complete 0x18f body, 0x44 frame/slots, CFG, and all 12 ordered relocation
// sites/effective targets align. The sole executable residual is +0x11f..+0x12e:
// retail reads the cell word before shifting packedSite17 for the bitfield write,
// while ours shifts first. Direct, `| 0`, and OD_STEER(packedSite17) assignments were
// byte-neutral; an explicit m_objectData mask/write changed the body size/register
// width. Ten bounded TU-state declaration families produced no canonical exact
// closure; revisit after later EVENTS TU or mapCell accessor changes.
VA(0x004b4ca4, 0x18f)
void advManager::RecruitSiteAIEvent(mapCell *cell, hero *eventHero)
{
    i32 cost16[AI_EVENT_RESOURCE_COUNT];
    i32 purchaseCount17;
    u32 packedSite17;
    i16 availableCount1;
    u32 siteType3;
    i32 creatureType13;
    i32 resourceIndex27;
    i32 purchaseValue5;
    i32 replacementSlot26;

    siteType3 = cell->m_objectMetadata;
    siteType3 &= AI_EVENT_RECRUIT_TYPE_MASK;
    availableCount1 = static_cast<i16>(cell->m_objectMetadata);
    availableCount1 >>= AI_EVENT_RECRUIT_COUNT_SHIFT;

    switch (siteType3) {
    case 0:
        creatureType13 = CREATURE_GHOST;
        break;
    case 1:
        creatureType13 = CREATURE_EARTH_ELEMENTAL;
        break;
    case 2:
        creatureType13 = CREATURE_AIR_ELEMENTAL;
        break;
    case 3:
        creatureType13 = CREATURE_FIRE_ELEMENTAL;
        break;
    case 4:
        creatureType13 = CREATURE_WATER_ELEMENTAL;
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
                static_cast<i16>(availableCount1 - purchaseCount17);
            packedSite17 =
                (availableCount1 << AI_EVENT_RECRUIT_COUNT_SHIFT) | siteType3;
            cell->m_objectMetadata = packedSite17;
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

// @semantic
// All 134 normalized instructions, frame/slots, CFG, and 12 ordered relocation
// sites/effective targets match. The only raw differences are rel32 bytes +0x4e
// and +0x65: retail's two early exits target the identical final five-byte
// continuation while ours target its epilogue destination directly. Four bounded
// CFG families were tried: nested empty arms, flattened positive guard, explicit
// early returns, and a shared terminal label; alternatives added or removed 5-15
// bytes. Revisit only if an earlier EVENTS source edit changes emitted branch
// targets or comparison normalization learns continuation equivalence.
VA(0x004b4e33, 0x1a2)
void advManager::JailAIEvent(mapCell *cell, hero *eventHero, i32 x, i32 y)
{
    i32 heroId9;
    hero *releasedHero13;

    heroId9 = cell->m_objectMetadata;
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
                static_cast<i8>(heroId9);
            gpCurPlayer->m_heroCount++;
            releasedHero13->m_x = x;
            releasedHero13->m_y = y;
            releasedHero13->m_eventFlags = 0;
            releasedHero13->m_direction = AI_EVENT_HERO_DIRECTION;
            releasedHero13->m_remainingMobility = releasedHero13->CalcMobility();
            releasedHero13->m_mobility = releasedHero13->m_remainingMobility;
            releasedHero13->m_locationType = cell->m_triggerType;
            releasedHero13->m_occupiedTown = cell->m_objectMetadata;
            cell->m_triggerType = MAP_TRIGGER_ACTION_FLAG | MAP_OBJECT_HERO_INTERACTION;
            cell->m_objectMetadata = heroId9;
            SendMapChange(AI_EVENT_HERO_MAP_CHANGE,
                          static_cast<i8>(heroId9), x, y,
                          AI_EVENT_HERO_MAP_CHANGE_VALUE, 0, 0);
        }
    }
}

// @early-stop
// @early-stop-reloc-only
// All 0x82b bytes match after masking the 85 aligned relocation sites. The
// remaining objdiff residual is limited to delinked symbol identities.
VA(0x004b4fd5, 0x82b)
void advManager::PlayerMonsterInteract(mapCell *cell, mapCell *combatCell, hero *eventHero,
                                       i32 *handled, i32 x, i32 y, i32 unused,
                                       i32 combatX, i32 combatY)
{
    i32 monster_n;
    float strengthRatio_p;
    i32 combatResult_f;
    i32 forcedJoin_f;
    i32 joining;
    i32 monsterCount_n;
    i32 joiningCost_i;
    char offerText_g[MONSTER_OFFER_BUFFER_SIZE];

    unused = 0;
    gpMouseManager->ShowColorPointer();
    monster_n = cell->m_objectIndex;
    forcedJoin_f = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    monsterCount_n = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    strengthRatio_p = static_cast<float>(gpPhilAI->FightValueOfStack(&eventHero->m_army, eventHero, 0, 0, 0, 0)) /
                      static_cast<float>(gMonsterDatabase[monster_n].fightValue * monsterCount_n);

    if (gbInCampaign &&
        ((gpGame->m_campaignAwards[CAMPAIGN_AWARD_DWARVEN_ALLIANCE] &&
          (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)) ||
         (gpGame->m_campaignAwards[CAMPAIGN_AWARD_OGRE_ALLIANCE] &&
          (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD)) ||
         (gpGame->m_campaignAwards[CAMPAIGN_AWARD_DRAGON_ALLIANCE] &&
          (monster_n == CREATURE_GREEN_DRAGON ||
           monster_n == CREATURE_RED_DRAGON ||
           monster_n == CREATURE_BLACK_DRAGON)))) {
        if (!eventHero->m_army.CanJoin(monster_n)) {
            if (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)
                NormalDialog("The dwarves hail you, \"Any friend of Roland is a friend of ours.  You may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD)
                NormalDialog("The ogres give you a grunt of recognition, \"Archibald's allies may pass.\"",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else
                NormalDialog("The dragons see you and call out.  \"Our alliance with Archibald compels us to join you.  Unfortunately you have no room.  A pity!\"  They quickly scatter.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            *handled = 1;
        }
        else {
            if (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)
                NormalDialog("The dwarves recognize their allies and gladly join your forces.",
                             1, -1, -1, -1, 0, -1, 0, -1, 0);
            else if (monster_n == CREATURE_OGRE || monster_n == CREATURE_OGRE_LORD)
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

    if (gbInCampaign && gpGame->m_campaignAwards[CAMPAIGN_AWARD_DWARFBANE] &&
        (monster_n == CREATURE_DWARF || monster_n == CREATURE_BATTLE_DWARF)) {
        NormalDialog("\"The Dwarfbane!!!!, run for your lives.\"", 1, -1, -1, -1, 0, -1, 0, -1, 0);
        *handled = 1;
        return;
    }

    if (xIsPlayingExpansionCampaign && xCampaign.HasAward(0) &&
        (monster_n == CREATURE_ELF || monster_n == CREATURE_GRAND_ELF)) {
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
        !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) &&
        monster_n != CREATURE_GHOST && monster_n != CREATURE_EARTH_ELEMENTAL &&
        monster_n != CREATURE_AIR_ELEMENTAL && monster_n != CREATURE_FIRE_ELEMENTAL &&
        monster_n != CREATURE_WATER_ELEMENTAL) {
        if (forcedJoin_f) {
            sprintf(gText, gEventText[EVENT_TEXT_FOLLOWERS],
                    gArmyNamesPlural[monster_n]);
            EventWindow(-1, 2, gText, -1, 0, -1, 0, -1);
            if (gpWindowManager->m_dialogResult == MONSTER_DIALOG_YES) {
                eventHero->m_army.Add(monster_n, monsterCount_n, -1);
                *handled = 1;
                return;
            }
            else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, 1, "",
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
            if (joiningCost_i > gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD]) {
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
                gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -= joiningCost_i;
                return;
            }
            else {
                EventWindow(EVENT_TEXT_MONSTER_REFUSAL, 1, "",
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

// @semantic
// Complete semantics, 0x40-byte frame, slots, CFG, and all 36
// relocation sites are recovered. Retail is 25 bytes larger solely because it
// retains five five-byte continuation jumps; the remaining relocation names
// are delinked aliases of the same monster fields and floating constants. No
// additional variant was retained in this pass because the five exact jump sites
// are already isolated; revisit only if earlier EVENTS source/header changes alter
// inline continuation placement or comparison normalizes those destinations.
VA(0x004b5800, 0x440)
void advManager::ComputerMonsterInteract(mapCell *cell, hero *eventHero,
                                         i32 *handled)
{
    i32 joiningCount;
    i32 purchaseCount;
    i32 monsterCount[2];
    float strengthRatio;
    i32 purchaseValue;
    i32 replacementSlot;
    u32 forcedJoin;
    i32 monsterType;

    monsterType = cell->m_objectIndex;
    monsterCount[0] = cell->m_objectMetadata & MONSTER_COUNT_MASK;
    forcedJoin = cell->m_objectMetadata & MONSTER_JOIN_FORCED;
    strengthRatio = static_cast<float>(gpPhilAI->FightValueOfStack(
                        &eventHero->m_army, eventHero, 0, 0, 0, 0)) /
                    static_cast<float>(gMonsterDatabase[monsterType].fightValue *
                                       monsterCount[0]);

    if (eventHero->m_army.CanJoin(monsterType) &&
        !eventHero->HasArtifact(ARTIFACT_HIDEOUS_MASK) &&
        strengthRatio > MONSTER_STRENGTH_JOIN &&
        monsterType != CREATURE_GHOST &&
        monsterType != CREATURE_EARTH_ELEMENTAL &&
        monsterType != CREATURE_AIR_ELEMENTAL &&
        monsterType != CREATURE_FIRE_ELEMENTAL &&
        monsterType != CREATURE_WATER_ELEMENTAL) {
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

                i32 joiningCost = static_cast<i32>(
                    gMonsterDatabase[monsterType].cost * joiningCount *
                    MONSTER_AI_JOIN_COST_FRACTION);
                if (gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] <
                    joiningCost) {
                    if (strengthRatio > MONSTER_STRENGTH_FLEE)
                        goto computerMonstersFlee;
                    goto fightComputerMonsters;
                }
                gpPhilAI->EvaluateOneTimeCreaturePurchase(
                    monsterType, monsterCount[0], 1, purchaseCount,
                    purchaseValue, replacementSlot);
                if (purchaseCount > 0) {
                    gpGame->m_players[eventHero->m_owner].m_resources[RES_GOLD] -=
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
            eventHero->m_army.CanJoin(CREATURE_SKELETON)) {
            gpGame->GiveArmy(
                &eventHero->m_army, CREATURE_SKELETON,
                static_cast<i32>(static_cast<double>(monsterCount[0]) *
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
        cell->m_objectMetadata = ((cell->m_objectMetadata & MONSTER_JOIN_FORCED) +
                      (static_cast<u16>(monsterCount[0]) &
                       MONSTER_COUNT_MASK)) |
                     0;
    }
}

VA(0x004b5c40, 0x1d0)
i32 advManager::DoNetCombat(char *packet)
{
    static i16 sourceLineBase = 0x1655;
    hero *secondHero9;
    i32 secondSide15;
    i32 combatX1;
    i32 combatY8;
    i32 randomSeed1;
    i8 combatResult16[4];
    i32 firstSide2;
    hero *firstHero29;
    i32 remotePlayer8;
    i32 firstPlayer3;
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
    combatResult16[0] = static_cast<i8>(
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
        BaseFree(firstArmy6, EVENTS_FILE, sourceLineBase + 0x46);
    if (secondArmy10)
        BaseFree(secondArmy10, EVENTS_FILE, sourceLineBase + 0x49);
    if (combatTown[0])
        BaseFree(combatTown[0], EVENTS_FILE, sourceLineBase + 0x4c);
    if (secondHero9)
        BaseFree(secondHero9, EVENTS_FILE, sourceLineBase + 0x4f);
    if (firstHero29)
        BaseFree(firstHero29, EVENTS_FILE, sourceLineBase + 0x52);
    gbRetreatWin = 0;
    return 1;
}

// @early-stop
// All 0x64e bytes match after masking the 109 aligned COFF relocations; frame
// 0x78, every visible slot, CFG, and relocation offsets/targets agree. Objdiff's
// 99.94% residual is delinked string/global-alias and switch-local identities.
// The one-case packet switch supplies command spill -0x74; direct && omitted it.
// Retail case order is 0, 1, -1, 3, and its final player test reloads giCurPlayer.
// An int[2] padding probe was rejected because it grew the frame to 0x7c.
VA(0x004b5e10, 0x64e)
i32 advManager::DoCombat(i32 x, i32 y, hero *firstHero, armyGroup *firstArmy,
                         town *combatTown, hero *secondHero,
                         armyGroup *secondArmy, i32 firstSide, i32 secondSide,
                         i32 randomSeed, i32 processLosses)
{
    static i16 sourceLineBase = 0x16ba;
    armyGroup *receivedSecondArmy2;
    hero *receivedSecondHero9;
    hero *receivedFirstHero1;
    armyGroup *receivedFirstArmy;
    town *receivedTown;
    i32 remotePlayer;
    char *packet8;
    i8 combatResult3[4];
    tag_message message9;
    i32 secondPlayer7;
    i32 firstPlayer4;
    i32 savedPlayer1;
    i32 savedShowIt_e;
    i32 unusedCombat_a;

    if (giDebugLevel == 4)
        return AutoResolveCombat(x, y, firstHero, firstArmy, combatTown,
                                 secondHero, secondArmy, firstSide, secondSide,
                                 randomSeed, processLosses);
    gbInCombat = 1;
    firstPlayer4 = firstHero ? firstHero->m_owner : -1;
    secondPlayer7 = secondHero ? secondHero->m_owner
                              : (combatTown ? combatTown->m_owner : -1);
    if (randomSeed == -1)
        randomSeed = Random(1, 1000);
    DemobilizeCurrHero();
    savedPlayer1 = giCurPlayer;
    savedShowIt_e = bShowIt;

    if (firstPlayer4 >= 0 && secondPlayer7 >= 0 &&
        gbHumanPlayer[secondPlayer7]) {
        if (!gbThisNetHumanPlayer[secondPlayer7]) {
            iCombatControlNetPos[COMBAT_ATTACKER_SIDE] = giThisNetPos;
            iCombatControlNetPos[COMBAT_DEFENDER_SIDE] =
                gbGamePosToNetPos[secondPlayer7];
            SendHeroTownData(x, y, firstHero, firstArmy, combatTown, secondHero,
                             secondArmy, firstSide, secondSide, randomSeed,
                             gbGamePosToNetPos[secondPlayer7], 0, 0, 0);
            if (!gbHumanPlayer[firstPlayer4]) {
                for (;;) {
                    PollSound();
                    FillBitmapArea(gpWindowManager->m_screen, 30, 30, 4, 4, 0);
                    packet8 = CheckHandleNet();
                    if (packet8) {
                        switch (packet8[6]) {
                        case COMBAT_REMOTE_COMMAND:
                            ReceiveHeroTownData(
                                packet8, &remotePlayer, &x, &y,
                                &receivedFirstHero1, &receivedFirstArmy,
                                &receivedTown, &receivedSecondHero9,
                                &receivedSecondArmy2, &firstSide, &secondSide,
                                &randomSeed, combatResult3, &gbRetreatWin,
                                &gbCombatSurrender);
                            if (receivedFirstArmy) {
                                memcpy(firstArmy, receivedFirstArmy,
                                       sizeof(armyGroup));
                                BaseFree(receivedFirstArmy, EVENTS_FILE,
                                         sourceLineBase + 0x71);
                            }
                            if (receivedSecondArmy2) {
                                memcpy(secondArmy, receivedSecondArmy2,
                                       sizeof(armyGroup));
                                BaseFree(receivedSecondArmy2, EVENTS_FILE,
                                         sourceLineBase + 0x77);
                            }
                            if (receivedTown) {
                                memcpy(combatTown, receivedTown, sizeof(town));
                                BaseFree(receivedTown, EVENTS_FILE,
                                         sourceLineBase + 0x7d);
                            }
                            if (receivedSecondHero9) {
                                memcpy(secondHero, receivedSecondHero9,
                                       sizeof(hero));
                                BaseFree(receivedSecondHero9, EVENTS_FILE,
                                         sourceLineBase + 0x83);
                            }
                            if (receivedFirstHero1) {
                                memcpy(firstHero, receivedFirstHero1,
                                       sizeof(hero));
                                BaseFree(receivedFirstHero1, EVENTS_FILE,
                                         sourceLineBase + 0x89);
                            }
                            gpCombatManager->m_combatResult = combatResult3[0];
                            goto combatFinished;
                        }
                    }
                    Process1WindowsMessage();
                    message9 = gpInputManager->GetEvent();
                    CheckHandleNetPlayerWait(message9, 1);
                }
            }
        } else if (!gbThisNetHumanPlayer[firstPlayer4]) {
            bShowIt = 1;
            gpGame->TurnOffAIMusic();
            sprintf(gText, "%s's %s is under attack!",
                    cPlayerNames[secondPlayer7], combatTown ? "Town" : "Hero");
            gpGame->WaitForPlayer(gText, secondPlayer7);
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
    gpMouseManager->SetPointer(const_cast<char *>("advmice.mse"), 0,
                               MOUSE_AUTO_CURSOR_TYPE);
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
        case -1:
            HeroLoses(firstHero);
            HeroLoses(secondHero);
            break;
        case 3:
            break;
        }
    }
    bShowIt = savedShowIt_e;
    giCurPlayer = savedPlayer1;
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
    while (gpMouseManager->m_hideCount)
        gpMouseManager->ShowColorPointer();
    return gpCombatManager->m_combatResult;
}

VA(0x004b645e, 0x36f)
void advManager::SendHeroTownData(
    i32 x, i32 y, hero *firstHero, armyGroup *firstArmy, town *combatTown,
    hero *secondHero, armyGroup *secondArmy, i32 firstSide, i32 secondSide,
    i32 randomSeed, i32 remotePlayer, i32 combatResult, i32 retreatWin,
    i32 combatSurrender)
{
    static i16 sourceLineBase = 0x17cc;
    char *reply;
    i32 result;
    combatRemoteData *buffer = 0;

    buffer = static_cast<combatRemoteData *>(BaseAlloc(
        COMBAT_REMOTE_BUFFER_SIZE, EVENTS_FILE, sourceLineBase + 3));
    reply = 0;
    buffer->fragment = 0;
    buffer->x = static_cast<i8>(x);
    buffer->y = static_cast<i8>(y);
    buffer->hasFirstHero = firstHero != 0;
    buffer->hasTown = combatTown != 0;
    buffer->hasSecondHero = secondHero != 0;
    buffer->firstSide = static_cast<i8>(firstSide);
    buffer->secondSide = static_cast<i8>(secondSide);
    buffer->randomSeed = randomSeed;
    buffer->combatResult = static_cast<i8>(combatResult);
    buffer->retreatWin = static_cast<i8>(retreatWin);
    buffer->combatSurrender = static_cast<i8>(combatSurrender);
    if (firstHero) {
        buffer->firstOwner = firstHero->m_owner;
    } else {
        buffer->firstOwner = -1;
    }
    if (firstHero) {
        buffer->firstGold =
            gpGame->m_players[firstHero->m_owner].m_resources[RES_GOLD];
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
            gpGame->m_players[secondHero->m_owner].m_resources[RES_GOLD];
    } else {
        buffer->secondGold = 0;
    }
    memcpy(&buffer->firstArmy, firstArmy, sizeof(armyGroup));
    memcpy(&buffer->secondArmy, secondArmy, sizeof(armyGroup));
    if (combatTown)
        memcpy(&buffer->combatTown, combatTown, sizeof(town));

    result = TransmitAndWait(reinterpret_cast<char *>(buffer), remotePlayer,
                             COMBAT_REMOTE_HEADER_SIZE, COMBAT_REMOTE_COMMAND,
                             COMBAT_REMOTE_CONFIRM_COMMAND, &reply);
    if (!result)
        ShutDown(0);

    if (firstHero) {
        EVENTS_HERO_BUFFER(buffer)->fragment = COMBAT_REMOTE_FIRST_HERO_FIRST;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data, firstHero,
               COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
        EVENTS_HERO_BUFFER(buffer)->fragment = COMBAT_REMOTE_FIRST_HERO_SECOND;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data,
               &firstHero->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
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
        EVENTS_HERO_BUFFER(buffer)->fragment = COMBAT_REMOTE_SECOND_HERO_FIRST;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data, secondHero,
               COMBAT_REMOTE_HERO_FIRST_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_FIRST_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
        EVENTS_HERO_BUFFER(buffer)->fragment = COMBAT_REMOTE_SECOND_HERO_SECOND;
        memcpy(EVENTS_HERO_BUFFER(buffer)->data,
               &secondHero->m_spells[COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
               COMBAT_REMOTE_HERO_SECOND_SIZE);
        result = TransmitRemoteData(
            reinterpret_cast<char *>(buffer), remotePlayer,
            COMBAT_REMOTE_HERO_SECOND_SIZE + 1,
            COMBAT_REMOTE_COMMAND, COMBAT_REMOTE_FRAGMENT_TYPE,
            COMBAT_REMOTE_FRAGMENT_TYPE, -1);
        if (!result)
            ShutDown(0);
    }
    BaseFree(buffer, EVENTS_FILE, sourceLineBase + 0x5c);
}

VA(0x004b67cd, 0x462)
void advManager::ReceiveHeroTownData(
    char *packet, i32 *remotePlayer, i32 *x, i32 *y, hero **firstHero,
    armyGroup **firstArmy, town **combatTown, hero **secondHero,
    armyGroup **secondArmy, i32 *firstSide, i32 *secondSide, i32 *randomSeed,
    i8 *combatResult, i8 *retreatWin,
    i8 *combatSurrender)
{
    static i16 sourceLineBase = 0x183d;
    i32 hasFirstHero7;
    i32 hasTown0;
    i32 hasSecondHero8;
    i32l lastPacketTime36;
    i32 result7;
    i32 gotFirstHeroFirst3;
    i32 gotFirstHeroSecond9;
    i32 gotSecondHeroFirst13;
    i32 gotSecondHeroSecond6;
    i32 firstOwner29;
    i32 secondOwner28;

    *firstHero = 0;
    *firstArmy = 0;
    *combatTown = 0;
    *secondHero = 0;
    *secondArmy = 0;
    hasFirstHero7 = hasSecondHero8 = hasTown0 = 0;
    *remotePlayer = EVENTS_REMOTE_MESSAGE(packet)->sender;
    *x = EVENTS_REMOTE_COMBAT(packet)->x;
    *y = EVENTS_REMOTE_COMBAT(packet)->y;
    hasFirstHero7 = EVENTS_REMOTE_COMBAT(packet)->hasFirstHero;
    hasTown0 = EVENTS_REMOTE_COMBAT(packet)->hasTown;
    hasSecondHero8 = EVENTS_REMOTE_COMBAT(packet)->hasSecondHero;
    *firstSide = EVENTS_REMOTE_COMBAT(packet)->firstSide;
    *secondSide = EVENTS_REMOTE_COMBAT(packet)->secondSide;
    *randomSeed = EVENTS_REMOTE_COMBAT(packet)->randomSeed;
    *combatResult = EVENTS_REMOTE_COMBAT(packet)->combatResult;
    *retreatWin = EVENTS_REMOTE_COMBAT(packet)->retreatWin;
    *combatSurrender = EVENTS_REMOTE_COMBAT(packet)->combatSurrender;
    firstOwner29 = EVENTS_REMOTE_COMBAT(packet)->firstOwner;
    if (firstOwner29 > 0)
        gpGame->m_players[firstOwner29].m_resources[RES_GOLD] =
            EVENTS_REMOTE_COMBAT(packet)->firstGold;
    secondOwner28 = EVENTS_REMOTE_COMBAT(packet)->secondOwner;
    if (secondOwner28 > 0)
        gpGame->m_players[secondOwner28].m_resources[RES_GOLD] =
            EVENTS_REMOTE_COMBAT(packet)->secondGold;

    *firstArmy = static_cast<armyGroup *>(
        BaseAlloc(sizeof(armyGroup), EVENTS_FILE, sourceLineBase + 0x26));
    memcpy(*firstArmy, &EVENTS_REMOTE_COMBAT(packet)->firstArmy,
           sizeof(armyGroup));
    *secondArmy = static_cast<armyGroup *>(
        BaseAlloc(sizeof(armyGroup), EVENTS_FILE, sourceLineBase + 0x29));
    memcpy(*secondArmy, &EVENTS_REMOTE_COMBAT(packet)->secondArmy,
           sizeof(armyGroup));
    if (hasTown0) {
        *combatTown = static_cast<town *>(
            BaseAlloc(sizeof(town), EVENTS_FILE,
                      sourceLineBase + 0x2e));
        memcpy(*combatTown, &EVENTS_REMOTE_COMBAT(packet)->combatTown,
               sizeof(town));
    }

    iCombatControlNetPos[COMBAT_ATTACKER_SIDE] = *remotePlayer;
    iCombatControlNetPos[COMBAT_DEFENDER_SIDE] = giThisNetPos;
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
                      sourceLineBase + 0x47));
        gotFirstHeroFirst3 = 0;
        gotFirstHeroSecond9 = 0;
    }
    if (hasSecondHero8) {
        *secondHero = static_cast<hero *>(
            BaseAlloc(sizeof(hero), EVENTS_FILE,
                      sourceLineBase + 0x4d));
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
        if (packet &&
            EVENTS_REMOTE_MESSAGE(packet)->type == REMOTE_MESSAGE_RELIABLE &&
            EVENTS_REMOTE_MESSAGE(packet)->command == COMBAT_REMOTE_COMMAND) {
            lastPacketTime36 = KBTickCount();
            if (EVENTS_REMOTE_HERO(packet)->fragment ==
                COMBAT_REMOTE_FIRST_HERO_FIRST) {
                memcpy(*firstHero, EVENTS_REMOTE_HERO(packet)->data,
                       COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotFirstHeroFirst3 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment ==
                COMBAT_REMOTE_FIRST_HERO_SECOND) {
                memcpy(&(*firstHero)->m_spells[
                           COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                       EVENTS_REMOTE_HERO(packet)->data,
                       COMBAT_REMOTE_HERO_SECOND_SIZE);
                gotFirstHeroSecond9 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment ==
                COMBAT_REMOTE_SECOND_HERO_FIRST) {
                memcpy(*secondHero, EVENTS_REMOTE_HERO(packet)->data,
                       COMBAT_REMOTE_HERO_FIRST_SIZE);
                gotSecondHeroFirst13 = 1;
            }
            if (EVENTS_REMOTE_HERO(packet)->fragment ==
                COMBAT_REMOTE_SECOND_HERO_SECOND) {
                memcpy(&(*secondHero)->m_spells[
                           COMBAT_REMOTE_HERO_SECOND_SPELL_INDEX],
                       EVENTS_REMOTE_HERO(packet)->data,
                       COMBAT_REMOTE_HERO_SECOND_SIZE);
                gotSecondHeroSecond6 = 1;
            }
        }
    }
}

// @semantic
// Complete semantics, frame, slots, CFG, and external relocation
// targets. The retail body is exactly one five-byte continuation jump longer;
// the apparent relocation excess is the delinked six-entry local switch table.
// No additional variant was retained in this pass because the single jump site
// is isolated; revisit only if earlier EVENTS source/header changes alter inline
// continuation placement or comparison normalizes the proven destination.
VA(0x004b6c2f, 0x254)
i32 advManager::AutoResolveCombat(
    i32 x, i32 y, hero *firstHero, armyGroup *firstArmy, town *combatTown,
    hero *secondHero, armyGroup *secondArmy, i32 firstSide, i32 secondSide,
    i32 randomSeed, i32 processLosses)
{
    tag_message message;
    i32 savedShowIt37;

    gbNoShowCombat = 1;
    savedShowIt37 = bShowIt;
    bShowIt = 0;
    gpMouseManager->SetPointer(0);
    gpMouseManager->m_forcePointerUpdate = 1;
    message.type = MESSAGE_NONE;
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

// @early-stop
// @early-stop-reloc-only
// All 0xb8 relocation-masked bytes, frame/slots, and CFG match. All four ordered
// relocation sites resolve identically; retail names the final CRT alias _strcmpi
// while the reconstructed SDK declaration emits the equivalent _stricmp symbol.
VA(0x004b6e83, 0xb8)
i32 RiddleStringsEqual(char *answer, char *expected)
{
    i32 index;
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
// @data-layout-note Retail's initialized EVENTS contribution is 0x5700 bytes at
// RVA 0x117620. The recovered candidate .data is 0x56ff bytes: 247 definitions
// cover every nonzero retail byte, and the one-byte stream difference is terminal
// zero padding rather than an allocation. s_twoStringFormat and gbNoShowCombat
// are the two source DATA owners. The reviewed compiler-private group has 245
// .data allocations: 147 unique payloads, 48 aligned relocation-addend proofs,
// 11 bounded-section runs, one direct relocation bijection, three remaining-slot
// bijections, and 35 members of four exact-payload equivalence classes. All seven
// .rdata temporaries replay exactly over the 0x38-byte retail contribution. The
// constrained group validator proves exact payloads, disjoint extents, complete
// nonzero-byte coverage, and zero diagnostics for all 254 definitions.
DATA(0x0051cc0c) i32 gbNoShowCombat = 0;
