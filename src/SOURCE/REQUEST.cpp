// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\REQUEST.OBJ   from: (directly linked into exe)
// functions: 16   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/fileRequester.h>
#include <SOURCE/REQUEST.h>
VA(0x0048c920, 0x80)
int GetMapHeader(char *, struct SMapHeader *) { return 0; }

VA(0x0048c9a0, 0x1b)
int CheckSumIsDemoOK(char *) { return 0; }

VA(0x0048c9bb, 0xbb)
int ShowThisMapGame(char *) { return 0; }

VA(0x0048ca76, 0x1b)
int ShowThisMap(char *) { return 0; }

VA(0x0048ca91, 0x7c1)
int fileRequester::InitializeFiles(char *, char *, int) { return 0; }

VA(0x0048d252, 0x16e)
fileRequester::fileRequester(int, int, int, char *, char *, char *) {}

VA(0x0048d3c0, 0x63)
int fileRequester::MapExistsForFilter(int) { return 0; }

VA(0x0048d423, 0x6c)
void fileRequester::SetupFiles(void) {}

VA(0x0048d48f, 0xc7)
void fileRequester::CleanUpData(void) {}

VA(0x0048d556, 0x8b)
void fileRequester::Close(void) {}

VA(0x0048d5e1, 0x466)
int fileRequester::Open(int) { return 0; }

VA(0x0048da47, 0xa5)
void fileRequester::SetOK(int) {}

VA(0x0048daec, 0x11ae)
int fileRequester::Main(struct tag_message &) { return 0; }

VA(0x0048ec9a, 0x2e8)
void fileRequester::DoKnob(void) {}

VA(0x0048ef82, 0xc42)
void fileRequester::Update(int) {}

VA(0x0048fbc4, 0x15b)
char * fileRequester::GetFilename(void) { return 0; }


// ===== vtable fileRequester : public baseManager  (3 slots) =====
//  [ 0] VA(0x0048d5e1, 0x466)  int fileRequester::Open(int)   <- override (implements baseManager pure virtual)
//  [ 1] VA(0x0048d556, 0x8b)  void fileRequester::Close(void)   <- override (implements baseManager pure virtual)
//  [ 2] VA(0x0048daec, 0x11ae)  int fileRequester::Main(struct tag_message &)   <- override (implements baseManager pure virtual)

// ---- vtables (compiler-emitted; census) ----
VTBL(fileRequester, 0x004eb888);
