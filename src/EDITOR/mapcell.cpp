// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\mapcell.obj   from: (directly linked into exe)
// functions: 11   data: 0
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x0040b070, 0x36)
void fullMap::constructor(void);

RVA(0x0040b0a6, 0x1e)
void fullMap::~destructor(void);

RVA(0x0040b0c4, 0x81)
void fullMap::Close(void);

RVA(0x0040b145, 0x53)
void fullMap::Init(int, int);

RVA(0x0040b198, 0xce)
void fullMap::ClearCellExtra(int);

RVA(0x0040b266, 0x130)
int fullMap::GetNewCellExtraIndex(void);

RVA(0x0040b396, 0x1d3)
struct mapCellExtra * fullMap::GetNewCellExtraOverlay(int, int);

RVA(0x0040b569, 0x1d3)
struct mapCellExtra * fullMap::GetNewCellExtraObject(int, int);

RVA(0x0040b73c, 0x9e)
void fullMap::Write(int);

RVA(0x0040b7da, 0x295)
void fullMap::Read(int, int);

RVA(0x0040ba6f, 0x2ea)
void fullMap::ChangeTilesetIndex(class mapCell *, int, int, int, int, int, int);

