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

class baseManager /* abstract */ {
public:
    // --- constructors ---
    baseManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) = 0;
    virtual void Close(void) = 0;
    virtual int Main(struct tag_message &) = 0;
};
#endif // HOMM2_BASE_BASEMANAGER_H
