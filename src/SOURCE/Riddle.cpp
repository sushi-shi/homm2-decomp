#include <SOURCE/EVENTS.h>
#include <BASE/Utf8.h>

#include <array>
#include <cstddef>

i32 RiddleStringsEqual(const char* answer, const char* expected) {
    if (answer == nullptr || expected == nullptr)
        return 0;

    // Retail compares a four-character field, padded with NULs for short
    // answers. Keep those rules while counting characters rather than bytes.
    std::array<std::uint32_t, 4> prefix{};
    for (std::uint32_t& codePoint : prefix) {
        const utf8::Decoded decoded = utf8::Decode(expected);
        if (!decoded.valid)
            return 0;
        codePoint = decoded.codePoint;
        expected += decoded.length;
    }

    std::size_t count = prefix.size();
    for (std::size_t index = prefix.size(); index > 0; --index) {
        if (prefix[index - 1] != ' ') {
            count = index;
            break;
        }
    }
    for (std::size_t index = 0; index < count && prefix[index] != 0; ++index) {
        const utf8::Decoded decoded = utf8::Decode(answer);
        if (!decoded.valid || utf8::ToLower(decoded.codePoint) != utf8::ToLower(prefix[index]))
            return 0;
        answer += decoded.length;
    }
    return 1;
}
