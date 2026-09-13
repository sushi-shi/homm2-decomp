#ifndef HOMM2_BASE_MESSAGE_H
#define HOMM2_BASE_MESSAGE_H

#include <va.h>

struct tag_message;

H2_ENUM_CLASS_BEGIN(MessageDispatchResult)
    MESSAGE_DISPATCH_CONTINUE = 0,
    MESSAGE_DISPATCH_CONSUME  = 1,
    MESSAGE_DISPATCH_FORWARD  = 2
H2_ENUM_CLASS_END(MessageDispatchResult)

typedef MessageDispatchResult (*MessageDispatchHandler)(tag_message&);

H2_ENUM_CLASS_BEGIN(MessageType)
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
H2_ENUM_CLASS_END(MessageType)
H2_ENUM_FLAGS(MessageType)

H2_ENUM_CLASS_BEGIN(BaseWidgetCommand)
    WIDGET_COMMAND_DRAW             = 2,
    WIDGET_COMMAND_SET_TEXT         = 3,
    WIDGET_COMMAND_SET_FRAME        = 4,
    WIDGET_COMMAND_SET_FLAGS        = 5,
    WIDGET_COMMAND_CLEAR_FLAGS      = 6,
    WIDGET_COMMAND_GET_TEXT         = 7,
    // Icon widgets use a palette index; text widgets use FontDrawMode.
    WIDGET_COMMAND_SET_FILL_COLOR   = 8,
    WIDGET_COMMAND_SET_ICON         = 9,
    WIDGET_COMMAND_DIALOG_SELECT    = 10,
    // Notifications forwarded by widgets, not commands sent to change their state.
    // SELECT covers button selection, list selection and completed text editing.
    WIDGET_NOTIFY_SELECT            = 12,
    // DESELECT includes pointer departure and repeat timing, not just button-up.
    WIDGET_NOTIFY_DESELECT          = 13,
    // Emitted on right-button-down; the receiving screen decides whether to show help.
    WIDGET_NOTIFY_RIGHT_CLICK       = 14,
    WIDGET_COMMAND_REMOTE_WAIT_EXIT = 16,
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
H2_ENUM_CLASS_END(BaseWidgetCommand)

// Notification only: callers retain type, button and routing policy checks.
#define IS_WIDGET_SELECTION_NOTIFICATION(command) \
    ((command) == WIDGET_NOTIFY_SELECT || (command) == WIDGET_NOTIFY_RIGHT_CLICK)

H2_ENUM_CLASS_BEGIN(MessageModifier)
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
H2_ENUM_CLASS_END(MessageModifier)
H2_ENUM_FLAGS(MessageModifier)

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
SIZE(tag_messageKeyboardPayload, 0x18);

struct tag_messageMousePayload {
    i32 x;
    i32 y;
    MessageModifier modifiers;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};
SIZE(tag_messageMousePayload, 0x18);

struct tag_messageHoverPayload {
    i32 x;
    i32 id;
    i32 subId;
    i32 screenX;
    i32 screenY;
    i32 unknown0x18;
};
SIZE(tag_messageHoverPayload, 0x18);

union tag_messageWidgetData {
    i32 value;
    H2_CONST char* text;
};
SIZE(tag_messageWidgetData, 0x4);

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
SIZE(tag_messageWidgetPayload, 0x18);

H2_ENUM_CLASS_BEGIN(ExecutiveCommand)
    EXECUTIVE_COMMAND_TERMINATE_LOOP = 1,
    EXECUTIVE_COMMAND_REMOVE_MANAGER = 2,
    EXECUTIVE_COMMAND_RETURN_RESULT  = 4,
H2_ENUM_CLASS_END(ExecutiveCommand)

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

// Retype the event without clearing its payload or surviving mouse modifiers.
// Arguments must be stable expressions: messageValue is evaluated three times.
#define SET_WIDGET_MESSAGE(messageValue, commandValue, idValue)                                    \
    ((messageValue).type = MESSAGE_WIDGET,                                                         \
     (messageValue).payload.widget.command = (commandValue),                                       \
     (messageValue).payload.widget.id = (idValue))

#endif
