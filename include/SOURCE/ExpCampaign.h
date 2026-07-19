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
    i32 m_campaignId;
    i32 m_currentMap;
    i32 m_mapCount;
    u8 m_mapChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_mapsPlayed[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    i16 m_mapDays[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    u8 m_awards[EXPANSION_CAMPAIGN_AWARD_COUNT];
    u8 m_bonusChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];
    char m_pad_0x3f[EXPANSION_CAMPAIGN_RUNTIME_GAP_SIZE];
    i32 m_viewMap;
    class heroWindow* m_window;
    i32 m_viewOnly;
    char m_pad_0x4f;
    ExpCampaign(void);
    ~ExpCampaign();
    void ResetMapChoices(void);
    void ResetMapsPlayed(void);
    void ResetAwards(void);
    void ResetBonusChoices(void);
    void GrantAward(i32);
    void RemoveAward(i32);
    i8 HasAward(i32);
    void SetMapWasPlayed(void);
    void InitNewCampaign(i32);
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
