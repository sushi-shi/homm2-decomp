#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 2 own-virtual, 0 static data.
#include "../_macros.h"
#include "widget.h"
// forward declarations:
struct tag_message;

class textEntryWidget : public widget {
public:
    // --- constructors ---
    textEntryWidget(void);
    textEntryWidget(short int, short int, short int, short int, short int, char *, char *, short int, char *, short int, short int, short int, short int, int, int);
    virtual ~textEntryWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(int);
    void SetupDisplayString(char *, unsigned short int);
};
