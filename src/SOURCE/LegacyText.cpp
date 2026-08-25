#include <SOURCE/LegacyText.h>

#include <BASE/Utf8.h>

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace localization {
namespace {

std::uint32_t DecodeWestern(unsigned char byte) {
    if (byte < 0x80 || byte >= 0xa0) {
        return byte;
    }
    constexpr std::uint32_t table[] = {
        0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
        0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017d, 0xfffd,
        0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
        0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178,
    };
    return table[byte - 0x80];
}

std::uint32_t DecodeCyrillic(unsigned char byte) {
    if (byte < 0x80) {
        return byte;
    }
    if (byte >= 0xc0) {
        return 0x0410 + byte - 0xc0;
    }
    constexpr std::uint32_t table[] = {
        0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
        0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
        0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
        0xfffd, 0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
        0x00a0, 0x040e, 0x045e, 0x0408, 0x00a4, 0x0490, 0x00a6, 0x00a7,
        0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x0407,
        0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5, 0x00b6, 0x00b7,
        0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455, 0x0457,
    };
    return table[byte - 0x80];
}

bool EncodeWestern(std::uint32_t codePoint, unsigned char& byte) {
    if (codePoint < 0x80 || (codePoint >= 0xa0 && codePoint <= 0xff)) {
        byte = static_cast<unsigned char>(codePoint);
        return true;
    }
    for (unsigned int candidate = 0x80; candidate < 0xa0; ++candidate) {
        if (DecodeWestern(static_cast<unsigned char>(candidate)) == codePoint) {
            byte = static_cast<unsigned char>(candidate);
            return true;
        }
    }
    return false;
}

bool EncodeCyrillic(std::uint32_t codePoint, unsigned char& byte) {
    if (codePoint < 0x80) {
        byte = static_cast<unsigned char>(codePoint);
        return true;
    }
    if (codePoint >= 0x0410 && codePoint <= 0x044f) {
        byte = static_cast<unsigned char>(0xc0 + codePoint - 0x0410);
        return true;
    }
    for (unsigned int candidate = 0x80; candidate < 0xc0; ++candidate) {
        if (DecodeCyrillic(static_cast<unsigned char>(candidate)) == codePoint) {
            byte = static_cast<unsigned char>(candidate);
            return true;
        }
    }
    return false;
}

}

std::string DecodeText(const char* text, TextEncoding encoding) {
    if (text == nullptr) {
        return std::string();
    }
    if (encoding == TextEncoding::Utf8) {
        return text;
    }

    std::string result;
    result.reserve(std::strlen(text) * 2);
    for (const auto* byte = reinterpret_cast<const unsigned char*>(text);
         *byte != 0; ++byte) {
        const std::uint32_t codePoint = encoding == TextEncoding::Windows1251
            ? DecodeCyrillic(*byte)
            : DecodeWestern(*byte);
        char encoded[4];
        const std::size_t length = utf8::Encode(codePoint, encoded);
        result.append(encoded, length);
    }
    return result;
}

bool EncodeText(
    const char* text,
    TextEncoding encoding,
    char* destination,
    std::size_t capacity
) {
    if (destination == nullptr || capacity == 0) {
        return text == nullptr || *text == '\0';
    }
    destination[0] = '\0';
    if (text == nullptr) {
        return true;
    }
    if (encoding == TextEncoding::Utf8) {
        return utf8::Copy(destination, capacity, text) == std::strlen(text);
    }

    bool complete = true;
    std::size_t output = 0;
    const char* cursor = text;
    while (*cursor != '\0') {
        const utf8::Decoded decoded = utf8::Decode(cursor);
        unsigned char byte = '?';
        const bool representable = decoded.valid
            && (encoding == TextEncoding::Windows1251
                    ? EncodeCyrillic(decoded.codePoint, byte)
                    : EncodeWestern(decoded.codePoint, byte));
        complete = complete && representable;
        if (output + 1 >= capacity) {
            complete = false;
            break;
        }
        destination[output++] = static_cast<char>(byte);
        cursor += std::max<std::size_t>(decoded.length, 1);
    }
    destination[output] = '\0';
    return complete;
}

TextEncoding DetectTextEncoding(
    const char* const* texts,
    std::size_t count,
    TextEncoding fallback
) {
    bool hasUtf8NonAscii = false;
    for (std::size_t index = 0; index < count; ++index) {
        const char* text = texts != nullptr ? texts[index] : nullptr;
        if (text == nullptr) {
            continue;
        }
        if (!utf8::IsValid(text)) {
            return fallback;
        }
        for (const auto* byte = reinterpret_cast<const unsigned char*>(text);
             *byte != 0; ++byte) {
            hasUtf8NonAscii = hasUtf8NonAscii || *byte >= 0x80;
        }
    }
    return hasUtf8NonAscii ? TextEncoding::Utf8 : fallback;
}

const char* TextEncodingName(TextEncoding encoding) {
    switch (encoding) {
        case TextEncoding::Utf8:
            return "utf-8";
        case TextEncoding::Windows1251:
            return "windows-1251";
        case TextEncoding::Windows1252:
            return "windows-1252";
    }
    return "unknown";
}

}
