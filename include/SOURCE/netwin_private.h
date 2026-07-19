#ifndef HOMM2_NETWIN_PRIVATE_H
#define HOMM2_NETWIN_PRIVATE_H

#include <Ints.h>

#include <SOURCE/netwin.h>

static void nb_add_name(void);
static void __stdcall nb_add_name_done(NetbiosControlBlock* controlBlock);
static u16 __fastcall nb_recv_any(i32 session);
static void __stdcall nb_recv_any_done(NetbiosControlBlock* controlBlock);
static u16 __fastcall nb_call(i32 session, void* name);
static u16 __fastcall nb_listen(i32 session, void* name);
static void __stdcall nb_call_done(NetbiosControlBlock* controlBlock);
static void __fastcall nb_arm_recv(i32 session);
static void __fastcall nb_close_session(i32 session);
static void __fastcall nb_recv_complete(i32 session);
static void __fastcall nb_format_name(char* source, u8* destination);

extern "C" u8 __stdcall Netbios(NetbiosControlBlock* controlBlock);

#endif
