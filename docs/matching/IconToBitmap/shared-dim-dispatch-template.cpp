/*
 * IconToBitmap, RVA 0xd0570: sibling dispatcher and shared dim-template
 * structures, 2026-07-27.
 *
 * Retail first:
 *
 *   homm2 sema disasm 0xd0570 --blocks --target --lite
 *   llvm-objdump -dr --no-show-raw-insn build/delink/BASE/Icon2b.c.obj
 *
 * Retail and the retained source both emit fill in B22-B38, dim in B39-B60,
 * and an exact 80-block flow graph.  Retail also keeps command, count, and
 * flags in local values and publishes gIcRun at the same three joins as the
 * retained source.  Therefore IconToBitmapYModify's global-run owner and
 * dim-before-fill fallthrough are not transferable to this target: they
 * contradict ordered relocation ownership and emitted body order.  The
 * semantically applicable sibling roles are already present.
 *
 * One ColorTable-family detail was missing from the effective source.  In
 * retail's left-clipped dim arm, gIcClipR is loaded, the original count is
 * published to gIcCnt2, and only then is the right edge compared.  icon2bc
 * has the same source structure.  The direct spelling retained in Icon2b is:
 */
#if 0
{
    i32 clipRight = gIcClipR;
    gIcCnt2 = count;
    if (right <= clipRight)
        count = (count - clipX) + X;
    else
        count = clipW;
}
#endif

/*
 * The publication was crossed with the previous spelling, an inline value
 * owner, and a parameterized macro boundary:
 *
 *   build/icon2b-sibling-dim-publication-axes-20260727.json
 *   build/icon2b-sibling-dim-publication-manifest-20260727.json
 *   build/match-variants/icon2b-sibling-dim-publication-20260727/
 *
 * Complete product: four structures * (clean + 50 forest states) = 204/204
 * compiled.  All four reached the existing 81.673080% MAX, size 1220,
 * 79/83 relocations, and exact 80-block flow (63 exact plus 17 size-only
 * blocks).  The direct source reached it at trial 38; the inline owner reached
 * it at trial 27.  No tested state preserved the otherwise evidenced extra
 * store: MSVC eliminated it before the later gIcCnt2 = cn publication.
 *
 * That does not invalidate the source evidence.  The direct publication is
 * retained because both retail relocation order and the forward ColorTable
 * sibling support it, while the assignment is semantically superseded before
 * the value can escape the arm.  Generated forest declarations are not
 * retained.  A standalone unchanged-source replay reproduced trial 38 and
 * recorded MAX 81.6731 for hash caa2c336fa71.02e5f0cc8453 through the
 * canonical script:
 *
 *   build/tu-state-noise/icon2b-shared-dim-publication-retained-20260727/
 *   build/icon2b-shared-dim-publication-retained-summary-20260727.json
 *
 * A complete reusable dim body was also represented as a plausible
 * include-template: per-TU aliases name the gIc/gCT publications, while the
 * pixel operation is the only behavioral hook (direct palette lookup here,
 * ColorTableDimPixel in icon2bc).  The representative invocation was:
 */
#if 0
#define ICON_DIM_CNT2 gIcCnt2
#define ICON_DIM_RUN gIcRun
#define ICON_DIM_PAL gIcDimPal
#define ICON_DIM_DST gIcDimDst
#define ICON_DIM_CNT gIcCnt
#define ICON_DIM_LEN gIcDimLen
#define ICON_DIM_PIXEL(dst, palette, counter) \
    i32 b = *dst;                           \
    dst = dst + 1;                         \
    ICON_DIM_DST = dst;                    \
    counter--;                             \
    ICON_DIM_PAL = palette;                \
    dst[-1] = palette[b]

dim_run:
    ICON_DIM_RUN_BODY();
#endif

/*
 * Artifacts:
 *
 *   build/icon2b-shared-dim-body-axes-20260727.json
 *   build/icon2b-shared-dim-body-manifest-20260727.json
 *   build/match-variants/icon2b-shared-dim-body-20260727/
 *
 * Expanded source and the alias-template invocation were crossed independently
 * with clean plus all 50 forest states: 102/102 compiled.  Every one of the
 * 51 state pairs had identical text hash, normalized relocation hash, score,
 * and CFG metrics.  Both reached 81.673080%, size 1220, 79/83 relocations,
 * and 63 exact plus 17 size-only blocks.
 *
 * This proves macro/include-template authorship is compatible with the object,
 * but also proves that token-equivalent factorization cannot expose another
 * compiler island.  Keep the readable expanded body; future progress needs a
 * real expanded-C++ boundary such as a justified inline owner.
 *
 * Remaining untested analogous goto surface:
 *
 *   - Iconf2b's set_skip/skip_set pair in its mirrored literal clipping arm
 *     has not received a direct shared-macro/include-boundary census.
 *
 * The fill/dim joins in Icon2b, icon2bc, Iconf2b, and iconf2bc and the
 * copy_literal/publish_literal_skip/literal_advance_done joins in iconf2by
 * already have structural matrices.  The closed Icon2b literal macro matrix
 * was not replayed.
 */

void IconToBitmap_shared_dim_dispatch_template_attempt() {
}
