#ifndef HOMM2_X_ARENA_H
#define HOMM2_X_ARENA_H

#include <Ints.h>

struct tag_message;

i32 DoArenaDialog(void);
i32 ArenaWindowHandler(struct tag_message&);
void UpdateArenaIcons(void);

extern i32 choice;
extern class iconWidget* skillWidget[3];
extern class heroWindow* arenaWinPtr;

#endif
