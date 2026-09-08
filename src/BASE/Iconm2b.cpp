#include <BASE/Iconm2b.h>
#include <BASE/ImageDecode.h>

void MonoIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    images::IconOptions options;
    options.color = color;
    options.paint = images::IconPaint::MaskFill;
    images::DrawIcon(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
