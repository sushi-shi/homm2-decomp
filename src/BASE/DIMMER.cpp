#include <va.h>
#include <BASE/widget.h>
#include <BASE/dimmerWidget.h>
#include <BASE/resourceManager.h>
#include <BASE/resourceGlobals.h>
template<class BaseWidget>
VA(0x004d3310, 0x2b)
heroWindow::DimmerWidget<BaseWidget>::DimmerWidget(void) : BaseWidget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}

template<class BaseWidget>
VA(0x004d3340, 0x3f)
heroWindow::DimmerWidget<BaseWidget>::DimmerWidget(
    i16 x,
    i16 y,
    i16 width,
    i16 height,
    i16 id,
    H2_ENUM_PARAM(WidgetKind, i16) kind
)
    : BaseWidget(x, y, width, height, id, kind) {}

template<class BaseWidget>
VA(0x004d3380, 0x77)
void heroWindow::DimmerWidget<BaseWidget>::Read(void) {
    READ_WIDGET_GEOMETRY(*this, gpResourceManager);
    this->m_id = gpResourceManager->ReadWord();
    this->m_kind = gpResourceManager->ReadWord();
}

template<class BaseWidget>
VA(0x004d3400, 0x19)
MessageDispatchResult heroWindow::DimmerWidget<BaseWidget>::Main(struct tag_message& message) {
    return BaseWidget::Main(message);
}

template<class BaseWidget>
VA(0x004d3420, 0x13)
void heroWindow::DimmerWidget<BaseWidget>::Draw(void) {
    BaseWidget::Dim();
}


// Compiler-emitted vtables; the markers are census claims, not definitions.
VTBL(dimmerWidget, 0x004eaa04)

template class heroWindow::DimmerWidget<widget>;
