// Parent 9d1aeeeaf; 2 complete arms in 3.2098246281966567 seconds.
// Retain trial 1: {'source_shape': 'byte_owner', 'candidate': 'baseline'}; size/text/normalized relocs (146, '4b5dc8046cdddf93', '512205d7888f7d96').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/c44-crc-manifest.json; build/source-variant-batch/c44-crc/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
i32 CalcFileCRC(char* file) {
    i32l len = FileSize(file);
    char* blk = static_cast<char*>(H2_ALLOC(len));
    i32 fp = open(file, _O_BINARY);
    if (fp == -1)
        FileError(file);
    read(fp, blk, len);
    i32 checksum = calc_crc_long(reinterpret_cast<u8*>(blk), len);
    close(fp);
    H2_FREE(blk);
    return checksum;
}

Option original:

Option byte_owner:
--- baseline
+++ byte_owner
@@ -1,11 +1,11 @@
 i32 CalcFileCRC(char* file) {
     i32l len = FileSize(file);
-    char* blk = static_cast<char*>(H2_ALLOC(len));
+    u8* blk = static_cast<u8*>(H2_ALLOC(len));
     i32 fp = open(file, _O_BINARY);
     if (fp == -1)
         FileError(file);
     read(fp, blk, len);
-    i32 checksum = calc_crc_long(reinterpret_cast<u8*>(blk), len);
+    i32 checksum = calc_crc_long(blk, len);
     close(fp);
     H2_FREE(blk);
     return checksum;
*/
