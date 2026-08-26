#include "Sdl3Internal.h"

#include <algorithm>
#include <cstdint>
#include <limits>

namespace platform::sdl3 {

bool ClipCopyRegion(
    Rect source,
    Point destination,
    Size sourceBounds,
    Size destinationBounds,
    CopyRegion& clipped
) {
    if (source.width <= 0 || source.height <= 0
        || sourceBounds.width <= 0 || sourceBounds.height <= 0
        || destinationBounds.width <= 0 || destinationBounds.height <= 0) {
        return false;
    }

    std::int64_t sourceX = source.x;
    std::int64_t sourceY = source.y;
    std::int64_t destinationX = destination.x;
    std::int64_t destinationY = destination.y;
    std::int64_t width = source.width;
    std::int64_t height = source.height;

    const auto clipLow = [](std::int64_t& from, std::int64_t& to, std::int64_t& extent) {
        if (from < 0) {
            const std::int64_t amount = -from;
            from = 0;
            to += amount;
            extent -= amount;
        }
        if (to < 0) {
            const std::int64_t amount = -to;
            to = 0;
            from += amount;
            extent -= amount;
        }
    };
    clipLow(sourceX, destinationX, width);
    clipLow(sourceY, destinationY, height);

    width = std::min(
        {width,
         static_cast<std::int64_t>(sourceBounds.width) - sourceX,
         static_cast<std::int64_t>(destinationBounds.width) - destinationX}
    );
    height = std::min(
        {height,
         static_cast<std::int64_t>(sourceBounds.height) - sourceY,
         static_cast<std::int64_t>(destinationBounds.height) - destinationY}
    );
    if (width <= 0 || height <= 0) {
        return false;
    }

    constexpr std::int64_t maximum = std::numeric_limits<int>::max();
    if (sourceX > maximum || sourceY > maximum || destinationX > maximum
        || destinationY > maximum || width > maximum || height > maximum) {
        return false;
    }

    clipped = {
        static_cast<int>(sourceX),
        static_cast<int>(sourceY),
        static_cast<int>(destinationX),
        static_cast<int>(destinationY),
        static_cast<int>(width),
        static_cast<int>(height),
    };
    return true;
}

bool TextInputDepth::Acquire() {
    if (m_count == std::numeric_limits<unsigned>::max()) {
        return false;
    }
    return m_count++ == 0;
}

bool TextInputDepth::Release() {
    if (m_count == 0) {
        return false;
    }
    --m_count;
    return m_count == 0;
}

bool TextInputDepth::Reset() {
    const bool active = m_count != 0;
    m_count = 0;
    return active;
}

}
