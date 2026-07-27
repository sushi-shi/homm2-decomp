/*
MonoIconToBitmap (RVA 0x000cfae0, retail size 614)
Scratch-storage scope and inline clipping boundary, 2026-07-27.

Live evidence before this pass was 31/31 flow-exact blocks, 23 exact plus
eight size-only, size 604, and 36/38 relocations.  The first two body-size
differences are the missing second setup reloads of gMonoX0 and gMonoY.

The prior arithmetic and predicate matrices did not test declaration scope or
an actual inline function boundary.  This matrix crossed:

  scratch storage:
    current file-scope statics or function-local statics;

  clipping boundary:
    open expression, inline value-parameter helper, or inline
    const-reference helper;

  compiler state:
    clean plus 50 top-inserted declaration-forest states.

All 2 x 3 x 51 = 306 variants completed in 263.96 seconds.  The run was not
truncated, produced 126 normalized states, and restored source.

Function-local static ownership was coherent but weaker:

  open predicate:     81.646410%, size 601, 36/38 relocations;
  either helper form: 81.453040%, size 601, 36/38 relocations.

File scope plus either inline helper formed the same new state:

  82.861880%, size 602, 36/38 relocations, forest trial 1;
  exact 31-block flow, 24 exact plus seven size-only bodies.

The value and const-reference helper forms were byte-identical in every
important state.  The simpler value form is retained.  It moves one remaining
block body into exact agreement and raises the durable MAX from 81.9503% even
though it does not yet materialize retail's second gMonoX0/gMonoY reads.

After the legitimate source change established hash
5105e389b353.8fbc0130c14c, the status row and MAX were generated entirely by
the official tools:

  homm2 status update --force-refresh
  scripts/tu_state_noise.py --only-trial 1 --record-max --retain-best

The replay recorded 82.8619%.  No probe declaration or generated source is
retained.

Artifacts:
  build/iconm2b-storage-helper-axes-20260727.json
  build/iconm2b-storage-helper-manifest-20260727.json
  build/match-variants/iconm2b-storage-helper-20260727/results.json
  build/tu-state-noise/iconm2b-inline-helper-trial1-record-max-20260727

Exact closure remains open at the two ordered setup reloads; raw relocation
count parity alone will not prove them.
*/

#if 0
static inline i32 MonoNeedsClipping(
    IconEntry* entry,
    i32 x0,
    i32 currentY,
    i32 clipX,
    i32 clipY,
    i32 clipW,
    i32 clipH
) {
    return clipX > x0 || x0 + entry->w > clipX + clipW || clipY > currentY
           || currentY + entry->h > clipY + clipH;
}
#endif
