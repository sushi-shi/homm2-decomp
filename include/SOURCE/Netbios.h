#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H
// Declarations of the free functions DEFINED in Netbios.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

signed char InitNetHost(void);
signed char InitNetGuest(void);
signed char WaitForHost(void);
signed char WaitForGuest(void);
int nbnet_init(void);

#endif // HOMM2_NETBIOS_H
