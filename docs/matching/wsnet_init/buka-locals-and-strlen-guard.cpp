// VC6 SP3 cross-version source/state matrix.
// Matrix: build/source-variant-batch/wsnet-init/results.json
// Manifest: build/wsnet-init-manifest.json
// Complete: 408/408 variants; generator and wall-time truncation false.
//
// Buka candidate: blockMode/plr; strlen(gcTCPAddress) > 0.
// Result:
//   - != 0 and > 0 had identical bytes, raw and normalized relocation hashes,
//     scores, and sizes in all 51 paired states.
//   - blockMode reached at most 99.989100%; plr reached at most 99.975480%.
//   - The batch raw exact gate rejects anonymous relocation identities; the
//     canonical clean object is exact (1461 bytes, 38 blocks, 164/164 relocs).
//
// Disposition: port the byte/relocation-invariant Buka strlen guard; retain
// the compiler-sensitive PoL names socketMode and player.
