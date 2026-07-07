#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H
// Declarations of the free functions DEFINED in Netbios.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

signed char InitNetHost(void);
signed char InitNetGuest(void);
signed char WaitForHost(void);
signed char WaitForGuest(void);
int nbnet_init(void);

// ---- globals (declarations, RVA order) ----
DATA(0x004ee5c8) extern signed char iInitNetHostStatus;
DATA(0x004ee5cc) extern signed char iInitNetGuestStatus;
DATA(0x004ee5d0) extern int iNameRetryCount;
DATA(0x004ee614) extern signed char iWaitForHostStatus;
DATA(0x004ee658) extern signed char iWaitForGuestStatus;
DATA(0x004ee65c) extern int iLastBroadcastTime;

#endif // HOMM2_NETBIOS_H
