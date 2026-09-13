// Readability structural matrices: ReadabilityStorage
// Not build input. All matrices use --min-depth 0 --max-depth 0
// --state-trials 0 and --limit equal to the complete exact-axis product.
// Exact flags are the tool audit results, not rounded fuzzy percentages.
// Preserving a pre-existing retail residual is not a new exact closure.
// Macro/helper declarations are in their corresponding owner headers.

// h60n-storage: src/SOURCE/ADVMGR.cpp, RVA 0x5fc9
// Artifacts: build/readability/h60n-storage-{axes,manifest}.json;
//            build/readability/h60n-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 554, "candidate_size": 9310, "choices": {"candidate": "baseline", "h60n-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "e8d928609afc1b2a"}
// {"candidate_relocs": 554, "candidate_size": 9310, "choices": {"candidate": "baseline", "h60n-storage": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "e8d928609afc1b2a"}
// Axis: h60n-storage
// Arm: explicit
IconToBitmap(
                        m_objectIcons[IDX(TILESET_ROUTE)],
                        gpWindowManager->m_screen,
                        s_drawPixelX - ROUTE_DRAW_X_OFFSET,
                        s_drawPixelY + ROUTE_DRAW_Y_OFFSET,
                        (ADVMGR_VISIBILITY_AT(mapX, mapY) - 1) & ROUTE_FRAME_MASK,
                        ICON_DRAW_CLIP,
                        0,
                        0,
                        DRAW_CLIP_WIDTH,
                        DRAW_CLIP_HEIGHT,
                        0
                    )

// Arm: macro
DRAW_ADVENTURE_ICON(m_objectIcons[IDX(TILESET_ROUTE)], s_drawPixelX - ROUTE_DRAW_X_OFFSET, s_drawPixelY + ROUTE_DRAW_Y_OFFSET, (ADVMGR_VISIBILITY_AT(mapX, mapY) - 1) & ROUTE_FRAME_MASK, ICON_DRAW_CLIP)

// h60f-storage: src/SOURCE/ADVMGR.cpp, RVA 0x5fc9
// Artifacts: build/readability/h60f-storage-{axes,manifest}.json;
//            build/readability/h60f-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 554, "candidate_size": 9310, "choices": {"candidate": "baseline", "h60f-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "e8d928609afc1b2a"}
// {"candidate_relocs": 554, "candidate_size": 9310, "choices": {"candidate": "baseline", "h60f-storage": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "acb15066bf798026", "retail_relocs": 554, "text_sha": "e8d928609afc1b2a"}
// Axis: h60f-storage
// Arm: explicit
FlipIconToBitmap(
                                    m_heroIcons[HERO_ICON_FROTH],
                                    gpWindowManager->m_screen,
                                    s_drawPixelX + CELL_PIXELS,
                                    s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL,
                                    s_drawHeroFrame & HERO_FRAME_INDEX_MASK,
                                    ICON_DRAW_CLIP,
                                    0,
                                    0,
                                    DRAW_CLIP_WIDTH,
                                    DRAW_CLIP_HEIGHT,
                                    0
                                )

// Arm: macro
DRAW_FLIPPED_ADVENTURE_ICON(m_heroIcons[HERO_ICON_FROTH], s_drawPixelX + CELL_PIXELS, s_drawPixelY + s_drawHeroYOffset + CELL_LAST_PIXEL, s_drawHeroFrame & HERO_FRAME_INDEX_MASK, ICON_DRAW_CLIP)

// h64i-storage: src/SOURCE/Wsnetwin.cpp, RVA 0xb1cf0
// Artifacts: build/readability/h64i-storage-{axes,manifest}.json;
//            build/readability/h64i-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 160, "candidate_size": 1394, "choices": {"candidate": "baseline", "h64i-storage": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "52b914962befbe67", "retail_relocs": 160, "text_sha": "6b3e75fb71c3dc22"}
// {"candidate_relocs": 160, "candidate_size": 1394, "choices": {"candidate": "baseline", "h64i-storage": "helper"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "52b914962befbe67", "retail_relocs": 160, "text_sha": "6b3e75fb71c3dc22"}
// Axis: h64i-storage
// Arm: explicit
ppDPRcvBuffer = static_cast<u8**>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*)));
    piDPRcvBufferSize = static_cast<i32*>(H2_ALLOC(WS_TRANSPORT_BUFFER_COUNT * sizeof(i32)));
    memset(ppDPRcvBuffer, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(u8*));
    memset(piDPRcvBufferSize, 0, WS_TRANSPORT_BUFFER_COUNT * sizeof(i32));

// Arm: helper
INIT_TRANSPORT_RECEIVE_STORAGE();

