#include <SOURCE/ARMY.h>
#include <cstring>
#include <initializer_list>

int main() {
    for (int finish = 0; finish < 2; ++finish) {
        for (int skip = 0; skip < 2; ++skip) {
            for (int length : {0, 1, 5, 6, 16, 17, 127, -1}) {
                SMonFrameInfo frame{};
                for (int source = 0; source < 6; ++source) {
                    frame.animationFrameCount[source] = length;
                    std::memset(frame.animationFrames[source], source + 1, ARMY_ANIMATION_FRAME_COUNT);
                    std::memset(frame.animationXOffsets[source], -source - 1, ARMY_ANIMATION_FRAME_COUNT);
                }
                const auto before = frame;
                BuildTempWalkSeq(&frame, finish, skip);
                const int sources[] = {skip ? 1 : 0, 2, finish ? 4 : 3};
                int expectedCount = 0;
                for (int part = 0; part < ((!skip && finish) ? 1 : 3); ++part) {
                    const int source = (!skip && finish) ? 5 : sources[part];
                    for (int i = 0; i < length && expectedCount < 16; ++i, ++expectedCount) {
                        if (frame.animationFrames[6][expectedCount] != source + 1
                            || frame.walkXOffsets[expectedCount] != -source - 1)
                            return 1;
                    }
                }
                if (frame.animationFrameCount[6] != expectedCount) return 2;
                std::memcpy(frame.animationFrames[6], before.animationFrames[6], 16);
                std::memcpy(frame.walkXOffsets, before.walkXOffsets, 16);
                frame.animationFrameCount[6] = before.animationFrameCount[6];
                if (std::memcmp(&frame, &before, sizeof(frame)) != 0) return 3;
            }
        }
    }
    return 0;
}
