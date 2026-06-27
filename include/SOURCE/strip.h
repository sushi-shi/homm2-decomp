#ifndef HOMM2_SOURCE_STRIP_H
#define HOMM2_SOURCE_STRIP_H
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 5 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class armyGroup;

class strip {
public:
    // --- constructors ---
    strip(int, int, int, unsigned long int, int, class armyGroup *, int, int, int);
    ~strip();
    // --- methods ---
    void Draw(void);
    void DrawIcons(int);
    void DrawFrame(void);
};
#endif // HOMM2_SOURCE_STRIP_H
