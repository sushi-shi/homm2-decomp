#ifndef HOMM2_CARCASS_TYPES_H
#define HOMM2_CARCASS_TYPES_H
#include <va.h>
// Minimal COMPLETE definitions for structs that some functions pass/return BY VALUE but
// that the recovered headers only forward-declare. Needed so carcass-phase stub bodies
// compile (a by-value param/return requires a complete type at the definition). Included
// at the tail of _all.h; recovered layouts are pinned as byte-matching proves them.

// Values emitted by INPUTMGR and consumed by manager/widget Main loops.
typedef enum MessageType {
    MESSAGE_NONE = 0,
    MESSAGE_KEY_DOWN = 1,
    MESSAGE_KEY_UP = 2,
    MESSAGE_MOUSE_MOVE = 4,
    MESSAGE_LEFT_BUTTON_DOWN = 8,
    MESSAGE_LEFT_BUTTON_UP = 0x10,
    MESSAGE_RIGHT_BUTTON_DOWN = 0x20,
    MESSAGE_RIGHT_BUTTON_UP = 0x40,
    MESSAGE_WIDGET = 0x200,
    MESSAGE_EXECUTIVE = 0x4000
} MessageType;

typedef enum WidgetInputCommand {
    WIDGET_COMMAND_DIALOG_SELECT = 10,
    WIDGET_COMMAND_SELECT = 12,
    WIDGET_COMMAND_DESELECT = 13,
    WIDGET_COMMAND_ALTERNATE_SELECT = 14
} WidgetInputCommand;

typedef enum MessageModifier {
    MESSAGE_MODIFIER_RIGHT_SHIFT = 1,
    MESSAGE_MODIFIER_LEFT_SHIFT = 2,
    MESSAGE_MODIFIER_CONTROL = 4,
    MESSAGE_MODIFIER_ALT = 0x20,
    MESSAGE_MODIFIER_LEFT_BUTTON = 0x100,
    MESSAGE_MODIFIER_RIGHT_BUTTON = 0x200,
    MESSAGE_MODIFIER_BUTTON_MASK = 0x300
} MessageModifier;

struct tag_messageKeyboardPayload {
    union {
        int keyCode;
        unsigned char keyByte;
    };
    int unknown0x08;
    int modifiers;
    int unknown0x10;
    int unknown0x14;
    int unknown0x18;
};
SIZE(tag_messageKeyboardPayload, 0x18);

struct tag_messageMousePayload {
    int x;
    int y;
    int modifiers;
    int screenX;
    int screenY;
    int unknown0x18;
};
SIZE(tag_messageMousePayload, 0x18);

struct tag_messageHoverPayload {
    int x;
    int id;
    int modifiers;
    int screenX;
    int screenY;
    int unknown0x18;
};
SIZE(tag_messageHoverPayload, 0x18);

union tag_messageWidgetData {
    int value;
    char *text;
};
SIZE(tag_messageWidgetData, 0x4);

struct tag_messageWidgetPayload {
    int command;
    int id;
    int parameter;
    int screenX;
    int screenY;
    tag_messageWidgetData data;
};
SIZE(tag_messageWidgetPayload, 0x18);

struct tag_messageExecutivePayload {
    int command;
    int unknown0x08;
    int unknown0x0c;
    int unknown0x10;
    int unknown0x14;
    int result;
};
SIZE(tag_messageExecutivePayload, 0x18);

struct tag_messageUnknownPayload {
    int unknown0x04;
    int unknown0x08;
    int unknown0x0c;
    int unknown0x10;
    int unknown0x14;
    int unknown0x18;
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
    int type;
    tag_messagePayload payload;
};
SIZE(tag_message, 0x1c);
#endif // HOMM2_CARCASS_TYPES_H
