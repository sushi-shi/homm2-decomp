#ifndef HOMM2_SPELL_MASK_H
#define HOMM2_SPELL_MASK_H

#include <Ints.h>
#include <algorithm>
#include <span>

// Inclusive screen-row intervals may extend beyond the allocated effect mask.
inline void FillSpellMask(std::span<i8> mask, i32 first, i32 last, i8 value) {
    const i32 begin = std::clamp(first, 0, static_cast<i32>(mask.size()));
    const i32 end = std::clamp(last, -1, static_cast<i32>(mask.size()) - 1);
    if (begin <= end)
        std::fill(mask.begin() + begin, mask.begin() + end + 1, value);
}

inline void SetSpellMaskRow(std::span<i8> mask, i32 row, i8 value) {
    if (row >= 0 && static_cast<std::size_t>(row) < mask.size())
        mask[row] = value;
}

#endif
