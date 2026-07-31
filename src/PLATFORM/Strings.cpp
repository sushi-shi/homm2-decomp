#include <PLATFORM/Strings.h>

#include <cctype>
#include <cstring>

namespace platform {
namespace {

char Folded(char character) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
}

}

i32 CompareIgnoringCase(const char* left, const char* right) {
    while (Folded(*left) == Folded(*right) && *left != '\0') {
        ++left;
        ++right;
    }
    return static_cast<i32>(Folded(*left)) - static_cast<i32>(Folded(*right));
}

i32 CompareIgnoringCase(const char* left, const char* right, u32 count) {
    for (u32 index = 0; index < count; ++index) {
        if (Folded(left[index]) != Folded(right[index]) || left[index] == '\0') {
            return static_cast<i32>(Folded(left[index])) - static_cast<i32>(Folded(right[index]));
        }
    }
    return 0;
}

char* Reverse(char* text) {
    if (text == nullptr) {
        return nullptr;
    }

    const std::size_t length = std::strlen(text);
    for (std::size_t index = 0; index + 1 < length - index; ++index) {
        const char swapped = text[index];
        text[index] = text[length - 1 - index];
        text[length - 1 - index] = swapped;
    }
    return text;
}

}
