// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\netwin.obj   from: (directly linked into exe)
// functions: 8   data: 0
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <SOURCE/netwin.h>
#include <_globals_model.h>
VA(0x004a6be0, 0xa8)
int is_netbios_avail(void) { return 0; }

VA(0x004a6c88, 0x244)
extern "C" int __fastcall nb_init(int, int) { return 0; }

VA(0x004a6ecc, 0x207)
extern "C" int __fastcall nb_term(void) { return 0; }

VA(0x004a70d3, 0xb3)
extern "C" int __fastcall nb_rcv(int, int) { return 0; }

VA(0x004a7186, 0xe4)
extern "C" int __fastcall nb_snd(int, int, int) { return 0; }

VA(0x004a726a, 0x4cd)
extern "C" int __cdecl nb_sess(void) { return 0; }

VA(0x004a7737, 0x21)
extern "C" char __fastcall nb_stat(short session) { return gNetStatus[session]; }

VA(0x004a7758, 0xdd2)
void nb_thr_ctl(void) {}

