#ifndef HOMM2_SOURCE_X_ARENA_H
#define HOMM2_SOURCE_X_ARENA_H

#include <H2/Ints.h>
#include <BASE/message.h>

struct tag_message;

i32 DoArenaDialog(void);
MessageDispatchResult ArenaWindowHandler(struct tag_message& message);
void UpdateArenaIcons(void);

extern i32 choice;
extern class iconWidget* skillWidget[];
extern class heroWindow* arenaWinPtr;

#endif // HOMM2_SOURCE_X_ARENA_H
