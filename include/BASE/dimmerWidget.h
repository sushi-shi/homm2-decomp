#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <BASE/heroWindow.h>
#include <BASE/widget.h>

#pragma pack(push, 1)
template<class BaseWidget>
class heroWindow::DimmerWidget : public BaseWidget {
public:
    DimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind);
    DimmerWidget(void);
    void Read(void);
    virtual MessageDispatchResult Main(tag_message& message) OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual ~DimmerWidget(void) OVERRIDE;
};
#pragma pack(pop)

typedef heroWindow::DimmerWidget<widget> dimmerWidget;
SIZE(dimmerWidget, 0x20);
#endif
