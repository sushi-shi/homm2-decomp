// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\WINDOW.OBJ   from: .\basewin.lib
// functions: 15   data: 0
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004ceb70, 0xaa)
void heroWindow::constructor(void);

RVA(0x004cec20, 0xa5)
void heroWindow::constructor(int, int, int, int, int);

RVA(0x004cecd0, 0x521)
void heroWindow::constructor(int, int, char *);

RVA(0x004cf200, 0x73)
int heroWindow::Open(int, int);

RVA(0x004cf280, 0x90)
void heroWindow::RemoveAndDeleteWidget(int);

RVA(0x004cf310, 0xaa)
void heroWindow::Close(void);

RVA(0x004cf3c0, 0x13c)
void heroWindow::AddWidget(class widget *, int);

RVA(0x004cf500, 0x116)
void heroWindow::RemoveWidget(class widget *);

RVA(0x004cf620, 0x95)
int heroWindow::BroadcastMessage(struct tag_message &);

RVA(0x004cf6c0, 0x20)
void heroWindow::DrawWindow(void);

RVA(0x004cf6e0, 0x2e)
void heroWindow::DrawWindow(int);

RVA(0x004cf710, 0x116)
void heroWindow::DrawWindow(int, int, int);

RVA(0x004cf830, 0x7f)
int heroWindow::SaveBackground(void);

RVA(0x004cf8b0, 0x97)
void heroWindow::RestoreBackground(void);

RVA(0x004cf950, 0x186)
void heroWindow::MoveWindow(int, int);

