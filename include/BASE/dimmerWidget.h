#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <Ints.h>
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    dimmerWidget(void);
    dimmerWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        i16 id,
        WidgetKind kind
    );
    virtual ~dimmerWidget() override {}
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void Read(void);
};
#pragma pack(pop)
#endif
