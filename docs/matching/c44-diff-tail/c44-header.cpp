// Parent 9d1aeeeaf; 4 complete arms in 5.228898446075618 seconds.
// Retain trial 3: {'source_shape': 'word1_byte1', 'candidate': 'baseline'}; size/text/normalized relocs (263, '82d21c4cc50cc48b', 'e3b0c44298fc1c14').
// All arms compiled; restored; no generator/wall-time truncation, AST mutation or TU-state census.
// Full artifacts: build/c44-header-manifest.json; build/source-variant-batch/c44-header/results.json.
// Each option below is an exact unified diff from the single baseline, including call-preserving omissions.
// See docs/reconstruction/C44-G52-G53-S43-B63.md for semantics, bounds and full verification.
/* Baseline:
void WriteDiffHeaderInfo(u8 cmd, i32 len, u8* buf, i32* pos) {
    u8 flags = 0;
    flags |= cmd << COMMAND_SHIFT;
    if (len > LEN_WORD_MAX) {
        flags |= LEN_WORD_FLAG;
        flags |= (len & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
        u16 word = static_cast<u16>(len & LEN_LOW_MASK);
        buf[*pos] = flags;
        *reinterpret_cast<u16*>(buf + *pos + 1) = word;
        *pos += DIFF_WORD_HEADER_SIZE;
    } else if (len > LEN_BYTE_MAX) {
        flags |= LEN_BYTE_FLAG;
        flags |= (len & LEN_MID_MASK) >> DIFF_BYTE_SHIFT;
        u8 lo = len & LEN_BYTE_MASK;
        buf[*pos] = flags;
        buf[*pos + 1] = lo;
        *pos += DIFF_BYTE_HEADER_SIZE;
    } else {
        flags |= static_cast<u8>(len);
        buf[*pos] = flags;
        (*pos)++;
    }
}

Option word0_byte0:

Option word0_byte1:
--- baseline
+++ word0_byte1
@@ -16,7 +16,7 @@
         buf[*pos + 1] = lo;
         *pos += DIFF_BYTE_HEADER_SIZE;
     } else {
-        flags |= static_cast<u8>(len);
+        flags |= len;
         buf[*pos] = flags;
         (*pos)++;
     }
Option word1_byte0:
--- baseline
+++ word1_byte0
@@ -4,7 +4,7 @@
     if (len > LEN_WORD_MAX) {
         flags |= LEN_WORD_FLAG;
         flags |= (len & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
-        u16 word = static_cast<u16>(len & LEN_LOW_MASK);
+        u16 word = len & LEN_LOW_MASK;
         buf[*pos] = flags;
         *reinterpret_cast<u16*>(buf + *pos + 1) = word;
         *pos += DIFF_WORD_HEADER_SIZE;
Option word1_byte1:
--- baseline
+++ word1_byte1
@@ -4,7 +4,7 @@
     if (len > LEN_WORD_MAX) {
         flags |= LEN_WORD_FLAG;
         flags |= (len & LEN_HIGH_MASK) >> DIFF_WORD_SHIFT;
-        u16 word = static_cast<u16>(len & LEN_LOW_MASK);
+        u16 word = len & LEN_LOW_MASK;
         buf[*pos] = flags;
         *reinterpret_cast<u16*>(buf + *pos + 1) = word;
         *pos += DIFF_WORD_HEADER_SIZE;
@@ -16,7 +16,7 @@
         buf[*pos + 1] = lo;
         *pos += DIFF_BYTE_HEADER_SIZE;
     } else {
-        flags |= static_cast<u8>(len);
+        flags |= len;
         buf[*pos] = flags;
         (*pos)++;
     }
*/
