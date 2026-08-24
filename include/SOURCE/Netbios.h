#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H

#include <Ints.h>

enum {
    SETUP_INITIALIZE          = 0,
    SETUP_CHECK_LOCAL_NAME    = 1,
    SETUP_REGISTER_LOCAL_NAME = 2,
    SETUP_WAIT_FOR_LOCAL_NAME = 3,
    SETUP_START_RECEIVE       = 4
};
typedef i32 NetbiosSetupState;
enum {
    WAIT_START = 0,
    WAIT_POLL  = 1
};
typedef i32 NetbiosWaitState;
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
