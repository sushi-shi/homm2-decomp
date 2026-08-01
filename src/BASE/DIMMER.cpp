#include <va.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
dimmerWidget::dimmerWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}

dimmerWidget::dimmerWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 id,
    H2_ENUM_PARAM(WidgetKind, i16) kind
)
    : widget(x, y, width, height, id, kind) {}

void dimmerWidget::Read(void) {
    m_x = gpResourceManager->ReadWord();
    m_y = gpResourceManager->ReadWord();
    m_width = gpResourceManager->ReadWord();
    m_height = gpResourceManager->ReadWord();
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

MessageDispatchResult dimmerWidget::Main(struct tag_message& msg) {
    return widget::Main(msg);
}

void dimmerWidget::Draw(void) {
    Dim();
}


