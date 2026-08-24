#include <Ints.h>
#include <BASE/resource.h>

resource::resource(void) {
    m_resourceType = RESOURCE_CATEGORY_BITMAP;
    m_refCount = RESOURCE_REFERENCE_EMPTY;
    m_id = 0;
    m_next = NULL;
}

resource::resource(ResourceCategory category, u32l id, i16 refCount, resource* next) {
    m_resourceType = category;
    m_id = id;
    m_refCount = refCount;
    m_next = next;
}

resource::~resource() {}
