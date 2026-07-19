#include <va.h>
#include <BASE/resource.h>

VA(0x004d9730, 0x19)
resource::resource(void) {
    m_resourceType = RESOURCE_CATEGORY_BITMAP;
    m_id = 0;
    m_refCount = 0;
    m_next = NULL;
}

VA(0x004d9750, 0x2d)
resource::resource(ResourceCategory a1, u32l a2, i16 a3, resource* a4) {
    m_resourceType = a1;
    m_id = a2;
    m_refCount = a3;
    m_next = a4;
}

VA(0x004d9780, 0x7)
resource::~resource() {}


VTBL(resource, 0x004ebaac);
