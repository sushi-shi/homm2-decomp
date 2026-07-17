#ifndef HOMM2_BASE_INPUTMANAGER_H
#define HOMM2_BASE_INPUTMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "baseManager.h"
#include <BASE/message.h>
// forward declarations:
struct tag_message;

typedef enum InputManagerKeyCodeType {
    INPUT_KEY_CODE_ASCII = 0,
    INPUT_KEY_CODE_SCAN = 1
} InputManagerKeyCodeType;

// IBM PC set-1 scan codes carried in bits 16..23 of the Win32 key-message data.
// MakeScanCodeTable proves the complete 0x00..0x58 domain used by the retail input
// manager, including the extended key values returned as scan-code << 8.
typedef enum InputManagerScanCode {
    INPUT_SCAN_NONE = 0x00,
    INPUT_SCAN_ESCAPE = 0x01,
    INPUT_SCAN_1 = 0x02,
    INPUT_SCAN_2 = 0x03,
    INPUT_SCAN_3 = 0x04,
    INPUT_SCAN_4 = 0x05,
    INPUT_SCAN_5 = 0x06,
    INPUT_SCAN_6 = 0x07,
    INPUT_SCAN_7 = 0x08,
    INPUT_SCAN_8 = 0x09,
    INPUT_SCAN_9 = 0x0a,
    INPUT_SCAN_0 = 0x0b,
    INPUT_SCAN_MINUS = 0x0c,
    INPUT_SCAN_EQUALS = 0x0d,
    INPUT_SCAN_BACKSPACE = 0x0e,
    INPUT_SCAN_TAB = 0x0f,
    INPUT_SCAN_Q = 0x10,
    INPUT_SCAN_W = 0x11,
    INPUT_SCAN_E = 0x12,
    INPUT_SCAN_R = 0x13,
    INPUT_SCAN_T = 0x14,
    INPUT_SCAN_Y = 0x15,
    INPUT_SCAN_U = 0x16,
    INPUT_SCAN_I = 0x17,
    INPUT_SCAN_O = 0x18,
    INPUT_SCAN_P = 0x19,
    INPUT_SCAN_LEFT_BRACKET = 0x1a,
    INPUT_SCAN_RIGHT_BRACKET = 0x1b,
    INPUT_SCAN_ENTER = 0x1c,
    INPUT_SCAN_CONTROL = 0x1d,
    INPUT_SCAN_A = 0x1e,
    INPUT_SCAN_S = 0x1f,
    INPUT_SCAN_D = 0x20,
    INPUT_SCAN_F = 0x21,
    INPUT_SCAN_G = 0x22,
    INPUT_SCAN_H = 0x23,
    INPUT_SCAN_J = 0x24,
    INPUT_SCAN_K = 0x25,
    INPUT_SCAN_L = 0x26,
    INPUT_SCAN_SEMICOLON = 0x27,
    INPUT_SCAN_APOSTROPHE = 0x28,
    INPUT_SCAN_GRAVE = 0x29,
    INPUT_SCAN_LEFT_SHIFT = 0x2a,
    INPUT_SCAN_BACKSLASH = 0x2b,
    INPUT_SCAN_Z = 0x2c,
    INPUT_SCAN_X = 0x2d,
    INPUT_SCAN_C = 0x2e,
    INPUT_SCAN_V = 0x2f,
    INPUT_SCAN_B = 0x30,
    INPUT_SCAN_N = 0x31,
    INPUT_SCAN_M = 0x32,
    INPUT_SCAN_COMMA = 0x33,
    INPUT_SCAN_PERIOD = 0x34,
    INPUT_SCAN_SLASH = 0x35,
    INPUT_SCAN_RIGHT_SHIFT = 0x36,
    INPUT_SCAN_NUMPAD_MULTIPLY = 0x37,
    INPUT_SCAN_ALT = 0x38,
    INPUT_SCAN_SPACE = 0x39,
    INPUT_SCAN_CAPS_LOCK = 0x3a,
    INPUT_SCAN_F1 = 0x3b,
    INPUT_SCAN_F2 = 0x3c,
    INPUT_SCAN_F3 = 0x3d,
    INPUT_SCAN_F4 = 0x3e,
    INPUT_SCAN_F5 = 0x3f,
    INPUT_SCAN_F6 = 0x40,
    INPUT_SCAN_F7 = 0x41,
    INPUT_SCAN_F8 = 0x42,
    INPUT_SCAN_F9 = 0x43,
    INPUT_SCAN_F10 = 0x44,
    INPUT_SCAN_NUM_LOCK = 0x45,
    INPUT_SCAN_SCROLL_LOCK = 0x46,
    INPUT_SCAN_NUMPAD_7 = 0x47,
    INPUT_SCAN_NUMPAD_8 = 0x48,
    INPUT_SCAN_NUMPAD_9 = 0x49,
    INPUT_SCAN_NUMPAD_MINUS = 0x4a,
    INPUT_SCAN_NUMPAD_4 = 0x4b,
    INPUT_SCAN_NUMPAD_5 = 0x4c,
    INPUT_SCAN_NUMPAD_6 = 0x4d,
    INPUT_SCAN_NUMPAD_PLUS = 0x4e,
    INPUT_SCAN_NUMPAD_1 = 0x4f,
    INPUT_SCAN_NUMPAD_2 = 0x50,
    INPUT_SCAN_NUMPAD_3 = 0x51,
    INPUT_SCAN_NUMPAD_0 = 0x52,
    INPUT_SCAN_NUMPAD_DELETE = 0x53,
    INPUT_SCAN_SYSREQ = 0x54,
    INPUT_SCAN_RESERVED_55 = 0x55,
    INPUT_SCAN_ISO_BACKSLASH = 0x56,
    INPUT_SCAN_F11 = 0x57,
    INPUT_SCAN_F12 = 0x58
} InputManagerScanCode;

