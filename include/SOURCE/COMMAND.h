#ifndef HOMM2_COMMAND_H
#define HOMM2_COMMAND_H
// Declarations of the free functions DEFINED in COMMAND.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

int WinCombatHandler(struct tag_message &);
int InCombatArea(int, int);

#endif // HOMM2_COMMAND_H
