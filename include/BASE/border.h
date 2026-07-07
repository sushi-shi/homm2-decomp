#ifndef HOMM2_BASE_BORDER_H
#define HOMM2_BASE_BORDER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 7 methods, 2 own-virtual, 0 static data.
#include <va.h>
#include "widget.h"
// forward declarations:
struct tag_message;

class border : public widget {
public:
    // --- constructors ---
    border(void);
    border(short int, short int, short int, short int, short int, short int, short int, char *);
    virtual ~border() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
};
#endif // HOMM2_BASE_BORDER_H
