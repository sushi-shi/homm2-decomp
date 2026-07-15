// Reconstructed from CodeView NB09 of HEROES2W.EXE — NOT original source.
// compiland: .\Win32_Re\Netbios.obj   from: (directly linked into exe)
// functions: 5   data: 6
// VA(addr,size)=function (size = span to next .text symbol - 0xCC/0x90 pad); DATA(addr)=global/vtable.

#include <va.h>
#include <stdio.h>
#include <BASE/Misc.h>
#include <SOURCE/KB.h>
#include <SOURCE/Netbios.h>
#include <SOURCE/REMOTE.h>
#include <SOURCE/kbwin.h>
#include <SOURCE/netwin.h>

// @early-stop
// Raw function bytes differ only at +0x135..+0x136 and the four DIR32 entries
// at +0x139,+0x13d,+0x141,+0x145: the dispatch/table self-relocations to
// InitNetHost local labels. Every instruction byte and all 21 external
// relocation targets agree; frame 0x34 and all referenced slots are exact.
VA(0x004132f0, 0x155)
signed char InitNetHost(void)
{
    char localName[NETBIOS_NAME_BUFFER_SIZE];
    int reserved;
    int status;

    switch (iInitNetHostStatus) {
    case NETBIOS_SETUP_INITIALIZE:
        if (static_cast<short>(
                nb_init(NETBIOS_SETUP_SESSION_COUNT, NETBIOS_HOST_SESSION)) ==
            NETBIOS_INIT_UNAVAILABLE)
            ShutDown("NETBIOS is not loaded.");
        else {
            iInitNetHostStatus++;
            gbRemoteOn = 1;
        }
        break;
    case NETBIOS_SETUP_CHECK_LOCAL_NAME:
        status = !(
            static_cast<unsigned char>(nb_stat(NETBIOS_HOST_SESSION)) &
            NETBIOS_SESSION_NAME_REGISTERED);
        if (status)
            iInitNetHostStatus++;
        else
            return 1;
        break;
    case NETBIOS_SETUP_REGISTER_LOCAL_NAME:
        sprintf(localName, "H2H%d",
                Random(NETBIOS_RANDOM_NAME_MIN, NETBIOS_RANDOM_NAME_MAX));
        if (static_cast<short>(nb_sess(NETBIOS_SESSION_REGISTER, localName)) ==
            NETBIOS_RESULT_SUCCESS)
            iInitNetHostStatus++;
        else
            ShutDown("Network initialization failed");
        break;
    case NETBIOS_SETUP_WAIT_FOR_LOCAL_NAME:
        status = static_cast<unsigned char>(nb_stat(NETBIOS_HOST_SESSION));
        if (status & NETBIOS_SESSION_NAME_REGISTERED)
            return 1;
        else if (status & NETBIOS_SESSION_ERROR) {
        }
        break;
    }
    return 0;
}

// @early-stop
// Raw function bytes differ only at +0x187..+0x188 and the five DIR32 entries
// at +0x18b,+0x18f,+0x193,+0x197,+0x19b: the dispatch/table self-relocations to
// InitNetGuest local labels. Every instruction byte and all 32 external
// relocation targets agree; frame 0x34 and all referenced slots are exact.
VA(0x00413445, 0x1ab)
signed char InitNetGuest(void)
{
    char localName[NETBIOS_NAME_BUFFER_SIZE];

    switch (iInitNetGuestStatus) {
    case NETBIOS_SETUP_INITIALIZE:
        if (static_cast<short>(
                nb_init(NETBIOS_SETUP_SESSION_COUNT, NETBIOS_GUEST_SESSION)) ==
            NETBIOS_INIT_UNAVAILABLE)
            ShutDown("NETBIOS is not loaded.");
        else {
            gbRemoteOn = 1;
            iInitNetGuestStatus++;
        }
        break;
    case NETBIOS_SETUP_CHECK_LOCAL_NAME:
        if (static_cast<unsigned char>(nb_stat(NETBIOS_GUEST_SESSION)) &
            NETBIOS_SESSION_NAME_REGISTERED)
            iInitNetGuestStatus += NETBIOS_REGISTERED_STATE_ADVANCE;
        else
            iInitNetGuestStatus++;
        break;
    case NETBIOS_SETUP_REGISTER_LOCAL_NAME:
        sprintf(localName, "H2G%d",
                Random(NETBIOS_RANDOM_NAME_MIN, NETBIOS_RANDOM_NAME_MAX));
        if (static_cast<short>(nb_sess(NETBIOS_SESSION_REGISTER, localName)) ==
            NETBIOS_RESULT_SUCCESS)
            iInitNetGuestStatus++;
        else
            iNameRetryCount++;
        break;
    case NETBIOS_SETUP_WAIT_FOR_LOCAL_NAME: {
        int status = static_cast<unsigned char>(nb_stat(NETBIOS_GUEST_SESSION));
        int namePending = !(status & NETBIOS_SESSION_NAME_REGISTERED);
        if (namePending) {
            if (status & NETBIOS_SESSION_ERROR) {
                iNameRetryCount++;
                iInitNetGuestStatus--;
            }
        } else {
            iInitNetGuestStatus++;
        }
        break;
    }
    case NETBIOS_SETUP_START_RECEIVE:
        if (static_cast<short>(
                nb_sess(NETBIOS_SESSION_RECEIVE_ANY, NETBIOS_HOST_SESSION)) !=
            NETBIOS_RESULT_SUCCESS) {
            sprintf(gText, "Network initialization failed");
            ShutDown(gText);
        }
        return 1;
    }
    return 0;
}

