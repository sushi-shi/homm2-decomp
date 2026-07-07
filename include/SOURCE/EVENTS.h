#ifndef HOMM2_EVENTS_H
#define HOMM2_EVENTS_H
// Declarations of the free functions DEFINED in EVENTS.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class hero;

signed char StrEqNoCase(char *, char *);
int GiveArtifact(class hero *, int, int, signed char);
void GiveTakeArtifactStat(class hero *, int, int);
int RiddleStringsEqual(char *, char *);

#endif // HOMM2_EVENTS_H
