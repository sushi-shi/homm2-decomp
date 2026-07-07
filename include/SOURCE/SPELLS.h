#ifndef HOMM2_SPELLS_H
#define HOMM2_SPELLS_H
// Declarations of the free functions DEFINED in SPELLS.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
struct tag_message;

int CombatSpecialHandler(struct tag_message &);
int HandleCastSpell(struct tag_message &);

#endif // HOMM2_SPELLS_H
