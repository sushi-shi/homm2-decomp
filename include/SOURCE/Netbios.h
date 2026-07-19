#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H

#include <Ints.h>

i8 InitNetHost(void);
i8 InitNetGuest(void);
i8 WaitForHost(void);
i8 WaitForGuest(void);
i32 nbnet_init(void);

extern i8 iInitNetHostStatus;
extern i8 iInitNetGuestStatus;
extern i32 iNameRetryCount;
extern i8 iWaitForHostStatus;
extern i8 iWaitForGuestStatus;
extern i32 iLastBroadcastTime;

#endif
