#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <BASE/widget.h>

#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    dimmerWidget() : widget(0, 0, 0, 0, 0, WIDGET_KIND_NONE) {}
    dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind)
        : widget(x, y, width, height, id, kind) {}
    virtual ~dimmerWidget() OVERRIDE {}

    void Read(void);
    virtual MessageDispatchResult Main(tag_message& message) OVERRIDE {
        return widget::Main(message);
    }
    virtual void Draw(void) OVERRIDE { widget::Dim(); }
};
#pragma pack(pop)

SIZE(dimmerWidget, 0x20);
#endif
