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
extern signed char iInitNetHostStatus;
extern signed char iInitNetGuestStatus;
extern int iNameRetryCount;
extern signed char iWaitForHostStatus;
extern signed char iWaitForGuestStatus;
extern int iLastBroadcastTime;

#endif // HOMM2_NETBIOS_H
