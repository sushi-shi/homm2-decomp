#ifndef HOMM2_BASE_UTF8_H
#define HOMM2_BASE_UTF8_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

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
std::size_t Append(char* destination, std::size_t capacity, const char* source);
bool IsValid(const char* text);

template <std::size_t Capacity>
std::size_t Copy(char (&destination)[Capacity], const char* source) {
    return Copy(destination, Capacity, source);
}

template <std::size_t Capacity>
std::size_t Append(char (&destination)[Capacity], const char* source) {
    return Append(destination, Capacity, source);
}

namespace detail {

inline void TrimInvalidTail(char* text) {
    std::size_t offset = 0;
    while (text[offset] != '\0') {
        const Decoded decoded = Decode(text + offset);
        if (!decoded.valid) {
            text[offset] = '\0';
            return;
        }
        offset += decoded.length;
    }
}

}

// Formats directly into a fixed destination. If snprintf truncates a UTF-8
// sequence, remove that incomplete final code point. The array overload keeps
// the destination capacity at the call site instead of relying on a convention.
template <typename... Args>
bool Format(
    char* destination,
    std::size_t capacity,
    const char* format,
    Args... args
) {
    if (destination == nullptr || capacity == 0 || format == nullptr) {
        return false;
    }
    destination[0] = '\0';
    if constexpr (sizeof...(Args) == 0) {
        return Copy(destination, capacity, format) == std::strlen(format);
    } else {
        const int written = std::snprintf(destination, capacity, format, args...);
        if (written < 0) {
            destination[0] = '\0';
            return false;
        }
        if (static_cast<std::size_t>(written) >= capacity) {
            detail::TrimInvalidTail(destination);
            return false;
        }
        return true;
    }
}

template <std::size_t Capacity, typename... Args>
bool Format(char (&destination)[Capacity], const char* format, Args... args) {
    return Format(destination, Capacity, format, args...);
}

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
