#ifndef HOMM2_PLATFORM_LEGACY_PRESENTATION_H
#define HOMM2_PLATFORM_LEGACY_PRESENTATION_H

#include <PLATFORM/Types.h>

namespace platform {

// DirectDraw painted adventure-map scrolls from the 16-pixel staging margin
// around the 448x448 viewport. Keeping this pure makes the recovered paint
// contract independently testable on Linux, Windows and Web builds.
inline Rect AdventureScrollSource(
    int dirtyX,
    int dirtyY,
    int dirtyWidth,
    int dirtyHeight,
    int scrollX,
    int scrollY
) {
    constexpr int margin = 16;
    constexpr int extent = 448;
    return {
        scrollX != 0 ? scrollX + margin : dirtyX,
        scrollY != 0 ? scrollY + margin : dirtyY,
        scrollX != 0 ? extent : dirtyWidth,
        scrollY != 0 ? extent : dirtyHeight,
    };
}

}

#endif
