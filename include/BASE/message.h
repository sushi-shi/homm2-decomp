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

enum class MessageType : i32 {
    MESSAGE_NONE                       = 0,
    MESSAGE_KEY_DOWN                   = 1,
    MESSAGE_KEY_UP                     = 2,
    MESSAGE_MOUSE_MOVE                 = 4,
    MESSAGE_LEFT_BUTTON_DOWN           = 8,
    MESSAGE_LEFT_BUTTON_UP             = 0x10,
    MESSAGE_RIGHT_BUTTON_DOWN          = 0x20,
    MESSAGE_RIGHT_BUTTON_UP            = 0x40,
    MESSAGE_WIDGET                     = 0x200,
    MESSAGE_EXECUTIVE                  = 0x4000,
};
using enum MessageType;
ENABLE_ENUM_FLAGS(MessageType)

enum class BaseWidgetCommand : i32 {
    WIDGET_COMMAND_DRAW             = 2,
    WIDGET_COMMAND_SET_TEXT         = 3,
    WIDGET_COMMAND_SET_FRAME        = 4,
    WIDGET_COMMAND_SET_FLAGS        = 5,
    WIDGET_COMMAND_CLEAR_FLAGS      = 6,
    WIDGET_COMMAND_GET_TEXT         = 7,

    WIDGET_COMMAND_SET_FILL_COLOR   = 8,
    WIDGET_COMMAND_SET_ICON         = 9,
    WIDGET_COMMAND_DIALOG_SELECT    = 10,


    WIDGET_NOTIFY_SELECT            = 12,

    WIDGET_NOTIFY_DESELECT          = 13,

    WIDGET_NOTIFY_RIGHT_CLICK       = 14,
    WIDGET_COMMAND_SET_MAX_LENGTH   = 0x33,
    WIDGET_COMMAND_SET_X            = 0x34,
    WIDGET_COMMAND_SET_Y            = 0x35,
    WIDGET_COMMAND_SET_SELECTION    = 0x36,
    WIDGET_COMMAND_GET_SELECTION    = 0x37,
    WIDGET_COMMAND_APPEND_ITEM      = 0x38,
    WIDGET_COMMAND_REPLACE_ITEM     = 0x39,
    WIDGET_COMMAND_DELETE_ITEM      = 0x3a,
    WIDGET_COMMAND_CLEAR_ITEMS      = 0x3b,
    WIDGET_COMMAND_REPLACE_ICON     = 0x3c,
    WIDGET_COMMAND_SET_WIDTH        = 0x3d
};
using enum BaseWidgetCommand;


#define IS_WIDGET_SELECTION_NOTIFICATION(command) \
    ((command) == WIDGET_NOTIFY_SELECT || (command) == WIDGET_NOTIFY_RIGHT_CLICK)

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


#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                    \
    ((messageValue).type = MESSAGE_WIDGET,                                                         \
     (messageValue).payload.widget.command = (commandValue),                                       \
     (messageValue).payload.widget.id = (idValue))

#endif
