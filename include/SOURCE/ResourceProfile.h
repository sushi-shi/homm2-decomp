#ifndef HOMM2_SOURCE_RESOURCE_PROFILE_H
#define HOMM2_SOURCE_RESOURCE_PROFILE_H

#include <SOURCE/LegacyText.h>

#include <filesystem>
#include <string>

namespace localization {

// A resource profile describes a coherent retail asset set: archive overlay,
// byte decoder and bitmap-font glyph layout. It is deliberately independent
// of the UI catalog language.
enum class ResourceProfile {
    Western,
    BukaCyrillic,
};

struct ResourceProfileSelection {
    ResourceProfile profile = ResourceProfile::Western;
    bool useOverlay = false;
};

TextEncoding ResourceTextEncoding(ResourceProfile profile);
const char* ResourceProfileName(ResourceProfile profile);

ResourceProfileSelection ChooseResourceProfile(
    bool primaryDetected,
    ResourceProfile primary,
    bool overlayDetected,
    ResourceProfile overlay,
    bool catalogRequiresProfile,
    ResourceProfile catalogProfile,
    bool hasExplicitProfile,
    ResourceProfile explicitProfile
);

// Identifies the retail profile structurally from FONT.ICN in an AGG archive.
// No copyrighted payload is extracted or retained.
bool DetectAggResourceProfile(
    const std::filesystem::path& archive,
    ResourceProfile& profile,
    std::string& error
);

}

#endif
