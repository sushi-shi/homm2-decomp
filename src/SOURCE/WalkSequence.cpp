#include <SOURCE/ARMY.h>
#include <algorithm>
#include <cstring>

void BuildTempWalkSeq(SMonFrameInfo* frameInfo, i32 finishStanding, i32 skipDrawing) {
    auto& count = frameInfo->animationFrameCount[H2EnumIndex(ARMY_ANIMATION_WALK)];
    count = 0;
    const auto append = [&](ArmyAnimationSequence sequence) {
        const auto source = H2EnumIndex(sequence);
        const i32 available = ARMY_ANIMATION_FRAME_COUNT - count;
        const i32 length = std::clamp<i32>(frameInfo->animationFrameCount[source], 0, available);
        if (length == 0)
            return;
        std::memcpy(frameInfo->animationFrames[H2EnumIndex(ARMY_ANIMATION_WALK)] + count,
                    frameInfo->animationFrames[source], length);
        std::memcpy(frameInfo->walkXOffsets + count, frameInfo->animationXOffsets[source], length);
        count += length;
    };
    if (!skipDrawing && finishStanding) {
        append(ARMY_ANIMATION_WALK_STAND);
    } else {
        append(skipDrawing ? ARMY_ANIMATION_WALK_BEGIN_STANDING : ARMY_ANIMATION_WALK_BEGIN);
        append(ARMY_ANIMATION_WALK_MIDDLE);
        append(finishStanding ? ARMY_ANIMATION_WALK_END_STANDING : ARMY_ANIMATION_WALK_END);
    }
}
