#ifndef HOMM2_BASE_UTF8_H
#define HOMM2_BASE_UTF8_H

#include <cstddef>
#include <cstdint>

namespace utf8 {

struct Decoded {
    std::uint32_t codePoint;
    std::size_t length;
    bool valid;
};

// Invalid input consumes one byte and produces U+FFFD, so callers always make
// progress. A NUL terminator produces length zero.
Decoded Decode(const char* text);
std::size_t Encode(std::uint32_t codePoint, char output[4]);

// Offsets are byte offsets, but returned offsets are always code-point
// boundaries. This lets recovered fixed-size byte buffers remain usable while
// editing and rendering UTF-8 safely.
std::size_t Next(const char* text, std::size_t offset);
std::size_t Previous(const char* text, std::size_t offset);
std::size_t Copy(char* destination, std::size_t capacity, const char* source);
bool IsValid(const char* text);

std::uint32_t ToUpper(std::uint32_t codePoint);
std::uint32_t ToLower(std::uint32_t codePoint);
bool UppercaseFirst(char* text);
bool LowercaseFirst(char* text);
bool IsLetter(std::uint32_t codePoint);
bool EqualIgnoringCase(
    const char* first,
    const char* second,
    std::size_t maximumCodePoints = static_cast<std::size_t>(-1)
);

}

#endif
