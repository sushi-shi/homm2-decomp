#include <va.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/X_GLOBAL.h>


DATA(0x004f2558) char* xSetupCampaignGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {
    DATA_COMPGEN(0x004f2acc, xSetupCampaignGameHelpOriginalCampaignEitherRolandSOr, "{Original Campaign}\n\nEither Roland's or Archibald's campaign from the original Heroes of "
    "Might and Magic II."),
    DATA_COMPGEN(0x004f2b3c, xSetupCampaignGameHelpExpansionCampaignOneOfTheFour, "{Expansion Campaign}\n\nOne of the four new campaigns from the Price of Loyalty expansion "
    "set."),
    DATA_COMPGEN(0x004f2b9c, xSetupCampaignGameHelpCancelCancelBackToTheMain, "{Cancel}\n\nCancel back to the main menu."),
    NULL
};
DATA(0x004f2568) char* xSetupStandardGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {
    DATA_COMPGEN(0x004f2bc4, xSetupStandardGameHelpOriginalMapSelectsAGameFrom, "{Original Map}\n\nSelects a game from the original Heroes of Might and Magic II."),
    DATA_COMPGEN(0x004f2c14, xSetupStandardGameHelpExpansionMapSelectsAGameFrom, "{Expansion Map}\n\nSelects a game from the Price of Loyalty expansion set."),
    DATA_COMPGEN(0x004f2c60, xSetupStandardGameHelpCancelCancelBackToMainMenu, "{Cancel}\n\nCancel back to main menu."),
    NULL
};
DATA(0x004f2578) char* xCampaignAwards[X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT] = {
    DATA_COMPGEN(0x004f2c84, xCampaignAwardsElvenAlliance, "Elven alliance"),
    DATA_COMPGEN(0x004f2c94, xCampaignAwardsBreastplate, "Breastplate"),
    DATA_COMPGEN(0x004f2ca0, xCampaignAwardsWoodBonus, "Wood bonus"),
    DATA_COMPGEN(0x004f2cac, xCampaignAwardsHelmet, "Helmet"),
    DATA_COMPGEN(0x004f2cb4, xCampaignAwardsKraegerDefeated, "Kraeger Defeated"),
    DATA_COMPGEN(0x004f2cc8, xCampaignAwardsBattleGarb, "Battle Garb"),
    DATA_COMPGEN(0x004f2cd4, xCampaignAwardsWaywardSon, "Wayward son"),
    DATA_COMPGEN(0x004f2ce0, xCampaignAwardsUncleIvan, "Uncle Ivan"),
    DATA_COMPGEN(0x004f2cec, xCampaignAwardsLegendarySceptre, "Legendary Sceptre"),
    DATA_COMPGEN(0x004f2d00, xCampaignAwardsGuardianSpell, "Guardian Spell"),
    DATA_COMPGEN(0x004f2d10, xCampaignAwardsSphereOfNegation, "Sphere of Negation"),
    NULL
};
DATA(0x004f25a8) char*
    xScenarioName[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT][X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] = {
        {DATA_COMPGEN(0x004f2d24, xScenarioNameUprising, "Uprising"),
         DATA_COMPGEN(0x004f2d30, xScenarioNameIslandOfChaos, "Island of Chaos"),
         DATA_COMPGEN(0x004f2d40, xScenarioNameArrowSFlight, "Arrow's Flight"),
         DATA_COMPGEN(0x004f2d50, xScenarioNameTheAbyss, "The Abyss"),
         DATA_COMPGEN(0x004f2d5c, xScenarioNameTheGiantSPass, "The Giant's Pass"),
         DATA_COMPGEN(0x004f2d70, xScenarioNameAuroraBorealis, "Aurora Borealis"),
         DATA_COMPGEN(0x004f2d80, xScenarioNameBetrayalSEnd, "Betrayal's End"),
         DATA_COMPGEN(0x004f2d90, xScenarioNameCorruptionSHeart, "Corruption's Heart")},
        {DATA_COMPGEN(0x004f2da4, xScenarioNameConquerAndUnify, "Conquer and Unify"),
         DATA_COMPGEN(0x004f2db8, xScenarioNameBorderTowns, "Border Towns"),
         DATA_COMPGEN(0x004f2dc8, xScenarioNameTheWaywardSon, "The Wayward Son"),
         DATA_COMPGEN(0x004f2dd8, xScenarioNameCrazyUncleIvan, "Crazy Uncle Ivan"),
         DATA_COMPGEN(0x004f2dec, xScenarioNameTheSouthernWar, "The Southern War"),
         DATA_COMPGEN(0x004f2e00, xScenarioNameIvoryGates, "Ivory Gates"),
         DATA_COMPGEN(0x004f2e0c, xScenarioNameTheElvenLands, "The Elven Lands"),
         DATA_COMPGEN(0x004f2e1c, xScenarioNameTheEpicBattle, "The Epic Battle")},
        {DATA_COMPGEN(0x004f2e2c, xScenarioNameTheShroudedIsles, "The Shrouded Isles"),
         DATA_COMPGEN(0x004f2e40, xScenarioNameTheEternalScrolls, "The Eternal Scrolls"),
         DATA_COMPGEN(0x004f2e54, xScenarioNamePowerSEnd, "Power's End"),
         DATA_COMPGEN(0x004f2e60, xScenarioNameFountOfWizardry, "Fount of Wizardry"),
         DATA_COMPGEN(0x004f2e74, xScenarioNameEmptyString, ""),
         DATA_COMPGEN(0x004f2e78, xScenarioNameEmptyString2, ""),
         DATA_COMPGEN(0x004f2e7c, xScenarioNameEmptyString3, ""),
         DATA_COMPGEN(0x004f2e80, xScenarioNameEmptyString4, "")},
        {DATA_COMPGEN(0x004f2e84, xScenarioNameStranded, "Stranded"), DATA_COMPGEN(0x004f2e90, xScenarioNamePirateIsles, "Pirate Isles"), DATA_COMPGEN(0x004f2ea0, xScenarioNameKingAndCountry, "King and Country"), DATA_COMPGEN(0x004f2eb4, xScenarioNameBloodIsThicker, "Blood is Thicker"), DATA_COMPGEN(0x004f2ec8, xScenarioNameEmptyString5, ""), DATA_COMPGEN(0x004f2ecc, xScenarioNameEmptyString6, ""), DATA_COMPGEN(0x004f2ed0, xScenarioNameEmptyString7, ""), DATA_COMPGEN(0x004f2ed4, xScenarioNameEmptyString8, "")}
};
DATA(0x004f2628) char*
    xScenarioDescription[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT][X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] =
        {{DATA_COMPGEN(0x004f2ed8, xScenarioDescriptionSubdueTheUnrulyLocalLordsIn, "Subdue the unruly local lords in order to provide the Empire with facilities to operate "
          "in this region."),
          DATA_COMPGEN(0x004f2f40, xScenarioDescriptionEliminateAllOpositionInThisArea, "Eliminate all oposition in this area. Then the first piece of the artifact will be "
          "yours."),
          DATA_COMPGEN(0x004f2f9c, xScenarioDescriptionTheSorceressesToTheNortheastAre, "The sorceresses to the northeast are rebelling! For the good of the empire you must "
          "quash their feeble uprising on your way to the mountains."),
          DATA_COMPGEN(0x004f302c, xScenarioDescriptionHavingPreparedForYourArrivalKraeger, "Having prepared for your arrival, Kraeger has arranged for a force of necromancers to "
          "thwart your quest. You must capture the castle of Scabsdale before the first day of the "
          "third week, or the Necromancers will be too strong for you."),
          DATA_COMPGEN(0x004f3118, xScenarioDescriptionTheBarbarianDespotInThisArea, "The barbarian despot in this area is, as yet, ignorant of your presence. Quickly, build "
          "up your forces before you are discovered and attacked! Secure the region by subduing "
          "all enemy forces."),
          DATA_COMPGEN(0x004f31d8, xScenarioDescriptionTheEmpireIsWeakInThis, "The Empire is weak in this region. You will be unable to completely subdue all forces "
          "in this area, so take what you can before reprisal strikes. Remember, your true goal is "
          "to claim the Helmet of Anduran."),
          DATA_COMPGEN(0x004f32a8, xScenarioDescriptionForTheGoodOfTheEmpire, "For the good of the Empire, eliminate Kraeger."),
          DATA_COMPGEN(0x004f32d8, xScenarioDescriptionAtLastYouHaveTheOpportunity, "At last, you have the opportunity and the facilities to rid the Empire of the "
          "necromancer's evil. Eradicate them completely, and you will be sung as a hero for all "
          "time.")},
         {DATA_COMPGEN(0x004f3384, xScenarioDescriptionConquerAndUniteAllTheEnemy, "Conquer and unite all the enemy tribes. Don't lose the hero Jarkonas, the forefather of "
          "all descendants."),
          DATA_COMPGEN(0x004f33f0, xScenarioDescriptionYourRivalTheKingdomOfHarondale, "Your rival, the kingdom of Harondale, is attacking weak towns on your border! Recover "
          "from their first strike and crush them completely!"),
          DATA_COMPGEN(0x004f347c, xScenarioDescriptionFindYourWaywardSonJosephWho, "Find your wayward son Joseph who is rumored to be living in the desolate lands. Do it "
          "before the first day of the third month or it will be of no help to your family."),
          DATA_COMPGEN(0x004f3524, xScenarioDescriptionRescueYourCrazyUncleIvanFind, "Rescue your crazy uncle Ivan. Find him before the first day of the fourth month or it "
          "will be no help to your kingdom."),
          DATA_COMPGEN(0x004f359c, xScenarioDescriptionDestroyTheBarbariansWhoAreAttacking, "Destroy the barbarians who are attacking the southern border of your kingdom! Recover "
          "your fallen towns, and then invade the jungle kingdom. Leave no enemy standing."),
          DATA_COMPGEN(0x004f3644, xScenarioDescriptionRetakeTheCastleOfIvoryGates, "Retake the castle of Ivory Gates, which has fallen due to treachery."),
          DATA_COMPGEN(0x004f368c, xScenarioDescriptionGainTheFavorOfTheElves, "Gain the favor of the elves. They will not allow trees to be chopped down, so we will "
          "send you wood every 2 weeks. You must complete your mission before the first day of the "
          "seventh month, or the kingdom will surely fall."),
          DATA_COMPGEN(0x004f376c, xScenarioDescriptionThisIsTheFinalBattleAgainst, "This is the final battle against your rival kingdom of Harondale. Eliminate everyone, "
          "and don't lose the hero Jarkonas VI.")},
         {DATA_COMPGEN(0x004f37e8, xScenarioDescriptionYourMissionIsToVanquishThe, "Your mission is to vanquish the warring mages in the magical Shrouded Isles. The "
          "completion of this task will give you a fighting chance against your rivals."),
          DATA_COMPGEN(0x004f3888, xScenarioDescriptionTheLocationOfTheGreatLibrary, "The location of the great library has been discovered! You must make your way to it, "
          "and reclaim the city of Chronos in which it lies."),
          DATA_COMPGEN(0x004f3910, xScenarioDescriptionFindTheOrbOfNegationWhich, "Find the Orb of negation, which is said to be buried in this land. There are clues "
          "inscribed on stone obelisks which will help lead you to your prize. Find the Orb before "
          "the first day of the sixth month, or your rivals will surely have gotten to the fount "
          "before you."),
          DATA_COMPGEN(0x004f3a20, xScenarioDescriptionYouMustTakeControlOfThe, "You must take control of the castle of Magic, where the fount of wizardry lies. Do this "
          "and your victory will be supreme."),
          DATA_COMPGEN(0x004f3a9c, xScenarioDescriptionEmptyString, ""),
          DATA_COMPGEN(0x004f3aa0, xScenarioDescriptionEmptyString2, ""),
          DATA_COMPGEN(0x004f3aa4, xScenarioDescriptionEmptyString3, ""),
          DATA_COMPGEN(0x004f3aa8, xScenarioDescriptionEmptyString4, "")},
         {DATA_COMPGEN(0x004f3aac, xScenarioDescriptionCaptureTheTownOnTheIsland, "Capture the town on the island off the southeast shore in order to construct a boat and "
          "travel back towards the mainland. Do not lose the hero Gallavant."),
          DATA_COMPGEN(0x004f3b48, xScenarioDescriptionFindAndDefeatMartineThePirate, "Find and defeat Martine, the pirate leader, who resides in Pirates Cove. Do not lose "
          "Gallavant or your quest will be over."),
          DATA_COMPGEN(0x004f3bc4, xScenarioDescriptionEliminateAllTheOtherForcesWho, "Eliminate all the other forces who oppose the rule of Lord Alberon. Gallavant must not "
          "die."),
          DATA_COMPGEN(0x004f3c20, xScenarioDescriptionOverthrowTheEntrenchedMonarchyOfLord, "Overthrow the entrenched monarchy of Lord Alberon, and claim all the land in your name. "
          "Gallavant must not die."),
          DATA_COMPGEN(0x004f3c90, xScenarioDescriptionEmptyString5, ""),
          DATA_COMPGEN(0x004f3c94, xScenarioDescriptionEmptyString6, ""),
          DATA_COMPGEN(0x004f3c98, xScenarioDescriptionEmptyString7, ""),
          DATA_COMPGEN(0x004f3c9c, xScenarioDescriptionEmptyString8, "")}};
