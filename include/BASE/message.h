#ifndef HOMM2_BASE_MESSAGE_H
#define HOMM2_BASE_MESSAGE_H

#include <Ints.h>

struct tag_message;

enum class MessageDispatchResult : i32 {
    MESSAGE_DISPATCH_CONTINUE = 0,
    MESSAGE_DISPATCH_CONSUME  = 1,
    MESSAGE_DISPATCH_FORWARD  = 2
};
using enum MessageDispatchResult;

typedef MessageDispatchResult (*MessageDispatchHandler)(tag_message&);

class MessageType {
public:
    MessageType() = default;
    explicit constexpr MessageType(i32 value) : m_value(value) {}
    constexpr operator i32() const { return m_value; }

private:
    i32 m_value;
};
static_assert(sizeof(MessageType) == sizeof(i32));

inline constexpr MessageType MESSAGE_NONE{0};
inline constexpr MessageType MESSAGE_KEY_DOWN{1};
inline constexpr MessageType MESSAGE_KEY_UP{2};
inline constexpr MessageType MESSAGE_MOUSE_MOVE{4};
inline constexpr MessageType MESSAGE_LEFT_BUTTON_DOWN{8};
inline constexpr MessageType MESSAGE_LEFT_BUTTON_UP{0x10};
inline constexpr MessageType MESSAGE_RIGHT_BUTTON_DOWN{0x20};
inline constexpr MessageType MESSAGE_RIGHT_BUTTON_UP{0x40};
inline constexpr MessageType MESSAGE_TEXT_INPUT{0x80};
inline constexpr MessageType MESSAGE_WIDGET{0x200};
inline constexpr MessageType MESSAGE_EXECUTIVE{0x4000};
ENABLE_ENUM_FLAGS(MessageType)

inline constexpr MessageType SPELL_MESSAGE_MOUSE_DOWN = MESSAGE_KEY_DOWN;
inline constexpr MessageType INIT_MENU_KEY_PRESS = MESSAGE_KEY_DOWN;
inline constexpr MessageType ADVMGR_REMOTE_WAIT_COMMAND_MESSAGE = MESSAGE_KEY_DOWN;
inline constexpr MessageType HERO_UI_HOVER = MESSAGE_MOUSE_MOVE;
inline constexpr MessageType ADVMGR_REMOTE_WAIT_MOUSE_MESSAGE = MESSAGE_MOUSE_MOVE;
inline constexpr MessageType SPELL_MESSAGE_HOVER = MESSAGE_MOUSE_MOVE;
inline constexpr MessageType VIEW_WORLD_MOUSE_MOVE = MESSAGE_MOUSE_MOVE;
inline constexpr MessageType INIT_MENU_MOUSE_MOVE = MESSAGE_MOUSE_MOVE;
inline constexpr MessageType SPELL_MESSAGE_SELECT = MESSAGE_LEFT_BUTTON_DOWN;
inline constexpr MessageType VIEW_WORLD_MOUSE_UP = MESSAGE_LEFT_BUTTON_UP;
inline constexpr MessageType SPELL_MESSAGE_CANCEL = MESSAGE_RIGHT_BUTTON_DOWN;
inline constexpr MessageType NORMAL_DIALOG_DISABLE_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType NET_BOX_UPDATE_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType INIT_MENU_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType SPELL_MESSAGE_DIALOG = MESSAGE_WIDGET;
inline constexpr MessageType VIEW_WORLD_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType HERO_UI_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_TOWN_PORTAL_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_LOCATOR_MESSAGE_TYPE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_BUTTON_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_INTERFACE_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_SYSTEM_OPTIONS_MESSAGE = MESSAGE_WIDGET;
inline constexpr MessageType ADVMGR_ENEMY_TURN_MESSAGE_TYPE = MESSAGE_WIDGET;
inline constexpr MessageType COMBAT_SYSTEM_OPTION_EVENT = MESSAGE_WIDGET;
inline constexpr MessageType COMBAT_MESSAGE_BROADCAST_EVENT = MESSAGE_WIDGET;
inline constexpr MessageType COMBAT_WIN_MESSAGE = MESSAGE_EXECUTIVE;
inline constexpr MessageType ADVMGR_REMOTE_WAIT_EXIT_MESSAGE = MESSAGE_EXECUTIVE;

