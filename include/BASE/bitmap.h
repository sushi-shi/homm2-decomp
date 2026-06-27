#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"

class bitmap {
public:
    // --- constructors ---
    bitmap(void);
    bitmap(short int, short int, short int);
    bitmap(unsigned long int);
    virtual ~bitmap();
    // --- methods ---
    void DrawToBufferCareful(short int, short int);
    void DrawToBuffer(short int, short int);
    void DrawToScreen(short int, short int);
    void GrabScreen(short int, short int);
    void GrabBitmap(class bitmap *, short int, short int);
    void GrabBitmapCareful(class bitmap *, short int, short int);
    void CopyTo(class bitmap *, int, int, int, int, int, int);
    void CopyToCareful(class bitmap *, int, int, int, int, int, int);
};
