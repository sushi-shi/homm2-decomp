#include <Ints.h>
#include <SOURCE/ExpCampaign.h>
#include <SOURCE/X_GLOBAL.h>
#include <SOURCE/KB_TYPES.h>


const char* xSetupCampaignGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {


    localization::Tr("table.xSetupCampaignGameHelp.0"),


    localization::Tr("table.xSetupCampaignGameHelp.1"),


    localization::Tr("table.xSetupCampaignGameHelp.2")
};
const char* xSetupStandardGameHelp[X_GLOBAL_SETUP_HELP_COUNT] = {


    localization::Tr("table.xSetupStandardGameHelp.0"),


    localization::Tr("table.xSetupStandardGameHelp.1"),


    localization::Tr("table.xSetupStandardGameHelp.2")
};
const char* xCampaignAwards[X_GLOBAL_EXPANSION_CAMPAIGN_AWARD_COUNT] = {
     localization::Tr("table.xCampaignAwards.0"),
     localization::Tr("table.xCampaignAwards.1"),
     localization::Tr("table.xCampaignAwards.2"),
     localization::Tr("table.xCampaignAwards.3"),
     localization::Tr("table.xCampaignAwards.4"),
     localization::Tr("table.xCampaignAwards.5"),
     localization::Tr("table.xCampaignAwards.6"),
     localization::Tr("table.xCampaignAwards.7"),
     localization::Tr("table.xCampaignAwards.8"),
     localization::Tr("table.xCampaignAwards.9"),
     localization::Tr("table.xCampaignAwards.10")
};
const char*
    xScenarioName[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT][X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] = {
        { localization::Tr("table.xScenarioName.0.0"),
          localization::Tr("table.xScenarioName.0.1"),
          localization::Tr("table.xScenarioName.0.2"),
          localization::Tr("table.xScenarioName.0.3"),
          localization::Tr("table.xScenarioName.0.4"),
          localization::Tr("table.xScenarioName.0.5"),
          localization::Tr("table.xScenarioName.0.6"),
          localization::Tr("table.xScenarioName.0.7")},
        { localization::Tr("table.xScenarioName.1.0"),
          localization::Tr("table.xScenarioName.1.1"),
          localization::Tr("table.xScenarioName.1.2"),
          localization::Tr("table.xScenarioName.1.3"),
          localization::Tr("table.xScenarioName.1.4"),
          localization::Tr("table.xScenarioName.1.5"),
          localization::Tr("table.xScenarioName.1.6"),
          localization::Tr("table.xScenarioName.1.7")},
        { localization::Tr("table.xScenarioName.2.0"),
          localization::Tr("table.xScenarioName.2.1"),
          localization::Tr("table.xScenarioName.2.2"),
          localization::Tr("table.xScenarioName.2.3"),
         "",
         "",
         "",
         ""},
        { localization::Tr("table.xScenarioName.3.0"),
          localization::Tr("table.xScenarioName.3.1"),
          localization::Tr("table.xScenarioName.3.2"),
          localization::Tr("table.xScenarioName.3.3"),
         "",
         "",
         "",
         ""}
};
const char*
    xScenarioDescription[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT][X_GLOBAL_EXPANSION_CAMPAIGN_MAP_COUNT] =
        {
            {
                 localization::Tr("table.xScenarioDescription.0.0"),
                 localization::Tr("table.xScenarioDescription.0.1"),
                 localization::Tr("table.xScenarioDescription.0.2"),
                 localization::Tr("table.xScenarioDescription.0.3"),
                 localization::Tr("table.xScenarioDescription.0.4"),
                 localization::Tr("table.xScenarioDescription.0.5"),
                 localization::Tr("table.xScenarioDescription.0.6"),
                 localization::Tr("table.xScenarioDescription.0.7")
            },
            {
                 localization::Tr("table.xScenarioDescription.1.0"),
                 localization::Tr("table.xScenarioDescription.1.1"),
                 localization::Tr("table.xScenarioDescription.1.2"),
                 localization::Tr("table.xScenarioDescription.1.3"),
                 localization::Tr("table.xScenarioDescription.1.4"),
                 localization::Tr("table.xScenarioDescription.1.5"),
                 localization::Tr("table.xScenarioDescription.1.6"),
                 localization::Tr("table.xScenarioDescription.1.7")
            },
            {
                 localization::Tr("table.xScenarioDescription.2.0"),
                 localization::Tr("table.xScenarioDescription.2.1"),
                 localization::Tr("table.xScenarioDescription.2.2"),
                 localization::Tr("table.xScenarioDescription.2.3"),
                "",
                "",
                "",
                ""
            },
            {
                 localization::Tr("table.xScenarioDescription.3.0"),
                 localization::Tr("table.xScenarioDescription.3.1"),
                 localization::Tr("table.xScenarioDescription.3.2"),
                 localization::Tr("table.xScenarioDescription.3.3"),
                "",
                "",
                "",
                ""
            }
        };
