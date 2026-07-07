#ifndef HOMM2_SOURCE_BANKBOX_H
#define HOMM2_SOURCE_BANKBOX_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 3 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class playerData;

class bankBox {
public:
    // --- constructors ---
    bankBox(int, int, class playerData *);
    ~bankBox();
    // --- methods ---
    void Update(int);
};
#endif // HOMM2_SOURCE_BANKBOX_H
