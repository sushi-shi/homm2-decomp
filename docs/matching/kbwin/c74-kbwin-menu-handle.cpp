// VC6 SP5 kbwin source family c74-kbwin-menu-handle; parentf0502d36.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c74-kbwin-menu-handle/results.json; RVA 0x70f42
// Complete 2 arms; elapsed 1.466482s; input restored.
// Disposition: retain this cast removal; combined all98 native proof.
// No EH/numerical exceptions in this TU; all sixteen retail checks are complete.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 748 / 84c063eb1a09fb99 / 747f3130b738dcf4 / 12:12 / 12,0,0,0 / True
// 1 / {'source_family': 'same_handle', 'candidate': 'baseline'} / 748 / 84c063eb1a09fb99 / 747f3130b738dcf4 / 12:12 / 12,0,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// | reinterpret_cast<HMENU>(hmnuDflt)
// ALTERNATIVE same_handle (unified source delta from BEFORE):
// | --- before
// | +++ same_handle
// | @@ -1 +1 @@
// | -reinterpret_cast<HMENU>(hmnuDflt)
// | +hmnuDflt
