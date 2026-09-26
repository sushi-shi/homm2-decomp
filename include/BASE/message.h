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
inline constexpr BaseWidgetCommand WIDGET_NOTIFY_SELECT{12};
inline constexpr BaseWidgetCommand WIDGET_NOTIFY_DESELECT{13};
inline constexpr BaseWidgetCommand WIDGET_NOTIFY_RIGHT_CLICK{14};
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

constexpr BaseWidgetCommand WidgetFlagsCommand(bool setFlags) {
    return setFlags ? WIDGET_COMMAND_SET_FLAGS : WIDGET_COMMAND_CLEAR_FLAGS;
}

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
