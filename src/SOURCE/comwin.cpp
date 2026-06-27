// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\comwin.obj   from: (directly linked into exe)
// functions: 11   data: 0
// RVA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

RVA(0x0048a640, 0x74)
void add_node(struct tag_Anchor *, struct tag_Node *);

RVA(0x0048a6b4, 0x4c)
struct tag_Node * pop_node(struct tag_Anchor *);

RVA(0x0048a700, 0x2e)
void init_anchor(struct tag_Anchor *, int, int);

RVA(0x0048a72e, 0x3e5)
void ShutdownComError(char *);

RVA(0x0048ab13, 0x34a)
short int com_init(unsigned char, int, int);

RVA(0x0048ae5d, 0x11d)
void com_term(short int);

RVA(0x0048af7a, 0xdd)
short int com_rcv(short int, unsigned short int, void *);

RVA(0x0048b057, 0x145)
short int com_snd(short int, unsigned short int, unsigned short int, void *, int);

RVA(0x0048b19c, 0x13)
short int com_sess(int, int);

RVA(0x0048b1af, 0x6e)
unsigned char com_stat(short int, unsigned short int);

RVA(0x0048b21d, 0xe8)
void comm_wrt_task(void);

