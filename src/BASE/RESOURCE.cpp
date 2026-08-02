#include <va.h>
#include <BASE/resource.h>

VA(0x004d4860, 0x3d)
resource::resource(void) {
    m_resourceType = RESOURCE_CATEGORY_BITMAP;
    m_id = 0;
    m_refCount = RESOURCE_REFERENCE_EMPTY;
    m_next = NULL;
}

VA(0x004d48a0, 0x41)
resource::resource(ResourceCategory category, u32l id, i16 refCount, resource* next) {
    m_resourceType = category;
    m_id = id;
    m_refCount = refCount;
    m_next = next;
}

VA(0x004d48f0, 0x14)
resource::~resource() {}