// h64e-storage: src/SOURCE/Wsnetwin.cpp, RVA 0xb2653
// Artifacts: build/readability/h64e-storage-{axes,manifest}.json;
//            build/readability/h64e-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 76, "candidate_size": 836, "choices": {"candidate": "baseline", "h64e-storage": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "d981d59f06821f88", "retail_relocs": 76, "text_sha": "e3fbfe405b677e8e"}
// {"candidate_relocs": 76, "candidate_size": 836, "choices": {"candidate": "baseline", "h64e-storage": "helper"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "d981d59f06821f88", "retail_relocs": 76, "text_sha": "e3fbfe405b677e8e"}
// Axis: h64e-storage
// Arm: explicit
ppDPRcvBuffer[iDPRcvBufferHead] = static_cast<u8*>(H2_ALLOC(size - 1));
            memcpy(ppDPRcvBuffer[iDPRcvBufferHead], rcvBufIn + 1, size - 1);
            piDPRcvBufferSize[iDPRcvBufferHead] = size;
            iDPRcvBufferHead = (iDPRcvBufferHead + 1) % WS_TRANSPORT_BUFFER_COUNT;

// Arm: helper
ENQUEUE_TRANSPORT_PACKET(rcvBufIn, size);

// h64d-storage: src/SOURCE/Wsnetwin.cpp, RVA 0xb2535
// Artifacts: build/readability/h64d-storage-{axes,manifest}.json;
//            build/readability/h64d-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 13, "candidate_size": 147, "choices": {"candidate": "baseline", "h64d-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "8212c2827ceb27d0", "retail_relocs": 13, "text_sha": "f226ff33dfce9d45"}
// {"candidate_relocs": 13, "candidate_size": 158, "choices": {"candidate": "baseline", "h64d-storage": "helper"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 158, retail 147", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "8212c2827ceb27d0", "retail_relocs": 13, "text_sha": "b490215ef47837c2"}
// Axis: h64d-storage
// Arm: explicit
if (iDPRcvBufferHead == iDPRcvBufferTail)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE(ppDPRcvBuffer[iDPRcvBufferTail]);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % WS_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);

// Arm: helper
return CopyNextTransportPacket(data, size);

// h64f-storage: src/SOURCE/Wsnetwin.cpp, RVA 0xb2262
// Artifacts: build/readability/h64f-storage-{axes,manifest}.json;
//            build/readability/h64f-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 19, "candidate_size": 179, "choices": {"candidate": "baseline", "h64f-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "a096f1c8bee1fe22", "retail_relocs": 19, "text_sha": "43900dc4236ea0ea"}
// {"candidate_relocs": 19, "candidate_size": 179, "choices": {"candidate": "baseline", "h64f-storage": "helper"}, "exact": true, "exact_rejections": [], "reloc_sha": "a096f1c8bee1fe22", "retail_relocs": 19, "text_sha": "43900dc4236ea0ea"}
// Axis: h64f-storage
// Arm: explicit
if (ppDPRcvBuffer != NULL)
        H2_FREE(ppDPRcvBuffer);
    ppDPRcvBuffer = NULL;
    if (piDPRcvBufferSize != NULL)
        H2_FREE(piDPRcvBufferSize);
    piDPRcvBufferSize = NULL;

// Arm: helper
DisposeTransportReceiveStorage();

