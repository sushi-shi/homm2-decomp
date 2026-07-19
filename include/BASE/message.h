#ifndef HOMM2_BASE_MESSAGE_H
#define HOMM2_BASE_MESSAGE_H

#include <va.h>

// Shared event ABI emitted by inputManager and consumed by BASE and SOURCE managers.
// The complete definition is required anywhere a message is passed or returned by value.

// Values emitted by INPUTMGR and consumed by manager/widget Main loops.
H2_ENUM_BEGIN(MessageType)
    MESSAGE_NONE                       = 0,
    MESSAGE_KEY_DOWN                   = 1,
    SPELL_MESSAGE_MOUSE_DOWN           = MESSAGE_KEY_DOWN,
    INIT_MENU_KEY_PRESS                = MESSAGE_KEY_DOWN,
    ADVMGR_REMOTE_WAIT_COMMAND_MESSAGE = MESSAGE_KEY_DOWN,
    MESSAGE_KEY_UP                     = 2,
    MESSAGE_MOUSE_MOVE                 = 4,
    HERO_UI_HOVER                      = MESSAGE_MOUSE_MOVE,
    ADVMGR_REMOTE_WAIT_MOUSE_MESSAGE   = MESSAGE_MOUSE_MOVE,
    SPELL_MESSAGE_HOVER                = MESSAGE_MOUSE_MOVE,
    VIEW_WORLD_MOUSE_MOVE              = MESSAGE_MOUSE_MOVE,
    INIT_MENU_MOUSE_MOVE               = MESSAGE_MOUSE_MOVE,
    MESSAGE_LEFT_BUTTON_DOWN           = 8,
    SPELL_MESSAGE_SELECT               = MESSAGE_LEFT_BUTTON_DOWN,
    MESSAGE_LEFT_BUTTON_UP             = 0x10,
    VIEW_WORLD_MOUSE_UP                = MESSAGE_LEFT_BUTTON_UP,
    MESSAGE_RIGHT_BUTTON_DOWN          = 0x20,
    SPELL_MESSAGE_CANCEL               = MESSAGE_RIGHT_BUTTON_DOWN,
    MESSAGE_RIGHT_BUTTON_UP            = 0x40,
    MESSAGE_WIDGET                     = 0x200,
    NORMAL_DIALOG_DISABLE_MESSAGE      = MESSAGE_WIDGET,
    NET_BOX_UPDATE_MESSAGE             = MESSAGE_WIDGET,
    INIT_MENU_MESSAGE                  = MESSAGE_WIDGET,
    SPELL_MESSAGE_DIALOG               = MESSAGE_WIDGET,
    VIEW_WORLD_MESSAGE                 = MESSAGE_WIDGET,
    HERO_UI_MESSAGE                    = MESSAGE_WIDGET,
    ADVMGR_TOWN_PORTAL_MESSAGE         = MESSAGE_WIDGET,
    ADVMGR_LOCATOR_MESSAGE_TYPE        = MESSAGE_WIDGET,
    ADVMGR_BUTTON_MESSAGE              = MESSAGE_WIDGET,
    ADVMGR_INTERFACE_MESSAGE           = MESSAGE_WIDGET,
    ADVMGR_SYSTEM_OPTIONS_MESSAGE      = MESSAGE_WIDGET,
    ADVMGR_ENEMY_TURN_MESSAGE_TYPE     = MESSAGE_WIDGET,
    COMBAT_SYSTEM_OPTION_EVENT         = MESSAGE_WIDGET,
    COMBAT_MESSAGE_BROADCAST_EVENT     = MESSAGE_WIDGET,
    MESSAGE_EXECUTIVE                  = 0x4000,
    COMBAT_WIN_MESSAGE                 = MESSAGE_EXECUTIVE,
    ADVMGR_REMOTE_WAIT_EXIT_MESSAGE    = MESSAGE_EXECUTIVE
H2_ENUM_END(MessageType)

