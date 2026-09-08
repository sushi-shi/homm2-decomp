#ifndef HOMM2_BASE_RASTER_H
#define HOMM2_BASE_RASTER_H

#include <Ints.h>
#include <algorithm>
#include <limits>

namespace images {

// Resource transfers and allocation tracking still use signed 32-bit sizes.
// Check against that limit and the member before multiplying by the count.
inline bool RasterPayloadSize(u32 count, i32 width, i32 height, u64 available, u32& bytes) {
    bytes = 0;
    if (count == 0 || width <= 0 || height <= 0)
        return false;
    const u64 limit = std::min<u64>(available, std::numeric_limits<i32>::max());
    const u64 area = static_cast<u64>(width) * static_cast<u64>(height);
    if (area > limit / count)
        return false;
    bytes = static_cast<u32>(area * count);
    return true;
}

} // namespace images

#endif
