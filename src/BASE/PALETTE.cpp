#include <va.h>
#include <BASE/palette.h>
#include <BASE/resourceManager.h>
#include <BASE/Misc.h>
#include <BASE/MiscGraphicsConstants.h>
#include <SOURCE/KB.h>

#define RETAIL_FILE "I:\\Projects\\Heroes\\Prog\\BASE\\PALETTE.CPP"


VA(0x004d2580, 0x2e)
palette::palette(void) : resource(RESOURCE_CATEGORY_PALETTE, 0, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC(PALETTE_DATA_SIZE, 8));
}

VA(0x004d2600, 0x55)
palette::palette(u32l id)
    : resource(RESOURCE_CATEGORY_PALETTE, id, RESOURCE_REFERENCE_INITIAL, NULL) {
    m_data = static_cast<i8*>(H2_ALLOC_AT(PALETTE_DATA_SIZE, RETAIL_FILE "\0", 13));
    gpResourceManager->PointToFile(id);
    gpResourceManager->ReadBlock(m_data, MISC_PALETTE_BYTE_COUNT);
}

VA(0x004d2660, 0x2c)
palette::~palette() {
    if (m_data != NULL)
        H2_FREE_AT(m_data, RETAIL_FILE "\0\0", 21);
    m_data = NULL;
}

VA(0x004d2690, 0x4)
i8* palette::Data(void) {
    return m_data;
}


VTBL(palette, 0x004eba7c);

#undef RETAIL_FILE
