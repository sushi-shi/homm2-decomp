// Diagnostic for the 2026-09-08 audit, not a passing regression test.
// Links the production REQUEST.cpp, LegacyText.cpp, and Utf8.cpp.
#include <SOURCE/REQUEST.h>
#include <SOURCE/Localization.h>
#include <cstring>

namespace localization {
// Avoid game startup; the fallback does not affect the invalid read.
TextEncoding DefaultFileTextEncoding() { return TextEncoding::Windows1252; }
}

int main(int argc, char**) {
    auto* header = new SMapHeader;
    std::memset(header, 'A', sizeof(*header));
    header->magic = MAP_HEADER_MAGIC_EXPANSION_GAME;
    header->width = header->height = MAP_DIMENSION_SMALL;
    header->name[0] = '\0';
    if (argc > 1) header->description[sizeof(header->description) - 1] = '\0';
    const auto encoding = GetMapHeaderTextEncoding(header);
    delete header;
    return encoding == localization::TextEncoding::Windows1252 ? 0 : 2;
}
