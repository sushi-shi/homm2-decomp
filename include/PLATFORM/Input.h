#ifndef HOMM2_PLATFORM_INPUT_H
#define HOMM2_PLATFORM_INPUT_H

#include "Types.h"

namespace platform {

enum InputMessageCode : unsigned {
    INPUT_MESSAGE_KEY_DOWN = 0x100,
    INPUT_MESSAGE_KEY_UP = 0x101,
    INPUT_MESSAGE_MOUSE_MOVE = 0x200,
    INPUT_MESSAGE_LEFT_DOWN = 0x201,
    INPUT_MESSAGE_LEFT_UP = 0x202,
    INPUT_MESSAGE_LEFT_DOUBLE = 0x203,
    INPUT_MESSAGE_RIGHT_DOWN = 0x204,
    INPUT_MESSAGE_RIGHT_UP = 0x205,
    INPUT_MESSAGE_RIGHT_DOUBLE = 0x206,
};

struct Event {
    enum class Type {
        None,
        KeyDown,
        KeyUp,
        MouseMove,
        MouseDown,
        MouseUp,

        Quit,

        FocusChanged,
    };

    Type type = Type::None;

    Key key = Key::Unknown;

    unsigned character = 0;
    unsigned modifiers = ModNone;

    unsigned scanCode = 0;

    Point position;
    MouseButton button = MouseButton::Left;

    bool focused = false;
};

class IInput {
public:
    virtual ~IInput() = default;

    virtual bool Poll(Event& event) = 0;

    virtual bool IsKeyDown(Key key) const = 0;
    virtual bool IsMouseButtonDown(MouseButton button) const = 0;

    virtual Point MousePosition() const = 0;
    virtual void WarpMouse(Point position) = 0;
};

}

#endif
