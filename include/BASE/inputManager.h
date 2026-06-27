#ifndef HOMM2_BASE_INPUTMANAGER_H
#define HOMM2_BASE_INPUTMANAGER_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 12 methods, 3 own-virtual, 0 static data.
#include "../_macros.h"
#include "baseManager.h"
// forward declarations:
struct tag_message;

class inputManager : public baseManager {
public:
    // --- constructors ---
    inputManager(void);
    // --- virtual methods (vtable order) ---
    virtual int Open(int) OVERRIDE;
    virtual void Close(void) OVERRIDE;
    virtual int Main(struct tag_message &) OVERRIDE;
    // --- methods ---
    void Flush(void);
    struct tag_message GetEvent(void);
    struct tag_message PeekEvent(void);
    void SetMouseCoords(int, int);
    void SetKeyCodeType(int);
    void AsciiConvert(struct tag_message &);
    void MakeScanCodeTable(void);
    void ForceMouseMove(void);
};
#endif // HOMM2_BASE_INPUTMANAGER_H
