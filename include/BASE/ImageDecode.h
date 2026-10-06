#ifndef HOMM2_BASE_IMAGEDECODE_H
#define HOMM2_BASE_IMAGEDECODE_H

#include <Ints.h>
#include <BASE/IconDraw.h>
#include <span>

class icon;
class bitmap;

namespace images {

inline constexpr std::size_t IconFrameBytes = 13;

// Runtime values read explicitly from the little-endian ICN frame record.
struct IconFrame {
    i16 x, y, width, height;
    u8 kind;
    u32 offset;
};

bool ReadIconFrame(std::span<const u8> body, i32 count, i32 index, IconFrame& frame);
bool ValidateIconPayload(std::span<const u8> body, i32 count, const char*& error);

enum class IconPaint { Color, MaskFill, MaskDim };

struct IconOptions {
    bool mirrored = false;
    IconPaint paint = IconPaint::Color;
    i32 color = 0;
    bool shadows = true;
    const u8* colorTable = nullptr; // exactly 256 entries when supplied
    bool sheared = false;
    std::span<const i8> shear;
};

// A malformed frame is rejected before touching the destination. Every write
// is intersected with the bitmap even when the caller requests NO_CLIP.
bool DrawIcon(
    const icon* source, bitmap* destination, i32 x, i32 y, i32 frame,
    IconDrawClipMode clip, i32 clipX, i32 clipY, i32 clipW, i32 clipH,
    const IconOptions& options = {}
);

} // namespace images

#endif