// One widget-dispatch command space; per-window names are value-verified aliases.
H2_ENUM_BEGIN(BaseWidgetCommand)
    WIDGET_COMMAND_DRAW                       = 2,
    WIDGET_COMMAND_SET_TEXT                   = 3,
    HERO_UI_WIDGET_TEXT                       = WIDGET_COMMAND_SET_TEXT,
    NEW_GAME_WIDGET_SET_TEXT                  = WIDGET_COMMAND_SET_TEXT,
    COMBAT_WIN_LOSE_TEXT_COMMAND              = WIDGET_COMMAND_SET_TEXT,
    OVERVIEW_WIDGET_SET_TEXT                  = WIDGET_COMMAND_SET_TEXT,
    NET_BOX_TEXT_COMMAND                      = WIDGET_COMMAND_SET_TEXT,
    CASTLE_WIDGET_TEXT                        = WIDGET_COMMAND_SET_TEXT,
    VIEW_GENERAL_SET_TEXT                     = WIDGET_COMMAND_SET_TEXT,
    TOWN_WIDGET_SET_TEXT                      = WIDGET_COMMAND_SET_TEXT,
    NORMAL_DIALOG_SET_TEXT_COMMAND            = WIDGET_COMMAND_SET_TEXT,
    COMBAT_SYSTEM_OPTION_TEXT_MESSAGE         = WIDGET_COMMAND_SET_TEXT,
    COMBAT_MESSAGE_TEXT_ACTION                = WIDGET_COMMAND_SET_TEXT,
    BANK_BOX_SET_TEXT_COMMAND                 = WIDGET_COMMAND_SET_TEXT,
    ARENA_BROADCAST_TEXT                      = WIDGET_COMMAND_SET_TEXT,
    ADVMGR_TOWN_PORTAL_COMMAND_TEXT           = WIDGET_COMMAND_SET_TEXT,
    ADVMGR_SYSTEM_OPTIONS_SET_TEXT            = WIDGET_COMMAND_SET_TEXT,
    TRADING_POST_SET_TEXT                     = WIDGET_COMMAND_SET_TEXT,
    WIDGET_COMMAND_SET_FRAME                  = 4,
    HERO_UI_WIDGET_FRAME                      = WIDGET_COMMAND_SET_FRAME,
    CASTLE_WIDGET_FRAME                       = WIDGET_COMMAND_SET_FRAME,
    ADVMGR_LOCATOR_COMMAND_SET_FRAME          = WIDGET_COMMAND_SET_FRAME,
    NEW_GAME_WIDGET_SET_FRAME                 = WIDGET_COMMAND_SET_FRAME,
    INIT_MENU_SET_WIDGET_COMMAND              = WIDGET_COMMAND_SET_FRAME,
    HIGH_SCORE_WIDGET_SET_FRAME               = WIDGET_COMMAND_SET_FRAME,
    OVERVIEW_WIDGET_SET_FRAME                 = WIDGET_COMMAND_SET_FRAME,
    ADVMGR_ENEMY_TURN_MESSAGE_SET_FRAME       = WIDGET_COMMAND_SET_FRAME,
    VIEW_GENERAL_SET_FRAME                    = WIDGET_COMMAND_SET_FRAME,
    NET_BOX_COLOR_COMMAND                     = WIDGET_COMMAND_SET_FRAME,
    COMBAT_WIN_LOSE_ANIMATION_COMMAND         = WIDGET_COMMAND_SET_FRAME,
    COMBAT_SYSTEM_OPTION_BUTTON_MESSAGE       = WIDGET_COMMAND_SET_FRAME,
    COMBAT_SURRENDER_CAPTAIN_OVERLAY_COMMAND  = WIDGET_COMMAND_SET_FRAME,
    ADVMGR_SYSTEM_OPTIONS_SET_FRAME           = WIDGET_COMMAND_SET_FRAME,
    TRADING_POST_SET_ICON                     = WIDGET_COMMAND_SET_FRAME,
    WIDGET_COMMAND_SET_FLAGS                  = 5,
    NEW_GAME_WIDGET_ENABLE                    = WIDGET_COMMAND_SET_FLAGS,
    HERO_UI_WIDGET_ENABLE                     = WIDGET_COMMAND_SET_FLAGS,
    CASTLE_WIDGET_ENABLE                      = WIDGET_COMMAND_SET_FLAGS,
    ADVMGR_LOCATOR_COMMAND_SET_FLAGS          = WIDGET_COMMAND_SET_FLAGS,
    HIGH_SCORE_WIDGET_HIDE                    = WIDGET_COMMAND_SET_FLAGS,
    CAMPAIGN_MESSAGE_SELECT                   = WIDGET_COMMAND_SET_FLAGS,
    TOWN_WIDGET_DISABLE                       = WIDGET_COMMAND_SET_FLAGS,
    COMBAT_SURRENDER_CAPTAIN_PORTRAIT_COMMAND = WIDGET_COMMAND_SET_FLAGS,
    ADVMGR_BUTTON_ENABLE                      = WIDGET_COMMAND_SET_FLAGS,
    WIDGET_COMMAND_CLEAR_FLAGS                = 6,
    NEW_GAME_WIDGET_DISABLE                   = WIDGET_COMMAND_CLEAR_FLAGS,
    HERO_UI_WIDGET_DISABLE                    = WIDGET_COMMAND_CLEAR_FLAGS,
    CAMPAIGN_MESSAGE_DESELECT                 = WIDGET_COMMAND_CLEAR_FLAGS,
    ADVMGR_LOCATOR_COMMAND_CLEAR_FLAGS        = WIDGET_COMMAND_CLEAR_FLAGS,
    CASTLE_WIDGET_DISABLE                     = WIDGET_COMMAND_CLEAR_FLAGS,
    HIGH_SCORE_WIDGET_SHOW                    = WIDGET_COMMAND_CLEAR_FLAGS,
    TOWN_WIDGET_ENABLE                        = WIDGET_COMMAND_CLEAR_FLAGS,
    NORMAL_DIALOG_DISABLE_COMMAND             = WIDGET_COMMAND_CLEAR_FLAGS,
    COMBAT_SURRENDER_HERO_PORTRAIT_COMMAND    = WIDGET_COMMAND_CLEAR_FLAGS,
    ARENA_BROADCAST_CONTROL                   = WIDGET_COMMAND_CLEAR_FLAGS,
    ADVMGR_BUTTON_DISABLE                     = WIDGET_COMMAND_CLEAR_FLAGS,
    WIDGET_COMMAND_GET_TEXT                   = 7,
    WIDGET_COMMAND_SET_FILL_COLOR             = 8,
    HIGH_SCORE_WIDGET_SELECT                  = WIDGET_COMMAND_SET_FILL_COLOR,
    ADVMGR_LOCATOR_COMMAND_SET_COLOR          = WIDGET_COMMAND_SET_FILL_COLOR,
    WIDGET_COMMAND_SET_ICON                   = 9,
    WIDGET_COMMAND_DIALOG_SELECT              = 10,
    SPELL_COMMAND_CLOSE                       = WIDGET_COMMAND_DIALOG_SELECT,
    WIDGET_COMMAND_SELECT                     = 12,
    ADVMGR_SYSTEM_OPTIONS_ACTIVATE            = WIDGET_COMMAND_SELECT,
    CAMPAIGN_MESSAGE_HOVER                    = WIDGET_COMMAND_SELECT,
    COMBAT_SYSTEM_OPTION_BUTTON_EVENT         = WIDGET_COMMAND_SELECT,
    HERO_UI_INPUT_SELECT                      = WIDGET_COMMAND_SELECT,
    INIT_MENU_HOVER_COMMAND                   = WIDGET_COMMAND_SELECT,
    NEW_GAME_EVENT_PRESS                      = WIDGET_COMMAND_SELECT,
    VIEW_WORLD_SELECT                         = WIDGET_COMMAND_SELECT,
    WIDGET_COMMAND_DESELECT                   = 13,
    CAMPAIGN_MESSAGE_ACTIVATE                 = WIDGET_COMMAND_DESELECT,
    COMBAT_SYSTEM_OPTION_CLOSE_EVENT          = WIDGET_COMMAND_DESELECT,
    HERO_UI_INPUT_DESELECT                    = WIDGET_COMMAND_DESELECT,
    HIGH_SCORE_WIDGET_TOGGLE                  = WIDGET_COMMAND_DESELECT,
    INIT_MENU_CLICK_COMMAND                   = WIDGET_COMMAND_DESELECT,
    NEW_GAME_EVENT_RELEASE                    = WIDGET_COMMAND_DESELECT,
    VIEW_WORLD_RELEASE                        = WIDGET_COMMAND_DESELECT,
    WIDGET_COMMAND_ALTERNATE_SELECT           = 14,
    ADVMGR_SYSTEM_OPTIONS_HOVER               = WIDGET_COMMAND_ALTERNATE_SELECT,
    CAMPAIGN_MESSAGE_HELP                     = WIDGET_COMMAND_ALTERNATE_SELECT,
    COMBAT_SYSTEM_OPTION_HOVER_EVENT          = WIDGET_COMMAND_ALTERNATE_SELECT,
    HERO_UI_INPUT_ALTERNATE_SELECT            = WIDGET_COMMAND_ALTERNATE_SELECT,
    INIT_MENU_HELP_COMMAND                    = WIDGET_COMMAND_ALTERNATE_SELECT,
    NEW_GAME_EVENT_ALTERNATE_PRESS            = WIDGET_COMMAND_ALTERNATE_SELECT,
    ADVMGR_REMOTE_WAIT_EXIT_COMMAND           =
        16, // custom remote-wait exit; no canonical widget command at 16
    HERO_UI_WIDGET_ICON_FILE                  = WIDGET_COMMAND_SET_ICON,
    COMBAT_WIN_LOSE_RESOURCE_COMMAND          = WIDGET_COMMAND_SET_ICON,
    CASTLE_WIDGET_ICON_FILE                   = WIDGET_COMMAND_SET_ICON,
    CAMPAIGN_MESSAGE_SET_ICON                 = WIDGET_COMMAND_SET_ICON,
    VIEW_WORLD_ICON_MESSAGE                   = WIDGET_COMMAND_SET_ICON,
    VIEW_GENERAL_SET_ICON                     = WIDGET_COMMAND_SET_ICON,
    WIDGET_COMMAND_SET_MAX_LENGTH             = 0x33,
    WIDGET_COMMAND_SET_X                      = 0x34,
    HIGH_SCORE_WIDGET_RESIZE                  = WIDGET_COMMAND_SET_X,
    WIDGET_COMMAND_SET_Y                      = 0x35,
    WIDGET_COMMAND_SET_SELECTION              = 0x36,
    ADVMGR_TOWN_PORTAL_COMMAND_FINISH         = WIDGET_COMMAND_SET_SELECTION,
    WIDGET_COMMAND_GET_SELECTION              = 0x37,
    ADVMGR_TOWN_PORTAL_COMMAND_CHOICE         = WIDGET_COMMAND_GET_SELECTION,
    WIDGET_COMMAND_APPEND_ITEM                = 0x38,
    ADVMGR_TOWN_PORTAL_COMMAND_ADD_TOWN       = WIDGET_COMMAND_APPEND_ITEM,
    WIDGET_COMMAND_REPLACE_ITEM               = 0x39,
    WIDGET_COMMAND_DELETE_ITEM                = 0x3a,
    WIDGET_COMMAND_CLEAR_ITEMS                = 0x3b,
    WIDGET_COMMAND_REPLACE_ICON               = 0x3c,
    ADVMGR_REMOTE_WAIT_POP_NET_BOX_COMMAND    = WIDGET_COMMAND_REPLACE_ICON,
    ADVMGR_INTERFACE_REPLACE_RESOURCE         = WIDGET_COMMAND_REPLACE_ICON,
    WIDGET_COMMAND_SET_WIDTH                  = 0x3d
