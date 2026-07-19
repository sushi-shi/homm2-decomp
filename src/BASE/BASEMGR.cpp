#include <va.h>
#include <BASE/baseManager.h>
#include <string.h>

// @semantic: first divergence is at +0x00: base emits the vptr store before saving registers, while retail saves ebx.
VA(0x004d2530, 0x47)
baseManager::baseManager(void) {
    m_prev = m_next = 0;
    m_priority = BASE_MANAGER_PRIORITY_UNASSIGNED;
    m_messageMask = BASE_MANAGER_MESSAGE_MASK_ALL;
    m_active = false;
    strcpy(m_name, "Unknown");
}


VTBL(baseManager, 0x004eba70);
