#ifndef HOMM2_ADVMGR_H
#define HOMM2_ADVMGR_H
// Declarations of the free functions DEFINED in ADVMGR.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):
class mapCell;
struct tag_message;

int SaveGame(void);
int DimensionDoorHandler(struct tag_message &);
int TownPortalHandler(struct tag_message &);
void ComputeAdvNetControl(void);
int MapExtraPosAndAdjacentsSet(int, int, unsigned char);
int APanelHandler(struct tag_message &);
int CPanelHandler(struct tag_message &);
void UpdateSystemOptions(int);
int SystemOptionsHandler(struct tag_message &);
int GetMobilityFrame(int);
int GetManaFrame(int);
unsigned char StopOnTrigger(class mapCell *);

#endif // HOMM2_ADVMGR_H
