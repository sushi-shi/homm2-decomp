#include <va.h>
#include <BASE/resource.h>

VA(0x004d9730, 0x19)
resource::resource(void) {
    m_resourceType = RESOURCE_CATEGORY_BITMAP;
    m_id = 0;
    m_refCount = RESOURCE_REFERENCE_EMPTY;
    m_next = NULL;
}

VA(0x004d9750, 0x2d)
resource::resource(ResourceCategory category, u32l id, i16 refCount, resource* next) {
    m_resourceType = category;
    m_id = id;
    m_refCount = refCount;
    m_next = next;
}

VA(0x004d9780, 0x7)
resource::~resource() {}


VTBL(resource, 0x004ebaac);
