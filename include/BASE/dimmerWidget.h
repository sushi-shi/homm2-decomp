#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <BASE/widget.h>

#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id,
        H2_ENUM_PARAM(WidgetKind, i16) kind);
    dimmerWidget(void);
    void Read(void);
    virtual MessageDispatchResult Main(tag_message& message) OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual ~dimmerWidget(void) OVERRIDE;
};
#pragma pack(pop)

SIZE(dimmerWidget, 0x20);
#endif
