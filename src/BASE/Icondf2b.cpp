#include <BASE/Icondf2b.h>
#include <BASE/ImageDecode.h>

void FlipDimIconToBitmap(
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
    options.mirrored = true;
    options.paint = images::IconPaint::MaskDim;
    images::DrawIcon(srcIcon, dest, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
