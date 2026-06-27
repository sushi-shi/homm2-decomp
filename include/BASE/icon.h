#ifndef HOMM2_BASE_ICON_H
#define HOMM2_BASE_ICON_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
struct SLimitData;

class icon {
public:
    // --- constructors ---
    icon(unsigned long int);
    virtual ~icon();
    // --- methods ---
    void DrawToBuffer(int, int, int, int);
    int CombatClipDrawToBuffer(int, int, int, struct SLimitData *, int, int, unsigned char *, signed char *);
    void ClipFillToBuffer(int, int, int, int, int, int, int, int, int);
    void FillToBuffer(int, int, int, int, int, struct SLimitData *);
    void DimToBuffer(int, int, int, int);
};
#endif // HOMM2_BASE_ICON_H
