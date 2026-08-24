// VC6 SP3 /Od cross-version source/state matrix.
// Matrix: build/source-variant-batch/check-screen-scroll/results.json
// Manifest: build/check-screen-scroll-manifest.json
// Complete: 408/408 variants; generator and wall-time truncation false.
//
// Buka candidate:
//   i32 mouseX, mouseY, oldMapX, oldMapY;
//   oldMapX == m_mapOriginX && oldMapY == m_mapOriginY
//
// Result:
//   - Both comparison directions were byte-identical in all 51 paired states.
//   - Every PoL-name arm was exact in all 51 states (443 bytes, 30 blocks,
//     18/18 relocations).
//   - Every Buka-name arm had zero exact states; best fuzzy score 99.963970%.
//
// Disposition: port both Buka comparison spellings; retain the slot-sensitive
// PoL local names mouseX6/mouseY1/oldOriginX9/oldOriginY3.
