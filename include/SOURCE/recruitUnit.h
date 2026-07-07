#ifndef HOMM2_SOURCE_RECRUITUNIT_H
#define HOMM2_SOURCE_RECRUITUNIT_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 3 own-virtual, 0 static data.
#include <va.h>
#include "../BASE/baseManager.h"
// forward declarations:
class armyGroup;
class town;
struct tag_message;

class recruitUnit : public baseManager {
public:
    // --- constructors ---
    recruitUnit(class armyGroup *, int, short int *);
    recruitUnit(class town *, int, int);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Update(void);
};
#endif // HOMM2_SOURCE_RECRUITUNIT_H