class BaseWidgetCommand {
public:
    BaseWidgetCommand() = default;
    explicit constexpr BaseWidgetCommand(i32 value) : m_value(value) {}
    constexpr operator i32() const { return m_value; }

private:
    i32 m_value;
};
static_assert(sizeof(BaseWidgetCommand) == sizeof(i32));

inline constexpr BaseWidgetCommand WIDGET_COMMAND_DRAW{2};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_TEXT{3};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_FRAME{4};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_FLAGS{5};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_CLEAR_FLAGS{6};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_GET_TEXT{7};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_FILL_COLOR{8};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_ICON{9};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_DIALOG_SELECT{10};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SELECT{12};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_DESELECT{13};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_ALTERNATE_SELECT{14};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_REMOTE_WAIT_EXIT{16};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_MAX_LENGTH{0x33};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_X{0x34};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_Y{0x35};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_SELECTION{0x36};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_GET_SELECTION{0x37};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_APPEND_ITEM{0x38};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_REPLACE_ITEM{0x39};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_DELETE_ITEM{0x3a};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_CLEAR_ITEMS{0x3b};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_REPLACE_ICON{0x3c};
inline constexpr BaseWidgetCommand WIDGET_COMMAND_SET_WIDTH{0x3d};

inline constexpr BaseWidgetCommand HERO_UI_WIDGET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand NEW_GAME_WIDGET_SET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand COMBAT_WIN_LOSE_TEXT_COMMAND = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand OVERVIEW_WIDGET_SET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand NET_BOX_TEXT_COMMAND = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand CASTLE_WIDGET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand VIEW_GENERAL_SET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand TOWN_WIDGET_SET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand NORMAL_DIALOG_SET_TEXT_COMMAND = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand COMBAT_SYSTEM_OPTION_TEXT_MESSAGE = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand COMBAT_MESSAGE_TEXT_ACTION = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand BANK_BOX_SET_TEXT_COMMAND = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand ARENA_BROADCAST_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand ADVMGR_TOWN_PORTAL_COMMAND_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand ADVMGR_SYSTEM_OPTIONS_SET_TEXT = WIDGET_COMMAND_SET_TEXT;
inline constexpr BaseWidgetCommand TRADING_POST_SET_TEXT = WIDGET_COMMAND_SET_TEXT;

inline constexpr BaseWidgetCommand HERO_UI_WIDGET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand CASTLE_WIDGET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand ADVMGR_LOCATOR_COMMAND_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand NEW_GAME_WIDGET_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand INIT_MENU_SET_WIDGET_COMMAND = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand OVERVIEW_WIDGET_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand VIEW_GENERAL_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand NET_BOX_COLOR_COMMAND = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand COMBAT_WIN_LOSE_ANIMATION_COMMAND = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand COMBAT_SYSTEM_OPTION_BUTTON_MESSAGE = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand ADVMGR_SYSTEM_OPTIONS_SET_FRAME = WIDGET_COMMAND_SET_FRAME;
inline constexpr BaseWidgetCommand TRADING_POST_SET_ICON = WIDGET_COMMAND_SET_FRAME;

inline constexpr BaseWidgetCommand NEW_GAME_WIDGET_ENABLE = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand HERO_UI_WIDGET_ENABLE = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand CASTLE_WIDGET_ENABLE = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand ADVMGR_LOCATOR_COMMAND_SET_FLAGS = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_HIDE = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_SELECT = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand TOWN_WIDGET_DISABLE = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND = WIDGET_COMMAND_SET_FLAGS;
inline constexpr BaseWidgetCommand ADVMGR_BUTTON_ENABLE = WIDGET_COMMAND_SET_FLAGS;

