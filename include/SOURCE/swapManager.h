#ifndef HOMM2_SOURCE_SWAPMANAGER_H
#define HOMM2_SOURCE_SWAPMANAGER_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class hero;
struct tag_message;

class swapManager : public baseManager {
public:
    // --- constructors ---
    swapManager(void);
    swapManager(class hero *, class hero *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Reset(void);
    int DrawSwapWin(void);
    void DrawSelector(void);
    void ViewMon(void);
    void SwapArtifacts(void);
    void SwapMons(void);
    void Update(void);
    void SplitMons(void);
};
#endif // HOMM2_SOURCE_SWAPMANAGER_H
