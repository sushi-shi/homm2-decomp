#ifndef HOMM2_SOURCE_LOCALIZATION_H
#define HOMM2_SOURCE_LOCALIZATION_H

#include <SOURCE/LegacyText.h>
#include <SOURCE/ResourceProfile.h>

#include <cstdint>
#include <cstddef>
#include <string>

namespace localization {

enum class FontProfile {
    Latin,
    BukaCyrillic,
};

// Selects and loads a catalog. This must run after platform::Startup() and
// before retail preferences/defaults are initialized.
void Initialize(const char* commandLine);

const char* Language();
FontProfile ActiveFontProfile();
ResourceProfile ActiveResourceProfile();
TextEncoding ActiveResourceTextEncoding();
TextEncoding DefaultFileTextEncoding();
bool UsesResourceOverlay();
bool HasCatalog();

// Rejects only the selected UI catalog. The resource profile and its decoder
// remain unchanged, so English UI can still run on Buka retail data.
void UseEnglish(const char* reason);

// Called by the font loader when an archive passed structural detection but
// cannot render the selected profile in practice. This falls back to primary
// resources and rejects only catalogs that require the unavailable profile.
void RejectResourceProfile(const char* reason);

// Migration bridge for recovered global char* tables. New code should use
// Tr() directly.
std::size_t ApplyLegacyTable(const char* idPrefix, char** values, std::size_t count);
std::size_t ApplyLegacyString(const char* id, char*& value);
void LocalizeLegacyTables();

// IDs are stable semantic keys. English fallbacks have one authoritative copy
// in locales/messages.def; generated POT data keeps translator catalogs in
// sync with it.
const char* Tr(const char* id);
const char* TrPlural(const char* id, std::uint32_t count);

// Original UI resources are byte-encoded. Convert their text fields once at
// the deserialization boundary; catalog and runtime strings remain UTF-8.
std::string DecodeResourceText(const char* text);
std::string DecodeExternalText(const char* text, TextEncoding encoding);
std::string DecodeExternalText(const char* text);

// The active game/map file's encoding provenance. New games inherit the
// resource edition; loaded files replace it after inspecting their raw fields.
TextEncoding CurrentFileTextEncoding();
void SetCurrentFileTextEncoding(TextEncoding encoding);

}

#endif
