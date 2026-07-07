#ifndef HOMM2_BASE_FONT_H
#define HOMM2_BASE_FONT_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 9 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"

class font {
public:
    // --- constructors ---
    font(unsigned long int);
    virtual ~font();
    // --- methods ---
protected:
    void DrawStringExecute(char *, int, int, int, int, int, int, int);   // ?...@font@@IAE... (protected)
public:
    void DrawString(char *, int, int, int);
    int GetCharacterWidth(unsigned char);
    void DrawBoundedString(char *, int, int, int, int, int, int);
    int LineLength(char *, int);
    int LineWidth(char *);
};
#endif // HOMM2_BASE_FONT_H
