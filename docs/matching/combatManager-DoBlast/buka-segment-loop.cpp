// combatManager::DoBlast (PoL RVA 0x28d4f; Buka RVA 0xa0178)
//
// Arms: segmentCount_f > segment_h; segment_h < segmentCount_f.
// Artifact: build/source-variant-batch/do-blast-segment-loop/results.json
// Coverage: complete 2/2 clean-source product; source restored.
//
// Both arms emit the same 99.938324% probe object: 826 bytes, exact 23/23
// CFG, and 50/50 relocation count. The shared compiler-state residual is
// unrelated. PoL adopts exact-Buka's counter-first loop condition.
// Final normal build: 99.98%, exact 23/23 CFG, branch sequences agree, and
// focused relocation review reports 50/50 with no missing base owners.
// No gameplay or emitted-byte change within the measured state.
