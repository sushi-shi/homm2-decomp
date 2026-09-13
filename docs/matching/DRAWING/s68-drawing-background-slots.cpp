// VC6 SP5 source family s68-drawing-background-slots; parent aab81af5.
// Source record only: each alternative is relative to its own BEFORE, not cumulative.
// No cross-family or TU-state census is claimed.
// build/source-variant-batch/s68-drawing-background-slots/results.json; RVA 0x38d17
// Complete 4 arms; elapsed 2.879620s; source restored.
// Disposition: retain original; every tested nonoriginal arm changes function text.
// Raw/normalized relocation hashes can reflect private label counter spelling;
// final proof compares actual ordered targets/sites and the complete claimed byte span.
// UpdateMouseGrid has an unchanged EH/FS site-model gap, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'unusedBackgroundHeight': 'keep', 'unusedBackgroundSide': 'keep', 'candidate': 'baseline'} / 833 / 18da8437dfebb235 / d88dc4b4cf1216f5 / 21:21 / 21,0,0,0 / True
// 1 / {'unusedBackgroundHeight': 'keep', 'unusedBackgroundSide': 'remove', 'candidate': 'baseline'} / 833 / f1244b35173f7ba1 / d88dc4b4cf1216f5 / 21:21 / 21,0,0,0 / True
// 2 / {'unusedBackgroundHeight': 'remove', 'unusedBackgroundSide': 'keep', 'candidate': 'baseline'} / 833 / f1244b35173f7ba1 / d88dc4b4cf1216f5 / 21:21 / 21,0,0,0 / True
// 3 / {'unusedBackgroundHeight': 'remove', 'unusedBackgroundSide': 'remove', 'candidate': 'baseline'} / 833 / 7956a08b882ad5a9 / d88dc4b4cf1216f5 / 21:21 / 21,0,0,0 / True

// AXIS unusedBackgroundHeight
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedBackgroundHeight);
// ALTERNATIVE remove (unified delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedBackgroundHeight);

// AXIS unusedBackgroundSide
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedBackgroundSide);
// ALTERNATIVE remove (unified delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedBackgroundSide);
