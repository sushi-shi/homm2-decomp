#include <va.h>
#include <BASE/Iconm2b.h>
#include <BASE/IconDraw.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <BASE/IconRle.h>
#include <BASE/IconMonoRle.h>
#include <string.h>

static inline i32 MonoRowVisible(i32 clipTop, i32 currentY, i32 clipBottom) {
    return clipTop <= currentY && currentY <= clipBottom;
}

static inline u8* MonoInitialRow(bitmap* dest, i16 pitch, i32 currentY) {
    return dest->m_pixels + currentY * pitch;
}

VA(0x004cfae0, 0x266)
void MonoIconToBitmap(
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

    IconEntry* entry = &srcIcon->Entries()[frame];
    s_entry = entry;
    s_src = srcIcon->m_data + entry->srcOffset;
    i32 X = x + entry->x;
    i32 x0 = X;
    s_left = X;
    s_x = X;
    s_y = entry->y + y;
    i32 currentY = s_y;
    RLE_SETUP_NORMALIZE_CLIP(s_clipR, s_clipB);

    i16 pitch = dest->m_width;
    u8* row = MonoInitialRow(dest, pitch, s_y);
    i32 command;

    for (;;) {
        command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            RLE_HANDLER_MONO_STATIC_FORWARD_SKIP_OR_END();
        }
        s_run = command;
        if (command != ICON_RLE_MONO_NEWLINE_COMMAND) {
            RLE_HANDLER_MONO_FORWARD_FILL_RUN((MonoRowVisible(clipY, s_y, s_clipB)));
        }
        RLE_HANDLER_MONO_STATIC_FORWARD_NEW_ROW(pitch);
    }
}
