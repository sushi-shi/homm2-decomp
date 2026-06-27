#pragma once
// Reconstructed class (SOURCE) from CodeView NB09 of HEROES2W.EXE — NOT original source.
// 8 methods, 0 own-virtual, 0 static data.
#include "../_macros.h"

class hexcell {
public:
    // --- constructors ---
    hexcell(void);
    // --- methods ---
    void DrawGround(void);
    void DrawLowerDeadOccupants(void);
    void DrawUpperDeadOccupant(void);
    void DrawOccupant(int, int);
    void DrawTower(int);
    void DrawClouds(void);
    void DrawObstacle(void);
};
