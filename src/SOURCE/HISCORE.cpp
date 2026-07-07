// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HISCORE.OBJ   from: (directly linked into exe)
// functions: 5   data: 1
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/highScoreManager.h>
VA(0x00489a30, 0x66)
highScoreManager::highScoreManager(void) {}

VA(0x00489a96, 0x14c)
int highScoreManager::Open(int) { return 0; }

VA(0x00489be2, 0x5e)
void highScoreManager::Close(void) {}

VA(0x00489c40, 0x22a)
int highScoreManager::Main(struct tag_message &) { return 0; }

VA(0x00489e6a, 0x7d3)
void highScoreManager::Update(void) {}


// ===== vtable highScoreManager : public baseManager  (3 slots) =====
//  [ 0] VA(0x00489a96, 0x14c)  int highScoreManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x00489be2, 0x5e)  void highScoreManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x00489c40, 0x22a)  int highScoreManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
