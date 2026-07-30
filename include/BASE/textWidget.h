#ifndef HOMM2_BASE_TEXTWIDGET_H
#define HOMM2_BASE_TEXTWIDGET_H

#include <Ints.h>
#include <BASE/font.h>
#include "widget.h"

struct tag_message;

class resource;
class font;

#pragma pack(push, 1)
class textWidget : public widget {
public:
    char* m_text;
    font* m_font;
    H2EnumStorage<FontDrawMode, i16> m_color;
    H2EnumStorage<FontAlignment, char> m_alignment;
    textWidget(void);
    textWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        char* text,
        char* fontName,
        FontDrawMode color,
        i16 id,
        WidgetKind kind,
        FontAlignment alignment
    );
    virtual ~textWidget() override;
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void Read(void);
    void SetColorIndex(FontDrawMode color);
    void SetText(char* text);
};
#pragma pack(pop)
#endif
