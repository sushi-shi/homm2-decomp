#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include "../_macros.h"
#include "../BASE/baseManager.h"
// forward declarations:
struct tag_message;

class fileRequester : public baseManager {
public:
    // --- constructors ---
    fileRequester(int, int, int, char *, char *, char *);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    int InitializeFiles(char *, char *, int);
    int MapExistsForFilter(int);
    void SetupFiles(void);
    void CleanUpData(void);
    void SetOK(int);
    void DoKnob(void);
    void Update(int);
    char * GetFilename(void);
};
