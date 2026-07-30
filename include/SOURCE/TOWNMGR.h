#ifndef HOMM2_TOWNMGR_H
#define HOMM2_TOWNMGR_H

#include <Ints.h>
#include <BASE/message.h>
#include <SOURCE/GAME.h>
#include <SOURCE/KB_TYPES.h>

struct tag_message;

#pragma pack(push, 1)
struct SBuildingInfo {
    i8 animationFrameCount;
    i16 x;
    i16 y;
    i16 width;
    i16 height;
};
#pragma pack(pop)

enum class TownExtraBuildingMask : u32 {
    TOWN_EXTRA_DOCK_GRAPHIC_MASK  = 0x4000,
    TOWN_EXTRA_RACE_FIRST_MASK    = 0x10000,
    TOWN_EXTRA_RACE_SECOND_MASK   = 0x20000,
    TOWN_EXTRA_RACE_THIRD_MASK    = 0x40000,
    TOWN_EXTRA_RACE_LAST_MASK     = 0x80000000,
    TOWN_EXTRA_DYNAMIC_CLEAR_MASK = 0x7ff8bfff,
    TOWN_CLOSE_DYNAMIC_CLEAR_MASK = 0xfff8bfff
};
using enum TownExtraBuildingMask;
ENABLE_ENUM_FLAGS(TownExtraBuildingMask)

enum class TownThievesGuildCategory : i32 {
    THIEVES_CATEGORY_TOWNS          = 0,
    THIEVES_CATEGORY_CASTLES        = 1,
    THIEVES_CATEGORY_HEROES         = 2,
    THIEVES_CATEGORY_GOLD           = 3,
    THIEVES_CATEGORY_WOOD_AND_ORE   = 4,
    THIEVES_CATEGORY_RARE_RESOURCES = 5,
    THIEVES_CATEGORY_OBELISKS       = 6,
    THIEVES_CATEGORY_ARTIFACTS      = 7,
    THIEVES_CATEGORY_ARMY_STRENGTH  = 8,
    THIEVES_CATEGORY_INCOME         = 9
};
using enum TownThievesGuildCategory;
ENABLE_ENUM_STEPS(TownThievesGuildCategory)

