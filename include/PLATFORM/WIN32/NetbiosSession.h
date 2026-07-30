#ifndef HOMM2_PLATFORM_WIN32_NETBIOSSESSION_H
#define HOMM2_PLATFORM_WIN32_NETBIOSSESSION_H

#include <Ints.h>

enum class NetbiosSetupState : i8 {
    SETUP_INITIALIZE          = 0,
    SETUP_CHECK_LOCAL_NAME    = 1,
    SETUP_REGISTER_LOCAL_NAME = 2,
    SETUP_WAIT_FOR_LOCAL_NAME = 3,
    SETUP_START_RECEIVE       = 4
};
using enum NetbiosSetupState;

enum class NetbiosWaitState : i8 {
    WAIT_START = 0,
    WAIT_POLL  = 1
};
using enum NetbiosWaitState;

i8 InitNetHost(void);
i8 InitNetGuest(void);
i8 WaitForHost(void);
i8 WaitForGuest(void);
i32 nbnet_init(void);

extern H2SteppedEnumStorage<NetbiosSetupState, i8> iInitNetHostStatus;
extern H2SteppedEnumStorage<NetbiosSetupState, i8> iInitNetGuestStatus;
extern i32 iNameRetryCount;
extern H2SteppedEnumStorage<NetbiosWaitState, i8> iWaitForHostStatus;
extern H2SteppedEnumStorage<NetbiosWaitState, i8> iWaitForGuestStatus;
extern i32 iLastBroadcastTime;

#endif
