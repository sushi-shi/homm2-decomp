#ifndef HOMM2_PLATFORM_INPUT_H
#define HOMM2_PLATFORM_INPUT_H

#include "Types.h"

namespace platform {

struct Event {
    enum class Type {
        None,
        KeyDown,
        KeyUp,
        MouseMove,
        MouseDown,
        MouseUp,
        MouseWheel,

        Quit,

        Resized,

        FocusChanged,
    };

    Type type = Type::None;

    Key key = Key::Unknown;

    unsigned character = 0;
    unsigned modifiers = ModNone;

    unsigned scanCode = 0;

    Point position;
    MouseButton button = MouseButton::Left;
    int wheel = 0;

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
