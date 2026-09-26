// C18, complete2x2 native matrix: build/c18-{axes,manifest}.json,
// build/c18-batch/results.json,4/4 in2.60s. Each output independently i32
// plus LPDPID cast, or actual DPID plus direct address. All152 bytes/14 blocks.
// Retained both DPID owners and removed the now-redundant to comparison cast.
// All19 native functions/allocated sections equal after private normalization;
// scoped genuine-retail bytes/sites/ordered targets exact. See C18.md.

void dpProcessMessages(void) {
    DWORD size;
    DPID to;
    i32 H2_UNUSED(i);
    i32 H2_UNUSED(j);  // i and j are unreferenced; retail's frame reserves both slots
    DPID sender;
    i32 receiveResult;

    if (lpIDC == NULL)
        return;
    while (1) {
        size = DP_TRANSPORT_RECEIVE_SIZE;
        receiveResult = lpIDC->Receive(
            &sender,
            &to,
            1,
            rcvBufIn,
            &size
        );
        if (receiveResult == DPERR_NOMESSAGES)
            return;
        if (receiveResult != DP_OK)
            DPSD(receiveResult, RETAIL_FILE, 335);
        if (sender == 0) {
        } else {
            if (to == 0 || to == dcoID)
                dpEvaluateMessage(size, sender);
        }
    }
}
