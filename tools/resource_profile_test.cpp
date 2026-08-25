#include <SOURCE/ResourceProfile.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace {

void PutWord(std::ofstream& stream, std::uint16_t value) {
    const std::array<char, 2> bytes = {
        static_cast<char>(value & 0xff),
        static_cast<char>((value >> 8) & 0xff),
    };
    stream.write(bytes.data(), bytes.size());
}

void PutDword(std::ofstream& stream, std::uint32_t value) {
    const std::array<char, 4> bytes = {
        static_cast<char>(value & 0xff),
        static_cast<char>((value >> 8) & 0xff),
        static_cast<char>((value >> 16) & 0xff),
        static_cast<char>((value >> 24) & 0xff),
    };
    stream.write(bytes.data(), bytes.size());
}

bool WriteArchive(const std::filesystem::path& path, std::uint16_t frameCount) {
    std::ofstream stream(path, std::ios::binary);
    if (!stream) {
        return false;
    }
    PutWord(stream, 1);
    PutDword(stream, 0x12345678);
    PutDword(stream, 14);
    PutDword(stream, 2);
    PutWord(stream, frameCount);
    std::array<char, 15> name{};
    const std::string font = "FONT.ICN";
    std::copy(font.begin(), font.end(), name.begin());
    stream.write(name.data(), name.size());
    return stream.good();
}

}

int main() {
    using localization::ResourceProfile;
    const localization::ResourceProfileSelection englishOnBuka =
        localization::ChooseResourceProfile(
            true, ResourceProfile::BukaCyrillic,
            false, ResourceProfile::Western,
            false, ResourceProfile::Western,
            false, ResourceProfile::Western
        );
    const localization::ResourceProfileSelection russianOverlay =
        localization::ChooseResourceProfile(
            true, ResourceProfile::Western,
            true, ResourceProfile::BukaCyrillic,
            true, ResourceProfile::BukaCyrillic,
            false, ResourceProfile::Western
        );
    const localization::ResourceProfileSelection missingRussianAssets =
        localization::ChooseResourceProfile(
            true, ResourceProfile::Western,
            false, ResourceProfile::BukaCyrillic,
            true, ResourceProfile::BukaCyrillic,
            false, ResourceProfile::Western
        );
    if (englishOnBuka.profile != ResourceProfile::BukaCyrillic
        || englishOnBuka.useOverlay
        || russianOverlay.profile != ResourceProfile::BukaCyrillic
        || !russianOverlay.useOverlay
        || missingRussianAssets.profile != ResourceProfile::Western
        || missingRussianAssets.useOverlay) {
        std::fputs("resource-profile selection matrix mismatch\n", stderr);
        return 1;
    }

    const std::filesystem::path path =
        std::filesystem::temp_directory_path() / "homm2-resource-profile-test.agg";
    ResourceProfile profile = ResourceProfile::Western;
    std::string error;
    if (!WriteArchive(path, 96)
        || !localization::DetectAggResourceProfile(path, profile, error)
        || profile != ResourceProfile::Western
        || !WriteArchive(path, 162)
        || !localization::DetectAggResourceProfile(path, profile, error)
        || profile != ResourceProfile::BukaCyrillic) {
        std::fprintf(stderr, "AGG resource-profile detection mismatch: %s\n", error.c_str());
        std::filesystem::remove(path);
        return 1;
    }
    std::filesystem::remove(path);
    return 0;
}
