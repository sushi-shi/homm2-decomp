#ifndef HOMM2_BASE_MESSAGE_H
#define HOMM2_BASE_MESSAGE_H
#include <va.h>
// Shared event ABI emitted by inputManager and consumed by BASE and SOURCE managers.
// The complete definition is required anywhere a message is passed or returned by value.

// Values emitted by INPUTMGR and consumed by manager/widget Main loops.
H2_ENUM_BEGIN(MessageType)
    MESSAGE_NONE = 0,
    MESSAGE_KEY_DOWN = 1,
    SPELL_MESSAGE_MOUSE_DOWN = MESSAGE_KEY_DOWN, // reconstruction name predates value proof
    INIT_MENU_KEY_PRESS = MESSAGE_KEY_DOWN,
    ADVMGR_REMOTE_WAIT_COMMAND_MESSAGE = MESSAGE_KEY_DOWN,
    MESSAGE_KEY_UP = 2,
    MESSAGE_MOUSE_MOVE = 4,
    HERO_UI_HOVER = MESSAGE_MOUSE_MOVE,
    ADVMGR_REMOTE_WAIT_MOUSE_MESSAGE = MESSAGE_MOUSE_MOVE,
    SPELL_MESSAGE_HOVER = MESSAGE_MOUSE_MOVE,
    VIEW_WORLD_MOUSE_MOVE = MESSAGE_MOUSE_MOVE,
    INIT_MENU_MOUSE_MOVE = MESSAGE_MOUSE_MOVE,
    MESSAGE_LEFT_BUTTON_DOWN = 8,
    SPELL_MESSAGE_SELECT = MESSAGE_LEFT_BUTTON_DOWN,
    MESSAGE_LEFT_BUTTON_UP = 0x10,
    VIEW_WORLD_MOUSE_UP = MESSAGE_LEFT_BUTTON_UP,
    MESSAGE_RIGHT_BUTTON_DOWN = 0x20,
    SPELL_MESSAGE_CANCEL = MESSAGE_RIGHT_BUTTON_DOWN,
    MESSAGE_RIGHT_BUTTON_UP = 0x40,
    MESSAGE_WIDGET = 0x200,
    NORMAL_DIALOG_DISABLE_MESSAGE = MESSAGE_WIDGET,
    NET_BOX_UPDATE_MESSAGE = MESSAGE_WIDGET,
    INIT_MENU_MESSAGE = MESSAGE_WIDGET,
    SPELL_MESSAGE_DIALOG = MESSAGE_WIDGET,
    VIEW_WORLD_MESSAGE = MESSAGE_WIDGET,
    HERO_UI_MESSAGE = MESSAGE_WIDGET,
    ADVMGR_TOWN_PORTAL_MESSAGE = MESSAGE_WIDGET,
    ADVMGR_LOCATOR_MESSAGE_TYPE = MESSAGE_WIDGET,
    ADVMGR_BUTTON_MESSAGE = MESSAGE_WIDGET,
    ADVMGR_INTERFACE_MESSAGE = MESSAGE_WIDGET,
    ADVMGR_SYSTEM_OPTIONS_MESSAGE = MESSAGE_WIDGET,
    ADVMGR_ENEMY_TURN_MESSAGE_TYPE = MESSAGE_WIDGET,
    COMBAT_SYSTEM_OPTION_EVENT = MESSAGE_WIDGET,
    COMBAT_MESSAGE_BROADCAST_EVENT = MESSAGE_WIDGET,
    MESSAGE_EXECUTIVE = 0x4000,
    COMBAT_WIN_MESSAGE = MESSAGE_EXECUTIVE,
    ADVMGR_REMOTE_WAIT_EXIT_MESSAGE = MESSAGE_EXECUTIVE
H2_ENUM_END(MessageType)

H2_ENUM_BEGIN(WidgetInputCommand)
    WIDGET_COMMAND_DIALOG_SELECT = 10,
    WIDGET_COMMAND_SELECT = 12,
    WIDGET_COMMAND_DESELECT = 13,
    WIDGET_COMMAND_ALTERNATE_SELECT = 14
H2_ENUM_END(WidgetInputCommand)

H2_ENUM_BEGIN(MessageModifier)
    MESSAGE_MODIFIER_RIGHT_SHIFT = 1,
    MESSAGE_MODIFIER_LEFT_SHIFT = 2,
    MESSAGE_MODIFIER_CONTROL = 4,
    MESSAGE_MODIFIER_ALT = 0x20,
    MESSAGE_MODIFIER_CONTROL_KEYS = 0xc, // either control-key modifier bit
    MESSAGE_MODIFIER_LEFT_BUTTON = 0x100,
    MESSAGE_MODIFIER_RIGHT_BUTTON = 0x200,
    MESSAGE_MODIFIER_BUTTON_MASK = 0x300
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
    i32 command;
    i32 id;
    i32 parameter;
    i32 screenX;
    i32 screenY;
    tag_messageWidgetData data;
};
SIZE(tag_messageWidgetPayload, 0x18);

struct tag_messageExecutivePayload {
    i32 command;
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
#endif // HOMM2_BASE_MESSAGE_H
