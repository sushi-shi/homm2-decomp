// VC6 SP5 REQUEST source family r25-request-initialize-roles; parent13e6af40.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/r25-request-initialize-roles/results.json; RVA 0x8e836
// Complete 2 arms; elapsed 1.710768s; input restored.
// Disposition: retain original; every tested alternative changes text.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1827 / 0b83b728b871977f / d778ce17cfcff435 / 82:82 / 82,0,0,0 / True
// 1 / {'source_family': 'clear_header_slots', 'candidate': 'baseline'} / 1827 / 40ef098c82b27b7a / d778ce17cfcff435 / 82:82 / 82,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |
// |     for (indexData5 = 0; indexData5 < m_fileCount; ++indexData5) {
// |         strcpy(
// |             m_fileNames[indexData5].text,
// |             ""
// |         );
// |         strcpy(
// |             m_extensions[indexData5].text,
// |             ""
// |         );
// |     }
// |
// |     insertCount = 0;
// |     sprintf(gText, "%s%s", directory, pattern);
// |     findHandleWork = FindFirstFile(gText, &findFileData);
// |     if (findHandleWork != INVALID_HANDLE_VALUE) {
// |         haveMore = 1;
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
// |         }
// |         FindClose(findHandleWork);
// |     }
// |
// |     if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
// |         for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
// |             sprintf(fullPath, "%s%s", m_fileNames[indexData5].text, m_extensions[indexData5].text);
// |             GetMapHeader(fullPath, &m_mapHeaders[indexData5]);
// |         }
// ALTERNATIVE clear_header_slots (unified source delta from BEFORE):
// | --- before
// | +++ clear_header_slots
// | @@ -1,11 +1,11 @@
// |
// | -    for (indexData5 = 0; indexData5 < m_fileCount; ++indexData5) {
// | +    for (i32 clearSlot = 0; clearSlot < m_fileCount; ++clearSlot) {
// |          strcpy(
// | -            m_fileNames[indexData5].text,
// | +            m_fileNames[clearSlot].text,
// |              ""
// |          );
// |          strcpy(
// | -            m_extensions[indexData5].text,
// | +            m_extensions[clearSlot].text,
// |              ""
// |          );
// |      }
// | @@ -62,7 +62,7 @@
// |      }
// |
// |      if (m_mode == FILE_REQUESTER_MAP_GAME || m_mode == FILE_REQUESTER_MAP) {
// | -        for (indexData5 = 0; indexData5 < insertCount; ++indexData5) {
// | -            sprintf(fullPath, "%s%s", m_fileNames[indexData5].text, m_extensions[indexData5].text);
// | -            GetMapHeader(fullPath, &m_mapHeaders[indexData5]);
// | +        for (i32 headerSlot = 0; headerSlot < insertCount; ++headerSlot) {
// | +            sprintf(fullPath, "%s%s", m_fileNames[headerSlot].text, m_extensions[headerSlot].text);
// | +            GetMapHeader(fullPath, &m_mapHeaders[headerSlot]);
// |          }
