#include <BASE/icon2bc.h>
#include <BASE/ImageDecode.h>

void IconToBitmapColorTable(
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
    u8* colorTable,
    i32 drawShadows
) {
    images::IconOptions options;
    options.color = color;
    if (colorTable == nullptr)
        return;
    options.colorTable = colorTable;
    options.shadows = drawShadows != 0;
    images::DrawIcon(sourceIcon, destination, x, y, frame, clip, clipX, clipY, clipW, clipH, options);
}
