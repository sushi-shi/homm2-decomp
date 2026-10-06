#include <Ints.h>
#include <BASE/widget.h>
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
    WidgetKind kind
)
    : widget(x, y, width, height, id, kind) {}

void dimmerWidget::Read(void) {
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

MessageDispatchResult dimmerWidget::Main(struct tag_message& message) {
    return widget::Main(message);
}

void dimmerWidget::Draw(void) {
    Dim();
}

dimmerWidget::~dimmerWidget() {}
