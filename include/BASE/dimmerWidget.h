#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 2 own-virtual, 0 static data.
#include "../_macros.h"
#include "widget.h"
// forward declarations:
struct tag_message;

class dimmerWidget : public widget {
public:
    // --- constructors ---
    dimmerWidget(void);
    dimmerWidget(short int, short int, short int, short int, short int, short int);
    virtual ~dimmerWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
};