DATA(0x004f26a8) char* xShortCampaignNames[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT] =
    {DATA_COMPGEN(0x004f3ca0, xShortCampaignNamesPOL, "POL"), DATA_COMPGEN(0x004f3ca4, xShortCampaignNamesDES, "DES"), DATA_COMPGEN(0x004f3ca8, xShortCampaignNamesWIZ, "WIZ"), DATA_COMPGEN(0x004f3cac, xShortCampaignNamesVOY, "VOY")};
DATA(0x004f26b8) char* xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT] =
    {DATA_COMPGEN(0x004f3cb0, xHSCampaignNamesPriceOfLoyalty, "Price of Loyalty"), DATA_COMPGEN(0x004f3cc4, xHSCampaignNamesDescendants, "Descendants"), DATA_COMPGEN(0x004f3cd0, xHSCampaignNamesWizardSIsle, "Wizard's Isle"), DATA_COMPGEN(0x004f3ce0, xHSCampaignNamesVoyageHome, "Voyage Home")};
DATA(0x004f26c8) char* xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT] = {
    DATA_COMPGEN(0x004f3cec, xRecruitEmptyThisBurialSiteIsDeathlyStill, "This burial site is deathly still."),
    DATA_COMPGEN(0x004f3d10, xRecruitEmptyThePitOfMudBubblesFor, "The pit of mud bubbles for a minute and then lies still."),
    DATA_COMPGEN(0x004f3d4c, xRecruitEmptyYouEnterTheStructureOfWhite, "You enter the structure of white stone pillars, and find nothing."),
    DATA_COMPGEN(0x004f3d90, xRecruitEmptyNoFireElementalsApproachYouFrom, "No Fire Elementals approach you from the lava pool."),
    DATA_COMPGEN(0x004f3dc4, xRecruitEmptyAFaceFormsInTheWater, "A face forms in the water for a moment, and then is gone."),
    NULL
};
DATA(0x004f26e0) char* xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT] = {
    DATA_COMPGEN(0x004f3e00, xRecruitBuyRestlessSpiritsOfLongDeadWarriors, "Restless spirits of long dead warriors seeking their final resting place offer to join you in "
    "hopes of finding peace.  Do you wish to recruit ghosts?"),
    DATA_COMPGEN(0x004f3e98, xRecruitBuyAsYouApproachTheBubblingPit, "As you approach the bubbling pit of mud, creatures begin to climb out and position themselves "
    "around it.  In unison they say: \"Mother Earth would like to offer you a few of her troops.  "
    "Do you want to recruit Earth Elementals?\""),
    DATA_COMPGEN(0x004f3f7c, xRecruitBuyWhiteStonePillarsSupportARoof, "White stone pillars support a roof that rises up to the sky.  As you enter the structure, the "
    "dead air of the outside gives way to a whirling gust that almost pushes you back out.  The "
    "air current materializes into a barely visible form.  The creature asks, in what can only be "
    "described as a loud whisper: \"Why have you come?  Are you here to call upon the forces of "
    "the air?\""),
    DATA_COMPGEN(0x004f40f8, xRecruitBuyBeneathAStructureThatServesTo, "Beneath a structure that serves to hold in heat, Fire Elementals move about in a fiery pool "
    "of molten lava.  A group of them approach you and offer their services.  Would you like to "
    "recruit Fire Elementals?"),
    DATA_COMPGEN(0x004f41c8, xRecruitBuyCrystallineStructuresCastShadowsOverA, "Crystalline structures cast shadows over a small reflective pool of water.  You peer into the "
    "pool, and a face that is not your own peers back.  It asks: \"Would you like to call upon the "
    "powers of water?\"")
};
DATA(0x004f26f4) char* xNecromancerShrine = DATA_COMPGEN(0x004f4298, xNecromancerShrineShrine, "Shrine");
DATA(0x004f26f8) char* xNecromancerShrineDesc =
    DATA_COMPGEN(0x004f42a0, xNecromancerShrineDescTheShrineIncreasesTheNecromancySkill, "The Shrine increases the necromancy skill of all your necromancers by 10 percent.");
