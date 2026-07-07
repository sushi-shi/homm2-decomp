#ifndef HOMM2_COMWIN_H
#define HOMM2_COMWIN_H
// Declarations of the free functions DEFINED in comwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).
#include <_all.h>

void add_node(struct tag_Anchor *, struct tag_Node *);
struct tag_Node * pop_node(struct tag_Anchor *);
void init_anchor(struct tag_Anchor *, int, int);
void ShutdownComError(char *);
short int com_init(unsigned char, int, int);
void com_term(short int);
short int com_rcv(short int, unsigned short int, void *);
short int com_snd(short int, unsigned short int, unsigned short int, void *, int);
short int __cdecl com_sess(int, int, ...);
unsigned char com_stat(short int, unsigned short int);
void comm_wrt_task(void);

#endif // HOMM2_COMWIN_H
