#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 13 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"
// forward declarations:
class army;

class searchArray {
public:
    // --- constructors ---
    searchArray(void);
    ~searchArray();
    // --- methods ---
    int BuildPath(int, int, int, int, int);
    void SeedPosition(int, int, int, int, int, int, int, int, int, int, int, int);
    void Init(void);
    void Close(void);
    void Clear(void);
    int QuickDistance(int, int, int, int);
    void PushPoint(int, int, int, int, int, int, int, int, int, int, int, int);
    void TestPossibleDirections(int, int, signed char * const, signed char * const, int, int);
    void SeedCombatPosition(class army *);
    int FindCombatPath(int, int, class army *, int, int);
    void PushCombatPoint(int, int, int, int);
};
