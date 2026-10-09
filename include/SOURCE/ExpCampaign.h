#ifndef HOMM2_SOURCE_EXPCAMPAIGN_H
#define HOMM2_SOURCE_EXPCAMPAIGN_H

#include <H2/Ints.h>
#include <Domains.h>
#include <BASE/message.h>
#include <SOURCE/Campaign.h>
#include <SOURCE/KB.h>

class heroWindow;
struct tag_message;

enum class ExpansionCampaignId : i32 {
    EXPANSION_CAMPAIGN_NONE             = -1,
    EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY = 0,
    EXPANSION_CAMPAIGN_DESCENDANTS      = 1,
    EXPANSION_CAMPAIGN_WIZARDS_ISLE     = 2,
    EXPANSION_CAMPAIGN_VOYAGE_HOME      = 3,
    EXPANSION_CAMPAIGN_COUNT            = 4
};
using enum ExpansionCampaignId;
ENABLE_ENUM_STEPS(ExpansionCampaignId)

enum class ExpansionCampaignMap : i32 {
    MAP_NONE                                    = -1,
    MAP_FIRST                                   = 0,
    MAP_POL_UPRISING                            = 0,
    MAP_POL_ISLAND_OF_CHAOS                     = 1,
    MAP_POL_ARROWS_FLIGHT                       = 2,
    MAP_POL_ABYSS                               = 3,
    MAP_POL_GIANTS_PASS                         = 4,
    MAP_POL_AURORA_BOREALIS                     = 5,
    MAP_POL_BETRAYALS_END                       = 6,
    MAP_POL_CORRUPTIONS_HEART                   = 7,
    MAP_DES_CONQUER_AND_UNIFY                   = 0,
    MAP_DES_BORDER_TOWNS                        = 1,
    MAP_DES_WAYWARD_SON                         = 2,
    MAP_DES_UNCLE_IVAN                          = 3,
    MAP_DES_SOUTHERN_WAR                        = 4,
    MAP_DES_IVORY_GATES                         = 5,
    MAP_DES_ELVEN_LANDS                         = 6,
    MAP_DES_EPIC_BATTLE                         = 7,
    MAP_WIZ_SHROUDED_ISLES                      = 0,
    MAP_WIZ_ETERNAL_SCROLLS                     = 1,
    MAP_WIZ_POWERS_END                          = 2,
    MAP_WIZ_FOUNT_OF_WIZARDRY                   = 3,
    MAP_VOY_STRANDED                            = 0,
    MAP_VOY_PIRATE_ISLES                        = 1,
    MAP_VOY_KING_AND_COUNTRY                    = 2,
    MAP_VOY_BLOOD_IS_THICKER                    = 3,
    EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP = 4
};
using enum ExpansionCampaignMap;
ENABLE_ENUM_INDEX_OFFSETS(ExpansionCampaignMap)

enum class ExpansionCampaignAward : i32 {
    AWARD_ELVEN_ALLIANCE      = 0,
    AWARD_BREASTPLATE_ANDURAN = 1,
    AWARD_WOOD_BONUS          = 2,
    AWARD_HELMET_ANDURAN      = 3,
    AWARD_DEFEAT_KRAEGER      = 4,
    AWARD_BATTLE_GARB         = 5,
    AWARD_WAYWARD_SON         = 6,
    AWARD_UNCLE_IVAN          = 7,
    AWARD_LEGENDARY_SCEPTER   = 8,
    AWARD_SET_GUARDIAN        = 9,
    AWARD_SPHERE_NEGATION     = 10
};
using enum ExpansionCampaignAward;
ENABLE_ENUM_STEPS(ExpansionCampaignAward)

typedef enum ExpansionCampaignConstant {
    EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE = 52,
    EXPANSION_CAMPAIGN_MAX_MAP_COUNT      = 8,
    EXPANSION_CAMPAIGN_AWARD_COUNT        = 11,
    EXPANSION_CAMPAIGN_RUNTIME_GAP_SIZE   = 4,
    EXPANSION_CAMPAIGN_ICON_FRAME_BASE    = 15,
    EXPANSION_CAMPAIGN_MAIN_PLAYER        = 0,
    EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_X = 5,
    EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_Y = 0
} ExpansionCampaignConstant;

extern const char* xCampaignAwards[EXPANSION_CAMPAIGN_AWARD_COUNT];
extern const char* xScenarioName[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)][EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
extern const char* xScenarioDescription[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)]
                                         [EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
extern const char* xShortCampaignNames[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)];
extern const char* xHSCampaignNames[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)];

#pragma pack(push, 1)
class ExpCampaign {
public:
    ExpansionCampaignId m_campaignId;
    ExpansionCampaignMap m_currentMap;
    i32 m_mapCount;
    u8 m_mapsAvailable[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_mapsPlayed[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    i16 m_mapStartDays[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_awards[EXPANSION_CAMPAIGN_AWARD_COUNT];
    u8 m_bonusChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    char m_unused3f[EXPANSION_CAMPAIGN_RUNTIME_GAP_SIZE];
    ExpansionCampaignMap m_viewMap;
    class heroWindow* m_window;
    b32 m_viewOnly;
    ExpCampaign(void);
    ~ExpCampaign();
    void ResetMapChoices(void);
    void ResetMapsPlayed(void);
    void ResetAwards(void);
    void ResetBonusChoices(void);
    void GrantAward(ExpansionCampaignAward award);
    void RemoveAward(ExpansionCampaignAward award);
    i8 HasAward(ExpansionCampaignAward award);
    void SetMapWasPlayed(void);
    void InitNewCampaign(ExpansionCampaignId campaignId);
    void InitMap(void);
    void ShowInfo(b32 viewOnly, i32);
    void UpdateInfo(b32 redraw);
    b32 HandleVictory(void);
    void HandleVictory1(void);
    void HandleVictory2(void);
    void HandleVictory3(void);
    void HandleVictory4(void);
    void HandleVictoryCustomCampaign(void);
    void ReplaySmacker(void);
    void ReplaySmacker1(void);
    void ReplaySmacker2(void);
    void ReplaySmacker3(void);
    void ReplaySmacker4(void);
    void ReplaySmackerCustomCampaign(void);
    u8 IsCompleted(void);
    b8 IsThisMapCompleted(void);

private:
    static MessageDispatchResult MessageHandler(struct tag_message& message);

public:
    void Autosave(void);
    ExpansionCampaignId Choose(void);
    i16 Days(void);
    i32 CampaignID(void);
    const char* JosephName(void);
    const char* IvanName(void);
    b8 IsSpecialGoldenBow(i32 x, i32 y);
    b8 IsSpecialUA(void);
    b8 IsSpecialLossCondition(i32 playerIndex);
};
#pragma pack(pop)

extern struct SCampaignChoice xCampaignChoices[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)]
                                              [EXPANSION_CAMPAIGN_MAX_MAP_COUNT]
                                              [CAMPAIGN_BONUS_CHOICE_COUNT];
extern i32 expansionCampaignMapCounts[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)];
extern i32 expansionCampaignTrackXY[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)]
                                   [EXPANSION_CAMPAIGN_MAX_MAP_COUNT][2];
enum class GameDifficulty : i8;
extern H2EnumStorage<GameDifficulty, i8>
    expansionCampaignDifficulty[H2EnumIndex(EXPANSION_CAMPAIGN_COUNT)]
                               [EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
#endif
