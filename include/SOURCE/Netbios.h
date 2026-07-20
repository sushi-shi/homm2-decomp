#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H

#include <Ints.h>

H2_ENUM_CLASS_BEGIN(NetbiosSetupState)
    SETUP_INITIALIZE          = 0,
    SETUP_CHECK_LOCAL_NAME    = 1,
    SETUP_REGISTER_LOCAL_NAME = 2,
    SETUP_WAIT_FOR_LOCAL_NAME = 3,
    SETUP_START_RECEIVE       = 4
H2_ENUM_CLASS_END(NetbiosSetupState)

H2_ENUM_CLASS_BEGIN(NetbiosWaitState)
    WAIT_START = 0,
    WAIT_POLL  = 1
H2_ENUM_CLASS_END(NetbiosWaitState)

i8 InitNetHost(void);
i8 InitNetGuest(void);
i8 WaitForHost(void);
i8 WaitForGuest(void);
i32 nbnet_init(void);

extern H2_ENUM_STORAGE_STEPPED(NetbiosSetupState, i8) iInitNetHostStatus;
extern H2_ENUM_STORAGE_STEPPED(NetbiosSetupState, i8) iInitNetGuestStatus;
extern i32 iNameRetryCount;
extern H2_ENUM_STORAGE_STEPPED(NetbiosWaitState, i8) iWaitForHostStatus;
extern H2_ENUM_STORAGE_STEPPED(NetbiosWaitState, i8) iWaitForGuestStatus;
extern i32 iLastBroadcastTime;

#endif
