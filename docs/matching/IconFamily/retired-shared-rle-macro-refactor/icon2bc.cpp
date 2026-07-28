#include <va.h>
#include <BASE/icon2bc.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

VA(0x004d32a0, 0x5af)
void IconToBitmapColorTable(
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
    i32 color,
    u8* colorTable,
    i32 dimGate
) {
    RLE_SETUP_GENERAL;
    RLE_SETUP_FORWARD_ROW;
    RLE_SETUP_RUN;
    RLE_SETUP_COLOR;
    RLE_SETUP_DIM;
    RLE_SETUP_COUNTED_PIXELS;
    RLE_SETUP_COLOR_TABLE_LITERAL;

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
        RLE_DISPATCH_SOLID_OR_DIM(s_src, colorTable[encodedColor]);
        fill_run:
            RLE_HANDLER_FILL_RUN(clipY <= s_y && s_y <= s_clipB);
        dim_run:
            RLE_HANDLER_FORWARD_DIM_RUN(dimGate);
        }
        s_x = X;
        s_run = command;
        if (command != 0) {
            RLE_HANDLER_FORWARD_MAPPED_LITERAL_RUN();
        }
        RLE_HANDLER_FORWARD_NEW_ROW(s_pitch);
    }
}
