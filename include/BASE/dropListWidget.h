#ifndef HOMM2_BASE_DROPLISTWIDGET_H
#define HOMM2_BASE_DROPLISTWIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

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
    short  m_contentX;  // +0x28
    short  m_contentY;  // +0x2a
    short  m_contentWidth;  // +0x2c
    short  m_contentHeight;  // +0x2e
    short  m_maxVisibleItems;  // +0x30  configured maximum list rows
    short  m_visibleItemCount;  // +0x32  rows in the current drop-down
    short  m_normalColor;  // +0x34
    short  m_selColor;  // +0x36
    short  m_unusedColor;  // +0x38  serialized color value, retained but never read
    short  m_textMode;  // +0x3a
    short  m_itemCount;  // +0x3c
    short  m_selectedIndex;  // +0x3e
    char   **m_items;  // +0x40  item-string array
    short  m_topIndex;  // +0x44
    short  m_scrollRange;  // +0x46  maximum valid top-item index
    short  m_closedContentFrame;  // +0x48
    short  m_dropButtonFrame;  // +0x4a
    short  m_dropButtonPressedFrame;  // +0x4c
    short  m_firstRowFrame;  // +0x4e
    short  m_middleRowFrame;  // +0x50
    short  m_lastRowFrame;  // +0x52
    short  m_scrollUpFrame;  // +0x54
    short  m_scrollUpPressedFrame;  // +0x56
    short  m_scrollDownFrame;  // +0x58
    short  m_scrollDownPressedFrame;  // +0x5a
    short  m_scrollTrackFirstFrame;  // +0x5c
    short  m_scrollTrackMiddleFrame;  // +0x5e
    short  m_scrollTrackLastFrame;  // +0x60
    short  m_scrollThumbFrame;  // +0x62
    short  m_dropButtonX;  // +0x64
    short  m_dropButtonY;  // +0x66
    short  m_dropButtonWidth;  // +0x68
    short  m_dropButtonHeight;  // +0x6a
    short  m_iconX;  // +0x6c
    short  m_iconY;  // +0x6e
    short  m_closedContentWidth;  // +0x70
    short  m_closedContentHeight;  // +0x72
    short  m_firstRowHeight;  // +0x74
    short  m_middleRowHeight;  // +0x76
    short  m_lastRowHeight;  // +0x78
    short  m_savedBackgroundX;  // +0x7a
    short  m_savedBackgroundY;  // +0x7c
    short  m_savedBackgroundWidth;  // +0x7e
    short  m_savedBackgroundHeight;  // +0x80
    short  m_listX;  // +0x82
    short  m_listY;  // +0x84
    short  m_listWidth;  // +0x86
    short  m_listHeight;  // +0x88
    short  m_scrollUpX;  // +0x8a
    short  m_scrollUpY;  // +0x8c
    short  m_scrollUpWidth;  // +0x8e
    short  m_scrollUpHeight;  // +0x90
    short  m_scrollTrackX;  // +0x92
    short  m_scrollTrackY;  // +0x94
    short  m_scrollTrackWidth;  // +0x96
    short  m_scrollTrackHeight;  // +0x98
    short  m_scrollDownX;  // +0x9a
    short  m_scrollDownY;  // +0x9c
    short  m_scrollDownWidth;  // +0x9e
    short  m_scrollDownHeight;  // +0xa0
    short  m_scrollThumbX;  // +0xa2
    short  m_scrollThumbY;  // +0xa4
    short  m_scrollThumbWidth;  // +0xa6
    short  m_scrollThumbHeight;  // +0xa8
    short  m_scrollThumbTravel;  // +0xaa
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
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(int);
    void DrawDropStuff(void);
    void SaveDropBackground(void);
    void RestoreDropBackground(void);
    void ProcessSelectDialog(void);
};
#pragma pack(pop)
SIZE(dropListWidget, 0xb4);
#endif // HOMM2_BASE_DROPLISTWIDGET_H
