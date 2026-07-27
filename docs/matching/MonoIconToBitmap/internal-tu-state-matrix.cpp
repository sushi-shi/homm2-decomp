/*
MonoIconToBitmap (RVA 0x000cfae0, retail size 614)
Internal spelling by TU-state matrix, 2026-07-27.

Artifacts:
  build/match-variants/iconm2b-internal-axes-20260727.json
  build/match-variants/iconm2b-internal-state-matrix-20260727.json
  build/match-variants/iconm2b-internal-state-islands-20260727/results.json

After retaining the direct 31-block structure, a complete Cartesian matrix
crossed 16 source spellings with the clean state and 50 declaration-forest
probes. All 816 variants compiled in 651.20 seconds; the run was not
wall-time-truncated and match_variants restored the source.

Source axes:
  - entry-field-first versus formal-first coordinate sums,
  - positive and historical mirrored clipping predicates,
  - coordinate-first and field-first icon extent sums, and
  - left-associative versus decrement-first clip bounds.

All 16 spellings emitted the same object within each important TU state. The
axes therefore do not create an independent useful source orbit.

The best disposable state was forest trial 50:
  81.005520%, size 602, 36/38 relocation records, 31/31 blocks with
  24 exact plus seven size-only blocks and exact flow.

It improves one block over the live object but is two bytes smaller than the
live object, 12 bytes smaller than retail, retains both missing coordinate
relocation owners, and remains below the retained 81.9503% MAX. Trial 6/11
formed the next island at 80.784530%, size 604, 36/38 relocations, and the
live 23-exact/eight-size-only block partition.

Disposition: clue artifacts only. Do not retain a source spelling, generated
probe declaration, generated configuration, or MAX update.
*/

#if 0
// Formal-first setup sums; byte-neutral for the tested clean/state orbits.
gMonoX0 = x + entry->x;
gMonoX = gMonoX0;
gMonoY = y + entry->y;

// Historical mirrored predicate; byte-neutral for the tested clean/state
// orbits and therefore not a reason to replace the clearer retained form.
if (gMonoX0 < clipX || clipX + clipW < entry->w + gMonoX0 || gMonoY < clipY
    || clipY + clipH < entry->h + gMonoY) {
    // existing clipped setup
}

// Alternate bound materialization; byte-neutral in the complete matrix.
gMonoClipR = clipX + (clipW - 1);
gMonoClipB = clipY + (clipH - 1);
#endif
