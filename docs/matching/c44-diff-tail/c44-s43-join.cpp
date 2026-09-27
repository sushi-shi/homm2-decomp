// Parent 9d1aeeeaf; 8 complete arms in 9.422179102897644 seconds.
// Retain trial 1: {'source_shape': 'diff0_out0_literal1', 'candidate': 'baseline'}; size/text/normalized relocs (894, '68647862ce45928b', 'a16244a6834c7ade').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/c44-s43-join-manifest.json; build/source-variant-batch/c44-s43-join/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
    u8* oldBuf = NULL;
    u8 copyFlag;
    u8* diffData = NULL;
    u8* outData = NULL;
    i32 outSize = 0;
    i32 diffLength;
    i32 diffFile;
    i32 copyLength;
    i32 oldSize;
    i32 position;
    i32 joinFile;

    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffLength = FileSize(gText);
    diffData = static_cast<u8*>(H2_ALLOC(diffLength));
    sprintf(gText, "%s%s", ".\\DATA\\", diffName);
    diffFile = open(gText, _O_BINARY);
    if (diffFile == -1)
        FileError(gText);
    read(diffFile, diffData, diffLength);
    close(diffFile);

    outData = static_cast<u8*>(H2_ALLOC(JOIN_BUFFER_SIZE));
    if (diffData[0] == 0) {
        memcpy(outData, diffData + JOIN_HEADER_SIZE, diffLength - JOIN_HEADER_SIZE);
        outSize = diffLength - JOIN_HEADER_SIZE;
    } else {
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        oldSize = FileSize(gText);
        oldBuf = static_cast<u8*>(H2_ALLOC(oldSize));
        sprintf(gText, "%s%s", ".\\DATA\\", oldName);
        diffFile = open(gText, _O_BINARY);
        if (diffFile == -1)
            FileError(gText);
        read(diffFile, oldBuf, oldSize);
        close(diffFile);
        memcpy(outData, oldBuf, oldSize);

        position = JOIN_HEADER_SIZE;
        while (position < diffLength) {
            copyFlag = diffData[position] >> DIFF_COPY_FLAG_SHIFT;
            copyLength = GetSkipCopyLen(diffData, &position);
            if (copyFlag) {
                memcpy(outData + outSize, diffData + position, copyLength);
                outSize += copyLength;
                position += copyLength;
            } else {
                outSize += copyLength;
            }
        }
    }

    sprintf(gText, "%s%s", ".\\DATA\\", joinName);
    joinFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile == -1)
        FileError(gText);
    write(joinFile, outData, outSize);
    close(joinFile);
    LogInt(
        const_cast<char*>("New Join CRC"),
        calc_crc_long(outData, outSize),
        outSize,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE,
        LOG_UNUSED_VALUE
    );

    sprintf(gText, "%s%s", ".\\DATA\\", oldName);
    joinFile = open(gText, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IWRITE);
    if (joinFile == -1)
        FileError(gText);
    write(joinFile, outData, outSize);
    close(joinFile);

    if (oldBuf)
        H2_FREE(oldBuf);
    if (diffData)
        H2_FREE(diffData);
    if (outData)
        H2_FREE(outData);
}

Option diff0_out0_literal0:

Option diff0_out0_literal1:
--- baseline
+++ diff0_out0_literal1
@@ -58,7 +58,7 @@
     write(joinFile, outData, outSize);
     close(joinFile);
     LogInt(
-        const_cast<char*>("New Join CRC"),
+        "New Join CRC",
         calc_crc_long(outData, outSize),
         outSize,
         LOG_UNUSED_VALUE,
Option diff0_out1_literal0:
--- baseline
+++ diff0_out1_literal0
@@ -2,7 +2,7 @@
     u8* oldBuf = NULL;
     u8 copyFlag;
     u8* diffData = NULL;
-    u8* outData = NULL;
+    u8* outData;
     i32 outSize = 0;
     i32 diffLength;
     i32 diffFile;
Option diff0_out1_literal1:
--- baseline
+++ diff0_out1_literal1
@@ -2,7 +2,7 @@
     u8* oldBuf = NULL;
     u8 copyFlag;
     u8* diffData = NULL;
-    u8* outData = NULL;
+    u8* outData;
     i32 outSize = 0;
     i32 diffLength;
     i32 diffFile;
@@ -58,7 +58,7 @@
     write(joinFile, outData, outSize);
     close(joinFile);
     LogInt(
-        const_cast<char*>("New Join CRC"),
+        "New Join CRC",
         calc_crc_long(outData, outSize),
         outSize,
         LOG_UNUSED_VALUE,
Option diff1_out0_literal0:
--- baseline
+++ diff1_out0_literal0
@@ -1,7 +1,7 @@
 void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
     u8* oldBuf = NULL;
     u8 copyFlag;
-    u8* diffData = NULL;
+    u8* diffData;
     u8* outData = NULL;
     i32 outSize = 0;
     i32 diffLength;
Option diff1_out0_literal1:
--- baseline
+++ diff1_out0_literal1
@@ -1,7 +1,7 @@
 void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
     u8* oldBuf = NULL;
     u8 copyFlag;
-    u8* diffData = NULL;
+    u8* diffData;
     u8* outData = NULL;
     i32 outSize = 0;
     i32 diffLength;
@@ -58,7 +58,7 @@
     write(joinFile, outData, outSize);
     close(joinFile);
     LogInt(
-        const_cast<char*>("New Join CRC"),
+        "New Join CRC",
         calc_crc_long(outData, outSize),
         outSize,
         LOG_UNUSED_VALUE,
Option diff1_out1_literal0:
--- baseline
+++ diff1_out1_literal0
@@ -1,8 +1,8 @@
 void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
     u8* oldBuf = NULL;
     u8 copyFlag;
-    u8* diffData = NULL;
-    u8* outData = NULL;
+    u8* diffData;
+    u8* outData;
     i32 outSize = 0;
     i32 diffLength;
     i32 diffFile;
Option diff1_out1_literal1:
--- baseline
+++ diff1_out1_literal1
@@ -1,8 +1,8 @@
 void CreateJoinFile(char* oldName, char* diffName, char* joinName) {
     u8* oldBuf = NULL;
     u8 copyFlag;
-    u8* diffData = NULL;
-    u8* outData = NULL;
+    u8* diffData;
+    u8* outData;
     i32 outSize = 0;
     i32 diffLength;
     i32 diffFile;
@@ -58,7 +58,7 @@
     write(joinFile, outData, outSize);
     close(joinFile);
     LogInt(
-        const_cast<char*>("New Join CRC"),
+        "New Join CRC",
         calc_crc_long(outData, outSize),
         outSize,
         LOG_UNUSED_VALUE,
*/
