#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 6 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"

class playerData {
public:
    // --- methods ---
    void Write(int);
    void Read(int);
    int NextHero(int);
    int HasMobileHero(void);
    int BuildingsOwned(int, int, int);
    int NumOfGivenArtifact(int);
};
