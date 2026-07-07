#ifndef HOMM2_REQUEST_H
#define HOMM2_REQUEST_H
// Declarations of the free functions DEFINED in REQUEST.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

int GetMapHeader(char *, struct SMapHeader *);
int CheckSumIsDemoOK(char *);
int ShowThisMapGame(char *);
int ShowThisMap(char *);

#endif // HOMM2_REQUEST_H
