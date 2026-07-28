#include <va.h>
#include <BASE/iconf2bc.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static inline i32 FlipColorRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

VA(0x004d9790, 0x54d)
void FlipIconToBitmapColorTable(
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
    u8* colorTable
) {
    RLE_SETUP_GENERAL;
    RLE_SETUP_REVERSE_ROW;
    RLE_SETUP_RUN;
    RLE_SETUP_COLOR;
    RLE_SETUP_DIM;
    RLE_SETUP_COUNTED_PIXELS;
    RLE_SETUP_DIM_DESTINATION;
    RLE_SETUP_REVERSE_SOURCE_SKIP;

    IconEntry* entry = &srcIcon->Entries()[frame];
    u8* src = srcIcon->m_data + entry->srcOffset;
    i32 width = entry->w;
    i32 x0 = x - entry->x - width + 1;
    i32 pitch;
    s_entry = entry;
    s_left = x0;
    i32 X = (s_right = s_left + width - 1);
    s_y = entry->y + y;
    i32 currentY = s_y;
    RLE_SETUP_NORMALIZE_CLIP(s_clipR, s_clipB);

    pitch = dest->m_width;
    s_row = dest->m_pixels + s_y * pitch;
    i32 command;
    for (;;) {
        command = ReadIconRleByte(src);
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                RLE_HANDLER_REVERSE_SKIP_OR_END(src);
            }
            RLE_DISPATCH_SOLID_OR_DIM(src, colorTable[encodedColor]);
        fill_run:
            RLE_HANDLER_REVERSE_FILL_RUN((FlipColorRowVisible(clipY, s_y, s_clipB)));
        dim_run:
            RLE_HANDLER_REVERSE_DIM_RUN((FlipColorRowVisible(clipY, s_y, s_clipB)));
        }
        s_run = command;
        s_x = X;
        if (command != 0) {
            RLE_HANDLER_REVERSE_LITERAL_RUN(src, (FlipColorRowVisible(clipY, s_y, s_clipB)), {
                *literalDestination = colorTable[*src];
            });
        }
        RLE_HANDLER_REVERSE_NEW_ROW(pitch);
    }
}
