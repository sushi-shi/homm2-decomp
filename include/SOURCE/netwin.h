#ifndef HOMM2_NETWIN_H
#define HOMM2_NETWIN_H
// Declarations of the free functions DEFINED in netwin.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

int is_netbios_avail(void);
extern "C" unsigned short __fastcall nb_init(unsigned short, unsigned short);
extern "C" int __fastcall nb_term(void);
extern "C" unsigned short __fastcall nb_rcv(short, void *);
extern "C" unsigned short __fastcall nb_snd(short, short, void *);
extern "C" int __cdecl nb_sess(void);
extern "C" char __fastcall nb_stat(short);
void nb_thr_ctl(void);

#endif // HOMM2_NETWIN_H
