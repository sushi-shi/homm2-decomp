// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\CURSOR.OBJ   from: (directly linked into exe)
// functions: 18   data: 13
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/advManager.h>
#include <SOURCE/CURSOR.h>
#include <_carcass_types.h>
VA(0x0040d5e0, 0x138)
void advManager::StartCursor(int) {}

VA(0x0040d718, 0x11c)
void advManager::StopCursor(int) {}

VA(0x0040d834, 0x5f8)
void advManager::DrawCursor(void) {}

VA(0x0040de2c, 0x36c)
void advManager::DrawCursorShadow(void) {}

VA(0x0040e198, 0x85)
int advManager::GetCursorBaseFrame(int) { return 0; }

VA(0x0040e21d, 0x256)
void advManager::TurnTo(int) {}

VA(0x0040e473, 0xac)
int advManager::GetMoveShowIt(class hero *, int) { return 0; }

VA(0x0040e51f, 0x1234)
class mapCell * advManager::MoveHero(int, int, int *, int *, int *, int, int *, int) { return 0; }

VA(0x0040f753, 0x174)
void advManager::CheckAdjacentMon(int *) {}

VA(0x0040f8c7, 0x14e)
int advManager::ValidMoveWithEvent(class hero *, int) { return 0; }

VA(0x0040fa15, 0x4f2)
int advManager::ValidMove(int, int) { return 0; }

VA(0x0040ff07, 0x24b)
void advManager::MoveOrigin(int, int) {}

VA(0x00410152, 0x74f)
void advManager::ProcessMapChange(struct SMapChange) {}

VA(0x004108a1, 0x1ba)
void advManager::ProcessIncomingSingleMapChange(struct SMapChange *) {}

VA(0x00410a5b, 0xce)
void advManager::ProcessIncomingGroupMapChange(char *) {}

VA(0x00410b29, 0x75)
void advManager::PurgeMapChangeQueue(void) {}

VA(0x00410b9e, 0x1d4)
void advManager::UnwindMapChangeQueue(int, int) {}

VA(0x00410d72, 0x11a)
void SendMapChange(int, signed char, unsigned char, unsigned char, int, unsigned char, unsigned char) {}

// ---- data / globals / vtables ----
DATA(0x004ee020)  // int bMoveSoundMade
DATA(0x004ee028)  // int * giPixelsPerStep
DATA(0x004ee040)  // int * giStepDelay
DATA(0x004ee054)  // struct _SAMPLE * hOldWalkSample
DATA(0x004ee058)  // struct _SAMPLE * hNewWalkSample
DATA(0x004ee05c)  // int EveryOther
DATA(0x004ee060)  // int * startVals
DATA(0x00524bc0)  // int S1cursorCycle
DATA(0x00524bc4)  // int S1cursorFrameCount
DATA(0x00524bc8)  // int S1cursorTurning
DATA(0x00524bcc)  // int S1cursorBaseFrame
DATA(0x00524bd0)  // int S1cursorDirection
DATA(0x00524bd8)  // struct SMapChange * sMapChangeLastFew
