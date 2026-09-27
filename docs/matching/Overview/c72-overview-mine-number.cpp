// VC6 SP5 Overview source family c72-overview-mine-number; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-mine-number/results.json; RVA 0x7bda1
// Complete 2 arms; elapsed 1.744998s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 9cebe67a573ac9ef / 42:42 / 42,0,0,0 / True
// 1 / {'source_family': 'number', 'candidate': 'baseline'} / 1202 / 688689bd5599e522 / 62049d293fd8bef0 / 42:42 / 42,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |         message.payload.widget.data.text = gText;
// |         sprintf(gText, "%d", static_cast<i32>(mineCounts[mine]));
// |         message.payload.widget.id = mine + MINE_FIRST_WIDGET;
// ALTERNATIVE number (unified source delta from BEFORE):
// | --- before
// | +++ number
// | @@ -1,3 +1,3 @@
// |          message.payload.widget.data.text = gText;
// | -        sprintf(gText, "%d", static_cast<i32>(mineCounts[mine]));
// | +        sprintf(gText, "%d", mineCounts[mine]);
// |          message.payload.widget.id = mine + MINE_FIRST_WIDGET;
