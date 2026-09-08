#ifndef HOMM2_BASE_FONT_GLYPH_H
#define HOMM2_BASE_FONT_GLYPH_H

#include <Ints.h>
#include <SOURCE/Localization.h>

// Shared by font drawing and measurement; profile selects the retail ICN layout.
i32 FontGlyphIndex(std::uint32_t codePoint, localization::FontProfile profile, i32 frameCount);

#endif
