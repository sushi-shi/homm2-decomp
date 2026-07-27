/*
 * FlipMonoIconToBitmap common-tail and clip-lifetime campaign, 2026-07-27.
 *
 * Starting evidence:
 *   live flat clip: 26 blocks, 26/27 relocations;
 *   retained MAX 82.580840%, size 518, blocks 22+4.
 *   A nested duplicated-tail experiment had emitted 27/27 relocations but
 *   expanded to 28 blocks. The missing site was retail's second initial-clip
 *   gFMY read.
 *
 * 1. Common-tail and accessor clean screen
 *
 *   build/flipmono-common-tail-clean-axes-20260727.json
 *   build/flipmono-common-tail-clean-manifest-20260727.json
 *   build/match-variants/flipmono-common-tail-clean-20260727/
 *
 * Ten clean shapes tested flat source, nested clip/common-bounds, nested
 * flag/common-tail, inverse containment, an explicit top-Y read, lower
 * reference/pointer parameters, one reference-return accessor, two distinct
 * reference accessors, and a lower value accessor. All common-tail/accessor
 * forms collapsed back to 26 relocations. The nested common-tail forms grew
 * to 537-539 bytes without preserving the second gFMY site.
 *
 * 2. Clip-bound lifetime clean cross
 *
 *   build/flipmono-clip-bound-lifetimes-clean-axes-20260727.json
 *   build/flipmono-clip-bound-lifetimes-clean-manifest-20260727.json
 *   build/match-variants/flipmono-clip-bound-lifetimes-clean-20260727/
 *
 * Explicit clipRight, clipBottom, and entryHeight lifetimes were crossed as
 * eight clean combinations. This found the compiler mechanism:
 *
 *   - any live clipBottom form emitted 27/27 relocations, initially 25 blocks;
 *   - clipRight plus entryHeight emitted 27/27 relocations, initially 27 blocks;
 *   - flat source remained 26 blocks and 26/27 relocations.
 *
 * These are credible original-source boundary and entry-field snapshots, not
 * compiler-state-only expressions.
 *
 * 3. Complete promising-lifetime state census
 *
 *   build/flipmono-clip-bound-promising-axes-20260727.json
 *   build/flipmono-clip-bound-promising-manifest-20260727.json
 *   build/match-variants/flipmono-clip-bound-promising-20260727/
 *
 * Six source families, including flat reference, each received clean plus 50
 * top forest states (306/306 complete). Four clipRight+entryHeight states
 * combined 27/27 relocations with exactly 26 blocks and exact flow. This
 * proved that the legitimate lifetime family can inhabit retail's CFG orbit.
 *
 * 4. Scope/order screen and complete state census
 *
 * Six variants crossed clipRight/entryHeight declaration order, outer versus
 * inner scope, staged clipRight construction, and const qualification:
 *
 *   build/flipmono-right-height-scope-clean-axes-20260727.json
 *   build/flipmono-right-height-scope-clean-manifest-20260727.json
 *   build/flipmono-right-height-scope-states-manifest-20260727.json
 *   build/match-variants/flipmono-right-height-scope-clean-20260727/
 *   build/match-variants/flipmono-right-height-scope-states-20260727/
 *
 * Complete state product: 306/306. The strongest retail-flow/relocation
 * island used function-inner clip scope with clipRight then entryHeight:
 *
 *   trial 30, seed 0x464d6f4e, top forest;
 *   77.323360%, size 514/530, 27/27 relocations;
 *   26/26 blocks, exact flow, 19 exact plus seven size-only.
 *
 * Its ordered relocation identities and addends match retail, including
 * seven ordered gFMY sites. Offsets remain displaced because bytes and size
 * are not yet exact. The paired clue is retained at:
 *
 *   build/tu-state-noise/
 *     iconmf2b-inside-right-height-trial30-structural-clue-20260727/
 *
 * The same effective source's highest fuzzy state is trial 37:
 *
 *   80.646706%, size 507, 26/27 relocations, 26 blocks (22+4).
 *
 * It was replayed with scripts/tu_state_noise.py --record-max at:
 *
 *   build/tu-state-noise/
 *     iconmf2b-inside-right-height-trial37-record-max-20260727/
 *
 * The official generated MAX is 80.6467 for source/dependency hash
 * 9c083285942a.2995e36488e1. The prior 82.5808 observation belongs to a
 * different effective source hash; the real source change correctly starts
 * a new hash-scoped maximum.
 *
 * Disposition: retain the inner-scope clipRight and entryHeight lifetimes.
 * They are the first legitimate source structure shown to emit the complete
 * ordered relocation identity in a retail-flow 26-block island. Retain no
 * generated forest declarations, common-tail duplication, or accessor-only
 * steering.
 */

#if 0
// Retained structure.
if (clip != ICON_DRAW_NO_CLIP) {
    i32 clipRight = clipX + clipW;
    i32 entryHeight = entry->h;
    if (x0 < clipX || clipRight < right || gFMY < clipY
        || entryHeight + gFMY > clipY + clipH) {
        clip = ICON_DRAW_CLIP;
        gFMClipR = clipX + clipW - 1;
        gFMClipB = clipY + clipH - 1;
    } else {
        clip = ICON_DRAW_NO_CLIP;
    }
}

// Representative rejected reference-accessor boundary.
static inline i32& FlipMonoYRef() {
    return gFMY;
}
if (gFMY < clipY || entry->h + FlipMonoYRef() > clipY + clipH) {
}

// Representative rejected common-tail spelling.
i32 outside;
if (x0 < clipX || clipX + clipW < right)
    outside = 1;
else if (gFMY < clipY || entry->h + gFMY > clipY + clipH)
    outside = 1;
else
    outside = 0;
#endif
