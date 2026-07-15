#ifndef HOMM2_BASE_LISTBOXWIDGET_H
#define HOMM2_BASE_LISTBOXWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

#pragma pack(push, 1)  // recovered layout is byte-packed
class font;
class icon;
class bitmap;
class listBoxWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    font   *m_font;  // +0x20  the list font
    icon   *m_icon;  // +0x24  the list icon
    short  m_maxVisibleItems;  // +0x28  configured number of list rows
    short  m_visibleItemCount;  // +0x2a  populated rows currently drawn
    short  m_normalColor;  // +0x2c
    short  m_selectedColor;  // +0x2e
    short  m_textMode;  // +0x30
    short  m_itemCount;  // +0x32
    short  m_selectedIndex;  // +0x34
    short  m_lastSelectedIndex;  // +0x36  item selected by the previous click
    int    m_lastClickTime;  // +0x38  tick count of the previous click
    char   **m_items;  // +0x3c  item-string array
    short  m_topIndex;  // +0x40
    short  m_scrollRange;  // +0x42  maximum valid top-item index
    short  m_firstRowFrame;  // +0x44
    short  m_middleRowFrame;  // +0x46
    short  m_lastRowFrame;  // +0x48
    short  m_scrollUpFrame;  // +0x4a
    short  m_scrollUpPressedFrame;  // +0x4c
    short  m_scrollDownFrame;  // +0x4e
    short  m_scrollDownPressedFrame;  // +0x50
    short  m_scrollTrackFirstFrame;  // +0x52
    short  m_scrollTrackMiddleFrame;  // +0x54
    short  m_scrollTrackLastFrame;  // +0x56
    short  m_scrollThumbFrame;  // +0x58
    short  m_firstRowHeight;  // +0x5a  height of the list's first row frame
    short  m_rowHeight;  // +0x5c  height of each middle list row
    short  m_lastRowHeight;  // +0x5e  height of the list's last row frame
    short  m_listX;  // +0x60
    short  m_listY;  // +0x62
    short  m_listWidth;  // +0x64
    short  m_listHeight;  // +0x66
    short  m_scrollUpX;  // +0x68
    short  m_scrollUpY;  // +0x6a
    short  m_scrollUpWidth;  // +0x6c
    short  m_scrollUpHeight;  // +0x6e
    short  m_scrollTrackX;  // +0x70
    short  m_scrollTrackY;  // +0x72
    short  m_scrollTrackWidth;  // +0x74
    short  m_scrollTrackHeight;  // +0x76
    short  m_scrollDownX;  // +0x78
    short  m_scrollDownY;  // +0x7a
    short  m_scrollDownWidth;  // +0x7c
    short  m_scrollDownHeight;  // +0x7e
    short  m_scrollThumbX;  // +0x80
    short  m_scrollThumbY;  // +0x82
    short  m_scrollThumbWidth;  // +0x84
    short  m_scrollThumbHeight;  // +0x86
    short  m_scrollThumbTravel;  // +0x88
    char   m_scrollUpPressed;  // +0x8a
    char   m_scrollDownPressed;  // +0x8b
    char   m_scrollThumbDragging;  // +0x8c
    char   m_itemSelectionTracking;  // +0x8d
    bitmap *m_scrollbar;  // +0x8e  separately-owned bitmap resource
    // --- constructors ---
    listBoxWidget(void);
    virtual ~listBoxWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(int);
    void DrawLBStuff(int);
    int ProcessMouseMessage(struct tag_message &);
};
#pragma pack(pop)
SIZE(listBoxWidget, 0x92);
#endif // HOMM2_BASE_LISTBOXWIDGET_H
