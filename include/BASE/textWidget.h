#ifndef HOMM2_BASE_TEXTWIDGET_H
#define HOMM2_BASE_TEXTWIDGET_H

#include <va.h>
#include "widget.h"

struct tag_message;

class resource;
class font;

#pragma pack(push, 1)
class textWidget : public widget {
public:
    char* m_text;
    font* m_font;
    i16 m_color;
    char m_alignment;
    textWidget(void);
    textWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* text,
        char* fontName,
        i16 color,
        i16 id,
        i16 kind,
        i16 alignment
    );
    virtual ~textWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
    void SetColorIndex(i16 color);
    void SetText(char* text);
};
#pragma pack(pop)
SIZE(textWidget, 0x2b);
#endif
