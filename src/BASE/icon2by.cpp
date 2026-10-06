#include <BASE/icon2by.h>
#include <BASE/ImageDecode.h>

void IconToBitmapYModify(
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
    i32 color,
    std::span<const i8> shear
) {
    images::IconOptions options;
    options.color = color;
    options.sheared = true;
    options.shear = shear;
    images::DrawIcon(sourceIcon, destination, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
