#include <va.h>
#include <BASE/baseManager.h>
#include <string.h>

VA(0x004b5660, 0x5d)
baseManager::baseManager(void)
    : m_next(NULL),
      m_prev(NULL) {
    m_priority = BASE_MANAGER_PRIORITY_UNASSIGNED;
    m_messageMask = BASE_MANAGER_MESSAGE_MASK_ALL;
    m_active = false;
    strcpy(m_name, "Unknown");
}
