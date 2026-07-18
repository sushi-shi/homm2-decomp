#ifndef HOMM2_BASE_BASEMANAGER_H
#define HOMM2_BASE_BASEMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 1 methods, 3 own-virtual (all pure), 0 static data.
// Standalone abstract interface — NOT derived from widget. (??_7baseManager@@6B@ is 3
// all-__purecall slots [Open, Close, Main], and every derived manager has a 3-slot
// [Open,Close,Main] vtable with NO widget prefix — so it does not inherit widget's
// layout. The earlier `: public widget` was a header-gen mis-inference: two unrelated
// abstract classes share __purecall pointers by coincidence.) All 3 virtuals are
// bodyless-pure (no symbols); identities recovered from the manager overrides
// (resourceManager, inputManager, advManager, heroWindowManager, mouseManager,
// soundManager, combatManager, townManager, ...). No virtual destructor.
#include <va.h>
// forward declarations:
struct tag_message;

// Message classes a manager accepts: the bits are the tag_message type ids.
H2_ENUM_CLASS_BEGIN_T(BaseManagerMessageMask, i32)
    BASE_MANAGER_MESSAGE_MASK_ALL = -1,
    BASE_MANAGER_ACCEPT_MOUSE_MOVE = 4,
    BASE_MANAGER_ACCEPT_LEFT_BUTTON_UP = 0x10,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_DOWN = 0x20,
    BASE_MANAGER_ACCEPT_RIGHT_BUTTON_UP = 0x40,
    BASE_MANAGER_ACCEPT_RESOURCE = 0x80,
    BASE_MANAGER_ACCEPT_SWAP = 0x100,
    BASE_MANAGER_ACCEPT_WIDGET = 0x200,
    BASE_MANAGER_ACCEPT_ADVENTURE = 0x400,
    BASE_MANAGER_ACCEPT_TOWN_EVENT = 0x800,
    BASE_MANAGER_ACCEPT_EXECUTIVE = 0x4000
H2_ENUM_CLASS_END_T(BaseManagerMessageMask, i32)
H2_ENUM_FLAGS(BaseManagerMessageMask)

// Executive ordering keys stay arithmetic; the catalog names each manager.
H2_ENUM_BEGIN(BaseManagerPriority)
    BASE_MANAGER_PRIORITY_UNASSIGNED = -1,
    INPUT_MANAGER_PRIORITY = BASE_MANAGER_PRIORITY_UNASSIGNED,
    SOUND_MANAGER_PRIORITY = BASE_MANAGER_PRIORITY_UNASSIGNED
H2_ENUM_END(BaseManagerPriority)

H2_ENUM_BEGIN(BaseManagerConstant)
    BASE_MANAGER_NAME_CAPACITY = 0x1e
H2_ENUM_END(BaseManagerConstant)

#pragma pack(push, 1) // recovered layout is byte-packed
class baseManager /* abstract */ {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    baseManager* m_next;                     // +0x04  next in the executive manager list
    baseManager* m_prev;                     // +0x08  prev in the executive manager list
    BaseManagerMessageMask m_messageMask;                       // +0x0c  event/message classes accepted by Main
    BaseManagerPriority m_priority;                          // +0x10  executive manager-list ordering key
    char m_name[BASE_MANAGER_NAME_CAPACITY]; // +0x14  manager name
    b32 m_active;                            // +0x32
    // --- constructors ---
    baseManager(void);
    void Activate(void) {
        m_active = true;
    }
    // --- virtual methods (vtable order) ---
    virtual i32 Open(i32) = 0;
    virtual void Close(void) = 0;
    virtual i32 Main(struct tag_message&) = 0;
};
#pragma pack(pop)
SIZE(baseManager, 0x36);
#endif // HOMM2_BASE_BASEMANAGER_H
