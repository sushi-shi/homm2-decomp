#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Ints.h>

/*
 * Ironfist's tagged rectangle helpers, used by the combat renderer to
 * merge and clip effect extents. The tag structs keep the constructor
 * arguments from being swapped silently.
 */

struct RectTop {
    i32 value;
    explicit RectTop(i32 v) : value(v) {}
};
struct RectBottom {
    i32 value;
    explicit RectBottom(i32 v) : value(v) {}
};
struct RectLeft {
    i32 value;
    explicit RectLeft(i32 v) : value(v) {}
};
struct RectRight {
    i32 value;
    explicit RectRight(i32 v) : value(v) {}
};

struct H2RECT;

struct Point {
    i32 x;
    i32 y;

    Point() : x(0), y(0) {}
    Point(i32 px, i32 py) : x(px), y(py) {}
    Point ClipToRect(const H2RECT&) const;
};

struct H2RECT {
    i32 left;
    i32 right;  // Inclusive.
    i32 top;
    i32 bottom; // Inclusive.

    H2RECT() : left(0), right(0), top(0), bottom(0) {}
    H2RECT(RectTop t, RectBottom b, RectLeft l, RectRight r)
        : left(l.value), right(r.value), top(t.value), bottom(b.value) {}

    H2RECT ClipToBounds(const H2RECT&) const;
    H2RECT UnionWith(const H2RECT&) const;

    i32 Width() const {
        return right - left + 1;
    }
    i32 Height() const {
        return bottom - top + 1;
    }
};

H2RECT RectAroundPoint(const Point&, i32 halfWidth, i32 halfHeight);

#endif
