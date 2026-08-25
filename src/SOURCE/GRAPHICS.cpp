#include <SOURCE/graphics.h>

#include <algorithm>

Point Point::ClipToRect(const H2RECT& rect) const {
    return Point(
        std::min(std::max(x, rect.left), rect.right),
        std::min(std::max(y, rect.top), rect.bottom)
    );
}

H2RECT H2RECT::ClipToBounds(const H2RECT& other) const {
    return H2RECT(
        RectTop(std::max(top, other.top)),
        RectBottom(std::min(bottom, other.bottom)),
        RectLeft(std::max(left, other.left)),
        RectRight(std::min(right, other.right))
    );
}

H2RECT H2RECT::UnionWith(const H2RECT& other) const {
    return H2RECT(
        RectTop(std::min(top, other.top)),
        RectBottom(std::max(bottom, other.bottom)),
        RectLeft(std::min(left, other.left)),
        RectRight(std::max(right, other.right))
    );
}

H2RECT RectAroundPoint(const Point& point, i32 halfWidth, i32 halfHeight) {
    return H2RECT(
        RectTop(point.y - halfHeight),
        RectBottom(point.y + halfHeight),
        RectLeft(point.x - halfWidth),
        RectRight(point.x + halfWidth)
    );
}
