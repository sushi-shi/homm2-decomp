// VC6 SP5 REQUEST source family g88-request-insertnextfile; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/g88-request-insertnextfile/results.json; RVA 0x8e836
// Complete 2 arms; elapsed 1.656927s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1827 / 0b83b728b871977f / d778ce17cfcff435 / 82:82 / 82,0,0,0 / True
// 1 / {'source_family': 'InsertNextFile', 'candidate': 'baseline'} / 1817 / c2223360e218270a / bc6352e85b750a21 / 78:82 / 62,0,14,2 / False

// AXIS source_family
// BEFORE (complete exact span):
// |         while (haveMore) {
// |             if (m_mode == FILE_REQUESTER_MAP_GAME) {
// |                 GetMapHeader(findFileData.cFileName, &header);
// |                 if (header.minHumanPlayers > giNumHumanPlayers
// |                     || header.maxHumanPlayers < giNumHumanPlayers
// |                     || (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// |                         && header.width != giMapSizes[IDX(giMapSizeFilter)]))
// |                     goto InsertNextFile;
// |                 if (!ShowThisMapGame(findFileData.cFileName))
// |                     goto InsertNextFile;
// |             }
// |             if (m_mode == FILE_REQUESTER_MAP) {
// |                 GetMapHeader(findFileData.cFileName, &header);
// |                 if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// |                     && header.width != giMapSizes[IDX(giMapSizeFilter)])
// |                     goto InsertNextFile;
// |                 if (!ShowThisMap(findFileData.cFileName))
// |                     goto InsertNextFile;
// |             }
// |
// |             strcpy(nameBuffer, findFileData.cFileName);
// |             dotPtr = FindLastToken(nameBuffer, '.');
// |             if (dotPtr != NULL) {
// |                 strcpy(extension, dotPtr);
// |                 *dotPtr = 0;
// |             }
// |
// |             for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
// |                 if (strcmpi(nameBuffer, m_fileNames[indexData5].text) < 0) {
// |                     for (moveValue = insertCount; moveValue > indexData5; --moveValue) {
// |                         strcpy(m_fileNames[moveValue].text, m_fileNames[moveValue - 1].text);
// |                         strcpy(m_extensions[moveValue].text, m_extensions[moveValue - 1].text);
// |                     }
// |                     goto InsertName;
// |                 }
// |             }
// |         InsertName:
// |             strcpy(m_fileNames[indexData5].text, nameBuffer);
// |             strcpy(m_extensions[indexData5].text, extension);
// |             ++insertCount;
// |         InsertNextFile:
// |             haveMore = FindNextFile(findHandleWork, &findFileData);
// ALTERNATIVE InsertNextFile (unified source delta from BEFORE):
// | --- before
// | +++ InsertNextFile
// | @@ -1,42 +1,43 @@
// |          while (haveMore) {
// | -            if (m_mode == FILE_REQUESTER_MAP_GAME) {
// | -                GetMapHeader(findFileData.cFileName, &header);
// | -                if (header.minHumanPlayers > giNumHumanPlayers
// | -                    || header.maxHumanPlayers < giNumHumanPlayers
// | -                    || (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | -                        && header.width != giMapSizes[IDX(giMapSizeFilter)]))
// | -                    goto InsertNextFile;
// | -                if (!ShowThisMapGame(findFileData.cFileName))
// | -                    goto InsertNextFile;
// | -            }
// | -            if (m_mode == FILE_REQUESTER_MAP) {
// | -                GetMapHeader(findFileData.cFileName, &header);
// | -                if (giMapSizeFilter != FILE_REQUESTER_MAP_SIZE_ALL
// | -                    && header.width != giMapSizes[IDX(giMapSizeFilter)])
// | -                    goto InsertNextFile;
// | -                if (!ShowThisMap(findFileData.cFileName))
// | -                    goto InsertNextFile;
// | -            }
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
// |
// | -            strcpy(nameBuffer, findFileData.cFileName);
// | -            dotPtr = FindLastToken(nameBuffer, '.');
// | -            if (dotPtr != NULL) {
// | -                strcpy(extension, dotPtr);
// | -                *dotPtr = 0;
// | -            }
// | +                strcpy(nameBuffer, findFileData.cFileName);
// | +                dotPtr = FindLastToken(nameBuffer, '.');
// | +                if (dotPtr != NULL) {
// | +                    strcpy(extension, dotPtr);
// | +                    *dotPtr = 0;
// | +                }
// |
// | -            for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
// | -                if (strcmpi(nameBuffer, m_fileNames[indexData5].text) < 0) {
// | -                    for (moveValue = insertCount; moveValue > indexData5; --moveValue) {
// | -                        strcpy(m_fileNames[moveValue].text, m_fileNames[moveValue - 1].text);
// | -                        strcpy(m_extensions[moveValue].text, m_extensions[moveValue - 1].text);
// | +                for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
// | +                    if (strcmpi(nameBuffer, m_fileNames[indexData5].text) < 0) {
// | +                        for (moveValue = insertCount; moveValue > indexData5; --moveValue) {
// | +                            strcpy(m_fileNames[moveValue].text, m_fileNames[moveValue - 1].text);
// | +                            strcpy(m_extensions[moveValue].text, m_extensions[moveValue - 1].text);
// | +                        }
// | +                        goto InsertName;
// |                      }
// | -                    goto InsertName;
// |                  }
// | -            }
// | -        InsertName:
// | -            strcpy(m_fileNames[indexData5].text, nameBuffer);
// | -            strcpy(m_extensions[indexData5].text, extension);
// | -            ++insertCount;
// | -        InsertNextFile:
// | +            InsertName:
// | +                strcpy(m_fileNames[indexData5].text, nameBuffer);
// | +                strcpy(m_extensions[indexData5].text, extension);
// | +                ++insertCount;
// | +            } while (0);
// |              haveMore = FindNextFile(findHandleWork, &findFileData);
