#ifndef HOMM2_X_ARENA_H
#define HOMM2_X_ARENA_H
// Declarations of the free functions DEFINED in x_arena.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

int DoArenaDialog(void);
int ArenaWindowHandler(struct tag_message &);
void UpdateArenaIcons(void);

#endif // HOMM2_X_ARENA_H
