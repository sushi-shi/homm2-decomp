#ifndef HOMM2_SOURCE_EXPCAMPAIGN_H
#define HOMM2_SOURCE_EXPCAMPAIGN_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 36 methods, 0 own-virtual, 0 static data.
#include <va.h>
#include <SOURCE/Campaign.h>
// forward declarations:
class heroWindow;
struct tag_message;

typedef enum ExpansionCampaignId {
    EXPANSION_CAMPAIGN_PRICE_OF_LOYALTY = 0,
    EXPANSION_CAMPAIGN_DESCENDANTS = 1,
    EXPANSION_CAMPAIGN_WIZARDS_ISLE = 2,
    EXPANSION_CAMPAIGN_VOYAGE_HOME = 3,
    EXPANSION_CAMPAIGN_COUNT = 4
} ExpansionCampaignId;

typedef enum ExpansionCampaignConstant {
    EXPANSION_CAMPAIGN_MAX_MAP_COUNT = 8,
    EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT = 3,
    EXPANSION_CAMPAIGN_AWARD_COUNT = 11,
    EXPANSION_CAMPAIGN_PLAYER_SETUP_RESET_SIZE = 0x41,
    EXPANSION_CAMPAIGN_ARMY_NAME_BUFFER_SIZE = 52,
    EXPANSION_CAMPAIGN_HERO_COUNT = 54,
    EXPANSION_CAMPAIGN_TRACK_ICON_SIZE = 41,
    EXPANSION_CAMPAIGN_TRACK_ICON_COLOR = 0x10,
    EXPANSION_CAMPAIGN_MAP_FILENAME_SIZE = 13,
    EXPANSION_CAMPAIGN_SCROLL_ARTIFACT = 0x56,
    EXPANSION_CAMPAIGN_SET_GUARDIAN_SPELL = 0x3d,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_LEVEL = 1,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL = 12,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT_LEVEL = 2,
    EXPANSION_CAMPAIGN_SPECIAL_SKILL_ALT = 2,
    EXPANSION_CAMPAIGN_NON_SCROLL_SPELL_FIRST = 0x22,
    EXPANSION_CAMPAIGN_NON_SCROLL_SPELL_SECOND = 0x27,
    EXPANSION_CAMPAIGN_ICON_FRAME_BASE = 15,
    EXPANSION_CAMPAIGN_MUSIC = 24
} ExpansionCampaignConstant;

typedef enum ExpansionCampaignHeroPortrait {
    EXPANSION_HERO_DAINWIN = 0x3d,
    EXPANSION_HERO_GALLAVANT = 0x41,
    EXPANSION_HERO_CEALLACH = 0x43
} ExpansionCampaignHeroPortrait;

typedef enum ExpansionCampaignAward {
    EXPANSION_AWARD_FIRST = 0,
    EXPANSION_AWARD_BREASTPLATE_ANDURAN = 1,
    EXPANSION_AWARD_THIRD = 2,
    EXPANSION_AWARD_HELMET_ANDURAN = 3,
    EXPANSION_AWARD_DEFEAT_KRAEGER = 4,
    EXPANSION_AWARD_BATTLE_GARB = 5,
    EXPANSION_AWARD_SEVENTH = 6,
    EXPANSION_AWARD_EIGHTH = 7,
    EXPANSION_AWARD_LEGENDARY_SCEPTER = 8,
    EXPANSION_AWARD_SET_GUARDIAN = 9,
    EXPANSION_AWARD_SPHERE_NEGATION = 10
} ExpansionCampaignAward;

typedef enum ExpansionCampaignResource {
    EXPANSION_CAMPAIGN_RESOURCE_WOOD = 0,
    EXPANSION_CAMPAIGN_RESOURCE_MERCURY = 1,
    EXPANSION_CAMPAIGN_RESOURCE_ORE = 2,
    EXPANSION_CAMPAIGN_RESOURCE_SULFUR = 3,
    EXPANSION_CAMPAIGN_RESOURCE_CRYSTAL = 4,
    EXPANSION_CAMPAIGN_RESOURCE_GEMS = 5,
    EXPANSION_CAMPAIGN_RESOURCE_GOLD = 6
} ExpansionCampaignResource;

