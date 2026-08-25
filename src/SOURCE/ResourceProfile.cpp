#include <SOURCE/ResourceProfile.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <limits>
#include <vector>

namespace localization {
namespace {

constexpr std::size_t kAggEntryBytes = 12;
constexpr std::size_t kAggNameBytes = 15;
constexpr std::uint16_t kBukaFontFrameCount = 162;

struct AggEntry {
    std::uint32_t offset;
    std::uint32_t size;
};

std::uint16_t ReadWord(const unsigned char* bytes) {
    return static_cast<std::uint16_t>(bytes[0])
        | static_cast<std::uint16_t>(bytes[1] << 8);
}

std::uint32_t ReadDword(const unsigned char* bytes) {
    return static_cast<std::uint32_t>(bytes[0])
        | (static_cast<std::uint32_t>(bytes[1]) << 8)
        | (static_cast<std::uint32_t>(bytes[2]) << 16)
        | (static_cast<std::uint32_t>(bytes[3]) << 24);
}

std::string UpperName(const std::array<char, kAggNameBytes>& raw) {
    std::string name;
    for (char character : raw) {
        if (character == '\0') {
            break;
        }
        name.push_back(static_cast<char>(
            std::toupper(static_cast<unsigned char>(character))
        ));
    }
    return name;
}

}

TextEncoding ResourceTextEncoding(ResourceProfile profile) {
    return profile == ResourceProfile::BukaCyrillic
        ? TextEncoding::Windows1251
        : TextEncoding::Windows1252;
}

const char* ResourceProfileName(ResourceProfile profile) {
    return profile == ResourceProfile::BukaCyrillic ? "buka-cyrillic" : "western";
}

ResourceProfileSelection ChooseResourceProfile(
    bool primaryDetected,
    ResourceProfile primary,
    bool overlayDetected,
    ResourceProfile overlay,
    bool catalogRequiresProfile,
    ResourceProfile catalogProfile,
    bool hasExplicitProfile,
    ResourceProfile explicitProfile
) {
    if (hasExplicitProfile) {
        return {
            explicitProfile,
            overlayDetected && overlay == explicitProfile,
        };
    }
    if (catalogRequiresProfile && overlayDetected && overlay == catalogProfile) {
        return {overlay, true};
    }
    return {
        primaryDetected ? primary : ResourceProfile::Western,
        false,
    };
}

bool DetectAggResourceProfile(
    const std::filesystem::path& archive,
    ResourceProfile& profile,
    std::string& error
) {
    std::ifstream stream(archive, std::ios::binary);
    if (!stream) {
        error = "cannot open archive";
        return false;
    }
    stream.seekg(0, std::ios::end);
    const std::streamoff rawLength = stream.tellg();
    if (rawLength < 2 || rawLength > std::numeric_limits<std::uint32_t>::max()) {
        error = "invalid archive length";
        return false;
    }
    const std::uint32_t length = static_cast<std::uint32_t>(rawLength);
    stream.seekg(0);

    unsigned char countBytes[2];
    if (!stream.read(reinterpret_cast<char*>(countBytes), sizeof(countBytes))) {
        error = "truncated archive header";
        return false;
    }
    const std::uint16_t count = ReadWord(countBytes);
    const std::uint64_t directoryEnd = 2ULL + count * kAggEntryBytes;
    const std::uint64_t namesBytes = static_cast<std::uint64_t>(count) * kAggNameBytes;
    if (count == 0 || directoryEnd > length || namesBytes > length - directoryEnd) {
        error = "invalid archive directory";
        return false;
    }
    const std::uint32_t namesOffset = length - static_cast<std::uint32_t>(namesBytes);

    std::vector<AggEntry> entries;
    entries.reserve(count);
    for (std::uint16_t index = 0; index < count; ++index) {
        unsigned char raw[kAggEntryBytes];
        if (!stream.read(reinterpret_cast<char*>(raw), sizeof(raw))) {
            error = "truncated archive directory";
            return false;
        }
        const std::uint32_t offset = ReadDword(raw + 4);
        const std::uint32_t size = ReadDword(raw + 8);
        if (offset < directoryEnd || offset > namesOffset || size > namesOffset - offset) {
            error = "archive entry lies outside the payload area";
            return false;
        }
        entries.push_back({offset, size});
    }

    stream.seekg(namesOffset);
    std::size_t fontIndex = entries.size();
    for (std::size_t index = 0; index < entries.size(); ++index) {
        std::array<char, kAggNameBytes> raw{};
        if (!stream.read(raw.data(), raw.size())) {
            error = "truncated archive name table";
            return false;
        }
        if (UpperName(raw) == "FONT.ICN") {
            fontIndex = index;
        }
    }
    if (fontIndex == entries.size() || entries[fontIndex].size < sizeof(std::uint16_t)) {
        error = "archive has no usable FONT.ICN entry";
        return false;
    }

    stream.seekg(entries[fontIndex].offset);
    unsigned char frameBytes[2];
    if (!stream.read(reinterpret_cast<char*>(frameBytes), sizeof(frameBytes))) {
        error = "truncated FONT.ICN entry";
        return false;
    }
    profile = ReadWord(frameBytes) >= kBukaFontFrameCount
        ? ResourceProfile::BukaCyrillic
        : ResourceProfile::Western;
    error.clear();
    return true;
}

}
