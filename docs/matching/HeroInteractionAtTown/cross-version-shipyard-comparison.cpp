/*
philAI::HeroInteractionAtTown cross-version audit, 2026-08-24.

PoL:  RVA 0x4183b, CodeView size 2178.
Buka: RVA 0x86ed0, size 0x7b7, exact.

RETAINED PoL SOURCE

    index7 = abs(townPtr->m_x - heroPtr->m_x)
             + abs(townPtr->m_y - heroPtr->m_y);
    if (index7 < giBestShipyardDist) { ... }

Both shipyard arms use this form.  The comparison and coordinate-term order are
the exact Buka spelling; neither changes gameplay semantics.

MEASURED

- Clean source product: 8/8 complete.  Shipyard comparison, float/double town
  share numerator, and victory-coordinate operand side were byte-identical.
- Source/state product: 408/408 complete (8 sources x clean plus 50 forest
  states).  Only `index7 < giBestShipyardDist` selected the better state-32
  family; the share and victory axes stayed neutral.
- Retained-source replay: forest/top, seed 1213156658, trial 32.  Result
  99.976950%, 2178/2178 bytes, ten raw byte differences, 126/126 blocks, 125
  exact blocks, one B109 target shift, and a complete ordered 71/71 relocation
  stream.  No exact closure.
- Replaying the same state after adopting Buka's x-then-y sum order emitted the
  identical state (`ebc1948245a2d86a`): VC4.2 erases this source distinction in
  the retained orbit.  Eight residual bytes still encode the opposite pair of
  coordinate loads; one relocation-masked raw field and the B109 branch
  displacement account for the other two.

ARTIFACTS

    build/hero-interaction-at-town-cross-version-manifest.json
    build/source-variant-batch/hero-interaction-at-town-cross-version/results.json
    build/hero-interaction-at-town-cross-version-state-manifest.json
    build/source-variant-batch/hero-interaction-at-town-cross-version-state/results.json
    build/hero-interaction-at-town-retained-xy-replay-summary.json
    build/tu-state-noise/hero-interaction-at-town-retained-xy-replay/

DISPOSITION

Retain the two Buka comparison mirrors and the semantically canonical Buka
distance expression.  The town-share and victory-coordinate spellings are not
transferred.  The remaining compiler-state/branch island stays open.
*/
