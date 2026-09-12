// V01: 2/2 exit-cleanup arms, build/v01-{axes,manifest}.json,
// build/v01-batch/results.json, 1.86s. Retail/current1223 bytes/31 blocks.
// This all-exit va_end candidate:1254 bytes,31 blocks,5 size-only blocks,
// 98.106930% fuzzy. REJECTED: x86 SDK va_end assigns NULL to args and emits
// stores absent from retail. Keep original (candidate below minus va_end
// statements/braces), whose1223 bytes and all83 ordered targets independently
// pass check-function.py. No fake no-op macro or state declarations retained.

extern "C" u16 __cdecl
nb_sess(H2_ENUM_PARAM(NetbiosSessionOperation, i16) operation, ...) {
    i32 oldsess;
    i32 destinationSession;
    i32 detach;
    NetbiosControlBlock controlBlock;
    char* peer;
    va_list args;
    H2_ENUM_STORAGE(NetbiosResult, i16) rc;

    va_start(args, operation);
    switch (operation) {
        case NETBIOS_SESSION_REGISTER:
            peer = va_arg(args, char*);
            gNetStatus[gNbMaxSess] &= ~NETBIOS_SESSION_ERROR;
            nb_format_name(peer, gNbNameBuf[gNbMaxSess].bytes);
            memset(&gNbSessNcb[gNbMaxSess], 0, sizeof(NetbiosControlBlock));
            memcpy(gNbSessNcb[gNbMaxSess].name, gNbNameBuf[gNbMaxSess].bytes, NETBIOS_NAME_SIZE);
            gNbSessNcb[gNbMaxSess].command =
                NETBIOS_COMMAND_ADD_NAME | NETBIOS_COMMAND_ASYNC;
            gNbSessNcb[gNbMaxSess].postRoutine = nb_add_name_done;
            gNbSessNcb[gNbMaxSess].commandComplete = NETBIOS_RESULT_PENDING;
            gNbSessNcb[gNbMaxSess].adapterNumber = gNetbiosLana;
            rc = Netbios(&gNbSessNcb[gNbMaxSess]);
            break;

        case NETBIOS_SESSION_RECEIVE_ANY: {
            destinationSession = va_arg(args, i32);
            if (gNbSessNcb[destinationSession].commandComplete == NETBIOS_RESULT_PENDING) {
                switch (gNbSessNcb[destinationSession].command & ~NETBIOS_COMMAND_ASYNC) {
                    case NETBIOS_COMMAND_CALL:
                    case NETBIOS_COMMAND_RECEIVE_BROADCAST:
                        { va_end(args); return 0; }
                    default:
                        break;
                }
                memset(&controlBlock, 0, sizeof(controlBlock));
                controlBlock.command = NETBIOS_COMMAND_CANCEL;
                controlBlock.adapterNumber = gNetbiosLana;
                controlBlock.buffer = &gNbSessNcb[destinationSession];
                Netbios(&controlBlock);
            }
            rc = nb_recv_any(destinationSession);
            break;
        }

        case NETBIOS_SESSION_CALL:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            nb_format_name(peer, gNbNameBuf[destinationSession].bytes);
            rc = nb_call(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case NETBIOS_SESSION_LISTEN_ANY:
            destinationSession = va_arg(args, i32);
            nb_snd(gNbMaxSess, 0, NULL);
            rc = nb_listen(destinationSession, gNbListenName);
            break;

        case NETBIOS_SESSION_LISTEN:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            nb_format_name(peer, gNbNameBuf[destinationSession].bytes);
            rc = nb_listen(destinationSession, gNbNameBuf[destinationSession].bytes);
            break;

        case NETBIOS_SESSION_MOVE:
            oldsess = va_arg(args, i32);
            destinationSession = va_arg(args, i32);
            detach = va_arg(args, i32);
            if (oldsess == gNbMaxSess)
                gNbMaxSess = static_cast<u8>(destinationSession);
            if (gNbSessLsn[oldsess] == NETBIOS_INVALID_ID)
                { va_end(args); return 0; }
            gNbSessLsn[destinationSession] = gNbSessLsn[oldsess];
            gNetStatus[destinationSession] = gNetStatus[oldsess];
            memcpy(
                gNbNameBuf[destinationSession].bytes,
                gNbNameBuf[oldsess].bytes,
                NETBIOS_NAME_SIZE
            );
            nb_arm_recv(destinationSession);
            if (detach != 0) {
                gNbSessLsn[oldsess] = NETBIOS_INVALID_ID;
                gNetStatus[oldsess] = 0;
                memset(gNbNameBuf[oldsess].bytes, 0, NETBIOS_NAME_SIZE);
            }
            rc = 0;
            break;

        case NETBIOS_SESSION_CLOSE:
            destinationSession = va_arg(args, i32);
            if (gNbSessNcb[destinationSession].commandComplete == NETBIOS_RESULT_PENDING) {
                memset(&controlBlock, 0, sizeof(controlBlock));
                controlBlock.command = NETBIOS_COMMAND_CANCEL;
                controlBlock.adapterNumber = gNetbiosLana;
                controlBlock.buffer = &gNbSessNcb[destinationSession];
                Netbios(&controlBlock);
            }
            nb_close_session(destinationSession);
            rc = 0;
            break;

        case NETBIOS_SESSION_CLEAR_CONNECTED:
            destinationSession = va_arg(args, i32);
            gNetStatus[destinationSession] &= ~NETBIOS_SESSION_CONNECTED;
            rc = 0;
            break;

        case NETBIOS_SESSION_GET_NAME:
            destinationSession = va_arg(args, i32);
            peer = va_arg(args, char*);
            memcpy(peer, gNbNameBuf[destinationSession].bytes, NETBIOS_NAME_SIZE);
            rc = 0;
            break;

        default:
            { va_end(args); return 1; }
    }
    if (rc == NETBIOS_RESULT_PENDING)
        rc = 0;
    va_end(args); return IDX(rc);
}
