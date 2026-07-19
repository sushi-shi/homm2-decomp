#ifndef HOMM2_BASE_DROPLISTWIDGET_H
#define HOMM2_BASE_DROPLISTWIDGET_H
#include <va.h>
#include "widget.h"
struct tag_message;

// Frame layout of the drop-list ICN resource; the m_*Frame members are
// initialized from these in Read and the entry table is indexed with them.
H2_ENUM_CLASS_BEGIN(DropListFrame)
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
H2_ENUM_CLASS_END(DropListFrame)

#pragma pack(push, 1)
class bitmap;
class icon;
class font;
class dropListWidget : public widget {
public:
    font* m_font;
    icon* m_icon;
    i16 m_contentX;
    i16 m_contentY;
    i16 m_contentWidth;
    i16 m_contentHeight;
    i16 m_maxVisibleItems;
    i16 m_visibleItemCount;
    i16 m_normalColor;
    i16 m_selColor;
    i16 m_unusedColor;
    i16 m_textMode;
    i16 m_itemCount;
    i16 m_selectedIndex;
    char** m_items;
    i16 m_topIndex;
    i16 m_scrollRange;
    i16 m_closedContentFrame;
    i16 m_dropButtonFrame;
    i16 m_dropButtonPressedFrame;
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
    i16 m_dropButtonX;
    i16 m_dropButtonY;
    i16 m_dropButtonWidth;
    i16 m_dropButtonHeight;
    i16 m_iconX;
    i16 m_iconY;
    i16 m_closedContentWidth;
    i16 m_closedContentHeight;
    i16 m_firstRowHeight;
    i16 m_middleRowHeight;
    i16 m_lastRowHeight;
    i16 m_savedBackgroundX;
    i16 m_savedBackgroundY;
    i16 m_savedBackgroundWidth;
    i16 m_savedBackgroundHeight;
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
    bitmap* m_savedBackground;
    dropListWidget(void);
    virtual ~dropListWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Read(void);
    void DeleteItem(i32);
    void DrawDropStuff(void);
    void SaveDropBackground(void);
    void RestoreDropBackground(void);
    void ProcessSelectDialog(void);
};
#pragma pack(pop)
SIZE(dropListWidget, 0xb4);
#endif
