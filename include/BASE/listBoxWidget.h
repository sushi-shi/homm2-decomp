#ifndef HOMM2_BASE_LISTBOXWIDGET_H
#define HOMM2_BASE_LISTBOXWIDGET_H
#include <va.h>
#include "widget.h"
struct tag_message;

// Frame layout of the list-box ICN resource; the m_*Frame members are
// initialized from these in Read and the entry table is indexed with them.
H2_ENUM_BEGIN(ListBoxFrame)
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
H2_ENUM_END(ListBoxFrame)

#pragma pack(push, 1)
class font;
class icon;
class bitmap;
class listBoxWidget : public widget {
public:
    font* m_font;
    icon* m_icon;
    i16 m_maxVisibleItems;
    i16 m_visibleItemCount;
    i16 m_normalColor;
    i16 m_selectedColor;
    i16 m_textMode;
    i16 m_itemCount;
    i16 m_selectedIndex;
    i16 m_lastSelectedIndex;
    i32 m_lastClickTime;
    char** m_items;
    i16 m_topIndex;
    i16 m_scrollRange;
    i16 m_firstRowFrame;
    i16 m_middleRowFrame;
    i16 m_lastRowFrame;
    i16 m_scrollUpFrame;
    i16 m_scrollUpPressedFrame;
    i16 m_scrollDownFrame;
    i16 m_scrollDownPressedFrame;
    i16 m_scrollTrackFirstFrame;
    i16 m_scrollTrackMiddleFrame;
    i16 m_scrollTrackLastFrame;
    i16 m_scrollThumbFrame;
    i16 m_firstRowHeight;
    i16 m_rowHeight;
    i16 m_lastRowHeight;
    i16 m_listX;
    i16 m_listY;
    i16 m_listWidth;
    i16 m_listHeight;
    i16 m_scrollUpX;
    i16 m_scrollUpY;
    i16 m_scrollUpWidth;
    i16 m_scrollUpHeight;
    i16 m_scrollTrackX;
    i16 m_scrollTrackY;
    i16 m_scrollTrackWidth;
    i16 m_scrollTrackHeight;
    i16 m_scrollDownX;
    i16 m_scrollDownY;
    i16 m_scrollDownWidth;
    i16 m_scrollDownHeight;
    i16 m_scrollThumbX;
    i16 m_scrollThumbY;
    i16 m_scrollThumbWidth;
    i16 m_scrollThumbHeight;
    i16 m_scrollThumbTravel;
    char m_scrollUpPressed;
    char m_scrollDownPressed;
    char m_scrollThumbDragging;
    char m_itemSelectionTracking;
    bitmap* m_scrollbar;
    listBoxWidget(void);
    virtual ~listBoxWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
    void DeleteItem(i32);
    void DrawLBStuff(i32);
    i32 ProcessMouseMessage(struct tag_message&);
};
#pragma pack(pop)
SIZE(listBoxWidget, 0x92);
#endif
