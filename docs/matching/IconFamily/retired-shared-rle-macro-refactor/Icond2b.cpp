#include <va.h>
#include <BASE/Icond2b.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>

VA(0x004cfd50, 0x26e)
void DimIconToBitmap(
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
    RLE_SETUP_RUN;
    RLE_SETUP_MONO_DIM;

    IconEntry* entry = &srcIcon->Entries()[frame];
    s_entry = entry;
    s_src = srcIcon->m_data + entry->srcOffset;
    i32 X = x + entry->x;
    i32 x0 = X;
    s_left = X;
    s_y = entry->y + y;
    i32 currentY = s_y;
    RLE_SETUP_NORMALIZE_CLIP(s_clipR, s_clipB);

    i16 pitch = dest->m_width;
    u8* row = dest->m_pixels + pitch * s_y;
    i32 command;

    for (;;) {
        command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            RLE_HANDLER_FORWARD_SKIP_OR_END(ICON_RLE_MONO_RUN_MASK);
        }
        s_x = X;
        s_run = command;
        if (command != ICON_RLE_MONO_NEWLINE_COMMAND) {
            RLE_HANDLER_MONO_FORWARD_SPAN_RUN((clipY <= s_y && s_y <= s_clipB), {
                s_spanCount = spanCount;
                u8* palette = uDimPal[0][color];
                u32 remaining = spanCount;
                RLE_PIXEL_ASCENDING_LOOP(spanDestination, remaining, s_dst, {
                    *spanDestination = palette[*spanDestination];
                });
            });
        }
        RLE_HANDLER_FORWARD_NEW_ROW(pitch);
    }
}
