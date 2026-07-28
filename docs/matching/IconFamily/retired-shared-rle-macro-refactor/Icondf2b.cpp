#include <va.h>
#include <BASE/Icondf2b.h>
#include <BASE/IconDraw.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <BASE/IconMonoRle.h>
#include <BASE/IconRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>

static inline i32 FlipDimRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

VA(0x004daa20, 0x23b)
void FlipDimIconToBitmap(
    class icon* srcIcon,
    class bitmap* dest,
    i32 x,
    i32 y,
    i32 frame,
    i32 color,
    H2_ENUM_PARAM(IconDrawClipMode, i32) clip,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    RLE_SETUP_GENERAL;
    RLE_SETUP_REVERSE_ROW;
    RLE_SETUP_RUN;
    RLE_SETUP_MONO_DIM;

    u8* data = srcIcon->m_data;
    IconEntry* entry = reinterpret_cast<IconEntry*>(data) + frame;
    i32 x0 = x;
    s_entry = entry;
    s_src = data + entry->srcOffset;
    i32 width = entry->w;
    i32 entryY = entry->y;
    x0 = x0 - entry->x;
    x0 = x0 - width;
    i32 right = width + x0 + 1;
    x0++;
    s_left = x0;
    i32 X = right - 1;
    s_right = X;
    s_y = y + entryY;
    i32 currentY = s_y;
    RLE_SETUP_REVERSE_NORMALIZE_CLIP(s_clipR, s_clipB, right);

    i16 pitch = dest->m_width;
    s_row = dest->m_pixels + s_y * pitch;

    for (;;) {
        s_x = X;
        i32 command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            RLE_HANDLER_MONO_REVERSE_SKIP_OR_END(s_src);
        }
        s_run = command;
        if (command != ICON_RLE_MONO_NEWLINE_COMMAND) {
            RLE_HANDLER_MONO_REVERSE_SPAN_RUN((FlipDimRowVisible(clipY, s_y, s_clipB)), {
                s_spanCount = spanCount;
                u8* palette = uDimPal[0][color];
                u32 remaining = spanCount;
                RLE_PIXEL_ASCENDING_LOOP(spanDestination, remaining, s_dst, {
                    *spanDestination = palette[*spanDestination];
                });
            });
        }
        RLE_HANDLER_REVERSE_NEW_ROW(pitch);
    }
}
