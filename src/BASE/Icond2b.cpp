#include <BASE/Icond2b.h>
#include <BASE/ImageDecode.h>

void DimIconToBitmap(
    class icon* sourceIcon,
    class bitmap* destination,
    i32 x,
    i32 y,
    i32 frame,
    i32 dimLevel,
    IconDrawClipMode clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    images::IconOptions options;
    options.color = dimLevel;
    options.paint = images::IconPaint::MaskDim;
    images::DrawIcon(sourceIcon, destination, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
