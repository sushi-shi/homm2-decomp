// VC6 SP3 /Od cross-version source/state matrix.
// Matrix: build/source-variant-batch/get-strength/results.json
// Manifest: build/get-strength-manifest.json
// Complete: 204/204 variants; generator and wall-time truncation false.
//
// Buka candidate:
//   idx/bitMask/totalStrength/currentArmy; (mask & bitMask)
//
// Result:
//   - Both mask orders paired byte-for-byte in every state, including all 18
//     exact PoL-name islands (193 bytes, 10 blocks, 1/1 relocation).
//   - Both Buka-name arms had zero exact states; best fuzzy score 99.857140%.
//
// Disposition: port the byte-invariant Buka mask order; retain the four
// slot-sensitive PoL local names.
