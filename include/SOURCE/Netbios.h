#ifndef HOMM2_NETBIOS_H
#define HOMM2_NETBIOS_H

#include <Ints.h>
// Declarations of the free functions DEFINED in Netbios.cpp — the single home for these
// symbols. Other TUs call them by including this header (no local externs).

typedef enum NetbiosSetupConstant {
    NETBIOS_INIT_UNAVAILABLE = 1,
    NETBIOS_SETUP_SESSION_COUNT = 5,
    NETBIOS_HOST_SESSION = 0,
    NETBIOS_CONNECTED_SESSION = 1,
    NETBIOS_GUEST_SESSION = 6,
    NETBIOS_RANDOM_NAME_MIN = 0,
    NETBIOS_NAME_BUFFER_SIZE = 40,
    NETBIOS_SCRATCH_BUFFER_SIZE = 80,
    NETBIOS_RANDOM_NAME_MAX = 99999,
    NETBIOS_BROADCAST_INTERVAL = 500,
    NETBIOS_REGISTERED_STATE_ADVANCE = 3,
    NETBIOS_DETACH_SOURCE_SESSION = 1
} NetbiosSetupConstant;

typedef enum NetbiosSetupState {
    NETBIOS_SETUP_INITIALIZE = 0,
    NETBIOS_SETUP_CHECK_LOCAL_NAME = 1,
    NETBIOS_SETUP_REGISTER_LOCAL_NAME = 2,
    NETBIOS_SETUP_WAIT_FOR_LOCAL_NAME = 3,
    NETBIOS_SETUP_START_RECEIVE = 4
} NetbiosSetupState;

typedef enum NetbiosWaitState {
    NETBIOS_WAIT_START = 0,
    NETBIOS_WAIT_POLL = 1
} NetbiosWaitState;

typedef enum NetbiosDialogWaitType {
    NETBIOS_WAIT_FOR_GUEST = 1,
    NETBIOS_WAIT_FOR_HOST = 2,
    NETBIOS_INITIALIZE_GUEST = 3,
    NETBIOS_INITIALIZE_HOST = 4
} NetbiosDialogWaitType;

typedef enum NetbiosGameMode {
    NETBIOS_GAME_MODE_HOST = 1,
    NETBIOS_GAME_MODE_GUEST = 2
} NetbiosGameMode;

i8 InitNetHost(void);
i8 InitNetGuest(void);
i8 WaitForHost(void);
i8 WaitForGuest(void);
i32 nbnet_init(void);

// ---- globals (declarations, RVA order) ----
extern i8 iInitNetHostStatus;
extern i8 iInitNetGuestStatus;
extern i32 iNameRetryCount;
extern i8 iWaitForHostStatus;
extern i8 iWaitForGuestStatus;
extern i32 iLastBroadcastTime;

#endif // HOMM2_NETBIOS_H
