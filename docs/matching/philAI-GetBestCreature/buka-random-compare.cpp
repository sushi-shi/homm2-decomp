// philAI::GetBestCreature (PoL RVA 0x3db58)
//
// Reviewed matrix arms:
//   weakestArmyValue < gMonsterDatabase[IDX(candidateMonster)].randomValue
//   gMonsterDatabase[IDX(candidateMonster)].randomValue > weakestArmyValue
//
// Artifact:
//   build/source-variant-batch/get-best-creature-random-compare/results.json
// Coverage: complete 2/2 clean-source product; source restored.
//
// Both arms emit the same text/relocation hashes in the matrix probe state:
// 99.977480%, 859 bytes, exact 46/46 CFG, and 23/23 relocation count. The
// shared probe-state ordered-relocation residual is unrelated to the axis.
// The retained Buka arm's normal build is exact at 859 bytes, 46/46 CFG, and
// complete ordered 23/23 relocations. No gameplay or shipped-byte change.
