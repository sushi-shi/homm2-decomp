#include <va.h>
#include <BASE/Icon2b.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static inline i32 IconRowVisible(i32 currentY, i32 clipBottom, i32 clipTop) {
    return clipTop <= currentY && currentY <= clipBottom;
}

VA(0x004d0570, 0x4ed)
void IconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH,
    i32 color
) {
    RLE_SETUP_GENERAL;
    RLE_SETUP_FORWARD_ROW;
    RLE_SETUP_RUN;
    RLE_SETUP_COLOR;
    RLE_SETUP_DIM;
    RLE_SETUP_COUNTED_PIXELS;

    RLE_SETUP_FORWARD_ENTRY(savedDestination);
    RLE_SETUP_FORWARD_ORIGIN;
    RLE_SETUP_NORMALIZE_CLIP(s_clipR, s_clipB);
    RLE_SETUP_FORWARD_OUTPUT_ROW;

    for (;;) {
        command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                RLE_HANDLER_FORWARD_SKIP_OR_END(ICON_RLE_COMMAND_RUN_MASK);
        }
        RLE_DISPATCH_SOLID_OR_DIM(s_src, encodedColor);
        fill_run:
            RLE_HANDLER_FILL_RUN(IconRowVisible(s_y, s_clipB, clipY));
        dim_run:
            RLE_HANDLER_FORWARD_DIM_RUN(1);
        }
        s_x = X;
        s_run = command;
        if (command != 0) {
            RLE_HANDLER_FORWARD_COPY_LITERAL_RUN();
        }
        RLE_HANDLER_FORWARD_NEW_ROW(s_pitch);
    }
}
