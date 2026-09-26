// VC6 SP5 Overview source family c72-overview-faction; parent ee5ee1cf.
// Source record only. All alternatives below are relative to the stated BEFORE,
// not cumulative diffs. No cross-family/TU-state census is claimed.
// build/source-variant-batch/c72-overview-faction/results.json; RVA 0x79ba0
// Complete 2 arms; elapsed 1.954647s; input restored.
// Disposition: retain this family's cast removals; combined all98 native proof.
// EH private symbol hashes may differ; native comparison canonicalizes actual section owners.
// trial / choices / bytes / text hash / normalized reloc hash / blocks / exact,size,shift,flow / graph exact
// 0 / {'source_family': 'keep', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 961908a478c1be46 / 234:234 / 233,1,0,0 / True
// 1 / {'source_family': 'faction', 'candidate': 'baseline'} / 7750 / 628178f0823444cb / 3504f5a403b415b3 / 234:234 / 233,1,0,0 / True

// AXIS source_family
// BEFORE (complete exact span):
// |                     const_cast<char*>(capt ? "minicapt.icn" : "miniport.icn"),
// |                     capt ? static_cast<u8>(IDX(record->m_type)) : IDX(heroData->m_portrait),
// |                     ICON_DRAW_NORMAL,
// ALTERNATIVE faction (unified source delta from BEFORE):
// | --- before
// | +++ faction
// | @@ -1,3 +1,3 @@
// |                      const_cast<char*>(capt ? "minicapt.icn" : "miniport.icn"),
// | -                    capt ? static_cast<u8>(IDX(record->m_type)) : IDX(heroData->m_portrait),
// | +                    capt ? IDX(record->m_type) : IDX(heroData->m_portrait),
// |                      ICON_DRAW_NORMAL,
