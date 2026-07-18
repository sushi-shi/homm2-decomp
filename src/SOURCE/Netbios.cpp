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
i8 InitNetHost(void) {
    char localName[NETBIOS_NAME_BUFFER_SIZE];
    i32 reserved;
    i32 status;

    switch (iInitNetHostStatus) {
        case IDX(NETBIOS_SETUP_INITIALIZE):
            if (static_cast<i16>(nb_init(NETBIOS_SETUP_SESSION_COUNT, NETBIOS_HOST_SESSION))
                == NETBIOS_INIT_UNAVAILABLE)
                ShutDown("NETBIOS is not loaded.");
            else {
                iInitNetHostStatus++;
                gbRemoteOn = true;
            }
            break;
        case IDX(NETBIOS_SETUP_CHECK_LOCAL_NAME):
            status =
                !(static_cast<u8>(nb_stat(NETBIOS_HOST_SESSION))
                  & IDX(NETBIOS_SESSION_NAME_REGISTERED));
            if (status)
                iInitNetHostStatus++;
            else
                return 1;
            break;
        case IDX(NETBIOS_SETUP_REGISTER_LOCAL_NAME):
            sprintf(localName, "H2H%d", Random(NETBIOS_RANDOM_NAME_MIN, NETBIOS_RANDOM_NAME_MAX));
            if (static_cast<i16>(nb_sess(IDX(NETBIOS_SESSION_REGISTER), localName))
                == IDX(NETBIOS_RESULT_SUCCESS))
                iInitNetHostStatus++;
            else
                ShutDown("Network initialization failed");
            break;
        case IDX(NETBIOS_SETUP_WAIT_FOR_LOCAL_NAME):
            status = static_cast<u8>(nb_stat(NETBIOS_HOST_SESSION));
            if (status & IDX(NETBIOS_SESSION_NAME_REGISTERED))
                return 1;
            else if (status & IDX(NETBIOS_SESSION_ERROR)) {
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
i8 InitNetGuest(void) {
    char localName[NETBIOS_NAME_BUFFER_SIZE];

    switch (iInitNetGuestStatus) {
        case IDX(NETBIOS_SETUP_INITIALIZE):
            if (static_cast<i16>(nb_init(NETBIOS_SETUP_SESSION_COUNT, NETBIOS_GUEST_SESSION))
                == NETBIOS_INIT_UNAVAILABLE)
                ShutDown("NETBIOS is not loaded.");
            else {
                gbRemoteOn = true;
                iInitNetGuestStatus++;
            }
            break;
        case IDX(NETBIOS_SETUP_CHECK_LOCAL_NAME):
            if (static_cast<u8>(nb_stat(NETBIOS_GUEST_SESSION))
                & IDX(NETBIOS_SESSION_NAME_REGISTERED))
                iInitNetGuestStatus += NETBIOS_REGISTERED_STATE_ADVANCE;
            else
                iInitNetGuestStatus++;
            break;
        case IDX(NETBIOS_SETUP_REGISTER_LOCAL_NAME):
            sprintf(localName, "H2G%d", Random(NETBIOS_RANDOM_NAME_MIN, NETBIOS_RANDOM_NAME_MAX));
            if (static_cast<i16>(nb_sess(IDX(NETBIOS_SESSION_REGISTER), localName))
                == IDX(NETBIOS_RESULT_SUCCESS))
                iInitNetGuestStatus++;
            else
                iNameRetryCount++;
            break;
        case IDX(NETBIOS_SETUP_WAIT_FOR_LOCAL_NAME): {
            i32 status = static_cast<u8>(nb_stat(NETBIOS_GUEST_SESSION));
            i32 namePending = !(status & IDX(NETBIOS_SESSION_NAME_REGISTERED));
            if (namePending) {
                if (status & IDX(NETBIOS_SESSION_ERROR)) {
                    iNameRetryCount++;
                    iInitNetGuestStatus--;
                }
            } else {
                iInitNetGuestStatus++;
            }
            break;
        }
        case IDX(NETBIOS_SETUP_START_RECEIVE):
            if (static_cast<i16>(nb_sess(IDX(NETBIOS_SESSION_RECEIVE_ANY), NETBIOS_HOST_SESSION))
                != IDX(NETBIOS_RESULT_SUCCESS)) {
                sprintf(gText, "Network initialization failed");
                ShutDown(gText);
            }
            return 1;
    }
    return 0;
}

VA(0x004135f0, 0x5f)
i8 WaitForHost(void) {
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    i32 status;

    switch (iWaitForHostStatus) {
        case IDX(NETBIOS_WAIT_START):
            status = static_cast<u8>(nb_stat(NETBIOS_HOST_SESSION)) & IDX(NETBIOS_SESSION_ACTIVE);
            if (status != 0)
                return 1;
            break;
    }
    return 0;
}

VA(0x0041364f, 0xe9)
i8 WaitForGuest(void) {
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    i32 status;

    switch (iWaitForGuestStatus) {
        case IDX(NETBIOS_WAIT_START):
            status =
                static_cast<i16>(nb_sess(IDX(NETBIOS_SESSION_LISTEN_ANY), NETBIOS_GUEST_SESSION));
            if (status == IDX(NETBIOS_RESULT_SUCCESS))
                iWaitForGuestStatus++;
            return 0;
        case IDX(NETBIOS_WAIT_POLL):
            status =
                !(static_cast<u8>(nb_stat(NETBIOS_GUEST_SESSION)) & IDX(NETBIOS_SESSION_ACTIVE));
            if (status) {
                if (KBTickCount() > iLastBroadcastTime + NETBIOS_BROADCAST_INTERVAL) {
                    iLastBroadcastTime = KBTickCount();
                    nb_snd(NETBIOS_HOST_SESSION, 0, 0);
                }
            } else {
                nb_sess(
                    IDX(NETBIOS_SESSION_MOVE),
                    NETBIOS_GUEST_SESSION,
                    NETBIOS_CONNECTED_SESSION,
                    NETBIOS_DETACH_SOURCE_SESSION
                );
                return 1;
            }
    }
    return 0;
}

VA(0x00413738, 0x1ba)
i32 nbnet_init(void) {
    char scratch[NETBIOS_SCRATCH_BUFFER_SIZE];
    i32 unused;

    LogStr("GUON1");
    switch (GameMode) {
        case IDX(NETBIOS_GAME_MODE_HOST):
            giWaitType = IDX(NETBIOS_INITIALIZE_HOST);
            sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(0);
            giWaitType = IDX(NETBIOS_WAIT_FOR_GUEST);
            sprintf(gText, "Waiting On Guest.\n\n  Press 'CANCEL' to abort.");
            LogStr("GUON2");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            LogStr("GUON3");
            if (gbFunctionComplete == 0)
                ShutDown(0);
            LogStr("GUON4");
            break;
        case IDX(NETBIOS_GAME_MODE_GUEST):
            giWaitType = IDX(NETBIOS_INITIALIZE_GUEST);
            sprintf(gText, "Initializing network.\n\n  Press 'CANCEL' to abort.");
            NormalDialog(gText, OLD_MAIN_DIALOG_WAIT, -1, -1, -1, 0, -1, 0, -1, 0);
            if (gbFunctionComplete == 0)
                ShutDown(0);
            giWaitType = IDX(NETBIOS_WAIT_FOR_HOST);
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
// @data-layout-note Retail .data is 0xee5c8+0x188 and places these public
// initialized zeros at +0/+4/+8/+0x4c/+0x90/+0x94 among 15 private literal
// allocations. Candidate .data is 0x186 and groups the public identities at
// +0/+4/+8/+0xc/+0x10/+0x14. Public and unique-payload anchors force the four
// repeated literal spans to 0xee5d4, 0xee5f4, 0xee618, and 0xee638; each span
// is byte-exact against retail. All later private owners have singleton
// relocation proof. The final two retail bytes are zero alignment; do not add
// a fake owner for them.
DATA(0x004ee5c8) i8 iInitNetHostStatus = 0;
DATA(0x004ee5cc) i8 iInitNetGuestStatus = 0;
DATA(0x004ee5d0) i32 iNameRetryCount = 0;
DATA(0x004ee614) i8 iWaitForHostStatus = 0;
DATA(0x004ee658) i8 iWaitForGuestStatus = 0;
DATA(0x004ee65c) i32 iLastBroadcastTime = 0;
