// S22, build/s22-{axes,manifest}.json and build/s22-batch/results.json.
// Complete 2/2 structural array/scalar arms in 2.14s, both 51 bytes/4 blocks.
// Before: u8 value[4]; com_rcv(0,1,value); return value[0].
// Retained below: one real byte. Full 18-function/all-section native equality;
// genuine-retail checker certifies bytes/size and the only ordered relocation.
i32 read_byte(void) {
    u8 value;
    i32 received = com_rcv(0, 1, &value);
    if (received == 1)
        return value;
    else
        return -1;
}
