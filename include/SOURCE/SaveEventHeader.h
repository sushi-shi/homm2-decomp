#ifndef HOMM2_SAVE_EVENT_HEADER_H
#define HOMM2_SAVE_EVENT_HEADER_H

#include <Ints.h>
#include <array>

// Retail repeats the first index in this four-byte prefix, even for count=0.
// It is a wire record, not four bytes belonging to the runtime count field.
using SaveEventHeader = std::array<u8, 4>;
inline SaveEventHeader EncodeSaveEventHeader(u16 count, u16 firstIndex) {
    return {static_cast<u8>(count), static_cast<u8>(count >> 8),
            static_cast<u8>(firstIndex), static_cast<u8>(firstIndex >> 8)};
}
inline void DecodeSaveEventHeader(const SaveEventHeader& bytes, u16& count, u16& firstIndex) {
    count = bytes[0] | (static_cast<u16>(bytes[1]) << 8);
    firstIndex = bytes[2] | (static_cast<u16>(bytes[3]) << 8);
}

#endif
