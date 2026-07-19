#ifndef HOMM2_BASE_DIMMERWIDGET_H
#define HOMM2_BASE_DIMMERWIDGET_H

#include <va.h>
#include "widget.h"

struct tag_message;

#pragma pack(push, 1)
class dimmerWidget : public widget {
public:
    dimmerWidget(void);
    dimmerWidget(i16 x, i16 y, i16 width, i16 height, i16 id, i16 kind);
    virtual ~dimmerWidget() OVERRIDE {}
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
SIZE(dimmerWidget, 0x20);
#endif
