#include <Ints.h>
#include <stdio.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <PLATFORM/WIN32/NetbiosSession.h>
#include <SOURCE/REMOTE.h>
#include <PLATFORM/WIN32/Application.h>
#include <PLATFORM/WIN32/NetbiosTransport.h>

typedef enum NetbiosSetupConstant {
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
} NetbiosSetupConstant;

i8 InitNetHost(void) {
    char localName[NAME_BUFFER_SIZE];
    i32 reserved;
    i32 needName;

    switch (iInitNetHostStatus) {
        case SETUP_INITIALIZE:
            if (static_cast<i16>(nb_init(SETUP_SESSION_COUNT, HOST_SESSION))
                == INIT_UNAVAILABLE)
                ShutDown("NETBIOS \xed\xe5 \xe7\xe0\xe3\xf0\xf3\xe6\xe5\xed\xe0.");
            else {
                iInitNetHostStatus++;
                gbRemoteOn = true;
            }
            break;
        case SETUP_CHECK_LOCAL_NAME:
            needName =
                !(H2EnumIndex((static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(HOST_SESSION)))) & (NETBIOS_SESSION_NAME_REGISTERED)));
            if (needName)
                iInitNetHostStatus++;
            else
                return 1;
            break;
        case SETUP_REGISTER_LOCAL_NAME:
            sprintf(
                localName,
                "H2H%d",
                Random(RANDOM_NAME_MIN, RANDOM_NAME_MAX)
            );
            if (static_cast<NetbiosResult>(
                    static_cast<i16>(nb_sess(NETBIOS_SESSION_REGISTER, localName))
                )
                == NETBIOS_RESULT_SUCCESS)
                iInitNetHostStatus++;
            else
                ShutDown("\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8 \xf1\xe5\xf2\xe8.");
            break;
        case SETUP_WAIT_FOR_LOCAL_NAME:
            needName = static_cast<u8>(nb_stat(HOST_SESSION));
            if ((H2EnumIndex((static_cast<NetbiosSessionStatus>(needName)) & (NETBIOS_SESSION_NAME_REGISTERED))))
                return 1;
            else if ((H2EnumIndex((static_cast<NetbiosSessionStatus>(needName)) & (NETBIOS_SESSION_ERROR)))) {
            }
            break;
    }
    return 0;
}

i8 InitNetGuest(void) {
    char localName[NAME_BUFFER_SIZE];
    i32 unregistered;

    switch (iInitNetGuestStatus) {
        case SETUP_INITIALIZE:
            if (static_cast<i16>(nb_init(SETUP_SESSION_COUNT, GUEST_SESSION))
                == INIT_UNAVAILABLE)
                ShutDown("NETBIOS \xed\xe5 \xe7\xe0\xe3\xf0\xf3\xe6\xe5\xed\xe0.");
            else {
                gbRemoteOn = true;
                iInitNetGuestStatus++;
            }
            break;
        case SETUP_CHECK_LOCAL_NAME:
            if ((H2EnumIndex((static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(GUEST_SESSION)))) & (NETBIOS_SESSION_NAME_REGISTERED))))
                iInitNetGuestStatus += REGISTERED_STATE_ADVANCE;
            else
                iInitNetGuestStatus++;
            break;
        case SETUP_REGISTER_LOCAL_NAME:
            sprintf(
                localName,
                "H2G%d",
                Random(RANDOM_NAME_MIN, RANDOM_NAME_MAX)
            );
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
            unregistered =
                !(H2EnumIndex((static_cast<NetbiosSessionStatus>(status)) & (NETBIOS_SESSION_NAME_REGISTERED)));
            if (unregistered) {
                if ((H2EnumIndex((static_cast<NetbiosSessionStatus>(status)) & (NETBIOS_SESSION_ERROR)))) {
                    iNameRetryCount++;
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
                sprintf(gText, "\xce\xf8\xe8\xe1\xea\xe0 \xe8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xe8 \xf1\xe5\xf2\xe8.");
                ShutDown(gText);
            }
            return 1;
    }
    return 0;
}

i8 WaitForHost(void) {
    i32 status;

    switch (iWaitForHostStatus) {
        case WAIT_START:
            status = (H2EnumIndex((static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(HOST_SESSION)))) & (NETBIOS_SESSION_ACTIVE)));
            if (status != 0)
                return 1;
            break;
    }
    return 0;
}

i8 WaitForGuest(void) {
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
            status = !(H2EnumIndex((static_cast<NetbiosSessionStatus>(static_cast<u8>(nb_stat(GUEST_SESSION)))) & (NETBIOS_SESSION_ACTIVE)));
            if (status) {
                if (platform::Ticks() > iLastBroadcastTime + BROADCAST_INTERVAL) {
                    iLastBroadcastTime = platform::Ticks();
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
    i32 unused;

    LogStr("GUON1");
    switch (GameMode) {
        case REMOTE_GAME_NETWORK_HOST:
            giWaitType = DIALOG_WAIT_NETBIOS_INIT_HOST;
            sprintf(gText, "\xc8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xff \xf1\xe5\xf2\xe8.\n\n  "
                "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            giWaitType = DIALOG_WAIT_NETBIOS_GUEST;
            sprintf(gText, "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xe3\xee\xf1\xf2\xff.\n\n  "
                "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.");
            LogStr("GUON2");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            LogStr("GUON3");
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            LogStr("GUON4");
            break;
        case REMOTE_GAME_NETWORK_GUEST:
            giWaitType = DIALOG_WAIT_NETBIOS_INIT_GUEST;
            sprintf(gText, "\xc8\xed\xe8\xf6\xe8\xe0\xeb\xe8\xe7\xe0\xf6\xe8\xff \xf1\xe5\xf2\xe8.\n\n  "
                "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            giWaitType = DIALOG_WAIT_NETBIOS_HOST;
            sprintf(gText, "\xce\xe6\xe8\xe4\xe0\xed\xe8\xe5 \xf5\xee\xf1\xf2\xe0.\n\n  "
                "\xcd\xe0\xe6\xec\xe8\xf2\xe5 '\xce\xd2\xcc\xc5\xcd\xc0', \xf7\xf2\xee\xe1\xfb "
                "\xef\xf0\xe5\xf0\xe2\xe0\xf2\xfc \xf1\xee\xe5\xe4\xe8\xed\xe5\xed\xe8\xe5.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(NULL);
            break;
    }
    LogStr("GUON5");
    return 0;
}

H2SteppedEnumStorage<NetbiosSetupState, i8> iInitNetHostStatus = SETUP_INITIALIZE;
H2SteppedEnumStorage<NetbiosSetupState, i8> iInitNetGuestStatus = SETUP_INITIALIZE;
i32 iNameRetryCount = 0;
H2SteppedEnumStorage<NetbiosWaitState, i8> iWaitForHostStatus = WAIT_START;
H2SteppedEnumStorage<NetbiosWaitState, i8> iWaitForGuestStatus = WAIT_START;
i32 iLastBroadcastTime = 0;
