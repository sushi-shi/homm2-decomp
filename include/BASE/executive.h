#ifndef HOMM2_BASE_EXECUTIVE_H
#define HOMM2_BASE_EXECUTIVE_H
// Reconstructed class (BASE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 10 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class baseManager;

class executive {
public:
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
#endif // HOMM2_BASE_EXECUTIVE_H
