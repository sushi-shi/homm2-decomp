#include <BASE/ImageDecode.h>
#include <BASE/icon.h>
#include <BASE/bitmap.h>
#include <BASE/IconShear.h>
#include <SOURCE/dimPalette.h>

#include <algorithm>
#include <bit>
#include <cstring>

namespace images {
namespace {

u16 Word(const u8* bytes) {
    return static_cast<u16>(bytes[0] | (static_cast<u16>(bytes[1]) << 8));
}

u32 Dword(const u8* bytes) {
    return static_cast<u32>(Word(bytes)) | (static_cast<u32>(Word(bytes + 2)) << 16);
}

enum class RunKind { Literal, Solid, Shadow, Skip, Newline };
struct Run {
    RunKind kind;
    u32 length = 0;
    u8 value = 0;
    std::span<const u8> literal;
};

// Both ICN dialects share the geometry checks. The file tag selects the
// dialect at load time; the drawing entry point selects it at draw time, as
// in retail. Streams may share suffixes and need not be stored in frame order.
template<class Visitor>
bool WalkStream(std::span<const u8> bytes, const IconFrame& frame, bool mask,
                Visitor visit, const char*& error) {
    std::size_t at = 0;
    u32 column = 0, row = 0;
    const auto byte = [&](u8& value) {
        if (at == bytes.size()) {
            error = "truncated ICN opcode or payload";
            return false;
        }
        value = bytes[at++];
        return true;
    };
    for (;;) {
        u8 opcode;
        if (!byte(opcode))
            return false;
        if (opcode == 0x80)
            return true;
        if (opcode == 0) {
            if (++row > static_cast<u32>(frame.height)) {
                error = "ICN stream exceeds its frame height";
                return false;
            }
            column = 0;
            visit(Run{RunKind::Newline});
            continue;
        }
        Run run{RunKind::Skip};
        if (mask) {
            run.length = opcode & 0x7f;
            if (opcode < 0x80)
                run.kind = RunKind::Solid;
        } else if (opcode < 0x80) {
            run.kind = RunKind::Literal;
            run.length = opcode;
            if (run.length > bytes.size() - at) {
                error = "truncated ICN literal pixels";
                return false;
            }
            run.literal = bytes.subspan(at, run.length);
            at += run.length;
        } else if (opcode < 0xc0) {
            run.length = opcode & 0x3f;
        } else if (opcode == 0xc0) {
            run.kind = RunKind::Shadow;
            if (!byte(run.value))
                return false;
            run.length = run.value & 3;
            if (run.length == 0) {
                u8 length;
                if (!byte(length))
                    return false;
                run.length = length;
            }
        } else {
            run.kind = RunKind::Solid;
            run.length = opcode & 0x3f;
            if (opcode == 0xc1) {
                u8 length;
                if (!byte(length))
                    return false;
                run.length = length;
            }
            if (!byte(run.value))
                return false;
        }
        if (run.length > static_cast<u32>(frame.width) - column
            || (run.length != 0 && row >= static_cast<u32>(frame.height))) {
            error = "ICN run exceeds its frame dimensions";
            return false;
        }
        column += run.length;
        visit(run);
    }
}

const u8* DimTable(u32 table) {
    // Index the actual nested array objects, including the mask-dim path.
    if (table < DIM_PALETTE_LIGHTEN_TABLE)
        return uDimPal[table / DIM_PALETTE_LEVEL_COUNT][table % DIM_PALETTE_LEVEL_COUNT];
    return GetDimPaletteTable(table);
}

} // namespace

bool ReadIconFrame(std::span<const u8> body, i32 count, i32 index, IconFrame& frame) {
    if (count <= 0 || count > 32767 || index < 0 || index >= count
        || static_cast<std::size_t>(count) > body.size() / IconFrameBytes)
        return false;
    const u8* bytes = body.data() + static_cast<std::size_t>(index) * IconFrameBytes;
    frame = {std::bit_cast<i16>(Word(bytes)), std::bit_cast<i16>(Word(bytes + 2)),
             std::bit_cast<i16>(Word(bytes + 4)), std::bit_cast<i16>(Word(bytes + 6)),
             bytes[8], Dword(bytes + 9)};
    return frame.width >= 0 && frame.height >= 0
        && frame.offset >= static_cast<std::size_t>(count) * IconFrameBytes
        && frame.offset < body.size();
}

bool ValidateIconPayload(std::span<const u8> body, i32 count, const char*& error) {
    error = nullptr;
    if (count <= 0 || count > 32767
        || static_cast<std::size_t>(count) > body.size() / IconFrameBytes) {
        error = "invalid or incomplete ICN frame table";
        return false;
    }
    for (i32 index = 0; index < count; ++index) {
        IconFrame frame;
        if (!ReadIconFrame(body, count, index, frame)) {
            error = "invalid ICN frame dimensions or stream offset";
            return false;
        }
        if (!WalkStream(body.subspan(frame.offset), frame, frame.kind == 32,
                        [](const Run&) {}, error))
            return false;
    }
    return true;
}

bool DrawIcon(const icon* source, bitmap* destination, i32 x, i32 y, i32 index,
              IconDrawClipMode clip, i32 clipX, i32 clipY, i32 clipW, i32 clipH,
              const IconOptions& options) {
    if (source == nullptr || source->m_data == nullptr || destination == nullptr
        || destination->m_pixels == nullptr || destination->m_width <= 0
        || destination->m_height <= 0)
        return false;
    const std::span<const u8> body(source->m_data, source->m_dataSize);
    IconFrame frame;
    if (!ReadIconFrame(body, source->m_frameCount, index, frame))
        return false;
    const bool mask = options.paint != IconPaint::Color;
    if (options.paint == IconPaint::MaskDim
        && (options.color < 0 || options.color >= DIM_PALETTE_LIGHTEN_TABLE))
        return false;
    const auto stream = body.subspan(frame.offset);
    const char* error = nullptr;
    if (!WalkStream(stream, frame, mask, [](const Run&) {}, error))
        return false;

    // Widen before adding caller coordinates or clip extents. No off-surface
    // pointer is ever formed, even for invisible rows or extreme coordinates.
    const i64 base = options.mirrored ? static_cast<i64>(x) - frame.x
                                      : static_cast<i64>(x) + frame.x;
    i64 row = static_cast<i64>(y) + frame.y;
    const i64 left = options.mirrored ? base - frame.width + 1 : base;
    const bool requestedClip = options.sheared || clip != ICON_DRAW_NO_CLIP;
    const bool contained = left >= clipX && left + frame.width <= static_cast<i64>(clipX) + clipW
        && row >= clipY && row + frame.height <= static_cast<i64>(clipY) + clipH;
    const bool legacyClip = requestedClip && (options.sheared || !contained);
    i64 clipLeft = 0, clipTop = 0;
    i64 clipRight = destination->m_width, clipBottom = destination->m_height;
    if (requestedClip) {
        if (clipW <= 0 || clipH <= 0)
            return true;
        clipLeft = std::max<i64>(clipLeft, clipX);
        clipTop = std::max<i64>(clipTop, clipY);
        clipRight = std::min<i64>(clipRight, static_cast<i64>(clipX) + clipW);
        clipBottom = std::min<i64>(clipBottom, static_cast<i64>(clipY) + clipH);
    }
    if (clipLeft >= clipRight || clipTop >= clipBottom)
        return true;
    const auto shearAt = [&](i64 at) -> i32 {
        if (!options.sheared || at < 0 || static_cast<u64>(at) >= options.shear.size())
            return 0;
        return options.shear[static_cast<std::size_t>(at)];
    };
    const i32 direction = options.mirrored ? -1 : 1;
    i64 cursor = base + direction * shearAt(row);
    const auto paint = [&](const Run& run) {
        if (run.kind == RunKind::Newline) {
            // Retail's next origin uses the row just finished; keep this lag.
            cursor = base + direction * shearAt(row);
            ++row;
            return;
        }
        const i64 runLeft = options.mirrored ? cursor - run.length + 1 : cursor;
        const i64 runRight = options.mirrored ? cursor + 1 : cursor + run.length;
        const i64 runCursor = cursor;
        cursor += direction * static_cast<i64>(run.length);
        if (run.kind == RunKind::Skip || run.length == 0 || row < clipTop || row >= clipBottom
            || (options.sheared && shearAt(row) == ICON_SHEAR_SKIP_ROW))
            return;
        // The mirrored retail decoders drop partial nonliteral runs at the
        // requested clip edge. Preserve that visible quirk independently of
        // the unconditional safety intersection with the destination.
        if (legacyClip && options.mirrored && run.kind != RunKind::Literal
            && (runLeft < clipX || runRight > static_cast<i64>(clipX) + clipW))
            return;
        const i64 begin = std::max(runLeft, clipLeft);
        const i64 end = std::min(runRight, clipRight);
        if (begin >= end)
            return;
        u8* pixels = destination->m_pixels + static_cast<std::size_t>(row) * destination->m_width;
        const u8* dim = nullptr;
        u8 solid = run.value;
        if (options.paint == IconPaint::MaskFill) {
            solid = static_cast<u8>(options.color);
        } else if (options.paint == IconPaint::MaskDim) {
            dim = DimTable(static_cast<u32>(options.color));
        } else if (run.kind == RunKind::Shadow) {
            if (options.color != 0 && (run.value & 0x80) != 0)
                solid = static_cast<u8>(options.color);
            else if (options.shadows && (run.value & 0x40) != 0)
                dim = DimTable((run.value & 0x3c) >> 2);
            else
                return;
        } else if (options.colorTable != nullptr) {
            solid = options.colorTable[solid];
        }
        if (run.kind == RunKind::Literal) {
            for (i64 column = begin; column < end; ++column) {
                const auto offset = static_cast<std::size_t>(options.mirrored
                    ? runCursor - column : column - runCursor);
                const u8 value = run.literal[offset];
                pixels[column] = options.colorTable != nullptr ? options.colorTable[value] : value;
            }
        } else if (dim != nullptr) {
            for (i64 column = begin; column < end; ++column)
                pixels[column] = dim[pixels[column]];
        } else {
            std::memset(pixels + begin, solid, static_cast<std::size_t>(end - begin));
        }
    };
    return WalkStream(stream, frame, mask, paint, error);
}

} // namespace images
