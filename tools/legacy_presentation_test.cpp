#include <PLATFORM/LegacyPresentation.h>

#include <cstdio>

namespace {

bool Same(platform::Rect value, int x, int y, int width, int height) {
    return value.x == x && value.y == y
        && value.width == width && value.height == height;
}

}

int main() {
    if (!Same(platform::AdventureScrollSource(3, 5, 17, 19, 0, 0), 3, 5, 17, 19)
        || !Same(platform::AdventureScrollSource(3, 5, 17, 19, 7, 0), 23, 5, 448, 19)
        || !Same(platform::AdventureScrollSource(3, 5, 17, 19, 0, -4), 3, 12, 17, 448)
        || !Same(platform::AdventureScrollSource(3, 5, 17, 19, 7, -4), 23, 12, 448, 448)) {
        std::fputs("legacy adventure-scroll source rectangle mismatch\n", stderr);
        return 1;
    }
    return 0;
}
