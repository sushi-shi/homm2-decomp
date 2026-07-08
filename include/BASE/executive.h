#ifndef HOMM2_BASE_EXECUTIVE_H
#define HOMM2_BASE_EXECUTIVE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 10 methods, 0 own-virtual, 0 static data.
#include <va.h>
// forward declarations:
class baseManager;

#pragma pack(push, 1)  // recovered layout is byte-packed
class executive {
public:
    // --- members (offsets from Ghidra this+off access-analysis; widths are
    // access-widths, NOT confirmed types; refine during byte-matching) ---
    baseManager *field_0x0;  // +0x00  manager-list head
    baseManager *field_0x4;  // +0x04  manager-list tail
    baseManager *field_0x8;  // +0x08  the current/active manager
    int    field_0xc;  // +0x0c
    // --- constructors ---
    executive(void);
    // --- methods ---
    int InitSystem(void);
    void ShutDownSystem(void);
    int DoDialog(class baseManager *);
    void PrintManagerList(void);
    int AddManager(class baseManager *, int);
    void RemoveManager(class baseManager *);
    void CallManager(class baseManager *);
    void MainLoop(void);
    void Terminate(void);
};
#pragma pack(pop)
SIZE(executive, 0x10);
#endif // HOMM2_BASE_EXECUTIVE_H