inline constexpr BaseWidgetCommand NEW_GAME_WIDGET_DISABLE = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand HERO_UI_WIDGET_DISABLE = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_DESELECT = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand CASTLE_WIDGET_DISABLE = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_SHOW = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand TOWN_WIDGET_ENABLE = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand NORMAL_DIALOG_DISABLE_COMMAND = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand ARENA_BROADCAST_CONTROL = WIDGET_COMMAND_CLEAR_FLAGS;
inline constexpr BaseWidgetCommand ADVMGR_BUTTON_DISABLE = WIDGET_COMMAND_CLEAR_FLAGS;

inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_SELECT = WIDGET_COMMAND_SET_FILL_COLOR;
inline constexpr BaseWidgetCommand ADVMGR_LOCATOR_COMMAND_SET_COLOR = WIDGET_COMMAND_SET_FILL_COLOR;
inline constexpr BaseWidgetCommand SPELL_COMMAND_CLOSE = WIDGET_COMMAND_DIALOG_SELECT;

inline constexpr BaseWidgetCommand ADVMGR_SYSTEM_OPTIONS_ACTIVATE = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_HOVER = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand COMBAT_SYSTEM_OPTION_BUTTON_EVENT = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand HERO_UI_INPUT_SELECT = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand INIT_MENU_HOVER_COMMAND = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand NEW_GAME_EVENT_PRESS = WIDGET_COMMAND_SELECT;
inline constexpr BaseWidgetCommand VIEW_WORLD_SELECT = WIDGET_COMMAND_SELECT;

inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_ACTIVATE = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand COMBAT_SYSTEM_OPTION_CLOSE_EVENT = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand HERO_UI_INPUT_DESELECT = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_TOGGLE = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand INIT_MENU_CLICK_COMMAND = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand NEW_GAME_EVENT_RELEASE = WIDGET_COMMAND_DESELECT;
inline constexpr BaseWidgetCommand VIEW_WORLD_RELEASE = WIDGET_COMMAND_DESELECT;

inline constexpr BaseWidgetCommand ADVMGR_SYSTEM_OPTIONS_HOVER = WIDGET_COMMAND_ALTERNATE_SELECT;
inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_HELP = WIDGET_COMMAND_ALTERNATE_SELECT;
inline constexpr BaseWidgetCommand COMBAT_SYSTEM_OPTION_HOVER_EVENT = WIDGET_COMMAND_ALTERNATE_SELECT;
inline constexpr BaseWidgetCommand HERO_UI_INPUT_ALTERNATE_SELECT = WIDGET_COMMAND_ALTERNATE_SELECT;
inline constexpr BaseWidgetCommand INIT_MENU_HELP_COMMAND = WIDGET_COMMAND_ALTERNATE_SELECT;
inline constexpr BaseWidgetCommand NEW_GAME_EVENT_ALTERNATE_PRESS = WIDGET_COMMAND_ALTERNATE_SELECT;

inline constexpr BaseWidgetCommand ADVMGR_REMOTE_WAIT_EXIT_COMMAND = WIDGET_COMMAND_REMOTE_WAIT_EXIT;
inline constexpr BaseWidgetCommand HERO_UI_WIDGET_ICON_FILE = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand COMBAT_WIN_LOSE_RESOURCE_COMMAND = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand CASTLE_WIDGET_ICON_FILE = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand CAMPAIGN_MESSAGE_SET_ICON = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand VIEW_WORLD_ICON_MESSAGE = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand VIEW_GENERAL_SET_ICON = WIDGET_COMMAND_SET_ICON;
inline constexpr BaseWidgetCommand HIGH_SCORE_WIDGET_RESIZE = WIDGET_COMMAND_SET_X;
inline constexpr BaseWidgetCommand ADVMGR_TOWN_PORTAL_COMMAND_FINISH = WIDGET_COMMAND_SET_SELECTION;
inline constexpr BaseWidgetCommand ADVMGR_TOWN_PORTAL_COMMAND_CHOICE = WIDGET_COMMAND_GET_SELECTION;
inline constexpr BaseWidgetCommand ADVMGR_TOWN_PORTAL_COMMAND_ADD_TOWN = WIDGET_COMMAND_APPEND_ITEM;
inline constexpr BaseWidgetCommand ADVMGR_REMOTE_WAIT_POP_NET_BOX_COMMAND = WIDGET_COMMAND_REPLACE_ICON;
inline constexpr BaseWidgetCommand ADVMGR_INTERFACE_REPLACE_RESOURCE = WIDGET_COMMAND_REPLACE_ICON;

