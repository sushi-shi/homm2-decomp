#include <PLATFORM/Types.h>

#include "src/PLATFORM/SDL3/Sdl3Internal.h"

#include <cstdio>
#include <limits>

namespace {

bool Expect(bool condition, const char* description) {
    if (condition) {
        return true;
    }
    std::fprintf(stderr, "platform primitive mismatch: %s\n", description);
    return false;
}

bool Same(
    const platform::sdl3::CopyRegion& region,
    int sourceX,
    int sourceY,
    int destinationX,
    int destinationY,
    int width,
    int height
) {
    return region.sourceX == sourceX && region.sourceY == sourceY
        && region.destinationX == destinationX && region.destinationY == destinationY
        && region.width == width && region.height == height;
}

}

int main() {
    using platform::Point;
    using platform::Rect;
    using platform::Size;
    using platform::sdl3::ClipCopyRegion;
    using platform::sdl3::CopyRegion;

    bool valid = true;
    CopyRegion region;
    valid &= Expect(
        ClipCopyRegion({1, 2, 3, 4}, {5, 6}, {20, 20}, {20, 20}, region)
            && Same(region, 1, 2, 5, 6, 3, 4),
        "ordinary copy"
    );
    valid &= Expect(
        ClipCopyRegion({-2, 1, 5, 4}, {10, 10}, {20, 20}, {20, 20}, region)
            && Same(region, 0, 1, 12, 10, 3, 4),
        "negative source origin"
    );
    valid &= Expect(
        ClipCopyRegion({2, 3, 5, 4}, {-2, -1}, {20, 20}, {20, 20}, region)
            && Same(region, 4, 4, 0, 0, 3, 3),
        "negative destination origin"
    );
    valid &= Expect(
        ClipCopyRegion({18, 17, 8, 8}, {16, 18}, {20, 20}, {20, 20}, region)
            && Same(region, 18, 17, 16, 18, 2, 2),
        "upper bounds"
    );
    valid &= Expect(
        !ClipCopyRegion({30, 0, 5, 5}, {0, 0}, {20, 20}, {20, 20}, region),
        "source outside bounds"
    );
    valid &= Expect(
        !ClipCopyRegion(
            {std::numeric_limits<int>::min(), 0, std::numeric_limits<int>::max(), 1},
            {0, 0},
            {20, 20},
            {20, 20},
            region
        ),
        "extreme coordinates"
    );

    platform::sdl3::TextInputDepth textInput;
    valid &= Expect(textInput.Acquire() && textInput.Count() == 1, "first text acquire");
    valid &= Expect(!textInput.Acquire() && textInput.Count() == 2, "nested text acquire");
    valid &= Expect(!textInput.Release() && textInput.Count() == 1, "nested text release");
    valid &= Expect(textInput.Release() && textInput.Count() == 0, "final text release");
    valid &= Expect(!textInput.Release() && textInput.Count() == 0, "text release underflow");
    textInput.Acquire();
    textInput.Acquire();
    valid &= Expect(textInput.Reset() && textInput.Count() == 0, "text reset");
    valid &= Expect(!textInput.Reset(), "inactive text reset");
    return valid ? 0 : 1;
}
