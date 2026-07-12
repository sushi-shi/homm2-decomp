// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Viewwrld.obj   from: (directly linked into exe)
// functions: 5   data: 17
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/advManager.h>
#include <SOURCE/Viewwrld.h>
VA(0x004333c0, 0x267)
void advManager::ViewWorld(int, int, int) {}

VA(0x00433627, 0x5a)
void advManager::VWCleanup(void) {}

VA(0x00433681, 0x253)
void advManager::VWInit(int, int) {}

VA(0x004338d4, 0x1346)
void advManager::VWCompleteDraw(void) {}

VA(0x00434c1a, 0x5e2)
int ViewWorldDialogHandler(struct tag_message &) { return 0; }

// ---- globals (definitions, RVA order) ----
DATA(0x004f11b8) signed char iVWHalf[3][6][2];
DATA(0x004f11dc) int giViewWorldScale;
DATA(0x004f11e0) int giViewWorldScaleLookup;
DATA(0x004f11e4) int gbInViewWorld;
DATA(0x00525110) class icon *pVWMisc;
DATA(0x00525114) class icon *pVWLetters;
DATA(0x00525118) int iVWYPixelOffset;
DATA(0x0052511c) class icon *pVWGround;
DATA(0x00525120) int iVWViewableCells;
DATA(0x00525124) class icon *pVWFlags;
DATA(0x00525128) int iVWDrawAllTerrains;
DATA(0x0052512c) int iVWWhatToDraw;
DATA(0x00525130) int iVWDrawAllObjs;
DATA(0x00525134) int iVWMapOriginX;
DATA(0x00525138) int iVWMapOriginY;
DATA(0x0052513c) int iVWCenterOffset;
DATA(0x00525140) int iVWXPixelOffset;
