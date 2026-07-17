#ifndef HOMM2_SOURCE_HIGHSCOREMANAGER_H
#define HOMM2_SOURCE_HIGHSCOREMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
struct tag_message;
class heroWindow;

typedef enum HighScoreManagerConstant {
    HIGH_SCORE_DISPLAY_ENTRY_COUNT = 10,
    HIGH_SCORE_ANIMATION_FRAME_COUNT = 18,
    HIGH_SCORE_ANIMATION_DELAY = 120,
    HIGH_SCORE_TIMER_SLOT = 0,
    HIGH_SCORE_FILENAME_LENGTH = 352,
    HIGH_SCORE_FADE_STEPS = 8,
    HIGH_SCORE_MONSTER_FRAME_STRIDE = 9,
    HIGH_SCORE_MONSTER_ACTIVE_FRAME_OFFSET = 1,
    HIGH_SCORE_TEXT_WIDGET_STRIDE = 4,
    HIGH_SCORE_WIDGET_SET_FRAME = 4,
    HIGH_SCORE_WIDGET_HIDE = 5,
    HIGH_SCORE_WIDGET_SHOW = 6,
    HIGH_SCORE_WIDGET_SELECT = 8,
    HIGH_SCORE_WIDGET_RESIZE = 0x34,
    HIGH_SCORE_WIDGET_TOGGLE = 0xd,
    HIGH_SCORE_STANDARD_BUTTON = 100,
    HIGH_SCORE_CAMPAIGN_BUTTON = 0x93,
    HIGH_SCORE_CLOSE_BUTTON = 0x7800,
    HIGH_SCORE_FIRST_MONSTER_WIDGET = 0xc9,
    HIGH_SCORE_FIRST_SHADOW_WIDGET = 0xdd,
    HIGH_SCORE_FIRST_TEXT_WIDGET = 0x6a,
    HIGH_SCORE_TEXT_SCENARIO_OFFSET = 1,
    HIGH_SCORE_TEXT_RATING_OFFSET = 2,
    HIGH_SCORE_TEXT_SCORE_OFFSET = 3,
    HIGH_SCORE_TITLE_WIDGET = 0x66,
    HIGH_SCORE_STANDARD_TITLE_FRAME = 6,
    HIGH_SCORE_CAMPAIGN_TITLE_FRAME = 7,
    HIGH_SCORE_WIDGET_DEFAULT_VALUE = 6,
    HIGH_SCORE_STANDARD_SCENARIO_RESIZE = 0xf1,
    HIGH_SCORE_CAMPAIGN_SCENARIO_RESIZE = 0x127,
    HIGH_SCORE_STANDARD_RATING_RESIZE = 0x190,
    HIGH_SCORE_CAMPAIGN_RATING_RESIZE = 0x1d3,
    HIGH_SCORE_PRIMARY_SELECTION_FRAME = 1,
    HIGH_SCORE_SECONDARY_SELECTION_FRAME = 2,
    HIGH_SCORE_DRAW_X = 0,
    HIGH_SCORE_DRAW_Y = 200,
    HIGH_SCORE_DRAW_HEIGHT = 230,
    HIGH_SCORE_UPDATE_X = 512,
    HIGH_SCORE_UPDATE_Y = 51,
    HIGH_SCORE_UPDATE_WIDTH = 76,
    HIGH_SCORE_UPDATE_HEIGHT = 406,
    HIGH_SCORE_INPUT_BLOCK_FLAG = 0x200,
    HIGH_SCORE_MANAGER_OPEN_OK = 0,
    HIGH_SCORE_MANAGER_CONTINUE = 1,
    HIGH_SCORE_MANAGER_CLOSE = 2
} HighScoreManagerConstant;

#pragma pack(push, 1)  // recovered layout is byte-packed
class highScoreManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    i16 m_animationFrames[HIGH_SCORE_DISPLAY_ENTRY_COUNT];  // +0x36
    i16 m_monsterTypes[HIGH_SCORE_DISPLAY_ENTRY_COUNT];  // +0x4a
    char m_showCampaignScores;  // +0x5e
    heroWindow *m_window;  // +0x5f
    // --- constructors ---
    highScoreManager(void);
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#pragma pack(pop)
SIZE(highScoreManager, 0x63);
#endif // HOMM2_SOURCE_HIGHSCOREMANAGER_H
