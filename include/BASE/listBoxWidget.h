#ifndef HOMM2_BASE_LISTBOXWIDGET_H
#define HOMM2_BASE_LISTBOXWIDGET_H

#include <va.h>
#include <BASE/font.h>
#include "widget.h"

struct tag_message;

H2_ENUM_BEGIN(ListBoxSelectionClickCount)
    // A list's select notification carries its click count as the widget
    // message's parameter.
    SELECTION_SINGLE_CLICK = 1,
    SELECTION_DOUBLE_CLICK = 2
H2_ENUM_END(ListBoxSelectionClickCount)

H2_ENUM_BEGIN(ListBoxSelectedIndex)
    // The selected index of a list without a selection, as
    // WIDGET_COMMAND_GET_SELECTION reports it.
    LIST_BOX_NO_SELECTION = -1
H2_ENUM_END(ListBoxSelectedIndex)

H2_ENUM_BEGIN(ListBoxLayout)
    // The rows and scroll bar a list box draws, and a drop list's dropped
    // list: the first and last rows' extra edge, the text insets, and the
    // scroll thumb's insets, travel and drag offset.
    LIST_BOX_EDGE_ROW_COUNT              = 2,
    LIST_BOX_TEXT_LEFT_INSET             = 5,
    LIST_BOX_TEXT_HORIZONTAL_INSET_COUNT = 2,
    LIST_BOX_FIRST_ROW_TEXT_TOP_INSET    = 4,
    LIST_BOX_ROW_TEXT_TOP_INSET          = 2,
    LIST_BOX_SCROLL_TRACK_EDGE_ROW_COUNT = 2,
    LIST_BOX_SCROLL_THUMB_X_INSET        = 5,
    LIST_BOX_SCROLL_THUMB_Y_INSET        = 3,
    LIST_BOX_SCROLL_THUMB_TRAVEL_PADDING = 7,
    LIST_BOX_SCROLL_THUMB_CENTER_DIVISOR = 2,
    LIST_BOX_SCROLL_DRAG_Y_ADJUSTMENT    = 4
H2_ENUM_END(ListBoxLayout)

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
    H2_ENUM_STORAGE(FontDrawMode, i16) m_normalColor;
    H2_ENUM_STORAGE(FontDrawMode, i16) m_selectedColor;
    H2_ENUM_STORAGE(FontAlignment, i16) m_alignment;
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
    u8 m_scrollUpPressed;
    u8 m_scrollDownPressed;
    u8 m_scrollThumbDragging;
    u8 m_itemSelectionTracking;
    bitmap* m_scrollbar;
    listBoxWidget(void);
    virtual ~listBoxWidget() OVERRIDE;
    virtual void Draw(void) OVERRIDE;
    virtual MessageDispatchResult Main(struct tag_message& message) OVERRIDE;
    void Read(void);
    void DeleteItem(i32 index);
    void DrawLBStuff(i32 doUpdate);
    MessageDispatchResult ProcessMouseMessage(struct tag_message& message);
};
#pragma pack(pop)
SIZE(listBoxWidget, 0x92);
#endif
