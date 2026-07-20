#ifndef HOMM2_SOURCE_EXPCAMPAIGN_H
#define HOMM2_SOURCE_EXPCAMPAIGN_H

#include <va.h>
#include <SOURCE/Campaign.h>

class heroWindow;
struct tag_message;

H2_ENUM_CLASS_BEGIN(ExpansionCampaignId)
    EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY = 0,
    EXPANSION_CAMPAIGN_DESCENDANTS      = 1,
    EXPANSION_CAMPAIGN_WIZARDS_ISLE     = 2,
    EXPANSION_CAMPAIGN_VOYAGE_HOME      = 3,
    EXPANSION_CAMPAIGN_COUNT            = 4
H2_ENUM_CLASS_END(ExpansionCampaignId)

H2_ENUM_BEGIN(ExpansionCampaignMap)
    MAP_NONE                  = -1,
    MAP_POL_UPRISING          = 0,
    MAP_POL_ISLAND_OF_CHAOS   = 1,
    MAP_POL_ARROWS_FLIGHT     = 2,
    MAP_POL_ABYSS             = 3,
    MAP_POL_GIANTS_PASS       = 4,
    MAP_POL_AURORA_BOREALIS   = 5,
    MAP_POL_BETRAYALS_END     = 6,
    MAP_POL_CORRUPTIONS_HEART = 7,
    MAP_DES_CONQUER_AND_UNIFY = 0,
    MAP_DES_BORDER_TOWNS      = 1,
    MAP_DES_WAYWARD_SON       = 2,
    MAP_DES_UNCLE_IVAN        = 3,
    MAP_DES_SOUTHERN_WAR      = 4,
    MAP_DES_IVORY_GATES       = 5,
    MAP_DES_ELVEN_LANDS       = 6,
    MAP_DES_EPIC_BATTLE       = 7,
    MAP_WIZ_SHROUDED_ISLES    = 0,
    MAP_WIZ_ETERNAL_SCROLLS   = 1,
    MAP_WIZ_POWERS_END        = 2,
    MAP_WIZ_FOUNT_OF_WIZARDRY = 3,
    MAP_VOY_STRANDED          = 0,
    MAP_VOY_PIRATE_ISLES      = 1,
    MAP_VOY_KING_AND_COUNTRY  = 2,
    MAP_VOY_BLOOD_IS_THICKER  = 3
H2_ENUM_END(ExpansionCampaignMap)

H2_ENUM_CLASS_BEGIN(ExpansionCampaignAward)
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
H2_ENUM_CLASS_END(ExpansionCampaignAward)

H2_ENUM_BEGIN(ExpansionCampaignConstant)
    EXPANSION_CAMPAIGN_MAX_MAP_COUNT            = 8,
    EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT       = 3,
    EXPANSION_CAMPAIGN_AWARD_COUNT              = 11,
    EXPANSION_CAMPAIGN_RUNTIME_GAP_SIZE         = 4,
    EXPANSION_CAMPAIGN_PLAYER_SETUP_RESET_SIZE  = 0x41,
    EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE    = 52,
    EXPANSION_CAMPAIGN_HERO_COUNT               = 54,
    EXPANSION_CAMPAIGN_TRACK_ICON_SIZE          = 41,
    EXPANSION_CAMPAIGN_TRACK_ICON_COLOR         = 0x10,
    EXPANSION_CAMPAIGN_MAP_FILENAME_SIZE        = 13,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_LEVEL      = 1,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL            = 12,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT_LEVEL  = 2,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT        = 2,
    EXPANSION_CAMPAIGN_ICON_FRAME_BASE          = 15,
    EXPANSION_CAMPAIGN_MUSIC                    = 24,
    EXPANSION_CAMPAIGN_DAYS_PER_WEEK            = 7,
    EXPANSION_CAMPAIGN_DAYS_PER_MONTH           = 28,
    EXPANSION_CAMPAIGN_FIRST_ALTERNATE_NAME_MAP = 4,
    EXPANSION_CAMPAIGN_IVAN_NAME_OFFSET         = 6,
    EXPANSION_CAMPAIGN_MAIN_PLAYER              = 0,
    EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_X       = 5,
    EXPANSION_CAMPAIGN_GOLDEN_BOW_EVENT_Y       = 0
H2_ENUM_END(ExpansionCampaignConstant)

#pragma pack(push, 1)
class ExpCampaign {
public:
    ExpansionCampaignId m_campaignId;
    ExpansionCampaignMap m_currentMap;
    i32 m_mapCount;
    u8 m_mapChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_mapsPlayed[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    i16 m_mapDays[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_awards[EXPANSION_CAMPAIGN_AWARD_COUNT];
    u8 m_bonusChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    char m_pad_0x3f[EXPANSION_CAMPAIGN_RUNTIME_GAP_SIZE];
    ExpansionCampaignMap m_viewMap;
    class heroWindow* m_window;
    i32 m_viewOnly;
    char m_pad_0x4f;
    ExpCampaign(void);
    ~ExpCampaign();
    void ResetMapChoices(void);
    void ResetMapsPlayed(void);
    void ResetAwards(void);
    void ResetBonusChoices(void);
    void GrantAward(H2_ENUM_PARAM(ExpansionCampaignAward, i32));
    void RemoveAward(H2_ENUM_PARAM(ExpansionCampaignAward, i32));
    i8 HasAward(H2_ENUM_PARAM(ExpansionCampaignAward, i32));
    void SetMapWasPlayed(void);
    void InitNewCampaign(ExpansionCampaignId);
    void InitMap(void);
    void ShowInfo(i32, i32);
    void UpdateInfo(i32);
    i32 HandleVictory(void);
    void HandleVictory1(void);
    void HandleVictory2(void);
    void HandleVictory3(void);
    void HandleVictory4(void);
    void ReplaySmacker(void);
    void ReplaySmacker1(void);
    void ReplaySmacker2(void);
    void ReplaySmacker3(void);
    void ReplaySmacker4(void);
    u8 IsCompleted(void);
    i8 IsThisMapCompleted(void);

private:
    static i32 MessageHandler(struct tag_message&);

public:
    void Autosave(void);
    i32 Choose(void);
    i16 Days(void);
    i32 CampaignID(void);
    char* JosephName(void);
    char* IvanName(void);
    i8 IsSpecialGoldenBow(i32, i32);
    i8 IsSpecialUA(void);
    i8 IsSpecialLossCondition(i32);
};
#pragma pack(pop)
SIZE(ExpCampaign, 0x50);

extern struct SCampaignChoice xCampaignChoices[IDX(EXPANSION_CAMPAIGN_COUNT)]
                                              [EXPANSION_CAMPAIGN_MAX_MAP_COUNT]
                                              [EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT];
#endif
