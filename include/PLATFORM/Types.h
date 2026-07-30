#ifndef HOMM2_PLATFORM_TYPES_H
#define HOMM2_PLATFORM_TYPES_H

#include <cstdint>

namespace platform {

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

struct Point {
    int x = 0;
    int y = 0;
};

struct Size {
    int width = 0;
    int height = 0;
};

struct Rect {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

enum class Key {
    Unknown = 0,

    Escape,
    Return,
    Space,
    Tab,
    Backspace,
    Delete,

    Left,
    Right,
    Up,
    Down,
    Home,
    End,
    PageUp,
    PageDown,
    Insert,

    Shift,
    Control,
    Alt,

    F1, F2, F3, F4, F5, F6,
    F7, F8, F9, F10, F11, F12,

    Digit0, Digit1, Digit2, Digit3, Digit4,
    Digit5, Digit6, Digit7, Digit8, Digit9,

    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

enum class MouseButton {
    Left,
    Right,
    Middle,
};

enum Modifier : unsigned {
    ModNone = 0u,
    ModShift = 1u << 0,
    ModControl = 1u << 1,
    ModAlt = 1u << 2,
};

}

#endif
