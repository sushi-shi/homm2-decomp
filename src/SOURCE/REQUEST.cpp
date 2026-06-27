// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REQUEST.OBJ   from: (directly linked into exe)
// functions: 16   data: 6
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x0048c920, 0x80)
int GetMapHeader(char *, struct SMapHeader *);

RVA(0x0048c9a0, 0x1b)
int CheckSumIsDemoOK(char *);

RVA(0x0048c9bb, 0xbb)
int ShowThisMapGame(char *);

RVA(0x0048ca76, 0x1b)
int ShowThisMap(char *);

RVA(0x0048ca91, 0x7c1)
int fileRequester::InitializeFiles(char *, char *, int);

RVA(0x0048d252, 0x16e)
void fileRequester::constructor(int, int, int, char *, char *, char *);

RVA(0x0048d3c0, 0x63)
int fileRequester::MapExistsForFilter(int);

RVA(0x0048d423, 0x6c)
void fileRequester::SetupFiles(void);

RVA(0x0048d48f, 0xc7)
void fileRequester::CleanUpData(void);

RVA(0x0048d556, 0x8b)
void fileRequester::Close(void);   // virtual [override (implements baseManager pure virtual)]

RVA(0x0048d5e1, 0x466)
int fileRequester::Open(int);   // virtual [override (implements baseManager pure virtual)]

RVA(0x0048da47, 0xa5)
void fileRequester::SetOK(int);

RVA(0x0048daec, 0x11ae)
int fileRequester::Main(struct tag_message &);   // virtual [override (implements baseManager pure virtual)]

RVA(0x0048ec9a, 0x2e8)
void fileRequester::DoKnob(void);

RVA(0x0048ef82, 0xc42)
void fileRequester::Update(int);

RVA(0x0048fbc4, 0x15b)
char * fileRequester::GetFilename(void);

// ---- data / globals / vtables ----
DATA(0x004eb888)  // const fileRequester::vftable
DATA(0x004f8674)  // int giMapSizeFilter
DATA(0x004f88c4)  // char * cFRDummy
DATA(0x0052857c)  // float fGutterMinY
DATA(0x00528580)  // float fGutterTravelLength
DATA(0x00528584)  // int iMaxListSize

// ===== vtable fileRequester : public baseManager  (3 slots) =====
//  [ 0] RVA(0x0048d5e1, 0x466)  int fileRequester::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] RVA(0x0048d556, 0x8b)  void fileRequester::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] RVA(0x0048daec, 0x11ae)  int fileRequester::Main(struct tag_message &)   <- override (implements baseManager pure virtual)
