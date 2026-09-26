// Parent 9d1aeeeaf; 16 complete arms in 17.76323782838881 seconds.
// Retain trial 0: {'source_shape': 'break0_unused0_time0_match0', 'candidate': 'baseline'}; size/text/normalized relocs (1363, 'd2303faab5ed8068', '0f0469824bcea086').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/g53-s43-diff-manifest.json; build/source-variant-batch/g53-s43-diff/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void CreateDiffFile(
    char* oldName,
    char* joinName,
    char* diffName,
    i32 remotePlayer,
    i32 forceWhole
) {
    i32 joinSize;
    u8* fullData;
    i32 H2_UNUSED(unusedVal);
    i32 inFd;
    i32l H2_UNUSED(timeIn);
    i32 matchLen;
    u8* prevData;
    i32 diffTotal;
    i32 readFile;
    i32 length;
    u8* diffOut;
    i32 oldSize;
    b32 fullSend;
    i32 destFile;
    i32 position;

    timeIn = KBTickCount();
    prevData = NULL;
    fullData = NULL;
    diffOut = NULL;
    oldSize = 0;
    joinSize = 0;
    diffTotal = 0;
    fullSend = false;

    if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
        fullSend = true;
    iLastDiffSendTo = remotePlayer;

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinSize = FileSize(gText);
    fullData = static_cast<u8*>(H2_ALLOC(joinSize));
    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    readFile = open(gText, _O_BINARY);
    if (readFile == -1)
        FileError(gText);
    read(readFile, fullData, joinSize);
    close(readFile);
    LogInt(
        const_cast<char*>("Orig Join CRC"),
        calc_crc_long(fullData, joinSize),
        joinSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    if (!forceWhole) {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        prevData = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        inFd = open(gText, _O_BINARY);
        if (inFd == -1)
            FileError(gText);
        read(inFd, prevData, oldSize);
        close(inFd);
    }

    diffOut = static_cast<u8*>(H2_ALLOC((oldSize > joinSize ? oldSize : joinSize) + DIFF_BUFFER_EXTRA));
    if (fullSend) {
        diffOut[0] = 0;
        diffOut[1] = 0;
        memcpy(diffOut + JOIN_HEADER_SIZE, fullData, joinSize);
        diffTotal = joinSize + JOIN_HEADER_SIZE;
    } else {
        diffOut[0] = 1;
        diffOut[1] = 0;
        diffTotal = JOIN_HEADER_SIZE;
        position = 0;
        length = 0;
        matchLen = length;
        while (1) {
            if (position + length >= oldSize || position + length >= joinSize) {
                length = oldSize - position;
                WriteDiffHeaderInfo(1, length, diffOut, &diffTotal);
                memcpy(diffOut + diffTotal, fullData + position, length);
                diffTotal += length;
                position += length;
                length = 0;
                goto Finish;
            }
            if (*(prevData + position + length) == *(fullData + position + length)) {
                matchLen = 1;
                while (position + length + matchLen < oldSize
                       && position + length + matchLen < joinSize
                       && *(prevData + position + length + matchLen)
                              == *(fullData + position + length + matchLen))
                    matchLen++;
                if (matchLen <= DIFF_MAX_SHORT_MATCH) {
                    length += matchLen;
                    matchLen = 0;
                } else {
                    if (length != 0) {
                        WriteDiffHeaderInfo(1, length, diffOut, &diffTotal);
                        memcpy(diffOut + diffTotal, fullData + position, length);
                        diffTotal += length;
                        position += length;
                        length = 0;
                    }
                    WriteDiffHeaderInfo(0, matchLen, diffOut, &diffTotal);
                    position += matchLen;
                    matchLen = 0;
                }
            } else {
                while (position + length < oldSize && position + length < joinSize
                       && *(prevData + position + length)
                              != *(fullData + position + length))
                    length++;
            }
        }
    }

Finish:
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (destFile == -1)
        FileError(gText);
    write(destFile, diffOut, diffTotal);
    close(destFile);

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (destFile == -1)
        FileError(gText);
    write(destFile, fullData, joinSize);
    close(destFile);

    if (prevData != NULL)
        H2_FREE(prevData);
    if (fullData != NULL)
        H2_FREE(fullData);
    if (diffOut != NULL)
        H2_FREE(diffOut);
    return;
}

Option break0_unused0_time0_match0:

Option break0_unused0_time0_match1:
--- baseline
+++ break0_unused0_time0_match1
@@ -78,7 +78,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
Option break0_unused0_time1_match0:
--- baseline
+++ break0_unused0_time1_match0
@@ -9,7 +9,6 @@
     u8* fullData;
     i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +20,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
Option break0_unused0_time1_match1:
--- baseline
+++ break0_unused0_time1_match1
@@ -9,7 +9,6 @@
     u8* fullData;
     i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +20,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -78,7 +77,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
Option break0_unused1_time0_match0:
--- baseline
+++ break0_unused1_time0_match0
@@ -7,7 +7,6 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
     i32l H2_UNUSED(timeIn);
     i32 matchLen;
Option break0_unused1_time0_match1:
--- baseline
+++ break0_unused1_time0_match1
@@ -7,7 +7,6 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
     i32l H2_UNUSED(timeIn);
     i32 matchLen;
@@ -78,7 +77,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
Option break0_unused1_time1_match0:
--- baseline
+++ break0_unused1_time1_match0
@@ -7,9 +7,7 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +19,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
Option break0_unused1_time1_match1:
--- baseline
+++ break0_unused1_time1_match1
@@ -7,9 +7,7 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +19,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -78,7 +76,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
Option break1_unused0_time0_match0:
--- baseline
+++ break1_unused0_time0_match0
@@ -87,7 +87,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +120,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused0_time0_match1:
--- baseline
+++ break1_unused0_time0_match1
@@ -78,7 +78,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
@@ -87,7 +86,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +119,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused0_time1_match0:
--- baseline
+++ break1_unused0_time1_match0
@@ -9,7 +9,6 @@
     u8* fullData;
     i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +20,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -87,7 +86,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +119,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused0_time1_match1:
--- baseline
+++ break1_unused0_time1_match1
@@ -9,7 +9,6 @@
     u8* fullData;
     i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +20,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -78,7 +77,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
@@ -87,7 +85,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +118,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused1_time0_match0:
--- baseline
+++ break1_unused1_time0_match0
@@ -7,7 +7,6 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
     i32l H2_UNUSED(timeIn);
     i32 matchLen;
@@ -87,7 +86,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +119,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused1_time0_match1:
--- baseline
+++ break1_unused1_time0_match1
@@ -7,7 +7,6 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
     i32l H2_UNUSED(timeIn);
     i32 matchLen;
@@ -78,7 +77,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
@@ -87,7 +85,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +118,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused1_time1_match0:
--- baseline
+++ break1_unused1_time1_match0
@@ -7,9 +7,7 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +19,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -87,7 +85,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +118,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
Option break1_unused1_time1_match1:
--- baseline
+++ break1_unused1_time1_match1
@@ -7,9 +7,7 @@
 ) {
     i32 joinSize;
     u8* fullData;
-    i32 H2_UNUSED(unusedVal);
     i32 inFd;
-    i32l H2_UNUSED(timeIn);
     i32 matchLen;
     u8* prevData;
     i32 diffTotal;
@@ -21,7 +19,7 @@
     i32 destFile;
     i32 position;

-    timeIn = KBTickCount();
+    KBTickCount();
     prevData = NULL;
     fullData = NULL;
     diffOut = NULL;
@@ -78,7 +76,6 @@
         diffTotal = JOIN_HEADER_SIZE;
         position = 0;
         length = 0;
-        matchLen = length;
         while (1) {
             if (position + length >= oldSize || position + length >= joinSize) {
                 length = oldSize - position;
@@ -87,7 +84,7 @@
                 diffTotal += length;
                 position += length;
                 length = 0;
-                goto Finish;
+                break;
             }
             if (*(prevData + position + length) == *(fullData + position + length)) {
                 matchLen = 1;
@@ -120,7 +117,6 @@
         }
     }

-Finish:
     sprintf(gText, "%s%s", ".\\DATA\\", diffName);
     destFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
     if (destFile == -1)
*/
