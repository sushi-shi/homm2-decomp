#include <BASE/Utf8.h>

#include <cstring>

namespace utf8 {

Decoded Decode(const char* text) {
    if (text == nullptr || *text == '\0') {
        return {0, 0, true};
    }

    const auto* input = reinterpret_cast<const unsigned char*>(text);
    const unsigned char first = input[0];
    if (first < 0x80) {
        return {first, 1, true};
    }

    std::uint32_t value;
    std::uint32_t minimum;
    std::size_t length;
    if ((first & 0xe0) == 0xc0) {
        value = first & 0x1f;
        minimum = 0x80;
        length = 2;
    } else if ((first & 0xf0) == 0xe0) {
        value = first & 0x0f;
        minimum = 0x800;
        length = 3;
    } else if ((first & 0xf8) == 0xf0) {
        value = first & 0x07;
        minimum = 0x10000;
        length = 4;
    } else {
        return {0xfffd, 1, false};
    }

    for (std::size_t index = 1; index < length; ++index) {
        if (input[index] == 0 || (input[index] & 0xc0) != 0x80) {
            return {0xfffd, 1, false};
        }
        value = (value << 6) | (input[index] & 0x3f);
    }
    if (value < minimum || value > 0x10ffff
        || (value >= 0xd800 && value <= 0xdfff)) {
        return {0xfffd, 1, false};
    }
    return {value, length, true};
}

std::size_t Encode(std::uint32_t codePoint, char output[4]) {
    if (output == nullptr || codePoint == 0 || codePoint > 0x10ffff
        || (codePoint >= 0xd800 && codePoint <= 0xdfff)) {
        return 0;
    }
    if (codePoint < 0x80) {
        output[0] = static_cast<char>(codePoint);
        return 1;
    }
    if (codePoint < 0x800) {
        output[0] = static_cast<char>(0xc0 | (codePoint >> 6));
        output[1] = static_cast<char>(0x80 | (codePoint & 0x3f));
        return 2;
    }
    if (codePoint < 0x10000) {
        output[0] = static_cast<char>(0xe0 | (codePoint >> 12));
        output[1] = static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f));
        output[2] = static_cast<char>(0x80 | (codePoint & 0x3f));
        return 3;
    }
    output[0] = static_cast<char>(0xf0 | (codePoint >> 18));
    output[1] = static_cast<char>(0x80 | ((codePoint >> 12) & 0x3f));
    output[2] = static_cast<char>(0x80 | ((codePoint >> 6) & 0x3f));
    output[3] = static_cast<char>(0x80 | (codePoint & 0x3f));
    return 4;
}

std::size_t Next(const char* text, std::size_t offset) {
    const Decoded decoded = Decode(text != nullptr ? text + offset : nullptr);
    return offset + decoded.length;
}

std::size_t Previous(const char* text, std::size_t offset) {
    if (text == nullptr || offset == 0) {
        return 0;
    }
    std::size_t previous = offset - 1;
    const auto* bytes = reinterpret_cast<const unsigned char*>(text);
    while (previous > 0 && (bytes[previous] & 0xc0) == 0x80) {
        --previous;
    }
    const Decoded decoded = Decode(text + previous);
    return decoded.valid && previous + decoded.length == offset ? previous : offset - 1;
}

std::size_t Copy(char* destination, std::size_t capacity, const char* source) {
    if (destination == nullptr || capacity == 0) {
        return 0;
    }
    destination[0] = 0;
    if (source == nullptr) {
        return 0;
    }
    std::size_t written = 0;
    while (source[written] != 0) {
        const Decoded decoded = Decode(source + written);
        if (!decoded.valid || written + decoded.length >= capacity) {
            break;
        }
        std::memcpy(destination + written, source + written, decoded.length);
        written += decoded.length;
    }
    destination[written] = 0;
    return written;
}

std::size_t Append(char* destination, std::size_t capacity, const char* source) {
    if (destination == nullptr || capacity == 0) {
        return 0;
    }
    std::size_t used = 0;
    while (used < capacity && destination[used] != 0) {
        ++used;
    }
    if (used == capacity) {
        destination[capacity - 1] = 0;
        return capacity - 1;
    }
    return used + Copy(destination + used, capacity - used, source);
}

bool IsValid(const char* text) {
    if (text == nullptr) {
        return false;
    }
    while (*text != 0) {
        const Decoded decoded = Decode(text);
        if (!decoded.valid) {
            return false;
        }
        text += decoded.length;
    }
    return true;
}

