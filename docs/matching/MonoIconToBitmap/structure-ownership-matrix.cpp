/*
MonoIconToBitmap (RVA 0x000cfae0, retail size 614)
Coordinate and clipping ownership matrix, 2026-07-27.

Artifacts:
  build/match-variants/iconm2b-structural-axes-20260727.json
  build/match-variants/iconm2b-structural-clean-islands-20260727/results.json
  build/match-variants/iconm2b-ownership-axes-20260727.json
  build/match-variants/iconm2b-ownership-clean-islands-20260727/results.json

Baseline evidence:
  80.756905%, size 604 versus retail 614, 36 versus 38 relocation
  records, and 31 versus 31 blocks. The successor graph is exact: 23
  blocks are exact and eight are size-only. The two missing relocation
  owners are the second setup reloads of gMonoX0 and gMonoY.

The first eight-arm clean matrix crossed setup coordinate sum order, icon
extent sum order in the clipping predicate, and clip-bound parenthesization.
All eight arms emitted identical text. Post-publication entryX/entryY locals
were also byte-identical to direct field use.

Explicit iconRight/iconBottom lifetimes were then tested as structural
versions because retail reloads those published coordinates:

  direct:
    80.756905%, size 604, 36/38 relocations, 23 exact + 8 size-only
    blocks, exact flow.

  iconRight snapshot:
    78.049720%, size 604, 36/38 relocations, 22 exact + 9 size-only
    blocks, exact flow. This is coherent but strictly weaker evidence.

  iconBottom snapshot:
    75.044200%, retail size 614 and 37/38 relocation records, but only
    one exact block, four size-only blocks, 12 flow-kind mismatches, and
    one missing block. The exact size is accidental, not closure.

  iconRight plus iconBottom:
    71.872925%, size 621 and 38/38 relocation count, with the same
    contradictory block topology. Count parity does not imply ordered
    relocation identity.

Disposition: preserve the direct 31-block source family. The lower exact-size
and relocation-count islands are rejected for contradictory topology. None
exceeds the retained hash-scoped MAX of 81.9503%.
*/

#if 0
// Byte-neutral setup ownership spelling.
gMonoEntry = entry;
gMonoSrc = data + entry->srcOffset;
i32 entryX = entry->x;
gMonoX0 = x + entryX;
gMonoX = gMonoX0;
i32 entryY = entry->y;
gMonoY = y + entryY;

// Coherent but weaker right-bound ownership.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 iconRight = gMonoX0 + entry->w;
    if (clipX > gMonoX0 || iconRight > clipX + clipW || clipY > gMonoY
        || gMonoY + entry->h > clipY + clipH) {
        // existing clipped setup
    }
}

// Exact-size but topology-contradictory bottom ownership.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 iconBottom = gMonoY + entry->h;
    if (clipX > gMonoX0 || gMonoX0 + entry->w > clipX + clipW || clipY > gMonoY
        || iconBottom > clipY + clipH) {
        // existing clipped setup
    }
}
#endif
