#ifndef HOMM2_BASE_INPUTMANAGER_H
#define HOMM2_BASE_INPUTMANAGER_H

#include <va.h>
#include "baseManager.h"
#include <BASE/message.h>

struct tag_message;

H2_ENUM_CLASS_BEGIN(InputManagerKeyCodeType)
    INPUT_KEY_CODE_ASCII = 0,
    INPUT_KEY_CODE_SCAN  = 1
H2_ENUM_CLASS_END(InputManagerKeyCodeType)

H2_ENUM_BEGIN(InputManagerScanCode)
    INPUT_SCAN_NONE            = 0x00,
    INPUT_SCAN_ESCAPE          = 0x01,
    INPUT_SCAN_1               = 0x02,
    INPUT_SCAN_2               = 0x03,
    INPUT_SCAN_3               = 0x04,
    INPUT_SCAN_4               = 0x05,
    INPUT_SCAN_5               = 0x06,
    INPUT_SCAN_6               = 0x07,
    INPUT_SCAN_7               = 0x08,
    INPUT_SCAN_8               = 0x09,
    INPUT_SCAN_9               = 0x0a,
    INPUT_SCAN_0               = 0x0b,
    INPUT_SCAN_MINUS           = 0x0c,
    INPUT_SCAN_EQUALS          = 0x0d,
    INPUT_SCAN_BACKSPACE       = 0x0e,
    INPUT_SCAN_TAB             = 0x0f,
    INPUT_SCAN_Q               = 0x10,
    INPUT_SCAN_W               = 0x11,
    INPUT_SCAN_E               = 0x12,
    INPUT_SCAN_R               = 0x13,
    INPUT_SCAN_T               = 0x14,
    INPUT_SCAN_Y               = 0x15,
    INPUT_SCAN_U               = 0x16,
    INPUT_SCAN_I               = 0x17,
    INPUT_SCAN_O               = 0x18,
    INPUT_SCAN_P               = 0x19,
    INPUT_SCAN_LEFT_BRACKET    = 0x1a,
    INPUT_SCAN_RIGHT_BRACKET   = 0x1b,
    INPUT_SCAN_ENTER           = 0x1c,
    INPUT_SCAN_CONTROL         = 0x1d,
    INPUT_SCAN_A               = 0x1e,
    INPUT_SCAN_S               = 0x1f,
    INPUT_SCAN_D               = 0x20,
    INPUT_SCAN_F               = 0x21,
    INPUT_SCAN_G               = 0x22,
    INPUT_SCAN_H               = 0x23,
    INPUT_SCAN_J               = 0x24,
    INPUT_SCAN_K               = 0x25,
    INPUT_SCAN_L               = 0x26,
    INPUT_SCAN_SEMICOLON       = 0x27,
    INPUT_SCAN_APOSTROPHE      = 0x28,
    INPUT_SCAN_GRAVE           = 0x29,
    INPUT_SCAN_LEFT_SHIFT      = 0x2a,
    INPUT_SCAN_BACKSLASH       = 0x2b,
    INPUT_SCAN_Z               = 0x2c,
    INPUT_SCAN_X               = 0x2d,
    INPUT_SCAN_C               = 0x2e,
    INPUT_SCAN_V               = 0x2f,
    INPUT_SCAN_B               = 0x30,
    INPUT_SCAN_N               = 0x31,
    INPUT_SCAN_M               = 0x32,
    INPUT_SCAN_COMMA           = 0x33,
    INPUT_SCAN_PERIOD          = 0x34,
    INPUT_SCAN_SLASH           = 0x35,
    INPUT_SCAN_RIGHT_SHIFT     = 0x36,
    INPUT_SCAN_NUMPAD_MULTIPLY = 0x37,
    INPUT_SCAN_ALT             = 0x38,
    INPUT_SCAN_SPACE           = 0x39,
    INPUT_SCAN_CAPS_LOCK       = 0x3a,
    INPUT_SCAN_F1              = 0x3b,
    INPUT_SCAN_F2              = 0x3c,
    INPUT_SCAN_F3              = 0x3d,
    INPUT_SCAN_F4              = 0x3e,
    INPUT_SCAN_F5              = 0x3f,
    INPUT_SCAN_F6              = 0x40,
    INPUT_SCAN_F7              = 0x41,
    INPUT_SCAN_F8              = 0x42,
    INPUT_SCAN_F9              = 0x43,
    INPUT_SCAN_F10             = 0x44,
    INPUT_SCAN_NUM_LOCK        = 0x45,
    INPUT_SCAN_SCROLL_LOCK     = 0x46,
    INPUT_SCAN_NUMPAD_7        = 0x47,
    INPUT_SCAN_NUMPAD_8        = 0x48,
    INPUT_SCAN_NUMPAD_9        = 0x49,
    INPUT_SCAN_NUMPAD_MINUS    = 0x4a,
    INPUT_SCAN_NUMPAD_4        = 0x4b,
    INPUT_SCAN_NUMPAD_5        = 0x4c,
    INPUT_SCAN_NUMPAD_6        = 0x4d,
    INPUT_SCAN_NUMPAD_PLUS     = 0x4e,
    INPUT_SCAN_NUMPAD_1        = 0x4f,
    INPUT_SCAN_NUMPAD_2        = 0x50,
    INPUT_SCAN_NUMPAD_3        = 0x51,
    INPUT_SCAN_NUMPAD_0        = 0x52,
    INPUT_SCAN_NUMPAD_DELETE   = 0x53,
    INPUT_SCAN_SYSREQ          = 0x54,
    INPUT_SCAN_RESERVED_55     = 0x55,
    INPUT_SCAN_ISO_BACKSLASH   = 0x56,
    INPUT_SCAN_F11             = 0x57,
    INPUT_SCAN_F12             = 0x58
H2_ENUM_END(InputManagerScanCode)

H2_ENUM_CLASS_BEGIN(InputManagerCapacity)
    INPUT_EVENT_RING_CAPACITY = 64,
    INPUT_SCAN_CODE_CAPACITY  = 128
H2_ENUM_CLASS_END(InputManagerCapacity)

#pragma pack(push, 1)
class inputManager : public baseManager {
public:
    tag_message m_eventRing[IDX(INPUT_EVENT_RING_CAPACITY)];
    i32 m_readIndex;
    i32 m_writeIndex;
    i32 m_mouseMessageActive;
    i32 field_0x742;
    i32 field_0x746;
    i32 field_0x74a;
    i16 m_keyState[IDX(INPUT_SCAN_CODE_CAPACITY)];
    i32 field_0x84e;
    i32 m_requestedPriority;
    InputManagerKeyCodeType m_keyCodeType;
    i32 m_field_0x85a;
    MessageModifier m_modifiers;
    i32 field_0x862;
    i32 field_0x866;
    inputManager(void);
    virtual i32 Open(i32) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual i32 Main(struct tag_message&) OVERRIDE;
    void Flush(void);
    struct tag_message GetEvent(void);
    struct tag_message PeekEvent(void);
    void SetMouseCoords(i32, i32);
    void SetKeyCodeType(i32);
    void AsciiConvert(struct tag_message&);
    void MakeScanCodeTable(void);
    void ForceMouseMove(void);
};
#pragma pack(pop)
SIZE(inputManager, 0x86a);
extern i32 iCurSwapPalette;
extern i32 bLastMouseOffscreen;
extern i32 bLastOnscreenMouseColor;
extern i32 bInCheckChangeCursor;
extern i32 iLastBWOnScreenCheck;

#endif
