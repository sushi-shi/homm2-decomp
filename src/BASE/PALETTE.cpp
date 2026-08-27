#include <va.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/MiscGraphicsConstants.h>
#include <SOURCE/KB.h>


VA(0x004caea0, 0x6f)
palette::palette(void) : resource(RESOURCE_CATEGORY_PALETTE, 0, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
}

VA(0x004caf40, 0x99)
palette::palette(u32l id)
    : resource(RESOURCE_CATEGORY_PALETTE, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE));
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(m_data, MISC_PALETTE_BYTE_COUNT);
}

VA(0x004cafe0, 0x46)
H2_RETAIL_INLINE palette::~palette() {
    if (m_data != NULL)
        H2_FREE(m_data);
    m_data = NULL;
}

VA(0x004cb030, 0x11)
i8* palette::Data(void) {
    return m_data;
}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(palette, 0x004ea9c0)
