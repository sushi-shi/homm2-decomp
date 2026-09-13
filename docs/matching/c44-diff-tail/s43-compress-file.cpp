// Parent 9d1aeeeaf; 32 complete arms in 34.84850823506713 seconds.
// Retain trial 1: {'source_shape': 'omit0_cast1', 'candidate': 'baseline'}; size/text/normalized relocs (396, '305e06f1eada5825', 'ac19b9edc1e7e28f').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/s43-compress-file-manifest.json; build/source-variant-batch/s43-compress-file/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void CompressTest(void) {
    char* fromData;
    char* encoded;
    i32 H2_UNUSED(srcCrcCheck);
    i32 H2_UNUSED(unpackedCrc);
    i32 hFile;
    i32l fileSize;
    char diffName[TEST_FILENAME_SIZE];
    i32 H2_UNUSED(srcCrc);
    char* unpackedData;
    i32l compSize;
    i32l H2_UNUSED(plainSize);

    LogStr(const_cast<char*>("C1"));
    strcpy(diffName, "c:\\TEMP\\Z.DIF");
    fileSize = FileSize(diffName);
    fromData = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    encoded = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    unpackedData = static_cast<char*>(
        H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
    );
    LogStr(const_cast<char*>("C2"));
    hFile = open(diffName, _O_BINARY);
    if (hFile == -1)
        FileError(diffName);
    read(hFile, fromData, fileSize);
    LogStr(const_cast<char*>("C3"));
    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    LogStr(const_cast<char*>("C4"));
    close(hFile);
    LogStr(const_cast<char*>("C5"));
    compSize = EncodeData(encoded, fromData, fileSize);
    LogStr(const_cast<char*>("C6"));
    plainSize = DecodeData(unpackedData, encoded, compSize);
    LogStr(const_cast<char*>("C7"));
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
    LogStr(const_cast<char*>("C8"));
}

Option omit0_cast0:

Option omit0_cast1:
--- baseline
+++ omit0_cast1
@@ -11,7 +11,7 @@
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +23,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit1_cast0:
--- baseline
+++ omit1_cast0
@@ -9,7 +9,6 @@
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -35,7 +34,7 @@
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
Option omit1_cast1:
--- baseline
+++ omit1_cast1
@@ -9,9 +9,8 @@
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +22,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit2_cast0:
--- baseline
+++ omit2_cast0
@@ -6,7 +6,6 @@
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);
@@ -29,7 +28,7 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
Option omit2_cast1:
--- baseline
+++ omit2_cast1
@@ -6,12 +6,11 @@
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +22,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit3_cast0:
--- baseline
+++ omit3_cast0
@@ -6,10 +6,8 @@
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -29,13 +27,13 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
Option omit3_cast1:
--- baseline
+++ omit3_cast1
@@ -6,12 +6,10 @@
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit4_cast0:
--- baseline
+++ omit4_cast0
@@ -2,7 +2,6 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
@@ -37,7 +36,7 @@
     LogStr(const_cast<char*>("C6"));
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit4_cast1:
--- baseline
+++ omit4_cast1
@@ -2,7 +2,6 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
@@ -11,7 +10,7 @@
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +22,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit5_cast0:
--- baseline
+++ omit5_cast0
@@ -2,14 +2,12 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -35,9 +33,9 @@
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit5_cast1:
--- baseline
+++ omit5_cast1
@@ -2,16 +2,14 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit6_cast0:
--- baseline
+++ omit6_cast0
@@ -2,11 +2,9 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);
@@ -29,7 +27,7 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
@@ -37,7 +35,7 @@
     LogStr(const_cast<char*>("C6"));
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit6_cast1:
--- baseline
+++ omit6_cast1
@@ -2,16 +2,14 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit7_cast0:
--- baseline
+++ omit7_cast0
@@ -2,14 +2,11 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -29,15 +26,15 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit7_cast1:
--- baseline
+++ omit7_cast1
@@ -2,16 +2,13 @@
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +20,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit8_cast0:
--- baseline
+++ omit8_cast0
@@ -1,7 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
@@ -38,7 +37,7 @@
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit8_cast1:
--- baseline
+++ omit8_cast1
@@ -1,7 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
@@ -11,7 +10,7 @@
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +22,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit9_cast0:
--- baseline
+++ omit9_cast0
@@ -1,7 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
@@ -9,7 +8,6 @@
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -35,10 +33,10 @@
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit9_cast1:
--- baseline
+++ omit9_cast1
@@ -1,7 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
@@ -9,9 +8,8 @@
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit10_cast0:
--- baseline
+++ omit10_cast0
@@ -1,12 +1,10 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);
@@ -29,7 +27,7 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
@@ -38,7 +36,7 @@
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit10_cast1:
--- baseline
+++ omit10_cast1
@@ -1,17 +1,15 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit11_cast0:
--- baseline
+++ omit11_cast0
@@ -1,15 +1,12 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -29,16 +26,16 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit11_cast1:
--- baseline
+++ omit11_cast1
@@ -1,17 +1,14 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
     i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +20,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit12_cast0:
