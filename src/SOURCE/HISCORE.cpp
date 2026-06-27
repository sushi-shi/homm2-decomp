// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\HISCORE.OBJ   from: (directly linked into exe)
// functions: 5   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x00489a30, 0x66)
void highScoreManager::constructor(void);

RVA(0x00489a96, 0x14c)
int highScoreManager::Open(int);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00489be2, 0x5e)
void highScoreManager::Close(void);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00489c40, 0x22a)
int highScoreManager::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

RVA(0x00489e6a, 0x7d3)
void highScoreManager::Update(void);

// ---- data / globals / vtables ----
DATA(0x004eb838)  // const highScoreManager::vftable

// ===== vtable highScoreManager : public baseManager  (3 slots) =====
//  [ 0] RVA(0x00489a96, 0x14c)  int highScoreManager::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] RVA(0x00489be2, 0x5e)  void highScoreManager::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] RVA(0x00489c40, 0x22a)  int highScoreManager::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