H2_ENUM_END(BaseWidgetCommand)

H2_ENUM_BEGIN(MessageModifier)
    MESSAGE_MODIFIER_RIGHT_SHIFT  = 1,
    MESSAGE_MODIFIER_LEFT_SHIFT   = 2,
    MESSAGE_MODIFIER_CONTROL      = 4,
    MESSAGE_MODIFIER_ALT          = 0x20,
    MESSAGE_MODIFIER_CONTROL_KEYS = 0xc, // either control-key modifier bit
    MESSAGE_MODIFIER_LEFT_BUTTON  = 0x100,
    MESSAGE_MODIFIER_RIGHT_BUTTON = 0x200,
    MESSAGE_MODIFIER_BUTTON_MASK  = 0x300
H2_ENUM_END(MessageModifier)

struct tag_messageKeyboardPayload {
    union {
        i32 keyCode;
        u8 keyByte;
    };
    i32 unknown0x08;
    i32 modifiers;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 unknown0x18;
};
SIZE(tag_messageKeyboardPayload, 0x18);

struct tag_messageMousePayload {
    i32 x;
    i32 y;
    i32 modifiers;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};
SIZE(tag_messageMousePayload, 0x18);

struct tag_messageHoverPayload {
    i32 x;
    i32 id;
    i32 modifiers;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};