DATA(0x004f2700) char* xStableText[X_GLOBAL_STABLE_TEXT_COUNT] = {
    DATA_COMPGEN(0x004f42f4, xStableTextTheHeadGroomApproachesYouAnd, "The head groom approaches you and speaks, \"You already have a fine horse, and have no "
    "inexperienced cavalry which might make use of our trained war horses.\""),
    DATA_COMPGEN(0x004f4394, xStableTextAsYouApproachTheStablesThe2, "As you approach the stables, the head groom appears, leading a fine looking war horse. \"This "
    "steed will help speed you in your travels. Alas, his endurance will wane with a lot of heavy "
    "riding, and you must return for a fresh mount in a week. We also have many fine war horses "
    "which could benefit mounted soldiers, but you have none we can help.\""),
    DATA_COMPGEN(0x004f44f0, xStableTextTheHeadGroomSpeaksToYou, "The head groom speaks to you, \"That is a fine looking horse you have. I am afraid we can "
    "give you no better, but the horses your cavalry are riding look to be of poor breeding stock. "
    "We have many trained war horses which would aid your riders greatly. I insist you take "
    "them.\""),
    DATA_COMPGEN(0x004f4608, xStableTextAsYouApproachTheStablesThe, "As you approach the stables, the head groom appears, leading a fine looking war horse. \"This "
    "steed will help speed you in your travels. Alas, he will grow tired in a week. You must also "
    "let me give better horses to your mounted soldiers, their horses look shoddy and weak.\"")
};
DATA(0x004f2710) char* xJosephName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] =
    {DATA_COMPGEN(0x004f471c, xJosephNameJoseph, "Joseph"), DATA_COMPGEN(0x004f4724, xJosephNameJosephII, "Joseph II"), DATA_COMPGEN(0x004f4730, xJosephNameJosephIII, "Joseph III"), DATA_COMPGEN(0x004f473c, xJosephNameJosephIV, "Joseph IV"), DATA_COMPGEN(0x004f4748, xJosephNameJosephV, "Joseph V"), DATA_COMPGEN(0x004f4754, xJosephNameJosephVI, "Joseph VI")};
