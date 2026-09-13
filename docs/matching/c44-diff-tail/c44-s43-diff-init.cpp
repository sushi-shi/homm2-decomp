// Parent 9d1aeeeaf; 32 complete arms in 35.67939787916839 seconds.
// Retain trial 1: {'source_shape': 'omit0_literal1', 'candidate': 'baseline'}; size/text/normalized relocs (1363, 'd2303faab5ed8068', '0f0469824bcea086').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/c44-s43-diff-init-manifest.json; build/source-variant-batch/c44-s43-diff-init/results.json.
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

Option omit0_literal0:

Option omit0_literal1:
--- baseline
+++ omit0_literal1
@@ -44,7 +44,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit1_literal0:
--- baseline
+++ omit1_literal0
@@ -23,7 +23,6 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
Option omit1_literal1:
--- baseline
+++ omit1_literal1
@@ -23,7 +23,6 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
@@ -44,7 +43,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit2_literal0:
--- baseline
+++ omit2_literal0
@@ -24,7 +24,6 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
     diffTotal = 0;
Option omit2_literal1:
--- baseline
+++ omit2_literal1
@@ -24,7 +24,6 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
     diffTotal = 0;
@@ -44,7 +43,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit3_literal0:
--- baseline
+++ omit3_literal0
@@ -23,8 +23,6 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
     diffTotal = 0;
Option omit3_literal1:
--- baseline
+++ omit3_literal1
@@ -23,8 +23,6 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
     diffTotal = 0;
@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit4_literal0:
--- baseline
+++ omit4_literal0
@@ -26,7 +26,6 @@
     fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

Option omit4_literal1:
--- baseline
+++ omit4_literal1
@@ -26,7 +26,6 @@
     fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

@@ -44,7 +43,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit5_literal0:
--- baseline
+++ omit5_literal0
@@ -23,10 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

Option omit5_literal1:
--- baseline
+++ omit5_literal1
@@ -23,10 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit6_literal0:
--- baseline
+++ omit6_literal0
@@ -24,9 +24,7 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

Option omit6_literal1:
--- baseline
+++ omit6_literal1
@@ -24,9 +24,7 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit7_literal0:
--- baseline
+++ omit7_literal0
@@ -23,10 +23,7 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

Option omit7_literal1:
--- baseline
+++ omit7_literal1
@@ -23,10 +23,7 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
     diffTotal = 0;
     fullSend = false;

@@ -44,7 +41,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit8_literal0:
--- baseline
+++ omit8_literal0
@@ -27,7 +27,6 @@
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit8_literal1:
--- baseline
+++ omit8_literal1
@@ -27,7 +27,6 @@
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +43,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit9_literal0:
--- baseline
+++ omit9_literal0
@@ -23,11 +23,9 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit9_literal1:
--- baseline
+++ omit9_literal1
@@ -23,11 +23,9 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit10_literal0:
--- baseline
+++ omit10_literal0
@@ -24,10 +24,8 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit10_literal1:
--- baseline
+++ omit10_literal1
@@ -24,10 +24,8 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit11_literal0:
--- baseline
+++ omit11_literal0
@@ -23,11 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit11_literal1:
--- baseline
+++ omit11_literal1
@@ -23,11 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
     joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +41,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit12_literal0:
--- baseline
+++ omit12_literal0
@@ -26,8 +26,6 @@
     fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit12_literal1:
--- baseline
+++ omit12_literal1
@@ -26,8 +26,6 @@
     fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +42,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit13_literal0:
--- baseline
+++ omit13_literal0
@@ -23,11 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit13_literal1:
--- baseline
+++ omit13_literal1
@@ -23,11 +23,8 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
     diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +41,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit14_literal0:
--- baseline
+++ omit14_literal0
@@ -24,10 +24,7 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit14_literal1:
--- baseline
+++ omit14_literal1
@@ -24,10 +24,7 @@
     timeIn = KBTickCount();
     prevData = NULL;
     fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +41,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
Option omit15_literal0:
--- baseline
+++ omit15_literal0
@@ -23,11 +23,7 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
Option omit15_literal1:
--- baseline
+++ omit15_literal1
@@ -23,11 +23,7 @@

     timeIn = KBTickCount();
     prevData = NULL;
-    fullData = NULL;
-    diffOut = NULL;
     oldSize = 0;
-    joinSize = 0;
-    diffTotal = 0;
     fullSend = false;

     if (forceWhole || (iLastDiffSendTo != -1 && iLastDiffSendTo != remotePlayer))
@@ -44,7 +40,7 @@
     read(readFile, fullData, joinSize);
     close(readFile);
     LogInt(
-        const_cast<char*>("Orig Join CRC"),
+        "Orig Join CRC",
         calc_crc_long(fullData, joinSize),
         joinSize,
         LOG_UNUSED_VALUE,
*/
