#include <va.h>
#include <stdio.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/Netbios.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/netwin.h>

H2_ENUM_BEGIN(NetbiosSetupConstant)
    INIT_UNAVAILABLE         = 1,
    SETUP_SESSION_COUNT      = 5,
    HOST_SESSION             = 0,
    CONNECTED_SESSION        = 1,
    GUEST_SESSION            = 6,
    RANDOM_NAME_MIN          = 0,
    NAME_BUFFER_SIZE         = 40,
    SCRATCH_BUFFER_SIZE      = 80,
    RANDOM_NAME_MAX          = 99999,
    BROADCAST_INTERVAL       = 500,
    REGISTERED_STATE_ADVANCE = 3,
    DETACH_SOURCE_SESSION    = 1
H2_ENUM_END(NetbiosSetupConstant)

i8 InitNetHost(void) {
    char localName[NAME_BUFFER_SIZE];
    i32 reserved;
    i32 status;

    switch (iInitNetHostStatus) {
        case SETUP_INITIALIZE:
            if (static_cast<i16>(nb_init(SETUP_SESSION_COUNT, HOST_SESSION))
                == INIT_UNAVAILABLE)
                ShutDown("NETBIOS is not loaded.");
            else {
                iInitNetHostStatus++;
                gbRemoteOn = true;
            }
            break;
        case SETUP_CHECK_LOCAL_NAME:
            status =
                !HAS(
                    static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(HOST_SESSION))),
                    NETBIOS_SESSION_NAME_REGISTERED
                );
            if (status)
                iInitNetHostStatus++;
            else
                return 1;
            break;
        case SETUP_REGISTER_LOCAL_NAME:
            sprintf(localName, "H2H%d", Random(RANDOM_NAME_MIN, RANDOM_NAME_MAX));
            if (static_cast<NetbiosResult>(
                    static_cast<i16>(nb_sess(NETBIOS_SESSION_REGISTER, localName))
                )
                == NETBIOS_RESULT_SUCCESS)
                iInitNetHostStatus++;
            else
                ShutDown("Network initialization failed");
            break;
        case SETUP_WAIT_FOR_LOCAL_NAME:
            status = static_cast<u8>(nb_stat(HOST_SESSION));
            if (HAS(static_cast<NetbiosSessionStatus>(status), NETBIOS_SESSION_NAME_REGISTERED))
                return 1;
            else if (HAS(static_cast<NetbiosSessionStatus>(status), NETBIOS_SESSION_ERROR)) {
            }
            break;
    }
    return 0;
}

i8 InitNetGuest(void) {
    char localName[NAME_BUFFER_SIZE];

    switch (iInitNetGuestStatus) {
        case SETUP_INITIALIZE:
            if (static_cast<i16>(nb_init(SETUP_SESSION_COUNT, GUEST_SESSION))
                == INIT_UNAVAILABLE)
                ShutDown("NETBIOS is not loaded.");
            else {
                gbRemoteOn = true;
                iInitNetGuestStatus++;
            }
            break;
        case SETUP_CHECK_LOCAL_NAME:
            if (HAS(
                    static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(GUEST_SESSION))),
                    NETBIOS_SESSION_NAME_REGISTERED
                ))
                iInitNetGuestStatus += REGISTERED_STATE_ADVANCE;
            else
                iInitNetGuestStatus++;
            break;
        case SETUP_REGISTER_LOCAL_NAME:
            sprintf(localName, "H2G%d", Random(RANDOM_NAME_MIN, RANDOM_NAME_MAX));
            if (static_cast<NetbiosResult>(
                    static_cast<i16>(nb_sess(NETBIOS_SESSION_REGISTER, localName))
                )
                == NETBIOS_RESULT_SUCCESS)
                iInitNetGuestStatus++;
            else
                iNameRetryCount++;
            break;
        case SETUP_WAIT_FOR_LOCAL_NAME: {
            i32 status = static_cast<u8>(nb_stat(GUEST_SESSION));
            i32 namePending =
                !HAS(static_cast<NetbiosSessionStatus>(status), NETBIOS_SESSION_NAME_REGISTERED);
            if (namePending) {
                if (HAS(static_cast<NetbiosSessionStatus>(status), NETBIOS_SESSION_ERROR)) {
                    iNameRetryCount++;
                    // SETUP_WAIT_FOR_LOCAL_NAME failed; return to SETUP_REGISTER_LOCAL_NAME.
                    iInitNetGuestStatus--;
                }
            } else {
                iInitNetGuestStatus++;
            }
            break;
        }
        case SETUP_START_RECEIVE:
            if (static_cast<NetbiosResult>(
                    static_cast<i16>(
                        nb_sess(NETBIOS_SESSION_RECEIVE_ANY, HOST_SESSION)
                    )
                )
                != NETBIOS_RESULT_SUCCESS) {
                sprintf(gText, "Network initialization failed");
                ShutDown(gText);
            }
            return 1;
    }
    return 0;
}