typedef enum TownManagerConstant {
    TOWN_DEBUG_BUILD_ALL                     = 100,
    TOWN_BUILDING_COUNT                      = 32,
    TOWN_OBJECT_FILENAME_SIZE                = 16,
    TOWN_MANAGER_EVENT_MASK                  = 0x800,
    TOWN_SELECTED_BUILDING_NONE              = -1,
    TOWN_HOVER_NONE                          = -1,
    TOWN_ARMY_SLOT_NONE                      = -1,
    TOWN_WIDGET_ID_NONE                      = -1,
    TOWN_WIDGET_INSERT_DEFAULT               = -1,
    TOWN_MUSIC_STOP                          = -1,
    TOWN_POINTER_DEFAULT                     = -1,
    TOWN_WINDOW_TEXT_CONTROL                 = 0x89,
    TOWN_WIDGET_DISABLED_VALUE               = 0x1000,
    TOWN_WIDGET_ENABLED_VALUE                = 2,
    TOWN_WINDOW_DRAW_WIDTH                   = 0x320,
    TOWN_WINDOW_DRAW_RIGHT                   = 0x321,
    TOWN_SCREEN_WIDTH                        = 0x280,
    TOWN_SCREEN_HEIGHT                       = 0x1e0,
    TOWN_BANK_BOX_X                          = 0x222,
    TOWN_GARRISON_STRIP_Y                    = 0x100,
    TOWN_HERO_STRIP_Y                        = 0x163,
    TOWN_HERO_STRIP_FRAME_COUNT              = 3,
    TOWN_GARRISON_FIRST_CONTROL              = 0x74,
    TOWN_HERO_FIRST_CONTROL                  = 0x7a,
    TOWN_ICON_RESOURCE_TYPE                  = 1,
    TOWN_PORTRAIT_FRAME_BASE                 = 0x5a,
    TOWN_CREST_FRAME_WITH_HERO               = 1,
    TOWN_CREST_FRAME_WITHOUT_HERO            = 4,
    TOWN_EMPTY_HERO_PORTRAIT_OFFSET          = 0x10,
    TOWN_BARBARIAN_ANIMATION_BUILD_STATE     = 5,
    TOWN_FADE_STEPS                          = 8,
    TOWN_GARRISON_SLOT_FIRST                 = 0x75,
    TOWN_GARRISON_SLOT_LAST                  = 0x79,
    TOWN_HERO_SLOT_FIRST                     = 0x7b,
    TOWN_HERO_SLOT_LAST                      = 0x7f,
    TOWN_ARMY_QUALIFIER_MASK                 = 3,
    TOWN_EMPTY_STATUS_CONTROL_FIRST          = 0x80,
    TOWN_EMPTY_STATUS_CONTROL_LAST           = 0x81,
    TOWN_STATUS_DRAW_LEFT                    = 0,
    TOWN_STATUS_DRAW_WIDTH                   = 0x384,
    TOWN_STATUS_DRAW_RIGHT                   = 0x386,
    TOWN_STATUS_REGION_X                     = 0x19,
    TOWN_STATUS_REGION_Y                     = 0x1d0,
    TOWN_STATUS_REGION_WIDTH                 = 0x24e,
    TOWN_STATUS_REGION_HEIGHT                = 0xf,
    TOWN_ARMY_VIEW_X                         = 0x77,
    TOWN_ARMY_VIEW_Y                         = 0x14,
    TOWN_MAX_BOATS                           = 48,
    TOWN_MAX_ARTIFACTS                       = 14,
    TOWN_SPELL_BOOK_COST                     = 500,
    TOWN_BOAT_GOLD_COST                      = 1000,
    TOWN_BOAT_WOOD_COST                      = 10,
    TOWN_INTERFACE_BROADCAST_FLAGS           = 0x4008,
    TOWN_CONTROL_STATUS_TEXT                 = 0x386,
    TOWN_DIALOG_CONFIRM                      = 0x7802,
    TOWN_RESOURCE_COUNT                      = 7,
    TOWN_MAGE_GUILD_MAX_LEVEL                = 5,
    TOWN_MAGE_SPELLS_PER_LEVEL               = 4,
    TOWN_MAGE_SPELL_UNAVAILABLE              = 999,
    TOWN_MAGE_FIRST_SPELL_CONTROL            = 10,
    TOWN_MAGE_FIRST_ICON_CONTROL             = 0x28,
    TOWN_MAGE_FIRST_DESCRIPTION_CONTROL      = 0x46,
    TOWN_MAGE_GUILD_ICON_CONTROL             = 100,
    TOWN_MAGE_DESCRIPTION_CONTROL            = 0x6e,
    TOWN_WIZARD_LIBRARY_BUILDING_FLAG        = 0x2000,
    TOWN_TAVERN_FIRST_ANIMATION_FRAME        = 2,
    TOWN_TAVERN_ANIMATION_FRAME_COUNT        = 20,
    TOWN_TAVERN_ANIMATION_DELAY              = 0x4b,
    TOWN_TAVERN_RUMOUR_CONTROL               = 0x259,
    TOWN_SPLIT_AMOUNT_CONTROL                = 0x44,
    TOWN_SPLIT_INCREASE_CONTROL              = 0x45,
    TOWN_SPLIT_DECREASE_CONTROL              = 0x46,
    TOWN_FACTION_COUNT                       = 6,
    TOWN_ARMY_SLOT_COUNT                     = 5,
    TOWN_REDRAW_INTERVAL                     = 150,
    TOWN_WELL_DWELLING_COUNT                 = 6,
    TOWN_WELL_FIRST_NAME_CONTROL             = 7,
    TOWN_WELL_FIRST_MONSTER_ICON_CONTROL     = 13,
    TOWN_WELL_FIRST_CREATURE_CONTROL         = 19,
    TOWN_WELL_FIRST_DETAIL_CONTROL           = 25,
    TOWN_WELL_FIRST_AVAILABLE_CONTROL        = 31,
    TOWN_WELL_FIRST_AVAILABLE_COUNT_CONTROL  = 37,
    TOWN_WELL_FIRST_UPGRADE_BUILDING         = 24,
    TOWN_WELL_LAST_UPGRADE_BUILDING          = 30,
    TOWN_WELL_FIRST_UPGRADE_OFFSET           = 5,
    TOWN_WELL_FIRST_DWELLING_GROWTH_BUILDING = 11,
    TOWN_WELL_BASE_GROWTH_BONUS              = 2,
    TOWN_WELL_FIRST_DWELLING_GROWTH_BONUS    = 8,
    TOWN_THIEVES_PLAYER_COUNT                = H2EnumIndex(GAME_PLAYER_COUNT),
    TOWN_THIEVES_ORDER_BUFFER_SIZE           = 8,
    TOWN_THIEVES_FIRST_RANK_CONTROL          = 800,
    TOWN_THIEVES_FIRST_PLAYER_CONTROL        = 900,
    TOWN_THIEVES_BASIC_CATEGORY_COUNT        = 3,
    TOWN_THIEVES_PRIMARY_CATEGORY_COUNT      = 5,
    TOWN_THIEVES_PERSONALITY_CATEGORY_COUNT  = 7,
    TOWN_THIEVES_CREATURE_CATEGORY_COUNT     = 9,
    TOWN_THIEVES_MAX_CATEGORIES              = 10,
    TOWN_THIEVES_INFO_STRONGEST_HERO         = 1,
    TOWN_THIEVES_INFO_PRIMARY_STATS          = 2,
    TOWN_THIEVES_INFO_PERSONALITY            = 3,
    TOWN_THIEVES_INFO_STRONGEST_CREATURE     = 4,
    TOWN_THIEVES_INFO_ALL_CATEGORIES         = 5,
    TOWN_THIEVES_PRIMARY_STAT_COUNT          = 4,
    TOWN_THIEVES_DEAD_PLAYER_STAT            = -1,
    TOWN_THIEVES_RANK_ICON_FRAME_BASE        = 22
} TownManagerConstant;

extern SBuildingInfo sBuildingInfo[][TOWN_BUILDING_COUNT];

MessageDispatchResult MageGuildHandler(struct tag_message&);
MessageDispatchResult TavernHandler(struct tag_message&);
MessageDispatchResult SplitArmyHandler(struct tag_message&);
void GetCategoryStats(
    TownThievesGuildCategory,
    i32l* const,
    i8* const
);
void SortStats(i32l* const, i8* const);

#endif
