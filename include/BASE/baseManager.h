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

#pragma pack(push, 1)  // recovered layout is byte-packed
class baseManager /* abstract */ {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    // (vptr auto-emitted at 0x00; own data starts at 0x04)
    baseManager *m_next;  // +0x04  next in the executive manager list
    baseManager *m_prev;  // +0x08  prev in the executive manager list
    int    m_messageMask;  // +0x0c  event/message classes accepted by Main
    int    m_priority;  // +0x10  executive manager-list ordering key
    char   m_name[0x1e];  // +0x14  manager name (strcpy'd, e.g. "Unknown")
    int    m_active;  // +0x32
    // --- constructors ---
    baseManager(void);
    void Activate(void) { m_active = 1; }
    // --- virtual methods (vtable order) ---
    virtual int Open(int) = 0;
    virtual void Close(void) = 0;
    virtual int Main(struct tag_message &) = 0;
};
#pragma pack(pop)
SIZE(baseManager, 0x36);
#endif // HOMM2_BASE_BASEMANAGER_H