i8 WaitForHost(void) {
    char scratch[SCRATCH_BUFFER_SIZE];
    i32 status;

    switch (iWaitForHostStatus) {
        case WAIT_START:
            status = HAS(
                static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(HOST_SESSION))),
                NETBIOS_SESSION_ACTIVE
            );
            if (status != 0)
                return 1;
            break;
    }
    return 0;
}

i8 WaitForGuest(void) {
    char scratch[SCRATCH_BUFFER_SIZE];
    i32 status;

    switch (iWaitForGuestStatus) {
        case WAIT_START:
            status = static_cast<i16>(
                nb_sess(NETBIOS_SESSION_LISTEN_ANY, GUEST_SESSION)
            );
            if (static_cast<NetbiosResult>(status) == NETBIOS_RESULT_SUCCESS)
                iWaitForGuestStatus++;
            return 0;
        case WAIT_POLL:
            status = !HAS(
                static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(GUEST_SESSION))),
                NETBIOS_SESSION_ACTIVE
            );
            if (status) {
                if (KBTickCount() > iLastBroadcastTime + BROADCAST_INTERVAL) {
                    iLastBroadcastTime = KBTickCount();
                    nb_snd(HOST_SESSION, 0, NULL);
                }
            } else {
                nb_sess(
                    NETBIOS_SESSION_MOVE,
                    GUEST_SESSION,
                    CONNECTED_SESSION,
                    DETACH_SOURCE_SESSION
                );
                return 1;
            }
    }
    return 0;
}

i32 nbnet_init(void) {
    char scratch[SCRATCH_BUFFER_SIZE];
    i32 unused;

    LogStr("GUON1");
    switch (GameMode) {
        case REMOTE_GAME_NETWORK_HOST:
            giWaitType = DIALOG_WAIT_NETBIOS_INIT_HOST;
            sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            giWaitType = DIALOG_WAIT_NETBIOS_GUEST;
            sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
            LogStr("GUON2");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            LogStr("GUON3");
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            LogStr("GUON4");
            break;
        case REMOTE_GAME_NETWORK_GUEST:
            giWaitType = DIALOG_WAIT_NETBIOS_INIT_GUEST;
            sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            giWaitType = DIALOG_WAIT_NETBIOS_HOST;
            sprintf(gText, "Waiting On Host.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            break;
    }
    LogStr("GUON5");
    return 0;
}

H2_ENUM_STORAGE_STEPPED(NetbiosSetupState, i8) iInitNetHostStatus = SETUP_INITIALIZE;
H2_ENUM_STORAGE_STEPPED(NetbiosSetupState, i8) iInitNetGuestStatus = SETUP_INITIALIZE;
i32 iNameRetryCount = 0;
H2_ENUM_STORAGE_STEPPED(NetbiosWaitState, i8) iWaitForHostStatus = WAIT_START;
H2_ENUM_STORAGE_STEPPED(NetbiosWaitState, i8) iWaitForGuestStatus = WAIT_START;
i32 iLastBroadcastTime = 0;