const char* xShortCampaignNames[X_GLOBAL_EXPANSION_CAMPAIGN_COUNT] = {
     localization::Tr("table.xShortCampaignNames.0"),
     localization::Tr("table.xShortCampaignNames.1"),
     localization::Tr("table.xShortCampaignNames.2"),
     localization::Tr("table.xShortCampaignNames.3")
};
const char* xHSCampaignNames[EXPANSION_HIGH_SCORE_CAMPAIGN_COUNT] = {
      localization::Tr("table.xHSCampaignNames.0"),
     localization::Tr("table.xHSCampaignNames.1"),
     localization::Tr("table.xHSCampaignNames.2"),
     localization::Tr("table.xHSCampaignNames.3")
};
const char* xRecruitEmpty[X_GLOBAL_RECRUIT_EMPTY_COUNT] = {
     localization::Tr("table.xRecruitEmpty.0"),
     localization::Tr("table.xRecruitEmpty.1"),
     localization::Tr("table.xRecruitEmpty.2"),
     localization::Tr("table.xRecruitEmpty.3"),
     localization::Tr("table.xRecruitEmpty.4")
};
const char* xRecruitBuy[X_GLOBAL_RECRUIT_BUY_COUNT] = {
     localization::Tr("table.xRecruitBuy.0"),
     localization::Tr("table.xRecruitBuy.1"),
     localization::Tr("table.xRecruitBuy.2"),
     localization::Tr("table.xRecruitBuy.3"),
     localization::Tr("table.xRecruitBuy.4")
};
const char* xNecromancerShrine =
     localization::Tr("site.necromancer_shrine.name");
const char* xNecromancerShrineDesc =
     localization::Tr("site.necromancer_shrine.description");
