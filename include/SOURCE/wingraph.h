#ifndef HOMM2_WINGRAPH_H
#define HOMM2_WINGRAPH_H
// Declarations of the free functions DEFINED in wingraph.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
// forward declarations (was <_all.h>):

void DDRestoreDisplayMode(void);
int DDQueryNewPalette(void);
void CreatePrimary(void);
void SetupClipper(void);
void DDInitGraphics(void);
int DDAppPaint(void *, void *);
void DDInitializePalette(void);
int DDSetPalette(void);
struct IDirectDrawSurface * DDCreateSurface(unsigned long int, unsigned long int, int);
void DDSD(int, char *, int);
void DDUpdatePalette(signed char *);
void DDCleanUpWinGraphics(void);
void DDSetFullScreenStatus(int);
int WGQueryNewPalette(void);
void WGInitGraphics(void);
void WGUpdatePalette(signed char *);
void WGInitializePalette(void);
int WGAppPaint(void *, void *);
void WGCleanUpWinGraphics(void);
void ConnectToDLLs(void);
void DisconnectDLLs(void);
void RestoreDisplayMode(void);
int SetPalette(void);
void GetGraphicsInfo(void);
void InitGraphics(void);
int AppPaint(void *, void *);
void InitializePalette(void);
void UpdatePalette(signed char *);
void CleanUpWinGraphics(void);
void SetFullScreenStatus(int);
int QueryNewPalette(void);
int SetGraphicsType(int);

#endif // HOMM2_WINGRAPH_H
