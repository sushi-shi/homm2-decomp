// army::FlyTo(i32) (SOURCE/FLY, RVA 0xa5fbf) — 2026-07-29 frame dossier, OPEN
//
// Retail frame 0xc0 vs ours 0xb4: three extra dead dword slots in retail plus
// a near-total slot permutation (73-count this-slot pair T-0x74 <- B-0x68).
// Named locals = 25 declared up top; the remaining ~0x50 bytes are compiler
// FP/int spill temps that follow the named layout.
//
// Verified bindings (ours): this=-0x68, sourceColumn=-0x5c,
// destinationColumn=-0x54, columnDifference=-0x20.
// Bijection extract (retail <- ours, count-weighted, from objdiff JSON):
//   -0x74<--0x68(this) -0x4c<--0x28 -0x54<--0x4 -0x48<--0x14 -0x70<--0x34
//   -0x60<--0x50 -0x6c<--0x40 -0x64<--0x44 -0x5c<--0x60 -0x58<--0x4c
//   -0x4<--0x38 -0x14<--0x3c -0x50<--0x58 -0x34<--0x1c -0x2c<--0x18
//   -0x24<--0x2c -0x20<--0x48 -0x1c<--0x64 -0x10<--0xc -0x8<--0x20
//   -0x68<--0x10 -0xc<--0x5c  (deep temps: -0xc0<--0xb4 -0xbc<--0xb0)
//
// Recipe (worked for DoVictory): bind every ours-slot to its variable via
// init statements, invert the bijection into the retail walk order, add the
// three phantom locals at their walk positions, then solve names with
// od_slots buckets and declare in reverse walk order. Beware 1x-count pairs
// (cross-alignment artifacts) — re-derive from unambiguous rows.
