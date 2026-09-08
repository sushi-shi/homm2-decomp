#ifndef HOMM2_SOURCE_LEGACY_TEXT_H
#define HOMM2_SOURCE_LEGACY_TEXT_H

#include <cstddef>
#include <string>
#include <string_view>

namespace localization {

// Text is UTF-8 everywhere after it crosses an I/O boundary. These values
// describe only the byte encoding at a legacy resource or file boundary.
enum class TextEncoding {
    Utf8,
    Windows1252,
    Windows1251,
};

std::string DecodeText(const char* text, TextEncoding encoding);

// Serialized fields carry a capacity, unlike runtime C strings. Inspection
// and decoding stop at the first NUL or the supplied bound, whichever is first.
template <std::size_t Capacity>
constexpr std::string_view TextField(const char (&text)[Capacity]) {
    return {text, Capacity};
}

bool HasTextTerminator(std::string_view field);
std::string DecodeText(std::string_view field, TextEncoding encoding);

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

TextEncoding DetectTextEncoding(
    const std::string_view* fields,
    std::size_t count,
    TextEncoding fallback
);

const char* TextEncodingName(TextEncoding encoding);

}

#endif
