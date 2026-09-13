#ifndef HOMM2_BASE_BORDER_H
#define HOMM2_BASE_BORDER_H

#include <Ints.h>
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
        WidgetKind kind,
        i16 fillColor,
        const char* name
    );
    virtual ~border() override;
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message& msg) override;
    void Read(void);
};
#pragma pack(pop)
#endif
