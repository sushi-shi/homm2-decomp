#include <va.h>
#include <BASE/iconf2by.h>
#include <BASE/IconEntry.h>
#include <BASE/IconMacro.h>
#include <BASE/IconRle.h>
#include <BASE/IconShear.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <SOURCE/dimPalette.h>
#include <string.h>

static inline i32 IconRowVisible(i8* shear, i32 clipTop, i32 currentY, i32 clipBottom) {
    return shear[currentY] != ICON_SHEAR_SKIP_ROW && clipTop <= currentY && currentY <= clipBottom;
}

VA(0x004d9ce0, 0x58d)
void FlipIconToBitmapYModify(
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
    i8* shear
) {
    RLE_SETUP_GENERAL;
    RLE_SETUP_REVERSE_ROW;
    RLE_SETUP_SIGNED_RUN;
    RLE_SETUP_COLOR;
    RLE_SETUP_SIGNED_DIM;
    RLE_SETUP_INDEXED_DIM_PIXELS;
    RLE_SETUP_REVERSE_LITERAL;

    IconEntry* entry = &srcIcon->Entries()[frame];
    s_entry = entry;
    s_src = srcIcon->m_data + entry->srcOffset;
    i32 width = entry->w;
    s_left = x - entry->x - width + 1;
    s_right = s_left + width - 1;
    s_y = entry->y + y;
    s_x = s_right - shear[s_y];
    s_clipB = clipY + clipH - 1;
    s_clipR = clipX + clipW - 1;
    s_row = dest->m_pixels + dest->m_width * s_y;
    i32 command;

    for (;;) {
        command = ReadIconRleByte(s_src);
        if (static_cast<i8>(command) < 0) {
            if ((command & ICON_RLE_COMMAND_SOLID_FLAG) == 0) {
                RLE_HANDLER_SHEARED_REVERSE_SKIP_OR_END();
            }
            RLE_DISPATCH_SOLID_OR_DIM(s_src, encodedColor);
        fill_run:
            RLE_HANDLER_SHEARED_REVERSE_FILL_RUN((IconRowVisible(shear, clipY, s_y, s_clipB)));
        dim_run:
            RLE_HANDLER_SHEARED_REVERSE_DIM_RUN((IconRowVisible(shear, clipY, s_y, s_clipB)));
        }
        s_run = command;
        if (command != 0) {
            RLE_HANDLER_SHEARED_REVERSE_LITERAL_RUN((IconRowVisible(shear, clipY, s_y, s_clipB)));
        }
        RLE_HANDLER_SHEARED_REVERSE_NEW_ROW();
    }
}
