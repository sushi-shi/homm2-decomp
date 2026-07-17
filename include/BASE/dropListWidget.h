#ifndef HOMM2_BASE_DROPLISTWIDGET_H
#define HOMM2_BASE_DROPLISTWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

// Frame layout of the drop-list ICN resource; the m_*Frame members are
// initialized from these in Read and the entry table is indexed with them.
typedef enum DropListFrame {
    DROPLIST_FRAME_CLOSED_CONTENT = 0,
    DROPLIST_FRAME_DROP_BUTTON = 1,
    DROPLIST_FRAME_DROP_BUTTON_PRESSED = 2,
    DROPLIST_FRAME_FIRST_ROW = 3,
    DROPLIST_FRAME_MIDDLE_ROW = 4,
    DROPLIST_FRAME_LAST_ROW = 5,
    DROPLIST_FRAME_SCROLL_UP = 6,
    DROPLIST_FRAME_SCROLL_UP_PRESSED = 7,
    DROPLIST_FRAME_SCROLL_DOWN = 8,
    DROPLIST_FRAME_SCROLL_DOWN_PRESSED = 9,
    DROPLIST_FRAME_SCROLL_TRACK_FIRST = 10,
    DROPLIST_FRAME_SCROLL_TRACK_MIDDLE = 11,
    DROPLIST_FRAME_SCROLL_TRACK_LAST = 12,
    DROPLIST_FRAME_SCROLL_THUMB = 13
} DropListFrame;

#pragma pack(push, 1)  // recovered layout is byte-packed
class bitmap;
class icon;
class font;
class dropListWidget : public widget {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base widget = 0x20 bytes at 0x00 via ': public widget'; own fields below)
    font   *m_font;  // +0x20  the list font
    icon   *m_icon;  // +0x24  drop button icon
    i16  m_contentX;  // +0x28
    i16  m_contentY;  // +0x2a
    i16  m_contentWidth;  // +0x2c
    i16  m_contentHeight;  // +0x2e
    i16  m_maxVisibleItems;  // +0x30  configured maximum list rows
    i16  m_visibleItemCount;  // +0x32  rows in the current drop-down
    i16  m_normalColor;  // +0x34
    i16  m_selColor;  // +0x36
    i16  m_unusedColor;  // +0x38  serialized color value, retained but never read
    i16  m_textMode;  // +0x3a
    i16  m_itemCount;  // +0x3c
    i16  m_selectedIndex;  // +0x3e
    char   **m_items;  // +0x40  item-string array
    i16  m_topIndex;  // +0x44
    i16  m_scrollRange;  // +0x46  maximum valid top-item index
    i16  m_closedContentFrame;  // +0x48
    i16  m_dropButtonFrame;  // +0x4a
    i16  m_dropButtonPressedFrame;  // +0x4c
    i16  m_firstRowFrame;  // +0x4e
    i16  m_middleRowFrame;  // +0x50
    i16  m_lastRowFrame;  // +0x52
    i16  m_scrollUpFrame;  // +0x54
    i16  m_scrollUpPressedFrame;  // +0x56
    i16  m_scrollDownFrame;  // +0x58
    i16  m_scrollDownPressedFrame;  // +0x5a
    i16  m_scrollTrackFirstFrame;  // +0x5c
    i16  m_scrollTrackMiddleFrame;  // +0x5e
    i16  m_scrollTrackLastFrame;  // +0x60
    i16  m_scrollThumbFrame;  // +0x62
    i16  m_dropButtonX;  // +0x64
    i16  m_dropButtonY;  // +0x66
    i16  m_dropButtonWidth;  // +0x68
    i16  m_dropButtonHeight;  // +0x6a
    i16  m_iconX;  // +0x6c
    i16  m_iconY;  // +0x6e
    i16  m_closedContentWidth;  // +0x70
    i16  m_closedContentHeight;  // +0x72
    i16  m_firstRowHeight;  // +0x74
    i16  m_middleRowHeight;  // +0x76
    i16  m_lastRowHeight;  // +0x78
    i16  m_savedBackgroundX;  // +0x7a
    i16  m_savedBackgroundY;  // +0x7c
    i16  m_savedBackgroundWidth;  // +0x7e
    i16  m_savedBackgroundHeight;  // +0x80
    i16  m_listX;  // +0x82
    i16  m_listY;  // +0x84
    i16  m_listWidth;  // +0x86
    i16  m_listHeight;  // +0x88
    i16  m_scrollUpX;  // +0x8a
    i16  m_scrollUpY;  // +0x8c
    i16  m_scrollUpWidth;  // +0x8e
    i16  m_scrollUpHeight;  // +0x90
    i16  m_scrollTrackX;  // +0x92
    i16  m_scrollTrackY;  // +0x94
    i16  m_scrollTrackWidth;  // +0x96
    i16  m_scrollTrackHeight;  // +0x98
    i16  m_scrollDownX;  // +0x9a
    i16  m_scrollDownY;  // +0x9c
    i16  m_scrollDownWidth;  // +0x9e
    i16  m_scrollDownHeight;  // +0xa0
    i16  m_scrollThumbX;  // +0xa2
    i16  m_scrollThumbY;  // +0xa4
    i16  m_scrollThumbWidth;  // +0xa6
    i16  m_scrollThumbHeight;  // +0xa8
    i16  m_scrollThumbTravel;  // +0xaa
    char   m_scrollUpPressed;  // +0xac
    char   m_scrollDownPressed;  // +0xad
    char   m_scrollThumbDragging;  // +0xae
    char   m_itemSelectionTracking;  // +0xaf
    bitmap *m_savedBackground;  // +0xb0  saved drop-background bitmap
    // --- constructors ---
    dropListWidget(void);
    virtual ~dropListWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(i32);
    void DrawDropStuff(void);
    void SaveDropBackground(void);
    void RestoreDropBackground(void);
    void ProcessSelectDialog(void);
};
#pragma pack(pop)
SIZE(dropListWidget, 0xb4);
#endif // HOMM2_BASE_DROPLISTWIDGET_H