typedef enum InputManagerCapacity {
    INPUT_EVENT_RING_CAPACITY = 64,
    INPUT_SCAN_CODE_CAPACITY = 128
} InputManagerCapacity;

typedef enum InputManagerScanCodeEncoding {
    INPUT_SCAN_CODE_MASK = 0xff
} InputManagerScanCodeEncoding;

typedef enum InputManagerCursorBounds {
    INPUT_CURSOR_INTERIOR_MIN_EXCLUSIVE = 3,
    INPUT_CURSOR_INTERIOR_MAX_X_EXCLUSIVE = 636,
    INPUT_CURSOR_INTERIOR_MAX_Y_EXCLUSIVE = 476
} InputManagerCursorBounds;

typedef enum InputManagerTiming {
    INPUT_CURSOR_CHECK_DELAY = 500
} InputManagerTiming;

typedef enum InputManagerModifierClearMask {
    INPUT_CLEAR_CONTROL_MASK = 0xfffb,
    INPUT_CLEAR_LEFT_SHIFT_MASK = 0xfffd,
    INPUT_CLEAR_RIGHT_SHIFT_MASK = 0xfffe,
    INPUT_CLEAR_ALT_MASK = 0xffdf
} InputManagerModifierClearMask;

#pragma pack(push, 1)  // recovered layout is byte-packed
class inputManager : public baseManager {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (derived: base baseManager = 0x36 bytes at 0x00 via ': public baseManager'; own fields below)
    tag_message m_eventRing[INPUT_EVENT_RING_CAPACITY];  // +0x36  event ring (64 x 0x1c)
    int    m_readIndex;  // +0x736
    int    m_writeIndex;  // +0x73a
    int    m_mouseMessageActive;  // +0x73e  reentrancy guard shared by mouse event producers
    int    field_0x742;  // +0x742
    int    field_0x746;  // +0x746
    int    field_0x74a;  // +0x74a
    short  m_keyState[INPUT_SCAN_CODE_CAPACITY];  // +0x74e  scan-code to ASCII/extended-key table
    int    field_0x84e;  // +0x84e
    int    field_0x852;  // +0x852
    InputManagerKeyCodeType m_keyCodeType;  // +0x856  zero converts scan codes to ASCII on dequeue
    int    m_field_0x85a;  // +0x85a
    int    m_modifiers;  // +0x85e  current keyboard modifier mask
    int    field_0x862;  // +0x862
    int    field_0x866;  // +0x866
    // --- constructors ---
    inputManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Flush(void);
    struct tag_message GetEvent(void);
    struct tag_message PeekEvent(void);
    void SetMouseCoords(int, int);
    void SetKeyCodeType(int);
    void AsciiConvert(struct tag_message &);
    void MakeScanCodeTable(void);
    void ForceMouseMove(void);
};
#pragma pack(pop)
SIZE(inputManager, 0x86a);
// ---- globals (declarations, RVA order) ----
extern int iCurSwapPalette;
extern int bLastMouseOffscreen;
extern int bLastOnscreenMouseColor;
extern int bInCheckChangeCursor;
extern int iLastBWOnScreenCheck;

#endif // HOMM2_BASE_INPUTMANAGER_H
