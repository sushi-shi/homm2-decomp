#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <va.h>
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    dimmerWidget(void);
    dimmerWidget(i16, i16, i16, i16, i16, i16);
    virtual ~dimmerWidget() OVERRIDE {}
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
SIZE(dimmerWidget, 0x20);
#endif
