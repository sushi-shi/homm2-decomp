#ifndef HOMM2_BASE_FONTGLYPH_H
#define HOMM2_BASE_FONTGLYPH_H

#include <H2/Ints.h>
#include <SOURCE/Localization.h>

// Shared by font drawing and measurement; profile selects the retail ICN layout.
i32 FontGlyphIndex(std::uint32_t codePoint, localization::FontProfile profile, i32 frameCount);

#endif
