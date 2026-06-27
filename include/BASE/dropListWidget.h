#pragma once
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 11 methods, 2 own-virtual, 0 static data.
#include "../_macros.h"
#include "widget.h"
// forward declarations:
struct tag_message;

class dropListWidget : public widget {
public:
    // --- constructors ---
    dropListWidget(void);
    virtual ~dropListWidget() OVERRIDE;
    // --- virtual methods (vtable order) ---
    virtual void Draw(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Read(void);
    void DeleteItem(int);
    void DrawDropStuff(void);
    void SaveDropBackground(void);
    void RestoreDropBackground(void);
    void ProcessSelectDialog(void);
};
