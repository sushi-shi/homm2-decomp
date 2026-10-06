#include <BASE/Iconf2b.h>
#include <BASE/ImageDecode.h>

void FlipIconToBitmap(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color
) {
    images::IconOptions options;
    options.color = color;
    options.mirrored = true;
    images::DrawIcon(sourceIcon, destination, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
