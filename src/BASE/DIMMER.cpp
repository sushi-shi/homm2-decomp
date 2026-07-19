#include <va.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004dd330, 0x1e)
dimmerWidget::dimmerWidget(void) : widget(0, 0, 0, 0, 0, 0) {}

VA(0x004dd350, 0x34)
dimmerWidget::dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id, i16 kind)
    : widget(x, y, width, height, id, kind) {}

VA(0x004dd390, 0x5f)
void dimmerWidget::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VA(0x004dd3f0, 0xd)
i32 dimmerWidget::Main(struct tag_message& msg) {
    return widget::Main(msg);
}

VA(0x004dd400, 0x5)
void dimmerWidget::Draw(void) {
    Dim();
}


VTBL(dimmerWidget, 0x004ebae0);
