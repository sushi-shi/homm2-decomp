// VC6 SP5 source family s68-drawing-small-slots; parent aab81af5.
// Source record only: each alternative is relative to its own BEFORE, not cumulative.
// No cross-family or TU-state census is claimed.
// build/source-variant-batch/s68-drawing-small-slots/results.json; RVA 0x3aad6
// Complete 4 arms; elapsed 2.839190s; source restored.
// Disposition: retain original; every tested nonoriginal arm changes function text.
// Raw/normalized relocation hashes can reflect private label counter spelling;
// final proof compares actual ordered targets/sites and the complete claimed byte span.
// UpdateMouseGrid has an unchanged EH/FS site-model gap, not waived by native equality.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'unusedSpell1': 'keep', 'unusedSlot': 'keep', 'candidate': 'baseline'} / 2857 / 588287b772bbe9a4 / 74ff48d9bcdae7f1 / 100:96 / 96,0,0,0 / False
// 1 / {'unusedSpell1': 'keep', 'unusedSlot': 'remove', 'candidate': 'baseline'} / 2854 / e499aad8a0f2c1d8 / 0946480f7bbefb75 / 100:96 / 96,0,0,0 / False
// 2 / {'unusedSpell1': 'remove', 'unusedSlot': 'keep', 'candidate': 'baseline'} / 2854 / d591e7149390a02a / 0946480f7bbefb75 / 100:96 / 96,0,0,0 / False
// 3 / {'unusedSpell1': 'remove', 'unusedSlot': 'remove', 'candidate': 'baseline'} / 2854 / de8f644b23069dab / 0946480f7bbefb75 / 100:96 / 96,0,0,0 / False

// AXIS unusedSpell1
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedSpell1);
// ALTERNATIVE remove (unified delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedSpell1);

// AXIS unusedSlot
// BEFORE (complete exact span):
// |     i32 H2_UNUSED(unusedSlot);
// ALTERNATIVE remove (unified delta from BEFORE):
// | --- before
// | +++ remove
// | @@ -1 +0,0 @@
// | -    i32 H2_UNUSED(unusedSlot);
