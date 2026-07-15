// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\X_GLOBAL.OBJ   from: (directly linked into exe)
// functions: 0   data: 25
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/X_GLOBAL.h>

// ---- globals (definitions, RVA order) ----
DATA(0x004f2558) char *xSetupCampaignGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {
    "{Original Campaign}\n\nEither Roland's or Archibald's campaign from the original Heroes of Might and Magic II.",
    "{Expansion Campaign}\n\nOne of the four new campaigns from the Price of Loyalty expansion set.",
    "{Cancel}\n\nCancel back to the main menu.",
    0
};
DATA(0x004f2568) char *xSetupStandardGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {
    "{Original Map}\n\nSelects a game from the original Heroes of Might and Magic II.",
    "{Expansion Map}\n\nSelects a game from the Price of Loyalty expansion set.",
    "{Cancel}\n\nCancel back to main menu.",
    0
};
DATA(0x004f2578) char *xCampaignAwards[X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT] = {
    "Elven alliance", "Breastplate", "Wood bonus", "Helmet",
    "Kraeger Defeated", "Battle Garb", "Wayward son", "Uncle Ivan",
    "Legendary Sceptre", "Guardian Spell", "Sphere of Negation", 0
};
DATA(0x004f25a8) char *xScenarioName[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                                      [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] = {
    {"Uprising", "Island of Chaos", "Arrow's Flight", "The Abyss",
     "The Giant's Pass", "Aurora Borealis", "Betrayal's End",
     "Corruption's Heart"},
    {"Conquer and Unify", "Border Towns", "The Wayward Son",
     "Crazy Uncle Ivan", "The Southern War", "Ivory Gates",
     "The Elven Lands", "The Epic Battle"},
    {"The Shrouded Isles", "The Eternal Scrolls", "Power's End",
     "Fount of Wizardry", "", "", "", ""},
    {"Stranded", "Pirate Isles", "King and Country", "Blood is Thicker",
     "", "", "", ""}
};
DATA(0x004f2628) char *xScenarioDescription[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT]
                                             [X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] = {
    {
        "Subdue the unruly local lords in order to provide the Empire with facilities to operate in this region.",
        "Eliminate all oposition in this area. Then the first piece of the artifact will be yours.",
        "The sorceresses to the northeast are rebelling! For the good of the empire you must quash their feeble uprising on your way to the mountains.",
        "Having prepared for your arrival, Kraeger has arranged for a force of necromancers to thwart your quest. You must capture the castle of Scabsdale before the first day of the third week, or the Necromancers will be too strong for you.",
        "The barbarian despot in this area is, as yet, ignorant of your presence. Quickly, build up your forces before you are discovered and attacked! Secure the region by subduing all enemy forces.",
        "The Empire is weak in this region. You will be unable to completely subdue all forces in this area, so take what you can before reprisal strikes. Remember, your true goal is to claim the Helmet of Anduran.",
        "For the good of the Empire, eliminate Kraeger.",
        "At last, you have the opportunity and the facilities to rid the Empire of the necromancer's evil. Eradicate them completely, and you will be sung as a hero for all time."
    },
    {
        "Conquer and unite all the enemy tribes. Don't lose the hero Jarkonas, the forefather of all descendants.",
        "Your rival, the kingdom of Harondale, is attacking weak towns on your border! Recover from their first strike and crush them completely!",
        "Find your wayward son Joseph who is rumored to be living in the desolate lands. Do it before the first day of the third month or it will be of no help to your family.",
        "Rescue your crazy uncle Ivan. Find him before the first day of the fourth month or it will be no help to your kingdom.",
        "Destroy the barbarians who are attacking the southern border of your kingdom! Recover your fallen towns, and then invade the jungle kingdom. Leave no enemy standing.",
        "Retake the castle of Ivory Gates, which has fallen due to treachery.",
        "Gain the favor of the elves. They will not allow trees to be chopped down, so we will send you wood every 2 weeks. You must complete your mission before the first day of the seventh month, or the kingdom will surely fall.",
        "This is the final battle against your rival kingdom of Harondale. Eliminate everyone, and don't lose the hero Jarkonas VI."
    },
    {
        "Your mission is to vanquish the warring mages in the magical Shrouded Isles. The completion of this task will give you a fighting chance against your rivals.",
        "The location of the great library has been discovered! You must make your way to it, and reclaim the city of Chronos in which it lies.",
        "Find the Orb of negation, which is said to be buried in this land. There are clues inscribed on stone obelisks which will help lead you to your prize. Find the Orb before the first day of the sixth month, or your rivals will surely have gotten to the fount before you.",
        "You must take control of the castle of Magic, where the fount of wizardry lies. Do this and your victory will be supreme.",
        "", "", "", ""
    },
    {
        "Capture the town on the island off the southeast shore in order to construct a boat and travel back towards the mainland. Do not lose the hero Gallavant.",
        "Find and defeat Martine, the pirate leader, who resides in Pirates Cove. Do not lose Gallavant or your quest will be over.",
        "Eliminate all the other forces who oppose the rule of Lord Alberon. Gallavant must not die.",
        "Overthrow the entrenched monarchy of Lord Alberon, and claim all the land in your name. Gallavant must not die.",
        "", "", "", ""
    }
};
DATA(0x004f26a8) char *xShortCampaignNames[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT] = {
    "POL", "DES", "WIZ", "VOY"
};
DATA(0x004f26b8) char *xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT];
DATA(0x004f26c8) char *xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT];
DATA(0x004f26e0) char *xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT];
DATA(0x004f26f4) char *xNecromancerShrine;
DATA(0x004f26f8) char *xNecromancerShrineDesc;
DATA(0x004f2700) char *xStableText[X_GLOBAL_STABLE_TEXT_COUNT];
DATA(0x004f2710) char *xJosephName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] = {
    "Joseph", "Joseph II", "Joseph III", "Joseph IV", "Joseph V", "Joseph VI"
};
DATA(0x004f2728) char *xUncleIvanName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] = {
    "Uncle Ivan", "Ivan II", "Ivan III", "Ivan IV", "Ivan V", "Ivan VI"
};
DATA(0x004f2740) char *xShortSSLevelNames[X_GLOBAL_SHORT_SKILL_LEVEL_COUNT] = {
    "Bas.", "Adv.", "Exp.", 0
};
DATA(0x004f2750) char *xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT];
DATA(0x004f2a9c) signed char xNetHasOldPlayers = 0;
DATA(0x004f2ab0) int xShrineBuildingCost[2334];
DATA(0x00527e78) int xPasswordStringsIndex[8];
DATA(0x00527e98) signed char xIsExpansionMap;
DATA(0x00527e9c) int xLastChoice;
DATA(0x00527e20) unsigned char xIsPlayingExpansionCampaign;
DATA(0x00527e28) ExpCampaign xCampaign;
DATA(0x00527ea0) int xTheSpell;
