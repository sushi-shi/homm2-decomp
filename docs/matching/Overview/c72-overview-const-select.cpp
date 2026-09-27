// VC6 SP5 Overview source family c72-overview-const-select; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-const-select/results.json; RVA 0x7c9b3
// Complete 2 arms; elapsed 1.877623s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 1187 / e41fb732eacaba9b / 4a868ea98d5d2bce / 59:55 / 55,0,0,0 / False
// 1 / {'source_family': 'literal_names', 'candidate': 'baseline'} / 1187 / e41fb732eacaba9b / db0d4b6a5319634b / 59:55 / 55,0,0,0 / False

// AXIS source_family
// BEFORE (complete exact span):
// |                     gpResourceManager->GetBackdrop(
// |                         const_cast<char*>("overback.icn"),
// |                         gpWindowManager->m_screen,
// ALTERNATIVE literal_names (unified source delta from BEFORE):
// | --- before
// | +++ literal_names
// | @@ -1,3 +1,3 @@
// |                      gpResourceManager->GetBackdrop(
// | -                        const_cast<char*>("overback.icn"),
// | +                        "overback.icn",
// |                          gpWindowManager->m_screen,
