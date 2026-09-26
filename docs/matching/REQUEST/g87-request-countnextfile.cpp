// VC6 SP5 REQUEST source family g87-request-countnextfile; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g87-request-countnextfile/results.json; RVA 0x8e836
// Complete 2 arms; elapsed 1.617053s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1827 / 0b83b728b871977f / d778ce17cfcff435 / 82:82 / 82,0,0,0 / True
// 1 / {'source_family': 'CountNextFile', 'candidate': 'baseline'} / 1810 / 3a8a6436ba32dc82 / f96f07f8c98c3401 / 78:82 / 70,0,6,2 / False

// AXIS source_family
// BEFORE (complete exact span):
// |         while (haveMore) {
// |             if (m_mode == FILE_REQUESTER_MAP_GAME) {
// |                 GetMapHeader(findFileData.cFileName, &header);
// |                 if (header.minHumanPlayers > giNumHumanPlayers
// |                     || header.maxHumanPlayers < giNumHumanPlayers
// |                     || (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// |                         && header.width != giMapSizes[IDX(giMapSizeFilter)]))
// |                     goto CountNextFile;
// |                 if (!ShowThisMapGame(findFileData.cFileName))
// |                     goto CountNextFile;
// |             }
// |             if (m_mode == FILE_REQUESTER_MAP) {
// |                 GetMapHeader(findFileData.cFileName, &header);
// |                 if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// |                     && header.width != giMapSizes[IDX(giMapSizeFilter)])
// |                     goto CountNextFile;
// |                 if (!ShowThisMap(findFileData.cFileName))
// |                     goto CountNextFile;
// |             }
// |             ++m_fileCount;
// |         CountNextFile:
// |             haveMore = FindNextFile(findHandleWork, &findFileData);
// ALTERNATIVE CountNextFile (unified source delta from BEFORE):
// | --- before
// | +++ CountNextFile
// | @@ -1,22 +1,23 @@
// |          while (haveMore) {
// | -            if (m_mode == FILE_REQUESTER_MAP_GAME) {
// | -                GetMapHeader(findFileData.cFileName, &header);
// | -                if (header.minHumanPlayers > giNumHumanPlayers
// | -                    || header.maxHumanPlayers < giNumHumanPlayers
// | -                    || (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | -                        && header.width != giMapSizes[IDX(giMapSizeFilter)]))
// | -                    goto CountNextFile;
// | -                if (!ShowThisMapGame(findFileData.cFileName))
// | -                    goto CountNextFile;
// | -            }
// | -            if (m_mode == FILE_REQUESTER_MAP) {
// | -                GetMapHeader(findFileData.cFileName, &header);
// | -                if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | -                    && header.width != giMapSizes[IDX(giMapSizeFilter)])
// | -                    goto CountNextFile;
// | -                if (!ShowThisMap(findFileData.cFileName))
// | -                    goto CountNextFile;
// | -            }
// | -            ++m_fileCount;
// | -        CountNextFile:
// | +            do {
// | +                if (m_mode == FILE_REQUESTER_MAP_GAME) {
// | +                    GetMapHeader(findFileData.cFileName, &header);
// | +                    if (header.minHumanPlayers > giNumHumanPlayers
// | +                        || header.maxHumanPlayers < giNumHumanPlayers
// | +                        || (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | +                            && header.width != giMapSizes[IDX(giMapSizeFilter)]))
// | +                        break;
// | +                    if (!ShowThisMapGame(findFileData.cFileName))
// | +                        break;
// | +                }
// | +                if (m_mode == FILE_REQUESTER_MAP) {
// | +                    GetMapHeader(findFileData.cFileName, &header);
// | +                    if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | +                        && header.width != giMapSizes[IDX(giMapSizeFilter)])
// | +                        break;
// | +                    if (!ShowThisMap(findFileData.cFileName))
// | +                        break;
// | +                }
// | +                ++m_fileCount;
// | +            } while (0);
// |              haveMore = FindNextFile(findHandleWork, &findFileData);
