#ifndef HOMM2_BASE_LISTBOXWIDGET_H
#define HOMM2_BASE_LISTBOXWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

// Frame layout of the list-box ICN resource; the m_*Frame members are
// initialized from these in Read and the entry table is indexed with them.
HOMM2_ENUM_BEGIN(ListBoxFrame)
    LISTBOX_FRAME_FIRST_ROW = 0,
    LISTBOX_FRAME_MIDDLE_ROW = 1,
    LISTBOX_FRAME_LAST_ROW = 2,
    LISTBOX_FRAME_SCROLL_UP = 3,
    LISTBOX_FRAME_SCROLL_UP_PRESSED = 4,
    LISTBOX_FRAME_SCROLL_DOWN = 5,
    LISTBOX_FRAME_SCROLL_DOWN_PRESSED = 6,
    LISTBOX_FRAME_SCROLL_TRACK_FIRST = 7,
    LISTBOX_FRAME_SCROLL_TRACK_MIDDLE = 8,
    LISTBOX_FRAME_SCROLL_TRACK_LAST = 9,
    LISTBOX_FRAME_SCROLL_THUMB = 10
HOMM2_ENUM_END(ListBoxFrame)

#pragma pack(push, 1) // recovered layout is byte-packed
class font;
class icon;
class bitmap;
class listBoxWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    font* m_font;                 // +0x20  the list font
    icon* m_icon;                 // +0x24  the list icon
    i16 m_maxVisibleItems;        // +0x28  configured number of list rows
    i16 m_visibleItemCount;       // +0x2a  populated rows currently drawn
    i16 m_normalColor;            // +0x2c
    i16 m_selectedColor;          // +0x2e
    i16 m_textMode;               // +0x30
    i16 m_itemCount;              // +0x32
    i16 m_selectedIndex;          // +0x34
    i16 m_lastSelectedIndex;      // +0x36  item selected by the previous click
    i32 m_lastClickTime;          // +0x38  tick count of the previous click
    char** m_items;               // +0x3c  item-string array
    i16 m_topIndex;               // +0x40
    i16 m_scrollRange;            // +0x42  maximum valid top-item index
    i16 m_firstRowFrame;          // +0x44
    i16 m_middleRowFrame;         // +0x46
    i16 m_lastRowFrame;           // +0x48
    i16 m_scrollUpFrame;          // +0x4a
    i16 m_scrollUpPressedFrame;   // +0x4c
    i16 m_scrollDownFrame;        // +0x4e
    i16 m_scrollDownPressedFrame; // +0x50
    i16 m_scrollTrackFirstFrame;  // +0x52
    i16 m_scrollTrackMiddleFrame; // +0x54
    i16 m_scrollTrackLastFrame;   // +0x56
    i16 m_scrollThumbFrame;       // +0x58
    i16 m_firstRowHeight;         // +0x5a  height of the list's first row frame
    i16 m_rowHeight;              // +0x5c  height of each middle list row
    i16 m_lastRowHeight;          // +0x5e  height of the list's last row frame
    i16 m_listX;                  // +0x60
    i16 m_listY;                  // +0x62
    i16 m_listWidth;              // +0x64
    i16 m_listHeight;             // +0x66
    i16 m_scrollUpX;              // +0x68
    i16 m_scrollUpY;              // +0x6a
    i16 m_scrollUpWidth;          // +0x6c
    i16 m_scrollUpHeight;         // +0x6e
    i16 m_scrollTrackX;           // +0x70
    i16 m_scrollTrackY;           // +0x72
    i16 m_scrollTrackWidth;       // +0x74
    i16 m_scrollTrackHeight;      // +0x76
    i16 m_scrollDownX;            // +0x78
    i16 m_scrollDownY;            // +0x7a
    i16 m_scrollDownWidth;        // +0x7c
    i16 m_scrollDownHeight;       // +0x7e
    i16 m_scrollThumbX;           // +0x80
    i16 m_scrollThumbY;           // +0x82
    i16 m_scrollThumbWidth;       // +0x84
    i16 m_scrollThumbHeight;      // +0x86
    i16 m_scrollThumbTravel;      // +0x88
    char m_scrollUpPressed;       // +0x8a
    char m_scrollDownPressed;     // +0x8b
    char m_scrollThumbDragging;   // +0x8c
    char m_itemSelectionTracking; // +0x8d
    bitmap* m_scrollbar;          // +0x8e  separately-owned bitmap resource
    // --- constructors ---
    listBoxWidget(void);
    virtual ~listBoxWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(i32);
    void DrawLBStuff(i32);
    i32 ProcessMouseMessage(struct tag_message&);
};
#pragma pack(pop)
SIZE(listBoxWidget, 0x92);
#endif // HOMM2_BASE_LISTBOXWIDGET_H
