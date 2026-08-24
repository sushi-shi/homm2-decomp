#ifndef HOMM2_BASE_TEXTENTRYWIDGET_H
#define HOMM2_BASE_TEXTENTRYWIDGET_H

#include <Ints.h>
#include <BASE/textWidget.h>
#include "widget.h"

struct tag_message;

enum {
    TEXT_ENTRY_READ_DEFAULT    = 1,
    TEXT_ENTRY_READ_RECT       = 2,
    TEXT_ENTRY_READ_MULTILINE  = 3,
    TEXT_ENTRY_READ_INSET_FIVE = 4,
    TEXT_ENTRY_READ_INSET_FOUR = 5
};
typedef i32 TextEntryReadMode;
enum {
    TEXT_ENTRY_LAYOUT_INSET = 4
};
typedef i32 TextEntryLayout;
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
        FontDrawMode color,
        char* iconName,
        i16 iconFrame,
        i16 id,
        WidgetKind kind,
        TextEntryLayout layout,
        i32 horizontalInset,
        i32 verticalInset
    );
    virtual ~textEntryWidget() override;
    virtual void Draw(void) override;
    virtual MessageDispatchResult Main(struct tag_message&) override;
    void Read(TextEntryReadMode);
    void SetupDisplayString(char*, u16);
};
#pragma pack(pop)
#endif
