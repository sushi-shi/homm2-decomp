// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_RE\droplist.obj   from: .\basewin.lib
// functions: 12   data: 1
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x004dbf00, 0x3b)
void dropListWidget::constructor(void);

RVA(0x004dbf40, 0x1f)
void * dropListWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004dbf40, 0x1f)
void * dropListWidget::scalar_dtor(unsigned int);   // virtual [override (implements widget pure virtual)]

RVA(0x004dbf60, 0x7c)
void dropListWidget::~destructor(void);

RVA(0x004dbfe0, 0x21d)
void dropListWidget::Read(void);

RVA(0x004dc200, 0xd5)
void dropListWidget::DeleteItem(int);

RVA(0x004dc2e0, 0x350)
int dropListWidget::Main(struct tag_message &);   // virtual [override (implements widget pure virtual)]

RVA(0x004dc630, 0xaf)
void dropListWidget::Draw(void);   // virtual [override (implements widget pure virtual)]

RVA(0x004dc6e0, 0x378)
void dropListWidget::DrawDropStuff(void);

RVA(0x004dca60, 0x50)
void dropListWidget::SaveDropBackground(void);

RVA(0x004dcab0, 0x56)
void dropListWidget::RestoreDropBackground(void);

RVA(0x004dcb10, 0x81f)
void dropListWidget::ProcessSelectDialog(void);

// ---- data / globals / vtables ----
DATA(0x004ebad0)  // const dropListWidget::vftable

// ===== vtable dropListWidget : public widget  (3 slots) =====
//  [ 0] RVA(0x004dc630, 0xaf)  void dropListWidget::Draw(void)   <- override (implements widget pure virtual)
//  [ 1] RVA(0x004dbf40, 0x1f)  void * dropListWidget::scalar_dtor(unsigned int)   <- override (implements widget pure virtual)
//  [ 2] RVA(0x004dc2e0, 0x350)  int dropListWidget::Main(struct tag_message &)   <- override (implements widget pure virtual)