VA(0x004135f0, 0x5f)
signed char WaitForHost(void)
{
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    int status;

    switch (iWaitForHostStatus) {
    case NETBIOS_WAIT_START:
        status = static_cast<unsigned char>(nb_stat(NETBIOS_HOST_SESSION)) &
                 NETBIOS_SESSION_ACTIVE;
        if (status != 0)
            return 1;
        break;
    }
    return 0;
}

VA(0x0041364f, 0xe9)
signed char WaitForGuest(void)
{
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    int status;

    switch (iWaitForGuestStatus) {
    case NETBIOS_WAIT_START:
        status = static_cast<short>(
            nb_sess(NETBIOS_SESSION_LISTEN_ANY, NETBIOS_GUEST_SESSION));
        if (status == NETBIOS_RESULT_SUCCESS)
            iWaitForGuestStatus++;
        return 0;
    case NETBIOS_WAIT_POLL:
        status = !(
            static_cast<unsigned char>(nb_stat(NETBIOS_GUEST_SESSION)) &
            NETBIOS_SESSION_ACTIVE);
        if (status) {
            if (KBTickCount() > iLastBroadcastTime + NETBIOS_BROADCAST_INTERVAL) {
                iLastBroadcastTime = KBTickCount();
                nb_snd(NETBIOS_HOST_SESSION, 0, 0);
            }
        } else {
            nb_sess(NETBIOS_SESSION_MOVE, NETBIOS_GUEST_SESSION,
                    NETBIOS_CONNECTED_SESSION, NETBIOS_DETACH_SOURCE_SESSION);
            return 1;
        }
    }
    return 0;
}

VA(0x00413738, 0x1ba)
int nbnet_init(void)
{
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    int unused;

    LogStr("GUON1");
    switch (GameMode) {
    case NETBIOS_GAME_MODE_HOST:
        giWaitType = NETBIOS_INITIALIZE_HOST;
        sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
        NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
        giWaitType = NETBIOS_WAIT_FOR_GUEST;
        sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
        LogStr("GUON2");
        NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
        LogStr("GUON3");
        if (gbFunctionComplete == 0)
            ShutDown(0);
        LogStr("GUON4");
        break;
    case NETBIOS_GAME_MODE_GUEST:
        giWaitType = NETBIOS_INITIALIZE_GUEST;
        sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
        NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
        giWaitType = NETBIOS_WAIT_FOR_HOST;
        sprintf(gText, "Waiting On Host.\n\n  Press 'CANCEL' to abort.");
        NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
        if (gbFunctionComplete == 0)
            ShutDown(0);
        break;
    }
    LogStr("GUON5");
    return 0;
}

// ---- globals (definitions, RVA order) ----
DATA(0x004ee5c8) signed char iInitNetHostStatus = 0;
DATA(0x004ee5cc) signed char iInitNetGuestStatus = 0;
DATA(0x004ee5d0) int iNameRetryCount = 0;
DATA(0x004ee614) signed char iWaitForHostStatus = 0;
DATA(0x004ee658) signed char iWaitForGuestStatus = 0;
DATA(0x004ee65c) int iLastBroadcastTime = 0;
