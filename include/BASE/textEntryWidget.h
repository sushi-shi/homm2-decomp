#ifndef HOMM2_BASE_TEXTENTRYWIDGET_H
#define HOMM2_BASE_TEXTENTRYWIDGET_H

#include <va.h>
#include <BASE/textWidget.h>
#include "widget.h"

struct tag_message;

H2_ENUM_BEGIN(TextEntryReadMode)
    TEXT_ENTRY_READ_DEFAULT    = 1,
    TEXT_ENTRY_READ_RECT       = 2,
    TEXT_ENTRY_READ_MULTILINE  = 3,
    TEXT_ENTRY_READ_INSET_FIVE = 4,
    TEXT_ENTRY_READ_INSET_FOUR = 5
H2_ENUM_END(TextEntryReadMode)

H2_ENUM_BEGIN(TextEntryLayout)
    TEXT_ENTRY_LAYOUT_INSET = 4
H2_ENUM_END(TextEntryLayout)

#pragma pack(push, 1)
class resource;
class icon;

class textEntryWidget : public textWidget {
public:
    icon* m_icon;
    i16 m_iconFrame;
    u16 m_cursorPosition;
    u16 m_maxLength;
    i16 m_rectX;
    i16 m_rectY;
    i16 m_rectW;
    i16 m_rectH;
    i16 m_innerW;
    i16 m_innerH;
    i16 m_innerX;
    i16 m_innerY;
    i16 m_maxLines;
    i16 m_preserveTextOnFocus;
    i16 m_entryType;
    i16 m_displayOffset;
    char m_cursorBlink;
    textEntryWidget(void);
    textEntryWidget(
        i16 x,
        i16 y,
        i16 width,
        i16 height,
        i16 maxLength,
        char* text,
        char* fontName,
        i16 color,
        char* iconName,
        i16 iconFrame,
        i16 id,
        i16 kind,
        i16 layout,
        i32 horizontalInset,
        i32 verticalInset
    );
    virtual ~textEntryWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(i32);
    void SetupDisplayString(char*, u16);
};
#pragma pack(pop)
SIZE(textEntryWidget, 0x4e);
#endif