typedef enum ExpansionCampaignChoiceConstant {
    EXPANSION_CAMPAIGN_CHOICE_UNUSED_VALUE = -1,
    EXPANSION_CAMPAIGN_CHOICE_NO_AMOUNT = -1,
    EXPANSION_CAMPAIGN_CHOICE_ZERO_AMOUNT = 0,
    EXPANSION_CAMPAIGN_ARTIFACT_AMOUNT_ONE = 1,
    EXPANSION_CAMPAIGN_PRIMARY_BONUS_ONE = 1,
    EXPANSION_CAMPAIGN_PRIMARY_BONUS_TWO = 2,
    EXPANSION_CAMPAIGN_CRUSADER_COUNT = 3,
    EXPANSION_CAMPAIGN_MAGE_COUNT = 5,
    EXPANSION_CAMPAIGN_WOOD_FIVE = 5,
    EXPANSION_CAMPAIGN_ORE_FIVE = 5,
    EXPANSION_CAMPAIGN_WOOD_TEN = 10,
    EXPANSION_CAMPAIGN_SULFUR_TEN = 10,
    EXPANSION_CAMPAIGN_CRYSTAL_TEN = 10,
    EXPANSION_CAMPAIGN_SULFUR_FIFTEEN = 15,
    EXPANSION_CAMPAIGN_WOOD_TWENTY = 20,
    EXPANSION_CAMPAIGN_WOOD_FIFTY = 50,
    EXPANSION_CAMPAIGN_GOLD_FIVE_HUNDRED = 500,
    EXPANSION_CAMPAIGN_GOLD_ONE_THOUSAND = 1000,
    EXPANSION_CAMPAIGN_GOLD_TWO_THOUSAND = 2000,
    EXPANSION_CAMPAIGN_GOLD_THREE_THOUSAND = 3000,
    EXPANSION_CAMPAIGN_GOLD_PENALTY_ONE_THOUSAND = -1000,
    EXPANSION_CAMPAIGN_WOOD_PENALTY_TEN = -10,
    EXPANSION_CAMPAIGN_MERCURY_PENALTY_TWO = -2,
    EXPANSION_CAMPAIGN_SPELL_VIEW_HEROES = 52
} ExpansionCampaignChoiceConstant;

#pragma pack(push, 1)  // recovered layout is byte-packed
class ExpCampaign {
public:
    int m_campaignId;  // +0x00
    int m_currentMap;  // +0x04
    int m_mapCount;  // +0x08
    unsigned char m_mapChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];  // +0x0c
    unsigned char m_mapsPlayed[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];  // +0x14
    short m_mapDays[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];  // +0x1c
    unsigned char m_awards[EXPANSION_CAMPAIGN_AWARD_COUNT];  // +0x2c
    unsigned char m_bonusChoices[EXPANSION_CAMPAIGN_MAX_MAP_COUNT];  // +0x37
    char m_pad_0x3f[4];
    int m_viewMap;  // +0x43
    class heroWindow *m_window;  // +0x47
    int m_viewOnly;  // +0x4b
    char m_pad_0x4f;
    // --- constructors ---
    ExpCampaign(void);
    ~ExpCampaign();
    // --- methods ---
    void ResetMapChoices(void);
    void ResetMapsPlayed(void);
    void ResetAwards(void);
    void ResetBonusChoices(void);
    void GrantAward(int);
    void RemoveAward(int);
    signed char HasAward(int);
    void SetMapWasPlayed(void);
    void InitNewCampaign(int);
    void InitMap(void);
    void ShowInfo(int, int);
    void UpdateInfo(int);
    int HandleVictory(void);
    void HandleVictory1(void);
    void HandleVictory2(void);
    void HandleVictory3(void);
    void HandleVictory4(void);
    void ReplaySmacker(void);
    void ReplaySmacker1(void);
    void ReplaySmacker2(void);
    void ReplaySmacker3(void);
    void ReplaySmacker4(void);
    unsigned char IsCompleted(void);
    signed char IsThisMapCompleted(void);
private:
    static int MessageHandler(struct tag_message &);   // ?...@ExpCampaign@@CIH... (private static)
public:
    void Autosave(void);
    int Choose(void);
    short int Days(void);
    int CampaignID(void);
    char * JosephName(void);
    char * IvanName(void);
    signed char IsSpecialGoldenBow(int, int);
    signed char IsSpecialUA(void);
    signed char IsSpecialLossCondition(int);
};
#pragma pack(pop)
SIZE(ExpCampaign, 0x50);

extern struct SCampaignChoice
    xCampaignChoices[EXPANSION_CAMPAIGN_COUNT]
                    [EXPANSION_CAMPAIGN_MAX_MAP_COUNT]
                    [EXPANSION_CAMPAIGN_BONUS_CHOICE_COUNT];
#endif // HOMM2_SOURCE_EXPCAMPAIGN_H
