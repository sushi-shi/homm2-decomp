#ifndef HOMM2_BASE_BORDER_H
#define HOMM2_BASE_BORDER_H

#include <va.h>
#include "widget.h"

class bitmap;
class icon;
struct tag_message;

#pragma pack(push, 1)
class border : public widget {
public:
    bitmap* m_backgroundBitmap;
    icon* m_backgroundIcon;
    i16 m_fillColor;
    border(void);
    border(
        i16 x,
        i16 y,
        i16 w,
        i16 h,
        i16 e,
        H2_ENUM_PARAM(WidgetKind, i16) kind,
        i16 fillColor,
        H2_CONST char* name
    );
    virtual H2_RETAIL_INLINE ~border() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
SIZE(border, 0x2a);
#endif
