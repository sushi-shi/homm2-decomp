/*
DimIconToBitmap, retail RVA 0x004cfd50, size 622.
Clipping-containment control-structure matrix, 2026-07-27.

The existing attempt set covered relational spellings and inline predicate
ownership, but not the high-level positive-containment control structure.
Three semantically equivalent four-clause short-circuit forms were crossed
with clean plus 50 declaration-forest states: 3 x 51 = 153/153.

Attempted alternate forms:

    if (!(gDimX0 >= clipX && gDimX0 + entry->w <= clipX + clipW
          && clipY <= gDimY && gDimY + entry->h <= clipY + clipH)) {
        // clipped setup
    }

    if (gDimX0 >= clipX && gDimX0 + entry->w <= clipX + clipW
        && clipY <= gDimY && gDimY + entry->h <= clipY + clipH) {
        clip = ICON_DRAW_NO_CLIP;
    } else {
        // clipped setup
    }

Result:
  all three families peak at 85.913795%, 608 bytes, 35/37 relocations
  all three converge on text SHA 31a0711950a2b0c5
  blocks remain 36/36, 34 exact plus two size-only

VC4.2 canonicalizes the outside disjunction, negated inside conjunction, and
inside-first branch to the same useful objects within the relevant states.
None restores the second gDimX0 or gDimY relocation.

Artifacts:
  build/icond2b-containment-control-axes.json
  build/icond2b-containment-control-manifest.json
  build/match-variants/icond2b-containment-control

Disposition: rejected as byte-neutral control syntax below the 86.229880% MAX.
No experimental source or generated state is retained.
*/

void DimIconToBitmap_containment_control_structure_record() {
}