--- baseline
+++ omit12_cast0
@@ -1,8 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
@@ -37,8 +35,8 @@
     LogStr(const_cast<char*>("C6"));
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit12_cast1:
--- baseline
+++ omit12_cast1
@@ -1,8 +1,6 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
@@ -11,7 +9,7 @@
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +21,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit13_cast0:
--- baseline
+++ omit13_cast0
@@ -1,15 +1,12 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -35,10 +32,10 @@
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit13_cast1:
--- baseline
+++ omit13_cast1
@@ -1,17 +1,14 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
     i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +20,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
+    LogStr("C3");
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit14_cast0:
--- baseline
+++ omit14_cast0
@@ -1,12 +1,9 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);
@@ -29,7 +26,7 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
@@ -37,8 +34,8 @@
     LogStr(const_cast<char*>("C6"));
     plainSize = DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit14_cast1:
--- baseline
+++ omit14_cast1
@@ -1,17 +1,14 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
     i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +20,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
+    LogStr("C6");
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
Option omit15_cast0:
--- baseline
+++ omit15_cast0
@@ -1,15 +1,11 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

     LogStr(const_cast<char*>("C1"));
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
@@ -29,16 +25,16 @@
         FileError(diffName);
     read(hFile, fromData, fileSize);
     LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     LogStr(const_cast<char*>("C4"));
     close(hFile);
     LogStr(const_cast<char*>("C5"));
     compSize = EncodeData(encoded, fromData, fileSize);
     LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit15_cast1:
--- baseline
+++ omit15_cast1
@@ -1,17 +1,13 @@
 void CompressTest(void) {
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);
-    i32 H2_UNUSED(unpackedCrc);
     i32 hFile;
     i32l fileSize;
     char diffName[TEST_FILENAME_SIZE];
-    i32 H2_UNUSED(srcCrc);
     char* unpackedData;
     i32l compSize;
-    i32l H2_UNUSED(plainSize);

-    LogStr(const_cast<char*>("C1"));
+    LogStr("C1");
     strcpy(diffName, "c:\\TEMP\\Z.DIF");
     fileSize = FileSize(diffName);
     fromData = static_cast<char*>(
@@ -23,24 +19,24 @@
     unpackedData = static_cast<char*>(
         H2_ALLOC(fileSize + TEST_FILE_BUFFER_EXTRA)
     );
-    LogStr(const_cast<char*>("C2"));
+    LogStr("C2");
     hFile = open(diffName, _O_BINARY);
     if (hFile == -1)
         FileError(diffName);
     read(hFile, fromData, fileSize);
-    LogStr(const_cast<char*>("C3"));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
-    LogStr(const_cast<char*>("C4"));
+    LogStr("C3");
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C4");
     close(hFile);
-    LogStr(const_cast<char*>("C5"));
+    LogStr("C5");
     compSize = EncodeData(encoded, fromData, fileSize);
-    LogStr(const_cast<char*>("C6"));
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    LogStr(const_cast<char*>("C7"));
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
+    LogStr("C6");
+    DecodeData(unpackedData, encoded, compSize);
+    LogStr("C7");
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), fileSize);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), fileSize);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
-    LogStr(const_cast<char*>("C8"));
+    LogStr("C8");
 }
*/
