#ifndef HOMM2_X_ARENA_H
#define HOMM2_X_ARENA_H
// Declarations of the free functions DEFINED in x_arena.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

int DoArenaDialog(void);
int ArenaWindowHandler(struct tag_message &);
void UpdateArenaIcons(void);

// ---- globals (declarations, RVA order) ----
DATA(0x005331a8) extern int choice;
DATA(0x005331b0) extern class iconWidget *skillWidget[3];
DATA(0x005331bc) extern class heroWindow *arenaWinPtr;

#endif // HOMM2_X_ARENA_H
