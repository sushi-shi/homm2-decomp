#include <PLATFORM/Network.h>

// Retail reached the other players over DirectPlay, NetBIOS or a serial cable.
// None of the three has a home on the systems this port runs on, and the
// emulation of them was only ever emulation. TCP/IP is the transport that
// works; these answer honestly until something replaces them.

i16 com_init(u8, ComBaudRate, i32) { return 0; }

void com_term(i16) {}

i16 com_rcv(i16, u16, void*) { return 0; }

i16 com_snd(i16, u16, u16, void*, i32) { return 0; }

void comm_wrt_task() {}

i16 dpnet_init() { return 0; }

void dpnet_term() {}

i32 dpnet_snd(i32, i32, void*) { return 0; }

i16 dpnet_rcv(i16, u16, void*) { return 0; }

void dpProcessMessages() {}

i32 dpWaitForFirstGuest() { return 0; }

i32 dpWaitForExtraGuests() { return 0; }

i32 dpWaitForHost() { return 0; }

i32 nbnet_init() { return 0; }

void nb_term() {}

u16 nb_rcv(i16, void*) { return 0; }

u16 nb_snd(i16, i16, void*) { return 0; }

void nb_thr_ctl() {}

i8 InitNetHost() { return 0; }

i8 InitNetGuest() { return 0; }

i8 WaitForHost() { return 0; }

i8 WaitForGuest() { return 0; }

void network_remove_player(i32) {}
