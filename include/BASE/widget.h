#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 7 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class heroWindow;
struct tag_message;

class widget /* abstract */ {
public:
    // --- constructors ---
    widget(short int, short int, short int, short int, short int, short int);
    widget(void);
    ~widget();
    // --- methods ---
    int Open(int, class heroWindow *);
    void Close(void);
    int Main(struct tag_message &);
    void Dim(void);
};
