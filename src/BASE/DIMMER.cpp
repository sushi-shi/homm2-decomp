#include <va.h>
#include <BASE/widget.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <SOURCE/KB.h>
VA(0x004d3310, 0x2b)
dimmerWidget::dimmerWidget(void) : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}

VA(0x004d3340, 0x3f)
dimmerWidget::dimmerWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 id,
    H2_ENUM_PARAM(WidgetKind, i16) kind
)
    : widget(x, y, width, height, id, kind) {}

VA(0x004d3380, 0x77)
void dimmerWidget::Read(void) {
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);
    m_id = gpResourceManager->ReadWord();
    m_kind = gpResourceManager->ReadWord();
}

VA(0x004d3400, 0x19)
MessageDispatchResult dimmerWidget::Main(struct tag_message& message) {
    return widget::Main(message);
}

VA(0x004d3420, 0x13)
void dimmerWidget::Draw(void) {
    Dim();
}

VA(0x004d3470, 0x1c)
dimmerWidget::~dimmerWidget() {}

// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(dimmerWidget, 0x004eaa04)
