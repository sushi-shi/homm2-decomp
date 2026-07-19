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
    border(i16, i16, i16, i16, i16, i16, i16, char*);
    virtual ~border() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
};
#pragma pack(pop)
SIZE(border, 0x2a);
#endif