std::uint32_t ToUpper(std::uint32_t codePoint) {
    if (codePoint >= 'a' && codePoint <= 'z') {
        return codePoint - ('a' - 'A');
    }
    if ((codePoint >= 0x00e0 && codePoint <= 0x00f6)
        || (codePoint >= 0x00f8 && codePoint <= 0x00fe)) {
        return codePoint - 0x20;
    }
    if ((codePoint >= 0x0101 && codePoint <= 0x012f && (codePoint & 1) != 0)
        || (codePoint >= 0x0133 && codePoint <= 0x0137 && (codePoint & 1) != 0)
        || (codePoint >= 0x014b && codePoint <= 0x0177 && (codePoint & 1) != 0)) {
        return codePoint - 1;
    }
    if ((codePoint >= 0x013a && codePoint <= 0x0148 && (codePoint & 1) == 0)
        || (codePoint >= 0x017a && codePoint <= 0x017e && (codePoint & 1) == 0)) {
        return codePoint - 1;
    }
    if (codePoint >= 0x0430 && codePoint <= 0x044f) {
        return codePoint - 0x20;
    }
    if ((codePoint >= 0x03b1 && codePoint <= 0x03c1)
        || (codePoint >= 0x03c3 && codePoint <= 0x03cb)) {
        return codePoint - 0x20;
    }
    switch (codePoint) {
        case 0x00ff: return 0x0178;
        case 0x0131: return 'I';
        case 0x0153: return 0x0152;
        case 0x0161: return 0x0160;
        case 0x017e: return 0x017d;
        case 0x03c2: return 0x03a3;
        case 0x0451: return 0x0401;
        case 0x0452: return 0x0402;
        case 0x0453: return 0x0403;
        case 0x0454: return 0x0404;
        case 0x0455: return 0x0405;
        case 0x0456: return 0x0406;
        case 0x0457: return 0x0407;
        case 0x0458: return 0x0408;
        case 0x0459: return 0x0409;
        case 0x045a: return 0x040a;
        case 0x045b: return 0x040b;
        case 0x045c: return 0x040c;
        case 0x045e: return 0x040e;
        case 0x045f: return 0x040f;
        case 0x0491: return 0x0490;
        default: return codePoint;
    }
}

std::uint32_t ToLower(std::uint32_t codePoint) {
    if (codePoint >= 'A' && codePoint <= 'Z') {
        return codePoint + ('a' - 'A');
    }
    if ((codePoint >= 0x00c0 && codePoint <= 0x00d6)
        || (codePoint >= 0x00d8 && codePoint <= 0x00de)) {
        return codePoint + 0x20;
    }
    if ((codePoint >= 0x0100 && codePoint <= 0x012e && (codePoint & 1) == 0)
        || (codePoint >= 0x0132 && codePoint <= 0x0136 && (codePoint & 1) == 0)
        || (codePoint >= 0x014a && codePoint <= 0x0176 && (codePoint & 1) == 0)) {
        return codePoint + 1;
    }
    if ((codePoint >= 0x0139 && codePoint <= 0x0147 && (codePoint & 1) != 0)
        || (codePoint >= 0x0179 && codePoint <= 0x017d && (codePoint & 1) != 0)) {
        return codePoint + 1;
    }
    if (codePoint >= 0x0410 && codePoint <= 0x042f) {
        return codePoint + 0x20;
    }
    if (codePoint >= 0x0391 && codePoint <= 0x03ab && codePoint != 0x03a2) {
        return codePoint + 0x20;
    }
    switch (codePoint) {
        case 0x0152: return 0x0153;
        case 0x0160: return 0x0161;
        case 0x0178: return 0x00ff;
        case 0x017d: return 0x017e;
        case 0x0401: return 0x0451;
        case 0x0402: return 0x0452;
        case 0x0403: return 0x0453;
        case 0x0404: return 0x0454;
        case 0x0405: return 0x0455;
        case 0x0406: return 0x0456;
        case 0x0407: return 0x0457;
        case 0x0408: return 0x0458;
        case 0x0409: return 0x0459;
        case 0x040a: return 0x045a;
        case 0x040b: return 0x045b;
        case 0x040c: return 0x045c;
        case 0x040e: return 0x045e;
        case 0x040f: return 0x045f;
        case 0x0490: return 0x0491;
        default: return codePoint;
    }
}

namespace {

bool TransformFirst(char* text, std::uint32_t (*transform)(std::uint32_t)) {
    const Decoded decoded = Decode(text);
    if (decoded.length == 0 || !decoded.valid) {
        return false;
    }
    char encoded[4];
    const std::size_t length = Encode(transform(decoded.codePoint), encoded);
    if (length != decoded.length) {
        return false;
    }
    std::memcpy(text, encoded, length);
    return true;
}

}

bool UppercaseFirst(char* text) { return TransformFirst(text, ToUpper); }

bool LowercaseFirst(char* text) { return TransformFirst(text, ToLower); }

bool IsLetter(std::uint32_t codePoint) {
    return (codePoint >= 'A' && codePoint <= 'Z')
        || (codePoint >= 'a' && codePoint <= 'z')
        || (codePoint >= 0x00c0 && codePoint <= 0x02af)
        || (codePoint >= 0x0370 && codePoint <= 0x052f);
}

bool EqualIgnoringCase(
    const char* first,
    const char* second,
    std::size_t maximumCodePoints
) {
    if (first == nullptr || second == nullptr) {
        return first == second;
    }
    std::size_t count = 0;
    while (count < maximumCodePoints) {
        const Decoded left = Decode(first);
        const Decoded right = Decode(second);
        if (!left.valid || !right.valid
            || ToLower(left.codePoint) != ToLower(right.codePoint)) {
            return false;
        }
        if (left.length == 0) {
            return true;
        }
        first += left.length;
        second += right.length;
        ++count;
    }
    return true;
}

}
