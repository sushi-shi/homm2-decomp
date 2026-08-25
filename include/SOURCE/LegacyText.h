#ifndef HOMM2_SOURCE_LEGACY_TEXT_H
#define HOMM2_SOURCE_LEGACY_TEXT_H

#include <cstddef>
#include <string>

namespace localization {

// Text is UTF-8 everywhere after it crosses an I/O boundary. These values
// describe only the byte encoding at a legacy resource or file boundary.
enum class TextEncoding {
    Utf8,
    Windows1252,
    Windows1251,
};

std::string DecodeText(const char* text, TextEncoding encoding);

// Encodes a NUL-terminated UTF-8 string into a fixed-size retail field. The
// destination is always NUL-terminated when capacity is non-zero. A false
// result means the text was truncated or contained an unrepresentable scalar;
// unrepresentable scalars are written as '?'.
bool EncodeText(
    const char* text,
    TextEncoding encoding,
    char* destination,
    std::size_t capacity
);

// A legacy file has no charset tag. Prefer UTF-8 only when all inspected
// fields are valid and at least one contains a non-ASCII UTF-8 sequence;
// otherwise retain the edition-derived fallback as the file's provenance.
TextEncoding DetectTextEncoding(
    const char* const* texts,
    std::size_t count,
    TextEncoding fallback
);

const char* TextEncodingName(TextEncoding encoding);

}

#endif