DATA(0x004f2728) char* xUncleIvanName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] =
    {DATA_COMPGEN(0x004f4760, xUncleIvanNameUncleIvan, "Uncle Ivan"), DATA_COMPGEN(0x004f476c, xUncleIvanNameIvanII, "Ivan II"), DATA_COMPGEN(0x004f4774, xUncleIvanNameIvanIII, "Ivan III"), DATA_COMPGEN(0x004f4780, xUncleIvanNameIvanIV, "Ivan IV"), DATA_COMPGEN(0x004f4788, xUncleIvanNameIvanV, "Ivan V"), DATA_COMPGEN(0x004f4790, xUncleIvanNameIvanVI, "Ivan VI")};
DATA(0x004f2740) char* xShortSSLevelNames[X_GLOBAL_SHORT_SKILL_LEVEL_COUNT] =
    {DATA_COMPGEN(0x004f4798, xShortSSLevelNamesBas, "Bas."), DATA_COMPGEN(0x004f47a0, xShortSSLevelNamesAdv, "Adv."), DATA_COMPGEN(0x004f47a8, xShortSSLevelNamesExp, "Exp."), NULL};
DATA(0x004f2750) char* xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT] = {
    DATA_COMPGEN(0x004f47b0, xPasswordStringsAdvance, "Advance"),    DATA_COMPGEN(0x004f47b8, xPasswordStringsAdvent, "Advent"),      DATA_COMPGEN(0x004f47c0, xPasswordStringsAdventure, "Adventure"),  DATA_COMPGEN(0x004f47cc, xPasswordStringsAnimate, "Animate"),     DATA_COMPGEN(0x004f47d4, xPasswordStringsArcher, "Archer"),     DATA_COMPGEN(0x004f47dc, xPasswordStringsArena, "Arena"),
    DATA_COMPGEN(0x004f47e4, xPasswordStringsArmor, "Armor"),      DATA_COMPGEN(0x004f47ec, xPasswordStringsArmy, "Army"),        DATA_COMPGEN(0x004f47f4, xPasswordStringsArrow, "Arrow"),      DATA_COMPGEN(0x004f47fc, xPasswordStringsArtifact, "Artifact"),    DATA_COMPGEN(0x004f4808, xPasswordStringsAttraction, "Attraction"), DATA_COMPGEN(0x004f4814, xPasswordStringsBackstab, "Backstab"),
    DATA_COMPGEN(0x004f4820, xPasswordStringsBarbarian, "Barbarian"),  DATA_COMPGEN(0x004f482c, xPasswordStringsBaron, "Baron"),       DATA_COMPGEN(0x004f4834, xPasswordStringsBattle, "Battle"),     DATA_COMPGEN(0x004f483c, xPasswordStringsBerserk, "Berserk"),     DATA_COMPGEN(0x004f4844, xPasswordStringsBlacksmith, "Blacksmith"), DATA_COMPGEN(0x004f4850, xPasswordStringsBlade, "Blade"),
    DATA_COMPGEN(0x004f4858, xPasswordStringsBless, "Bless"),      DATA_COMPGEN(0x004f4860, xPasswordStringsBlind, "Blind"),       DATA_COMPGEN(0x004f4868, xPasswordStringsBoots, "Boots"),      DATA_COMPGEN(0x004f4870, xPasswordStringsBreastplate, "Breastplate"), DATA_COMPGEN(0x004f487c, xPasswordStringsBuckler, "Buckler"),    DATA_COMPGEN(0x004f4884, xPasswordStringsCaptain, "Captain"),
    DATA_COMPGEN(0x004f488c, xPasswordStringsCastle, "Castle"),     DATA_COMPGEN(0x004f4894, xPasswordStringsCatapult, "Catapult"),    DATA_COMPGEN(0x004f48a0, xPasswordStringsCathedral, "Cathedral"),  DATA_COMPGEN(0x004f48ac, xPasswordStringsCauldron, "Cauldron"),    DATA_COMPGEN(0x004f48b8, xPasswordStringsCavalry, "Cavalry"),    DATA_COMPGEN(0x004f48c0, xPasswordStringsCentaur, "Centaur"),
    DATA_COMPGEN(0x004f48c8, xPasswordStringsChampion, "Champion"),   DATA_COMPGEN(0x004f48d4, xPasswordStringsCharm, "Charm"),       DATA_COMPGEN(0x004f48dc, xPasswordStringsCloak, "Cloak"),      DATA_COMPGEN(0x004f48e4, xPasswordStringsCoffin, "Coffin"),      DATA_COMPGEN(0x004f48ec, xPasswordStringsCommand, "Command"),    DATA_COMPGEN(0x004f48f4, xPasswordStringsControl, "Control"),
    DATA_COMPGEN(0x004f48fc, xPasswordStringsCower, "Cower"),      DATA_COMPGEN(0x004f4904, xPasswordStringsCreate, "Create"),      DATA_COMPGEN(0x004f490c, xPasswordStringsCrest, "Crest"),      DATA_COMPGEN(0x004f4914, xPasswordStringsCrossbow, "Crossbow"),    DATA_COMPGEN(0x004f4920, xPasswordStringsCrush, "Crush"),      DATA_COMPGEN(0x004f4928, xPasswordStringsCrypt, "Crypt"),
    DATA_COMPGEN(0x004f4930, xPasswordStringsCrystal, "Crystal"),    DATA_COMPGEN(0x004f4938, xPasswordStringsCurse, "Curse"),       DATA_COMPGEN(0x004f4940, xPasswordStringsCyclops, "Cyclops"),    DATA_COMPGEN(0x004f4948, xPasswordStringsDagger, "Dagger"),      DATA_COMPGEN(0x004f4950, xPasswordStringsDamage, "Damage"),     DATA_COMPGEN(0x004f4958, xPasswordStringsDarkness, "Darkness"),
    DATA_COMPGEN(0x004f4964, xPasswordStringsDeath, "Death"),      DATA_COMPGEN(0x004f496c, xPasswordStringsDecimate, "Decimate"),    DATA_COMPGEN(0x004f4978, xPasswordStringsDecisive, "Decisive"),   DATA_COMPGEN(0x004f4984, xPasswordStringsDefeat, "Defeat"),      DATA_COMPGEN(0x004f498c, xPasswordStringsDemon, "Demon"),      DATA_COMPGEN(0x004f4994, xPasswordStringsDesert, "Desert"),
    DATA_COMPGEN(0x004f499c, xPasswordStringsDestroy, "Destroy"),    DATA_COMPGEN(0x004f49a4, xPasswordStringsDevastation, "Devastation"), DATA_COMPGEN(0x004f49b0, xPasswordStringsDeviate, "Deviate"),    DATA_COMPGEN(0x004f49b8, xPasswordStringsDimension, "Dimension"),   DATA_COMPGEN(0x004f49c4, xPasswordStringsDiplomacy, "Diplomacy"),  DATA_COMPGEN(0x004f49d0, xPasswordStringsDiscover, "Discover"),
    DATA_COMPGEN(0x004f49dc, xPasswordStringsDisruption, "Disruption"), DATA_COMPGEN(0x004f49e8, xPasswordStringsDistance, "Distance"),    DATA_COMPGEN(0x004f49f4, xPasswordStringsDragon, "Dragon"),     DATA_COMPGEN(0x004f49fc, xPasswordStringsDruid, "Druid"),       DATA_COMPGEN(0x004f4a04, xPasswordStringsDungeon, "Dungeon"),    DATA_COMPGEN(0x004f4a0c, xPasswordStringsDwarven, "Dwarven"),
    DATA_COMPGEN(0x004f4a14, xPasswordStringsDwelling, "Dwelling"),   DATA_COMPGEN(0x004f4a20, xPasswordStringsEarthquake, "Earthquake"),  DATA_COMPGEN(0x004f4a2c, xPasswordStringsElemental, "Elemental"),  DATA_COMPGEN(0x004f4a38, xPasswordStringsElixir, "Elixir"),      DATA_COMPGEN(0x004f4a40, xPasswordStringsElven, "Elven"),      DATA_COMPGEN(0x004f4a48, xPasswordStringsEmblem, "Emblem"),
    DATA_COMPGEN(0x004f4a50, xPasswordStringsEngage, "Engage"),     DATA_COMPGEN(0x004f4a58, xPasswordStringsEnrage, "Enrage"),      DATA_COMPGEN(0x004f4a60, xPasswordStringsEntertain, "Entertain"),  DATA_COMPGEN(0x004f4a6c, xPasswordStringsEvil, "Evil"),        DATA_COMPGEN(0x004f4a74, xPasswordStringsFear, "Fear"),       DATA_COMPGEN(0x004f4a7c, xPasswordStringsFire, "Fire"),
    DATA_COMPGEN(0x004f4a84, xPasswordStringsFireball, "Fireball"),   DATA_COMPGEN(0x004f4a90, xPasswordStringsFlail, "Flail"),       DATA_COMPGEN(0x004f4a98, xPasswordStringsFlask, "Flask"),      DATA_COMPGEN(0x004f4aa0, xPasswordStringsFlood, "Flood"),       DATA_COMPGEN(0x004f4aa8, xPasswordStringsFollower, "Follower"),   DATA_COMPGEN(0x004f4ab4, xPasswordStringsFortunate, "Fortunate"),
    DATA_COMPGEN(0x004f4ac0, xPasswordStringsFortune, "Fortune"),    DATA_COMPGEN(0x004f4ac8, xPasswordStringsGargoyle, "Gargoyle"),    DATA_COMPGEN(0x004f4ad4, xPasswordStringsGauntlets, "Gauntlets"),  DATA_COMPGEN(0x004f4ae0, xPasswordStringsGems, "Gems"),        DATA_COMPGEN(0x004f4ae8, xPasswordStringsGenie, "Genie"),      DATA_COMPGEN(0x004f4af0, xPasswordStringsGhost, "Ghost"),
    DATA_COMPGEN(0x004f4af8, xPasswordStringsGiants, "Giants"),     DATA_COMPGEN(0x004f4b00, xPasswordStringsGloves, "Gloves"),      DATA_COMPGEN(0x004f4b08, xPasswordStringsGoblin, "Goblin"),     DATA_COMPGEN(0x004f4b10, xPasswordStringsGold, "Gold"),        DATA_COMPGEN(0x004f4b18, xPasswordStringsGolem, "Golem"),      DATA_COMPGEN(0x004f4b20, xPasswordStringsGraveyard, "Graveyard"),
    DATA_COMPGEN(0x004f4b2c, xPasswordStringsGravity, "Gravity"),    DATA_COMPGEN(0x004f4b34, xPasswordStringsGuard, "Guard"),       DATA_COMPGEN(0x004f4b3c, xPasswordStringsGuild, "Guild"),      DATA_COMPGEN(0x004f4b44, xPasswordStringsHalberd, "Halberd"),     DATA_COMPGEN(0x004f4b4c, xPasswordStringsHammer, "Hammer"),     DATA_COMPGEN(0x004f4b54, xPasswordStringsHarp, "Harp"),
    DATA_COMPGEN(0x004f4b5c, xPasswordStringsHaunt, "Haunt"),      DATA_COMPGEN(0x004f4b64, xPasswordStringsHelm, "Helm"),        DATA_COMPGEN(0x004f4b6c, xPasswordStringsHeroic, "Heroic"),     DATA_COMPGEN(0x004f4b74, xPasswordStringsHonor, "Honor"),       DATA_COMPGEN(0x004f4b7c, xPasswordStringsHope, "Hope"),       DATA_COMPGEN(0x004f4b84, xPasswordStringsHorde, "Horde"),
    DATA_COMPGEN(0x004f4b8c, xPasswordStringsHorse, "Horse"),      DATA_COMPGEN(0x004f4b94, xPasswordStringsHorseshoe, "Horseshoe"),   DATA_COMPGEN(0x004f4ba0, xPasswordStringsHydra, "Hydra"),      DATA_COMPGEN(0x004f4ba8, xPasswordStringsIdentify, "Identify"),    DATA_COMPGEN(0x004f4bb4, xPasswordStringsIgnite, "Ignite"),     DATA_COMPGEN(0x004f4bbc, xPasswordStringsIllusion, "Illusion"),
    DATA_COMPGEN(0x004f4bc8, xPasswordStringsInvisible, "Invisible"),  DATA_COMPGEN(0x004f4bd4, xPasswordStringsKingdom, "Kingdom"),     DATA_COMPGEN(0x004f4bdc, xPasswordStringsKnife, "Knife"),      DATA_COMPGEN(0x004f4be4, xPasswordStringsKnighthood, "Knighthood"),  DATA_COMPGEN(0x004f4bf0, xPasswordStringsLance, "Lance"),      DATA_COMPGEN(0x004f4bf8, xPasswordStringsLantern, "Lantern"),
    DATA_COMPGEN(0x004f4c00, xPasswordStringsLeadership, "Leadership"), DATA_COMPGEN(0x004f4c0c, xPasswordStringsLeather, "Leather"),     DATA_COMPGEN(0x004f4c14, xPasswordStringsLibrary, "Library"),    DATA_COMPGEN(0x004f4c1c, xPasswordStringsLiege, "Liege"),       DATA_COMPGEN(0x004f4c24, xPasswordStringsLife, "Life"),       DATA_COMPGEN(0x004f4c2c, xPasswordStringsLightning, "Lightning"),
    DATA_COMPGEN(0x004f4c38, xPasswordStringsLord, "Lord"),       DATA_COMPGEN(0x004f4c40, xPasswordStringsMace, "Mace"),        DATA_COMPGEN(0x004f4c48, xPasswordStringsMage, "Mage"),       DATA_COMPGEN(0x004f4c50, xPasswordStringsMagic, "Magic"),       DATA_COMPGEN(0x004f4c58, xPasswordStringsMajesty, "Majesty"),    DATA_COMPGEN(0x004f4c60, xPasswordStringsMalevolent, "Malevolent"),
    DATA_COMPGEN(0x004f4c6c, xPasswordStringsMedusa, "Medusa"),     DATA_COMPGEN(0x004f4c74, xPasswordStringsMeteor, "Meteor"),      DATA_COMPGEN(0x004f4c7c, xPasswordStringsMight, "Might"),      DATA_COMPGEN(0x004f4c84, xPasswordStringsMinotaur, "Minotaur"),    DATA_COMPGEN(0x004f4c90, xPasswordStringsMinutemen, "Minutemen"),  DATA_COMPGEN(0x004f4c9c, xPasswordStringsMissile, "Missile"),
    DATA_COMPGEN(0x004f4ca4, xPasswordStringsMonster, "Monster"),    DATA_COMPGEN(0x004f4cac, xPasswordStringsMummy, "Mummy"),       DATA_COMPGEN(0x004f4cb4, xPasswordStringsNavigation, "Navigation"), DATA_COMPGEN(0x004f4cc0, xPasswordStringsNecromancy, "Necromancy"),  DATA_COMPGEN(0x004f4ccc, xPasswordStringsNoho, "Noho"),       DATA_COMPGEN(0x004f4cd4, xPasswordStringsOgre, "Ogre"),
    DATA_COMPGEN(0x004f4cdc, xPasswordStringsOintment, "Ointment"),   DATA_COMPGEN(0x004f4ce8, xPasswordStringsPaladin, "Paladin"),     DATA_COMPGEN(0x004f4cf0, xPasswordStringsParalyze, "Paralyze"),   DATA_COMPGEN(0x004f4cfc, xPasswordStringsParry, "Parry"),       DATA_COMPGEN(0x004f4d04, xPasswordStringsPartake, "Partake"),    DATA_COMPGEN(0x004f4d0c, xPasswordStringsPhoenix, "Phoenix"),
    DATA_COMPGEN(0x004f4d14, xPasswordStringsPortal, "Portal"),     DATA_COMPGEN(0x004f4d1c, xPasswordStringsPotion, "Potion"),      DATA_COMPGEN(0x004f4d24, xPasswordStringsPouch, "Pouch"),      DATA_COMPGEN(0x004f4d2c, xPasswordStringsPowder, "Powder"),      DATA_COMPGEN(0x004f4d34, xPasswordStringsPower, "Power"),      DATA_COMPGEN(0x004f4d3c, xPasswordStringsPyramid, "Pyramid"),
    DATA_COMPGEN(0x004f4d44, xPasswordStringsQuest, "Quest"),      DATA_COMPGEN(0x004f4d4c, xPasswordStringsRainbow, "Rainbow"),     DATA_COMPGEN(0x004f4d54, xPasswordStringsRanger, "Ranger"),     DATA_COMPGEN(0x004f4d5c, xPasswordStringsRecruit, "Recruit"),     DATA_COMPGEN(0x004f4d64, xPasswordStringsRescue, "Rescue"),     DATA_COMPGEN(0x004f4d6c, xPasswordStringsResurrect, "Resurrect"),
    DATA_COMPGEN(0x004f4d78, xPasswordStringsRetreat, "Retreat"),    DATA_COMPGEN(0x004f4d80, xPasswordStringsRighteous, "Righteous"),   DATA_COMPGEN(0x004f4d8c, xPasswordStringsRobe, "Robe"),       DATA_COMPGEN(0x004f4d94, xPasswordStringsRogue, "Rogue"),       DATA_COMPGEN(0x004f4d9c, xPasswordStringsRope, "Rope"),       DATA_COMPGEN(0x004f4da4, xPasswordStringsScabbard, "Scabbard"),
    DATA_COMPGEN(0x004f4db0, xPasswordStringsScarab, "Scarab"),     DATA_COMPGEN(0x004f4db8, xPasswordStringsScimitar, "Scimitar"),    DATA_COMPGEN(0x004f4dc4, xPasswordStringsScroll, "Scroll"),     DATA_COMPGEN(0x004f4dcc, xPasswordStringsShield, "Shield"),      DATA_COMPGEN(0x004f4dd4, xPasswordStringsShout, "Shout"),      DATA_COMPGEN(0x004f4ddc, xPasswordStringsShrine, "Shrine"),
    DATA_COMPGEN(0x004f4de4, xPasswordStringsSiege, "Siege"),      DATA_COMPGEN(0x004f4dec, xPasswordStringsSkeleton, "Skeleton"),    DATA_COMPGEN(0x004f4df8, xPasswordStringsSkill, "Skill"),      DATA_COMPGEN(0x004f4e00, xPasswordStringsSlayer, "Slayer"),      DATA_COMPGEN(0x004f4e08, xPasswordStringsSling, "Sling"),      DATA_COMPGEN(0x004f4e10, xPasswordStringsSorceress, "Sorceress"),
    DATA_COMPGEN(0x004f4e1c, xPasswordStringsSpeed, "Speed"),      DATA_COMPGEN(0x004f4e24, xPasswordStringsStaff, "Staff"),       DATA_COMPGEN(0x004f4e2c, xPasswordStringsStatue, "Statue"),     DATA_COMPGEN(0x004f4e34, xPasswordStringsStorm, "Storm"),       DATA_COMPGEN(0x004f4e3c, xPasswordStringsSulfur, "Sulfur"),     DATA_COMPGEN(0x004f4e44, xPasswordStringsSummon, "Summon"),
    DATA_COMPGEN(0x004f4e4c, xPasswordStringsSwamp, "Swamp"),      DATA_COMPGEN(0x004f4e54, xPasswordStringsSword, "Sword"),       DATA_COMPGEN(0x004f4e5c, xPasswordStringsTelepathy, "Telepathy"),  DATA_COMPGEN(0x004f4e68, xPasswordStringsTerror, "Terror"),      DATA_COMPGEN(0x004f4e70, xPasswordStringsThunder, "Thunder"),    DATA_COMPGEN(0x004f4e78, xPasswordStringsTorch, "Torch"),
    DATA_COMPGEN(0x004f4e80, xPasswordStringsTower, "Tower"),      DATA_COMPGEN(0x004f4e88, xPasswordStringsTravesty, "Travesty"),    DATA_COMPGEN(0x004f4e94, xPasswordStringsTreaty, "Treaty"),     DATA_COMPGEN(0x004f4e9c, xPasswordStringsTrident, "Trident"),     DATA_COMPGEN(0x004f4ea4, xPasswordStringsTroll, "Troll"),      DATA_COMPGEN(0x004f4eac, xPasswordStringsUnicorn, "Unicorn"),
    DATA_COMPGEN(0x004f4eb4, xPasswordStringsVampire, "Vampire"),    DATA_COMPGEN(0x004f4ebc, xPasswordStringsVenom, "Venom"),       DATA_COMPGEN(0x004f4ec4, xPasswordStringsVictory, "Victory"),    DATA_COMPGEN(0x004f4ecc, xPasswordStringsVolcano, "Volcano"),     DATA_COMPGEN(0x004f4ed4, xPasswordStringsVulnerable, "Vulnerable"), DATA_COMPGEN(0x004f4ee0, xPasswordStringsWand, "Wand"),
    DATA_COMPGEN(0x004f4ee8, xPasswordStringsWarfare, "Warfare"),    DATA_COMPGEN(0x004f4ef0, xPasswordStringsWarlock, "Warlock"),     DATA_COMPGEN(0x004f4ef8, xPasswordStringsWeapon, "Weapon"),     DATA_COMPGEN(0x004f4f00, xPasswordStringsWings, "Wings"),       DATA_COMPGEN(0x004f4f08, xPasswordStringsWizardry, "Wizardry"),   DATA_COMPGEN(0x004f4f14, xPasswordStringsWolf, "Wolf"),
    DATA_COMPGEN(0x004f4f1c, xPasswordStringsZombie, "Zombie")
};
DATA(0x004f2a9c) i8 xNetHasOldPlayers = 0;
DATA(0x004f2aa0) u8 xNewHeroAlignment[X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT] = {
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_NECROMANCER),
    IDX(FACTION_BARBARIAN),
    IDX(FACTION_WIZARD),
    IDX(FACTION_SORCERESS),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_NECROMANCER),
    IDX(FACTION_BARBARIAN),
    IDX(FACTION_WARLOCK),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT),
    IDX(FACTION_KNIGHT)
};
DATA(0x004f2ab0) i32
    xShrineBuildingCost[X_GLOBAL_BUILDING_RESOURCE_COUNT] = {10, 0, 0, 0, 10, 0, 4000};
DATA(0x00527e78) i32 xPasswordStringsIndex[X_GLOBAL_PASSWORD_STRING_INDEX_COUNT];
DATA(0x00527e98) i8 xIsExpansionMap;
DATA(0x00527e9c) ExpansionCampaignId xLastChoice;
DATA(0x00527e20) u8 xIsPlayingExpansionCampaign;
DATA_COMPGEN_GUARD(0x004ec008, xCampaignInitializationGuard, xCampaign)
DATA(0x00527e28) ExpCampaign xCampaign;

VA_COMPGEN(0x004476a0, 0x1a, STATIC_INIT_DISPATCH, xCampaign)
VA_COMPGEN(0x004476ba, 0x1d, STATIC_ATEXIT, xCampaign)
VA_COMPGEN(0x004476d7, 0x1a, STATIC_DTOR, xCampaign)
VA_COMPGEN(0x004476f1, 0x1a, STATIC_CTOR, xCampaign)
DATA(0x00527ea0) i32 xTheSpell;
