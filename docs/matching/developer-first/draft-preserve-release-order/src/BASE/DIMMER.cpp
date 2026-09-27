#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>

VA(0x004d3380, 0x77)
void dimmerWidget::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VTBL(dimmerWidget, 0x004eaa04)
