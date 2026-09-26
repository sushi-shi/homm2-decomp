// C15, build/c15-{axes,manifest}.json and build/c15-batch/results.json.
// Complete 2/2 structural output-owner arms in 1.58s, both 193 bytes/12 blocks.
// Before: i16 bytesRead[2]; ReadFile(..., reinterpret_cast<LPDWORD>(bytesRead),
// NULL); return bytesRead[0]. Retained DWORD owner and explicit low-word return.
// Full 14-function/all-section native equality. Generic runner string-identity
// rejection affects baseline too; scoped genuine-retail content/target proof
// independently certifies all nine ordered operands, bytes and DIR32 sites.
i16 com_rcv(i16 portIndex, u16 requested, void* buffer) {
    DWORD err;
    DWORD bytesRead;
    COMSTAT status;
    BOOL success;
    u32 n;

    if (s_comPorts[portIndex].handle != INVALID_HANDLE_VALUE) {
        success = ClearCommError(s_comPorts[portIndex].handle, &err, &status);
        if (success == 0)
            ShutdownComError("Clear communications error queue");
        n = requested < status.cbInQue ? requested : status.cbInQue;
        if (n != 0) {
            success = ReadFile(
                s_comPorts[portIndex].handle,
                buffer,
                n,
                &bytesRead,
                NULL
            );
            if (success == 0)
                ShutdownComError("Read communications data");
            return static_cast<i16>(bytesRead);
        }
    }
    return 0;
}