enum class MessageModifier : i32 {
    MESSAGE_MODIFIER_NONE                   = 0,
    MESSAGE_MODIFIER_RIGHT_SHIFT            = 1,
    MESSAGE_MODIFIER_LEFT_SHIFT             = 2,
    MESSAGE_MODIFIER_SHIFT_KEYS             = 0x3,
    MESSAGE_MODIFIER_CONTROL                = 4,
    MESSAGE_MODIFIER_ALT                    = 0x20,
    MESSAGE_MODIFIER_CONTROL_KEYS           = 0xc,
    MESSAGE_MODIFIER_CLEAR_CONTROL_MASK     = 0xfffb,
    MESSAGE_MODIFIER_CLEAR_LEFT_SHIFT_MASK  = 0xfffd,
    MESSAGE_MODIFIER_CLEAR_RIGHT_SHIFT_MASK = 0xfffe,
    MESSAGE_MODIFIER_CLEAR_ALT_MASK         = 0xffdf,
    MESSAGE_MODIFIER_LEFT_BUTTON            = 0x100,
    MESSAGE_MODIFIER_RIGHT_BUTTON           = 0x200,
    MESSAGE_MODIFIER_BUTTON_MASK            = 0x300
};
using enum MessageModifier;
ENABLE_ENUM_FLAGS(MessageModifier)

struct tag_messageKeyboardPayload {
    union {
        i32 keyCode;
        u8 keyByte;
    };
    i32 unknown0x08;
    MessageModifier modifiers;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 unknown0x18;
};

struct tag_messageMousePayload {
    i32 x;
    i32 y;
    MessageModifier modifiers;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};

struct tag_messageHoverPayload {
    i32 x;
    i32 id;
    i32 subId;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};

union tag_messageWidgetData {
    i32 value;
    const char* text;
};

struct tag_messageWidgetPayload {
    BaseWidgetCommand command;
    i32 id;
    union {
        i32 parameter;
        MessageModifier modifiers;
    };
    i32 screenX;
    i32 screenY;
    tag_messageWidgetData data;
};

enum class ExecutiveCommand : i32 {
    EXECUTIVE_COMMAND_TERMINATE_LOOP = 1,
    EXECUTIVE_COMMAND_REMOVE_MANAGER = 2,
    EXECUTIVE_COMMAND_RETURN_RESULT  = 4,
    FILE_REQUESTER_EXECUTIVE_CLOSE   = EXECUTIVE_COMMAND_RETURN_RESULT,
    SWAP_COMMAND_EXIT                = EXECUTIVE_COMMAND_RETURN_RESULT
};
using enum ExecutiveCommand;

struct tag_messageExecutivePayload {
    ExecutiveCommand command;
    i32 unknown0x08;
    i32 unknown0x0c;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 result;
};

struct tag_messageUnknownPayload {
    i32 unknown0x04;
    i32 unknown0x08;
    i32 unknown0x0c;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 unknown0x18;
};

union tag_messagePayload {
    tag_messageKeyboardPayload keyboard;
    tag_messageMousePayload mouse;
    tag_messageHoverPayload hover;
    tag_messageWidgetPayload widget;
    tag_messageExecutivePayload executive;
    tag_messageUnknownPayload unknown;
};

struct tag_message {
    MessageType type;
    tag_messagePayload payload;
};
#endif