SIZE(tag_messageHoverPayload, 0x18);

union tag_messageWidgetData {
    i32 value;
    char* text;
};
SIZE(tag_messageWidgetData, 0x4);

struct tag_messageWidgetPayload {
    BaseWidgetCommand command;
    i32 id;
    i32 parameter;
    i32 screenX;
    i32 screenY;
    tag_messageWidgetData data;
};
SIZE(tag_messageWidgetPayload, 0x18);

// Executive-arm commands consumed by the manager loop (EXEC), not by widgets.
H2_ENUM_BEGIN(ExecutiveCommand)
    EXECUTIVE_COMMAND_TERMINATE_LOOP = 1,
    EXECUTIVE_COMMAND_REMOVE_MANAGER = 2,
    EXECUTIVE_COMMAND_RETURN_RESULT  = 4,
    FILE_REQUESTER_EXECUTIVE_CLOSE   = EXECUTIVE_COMMAND_RETURN_RESULT,
    SWAP_COMMAND_EXIT                = EXECUTIVE_COMMAND_RETURN_RESULT
H2_ENUM_END(ExecutiveCommand)

struct tag_messageExecutivePayload {
    ExecutiveCommand command;
    i32 unknown0x08;
    i32 unknown0x0c;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 result;
};
SIZE(tag_messageExecutivePayload, 0x18);

struct tag_messageUnknownPayload {
    i32 unknown0x04;
    i32 unknown0x08;
    i32 unknown0x0c;
    i32 unknown0x10;
    i32 unknown0x14;
    i32 unknown0x18;
};
SIZE(tag_messageUnknownPayload, 0x18);

union tag_messagePayload {
    tag_messageKeyboardPayload keyboard;
    tag_messageMousePayload mouse;
    tag_messageHoverPayload hover;
    tag_messageWidgetPayload widget;
    tag_messageExecutivePayload executive;
    tag_messageUnknownPayload unknown;
};
SIZE(tag_messagePayload, 0x18);

struct tag_message {
    MessageType type;
    tag_messagePayload payload;
};
SIZE(tag_message, 0x1c);
#endif