const char* xStableText[X_GLOBAL_STABLE_TEXT_COUNT] = {
     localization::Tr("table.xStableText.0"),
     localization::Tr("table.xStableText.1"),
     localization::Tr("table.xStableText.2"),
     localization::Tr("table.xStableText.3")
};
const char* xJosephName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] = {
     localization::Tr("table.xJosephName.0"),
     localization::Tr("table.xJosephName.1"),
     localization::Tr("table.xJosephName.2"),
     localization::Tr("table.xJosephName.3"),
     localization::Tr("table.xJosephName.4"),
     localization::Tr("table.xJosephName.5")
};
const char* xUncleIvanName[X_GLOBAL_EXPANSION_HERO_NAME_COUNT] = {
     localization::Tr("table.xUncleIvanName.0"),
     localization::Tr("table.xUncleIvanName.1"),
     localization::Tr("table.xUncleIvanName.2"),
     localization::Tr("table.xUncleIvanName.3"),
     localization::Tr("table.xUncleIvanName.4"),
     localization::Tr("table.xUncleIvanName.5")
};
const char* xShortSSLevelNames[X_GLOBAL_SHORT_SKILL_LEVEL_COUNT] = {
     localization::Tr("table.xShortSSLevelNames.0"),
     localization::Tr("table.xShortSSLevelNames.1"),
     localization::Tr("table.xShortSSLevelNames.2")
};
const char* xPasswordStrings[X_GLOBAL_PASSWORD_STRING_COUNT] = {
     localization::Tr("table.xPasswordStrings.0"),
     localization::Tr("table.xPasswordStrings.1"),
     localization::Tr("table.xPasswordStrings.2"),
     localization::Tr("table.xPasswordStrings.3"),
     localization::Tr("table.xPasswordStrings.4"),
     localization::Tr("table.xPasswordStrings.5"),
     localization::Tr("table.xPasswordStrings.6"),
     localization::Tr("table.xPasswordStrings.7"),
     localization::Tr("table.xPasswordStrings.8"),
     localization::Tr("table.xPasswordStrings.9"),
     localization::Tr("table.xPasswordStrings.10"),
     localization::Tr("table.xPasswordStrings.11"),
     localization::Tr("table.xPasswordStrings.12"),
     localization::Tr("table.xPasswordStrings.13"),
     localization::Tr("table.xPasswordStrings.14"),
     localization::Tr("table.xPasswordStrings.15"),
     localization::Tr("table.xPasswordStrings.16"),
     localization::Tr("table.xPasswordStrings.17"),
     localization::Tr("table.xPasswordStrings.18"),
     localization::Tr("table.xPasswordStrings.19"),
     localization::Tr("table.xPasswordStrings.20"),
     localization::Tr("table.xPasswordStrings.21"),
     localization::Tr("table.xPasswordStrings.22"),
     localization::Tr("table.xPasswordStrings.23"),
     localization::Tr("table.xPasswordStrings.24"),
     localization::Tr("table.xPasswordStrings.25"),
     localization::Tr("table.xPasswordStrings.26"),
     localization::Tr("table.xPasswordStrings.27"),
     localization::Tr("table.xPasswordStrings.28"),
     localization::Tr("table.xPasswordStrings.29"),
     localization::Tr("table.xPasswordStrings.30"),
     localization::Tr("table.xPasswordStrings.31"),
     localization::Tr("table.xPasswordStrings.32"),
     localization::Tr("table.xPasswordStrings.33"),
     localization::Tr("table.xPasswordStrings.34"),
     localization::Tr("table.xPasswordStrings.35"),
     localization::Tr("table.xPasswordStrings.36"),
     localization::Tr("table.xPasswordStrings.37"),
     localization::Tr("table.xPasswordStrings.38"),
     localization::Tr("table.xPasswordStrings.39"),
     localization::Tr("table.xPasswordStrings.40"),
     localization::Tr("table.xPasswordStrings.41"),
     localization::Tr("table.xPasswordStrings.42"),
     localization::Tr("table.xPasswordStrings.43"),
     localization::Tr("table.xPasswordStrings.44"),
     localization::Tr("table.xPasswordStrings.45"),
     localization::Tr("table.xPasswordStrings.46"),
     localization::Tr("table.xPasswordStrings.47"),
     localization::Tr("table.xPasswordStrings.48"),
     localization::Tr("table.xPasswordStrings.49"),
     localization::Tr("table.xPasswordStrings.50"),
     localization::Tr("table.xPasswordStrings.51"),
     localization::Tr("table.xPasswordStrings.52"),
     localization::Tr("table.xPasswordStrings.53"),
     localization::Tr("table.xPasswordStrings.54"),
     localization::Tr("table.xPasswordStrings.55"),
     localization::Tr("table.xPasswordStrings.56"),
     localization::Tr("table.xPasswordStrings.57"),
     localization::Tr("table.xPasswordStrings.58"),
     localization::Tr("table.xPasswordStrings.59"),
     localization::Tr("table.xPasswordStrings.60"),
     localization::Tr("table.xPasswordStrings.61"),
     localization::Tr("table.xPasswordStrings.62"),
     localization::Tr("table.xPasswordStrings.63"),
     localization::Tr("table.xPasswordStrings.64"),
     localization::Tr("table.xPasswordStrings.65"),
     localization::Tr("table.xPasswordStrings.66"),
     localization::Tr("table.xPasswordStrings.67"),
     localization::Tr("table.xPasswordStrings.68"),
     localization::Tr("table.xPasswordStrings.69"),
      "Elven",
     localization::Tr("table.xPasswordStrings.71"),
     localization::Tr("table.xPasswordStrings.72"),
     localization::Tr("table.xPasswordStrings.73"),
     localization::Tr("table.xPasswordStrings.74"),
     localization::Tr("table.xPasswordStrings.75"),
     localization::Tr("table.xPasswordStrings.76"),
     localization::Tr("table.xPasswordStrings.77"),
     localization::Tr("table.xPasswordStrings.78"),
     localization::Tr("table.xPasswordStrings.79"),
     localization::Tr("table.xPasswordStrings.80"),
     localization::Tr("table.xPasswordStrings.81"),
     localization::Tr("table.xPasswordStrings.82"),
     localization::Tr("table.xPasswordStrings.83"),
     localization::Tr("table.xPasswordStrings.84"),
     localization::Tr("table.xPasswordStrings.85"),
     localization::Tr("table.xPasswordStrings.86"),
     localization::Tr("table.xPasswordStrings.87"),
     localization::Tr("table.xPasswordStrings.88"),
     localization::Tr("table.xPasswordStrings.89"),
     localization::Tr("table.xPasswordStrings.90"),
     localization::Tr("table.xPasswordStrings.91"),
     localization::Tr("table.xPasswordStrings.92"),
     localization::Tr("table.xPasswordStrings.93"),
     localization::Tr("table.xPasswordStrings.94"),
     localization::Tr("table.xPasswordStrings.95"),
     localization::Tr("table.xPasswordStrings.96"),
     localization::Tr("table.xPasswordStrings.97"),
     localization::Tr("table.xPasswordStrings.98"),
     localization::Tr("table.xPasswordStrings.99"),
     localization::Tr("table.xPasswordStrings.100"),
     localization::Tr("table.xPasswordStrings.101"),
     localization::Tr("table.xPasswordStrings.102"),
     localization::Tr("table.xPasswordStrings.103"),
     localization::Tr("table.xPasswordStrings.104"),
     localization::Tr("table.xPasswordStrings.105"),
     localization::Tr("table.xPasswordStrings.106"),
     localization::Tr("table.xPasswordStrings.107"),
     localization::Tr("table.xPasswordStrings.108"),
     localization::Tr("table.xPasswordStrings.109"),
     localization::Tr("table.xPasswordStrings.110"),
     localization::Tr("table.xPasswordStrings.111"),
     localization::Tr("table.xPasswordStrings.112"),
     localization::Tr("table.xPasswordStrings.113"),
     localization::Tr("table.xPasswordStrings.114"),
     localization::Tr("table.xPasswordStrings.115"),
     localization::Tr("table.xPasswordStrings.116"),
     localization::Tr("table.xPasswordStrings.117"),
     localization::Tr("table.xPasswordStrings.118"),
     localization::Tr("table.xPasswordStrings.119"),
     localization::Tr("table.xPasswordStrings.120"),
     localization::Tr("table.xPasswordStrings.121"),
     localization::Tr("table.xPasswordStrings.122"),
     localization::Tr("table.xPasswordStrings.123"),
     localization::Tr("table.xPasswordStrings.124"),
     localization::Tr("table.xPasswordStrings.125"),
     localization::Tr("table.xPasswordStrings.126"),
     localization::Tr("table.xPasswordStrings.127"),
     localization::Tr("table.xPasswordStrings.128"),
     localization::Tr("table.xPasswordStrings.129"),
     localization::Tr("table.xPasswordStrings.130"),
     localization::Tr("table.xPasswordStrings.131"),
     localization::Tr("table.xPasswordStrings.132"),
     localization::Tr("table.xPasswordStrings.133"),
     localization::Tr("table.xPasswordStrings.134"),
     localization::Tr("table.xPasswordStrings.135"),
     localization::Tr("table.xPasswordStrings.136"),
     localization::Tr("table.xPasswordStrings.137"),
     localization::Tr("table.xPasswordStrings.138"),
     localization::Tr("table.xPasswordStrings.139"),
     localization::Tr("table.xPasswordStrings.140"),
     localization::Tr("table.xPasswordStrings.141"),
     localization::Tr("table.xPasswordStrings.142"),
     localization::Tr("table.xPasswordStrings.143"),
     localization::Tr("table.xPasswordStrings.144"),
     localization::Tr("table.xPasswordStrings.145"),
     localization::Tr("table.xPasswordStrings.146"),
     localization::Tr("table.xPasswordStrings.147"),
     localization::Tr("table.xPasswordStrings.148"),
     localization::Tr("table.xPasswordStrings.149"),
     localization::Tr("table.xPasswordStrings.150"),
     localization::Tr("table.xPasswordStrings.151"),
     localization::Tr("table.xPasswordStrings.152"),
     localization::Tr("table.xPasswordStrings.153"),
     localization::Tr("table.xPasswordStrings.154"),
     localization::Tr("table.xPasswordStrings.155"),
     localization::Tr("table.xPasswordStrings.156"),
     localization::Tr("table.xPasswordStrings.157"),
     localization::Tr("table.xPasswordStrings.158"),
     localization::Tr("table.xPasswordStrings.159"),
     localization::Tr("table.xPasswordStrings.160"),
     localization::Tr("table.xPasswordStrings.161"),
     localization::Tr("table.xPasswordStrings.162"),
     localization::Tr("table.xPasswordStrings.163"),
     localization::Tr("table.xPasswordStrings.164"),
     localization::Tr("table.xPasswordStrings.165"),
     localization::Tr("table.xPasswordStrings.166"),
     localization::Tr("table.xPasswordStrings.167"),
     localization::Tr("table.xPasswordStrings.168"),
     localization::Tr("table.xPasswordStrings.169"),
     localization::Tr("table.xPasswordStrings.170"),
     localization::Tr("table.xPasswordStrings.171"),
     localization::Tr("table.xPasswordStrings.172"),
     localization::Tr("table.xPasswordStrings.173"),
     localization::Tr("table.xPasswordStrings.174"),
     localization::Tr("table.xPasswordStrings.175"),
     localization::Tr("table.xPasswordStrings.176"),
     localization::Tr("table.xPasswordStrings.177"),
     localization::Tr("table.xPasswordStrings.178"),
     localization::Tr("table.xPasswordStrings.179"),
     localization::Tr("table.xPasswordStrings.180"),
     localization::Tr("table.xPasswordStrings.181"),
     localization::Tr("table.xPasswordStrings.182"),
     localization::Tr("table.xPasswordStrings.183"),
     localization::Tr("table.xPasswordStrings.184"),
     localization::Tr("table.xPasswordStrings.185"),
     localization::Tr("table.xPasswordStrings.186"),
     localization::Tr("table.xPasswordStrings.187"),
     localization::Tr("table.xPasswordStrings.188"),
     localization::Tr("table.xPasswordStrings.189"),
     localization::Tr("table.xPasswordStrings.190"),
     localization::Tr("table.xPasswordStrings.191"),
     localization::Tr("table.xPasswordStrings.192"),
     localization::Tr("table.xPasswordStrings.193"),
     localization::Tr("table.xPasswordStrings.194"),
     localization::Tr("table.xPasswordStrings.195"),
     localization::Tr("table.xPasswordStrings.196"),
     localization::Tr("table.xPasswordStrings.197"),
     localization::Tr("table.xPasswordStrings.198"),
     localization::Tr("table.xPasswordStrings.199"),
     localization::Tr("table.xPasswordStrings.200"),
     localization::Tr("table.xPasswordStrings.201"),
     localization::Tr("table.xPasswordStrings.202"),
     localization::Tr("table.xPasswordStrings.203"),
     localization::Tr("table.xPasswordStrings.204"),
     localization::Tr("table.xPasswordStrings.205"),
     localization::Tr("table.xPasswordStrings.206"),
     localization::Tr("table.xPasswordStrings.207"),
     localization::Tr("table.xPasswordStrings.208"),
     localization::Tr("table.xPasswordStrings.209"),
     localization::Tr("table.xPasswordStrings.210")
};
b8 xNetHasOldPlayers = false;
u8 xNewHeroAlignment[X_GLOBAL_NEW_HERO_ALIGNMENT_COUNT] = {
    H2EnumIndex(FACTION_KNIGHT),
    H2EnumIndex(FACTION_KNIGHT),
    H2EnumIndex(FACTION_NECROMANCER),
    H2EnumIndex(FACTION_BARBARIAN),
    H2EnumIndex(FACTION_WIZARD),
    H2EnumIndex(FACTION_SORCERESS),
    H2EnumIndex(FACTION_KNIGHT),
    H2EnumIndex(FACTION_KNIGHT),
    H2EnumIndex(FACTION_NECROMANCER),
    H2EnumIndex(FACTION_BARBARIAN),
    H2EnumIndex(FACTION_WARLOCK),
    H2EnumIndex(FACTION_KNIGHT)
};
i32
    xShrineBuildingCost[H2EnumIndex(RES_COUNT)] = {10, 0, 0, 0, 10, 0, 4000};
i32 xPasswordStringsIndex[X_GLOBAL_PASSWORD_STRING_INDEX_COUNT];
b8 xIsExpansionMap;
ExpansionCampaignId xLastChoice;
u8 xIsPlayingExpansionCampaign;
ExpCampaign xCampaign;

i32 xTheSpell;
