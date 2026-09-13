// Parent 9d1aeeeaf; 32 complete arms in 34.54917413648218 seconds.
// Retain trial 1: {'source_shape': 'omit0_cast1', 'candidate': 'baseline'}; size/text/normalized relocs (288, '578bcfa52e5fbb6e', '76bd2a4c82816058').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/s43-compress-random-manifest.json; build/source-variant-batch/s43-compress-random/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void CompressTest2(void) {
    i32l H2_UNUSED(plainSize);
    char* unpackedData;
    i32l compSize;
    i32 H2_UNUSED(srcCrc);
    i32 dataSz;
    i32 index;
    i32 H2_UNUSED(unpackedCrc);
    char* fromData;
    char* encoded;
    i32 H2_UNUSED(srcCrcCheck);

    dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
    fromData =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    encoded =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    unpackedData =
        static_cast<char*>(
            H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
        );
    for (index = 0; index < dataSz; index++)
        fromData[index] = static_cast<char>(Random(0, 255));
    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    compSize = EncodeData(encoded, fromData, dataSz);
    plainSize = DecodeData(unpackedData, encoded, compSize);
    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
    H2_FREE(fromData);
    H2_FREE(encoded);
    H2_FREE(unpackedData);
}

Option omit0_cast0:

Option omit0_cast1:
--- baseline
+++ omit0_cast1
@@ -24,7 +24,7 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
Option omit1_cast0:
--- baseline
+++ omit1_cast0
@@ -1,5 +1,4 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
@@ -27,7 +26,7 @@
         fromData[index] = static_cast<char>(Random(0, 255));
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
Option omit1_cast1:
--- baseline
+++ omit1_cast1
@@ -1,5 +1,4 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
@@ -24,10 +23,10 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
Option omit2_cast0:
--- baseline
+++ omit2_cast0
@@ -2,7 +2,6 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
@@ -25,7 +24,7 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
Option omit2_cast1:
--- baseline
+++ omit2_cast1
@@ -2,7 +2,6 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
@@ -24,8 +23,8 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
Option omit3_cast0:
--- baseline
+++ omit3_cast0
@@ -1,8 +1,6 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
@@ -25,9 +23,9 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
Option omit3_cast1:
--- baseline
+++ omit3_cast1
@@ -1,8 +1,6 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
@@ -24,10 +22,10 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
Option omit4_cast0:
--- baseline
+++ omit4_cast0
@@ -5,7 +5,6 @@
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -28,7 +27,7 @@
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit4_cast1:
--- baseline
+++ omit4_cast1
@@ -5,7 +5,6 @@
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -24,11 +23,11 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit5_cast0:
--- baseline
+++ omit5_cast0
@@ -1,11 +1,9 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -27,8 +25,8 @@
         fromData[index] = static_cast<char>(Random(0, 255));
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit5_cast1:
--- baseline
+++ omit5_cast1
@@ -1,11 +1,9 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -24,11 +22,11 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit6_cast0:
--- baseline
+++ omit6_cast0
@@ -2,10 +2,8 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -25,10 +23,10 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit6_cast1:
--- baseline
+++ omit6_cast1
@@ -2,10 +2,8 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -24,11 +22,11 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit7_cast0:
--- baseline
+++ omit7_cast0
@@ -1,11 +1,8 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -25,10 +22,10 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit7_cast1:
--- baseline
+++ omit7_cast1
@@ -1,11 +1,8 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
     i32 H2_UNUSED(srcCrcCheck);
@@ -24,11 +21,11 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
     srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
Option omit8_cast0:
--- baseline
+++ omit8_cast0
@@ -8,7 +8,6 @@
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -29,7 +28,7 @@
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit8_cast1:
--- baseline
+++ omit8_cast1
@@ -8,7 +8,6 @@
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +23,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit9_cast0:
--- baseline
+++ omit9_cast0
@@ -1,5 +1,4 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
@@ -8,7 +7,6 @@
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -27,9 +25,9 @@
         fromData[index] = static_cast<char>(Random(0, 255));
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit9_cast1:
--- baseline
+++ omit9_cast1
@@ -1,5 +1,4 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
@@ -8,7 +7,6 @@
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +22,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit10_cast0:
--- baseline
+++ omit10_cast0
@@ -2,13 +2,11 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -25,11 +23,11 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit10_cast1:
--- baseline
+++ omit10_cast1
@@ -2,13 +2,11 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +22,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit11_cast0:
--- baseline
+++ omit11_cast0
@@ -1,14 +1,11 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -25,11 +22,11 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit11_cast1:
--- baseline
+++ omit11_cast1
@@ -1,14 +1,11 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
     i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +21,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
+    DecodeData(unpackedData, encoded, compSize);
     unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit12_cast0:
--- baseline
+++ omit12_cast0
@@ -5,10 +5,8 @@
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -28,8 +26,8 @@
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit12_cast1:
--- baseline
+++ omit12_cast1
@@ -5,10 +5,8 @@
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +22,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit13_cast0:
--- baseline
+++ omit13_cast0
@@ -1,14 +1,11 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -27,9 +24,9 @@
         fromData[index] = static_cast<char>(Random(0, 255));
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit13_cast1:
--- baseline
+++ omit13_cast1
@@ -1,14 +1,11 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
     i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +21,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
+        fromData[index] = Random(0, 255);
     srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit14_cast0:
--- baseline
+++ omit14_cast0
@@ -2,13 +2,10 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -25,11 +22,11 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit14_cast1:
--- baseline
+++ omit14_cast1
@@ -2,13 +2,10 @@
     i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +21,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
     plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit15_cast0:
--- baseline
+++ omit15_cast0
@@ -1,14 +1,10 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -25,11 +21,11 @@
         );
     for (index = 0; index < dataSz; index++)
         fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
Option omit15_cast1:
--- baseline
+++ omit15_cast1
@@ -1,14 +1,10 @@
 void CompressTest2(void) {
-    i32l H2_UNUSED(plainSize);
     char* unpackedData;
     i32l compSize;
-    i32 H2_UNUSED(srcCrc);
     i32 dataSz;
     i32 index;
-    i32 H2_UNUSED(unpackedCrc);
     char* fromData;
     char* encoded;
-    i32 H2_UNUSED(srcCrcCheck);

     dataSz = Random(TEST_RANDOM_SIZE_MIN, TEST_RANDOM_SIZE_MAX);
     fromData =
@@ -24,12 +20,12 @@
             H2_ALLOC(dataSz + TEST_RANDOM_BUFFER_EXTRA)
         );
     for (index = 0; index < dataSz; index++)
-        fromData[index] = static_cast<char>(Random(0, 255));
-    srcCrc = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+        fromData[index] = Random(0, 255);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     compSize = EncodeData(encoded, fromData, dataSz);
-    plainSize = DecodeData(unpackedData, encoded, compSize);
-    unpackedCrc = calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
-    srcCrcCheck = calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
+    DecodeData(unpackedData, encoded, compSize);
+    calc_crc_long(reinterpret_cast<u8*>(unpackedData), dataSz);
+    calc_crc_long(reinterpret_cast<u8*>(fromData), dataSz);
     H2_FREE(fromData);
     H2_FREE(encoded);
     H2_FREE(unpackedData);
*/