// h65-storage: src/SOURCE/Wsnetwin.cpp, RVA 0xb2315
// Artifacts: build/readability/h65-storage-{axes,manifest}.json;
//            build/readability/h65-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 28, "candidate_size": 467, "choices": {"candidate": "baseline", "h65-storage": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "5f0c3dee54d22e13", "retail_relocs": 28, "text_sha": "06ef1283d49d5a8d"}
// {"candidate_relocs": 28, "candidate_size": 471, "choices": {"candidate": "baseline", "h65-storage": "inline"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0", "target size is not exact: candidate 471, retail 467", "ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "352308a4e8ba8119", "retail_relocs": 28, "text_sha": "99b5dad142e90b47"}
// Axis: h65-storage
// Arm: explicit
u8* packetBuffer = static_cast<u8*>(H2_ALLOC(size + 1));
    i32 attemptCount;
    struct sockaddr_in remote;
    i32 netPlayer;
    i32 error;

    packetBuffer[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(packetBuffer + 1, data, size);

// Arm: inline
u8* packetBuffer = AllocTransportMessage(type, size, data);
    i32 attemptCount;
    struct sockaddr_in remote;
    i32 netPlayer;
    i32 error;

// h66-storage: src/SOURCE/comwin.cpp, RVA 0x32e9e
// Artifacts: build/readability/h66-storage-{axes,manifest}.json;
//            build/readability/h66-storage-matrix/results.json
// Complete: 3/3
// {"candidate_relocs": 16, "candidate_size": 233, "choices": {"candidate": "baseline", "h66-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "285ba4fd35423fc4", "retail_relocs": 16, "text_sha": "31906d46d8a742fe"}
// {"candidate_relocs": 16, "candidate_size": 233, "choices": {"candidate": "baseline", "h66-storage": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "285ba4fd35423fc4", "retail_relocs": 16, "text_sha": "31906d46d8a742fe"}
// {"candidate_relocs": 16, "candidate_size": 233, "choices": {"candidate": "baseline", "h66-storage": "inline"}, "exact": false, "exact_rejections": ["unrounded objdiff score is not exactly 100.0"], "reloc_sha": "285ba4fd35423fc4", "retail_relocs": 16, "text_sha": "16777d5442252337"}
// Axis: h66-storage
// Arm: explicit
while ((node = pop_node(&s_comPorts[portIndex].normalQueue)) != NULL)
            H2_FREE(node);

// Arm: macro
FREE_NODE_QUEUE(node, &s_comPorts[portIndex].normalQueue);

// Arm: inline
FreeNodeQueue(&s_comPorts[portIndex].normalQueue);

// h70r-storage: src/BASE/RESMGR.cpp, RVA 0xb8d80
// Artifacts: build/readability/h70r-storage-{axes,manifest}.json;
//            build/readability/h70r-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 3, "candidate_size": 89, "choices": {"candidate": "baseline", "h70r-storage": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "36989fa7ba0c69eb", "retail_relocs": 3, "text_sha": "b96360d52af0a6aa"}
// {"candidate_relocs": 3, "candidate_size": 89, "choices": {"candidate": "baseline", "h70r-storage": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "36989fa7ba0c69eb", "retail_relocs": 3, "text_sha": "b96360d52af0a6aa"}
// Axis: h70r-storage
// Arm: explicit
    i32 H2_UNUSED(result) = read(m_aggregateFd[m_curAggregate], &value, sizeof(value));

// Arm: macro
    i32 H2_UNUSED(result) = READ_FILE_VALUE(m_aggregateFd[m_curAggregate], value);

// h70w-storage: src/BASE/Misc.cpp, RVA 0xc00a0
// Artifacts: build/readability/h70w-storage-{axes,manifest}.json;
//            build/readability/h70w-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 11, "candidate_size": 681, "choices": {"candidate": "baseline", "h70w-storage": "explicit"}, "exact": true, "exact_rejections": [], "reloc_sha": "d1ed23be264c4f57", "retail_relocs": 11, "text_sha": "f4e6000c0cd8bd76"}
// {"candidate_relocs": 11, "candidate_size": 681, "choices": {"candidate": "baseline", "h70w-storage": "macro"}, "exact": true, "exact_rejections": [], "reloc_sha": "d1ed23be264c4f57", "retail_relocs": 11, "text_sha": "f4e6000c0cd8bd76"}
// Axis: h70w-storage
// Arm: explicit
write(fd, &pcxHdr, sizeof(pcxHdr))

// Arm: macro
WRITE_FILE_VALUE(fd, pcxHdr)

// h70k-storage: src/SOURCE/KB.cpp, RVA 0x6bb0e
// Artifacts: build/readability/h70k-storage-{axes,manifest}.json;
//            build/readability/h70k-storage-matrix/results.json
// Complete: 2/2
// {"candidate_relocs": 27, "candidate_size": 1029, "choices": {"candidate": "baseline", "h70k-storage": "explicit"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "4d63cc04f5adde61", "retail_relocs": 27, "text_sha": "f49e399e44e04abb"}
// {"candidate_relocs": 27, "candidate_size": 1029, "choices": {"candidate": "baseline", "h70k-storage": "macro"}, "exact": false, "exact_rejections": ["ordered relocation offsets/types/identities/addends differ from retail"], "reloc_sha": "4d63cc04f5adde61", "retail_relocs": 27, "text_sha": "f49e399e44e04abb"}
// Axis: h70k-storage
// Arm: explicit
write(file_c, &entries_a[entry_a], sizeof(HighScoreEntry))

// Arm: macro
WRITE_FILE_VALUE(file_c, entries_a[entry_a])

// Disposition: see docs/readability/implementation.md for retained subsets,
// caller boundaries and the combined original-snapshot object comparison.

// Rejected inline declarations, present during these matrices and removed afterward.
// dpnetwin owner:
inline i16 CopyNextTransportPacket(void* data, u32& size) {
    if (iDPRcvBufferHead == iDPRcvBufferTail)
        return 0;
    size = piDPRcvBufferSize[iDPRcvBufferTail];
    memcpy(data, ppDPRcvBuffer[iDPRcvBufferTail], size);
    H2_FREE(ppDPRcvBuffer[iDPRcvBufferTail]);
    iDPRcvBufferTail = (iDPRcvBufferTail + 1) % DP_TRANSPORT_BUFFER_COUNT;
    return static_cast<i16>(size);
}
inline u8* AllocTransportMessage(H2_ENUM_PARAM(NetworkPacketType, u8) type, u16 size, void* data) {
    u8* message = static_cast<u8*>(H2_ALLOC(size + 1));
    message[0] = static_cast<u8>(type);
    if (size != 0)
        memcpy(message + 1, data, size);
    return message;
}
// comwin owner (the macro preserving the caller's node variable is retained):
inline void FreeNodeQueue(tag_Anchor* anchor) {
    tag_Node* node;
    while ((node = pop_node(anchor)) != NULL)
        H2_FREE(node);
}
