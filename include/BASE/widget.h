#ifndef HOMM2_BASE_WIDGET_H
#define HOMM2_BASE_WIDGET_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 7 methods, 3 own-virtual (all pure), 0 static data.
// Abstract root of the BASE UI-widget hierarchy. Verified from ??_7widget@@6B@: the
// vtable is 3 all-__purecall slots in order [Draw, ~widget, Main]. Draw is pure with
// NO body (emits no symbol); ~widget (??1widget@@UAE, 0x7) and Main (?Main@widget@@UAE,
// 0x2f4) are pure-virtual-WITH-body. Declaration order == vtable slot order; derived
// classes (border, iconWidget, textWidget, dimmerWidget, ...) override these 3 slots.
#include <va.h>
// forward declarations:
class heroWindow;
struct tag_message;

typedef enum WidgetFlag {
    WIDGET_FLAG_SELECTED = 1,
    WIDGET_FLAG_ENABLED = 2,
    WIDGET_FLAG_DRAW = 4,
    WIDGET_FLAG_DIMMED = 8,
    WIDGET_FLAG_GRAYED = 0x1000,
    WIDGET_FLAG_UPDATE = 0x4000
} WidgetFlag;

HOMM2_ENUM_BEGIN(BaseWidgetCommand)
WIDGET_COMMAND_DRAW = 2,
    WIDGET_COMMAND_SET_TEXT = 3, WIDGET_COMMAND_SET_FRAME = 4, WIDGET_COMMAND_SET_FLAGS = 5,
    WIDGET_COMMAND_CLEAR_FLAGS = 6, WIDGET_COMMAND_GET_TEXT = 7, WIDGET_COMMAND_SET_FILL_COLOR = 8,
    WIDGET_COMMAND_SET_ICON = 9, WIDGET_COMMAND_SET_MAX_LENGTH = 0x33, WIDGET_COMMAND_SET_X = 0x34,
    WIDGET_COMMAND_SET_Y = 0x35, WIDGET_COMMAND_SET_SELECTION = 0x36,
    WIDGET_COMMAND_GET_SELECTION = 0x37, WIDGET_COMMAND_APPEND_ITEM = 0x38,
    WIDGET_COMMAND_REPLACE_ITEM = 0x39, WIDGET_COMMAND_DELETE_ITEM = 0x3a,
    WIDGET_COMMAND_CLEAR_ITEMS = 0x3b, WIDGET_COMMAND_REPLACE_ICON = 0x3c,
    WIDGET_COMMAND_SET_WIDTH = 0x3d HOMM2_ENUM_END(BaseWidgetCommand)

        typedef enum WidgetCommandArgument {
    WIDGET_COMMAND_DIMMED = 0x1000
} WidgetCommandArgument;

#pragma pack(push, 1) // recovered layout is byte-packed
class widget /* abstract */ {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    heroWindow* m_owner; // +0x04  owning window
    widget* m_next;      // +0x08  next widget in the owner window's list
    widget* m_prev;      // +0x0c  prev widget in the owner window's list
    i16 m_id;            // +0x10
    i16 m_zOrder;        // +0x12
    i16 m_kind;          // +0x14  serialized widget rendering/behavior kind
    i16 m_flags;         // +0x16
    i16 m_x;             // +0x18
    i16 m_y;             // +0x1a
    i16 m_width;         // +0x1c
    i16 m_height;        // +0x1e
    // --- constructors ---
    widget(i16 x, i16 y, i16 width, i16 height, i16 id, i16 kind);
    widget(void);
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) = 0;
    virtual ~widget(void) = 0;
    virtual i32 Main(struct tag_message& message) = 0;
    // --- methods ---
    i32 Open(i32 zOrder, class heroWindow* owner);
    void Close(void);
    void Dim(void);
};
#pragma pack(pop)
SIZE(widget, 0x20);
#endif // HOMM2_BASE_WIDGET_H
