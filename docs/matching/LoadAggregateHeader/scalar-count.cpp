// S01: complete four-arm native matrix, build/s01-{axes,manifest}.json;
// build/s01-batch/results.json, 4/4 complete in3.30s, no TU-state probes.
// Structural count ownership {i16[2], i16} x spelling {fpCountBuffer,fileCount}.
// Both fpCountBuffer arms:312 bytes,6 exact blocks,100% fuzzy.
// Both fileCount arms:312 bytes,6-block topology,99.948456% fuzzy; not retained.
// The runner rejected retail exactness even for its baseline because literal
// identities differ. Independent scoped target/C-string-byte proof passes all
//14 relocations. Full34-function and all-allocated-section before/after proof
// passes after removing the obsolete enum too. See docs/reconstruction/S01.md.
// Retained structural arm follows; no synthetic state or padding survives.
i32 resourceManager::LoadAggregateHeader(H2_CONST char* aggregateName) {
    i16 fpCountBuffer;
    i32 aggregateFp;
    u32 directoryBytes;
    if (m_numAggregates >= RESOURCE_MANAGER_AGGREGATE_LIMIT) {
        sprintf(
            gText,
            "Only %d .AGG files can be used at once.",
            RESOURCE_MANAGER_AGGREGATE_LIMIT
        );
        ShutDown(gText);
        return LOAD_ERROR;
    }
    aggregateFp = open(aggregateName, BINARY_OPEN_MODE);
    if (aggregateFp == INVALID_FILE) {
        sprintf(
            gText,
            /* Не могу открыть файл: %s */
            "\xcd\xe5 \xec\xee\xe3\xf3 \xee\xf2\xea\xf0\xfb\xf2\xfc \xf4\xe0\xe9\xeb: %s",
            aggregateName
        );
        ShutDown(gText);
        return LOAD_ERROR;
    }
    m_curAggregate = m_numAggregates;
    m_numAggregates = m_numAggregates + 1;
    m_aggregateFd[m_curAggregate] = aggregateFp;
    read(m_aggregateFd[m_curAggregate], &fpCountBuffer, sizeof(i16));
    m_aggregateEntryCount[m_curAggregate] = fpCountBuffer;
    directoryBytes = m_aggregateEntryCount[m_curAggregate] * ENTRY_BYTES;
    m_aggregateDir[m_curAggregate] = static_cast<aggEntry*>(H2_ALLOC(directoryBytes));
    read(m_aggregateFd[m_curAggregate], m_aggregateDir[m_curAggregate], directoryBytes);
    return LOAD_SUCCESS;
}
